/*
 ============================================================================
 Name        : hstr_curses.h
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : Curses helpers
 ============================================================================
*/

#ifndef _HSTR_CURSES_H
#define _HSTR_CURSES_H

#include <curses.h>

#define color_attr_on(C) if(terminal_has_colors()) { attron(C); }
#define color_attr_off(C) if(terminal_has_colors()) { attroff(C); }
#define color_init_pair(X, Y, Z) if(terminal_has_colors()) { init_pair(X, Y, Z); }

void hstr_curses_start();
bool terminal_has_colors();
void hstr_curses_stop();

#endif
