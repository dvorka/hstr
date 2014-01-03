/*
 ============================================================================
 Name        : hstr.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Shell history completion utility
 ============================================================================
*/

#include <curses.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "include/hashset.h"
#include "include/hstr_utils.h"
#include "include/hstr_history.h"

#define LABEL_HISTORY " HISTORY "
#define FILE_BASHRC ".bashrc"
#define SELECTION_CURSOR_IN_PROMPT -1
#define SELECTION_PREFIX_MAX_LNG 500

#define Y_OFFSET_PROMPT 0
#define Y_OFFSET_HELP 2
#define Y_OFFSET_HISTORY 3
#define Y_OFFSET_ITEMS 4

#define KEY_TERMINAL_RESIZE 410
#define KEY_CTRL_A 1
#define KEY_CTRL_E 5
#define KEY_CTRL_R 18
#define KEY_CTRL_X 24

#ifdef DEBUG_KEYS
#define LOGKEYS(Y,KEY) mvprintw(Y, 0, "Key number: '%3d' / Char: '%c'", KEY, KEY)
#else
#define LOGKEYS(Y,KEY)
#endif

#ifdef DEBUG_CURPOS
#define LOGCURSOR(Y) mvprintw(Y, 0, "X/Y: %3d / %3d", getcurx(stdscr), getcury(stdscr))
#else
#define LOGCURSOR(Y)
#endif

static const char *INSTALL_STRING=
		 "\nshopt -s histappend"
		 "\nexport PROMPT_COMMAND=\"history -a; history -n; ${PROMPT_COMMAND}\""
		 "\nbind '\"\\C-r\": \"\\C-k\\C-uhh\\C-j\"'"
		 "\n\n";

static char *LABEL_HELP=
		 "Type to filter, UP/DOWN to move, Ctrl-r to remove, ENTER to select, Ctrl-x to exit";

static char **selection=NULL;
static unsigned selectionSize=0;
static bool terminalHasColors=FALSE;
static char screenLine[1000];

int print_prompt(WINDOW *win)
{
	char *hostname = get_hostname();
	char *user = getenv(ENV_VAR_USER);
	int xoffset = 1;

	mvwprintw(win, xoffset, Y_OFFSET_PROMPT, "%s@%s$ ", user, hostname);
	refresh();

	return xoffset+strlen(user)+1+strlen(hostname)+1;
}

void print_help_label(WINDOW *win)
{
	snprintf(screenLine, getmaxx(win), "%s", LABEL_HELP);
	mvwprintw(win, Y_OFFSET_HELP, 0, screenLine);
	refresh();
}

void print_cmd_deleted_label(WINDOW *win, char *cmd, int occurences)
{
	snprintf(screenLine, getmaxx(win), "History item '%s' deleted (%d occurrence%s)", cmd, occurences, (occurences==1?"":"s"));
	mvwprintw(win, Y_OFFSET_HELP, 0, screenLine);
	clrtoeol();
	refresh();
}

void print_history_label(WINDOW *win)
{
	char message[512];

	int width=getmaxx(win);

	strcpy(message, LABEL_HISTORY);
	width -= strlen(LABEL_HISTORY);
	unsigned i;
	for (i=0; i < width; i++) {
		strcat(message, " ");
	}

	wattron(win, A_REVERSE);
	mvwprintw(win, Y_OFFSET_HISTORY, 0, message);
	wattroff(win, A_REVERSE);

	refresh();
}

unsigned get_max_history_items(WINDOW *win)
{
	return (getmaxy(win)-Y_OFFSET_ITEMS);
}


void alloc_selection(unsigned size)
{
	selectionSize=size;
	if(selection!=NULL) {
		free(selection);
		selection=NULL;
	}
	if(size>0) {
		selection = malloc(size);
	}
}

