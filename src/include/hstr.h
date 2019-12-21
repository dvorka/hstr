/*
 hstr.h   header file for HSTR shell history completion utility

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

#ifndef HSTR_H
#define HSTR_H

#include <getopt.h>
#include <locale.h>
#ifdef __APPLE__
  #include <curses.h>
#elif defined(__FreeBSD__)
  #include <ncurses.h>
#else
  #include <ncursesw/curses.h>
#endif
#include <readline/chardefs.h>
#include <signal.h>
#include <termios.h>
#include <wchar.h>

#include "hstr_curses.h"
#include "hstr_blacklist.h"
#include "hstr_history.h"

int hstr_main(int argc, char* argv[]);

#endif
