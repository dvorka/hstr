/*
 ============================================================================
 Name        : hstr.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Shell history completion utility
 ============================================================================
*/

#define _GNU_SOURCE

#include <curses.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <readline/chardefs.h>

#include "include/hashset.h"
#include "include/hstr_curses.h"
#include "include/hstr_history.h"
#include "include/hstr_utils.h"

#define SELECTION_CURSOR_IN_PROMPT -1
#define SELECTION_PREFIX_MAX_LNG 500
#define CMDLINE_LNG 250

#define Y_OFFSET_PROMPT 0
#define Y_OFFSET_HELP 2
#define Y_OFFSET_HISTORY 3
#define Y_OFFSET_ITEMS 4

#define K_CTRL_A 1
#define K_CTRL_E 5
#define K_CTRL_G 7
#define K_CTRL_H 8
#define K_CTRL_L 12
#define K_CTRL_R 18
#define K_CTRL_T 20
#define K_CTRL_U 21
#define K_CTRL_W 23
#define K_CTRL_X 24
#define K_CTRL_Z 26

#define K_CTRL_SLASH 31

#define K_ESC 27
#define K_TAB 9
#define K_BACKSPACE 127
#define K_ENTER 10

#ifdef DEBUG_KEYS
#define LOGKEYS(Y,KEY) mvprintw(Y, 0, "Key: '%3d' / Char: '%c'", KEY, KEY); clrtoeol()
#else
#define LOGKEYS(Y,KEY)
#endif

#ifdef DEBUG_CURPOS
#define LOGCURSOR(Y) mvprintw(Y, 0, "X/Y: %3d / %3d", getcurx(stdscr), getcury(stdscr))
#else
#define LOGCURSOR(Y)
#endif

static const char *INSTALL_STRING=
		"\n# add this configuration to ~/.bashrc"
		"\nshopt -s histappend              # append new history items to .bash_history"
		"\nexport HISTCONTROL=ignorespace   # leading space hides commands from history"
		"\nexport HISTFILESIZE=10000        # increase history file size (default is 500)"
		"\nexport HISTSIZE=${HISTFILESIZE}  # increase history size (default is 500)"
		"\nexport PROMPT_COMMAND=\"history -a; history -n; ${PROMPT_COMMAND}\"   # mem/file sync"
		"\nbind '\"\\C-r\": \"\\C-a hh \\C-j\"'    # bind hh to Ctrl-r"
		"\n\n";

static const char *HELP_STRING=
		"Usage: hh [option] [arg1] [arg2]..."
		"\nShell history suggest box (build: "__DATE__" " __TIME__")"
		"\n"
		"\n  --show-configuration    ... show configuration to be added to .bashrc"
		"\n  --help                  ... display this help and exit"
		"\n"
		"\nReport bugs to martin.dvorak@mindforger.com"
		"\nHome page: https://github.com/dvorka/hstr"
		"\n";

static const char *LABEL_HELP=
		 "Type to filter, UP/DOWN to move, DEL to remove, TAB to select, C-g to cancel";

static char **selection=NULL;
static unsigned selectionSize=0;
static bool caseSensitive=FALSE;
static bool defaultOrder=FALSE;
static char screenLine[1000];
static char cmdline[CMDLINE_LNG];

int print_prompt()
{
	char *hostname = get_hostname();
	char *user = getenv(ENV_VAR_USER);
	int xoffset = 1;

	mvprintw(xoffset, Y_OFFSET_PROMPT, "%s@%s$ ", user, hostname);
	refresh();

	return xoffset+strlen(user)+1+strlen(hostname)+1;
}

void print_help_label()
{
	snprintf(screenLine, getmaxx(stdscr), "%s", LABEL_HELP);
	mvprintw(Y_OFFSET_HELP, 0, screenLine);
	refresh();
}

void print_cmd_deleted_label(char *cmd, int occurences)
{
	snprintf(screenLine, getmaxx(stdscr), "History item '%s' deleted (%d occurrence%s)", cmd, occurences, (occurences==1?"":"s"));
	mvprintw(Y_OFFSET_HELP, 0, screenLine);
	clrtoeol();
	refresh();
}

