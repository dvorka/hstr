/*
 hstr_utils.c       utilities

 Copyright (C) 2014-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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

// wide char aware strlen()
int hstr_strlen(const char *s)
{
    if(s) {
        int result=0;
        bool isHighBitSet=false;
        int i=0;
        while(s[i]) {
            if(1<<7 & s[i]) {
                if(isHighBitSet) {
                    isHighBitSet=false;
                    result++;
                } else {
                    isHighBitSet=true;
                }
            } else {
                result++;
            }
            i++;
        }
        return result;
    } else {
        return 0;
    }
}

// IMPROVE make this work w/ wide strings
char* hstr_strelide(char* buffer, const char* s, unsigned maxlength)
{
    if(s) {
        if(strlen(s)>maxlength && strlen(s)>3) {
            unsigned dotOffset = maxlength/2-1; // 4/2-1=1 ~ "a..."
            strncpy(buffer, s, dotOffset);
            unsigned i, offset = dotOffset;
            for(i=0; i<3; i++) {
                buffer[offset] = '.';
                offset++;
            }
            // fill from the end
            dotOffset=offset;
            offset=maxlength-1;
            i=strlen(s)-1;
            while(i && offset>=dotOffset) {
                buffer[offset--] = s[i--];
            }
            buffer[maxlength]=0;
        } else {
            strcpy(buffer, s);
        }
    } else {
        buffer[0] = 0;
    }

    return buffer;
}

void hstr_chop(char *s)
{
    if(s) {
        int i=strlen(s);
        if(i) {
            if(1<<7 & s[i-1]) {
                s[i-2]=0;
            } else {
                s[i-1]=0;
            }
        }
    }
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

/* See zsh utils.c https://github.com/zsh-users/zsh/blob/master/Src/utils.c#L4922 */
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