unsigned make_selection(char *prefix, HistoryItems *history, int maxSelectionCount)
{
	alloc_selection(sizeof(char*) * maxSelectionCount); // TODO realloc
	unsigned i, selectionCount=0;

	for(i=0; i<history->count && selectionCount<maxSelectionCount; i++) {
		if(history->items[i]) {
			if(prefix==NULL) {
				selection[selectionCount++]=history->items[i];
			} else {
				if(history->items[i]==strstr(history->items[i], prefix)) {
					selection[selectionCount++]=history->items[i];
				}
			}
		}
	}

	if(prefix && selectionCount<maxSelectionCount) {
		for(i=0; i<history->count && selectionCount<maxSelectionCount; i++) {
			char *substring = strstr(history->items[i], prefix);
			if (substring != NULL && substring!=history->items[i]) {
				selection[selectionCount++]=history->items[i];
			}
		}
	}

	selectionSize=selectionCount;
	return selectionCount;
}

char *print_selection(WINDOW *win, unsigned maxHistoryItems, char *prefix, HistoryItems *history)
{
	char *result="";
	unsigned selectionCount=make_selection(prefix, history, maxHistoryItems);
	if (selectionCount > 0) {
		result = selection[0];
	}

	int height=get_max_history_items(win);
	int width=getmaxx(win);
	unsigned i;
	int y=Y_OFFSET_ITEMS;

	move(Y_OFFSET_ITEMS, 0);
	wclrtobot(win);

	for (i = 0; i<height; ++i) {
		if(i<selectionSize) {
			snprintf(screenLine, width, " %s", selection[i]);
			mvwprintw(win, y++, 0, screenLine);
			if(prefix!=NULL) {
				wattron(win,A_BOLD);
				char *p=strstr(selection[i], prefix);
				mvwprintw(win, (y-1), 1+(p-selection[i]), "%s", prefix);
				wattroff(win,A_BOLD);
			}
		} else {
			mvwprintw(win, y++, 0, " ");
		}
	}
	refresh();

	return result;
}

void highlight_selection(int selectionCursorPosition, int previousSelectionCursorPosition)
{
	if(previousSelectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
		mvprintw(Y_OFFSET_ITEMS+previousSelectionCursorPosition, 0, " ");
	}
	if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
		mvprintw(Y_OFFSET_ITEMS+selectionCursorPosition, 0, ">");
	}
}

void color_start()
{
	terminalHasColors=has_colors();
	if(terminalHasColors) {
		start_color();
	}
}

void color_init_pair(short int x, short int y, short int z)
{
	if(terminalHasColors) {
		init_pair(x, y, z);
	}
}

void color_attr_on(int c)
{
	if(terminalHasColors) {
		attron(c);
	}
}

void color_attr_off(int c)
{
	if(terminalHasColors) {
		attroff(c);
	}
}

void selection_remove(char *cmd, HistoryItems *history)
{
	if(history->count) {
		int i, w;
		for(i=0, w=0; i<history->count; i++) {
			if(strcmp(history->items[i], cmd)) {
				history->items[w]=history->items[i];
				w++;
			}
		}
		history->count=w;
	}
}

