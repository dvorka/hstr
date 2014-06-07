/*
 ============================================================================
 Name        : hstr.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : BASH history completion utility
 ============================================================================
*/

#define _GNU_SOURCE

#include <curses.h>
#include <getopt.h>
#include <regex.h>
#include <locale.h>
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
#include "include/hstr_favorites.h"
#include "include/hstr_history.h"
#include "include/hstr_regexp.h"
#include "include/hstr_utils.h"

#define SELECTION_CURSOR_IN_PROMPT -1
#define SELECTION_PREFIX_MAX_LNG 512
#define CMDLINE_LNG 2048
#define HOSTNAME_BUFFER 128

#define Y_OFFSET_PROMPT 0
#define Y_OFFSET_HELP 1
#define Y_OFFSET_HISTORY 2
#define Y_OFFSET_ITEMS 3

#define K_CTRL_A 1
#define K_CTRL_E 5
#define K_CTRL_F 6
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

#define HH_COLOR_NORMAL  1
#define HH_COLOR_HIROW   2
#define HH_COLOR_INFO    2
#define HH_COLOR_PROMPT  3
#define HH_COLOR_DELETE  4

#define HH_ENV_VAR_CONFIG  "HH_CONFIG"

#define HH_CONFIG_MONO      "monochromatic"
#define HH_CONFIG_HICOLOR   "hicolor"
#define HH_CONFIG_CASE      "casesensitive"
#define HH_CONFIG_REGEXP    "regexp"
#define HH_CONFIG_SUBSTRING "substring"
#define HH_CONFIG_SORTING   "rawhistory"
#define HH_CONFIG_FAVORITES "favorites"
#define HH_CONFIG_DEBUG     "debug"
#define HH_CONFIG_WARN      "warning"

#define HH_DEBUG_LEVEL_NONE  0
#define HH_DEBUG_LEVEL_WARN  1
#define HH_DEBUG_LEVEL_DEBUG 2

#define HH_VIEW_RANKING		0
#define HH_VIEW_HISTORY		1
#define HH_VIEW_FAVORITES	2

#define HH_MATCH_SUBSTRING 	0
#define HH_MATCH_REGEXP     1

#define HH_CASE_INSENSITIVE	0
#define HH_CASE_SENSITIVE	1

#define SPACE_PADDING "                                                              "

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

static const char *HH_VIEW_LABELS[]={
		"ranking",
		"history",
		"favorites"
};

static const char *HH_MATCH_LABELS[]={
		"exact",
		"regexp"
};

static const char *HH_CASE_LABELS[]={
		"insensitive",
		"sensitive"
};

static const char *INSTALL_STRING=
		"\n# add this configuration to ~/.bashrc"
		"\nexport HH_CONFIG=hicolor         # get more colors"
		"\nshopt -s histappend              # append new history items to .bash_history"
		"\nexport HISTCONTROL=ignorespace   # leading space hides commands from history"
		"\nexport HISTFILESIZE=10000        # increase history file size (default is 500)"
		"\nexport HISTSIZE=${HISTFILESIZE}  # increase history size (default is 500)"
		"\nexport PROMPT_COMMAND=\"history -a; history -n; ${PROMPT_COMMAND}\"   # mem/file sync"
		"\n# if this is interactive shell, then bind hh to Ctrl-r"
		"\nif [[ $- =~ .*i.* ]]; then bind '\"\\C-r\": \"\\C-a hh \\C-j\"'; fi"
		"\n\n";

static const char *HELP_STRING=
		"Usage: hh [option] [arg1] [arg2]..."
		"\nShell history suggest box:"
		"\n"
		"\n  --favorites          -f ... show favorites view"
		"\n  --non-interactive    -n ... print filtered history and exit"
		"\n  --show-configuration -s ... show configuration to be added to ~/.bashrc"
		"\n  --version            -V ... show version details"
		"\n  --help               -h ... help"
		"\n"
		"\nReport bugs to martin.dvorak@mindforger.com"
		"\nHome page: https://github.com/dvorka/hstr"
		"\n";

static const char *VERSION_STRING=
		"hh version \"1.13\""
		"\n   build   \""__DATE__" " __TIME__"\""
		"\n";

