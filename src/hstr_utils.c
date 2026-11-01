/*
 hstr_utils.c       utilities

 Copyright (C) 2014-2026 Martin Dvorak <martin.dvorak@mindforger.com>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#define _GNU_SOURCE

#include "include/hstr_utils.h"

#define DEFAULT_COMMAND "pwd"
#define PROC_HOSTNAME "/proc/sys/kernel/hostname"
// TODO PID_BUFFER 20+ characters might be enough
#define PID_BUFFER_SIZE 128

/*
 * global variables
 */

bool is_tiocsti = false;

/*
 * functions
 */

// strdup() not in ISO C
char* hstr_strdup(const char* s)
{
  size_t len = 1+strlen(s);
  char *p = malloc(len);
  return p ? memcpy(p, s, len) : NULL;
}

// allocate copy of the environment variable value or NULL if it is not set or empty
char* hstr_getenv_strdup(const char* name)
{
    char* value = name ? getenv(name) : NULL;
    return (value && strlen(value)) ? hstr_strdup(value) : NULL;
}

// wide char aware strlen()
// UTF-8 continuation byte (10xxxxxx) and the first byte of a multibyte sequence (11xxxxxx)
#define IS_UTF8_CONTINUATION(C) ((((unsigned char)(C))&0xC0)==0x80)
#define IS_UTF8_LEAD(C) ((((unsigned char)(C))&0xC0)==0xC0)
#define UTF8_MAX_BYTES 4

// returns display width (terminal columns) of the first n bytes of s in the current locale:
// incomplete trailing sequence is not counted (it is not displayed yet), invalid bytes
// and non-printable characters count as 1 column
int hstr_strnwidth(const char* s, size_t n)
{
    int width=0;
    if(s) {
        mbstate_t state;
        memset(&state, 0, sizeof(state));
        size_t i=0;
        while(i<n && s[i]) {
            wchar_t wc;
            size_t length=mbrtowc(&wc, s+i, n-i, &state);
            if(length==(size_t)-2 || length==0) {
                break;
            }
            if(length==(size_t)-1) {
                memset(&state, 0, sizeof(state));
                width++;
                i++;
                continue;
            }
            int charWidth=wcwidth(wc);
            width+=charWidth<0?1:charWidth;
            i+=length;
        }
    }
    return width;
}

// returns display width (terminal columns) of s
int hstr_strwidth(const char* s)
{
    return s?hstr_strnwidth(s, strlen(s)):0;
}

// returns length in bytes of the longest prefix of s which fits maxWidth columns (characters are not split)
static size_t prefix_fitting_width(const char* s, int maxWidth)
{
    size_t length=strlen(s), end=0;
    int width=0;
    while(end<length) {
        size_t next=end+1;
        while(next<length && next-end<UTF8_MAX_BYTES && IS_UTF8_CONTINUATION(s[next])) {
            next++;
        }
        int charWidth=hstr_strnwidth(s+end, next-end);
        if(width+charWidth>maxWidth) {
            break;
        }
        width+=charWidth;
        end=next;
    }
    return end;
}

// returns length in bytes of the longest suffix of s which fits maxWidth columns (characters are not split)
static size_t suffix_fitting_width(const char* s, int maxWidth)
{
    size_t length=strlen(s), start=length;
    int width=0;
    while(start>0) {
        size_t charStart=start-1;
        while(charStart>0 && start-charStart<UTF8_MAX_BYTES && IS_UTF8_CONTINUATION(s[charStart])) {
            charStart--;
        }
        int charWidth=hstr_strnwidth(s+charStart, start-charStart);
        if(width+charWidth>maxWidth) {
            break;
        }
        width+=charWidth;
        start=charStart;
    }
    return length-start;
}

// IMPROVE make this work w/ wide strings
// writes s elided to maxWidth columns to buffer - middle of s is replaced by "...", strings
// of up to 3 columns are never elided, characters are not split
char* hstr_strelide(char* buffer, size_t bufferSize, const char* s, unsigned maxWidth)
{
    if(!buffer || !bufferSize) {
        return buffer;
    }
    buffer[0]=0;
    if(!s) {
        return buffer;
    }

    int width=hstr_strwidth(s);
    int maxColumns=maxWidth>INT_MAX?INT_MAX:(int)maxWidth;
    if(width<=maxColumns || width<=3) {
        snprintf(buffer, bufferSize, "%s", s);
    } else if(maxColumns<=3) {
        // no space for "..."
        snprintf(buffer, bufferSize, "%.*s", (int)prefix_fitting_width(s, maxColumns), s);
    } else {
        size_t prefixLength=prefix_fitting_width(s, maxColumns/2-1);
        int prefixWidth=hstr_strnwidth(s, prefixLength);
        size_t suffixLength=suffix_fitting_width(s, maxColumns-prefixWidth-3);
        snprintf(buffer, bufferSize, "%.*s...%s", (int)prefixLength, s, s+strlen(s)-suffixLength);
    }
    return buffer;
}

// removes the last (UTF-8 multibyte) character of s, invalid sequence is removed byte by byte
void hstr_chop(char *s)
{
    if(s) {
        size_t length=strlen(s);
        if(length) {
            size_t last=length-1;
            while(last>0 && length-last<UTF8_MAX_BYTES && IS_UTF8_CONTINUATION(s[last])) {
                last--;
            }
            if(last!=length-1 && !IS_UTF8_LEAD(s[last])) {
                last=length-1;
            }
            s[last]=0;
        }
    }
}

