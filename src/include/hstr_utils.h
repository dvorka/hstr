/*
 hstr_utils.h       header file for HSTR utilities

 Copyright (C) 2014-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>

#define ENV_VAR_HOME "HOME"

#define UNUSED_ARG(expr) do { (void)(expr); } while (0)
#define ZSH_META ((char) 0x83)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// TIOCSTI is an acronym for "Terminal Input Output Control STack Input",
// and it is a system call used in Unix-like operating systems. The TIOCSTI
// system call allows to insert data into the input buffer of a terminal
// as if it had been typed by the user.
//
// TIOCSTI is NOT available on:
//  - Linux kernel >=6.2.0
//  - Cygwin
//  - WSL
//
// HSTR uses TIOCSTI to insert a command chosen by the user in HSTR
// to shell prompt - if TIOCSTI is available. Otherwise shell features
// must be used.
//
// HSTR modus operandi:
//
// - user runs `hstr` command
//   - `hstr` checks whether TIOCSTI is supported by the kernel or not
//      - if TIOCSTI SUPPORTED, then HSTR continues as it is able to insert
//        any chosen command into Bash/Zsh prompt
//      - if TIOCSTI NOT supported, then:
//        - HSTR checks for `HSTR_TIOCSTI` environment variable:
//          - if `HSTR_TIOCSTI` is NOT defined, then it prints an error and asks
//            user to configure HSTR (--show-configuration >> .*rc)
//          - if `HSTR_TIOCSTI` is DEFINED, then:
//            - if it is SET to `n`, then HSTR presumes that it has
//              been configured and that the shell function (which replaces TIOCSTI)
//              is defined > HSTR uses the function to insert command > DONE
//            - OTHERWISE HSTR prints and error and ask user to configure HSTR
//              with --show-configuration >> .*rc
//
// HSTR features related to TIOCSTI
//
// - HSTR can detect whether TIOCSTI is supported by the kernel:
//     hstr --is-tiocsti
//   the command returns "y" or "n" (and exit code: 0 or 1)
// - HSTR checks for the following env variable if TIOCSTI is not detected
//     HSTR_TIOCSTI=y or HSTR_TIOCSTI=n
//

// global TIOCSTI indicator declaration
extern bool is_tiocsti;

char* hstr_strdup(const char* s);
int hstr_strlen(const char* s);
char* hstr_strelide(char* buffer, const char* s, unsigned maxlength);
void hstr_chop(char* s);
bool is_tiocsti_supported(void);
#if !defined(__MS_WSL__) && !defined(__CYGWIN__)
void tiocsti(void);
#endif
void fill_terminal_input(char* cmd, bool padding);
void reverse_char_pointer_array(char** array, unsigned length);
void get_hostname(int bufferSize, char* buffer);
char* get_home_file_path(char* filename);
void toggle_case(char* str, bool lowercase);
bool is_zsh_parent_shell(void);
char* zsh_unmetafy(char* s, int* len);

#endif