// TODO help screen - tig
static const char *LABEL_HELP=
		 "Type to filter, UP/DOWN move, DEL remove, TAB select, C-f add favorite, C-g cancel";

#define GETOPT_NO_ARGUMENT 		    0
#define GETOPT_REQUIRED_ARGUMENT 	1
#define GETOPT_OPTIONAL_ARGUMENT 	2

static const struct option long_options[] = {
		{"favorites",     	   GETOPT_NO_ARGUMENT, NULL, 'f'},
		{"version",  		   GETOPT_NO_ARGUMENT, NULL, 'V'},
		{"help",  		       GETOPT_NO_ARGUMENT, NULL, 'h'},
		{"non-interactive",    GETOPT_NO_ARGUMENT, NULL, 'n'},
		{"show-configuration", GETOPT_NO_ARGUMENT, NULL, 's'},
		{0,         		   0,           NULL,  0 }
};

typedef struct {
	HistoryItems *history;
	FavoriteItems *favorites;

	char **selection;
	regmatch_t *selectionRegexpMatch;
	unsigned selectionSize;

	int historyMatch; // TODO patternMatching: exact, regexp
	int historyView; // TODO view: favs, ...
	int caseSensitive;

	bool interactive;

	bool hicolor;
	int debugLevel;

	HstrRegexp regexp;

	char cmdline[CMDLINE_LNG];
} Hstr;

static Hstr *hstr;

void hstr_init(Hstr *hstr)
{
	hstr->selection=NULL;
	hstr->selectionRegexpMatch=NULL;
	hstr->selectionSize=0;

	hstr->historyMatch=HH_MATCH_SUBSTRING;
	hstr->historyView=HH_VIEW_RANKING;
	hstr->caseSensitive=HH_CASE_INSENSITIVE;

	hstr->interactive=true;

	hstr->hicolor=FALSE;

	hstr->debugLevel=HH_DEBUG_LEVEL_NONE;
	hstr->cmdline[0]=0;
	hstr_regexp_init(&hstr->regexp);
}

void hstr_get_env_configuration(Hstr *hstr)
{
	char *hstr_config=getenv(HH_ENV_VAR_CONFIG);
	if(hstr_config && strlen(hstr_config)>0) {
		if(strstr(hstr_config,HH_CONFIG_HICOLOR)) {
			hstr->hicolor=TRUE;
		}
		if(strstr(hstr_config,HH_CONFIG_CASE)) {
			hstr->caseSensitive=HH_CASE_SENSITIVE;
		}
		if(strstr(hstr_config,HH_CONFIG_REGEXP)) {
			hstr->historyMatch=HH_MATCH_REGEXP;
		} else {
			if(strstr(hstr_config,HH_CONFIG_SUBSTRING)) {
				hstr->historyMatch=HH_MATCH_SUBSTRING;
			}
		}
		if(strstr(hstr_config,HH_CONFIG_SORTING)) {
			hstr->historyView=HH_VIEW_HISTORY;
		} else {
			if(strstr(hstr_config,HH_CONFIG_FAVORITES)) {
				hstr->historyView=HH_VIEW_FAVORITES;
			}
		}

		if(strstr(hstr_config,HH_CONFIG_DEBUG)) {
			hstr->debugLevel=HH_DEBUG_LEVEL_DEBUG;
		} else {
			if(strstr(hstr_config,HH_CONFIG_WARN)) {
				hstr->debugLevel=HH_DEBUG_LEVEL_WARN;
			}
		}
	}
}

int print_prompt(Hstr *hstr)
{
	char *user = getenv(ENV_VAR_USER);
	char *hostname=malloc(HOSTNAME_BUFFER);
	int xoffset = 0, promptLength;

	if(hstr->hicolor) {
		color_attr_on(COLOR_PAIR(HH_COLOR_PROMPT));
		color_attr_on(A_BOLD);
	}
	user=(user?user:"me");
	get_hostname(HOSTNAME_BUFFER, hostname);
	mvprintw(Y_OFFSET_PROMPT, xoffset, "%s@%s$ ", user, hostname);
	promptLength=strlen(user)+1+strlen(hostname)+1+1;
	free(hostname);
	if(hstr->hicolor) {
		color_attr_off(A_BOLD);
		color_attr_off(COLOR_PAIR(HH_COLOR_PROMPT));
	}
	refresh();

	return promptLength;
}

