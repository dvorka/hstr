/*
 hstr_curses.h      header file for curses helpers

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

#ifndef HSTR_CURSES_H
#define HSTR_CURSES_H

// the only place where the curses header is chosen
#ifdef __APPLE__
  #include <curses.h>
#elif defined(__FreeBSD__)
  #include <ncurses.h>
#else
  // distributions w/o ncursesw/ directory (e.g. Arch Linux) ship wide-char capable curses.h
  #if defined(__has_include)
    // - it is used only if ncursesw/curses.h is not available
    #if __has_include(<ncursesw/curses.h>)
      #include <ncursesw/curses.h>
    #else
      #include <curses.h>
    #endif
  #else
    #include <ncursesw/curses.h>
  #endif
#endif

#define color_attr_on(C) if(terminal_has_colors()) { attron(C); }
#define color_attr_off(C) if(terminal_has_colors()) { attroff(C); }
#define color_init_pair(X, Y, Z) if(terminal_has_colors()) { init_pair(X, Y, Z); }

void hstr_curses_start(void);
bool terminal_has_colors(void);
void hstr_curses_stop(bool keepPage);

#endif
