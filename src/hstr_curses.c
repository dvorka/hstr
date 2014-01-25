/*
 ============================================================================
 Name        : hstr_curses.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Curses utilities
 ============================================================================
*/

#include "include/hstr_curses.h"

static bool terminalHasColors=FALSE;

void color_start()
{
	terminalHasColors=has_colors();
	if(terminalHasColors) {
		start_color();
	}
}

bool terminal_has_colors() {
	return terminalHasColors;
}