void print_help_label()
{
	char screenLine[CMDLINE_LNG];
	snprintf(screenLine, getmaxx(stdscr), "%s", LABEL_HELP);
	mvprintw(Y_OFFSET_HELP, 0, "%s", screenLine); clrtoeol();
	refresh();
}

void print_cmd_deleted_label(const char *cmd, int occurences, Hstr *hstr)
{
	char screenLine[CMDLINE_LNG];
	snprintf(screenLine, getmaxx(stdscr), "History item '%s' deleted (%d occurrence%s)", cmd, occurences, (occurences==1?"":"s"));
	// TODO make this function
	if(hstr->hicolor) {
		color_attr_on(COLOR_PAIR(HH_COLOR_DELETE));
		color_attr_on(A_BOLD);
	}
	mvprintw(Y_OFFSET_HELP, 0, "%s", screenLine);
	if(hstr->hicolor) {
		color_attr_off(A_BOLD);
		color_attr_on(COLOR_PAIR(1));
	}
	clrtoeol();
	refresh();
}

void print_regexp_error(const char *errorMessage)
{
	char screenLine[CMDLINE_LNG];
	snprintf(screenLine, getmaxx(stdscr), "%s", errorMessage);
	if(hstr->hicolor) {
		color_attr_on(COLOR_PAIR(HH_COLOR_DELETE));
		color_attr_on(A_BOLD);
	}
	mvprintw(Y_OFFSET_HELP, 0, "%s", screenLine);
	if(hstr->hicolor) {
		color_attr_off(A_BOLD);
		color_attr_on(COLOR_PAIR(1));
	}
	clrtoeol();
	refresh();
}

void print_cmd_added_favorite_label(const char *cmd, Hstr *hstr)
{
	char screenLine[CMDLINE_LNG];
	snprintf(screenLine, getmaxx(stdscr), "Command '%s' added to favorites (C-/ to show favorites)", cmd);
	if(hstr->hicolor) {
		color_attr_on(COLOR_PAIR(HH_COLOR_INFO));
		color_attr_on(A_BOLD);
	}
	mvprintw(Y_OFFSET_HELP, 0, screenLine);
	if(hstr->hicolor) {
		color_attr_off(A_BOLD);
		color_attr_on(COLOR_PAIR(1));
	}
	clrtoeol();
	refresh();
}

void print_history_label(Hstr *hstr)
{
	int width=getmaxx(stdscr);

	char screenLine[CMDLINE_LNG];
	snprintf(screenLine, width, "- HISTORY - view:%s (C-/) - match:%s (C-e) - case:%s (C-t) - %d/%d/%d ",
			HH_VIEW_LABELS[hstr->historyView],
			HH_MATCH_LABELS[hstr->historyMatch],
			HH_CASE_LABELS[hstr->caseSensitive],
			hstr->history->count,
			hstr->history->rawCount,
			hstr->favorites->count);
	width -= strlen(screenLine);
	unsigned i;
	for (i=0; i < width; i++) {
		strcat(screenLine, "-");
	}
	if(hstr->hicolor) {
		color_attr_on(A_BOLD);
	}
	color_attr_on(A_REVERSE);
	mvprintw(Y_OFFSET_HISTORY, 0, "%s", screenLine);
	color_attr_off(A_REVERSE);
	if(hstr->hicolor) {
		color_attr_off(A_BOLD);
	}
	refresh();
}

void print_prefix(char *pattern, int y, int x)
{
	color_attr_on(A_BOLD);
	mvprintw(y, x, "%s", pattern);
	color_attr_off(A_BOLD);
	clrtoeol();
}

unsigned get_max_history_items()
{
	return (getmaxy(stdscr)-Y_OFFSET_ITEMS);
}