void print_history_label(HistoryItems *history)
{
	sprintf(screenLine, "- HISTORY - case:%s (C-t) - order:%s (C-/) - %d/%d ",
			(caseSensitive?"sensitive":"insensitive"),
			(defaultOrder?"history":"ranking"),
			history->count,
			history->rawCount);
	int width=getmaxx(stdscr);
	width -= strlen(screenLine);
	if(width<0) {
		width = 0;
		screenLine[getmaxx(stdscr)]=0;
	}
	unsigned i;
	for (i=0; i < width; i++) {
		strcat(screenLine, "-");
	}
	color_attr_on(A_REVERSE);
	mvprintw(Y_OFFSET_HISTORY, 0, screenLine);
	color_attr_off(A_REVERSE);
	refresh();
}

void print_pattern(char *pattern, int y, int x) {
	color_attr_on(A_BOLD);
	mvprintw(y, x, "%s", pattern);
	color_attr_off(A_BOLD);
	clrtoeol();
}

unsigned get_max_history_items()
{
	return (getmaxy(stdscr)-Y_OFFSET_ITEMS);
}


void realloc_selection(unsigned size)
{
	selectionSize=size;
	if(selection!=NULL) {
		if(size>0) {
			selection=realloc(selection, size);
		} else {
			free(selection);
			selection=NULL;
		}
	} else {
		if(size>0) {
			selection = malloc(size);
		}
	}
}

unsigned make_selection(char *prefix, HistoryItems *history, int maxSelectionCount)
{
	realloc_selection(sizeof(char*) * maxSelectionCount);
	unsigned i, selectionCount=0;
	char **source=(defaultOrder?history->raw:history->items);

	for(i=0; i<history->count && selectionCount<maxSelectionCount; i++) {
		if(source[i]) {
			if(prefix==NULL) {
				selection[selectionCount++]=source[i];
			} else {
				if(caseSensitive) {
					if(source[i]==strstr(source[i], prefix)) {
						selection[selectionCount++]=source[i];
					}
				} else {
					if(source[i]==strcasestr(source[i], prefix)) {
						selection[selectionCount++]=source[i];
					}
				}
			}
		}
	}

	if(prefix && selectionCount<maxSelectionCount) {
		char *substring;
		for(i=0; i<history->count && selectionCount<maxSelectionCount; i++) {
			if(caseSensitive) {
				substring = strstr(source[i], prefix);
				if (substring != NULL && substring!=source[i]) {
					selection[selectionCount++]=source[i];
				}
			} else {
				substring = strcasestr(source[i], prefix);
				if (substring != NULL && substring!=source[i]) {
					selection[selectionCount++]=source[i];
				}
			}
		}
	}

	selectionSize=selectionCount;
	return selectionCount;
}

void print_selection_row(char *text, int y, int width, char *prefix) {
	snprintf(screenLine, width, " %s", text);
	mvprintw(y, 0, screenLine);
	if(prefix!=NULL && strlen(prefix)>0) {
		color_attr_on(A_BOLD);
		char *p;
		if(caseSensitive) {
			p=strstr(text, prefix);
			mvprintw(y, 1+(p-text), "%s", prefix);
		} else {
			p=strcasestr(text, prefix);
			snprintf(screenLine, strlen(prefix)+1, "%s", p);
			mvprintw(y, 1+(p-text), "%s", screenLine);
		}
		color_attr_off(A_BOLD);
	}
}

void print_highlighted_selection_row(char *text, int y, int width) {
	color_attr_on(A_REVERSE);
	color_attr_on(A_BOLD);
	snprintf(screenLine, getmaxx(stdscr), "%s%s", (terminal_has_colors()?" ":">"), text);
	mvprintw(y, 0, "%s", screenLine);
	color_attr_off(A_BOLD);
	color_attr_off(A_REVERSE);
}

