/*
 hstr_utils.c       utilities

 Copyright (C) 2014  Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <ctype.h>

#define DEFAULT_COMMAND "pwd"
#define PROC_HOSTNAME "/proc/sys/kernel/hostname"
// TODO PID_BUFFER 20+ characters might be enough
#define PID_BUFFER_SIZE 128

// strdup() not in ISO C
char *hstr_strdup(const char * s)
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

#if !defined(__MS_WSL__) && !defined(__CYGWIN__)
void tiocsti()
{
    char buf[] = DEFAULT_COMMAND;
    int i;
    for (i = 0; i < sizeof buf - 1; i++) {
        ioctl(0, TIOCSTI, &buf[i]);
    }
}
#endif

void fill_terminal_input(char *cmd, bool padding)
{
    if(cmd && strlen(cmd)>0) {
#if defined(__MS_WSL__) || defined(__CYGWIN__)
        fprintf(stderr,cmd);
        if(padding) fprintf(stderr,"\n");
#else
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
#endif
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
    strcpy(buffer,"localhost");
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
        sprintf(name, "/proc/%d/cmdline",pid);
        FILE* f = fopen(name,"r");
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
              strncpy(name,shell,sizeof(char)*strlen(shell));
          }
      }
    }
    return name;
}

bool isZshParentShell() {
    pid_t parentPid=getppid();
    char* cmdline=get_shell_name_by_ppid(parentPid);
    bool result=cmdline && strstr(cmdline, "zsh");
    free(cmdline);
    return result;
}