// TODO don't realloc if size doesn't change
void hstr_realloc_selection(unsigned size, Hstr *hstr)
{
	if(hstr->selection) {
		if(size) {
			hstr->selection
				=realloc(hstr->selection, sizeof(char*) * size);
			hstr->selectionRegexpMatch
				=realloc(hstr->selectionRegexpMatch, sizeof(regmatch_t) * size);
		} else {
			free(hstr->selection);
			free(hstr->selectionRegexpMatch);
			hstr->selection=NULL;
			hstr->selectionRegexpMatch=NULL;
		}
	} else {
		if(size) {
			hstr->selection = malloc(sizeof(char*) * size);
			hstr->selectionRegexpMatch = malloc(sizeof(regmatch_t) * size);
		}
	}
}

unsigned hstr_make_selection(char *prefix, HistoryItems *history, int maxSelectionCount, Hstr *hstr)
{
	hstr_realloc_selection(maxSelectionCount, hstr);

	unsigned i, selectionCount=0;
	char **source;
	unsigned count;

	switch(hstr->historyView) {
	case HH_VIEW_HISTORY:
		source=history->raw;
		count=history->rawCount;
		break;
	case HH_VIEW_FAVORITES:
		source=hstr->favorites->items;
		count=hstr->favorites->count;
		break;
	case HH_VIEW_RANKING:
	default:
		source=history->items;
		count=history->count;
		break;
	}

	regmatch_t regexpMatch;
	char regexpErrorMessage[CMDLINE_LNG];
	bool regexpCompilationError=false;
	for(i=0; i<count && selectionCount<maxSelectionCount; i++) {
		if(source[i]) {
			if(!prefix || !strlen(prefix)) {
				hstr->selection[selectionCount++]=source[i];
			} else {
				switch(hstr->historyMatch) {
				case HH_MATCH_SUBSTRING:
					switch(hstr->caseSensitive) {
					case HH_CASE_SENSITIVE:
						if(source[i]==strstr(source[i], prefix)) {
							hstr->selection[selectionCount++]=source[i];
						}
						break;
					case HH_CASE_INSENSITIVE:
						if(source[i]==strcasestr(source[i], prefix)) {
							hstr->selection[selectionCount++]=source[i];
						}
						break;
					}
					break;
				case HH_MATCH_REGEXP:
					if(hstr_regexp_match(&(hstr->regexp), prefix, source[i], &regexpMatch, regexpErrorMessage, CMDLINE_LNG)) {
						hstr->selection[selectionCount]=source[i];
						hstr->selectionRegexpMatch[selectionCount].rm_so=regexpMatch.rm_so;
						hstr->selectionRegexpMatch[selectionCount].rm_eo=regexpMatch.rm_eo;
						selectionCount++;
					} else {
						if(!regexpCompilationError) {
							// TODO fix broken messages - getting just escape sequences
							// print_regexp_error(regexpErrorMessage);
							regexpCompilationError=true;
						}
					}
					break;
				}
			}
		}
	}

	if(prefix && selectionCount<maxSelectionCount) {
		char *substring;
		for(i=0; i<count && selectionCount<maxSelectionCount; i++) {
			switch(hstr->historyMatch) {
			case HH_MATCH_SUBSTRING:
				switch(hstr->caseSensitive) {
				case HH_CASE_SENSITIVE:
					substring = strstr(source[i], prefix);
					if (substring != NULL && substring!=source[i]) {
						hstr->selection[selectionCount++]=source[i];
					}
					break;
				case HH_CASE_INSENSITIVE:
					substring = strcasestr(source[i], prefix);
					if (substring != NULL && substring!=source[i]) {
						hstr->selection[selectionCount++]=source[i];
					}
					break;
				}
				break;
			case HH_MATCH_REGEXP:
				// all regexps matched previously - user decides whether match ^ or infix
			break;
			}
		}
	}

	hstr->selectionSize=selectionCount;
	return selectionCount;
}

