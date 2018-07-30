/*
 hstr_curses.c   Curses utilities
                 http://pueblo.sourceforge.net/doc/manual/ansi_color_codes.html

 Copyright (C) 2014-2018  Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "include/hstr_curses.h"

static bool terminalHasColors=FALSE;

void hstr_curses_start()
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    nonl(); // prevent carriage return from being mapped to newline
    terminalHasColors=has_colors();
    if(terminalHasColors) {
        start_color();
        use_default_colors();
    }

#if defined(NCURSES_EXT_FUNCS) && NCURSES_EXT_FUNCS >= 20081102
    // Use ncurses specific function to make delay after pressing escape key
    // unnoticeable.  Can be zero, but in some corner cases multiple bytes
    // composing a functional key code might be handled to the application with
    // a delay, so small delay is safer.
    set_escdelay(5);
#endif
}

bool terminal_has_colors() {
    return terminalHasColors;
}

void hstr_curses_stop(bool keepPage) {
    if(!keepPage) {
        clear();
    }
    refresh();
    doupdate();
    endwin();
}
