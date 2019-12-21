/*
 hstr_utils.h       header file for HSTR utilities

 Copyright (C) 2014-2020  Martin Dvorak <martin.dvorak@mindforger.com>

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

#ifndef HSTR_UTILS_H
#define HSTR_UTILS_H

#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define ENV_VAR_HOME "HOME"

#define UNUSED_ARG(expr) do { (void)(expr); } while (0)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

char* hstr_strdup(const char* s);
int hstr_strlen(const char* s);
char* hstr_strelide(char* buffer, const char* s, unsigned maxlength);
void hstr_chop(char* s);
#ifndef __CYGWIN__
void tiocsti();
#endif
void fill_terminal_input(char* cmd, bool padding);
void reverse_char_pointer_array(char** array, unsigned length);
void get_hostname(int bufferSize, char* buffer);
char* get_home_file_path(char* filename);
void toggle_case(char* str, bool lowercase);
bool isZshParentShell(void);

#endif
