/*
 test_*.c       HSTR test

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

#include <ncursesw/curses.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    initscr();
    noecho();
    keypad(stdscr, TRUE);

    int c;
    while(TRUE) {
        c = getch();

        mvprintw(1, 0, "Key number: '%3d' / Char: '%c'", c, c);

        switch(c) {
        // ctrl-r, ctrl-h, ctrl-i
        case KEY_BACKSPACE:
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_UP:
        case KEY_DOWN:
            mvprintw(5, 0, "CATCHED! %3d",c);
            break;

        case KEY_STAB:
        case KEY_BTAB:
        case KEY_CTAB:
            mvprintw(5, 0, "TAB! %3d",c);
            break;

        case KEY_RESIZE:
            mvprintw(5, 0, "RESIZE! %3d",c);
            break;

        case KEY_ENTER:
            endwin();
            exit(0);
        default:
            break;
        }
    }

    endwin();
}