void print_selection_row(char *text, int y, int width, char *pattern)
{
	char screenLine[CMDLINE_LNG];
	snprintf(screenLine, width, " %s", text);
	mvprintw(y, 0, "%s", screenLine); clrtoeol();

	if(pattern && strlen(pattern)) {
		color_attr_on(A_BOLD);
		char *p;

		switch(hstr->historyMatch) {
		case HH_MATCH_SUBSTRING:
			switch(hstr->caseSensitive) {
			case HH_CASE_INSENSITIVE:
				p=strcasestr(text, pattern);
				snprintf(screenLine, strlen(pattern)+1, "%s", p);
				mvprintw(y, 1+(p-text), "%s", screenLine);
				break;
			case HH_CASE_SENSITIVE:
				p=strstr(text, pattern);
				mvprintw(y, 1+(p-text), "%s", pattern);
				break;
			}
			break;
		case HH_MATCH_REGEXP:
			p=strstr(text, pattern);
			mvprintw(y, 1+(p-text), "%s", pattern);
			break;
		}
		color_attr_off(A_BOLD);
	}
}

void hstr_print_highlighted_selection_row(char *text, int y, int width, Hstr *hstr)
{
	color_attr_on(A_BOLD);
	if(hstr->hicolor) {
		color_attr_on(COLOR_PAIR(2));
	} else {
		color_attr_on(A_REVERSE);
	}
	char screenLine[CMDLINE_LNG];
	snprintf(screenLine, getmaxx(stdscr),
			"%s%s" SPACE_PADDING SPACE_PADDING SPACE_PADDING,
			(terminal_has_colors()?" ":">"), text);
	mvprintw(y, 0, "%s", screenLine);
	if(hstr->hicolor) {
		color_attr_on(COLOR_PAIR(1));
	} else {
		color_attr_off(A_REVERSE);
	}
	color_attr_off(A_BOLD);
}

char *hstr_print_selection(unsigned maxHistoryItems, char *pattern, Hstr *hstr)
{
	char *result=NULL;
	unsigned selectionCount=hstr_make_selection(pattern, hstr->history, maxHistoryItems, hstr);
	if (selectionCount > 0) {
		result=hstr->selection[0];
	}

	int height=get_max_history_items();
	int width=getmaxx(stdscr);
	unsigned i;
	int y=Y_OFFSET_ITEMS;

	move(Y_OFFSET_ITEMS, 0);
	clrtobot();

	int start, end;
	char screenLine[CMDLINE_LNG];
	for (i = 0; i<height; ++i) {
		if(i<hstr->selectionSize) {
			// TODO make this function
			if(pattern && strlen(pattern)) {
				if(hstr->historyMatch==HH_MATCH_REGEXP) {
					start=hstr->selectionRegexpMatch[i].rm_so;
					end=hstr->selectionRegexpMatch[i].rm_eo-start;
					strncpy(screenLine,
							hstr->selection[i]+start,
							end);
					screenLine[end]=0;
				} else {
					strcpy(screenLine, pattern);
				}
				print_selection_row(hstr->selection[i], y++, width, screenLine);
			} else {
				print_selection_row(hstr->selection[i], y++, width, pattern);
			}
		} else {
			mvprintw(y++, 0, " ");
		}
	}
	refresh();

	return result;
}

void highlight_selection(int selectionCursorPosition, int previousSelectionCursorPosition, char *pattern, Hstr *hstr)
{
	if(previousSelectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
		// TODO make this function
		char buffer[CMDLINE_LNG];
		if(pattern && strlen(pattern) && hstr->historyMatch==HH_MATCH_REGEXP) {
			int start=hstr->selectionRegexpMatch[previousSelectionCursorPosition].rm_so;
			int end=hstr->selectionRegexpMatch[previousSelectionCursorPosition].rm_eo-start;
			strncpy(buffer,
					hstr->selection[previousSelectionCursorPosition]+start,
					end);
			buffer[end]=0;
		} else {
			strcpy(buffer, pattern);
		}
		print_selection_row(
				hstr->selection[previousSelectionCursorPosition],
				Y_OFFSET_ITEMS+previousSelectionCursorPosition,
				getmaxx(stdscr),
				buffer);
	}
	if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
		hstr_print_highlighted_selection_row(
				hstr->selection[selectionCursorPosition],
				Y_OFFSET_ITEMS+selectionCursorPosition,
				getmaxx(stdscr),
				hstr);
	}
}