bool hstr_strcat_char(char* s, size_t size, int c)
{
    // append byte c to string s stored in buffer of the given size:
    // - c must be a byte (UTF-8 characters are appended byte by byte),
    //     other values (e.g. curses KEY_* codes) are rejected
    // - returns false if c was not appended (invalid or full buffer)

    if(!s || c<=0 || c>UCHAR_MAX) {
        return false;
    }

    // bounded search - s may be unterminated within size
    const char* end=memchr(s, 0, size);
    if(!end) {
        return false;
    }
    size_t length=(size_t)(end-s);
    if(length+1>=size) {
        return false;
    }
    s[length]=(char)c;
    s[length+1]=0;
    return true;
}

bool is_tiocsti_supported(void)
{
#if defined(__MS_WSL__) || defined(__CYGWIN__)
    return false;
#else
   int fd;
   struct termios t;

   fd = open("/dev/tty", O_RDWR);
   if (fd < 0) {
      printf("Error: unable to detect whether TIOCSTI is supported by the kernel");
      perror("open /dev/tty");
      return false;
   }
   if (tcgetattr(fd, &t) < 0) {
      printf("Error: unable to detect whether TIOCSTI is supported by the kernel");
      perror("tcgetattr");
      return false;
   }

   bool is_supported = false;
   // probe TIOCSTI by sending (and clearing) the character
   if (!ioctl(fd, TIOCSTI, " ")) { // send probe character
      tcflush(fd, TCIOFLUSH); // flush probe character (avoid insert to prompt)
      return true;
   }
   close(fd);

   return is_supported;
#endif
}

#if !defined(__MS_WSL__) && !defined(__CYGWIN__)
void tiocsti(void)
{
    char buf[] = DEFAULT_COMMAND;
    unsigned i;
    for (i=0; i<sizeof buf-1; i++) {
        if (ioctl(0, TIOCSTI, &buf[i]) < 0) {
            perror("Unable to execute TIOCSTI using ioctl()");
            exit(1);
        }
    }
}
#endif

void fill_terminal_input(char* cmd, bool padding)
{
    if(cmd && strlen(cmd)>0) {
            if(is_tiocsti) {
            size_t size = strlen(cmd);
            unsigned i;
            char *c;
            for (i = 0; i < size; i++) {
                // terminal I/O control, simulate terminal input
                c=(cmd+i);
                ioctl(0, TIOCSTI, c);
            }
            // echo, but don't flush to terminal
            if(padding) printf("\n");
        } else {
            fprintf(stderr, "%s", cmd);
            if(padding) fprintf(stderr, "%s", "\n");
        }
    }
}

void reverse_char_pointer_array(char **array, unsigned length)
{
    char *temp;
    unsigned i;
    for (i=0; i<length/2; i++) {
        temp = array[i];
        array[i] = array[length-i-1];
        array[length-i-1] = temp;
    }
}

void get_hostname(int bufferSize, char *buffer)
{
    char *b=buffer;
    if(access(PROC_HOSTNAME, F_OK) != -1) {
        FILE *file = fopen(PROC_HOSTNAME, "r");
        b=fgets(buffer, bufferSize, file);
        fclose(file);
        if(b) {
            b[strlen(b)-1]=0;
            return;
        }
    }
    strcpy(buffer, "localhost");
}

char* get_home_file_path(char* filename)
{
    char* home = getenv(ENV_VAR_HOME);
    char* path = malloc(strlen(home) + 1 + strlen(filename) + 1);
    strcat(strcat(strcpy(path, home), "/"), filename);
    return path;
}

void toggle_case(char *str, bool lowercase) {
    if(str && strlen(str)>0) {
        int i;
        for(i = 0; str[i]; i++){
            if(lowercase) {
                str[i] = tolower((unsigned char)str[i]);
            } else {
                str[i] = toupper((unsigned char)str[i]);
            }
        }
    }
}

char *get_shell_name_by_ppid(const int pid)
{
    char* name = (char*)calloc(PID_BUFFER_SIZE,sizeof(char));
    if(name){
        // First we should try to open /proc/[pid]/comm (since Linux 2.6.33) and only if it missing
        // then use /proc/[pid]/cmdline. Second way have problems in some cases - when for example
        // we open session with using "su - [username]". Then /proc/[pid]/cmdline will contains
        // "-su" instead real shell name, which is present in the /proc/[pid]/comm file.
        sprintf(name, "/proc/%d/comm",pid);
        FILE* f = fopen(name,"r");
        if (!f){
            sprintf(name, "/proc/%d/cmdline",pid);
            f = fopen(name,"r");
        }
        if(f){
            size_t size = fread(name, sizeof(char), PID_BUFFER_SIZE-1, f);
            if(size>0){
                if('\n'==name[size-1]) {
                    name[size-1]='\0';
                }
            }
            fclose(f);
        }
    }

    // if name isn't e.g. bash/zsh at this point, fall back to $SHELL
    if(strlen(name) > 4){
      char* shell = getenv("SHELL");
      if(shell != NULL) {
          shell=strrchr(shell,'/');
          if(shell != NULL) {
              shell++;
              unsigned len=(strlen(shell)+1)*sizeof(char);
              name = realloc(name, len);
              strncpy(name, shell, len);
          }
      }
    }

    return name;
}

bool is_zsh_parent_shell(void) {
    pid_t parentPid=getppid();
    char* cmdline=get_shell_name_by_ppid(parentPid);
    bool result=cmdline && strstr(cmdline, "zsh");
    free(cmdline);
    return result;
}

// see zsh utils.c https://github.com/zsh-users/zsh/blob/master/Src/utils.c#L4922
char* zsh_unmetafy(char* s, int* len)
{
  char *p, *t;

  for (p = s; *p && *p != ZSH_META; p++);
  for (t = p; (*t = *p++);)
    if (*t++ == ZSH_META)
      t[-1] = *p++ ^ 32;
  if (len)
    *len = t - s;

  return s;
}
