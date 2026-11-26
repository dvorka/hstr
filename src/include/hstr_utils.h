/*
 hstr_utils.h       header file for HSTR utilities

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

#ifndef HSTR_UTILS_H
#define HSTR_UTILS_H

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>

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
//  - Linux kernel >=6.2.0 (unless enabled by dev.tty.legacy_tiocsti sysctl)
//  - Cygwin
//  - WSL
//
// HSTR uses TIOCSTI to insert a command chosen by the user in HSTR
// to shell prompt - if TIOCSTI is available. Otherwise shell features
// must be used: shell function, which reads the command from HSTR's stderr.
//
// HSTR modus operandi:
//
// - `HSTR_TIOCSTI` environment variable tells HSTR which shell configuration
//   is installed (`--show-configuration` emits `y` or `n` based on detection):
//     - `y` ... key binding which relies on TIOCSTI is configured
//     - `n` ... shell function which replaces TIOCSTI is configured
//               (reads the command from HSTR's stderr, works on any kernel)
//     - `force` ... never emitted, set manually by the user: use TIOCSTI
//                   even if it is NOT detected (detection does not work
//                   on the platform e.g. some Android/Termux setups)
//
// - user runs `hstr` command
//   - `hstr` checks whether TIOCSTI is supported by the kernel or not:
//     it probes TIOCSTI on stdin (on /dev/tty if stdin is not a TTY) i.e. on
//     the same terminal where the command is to be inserted
//      - if TIOCSTI SUPPORTED, then HSTR continues as it is able to insert
//        any chosen command into Bash/Zsh prompt using TIOCSTI - regardless
//        of `HSTR_TIOCSTI` value:
//          - if `HSTR_TIOCSTI` is NOT defined, then key binding is presumed
//            (or HSTR is run from the command line) > TIOCSTI > DONE
//          - if `HSTR_TIOCSTI` is `y`, then key binding is configured
//            > TIOCSTI > DONE
//          - if `HSTR_TIOCSTI` is `n`, then shell function is configured,
//            TIOCSTI inserts the command into the terminal input (stderr
//            is empty) and the shell reads it once the function returns
//            > TIOCSTI > DONE (ENTER executes the command)
//          - if `HSTR_TIOCSTI` is `force`, then it is no-op > TIOCSTI > DONE
//      - if TIOCSTI NOT supported, then:
//        - HSTR checks for `HSTR_TIOCSTI` environment variable:
//          - if `HSTR_TIOCSTI` is NOT defined, then it prints a warning and asks
//            user to configure HSTR (--show-configuration >> .*rc)
//          - if `HSTR_TIOCSTI` is DEFINED, then:
//            - if it is SET to `n`, then HSTR presumes that it has
//              been configured and that the shell function (which replaces TIOCSTI)
//              is defined > HSTR prints the command to stderr and the function
//              inserts it into the prompt > DONE
//            - if it is SET to `force`, then HSTR uses TIOCSTI anyway (no warning):
//              - if TIOCSTI works (detection was wrong), then > DONE
//              - if TIOCSTI fails, then HSTR falls back to stderr (for the
//                rest of the command) and prints a hint to configure HSTR
//                with --show-configuration >> .*rc (hint only if stderr is a
//                terminal, so that it is not captured by the shell function)
//            - OTHERWISE (`y` e.g. config from an older kernel/other machine,
//              or any other value) HSTR prints a warning and asks user to
//              configure HSTR with --show-configuration >> .*rc
//          - the warning is NOT shown if HSTR_SUPPRESS_TIOCSTI_WARNING is set,
//            for non-interactive options (--show-configuration, --is-tiocsti,
//            --non-interactive, ...) and if stdin is not a terminal
//            > HSTR continues and prints the command to stderr
//          - the user may continue anyway (command printed to stderr) or exit
//            with HSTR_EXIT_CONFIG_REQUIRED exit code
//
// HSTR features related to TIOCSTI
//
// - HSTR can detect whether TIOCSTI is supported by the kernel:
//     hstr --is-tiocsti
//   the command prints "y" or "n" (exit code is always 0) - it reports the
//   probe result i.e. it is NOT affected by HSTR_TIOCSTI=force
// - --show-configuration is based on the probe result as well, so that
//   forced TIOCSTI never makes HSTR emit configuration which does not work
//

// global TIOCSTI indicator declaration: TIOCSTI used to insert commands
// (detected or forced by HSTR_TIOCSTI=force)
extern bool is_tiocsti;
// global TIOCSTI indicator declaration: TIOCSTI detected by the probe
extern bool is_tiocsti_detected;

char* hstr_strdup(const char* s);
char* hstr_getenv_strdup(const char* name);
int hstr_strnwidth(const char* s, size_t n);
int hstr_strwidth(const char* s);
char* hstr_strelide(char* buffer, size_t bufferSize, const char* s, unsigned maxWidth);
void hstr_chop(char* s);
bool hstr_strcat_char(char* s, size_t size, int c);
bool hstr_strcat_char_fitting(char* s, size_t size, int c, int maxWidth);
bool is_tiocsti_supported(bool verbose);
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