char *print_selection(unsigned maxHistoryItems, char *prefix, HistoryItems *history)
{
	char *result="";
	unsigned selectionCount=make_selection(prefix, history, maxHistoryItems);
	if (selectionCount > 0) {
		result = selection[0];
	}

	int height=get_max_history_items(stdscr);
	int width=getmaxx(stdscr);
	unsigned i;
	int y=Y_OFFSET_ITEMS;

	move(Y_OFFSET_ITEMS, 0);
	clrtobot();

	for (i = 0; i<height; ++i) {
		if(i<selectionSize) {
			print_selection_row(selection[i], y++, width, prefix);
		} else {
			mvprintw(y++, 0, " ");
		}
	}
	refresh();

	return result;
}

void highlight_selection(int selectionCursorPosition, int previousSelectionCursorPosition, char *prefix)
{
	if(previousSelectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
		print_selection_row(
				selection[previousSelectionCursorPosition],
				Y_OFFSET_ITEMS+previousSelectionCursorPosition,
				getmaxx(stdscr),
				prefix);
	}
	if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
		print_highlighted_selection_row(
				selection[selectionCursorPosition],
				Y_OFFSET_ITEMS+selectionCursorPosition,
				getmaxx(stdscr));
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

void hstr_on_exit() {
	history_mgmt_flush();
	free_prioritized_history();
}

void signal_callback_handler_ctrl_c(int signum)
{
	if(signum==SIGINT) {
		endwin();
		hstr_on_exit();
		exit(signum);
	}
}

void loop_to_select(HistoryItems *history)
{
	signal(SIGINT, signal_callback_handler_ctrl_c);

	initscr();
	keypad(stdscr, TRUE);
	noecho();
	color_start();
	color_init_pair(1, COLOR_WHITE, COLOR_BLACK);
	color_attr_on(COLOR_PAIR(1));

	print_history_label(history);
	print_help_label();
	print_selection(get_max_history_items(stdscr), NULL, history);

	color_attr_off(COLOR_PAIR(1));

	bool done=FALSE, skip=TRUE, executeResult=FALSE, lowercase=TRUE;
	int basex=print_prompt(stdscr);
	int x=basex, y=1, c, cursorX=0, cursorY=0, maxHistoryItems, deleteOccurences;
	int width=getmaxx(stdscr);
	int selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
	int previousSelectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
	char *result="", *msg, *delete;
	char pattern[SELECTION_PREFIX_MAX_LNG];
	pattern[0]=0;
	strcpy(pattern, cmdline);
	while (!done) {
		maxHistoryItems=get_max_history_items(stdscr);

		if(!skip) {
			c = wgetch(stdscr);
		} else {
			if(strlen(pattern)>0) {
				color_attr_on(A_BOLD);
				mvprintw(y, basex, "%s", pattern);
				color_attr_off(A_BOLD);
				cursorX=getcurx(stdscr);
				cursorY=getcury(stdscr);
				result=print_selection(maxHistoryItems, pattern, history);
				move(cursorY, cursorX);
			}
			skip=FALSE;
			continue;
		}

		switch (c) {
		case KEY_DC:
			if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
				delete=selection[selectionCursorPosition];
				msg=malloc(strlen(delete)+1);
				strcpy(msg,delete);
				selection_remove(delete, history);
				deleteOccurences=history_mgmt_remove(delete);
				result=print_selection(maxHistoryItems, pattern, history);
				print_cmd_deleted_label(msg, deleteOccurences);
				move(y, basex+strlen(pattern));
			}
			print_history_label(history);
			break;
		case K_CTRL_T:
			caseSensitive=!caseSensitive;
			result=print_selection(maxHistoryItems, pattern, history);
			print_history_label(history);
			selectionCursorPosition=0;
			break;
		case K_CTRL_SLASH:
			defaultOrder=!defaultOrder;
			result=print_selection(maxHistoryItems, pattern, history);
			print_history_label(history);
			selectionCursorPosition=0;
			break;
		case KEY_RESIZE:
			print_history_label(history);
			move(y, basex+strlen(pattern));
			break;
		case K_CTRL_U:
		case K_CTRL_W: // TODO supposed to delete just one word backward
			pattern[0]=0;
			print_pattern(pattern, y, basex);
			break;
		case K_CTRL_L:
			toggle_case(pattern, lowercase);
			lowercase=!lowercase;
			print_pattern(pattern, y, basex);
			selectionCursorPosition=0;
			break;
		case K_CTRL_H:
		case K_BACKSPACE:
		case KEY_BACKSPACE:
			if(strlen(pattern)>0) {
				pattern[strlen(pattern)-1]=0;
				x--;
				print_pattern(pattern, y, basex);
			}

			if(strlen(pattern)>0) {
				make_selection(pattern, history, maxHistoryItems);
			} else {
				make_selection(NULL, history, maxHistoryItems);
			}
			result=print_selection(maxHistoryItems, pattern, history);

			move(y, basex+strlen(pattern));
			break;
		case KEY_UP:
			previousSelectionCursorPosition=selectionCursorPosition;
			if(selectionCursorPosition>0) {
				selectionCursorPosition--;
			} else {
				selectionCursorPosition=selectionSize-1;
			}
			highlight_selection(selectionCursorPosition, previousSelectionCursorPosition, pattern);
			move(y, basex+strlen(pattern));
			break;
		case K_CTRL_R:
		case KEY_DOWN:
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
			highlight_selection(selectionCursorPosition, previousSelectionCursorPosition, pattern);
			move(y, basex+strlen(pattern));
			break;
		case K_ENTER:
		case KEY_ENTER:
			executeResult=TRUE;
		case K_TAB:
		case KEY_LEFT:
		case KEY_RIGHT:
			if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
				result=selection[selectionCursorPosition];
			}
			done=TRUE;
			break;
		case K_CTRL_G:
		case K_ESC:
			result=NULL;
			history_clear_dirty();
			done=TRUE;
			break;
		case K_CTRL_X:
			result=NULL;
			done=TRUE;
			break;
		default:
			LOGKEYS(Y_OFFSET_HELP, c);
			LOGCURSOR(Y_OFFSET_HELP);

			if(c>K_CTRL_Z) {
				selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;

				if(strlen(pattern)<(width-basex-1)) {
					strcat(pattern, (char*)(&c));
					print_pattern(pattern, y, basex);
					cursorX=getcurx(stdscr);
					cursorY=getcury(stdscr);
				}

				result = print_selection(maxHistoryItems, pattern, history);
				move(cursorY, cursorX);
				refresh();
			}
			break;
		}
	}
	endwin();

	if(result!=NULL) {
		fill_terminal_input(result, TRUE);
		if(executeResult) {
			fill_terminal_input("\n", FALSE);
		}
	}
}