void selection_remove(char *cmd, Hstr *hstr)
{
	if(hstr->historyView==HH_VIEW_FAVORITES) {
		favorites_remove(hstr->favorites, cmd);
	} else {
		if(hstr->history->count) {
			int i, w;
			for(i=0, w=0; i<hstr->history->count; i++) {
				if(strcmp(hstr->history->items[i], cmd)) {
					hstr->history->items[w]=hstr->history->items[i];
					w++;
				}
			}
			hstr->history->count=w;
		}
	}
}

void hstr_on_exit(Hstr *hstr)
{
	history_mgmt_flush();
	free_prioritized_history();
}

void signal_callback_handler_ctrl_c(int signum)
{
	if(signum==SIGINT) {
		hstr_curses_stop();
		hstr_on_exit(hstr);
		exit(signum);
	}
}

int seletion_source_remove(char *delete, Hstr *hstr)
{
	if(hstr->historyView!=HH_VIEW_FAVORITES) {
		return history_mgmt_remove(delete);
	} else {
		return favorites_remove(hstr->favorites, delete);
	}
}

void hstr_next_view(Hstr *hstr)
{
	hstr->historyView++;
	hstr->historyView=hstr->historyView%3;
}

void stdout_history_and_return(Hstr *hstr) {
	unsigned selectionCount=hstr_make_selection(hstr->cmdline, hstr->history, hstr->history->rawCount, hstr);
	if (selectionCount > 0) {
		int i;
		for(i=0; i<selectionCount; i++) {
			printf("%s\n",hstr->selection[i]);
		}
	}
}

