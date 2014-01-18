/*
 ============================================================================
 Name        : test_curses_keyb.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : A test
 ============================================================================
*/

#include <curses.h>
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
