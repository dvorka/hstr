/*
 ============================================================================
 Name        : hstr_curses.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : Curses utilities
               http://pueblo.sourceforge.net/doc/manual/ansi_color_codes.html
 ============================================================================
*/

#include "include/hstr_curses.h"

static bool terminalHasColors=FALSE;

void hstr_curses_start()
{
	initscr();
	keypad(stdscr, TRUE);
	noecho();
	terminalHasColors=has_colors();
	if(terminalHasColors) {
		start_color();
		use_default_colors();
	}
}

bool terminal_has_colors() {
	return terminalHasColors;
}

void hstr_curses_stop() {
    clear();
    refresh();
    doupdate();
    endwin();
}
