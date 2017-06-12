/*
 hstr_curses.h      header file for curses helpers

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

#ifndef _HSTR_CURSES_H
#define _HSTR_CURSES_H

#ifdef __APPLE__
#include <curses.h>
#else
#include <ncursesw/curses.h>
#endif

#define color_attr_on(C) if(terminal_has_colors()) { attron(C); }
#define color_attr_off(C) if(terminal_has_colors()) { attroff(C); }
#define color_init_pair(X, Y, Z) if(terminal_has_colors()) { init_pair(X, Y, Z); }

void hstr_curses_start();
bool terminal_has_colors();
void hstr_curses_stop(bool keepPage);

#endif