char *selection_loop(HistoryItems *history)
{
	initscr();
	color_start();

	color_init_pair(1, COLOR_WHITE, COLOR_BLACK);
	color_attr_on(COLOR_PAIR(1));
	print_history_label(stdscr);
	print_help_label(stdscr);
	print_selection(stdscr, get_max_history_items(stdscr), NULL, history);
	int basex = print_prompt(stdscr);
	int x = basex;
	int width=getmaxx(stdscr);
	color_attr_off(COLOR_PAIR(1));

	int selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
	int previousSelectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;

	int y = 1, c, maxHistoryItems, cursorX=0, cursorY=0, deleteOccurences;
	bool done = FALSE;
	char prefix[SELECTION_PREFIX_MAX_LNG]="";
	char *result="", *msg, *delete;
	while (!done) {
		maxHistoryItems=get_max_history_items(stdscr);

		noecho();
		c = wgetch(stdscr);
		echo();

		switch (c) {
		case KEY_TERMINAL_RESIZE:
		case KEY_CTRL_A:
		case KEY_CTRL_E:
			break;
		case KEY_CTRL_R:
			if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
				delete=selection[selectionCursorPosition];
				msg=malloc(strlen(delete)+1);
				strcpy(msg,delete);
				selection_remove(delete, history);
				deleteOccurences=history_mgmt_remove(delete);
				result = print_selection(stdscr, maxHistoryItems, prefix, history);
				print_cmd_deleted_label(stdscr, msg, deleteOccurences);
				move(y, basex+strlen(prefix));
			}
			break;
		case 91: // TODO 91 killed > debug to determine how to distinguish \e and [
		case 68: // left arrow
		case 67: // rigtht arrow
			break;
		case KEY_BACKSPACE:
		case 127:
			if(strlen(prefix)>0) {
				prefix[strlen(prefix)-1]=0;
				x--;
				wattron(stdscr,A_BOLD);
				mvprintw(y, basex, "%s", prefix);
				wattroff(stdscr,A_BOLD);
				clrtoeol();
			}

			if(strlen(prefix)>0) {
				make_selection(prefix, history, maxHistoryItems);
			} else {
				make_selection(NULL, history, maxHistoryItems);
			}
			result = print_selection(stdscr, maxHistoryItems, prefix, history);

			move(y, basex+strlen(prefix));
			break;
		case KEY_UP:
		case 65:
			previousSelectionCursorPosition=selectionCursorPosition;
			if(selectionCursorPosition>0) {
				selectionCursorPosition--;
			} else {
				selectionCursorPosition=selectionSize-1;
			}
			highlight_selection(selectionCursorPosition, previousSelectionCursorPosition);
			break;
		case KEY_DOWN:
		case 66:
			if(selectionCursorPosition==SELECTION_CURSOR_IN_PROMPT) {
				selectionCursorPosition=previousSelectionCursorPosition=0;
			} else {
				previousSelectionCursorPosition=selectionCursorPosition;
				if((selectionCursorPosition+1)<selectionSize) {
					selectionCursorPosition++;
				} else {
					selectionCursorPosition=0;
				}
			}
			highlight_selection(selectionCursorPosition, previousSelectionCursorPosition);
			break;
		case 10:
			if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
				result=selection[selectionCursorPosition];
				alloc_selection(0);
			}
			done = TRUE;
			break;
		case KEY_CTRL_X:
			result = NULL;
			done = TRUE;
			break;
		default:
			LOGKEYS(Y_OFFSET_HELP,c);
			LOGCURSOR(Y_OFFSET_HELP);

			if(c!=27) {
				selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;

				if(strlen(prefix)<(width-basex-1)) {
					strcat(prefix, (char*)(&c));
					wattron(stdscr,A_BOLD);
					mvprintw(y, basex, "%s", prefix);
					cursorX=getcurx(stdscr);
					cursorY=getcury(stdscr);
					wattroff(stdscr,A_BOLD);
					clrtoeol();
				}

				result = print_selection(stdscr, maxHistoryItems, prefix, history);
				move(cursorY, cursorX);
				refresh();
			}
			break;
		}
	}
	endwin();

	return result;
}

void install_write()
{
	char *home = getenv(ENV_VAR_HOME);
	sprintf(screenLine, "%s/%s", home, FILE_BASHRC);
	FILE *file = fopen(screenLine,"a");
	fseek(file,0, SEEK_END);
	fprintf(file,"%s",INSTALL_STRING);
	fprintf(file,"%s","\n\n");
	fclose(file);
}

void install_show()
{
	printf("%s", INSTALL_STRING);
}

void hstr()
{
	HistoryItems *history=get_prioritized_history();
	history_mgmt_open();
	char *command = selection_loop(history);
	history_mgmt_close();
	fill_terminal_input(command, true);
	free_prioritized_history();
}

int main(int argc, char *argv[])
{
	if(argc>1) {
		install_show();
	} else {
		hstr();
	}
	return EXIT_SUCCESS;
}