void loop_to_select(Hstr *hstr)
{
	signal(SIGINT, signal_callback_handler_ctrl_c);

	hstr_curses_start();
	// TODO move the code below to hstr_curses
	color_init_pair(HH_COLOR_NORMAL, -1, -1);
	if(hstr->hicolor) {
		color_init_pair(HH_COLOR_HIROW, COLOR_WHITE, COLOR_GREEN);
		color_init_pair(HH_COLOR_PROMPT, COLOR_BLUE, -1);
		color_init_pair(HH_COLOR_DELETE, COLOR_WHITE, COLOR_RED);
	}

	color_attr_on(COLOR_PAIR(HH_COLOR_NORMAL));
	print_help_label();
	print_history_label(hstr);
	// TODO why do I print non-filtered selection when on command line there is a pattern?
	hstr_print_selection(get_max_history_items(), NULL, hstr);
	color_attr_off(COLOR_PAIR(HH_COLOR_NORMAL));

	bool done=FALSE, skip=TRUE, executeResult=FALSE, lowercase=TRUE, printDefaultLabel=FALSE;
	int basex=print_prompt(hstr);
	int x=basex, y=0, c, cursorX=0, cursorY=0, maxHistoryItems, deleteOccurences;
	int width=getmaxx(stdscr);
	int selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
	int previousSelectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
	char *result="", *msg, *delete;
	char pattern[SELECTION_PREFIX_MAX_LNG];
	pattern[0]=0;
	// TODO this is too late! > don't render twice
	// TODO overflow
	strcpy(pattern, hstr->cmdline);
	while (!done) {
		maxHistoryItems=get_max_history_items();

		if(!skip) {
			c = wgetch(stdscr);
		} else {
			if(strlen(pattern)) {
				color_attr_on(A_BOLD);
				mvprintw(y, basex, "%s", pattern);
				color_attr_off(A_BOLD);
				cursorX=getcurx(stdscr);
				cursorY=getcury(stdscr);
				result=hstr_print_selection(maxHistoryItems, pattern, hstr);
				move(cursorY, cursorX);
			}
			skip=FALSE;
			continue;
		}

		if(printDefaultLabel) {
			print_help_label();
			printDefaultLabel=FALSE;
		}

		switch (c) {
		case KEY_DC: // DEL
			if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
				delete=hstr->selection[selectionCursorPosition];
				msg=malloc(strlen(delete)+1);
				strcpy(msg,delete);
				selection_remove(delete, hstr);
				deleteOccurences=seletion_source_remove(delete, hstr);
				result=hstr_print_selection(maxHistoryItems, pattern, hstr);
				print_cmd_deleted_label(msg, deleteOccurences, hstr);
				move(y, basex+strlen(pattern));
				printDefaultLabel=TRUE;
			}
			print_history_label(hstr);
			break;
		case K_CTRL_E:
			hstr->historyMatch++;
			hstr->historyMatch=hstr->historyMatch%2;
			// TODO make this a function
			result=hstr_print_selection(maxHistoryItems, pattern, hstr);
			print_history_label(hstr);
			selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
			if(strlen(pattern)<(width-basex-1)) {
				print_prefix(pattern, y, basex);
				cursorX=getcurx(stdscr);
				cursorY=getcury(stdscr);
			}
			break;
		case K_CTRL_T:
			hstr->caseSensitive=!hstr->caseSensitive;
			hstr->regexp.caseSensitive=hstr->caseSensitive;
			result=hstr_print_selection(maxHistoryItems, pattern, hstr);
			print_history_label(hstr);
			selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
			if(strlen(pattern)<(width-basex-1)) {
				print_prefix(pattern, y, basex);
				cursorX=getcurx(stdscr);
				cursorY=getcury(stdscr);
			}
			break;
		case K_CTRL_SLASH:
			hstr_next_view(hstr);
			result=hstr_print_selection(maxHistoryItems, pattern, hstr);
			print_history_label(hstr);
			// TODO function
			selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
			if(strlen(pattern)<(width-basex-1)) {
				print_prefix(pattern, y, basex);
				cursorX=getcurx(stdscr);
				cursorY=getcury(stdscr);
			}
			break;
		case K_CTRL_F:
			if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
				result=hstr->selection[selectionCursorPosition];

				if(hstr->historyView==HH_VIEW_FAVORITES) {
					favorites_choose(hstr->favorites, result);
				} else {
					favorites_add(hstr->favorites, result);
					print_cmd_added_favorite_label(result, hstr);
					printDefaultLabel=TRUE;
				}
				result=hstr_print_selection(maxHistoryItems, pattern, hstr);
				selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
				// TODO code review
				if(strlen(pattern)<(width-basex-1)) {
					print_prefix(pattern, y, basex);
					cursorX=getcurx(stdscr);
					cursorY=getcury(stdscr);
				}
			}
			break;
		case KEY_RESIZE:
			print_history_label(hstr);
			result=hstr_print_selection(maxHistoryItems, pattern, hstr);
			print_history_label(hstr);
			selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
			move(y, basex+strlen(pattern));
			break;
		case K_CTRL_U:
		case K_CTRL_W: // TODO supposed to delete just one word backward
			pattern[0]=0;
			print_prefix(pattern, y, basex);
			break;
		case K_CTRL_L:
			toggle_case(pattern, lowercase);
			lowercase=!lowercase;
			print_prefix(pattern, y, basex);
			selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
			break;
		case K_CTRL_H:
		case K_BACKSPACE:
		case KEY_BACKSPACE:
			if(strlen(pattern)>0) {
				pattern[strlen(pattern)-1]=0;
				x--;
				print_prefix(pattern, y, basex);
			}

			// TODO why I make selection if it's done in print_selection?
			if(strlen(pattern)>0) {
				hstr_make_selection(pattern, hstr->history, maxHistoryItems, hstr);
			} else {
				hstr_make_selection(NULL, hstr->history, maxHistoryItems, hstr);
			}
			result=hstr_print_selection(maxHistoryItems, pattern, hstr);

			move(y, basex+strlen(pattern));
			break;
		case KEY_UP:
			previousSelectionCursorPosition=selectionCursorPosition;
			if(selectionCursorPosition>0) {
				selectionCursorPosition--;
			} else {
				selectionCursorPosition=hstr->selectionSize-1;
			}
			highlight_selection(selectionCursorPosition, previousSelectionCursorPosition, pattern, hstr);
			move(y, basex+strlen(pattern));
			break;
		case K_CTRL_R:
		case KEY_DOWN:
			if(selectionCursorPosition==SELECTION_CURSOR_IN_PROMPT) {
				selectionCursorPosition=previousSelectionCursorPosition=0;
			} else {
				previousSelectionCursorPosition=selectionCursorPosition;
				if((selectionCursorPosition+1)<hstr->selectionSize) {
					selectionCursorPosition++;
				} else {
					selectionCursorPosition=0;
				}
			}
			if(hstr->selectionSize) {
				highlight_selection(selectionCursorPosition, previousSelectionCursorPosition, pattern, hstr);
			}
			move(y, basex+strlen(pattern));
			break;
		case K_ENTER:
		case KEY_ENTER:
			executeResult=TRUE;
		case K_TAB:
		case KEY_LEFT:
		case KEY_RIGHT:
			if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
				result=hstr->selection[selectionCursorPosition];
				if(hstr->historyView==HH_VIEW_FAVORITES) {
					favorites_choose(hstr->favorites,result);
				}
			} else {
				result=(pattern==NULL?"":pattern);
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
					print_prefix(pattern, y, basex);
					cursorX=getcurx(stdscr);
					cursorY=getcury(stdscr);
				}

				result = hstr_print_selection(maxHistoryItems, pattern, hstr);
				move(cursorY, cursorX);
				refresh();
			}
			break;
		}
	}
	hstr_curses_stop();

	if(result!=NULL) {
		fill_terminal_input(result, TRUE);
		if(executeResult) {
			fill_terminal_input("\n", FALSE);
		}
	}
}