void assemble_cmdline(int argc, char *argv[]) {
	// TODO support BASH substitutions: !!, !!ps, !$, !*
	int i;
	cmdline[0]=0;
	for(i=1; i<argc; i++) {
		if((strlen(cmdline)+strlen(argv[i])*2)>CMDLINE_LNG) break;
		if(strstr(argv[i], " ")) {
			strcat(cmdline, "\"");
		}
		strcat(cmdline, argv[i]);
		if(strstr(argv[i], " ")) {
			strcat(cmdline, "\"");
		}
		if((i+1<argc)) {
			strcat(cmdline, " ");
		}
	}
}

void hstr()
{
	HistoryItems *history=get_prioritized_history();
	if(history) {
		history_mgmt_open();
		loop_to_select(history);
		hstr_on_exit();
	} else {
		printf("Empty shell history - nothing to suggest...\n");
	}
}

int main(int argc, char *argv[])
{
	if(argc>0) {
		if(argc==2) {
			if(strstr(argv[1], "--show-configuration")) {
				printf("%s", INSTALL_STRING);
				return EXIT_SUCCESS;
			}
			if(strstr(argv[1], "--help")) {
				printf("%s", HELP_STRING);
				return EXIT_SUCCESS;
			}
		} else {
			assemble_cmdline(argc, argv);
			hstr();
		}
	} else {
		cmdline[0]=0;
		hstr();
	}

	return EXIT_SUCCESS;
}