// TODO protection from line overflow (snprinf)
void hstr_assemble_cmdline_pattern(int argc, char* argv[], Hstr* hstr, int startIndex)
{
	if(argc>0) {
		int i;
		for(i=startIndex; i<argc; i++) {
			if((strlen(hstr->cmdline)+strlen(argv[i])*2)>CMDLINE_LNG) break;
			if(strstr(argv[i], " ")) {
				strcat(hstr->cmdline, "\"");
			}
			strcat(hstr->cmdline, argv[i]);
			if(strstr(argv[i], " ")) {
				strcat(hstr->cmdline, "\"");
			}
			if((i+1<argc)) {
				strcat(hstr->cmdline, " ");
			}
		}
	}
}

// TODO make favorites loading lazy (load on the first opening of favorites view)
void hstr_init_favorites(Hstr *hstr)
{
	hstr->favorites=malloc(sizeof(FavoriteItems));
	favorites_init(hstr->favorites);
	favorites_get(hstr->favorites);
}

void hstr_main(Hstr *hstr)
{
	hstr->history=get_prioritized_history();
	if(hstr->history) {
		history_mgmt_open();
		if(hstr->interactive) {
			loop_to_select(hstr);
		} else {
			stdout_history_and_return(hstr);
		}
		hstr_on_exit(hstr);
	} else {
		printf("No history - nothing to suggest...\n");
	}
}

void hstr_getopt(int argc, char **argv, Hstr *hstr)
{
	int option_index = 0;
	int option = getopt_long(argc, argv, "fVhns", long_options, &option_index);
	if(option != -1) {
		switch(option) {
		case 'f':
			hstr->historyView=HH_VIEW_FAVORITES;
			break;
		case 'n':
			hstr->interactive=false;
			break;
		case 'V':
			printf("%s", VERSION_STRING);
			exit(EXIT_SUCCESS);
		case 'h':
			printf("%s", HELP_STRING);
			exit(EXIT_SUCCESS);
		case 's':
			printf("%s", INSTALL_STRING);
			exit(EXIT_SUCCESS);

		case '?':
		default:
			printf("Unrecognized option: '%s'", HELP_STRING);
			printf("%s", HELP_STRING);
			exit(EXIT_SUCCESS);
		}
	}

	if(optind < argc) {
		hstr_assemble_cmdline_pattern(argc, argv, hstr, optind);
	}
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");

	hstr=malloc(sizeof(Hstr));

	hstr_init(hstr);
	hstr_get_env_configuration(hstr);
	hstr_getopt(argc, argv, hstr);
	hstr_init_favorites(hstr);
	hstr_main(hstr);

	favorites_destroy(hstr->favorites);
	free(hstr);

	return EXIT_SUCCESS;
}
