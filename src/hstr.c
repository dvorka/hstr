/*
 hstr.c     HSTR shell history completion utility

 Copyright (C) 2014-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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

#define _GNU_SOURCE

#include "include/hstr.h"

#define SELECTION_CURSOR_IN_PROMPT -1
#define SELECTION_PREFIX_MAX_LNG 512
#define CMDLINE_LNG 2048
#define HOSTNAME_BUFFER 128

#define PG_JUMP_SIZE 10

#define K_CTRL_A 1
#define K_CTRL_E 5
#define K_CTRL_F 6
#define K_CTRL_G 7
#define K_CTRL_H 8
#define K_CTRL_L 12
#define K_CTRL_J 10
#define K_CTRL_K 11

#define K_CTRL_N 14
#define K_CTRL_P 16

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
#define K_ENTER 13

#define HSTR_THEME_MONO   0
#define HSTR_THEME_COLOR  1<<7
#define HSTR_THEME_LIGHT  1|HSTR_THEME_COLOR
#define HSTR_THEME_DARK   2|HSTR_THEME_COLOR

#define HSTR_COLOR_NORMAL  1
#define HSTR_COLOR_HIROW   2
#define HSTR_COLOR_INFO    2
#define HSTR_COLOR_PROMPT  3
#define HSTR_COLOR_DELETE  4
#define HSTR_COLOR_MATCH   5

#define HSTR_ENV_VAR_CONFIG      "HSTR_CONFIG"
#define HSTR_ENV_VAR_PROMPT      "HSTR_PROMPT"
#define HSTR_ENV_VAR_IS_SUBSHELL "HSTR_IS_SUBSHELL"

#define HSTR_CONFIG_THEME_MONOCHROMATIC     "monochromatic"
#define HSTR_CONFIG_THEME_HICOLOR           "hicolor"
#define HSTR_CONFIG_STATIC_FAVORITES        "static-favorites"
#define HSTR_CONFIG_SKIP_FAVORITES_COMMENTS "skip-favorites-comments"
#define HSTR_CONFIG_FAVORITES               "favorites-view"
#define HSTR_CONFIG_SORTING                 "raw-history-view"
#define HSTR_CONFIG_CASE                    "case-sensitive"
#define HSTR_CONFIG_REGEXP                  "regexp-matching"
#define HSTR_CONFIG_SUBSTRING               "substring-matching"
#define HSTR_CONFIG_KEYWORDS                "keywords-matching"
#define HSTR_CONFIG_NOCONFIRM               "no-confirm"
#define HSTR_CONFIG_VERBOSE_KILL            "verbose-kill"
#define HSTR_CONFIG_PROMPT_BOTTOM           "prompt-bottom"
#define HSTR_CONFIG_HELP_ON_OPPOSITE_SIDE   "help-on-opposite-side"
#define HSTR_CONFIG_HIDE_BASIC_HELP         "hide-basic-help"
#define HSTR_CONFIG_HIDE_HELP               "hide-help"
#define HSTR_CONFIG_BLACKLIST               "blacklist"
#define HSTR_CONFIG_KEEP_PAGE               "keep-page"
#define HSTR_CONFIG_DEBUG                   "debug"
#define HSTR_CONFIG_WARN                    "warning"
#define HSTR_CONFIG_BIG_KEYS_SKIP           "big-keys-skip"
#define HSTR_CONFIG_BIG_KEYS_FLOOR          "big-keys-floor"
#define HSTR_CONFIG_BIG_KEYS_EXIT           "big-keys-exit"
#define HSTR_CONFIG_DUPLICATES              "duplicates"

#define HSTR_DEBUG_LEVEL_NONE  0
#define HSTR_DEBUG_LEVEL_WARN  1
#define HSTR_DEBUG_LEVEL_DEBUG 2

#define HSTR_VIEW_RANKING      0
#define HSTR_VIEW_HISTORY      1
#define HSTR_VIEW_FAVORITES    2

#define HSTR_MATCH_SUBSTRING   0
#define HSTR_MATCH_REGEXP      1
#define HSTR_MATCH_KEYWORDS    2

#define HSTR_NUM_HISTORY_MATCH 3

#define HSTR_CASE_INSENSITIVE  0
#define HSTR_CASE_SENSITIVE    1

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

#ifdef DEBUG_UTF8
#define LOGUTF8(Y,P) mvprintw(Y, 0, "strlen() %zd, mbstowcs() %zd, hstr_strlen() %d",strlen(P),mbstowcs(NULL,P,0),hstr_strlen(P)); clrtoeol()
#else
#define LOGUTF8(Y,P)
#endif

#ifdef DEBUG_SELECTION
#define LOGSELECTION(Y,SCREEN,MODEL) mvprintw(Y, 0, "Selection: screen %3d, model %3d", SCREEN, MODEL); clrtoeol()
#else
#define LOGSELECTION(Y,SCREEN,MODEL)
#endif

// major.minor.revision
static const char* VERSION_STRING=
        "hstr version \"2.2.0\" (2019-12-21T08:47:00)"
        "\n";

static const char* HSTR_VIEW_LABELS[]={
        "ranking",
        "history",
        "favorites"
};

static const char* HSTR_MATCH_LABELS[]={
        "exact",
        "regexp",
        "keywords"
};

static const char* HSTR_CASE_LABELS[]={
        "insensitive",
        "sensitive"
};

static const char* INSTALL_BASH_STRING=
        "\n# HSTR configuration - add this to ~/.bashrc"
        "\nalias hh=hstr                    # hh to be alias for hstr"
        "\nexport HSTR_CONFIG=hicolor       # get more colors"
        "\nshopt -s histappend              # append new history items to .bash_history"
        "\nexport HISTCONTROL=ignorespace   # leading space hides commands from history"
        "\nexport HISTFILESIZE=10000        # increase history file size (default is 500)"
        "\nexport HISTSIZE=${HISTFILESIZE}  # increase history size (default is 500)"
        // PROMPT_COMMAND considerations:
        //   history -a ... append NEW entries from memory to .bash_history (i.e. flush to file where HSTR reads commands)
        //   history -n ... append NEW entries from .bash_history to memory i.e. NOT entire history reload
        //   history -c ... CLEAR in memory history (keeps .bash_history content)
        //   history -r ... append ALL entries from .bash_history to memory (useful to sync DIFFERENT bash sessions)
        // Conclusion:
        //   -a -n ... Fastest and almost-consistent option i.e. there is efficiency/integrity trade-off.
        //             It works correctly if memory entries are not deleted by HSTR. It doesn't synchronize history
        //             across different bash sessions.
        //   -c -r ... Forces entire .bash_history to be reloaded (handles history deletes, synchronizes different bash sessions)
        "\n# ensure synchronization between bash memory and history file"
        "\nexport PROMPT_COMMAND=\"history -a; history -n; ${PROMPT_COMMAND}\""
        "\n# if this is interactive shell, then bind hstr to Ctrl-r (for Vi mode check doc)"
#if defined(__MS_WSL__)
        // IMPROVE commands are NOT executed on return under win10 > consider hstr_utils changes
        // Script hints:
        //  {...} is inline group ~ lambda function whose vars are visible to the other commands
        //   V=$(c) executes commands and stores it to var V
        "\nfunction hstrwsl {"
        "\n  offset=${READLINE_POINT}"
        "\n  READLINE_POINT=0"
        "\n  { READLINE_LINE=$(</dev/tty hstr ${READLINE_LINE:0:offset} 2>&1 1>&$hstrout); } {hstrout}>&1"
        "\n  READLINE_POINT=${#READLINE_LINE}"
        "\n}"
        "\nif [[ $- =~ .*i.* ]]; then bind -x '\"\\C-r\": \"hstrwsl\"'; fi"
#elif defined(__CYGWIN__)
        "\nfunction hstrcygwin {"
        "\n  offset=${READLINE_POINT}"
        "\n  READLINE_POINT=0"
        "\n  { READLINE_LINE=$(</dev/tty hstr ${READLINE_LINE:0:offset} 2>&1 1>&$hstrout); } {hstrout}>&1"
        "\n  READLINE_POINT=${#READLINE_LINE}"
        "\n}"
        "\nif [[ $- =~ .*i.* ]]; then bind -x '\"\\C-r\": \"hstrcygwin\"'; fi"
#else
        "\nif [[ $- =~ .*i.* ]]; then bind '\"\\C-r\": \"\\C-a hstr -- \\C-j\"'; fi"
        "\n# if this is interactive shell, then bind 'kill last command' to Ctrl-x k"
        "\nif [[ $- =~ .*i.* ]]; then bind '\"\\C-xk\": \"\\C-a hstr -k \\C-j\"'; fi"
#endif
        "\n\n";

// zsh doc: http://zsh.sourceforge.net/Guide/zshguide.html
static const char* INSTALL_ZSH_STRING=
        "\n# HSTR configuration - add this to ~/.zshrc"
        "\nalias hh=hstr                    # hh to be alias for hstr"
        "\nsetopt histignorespace           # skip cmds w/ leading space from history"
        // HISTFILE should not be needed - HSTR must work on blank environment as well
        // "\nexport HISTFILE=~/.zsh_history   # ensure history file visibility"
        "\nexport HSTR_CONFIG=hicolor       # get more colors"
#if defined(__MS_WSL__)
        // TODO binding to be rewritten for zsh@WSL as it's done for bash - hstr_winwsl() like function to be implemented to make it work on WSL

        "\n# Function and binding below is bash script that makes command completion work under WSL."
        "\n# If you can rewrite the function and binding from bash to zsh please send it to martin.dvorak@mindforger.com"
        "\n# so that I can share it with other users."
        "\n#function hstr_winwsl {"
        "\n#  offset=${READLINE_POINT}"
        "\n#  READLINE_POINT=0"
        "\n#  { READLINE_LINE=$(</dev/tty hstr ${READLINE_LINE:0:offset} 2>&1 1>&$hstrout); } {hstrout}>&1"
        "\n#  READLINE_POINT=${#READLINE_LINE}"
        "\n#}"
        "\n#bindkey -s \"\\C-r\" \"\\eqhstr_winwsl\\n\""
        "\n"
        "\nbindkey -s \"\\C-r\" \"\\eqhstr\\n\"     # bind hstr to Ctrl-r (for Vi mode check doc)"
#elif defined(__CYGWIN__)
        // TODO binding to be rewritten for zsh@Cygwin as it's done for bash - hstr_cygwin() like function to be implemented to make it work under Cygwin

        "\n# Function and binding below is bash script that makes command completion work under Cygwin."
        "\n# If you can rewrite the function and binding from bash to zsh please send it to martin.dvorak@mindforger.com"
        "\n# so that I can share it with other users."
        "\n#function hstr_cygwin {"
        "\n#  offset=${READLINE_POINT}"
        "\n#  READLINE_POINT=0"
        "\n#  { READLINE_LINE=$(</dev/tty hstr ${READLINE_LINE:0:offset} 2>&1 1>&$hstrout); } {hstrout}>&1"
        "\n#  READLINE_POINT=${#READLINE_LINE}"
        "\n#}"
        "\n#bindkey -s \"\\C-r\" \"\\eqhstr_cygwin\\n\""
        "\n"
        "\nbindkey -s \"\\C-r\" \"\\eqhstr\\n\"     # bind hstr to Ctrl-r (for Vi mode check doc)"
#else
        "\nbindkey -s \"\\C-r\" \"\\C-a hstr -- \\C-j\"     # bind hstr to Ctrl-r (for Vi mode check doc)"
#endif
        // TODO try variant with args/pars separation
        //"\nbindkey -s \"\\C-r\" \"\\eqhstr --\\n\"     # bind hstr to Ctrl-r (for Vi mode check doc)"
        // alternate binding options in zsh:
        //   bindkey -s '^R' '^Ahstr ^M'
        //   bindkey -s "\C-r" "\C-a hstr -- \C-j"
        "\n\n";

static const char* HELP_STRING=
        "Usage: hstr [option] [arg1] [arg2]..."
        "\nShell history suggest box:"
        "\n"
        "\n  --favorites              -f ... show favorites view"
        "\n  --kill-last-command      -k ... delete last command in history"
        "\n  --non-interactive        -n ... print filtered history and exit"
        "\n  --show-configuration     -s ... show configuration to be added to ~/.bashrc"
        "\n  --show-zsh-configuration -z ... show zsh configuration to be added to ~/.zshrc"
        "\n  --show-blacklist         -b ... show commands to skip on history indexation"
        "\n  --version                -V ... show version details"
        "\n  --help                   -h ... help"
        "\n"
        "\nReport bugs to martin.dvorak@mindforger.com"
        "\nHome page: https://github.com/dvorka/hstr"
        "\n";

// TODO help screen - curses window (tig)
static const char* LABEL_HELP=
        "Type to filter, UP/DOWN move, RET/TAB select, DEL remove, C-f add favorite, C-g cancel";

#define GETOPT_NO_ARGUMENT           0
#define GETOPT_REQUIRED_ARGUMENT     1
#define GETOPT_OPTIONAL_ARGUMENT     2

static const struct option long_options[] = {
        {"favorites",              GETOPT_NO_ARGUMENT, NULL, 'f'},
        {"kill-last-command",      GETOPT_NO_ARGUMENT, NULL, 'k'},
        {"version",                GETOPT_NO_ARGUMENT, NULL, 'V'},
        {"help",                   GETOPT_NO_ARGUMENT, NULL, 'h'},
        {"non-interactive",        GETOPT_NO_ARGUMENT, NULL, 'n'},
        {"show-configuration",     GETOPT_NO_ARGUMENT, NULL, 's'},
        {"show-zsh-configuration", GETOPT_NO_ARGUMENT, NULL, 'z'},
        {"show-blacklist",         GETOPT_NO_ARGUMENT, NULL, 'b'},
        {0,                        0,                  NULL,  0 }
};

typedef struct {
    HistoryItems* history;
    FavoriteItems* favorites;
    Blacklist blacklist;
    HstrRegexp regexp;

    char cmdline[CMDLINE_LNG];

    char **selection;
    unsigned selectionSize;
    regmatch_t *selectionRegexpMatch;

    bool interactive;

    int matching;
    int view;
    int caseSensitive;

    unsigned char theme;
    bool noRawHistoryDuplicates;
    bool keepPage; // do NOT clear page w/ selection on HSTR exit
    bool noConfirm; // do NOT ask for confirmation on history entry delete
    bool verboseKill; // write a message on delete of the last command in history
    int bigKeys;
    int debugLevel;
    bool promptBottom;
    bool helpOnOppositeSide;
    bool hideBasicHelp;
    bool hideHistoryHelp;

    int promptY;
    int promptYHelp;
    int promptYHistory;
    int promptYNotification;
    int promptYItemsStart;
    int promptYItemsEnd;
    int promptItems;

    bool noIoctl;
} Hstr;

static Hstr* hstr;

void hstr_init(void)
{
    hstr->history=NULL;
    hstr->favorites=malloc(sizeof(FavoriteItems));
    favorites_init(hstr->favorites);
    blacklist_init(&hstr->blacklist);
    hstr_regexp_init(&hstr->regexp);

    hstr->cmdline[0]=0;

    hstr->selection=NULL;
    hstr->selectionRegexpMatch=NULL;
    hstr->selectionSize=0;

    hstr->interactive=true;

    hstr->matching=HSTR_MATCH_KEYWORDS;
    hstr->view=HSTR_VIEW_RANKING;
    hstr->caseSensitive=HSTR_CASE_INSENSITIVE;

    hstr->theme=HSTR_THEME_MONO;
    hstr->noRawHistoryDuplicates=true;
    hstr->keepPage=false;
    hstr->noConfirm=false;
    hstr->verboseKill=false;
    hstr->bigKeys=RADIX_BIG_KEYS_SKIP;
    hstr->debugLevel=HSTR_DEBUG_LEVEL_NONE;
    hstr->promptBottom=false;
    hstr->helpOnOppositeSide=false;
    hstr->hideBasicHelp=false;
    hstr->hideHistoryHelp=false;

    hstr->promptY
     =hstr->promptYHelp
     =hstr->promptYHistory
     =hstr->promptYNotification
     =hstr->promptYItemsStart
     =hstr->promptYItemsEnd
     =hstr->promptItems
     =0;

    hstr->noIoctl=false;
}

void hstr_destroy(void)
{
    favorites_destroy(hstr->favorites);
    hstr_regexp_destroy(&hstr->regexp);
    // blacklist is allocated by hstr struct
    blacklist_destroy(&hstr->blacklist, false);
    prioritized_history_destroy(hstr->history);
    if(hstr->selection) free(hstr->selection);
    if(hstr->selectionRegexpMatch) free(hstr->selectionRegexpMatch);
    free(hstr);
}

void hstr_exit(int status)
{
    hstr_destroy();
    exit(status);
}

void signal_callback_handler_ctrl_c(int signum)
{
    if(signum==SIGINT) {
        history_mgmt_flush();
        hstr_curses_stop(false);
        hstr_exit(signum);
    }
}

unsigned recalculate_max_history_items(void)
{
    int n = getmaxy(stdscr);
    hstr->promptItems = n-1;
    if(!hstr->hideBasicHelp) {
        hstr->promptItems--;
    }
    if(!hstr->hideHistoryHelp) {
        hstr->promptItems--;
    }
    if(hstr->promptBottom) {
        if(hstr->helpOnOppositeSide) {
            // Layout:
            // - [basic help]
            // - [history help]
            // - items start
            // - ...
            // - items end
            // - prompt
            int top = 0;
            hstr->promptY = n-1;
            if(!hstr->hideBasicHelp) {
                hstr->promptYHelp = top++;
            }
            if(!hstr->hideHistoryHelp) {
                hstr->promptYHistory = top++;
            }
            hstr->promptYItemsStart = top++;
        } else {
            // Layout:
            // - items start
            // - ...
            // - items end
            // - [history help]
            // - [basic help]
            // - prompt
            int bottom = n-1;
            hstr->promptY = bottom--;
            if(!hstr->hideBasicHelp) {
                hstr->promptYHelp = bottom--;
            }
            if(!hstr->hideHistoryHelp) {
                hstr->promptYHistory = bottom--;
            }
            hstr->promptYItemsStart = 0;
        }
    } else {
        if(hstr->helpOnOppositeSide) {
            // Layout:
            // - prompt
            // - items start
            // - ...
            // - items end
            // - [history help]
            // - [basic help]
            int bottom = n-1;
            hstr->promptY = 0;
            if(!hstr->hideBasicHelp) {
                hstr->promptYHelp = bottom--;
            }
            if(!hstr->hideHistoryHelp) {
                hstr->promptYHistory = bottom--;
            }
            hstr->promptYItemsStart = 1;
        } else {
            // Layout:
            // - prompt
            // - [basic help]
            // - [history help]
            // - items start
            // - ...
            // - items end
            int top = 0;
            hstr->promptY = top++;
            if(!hstr->hideBasicHelp) {
                hstr->promptYHelp = top++;
            }
            if(!hstr->hideHistoryHelp) {
                hstr->promptYHistory = top++;
            }
            hstr->promptYItemsStart = top++;
        }
    }
    hstr->promptYItemsEnd = hstr->promptYItemsStart+hstr->promptItems-1;
    if(!hstr->hideBasicHelp) {
        // Use basic help label for notifications.
        hstr->promptYNotification = hstr->promptYHelp;
    }
    else {
        // If basic help is hidden, we need another place to put notifications to.
        if(hstr->hideBasicHelp) {
            if(!hstr->hideHistoryHelp) {
                // Use history help label.
                hstr->promptYHelp = hstr->promptYHistory;
            } else {
                // Use one of the command item lines.
                if((hstr->promptBottom && hstr->helpOnOppositeSide) || (!hstr->promptBottom && !hstr->helpOnOppositeSide)) {
                    hstr->promptYHelp = hstr->promptYItemsStart;
                } else {
                    hstr->promptYHelp = hstr->promptYItemsEnd;
                }
            }
        }
    }
    return hstr->promptItems;
}

void hstr_get_env_configuration()
{
    char *hstr_config=getenv(HSTR_ENV_VAR_CONFIG);
    if(hstr_config && strlen(hstr_config)>0) {
        if(strstr(hstr_config,HSTR_CONFIG_THEME_MONOCHROMATIC)) {
            hstr->theme=HSTR_THEME_MONO;
        } else {
            if(strstr(hstr_config,HSTR_CONFIG_THEME_HICOLOR)) {
                hstr->theme=HSTR_THEME_DARK;
            }
        }
        if(strstr(hstr_config,HSTR_CONFIG_CASE)) {
            hstr->caseSensitive=HSTR_CASE_SENSITIVE;
        }
        if(strstr(hstr_config,HSTR_CONFIG_REGEXP)) {
            hstr->matching=HSTR_MATCH_REGEXP;
        } else {
            if(strstr(hstr_config,HSTR_CONFIG_SUBSTRING)) {
                hstr->matching=HSTR_MATCH_SUBSTRING;
            } else {
                if(strstr(hstr_config, HSTR_CONFIG_KEYWORDS)) {
                    hstr->matching=HSTR_MATCH_KEYWORDS;
                }
            }
        }
        if(strstr(hstr_config,HSTR_CONFIG_SORTING)) {
            hstr->view=HSTR_VIEW_HISTORY;
        } else {
            if(strstr(hstr_config,HSTR_CONFIG_FAVORITES)) {
                hstr->view=HSTR_VIEW_FAVORITES;
            }
        }
        if(strstr(hstr_config,HSTR_CONFIG_BIG_KEYS_EXIT)) {
            hstr->bigKeys=RADIX_BIG_KEYS_EXIT;
        } else {
            if(strstr(hstr_config,HSTR_CONFIG_BIG_KEYS_FLOOR)) {
                hstr->bigKeys=RADIX_BIG_KEYS_FLOOR;
            } else {
                hstr->bigKeys=RADIX_BIG_KEYS_SKIP;
            }
        }
        if(strstr(hstr_config,HSTR_CONFIG_VERBOSE_KILL)) {
            hstr->verboseKill=true;
        }
        if(strstr(hstr_config,HSTR_CONFIG_BLACKLIST)) {
            hstr->blacklist.useFile=true;
        }
        if(strstr(hstr_config,HSTR_CONFIG_KEEP_PAGE)) {
            hstr->keepPage=true;
        }
        if(strstr(hstr_config,HSTR_CONFIG_NOCONFIRM)) {
            hstr->noConfirm=true;
        }
        if(strstr(hstr_config,HSTR_CONFIG_STATIC_FAVORITES)) {
            hstr->favorites->reorderOnChoice=false;
        }
        if(strstr(hstr_config,HSTR_CONFIG_SKIP_FAVORITES_COMMENTS)) {
            hstr->favorites->skipComments=true;
        }

        if(strstr(hstr_config,HSTR_CONFIG_DEBUG)) {
            hstr->debugLevel=HSTR_DEBUG_LEVEL_DEBUG;
        } else {
            if(strstr(hstr_config,HSTR_CONFIG_WARN)) {
                hstr->debugLevel=HSTR_DEBUG_LEVEL_WARN;
            }
        }

        if(strstr(hstr_config,HSTR_CONFIG_DUPLICATES)) {
            hstr->noRawHistoryDuplicates=false;
        }

        if(strstr(hstr_config,HSTR_CONFIG_PROMPT_BOTTOM)) {
            hstr->promptBottom = true;
        } else {
            hstr->promptBottom = false;
        }
        if(strstr(hstr_config,HSTR_CONFIG_HELP_ON_OPPOSITE_SIDE)) {
            hstr->helpOnOppositeSide = true;
        } else {
            hstr->helpOnOppositeSide = false;
        }
        if(strstr(hstr_config,HSTR_CONFIG_HIDE_HELP)) {
            hstr->hideBasicHelp = true;
            hstr->hideHistoryHelp = true;
        } else {
            if(strstr(hstr_config,HSTR_CONFIG_HIDE_BASIC_HELP)) {
                hstr->hideBasicHelp = true;
                hstr->hideHistoryHelp = false;
            } else {
                hstr->hideBasicHelp = false;
                hstr->hideHistoryHelp = false;
            }
        }
        recalculate_max_history_items();
    }
}

unsigned print_prompt(void)
{
    unsigned xoffset = 0, promptLength;

    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_on(COLOR_PAIR(HSTR_COLOR_PROMPT));
        color_attr_on(A_BOLD);
    }

    char *prompt = getenv(HSTR_ENV_VAR_PROMPT);
    if(prompt) {
        mvprintw(hstr->promptY, xoffset, "%s", prompt);
        promptLength=strlen(prompt);
    } else {
        char *user = getenv(ENV_VAR_USER);
        char *hostname=malloc(HOSTNAME_BUFFER);
        user=(user?user:"me");
        get_hostname(HOSTNAME_BUFFER, hostname);
        mvprintw(hstr->promptY, xoffset, "%s@%s$ ", user, hostname);
        promptLength=strlen(user)+1+strlen(hostname)+1+1;
        free(hostname);
    }

    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_off(A_BOLD);
        color_attr_off(COLOR_PAIR(HSTR_COLOR_PROMPT));
    }
    refresh();

    return promptLength;
}

void add_to_selection(char* line, unsigned int* index)
{
    if (hstr->noRawHistoryDuplicates) {
        unsigned i;
        for(i = 0; i < *index; i++) {
            if (strcmp(hstr->selection[i], line) == 0) {
                return;
            }
        }
    }
    hstr->selection[*index]=line;
    (*index)++;
}

void print_help_label(void)
{
    if(hstr->hideBasicHelp)
        return;

    int cursorX=getcurx(stdscr);
    int cursorY=getcury(stdscr);

    char screenLine[CMDLINE_LNG];
    snprintf(screenLine, getmaxx(stdscr), "%s", LABEL_HELP);
    mvprintw(hstr->promptYHelp, 0, "%s", screenLine); clrtoeol();
    refresh();

    move(cursorY, cursorX);
}

void print_confirm_delete(const char* cmd)
{
    char screenLine[CMDLINE_LNG];
    if(hstr->view==HSTR_VIEW_FAVORITES) {
        snprintf(screenLine, getmaxx(stdscr), "Do you want to delete favorites item '%s'? y/n", cmd);
    } else {
        snprintf(screenLine, getmaxx(stdscr), "Do you want to delete all occurrences of '%s'? y/n", cmd);
    }
    // TODO make this function
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_on(COLOR_PAIR(HSTR_COLOR_DELETE));
        color_attr_on(A_BOLD);
    }
    mvprintw(hstr->promptYNotification, 0, "%s", screenLine);
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_off(A_BOLD);
        color_attr_on(COLOR_PAIR(HSTR_COLOR_NORMAL));
    }
    clrtoeol();
    refresh();
}

void print_cmd_deleted_label(const char* cmd, int occurences)
{
    char screenLine[CMDLINE_LNG];
    if(hstr->view==HSTR_VIEW_FAVORITES) {
        snprintf(screenLine, getmaxx(stdscr), "Favorites item '%s' deleted", cmd);
    } else {
        snprintf(screenLine, getmaxx(stdscr), "History item '%s' deleted (%d occurrence%s)", cmd, occurences, (occurences==1?"":"s"));
    }
    // TODO make this function
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_on(COLOR_PAIR(HSTR_COLOR_DELETE));
        color_attr_on(A_BOLD);
    }
    mvprintw(hstr->promptYNotification, 0, "%s", screenLine);
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_off(A_BOLD);
        color_attr_on(COLOR_PAIR(HSTR_COLOR_NORMAL));
    }
    clrtoeol();
    refresh();
}

void print_regexp_error(const char* errorMessage)
{
    char screenLine[CMDLINE_LNG];
    snprintf(screenLine, getmaxx(stdscr), "%s", errorMessage);
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_on(COLOR_PAIR(HSTR_COLOR_DELETE));
        color_attr_on(A_BOLD);
    }
    mvprintw(hstr->promptYNotification, 0, "%s", screenLine);
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_off(A_BOLD);
        color_attr_on(COLOR_PAIR(HSTR_COLOR_NORMAL));
    }
    clrtoeol();
    refresh();
}

void print_cmd_added_favorite_label(const char* cmd)
{
    char screenLine[CMDLINE_LNG];
    snprintf(screenLine, getmaxx(stdscr), "Command '%s' added to favorites (C-/ to show favorites)", cmd);
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_on(COLOR_PAIR(HSTR_COLOR_INFO));
        color_attr_on(A_BOLD);
    }
    mvprintw(hstr->promptYNotification, 0, screenLine);
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_off(A_BOLD);
        color_attr_on(COLOR_PAIR(HSTR_COLOR_NORMAL));
    }
    clrtoeol();
    refresh();
}

void print_history_label(void)
{
    if(hstr->hideHistoryHelp)
        return;

    unsigned width=getmaxx(stdscr);

    char screenLine[CMDLINE_LNG];
#ifdef __APPLE__
    snprintf(screenLine, width, "- HISTORY - view:%s (C-w) - match:%s (C-e) - case:%s (C-t) - %d/%d/%d ",
#else
    snprintf(screenLine, width, "- HISTORY - view:%s (C-/) - match:%s (C-e) - case:%s (C-t) - %d/%d/%d ",
#endif
            HSTR_VIEW_LABELS[hstr->view],
            HSTR_MATCH_LABELS[hstr->matching],
            HSTR_CASE_LABELS[hstr->caseSensitive],
            hstr->history->count,
            hstr->history->rawCount,
            hstr->favorites->count);
    width -= strlen(screenLine);
    unsigned i;
    for(i=0; i<width; i++) {
        strcat(screenLine, "-");
    }
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_on(A_BOLD);
    }
    color_attr_on(A_REVERSE);
    mvprintw(hstr->promptYHistory, 0, "%s", screenLine);
    color_attr_off(A_REVERSE);
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_off(A_BOLD);
    }
    refresh();
}

void print_pattern(char* pattern, int y, int x)
{
    if(pattern) {
        color_attr_on(A_BOLD);
        mvprintw(y, x, "%s", pattern);
        color_attr_off(A_BOLD);
        clrtoeol();
    }
}

// TODO don't realloc if size doesn't change
void hstr_realloc_selection(unsigned size)
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

unsigned hstr_make_selection(char* prefix, HistoryItems* history, unsigned maxSelectionCount)
{
    hstr_realloc_selection(maxSelectionCount);

    unsigned i, selectionCount=0;
    char **source;
    unsigned count;

    switch(hstr->view) {
    case HSTR_VIEW_HISTORY:
        source=history->rawItems;
        count=history->rawCount;
        break;
    case HSTR_VIEW_FAVORITES:
        source=hstr->favorites->items;
        count=hstr->favorites->count;
        break;
    case HSTR_VIEW_RANKING:
    default:
        source=history->items;
        count=history->count;
        break;
    }

    regmatch_t regexpMatch;
    char regexpErrorMessage[CMDLINE_LNG];
    bool regexpCompilationError=false;
    bool keywordsAllMatch;
    char *keywordsSavePtr=NULL;
    char *keywordsToken=NULL;
    char *keywordsParsedLine=NULL;
    char *keywordsPointerToDelete=NULL;
    for(i=0; i<count && selectionCount<maxSelectionCount; i++) {
        if(source[i]) {
            if(!prefix || !strlen(prefix)) {
                add_to_selection(source[i], &selectionCount);
            } else {
                switch(hstr->matching) {
                case HSTR_MATCH_SUBSTRING:
                    switch(hstr->caseSensitive) {
                    case HSTR_CASE_SENSITIVE:
                        if(source[i]==strstr(source[i], prefix)) {
                            add_to_selection(source[i], &selectionCount);
                        }
                        break;
                    case HSTR_CASE_INSENSITIVE:
                        if(source[i]==strcasestr(source[i], prefix)) {
                            add_to_selection(source[i], &selectionCount);
                        }
                        break;
                    }
                    break;
                case HSTR_MATCH_REGEXP:
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
                case HSTR_MATCH_KEYWORDS:
                    keywordsParsedLine = strdup(prefix);
                    keywordsAllMatch = true;
                    keywordsPointerToDelete = keywordsParsedLine;
                    while(true) {
                        keywordsToken = strtok_r(keywordsParsedLine, " ", &keywordsSavePtr);
                        if (keywordsToken == NULL) {
                            break;
                        }
                        keywordsParsedLine = NULL;
                        switch(hstr->caseSensitive) {
                        case HSTR_CASE_SENSITIVE:
                            if(strstr(source[i], keywordsToken) == NULL) {
                                keywordsAllMatch = false;
                            }
                            break;
                        case HSTR_CASE_INSENSITIVE:
                            if(strcasestr(source[i], keywordsToken) == NULL) {
                                keywordsAllMatch = false;
                            }
                            break;
                        }
                    }
                    if(keywordsAllMatch) {
                        add_to_selection(source[i], &selectionCount);
                    }
                    free(keywordsPointerToDelete);
                    break;
                }
            }
        }
    }

    if(prefix && selectionCount<maxSelectionCount) {
        char *substring;
        for(i=0; i<count && selectionCount<maxSelectionCount; i++) {
            switch(hstr->matching) {
            case HSTR_MATCH_SUBSTRING:
                switch(hstr->caseSensitive) {
                case HSTR_CASE_SENSITIVE:
                    substring = strstr(source[i], prefix);
                    if (substring != NULL && substring!=source[i]) {
                        add_to_selection(source[i], &selectionCount);
                    }
                    break;
                case HSTR_CASE_INSENSITIVE:
                    substring = strcasestr(source[i], prefix);
                    if (substring != NULL && substring!=source[i]) {
                        add_to_selection(source[i], &selectionCount);
                    }
                    break;
                }
                break;
            case HSTR_MATCH_REGEXP:
                // all regexps matched previously - user decides whether match ^ or infix
            break;
            case HSTR_MATCH_KEYWORDS:
                // TODO MD consider adding lines that didn't matched all keywords, but some of them
                //         (ordered by number of matched keywords)
            break;
            }
        }
    }

    hstr->selectionSize=selectionCount;
    return selectionCount;
}

void print_selection_row(char* text, int y, int width, char* pattern)
{
    char screenLine[CMDLINE_LNG];
    char buffer[CMDLINE_LNG];
    hstr_strelide(buffer, text, width>2?width-2:0);
    int size = snprintf(screenLine, width, " %s", buffer);
    if(size < 0) screenLine[0]=0;
    mvprintw(y, 0, "%s", screenLine); clrtoeol();

    if(pattern && strlen(pattern)) {
        color_attr_on(A_BOLD);
        if(hstr->theme & HSTR_THEME_COLOR) {
            color_attr_on(COLOR_PAIR(HSTR_COLOR_MATCH));
        }
        char* p=NULL;
        char* pp=NULL;
        char* keywordsSavePtr=NULL;
        char* keywordsToken=NULL;
        char* keywordsParsedLine=NULL;
        char* keywordsPointerToDelete=NULL;

        switch(hstr->matching) {
        case HSTR_MATCH_SUBSTRING:
            switch(hstr->caseSensitive) {
            case HSTR_CASE_INSENSITIVE:
                p=strcasestr(screenLine, pattern);
                if(p) {
                    snprintf(buffer, strlen(pattern)+1, "%s", p);
                    mvprintw(y, p-screenLine, "%s", buffer);
                }
                break;
            case HSTR_CASE_SENSITIVE:
                p=strstr(screenLine, pattern);
                if(p) {
                    mvprintw(y, p-screenLine, "%s", pattern);
                }
                break;
            }
            break;
        case HSTR_MATCH_REGEXP:
            p=strstr(screenLine, pattern);
            if(p) {
                mvprintw(y, p-screenLine, "%s", pattern);
            }
            break;
        case HSTR_MATCH_KEYWORDS:
            keywordsParsedLine = strdup(pattern);
            keywordsPointerToDelete = keywordsParsedLine;
            while (true) {
                keywordsToken = strtok_r(keywordsParsedLine, " ", &keywordsSavePtr);
                keywordsParsedLine = NULL;
                if (keywordsToken == NULL) {
                    break;
                }
                switch(hstr->caseSensitive) {
                case HSTR_CASE_SENSITIVE:
                    p=strstr(screenLine, keywordsToken);
                    if(p) {
                        mvprintw(y, p-screenLine, "%s", keywordsToken);
                    }
                    break;
                case HSTR_CASE_INSENSITIVE:
                    p=strcasestr(screenLine, keywordsToken);
                    if(p) {
                        pp=strdup(p);
                        pp[strlen(keywordsToken)]=0;
                        mvprintw(y, p-screenLine, "%s", pp);
                        free(pp);
                    }
                    break;
                }
            }
            free(keywordsPointerToDelete);

            break;
        }
        if(hstr->theme & HSTR_THEME_COLOR) {
            color_attr_on(COLOR_PAIR(HSTR_COLOR_NORMAL));
        }
        color_attr_off(A_BOLD);
    }
}

void hstr_print_highlighted_selection_row(char* text, int y, int width)
{
    color_attr_on(A_BOLD);
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_on(COLOR_PAIR(HSTR_COLOR_HIROW));
    } else {
        color_attr_on(A_REVERSE);
    }
    char buffer[CMDLINE_LNG];
    hstr_strelide(buffer, text, width>2?width-2:0);
    char screenLine[CMDLINE_LNG];
    snprintf(screenLine, getmaxx(stdscr)+1, "%s%-*.*s ",
            (terminal_has_colors()?" ":">"),
            getmaxx(stdscr)-2, getmaxx(stdscr)-2, buffer);
    mvprintw(y, 0, "%s", screenLine);
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_attr_on(COLOR_PAIR(HSTR_COLOR_NORMAL));
    } else {
        color_attr_off(A_REVERSE);
    }
    color_attr_off(A_BOLD);
}

char* hstr_print_selection(unsigned maxHistoryItems, char* pattern)
{
    char* result=NULL;
    unsigned selectionCount=hstr_make_selection(pattern, hstr->history, maxHistoryItems);
    if (selectionCount > 0) {
        result=hstr->selection[0];
    }

    unsigned height=recalculate_max_history_items();
    unsigned width=getmaxx(stdscr);
    unsigned i;
    int y;

    move(hstr->promptYItemsStart, 0);
    clrtobot();
    bool labelsAreOnBottom = (hstr->promptBottom && !hstr->helpOnOppositeSide) || (!hstr->promptBottom && hstr->helpOnOppositeSide);
    if(labelsAreOnBottom) {
        // TODO: Why is the reprinting here necessary? Please make a comment.
        print_help_label();
        print_history_label();
    }
    if(hstr->promptBottom) {
        // TODO: Why is the reprinting here necessary? Please make a comment.
        print_pattern(pattern, hstr->promptY, print_prompt());
        y=hstr->promptYItemsEnd;
    } else {
        y=hstr->promptYItemsStart;
    }

    int start, count;
    char screenLine[CMDLINE_LNG];
    for(i=0; i<height; ++i) {
        if(i<hstr->selectionSize) {
            // TODO make this function
            if(pattern && strlen(pattern)) {
                if(hstr->matching==HSTR_MATCH_REGEXP) {
                    start=hstr->selectionRegexpMatch[i].rm_so;
                    count=hstr->selectionRegexpMatch[i].rm_eo-start;
                    if(count>CMDLINE_LNG) {
                        count=CMDLINE_LNG-1;
                    }
                    strncpy(screenLine,
                            hstr->selection[i]+start,
                            count);
                    screenLine[count]=0;
                } else {
                    strcpy(screenLine, pattern);
                }
                print_selection_row(hstr->selection[i], y, width, screenLine);
            } else {
                print_selection_row(hstr->selection[i], y, width, pattern);
            }
        } else {
            mvprintw(y, 0, " ");
        }

        if(hstr->promptBottom) {
            y--;
        } else {
            y++;
        }
    }
    refresh();

    return result;
}

void highlight_selection(int selectionCursorPosition, int previousSelectionCursorPosition, char* pattern)
{
    if(previousSelectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
        // TODO make this function
        char buffer[CMDLINE_LNG];
        if(pattern && strlen(pattern) && hstr->matching==HSTR_MATCH_REGEXP) {
            int start=hstr->selectionRegexpMatch[previousSelectionCursorPosition].rm_so;
            int end=hstr->selectionRegexpMatch[previousSelectionCursorPosition].rm_eo-start;
            end = MIN(end,getmaxx(stdscr));
            strncpy(buffer,
                    hstr->selection[previousSelectionCursorPosition]+start,
                    end);
            buffer[end]=0;
        } else {
            strcpy(buffer, pattern);
        }

        int text, y;
        if(hstr->promptBottom) {
            text=hstr->promptItems-previousSelectionCursorPosition-1;
            y=hstr->promptYItemsStart+previousSelectionCursorPosition;
        } else {
            text=previousSelectionCursorPosition;
            y=hstr->promptYItemsStart+previousSelectionCursorPosition;
        }
        print_selection_row(
                hstr->selection[text],
                y,
                getmaxx(stdscr),
                buffer);
    }
    if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
        int text, y;
        if(hstr->promptBottom) {
            text=hstr->promptItems-selectionCursorPosition-1;
            y=hstr->promptYItemsStart+selectionCursorPosition;
        } else {
            text=selectionCursorPosition;
            y=hstr->promptYItemsStart+selectionCursorPosition;
        }
        hstr_print_highlighted_selection_row(hstr->selection[text], y, getmaxx(stdscr));
    }
}

int remove_from_history_model(char* almostDead)
{
    if(hstr->view==HSTR_VIEW_FAVORITES) {
        return (int)favorites_remove(hstr->favorites, almostDead);
    } else {
        // raw & ranked history is pruned first as its items point to system history lines
        int systemOccurences=0, rawOccurences=history_mgmt_remove_from_raw(almostDead, hstr->history);
        history_mgmt_remove_from_ranked(almostDead, hstr->history);
        if(rawOccurences) {
            systemOccurences=history_mgmt_remove_from_system_history(almostDead);
        }
        if(systemOccurences!=rawOccurences && hstr->debugLevel>HSTR_DEBUG_LEVEL_NONE) {
            fprintf(stderr, "WARNING: system and raw items deletion mismatch %d / %d\n", systemOccurences, rawOccurences);
        }
        return systemOccurences;
    }
}

void hstr_next_view(void)
{
    hstr->view++;
    hstr->view=hstr->view%3;
}

void stdout_history_and_return(void)
{
    unsigned selectionCount=hstr_make_selection(hstr->cmdline, hstr->history, hstr->history->rawCount);
    if (selectionCount > 0) {
        unsigned i;
        for(i=0; i<selectionCount; i++) {
            printf("%s\n",hstr->selection[i]);
        }
    }
}

// IMPROVE hstr doesn't have to be passed as parameter - it's global static
char* getResultFromSelection(int selectionCursorPosition, Hstr* hstr, char* result) {
    if (hstr->promptBottom) {
        result=hstr->selection[hstr->promptItems-selectionCursorPosition-1];
    } else {
        result=hstr->selection[selectionCursorPosition];
    }
    return result;
}

void hide_notification(void)
{
    if(!hstr->hideBasicHelp) {
        print_help_label();
    } else {
        if(!hstr->hideHistoryHelp) {
            print_history_label();
        } else {
            // TODO: If possible, we should rerender the command list here,
            //  because one of the items was used to print the notification.
        }
    }
}

void loop_to_select(void)
{
    signal(SIGINT, signal_callback_handler_ctrl_c);

    bool isSubshellHint=FALSE;
    char* isSubshellHintText = getenv(HSTR_ENV_VAR_IS_SUBSHELL);
    if(isSubshellHintText && strlen(isSubshellHintText)>0) {
        isSubshellHint=TRUE;
    }

    hstr_curses_start();
    // TODO move the code below to hstr_curses
    color_init_pair(HSTR_COLOR_NORMAL, -1, -1);
    if(hstr->theme & HSTR_THEME_COLOR) {
        color_init_pair(HSTR_COLOR_HIROW, COLOR_WHITE, COLOR_GREEN);
        color_init_pair(HSTR_COLOR_PROMPT, COLOR_BLUE, -1);
        color_init_pair(HSTR_COLOR_DELETE, COLOR_WHITE, COLOR_RED);
        color_init_pair(HSTR_COLOR_MATCH, COLOR_RED, -1);
    }

    color_attr_on(COLOR_PAIR(HSTR_COLOR_NORMAL));
    // TODO why do I print non-filtered selection when on command line there is a pattern?
    hstr_print_selection(recalculate_max_history_items(), NULL);
    color_attr_off(COLOR_PAIR(HSTR_COLOR_NORMAL));
    bool labelsAreOnBottom = (hstr->promptBottom && !hstr->helpOnOppositeSide) || (!hstr->promptBottom && hstr->helpOnOppositeSide);
    if(!labelsAreOnBottom) {
        // TODO: Why is the reprinting here necessary? Please make a comment.
        print_help_label();
        print_history_label();
    }

    bool done=FALSE, skip=TRUE, executeResult=FALSE, lowercase=TRUE;
    bool hideNotificationOnNextTick=TRUE, fixCommand=FALSE, editCommand=FALSE;
    unsigned basex=print_prompt();
    int x=basex, c, cc, cursorX=0, cursorY=0, maxHistoryItems, deletedOccurences;
    int width=getmaxx(stdscr);
    int selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
    int previousSelectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
    char *result="", *msg, *almostDead;
    char pattern[SELECTION_PREFIX_MAX_LNG];
    pattern[0]=0;
    // TODO this is too late! > don't render twice
    // TODO overflow
    strcpy(pattern, hstr->cmdline);

    while (!done) {
        maxHistoryItems=recalculate_max_history_items();

        if(!skip) {
            c = wgetch(stdscr);
        } else {
            if(strlen(pattern)) {
                color_attr_on(A_BOLD);
                mvprintw(hstr->promptY, basex, "%s", pattern);
                color_attr_off(A_BOLD);
                cursorX=getcurx(stdscr);
                cursorY=getcury(stdscr);
                result=hstr_print_selection(maxHistoryItems, pattern);
                move(cursorY, cursorX);
            }
            skip=FALSE;
            continue;
        }

        if(hideNotificationOnNextTick) {
            hide_notification();
            hideNotificationOnNextTick=FALSE;
        }

        if(c == K_CTRL_R) {
            c = (hstr->promptBottom ? K_CTRL_P : K_CTRL_N);
        }

        switch (c) {
        case KEY_HOME:
            // avoids printing of wild chars in search prompt
            break;
        case KEY_END:
            // avoids printing of wild chars in search prompt
            break;
        case KEY_DC: // DEL
            if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
                almostDead=getResultFromSelection(selectionCursorPosition, hstr, result);
                msg=malloc(strlen(almostDead)+1);
                strcpy(msg, almostDead);

                if(!hstr->noConfirm) {
                    print_confirm_delete(msg);
                    cc = wgetch(stdscr);
                }
                if(hstr->noConfirm || cc == 'y') {
                    deletedOccurences=remove_from_history_model(msg);
                    result=hstr_print_selection(maxHistoryItems, pattern);
                    print_cmd_deleted_label(msg, deletedOccurences);
                } else {
                    hide_notification();
                }
                free(msg);
                move(hstr->promptY, basex+strlen(pattern));
                hideNotificationOnNextTick=TRUE;
                print_history_label();  // TODO: Why is this necessary? Add comment!

                if(hstr->selectionSize == 0) {
                    // just update the cursor, there are no elements to select
                    move(hstr->promptY, basex+strlen(pattern));
                    break;
                }

                if(hstr->promptBottom) {
                    if(selectionCursorPosition < (int)(hstr->promptItems-hstr->selectionSize)) {
                        selectionCursorPosition=hstr->promptItems-hstr->selectionSize;
                    }
                } else {
                    if(selectionCursorPosition >= (int)hstr->selectionSize) {
                        selectionCursorPosition = hstr->selectionSize-1;
                    }
                }
                highlight_selection(selectionCursorPosition, SELECTION_CURSOR_IN_PROMPT, pattern);
                move(hstr->promptY, basex+strlen(pattern));
            }
            break;
        case K_CTRL_E:
            hstr->matching++;
            hstr->matching=hstr->matching%HSTR_NUM_HISTORY_MATCH;
            // TODO make this a function
            result=hstr_print_selection(maxHistoryItems, pattern);
            print_history_label();
            selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
            if(strlen(pattern)<(width-basex-1)) {
                print_pattern(pattern, hstr->promptY, basex);
                cursorX=getcurx(stdscr);
                cursorY=getcury(stdscr);
            }
            break;
        case K_CTRL_T:
            hstr->caseSensitive=!hstr->caseSensitive;
            hstr->regexp.caseSensitive=hstr->caseSensitive;
            result=hstr_print_selection(maxHistoryItems, pattern);
            print_history_label();
            selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
            if(strlen(pattern)<(width-basex-1)) {
                print_pattern(pattern, hstr->promptY, basex);
                cursorX=getcurx(stdscr);
                cursorY=getcury(stdscr);
            }
            break;
#ifdef __APPLE__
        // reserved for view rotation on macOS
        case K_CTRL_W:
#endif
        case K_CTRL_SLASH:
            hstr_next_view();
            result=hstr_print_selection(maxHistoryItems, pattern);
            print_history_label();
            // TODO function
            selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
            if(strlen(pattern)<(width-basex-1)) {
                print_pattern(pattern, hstr->promptY, basex);
                cursorX=getcurx(stdscr);
                cursorY=getcury(stdscr);
            }
            break;
        case K_CTRL_F:
            if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
                result=getResultFromSelection(selectionCursorPosition, hstr, result);
                if(hstr->view==HSTR_VIEW_FAVORITES) {
                    favorites_choose(hstr->favorites, result);
                } else {
                    favorites_add(hstr->favorites, result);
                }
                hstr_print_selection(maxHistoryItems, pattern);
                selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
                if(hstr->view!=HSTR_VIEW_FAVORITES) {
                    print_cmd_added_favorite_label(result);
                    hideNotificationOnNextTick=TRUE;
                }
                // TODO code review
                if(strlen(pattern)<(width-basex-1)) {
                    print_pattern(pattern, hstr->promptY, basex);
                    cursorX=getcurx(stdscr);
                    cursorY=getcury(stdscr);
                }
            }
            break;
        case KEY_RESIZE:
            print_history_label();
            maxHistoryItems=recalculate_max_history_items();
            result=hstr_print_selection(maxHistoryItems, pattern);
            print_history_label();
            selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
            move(hstr->promptY, basex+strlen(pattern));
            break;
#ifndef __APPLE__
        case K_CTRL_W: // TODO supposed to delete just one word backward
#endif
        case K_CTRL_U:
            pattern[0]=0;
            print_pattern(pattern, hstr->promptY, basex);
            break;
        case K_CTRL_L:
            toggle_case(pattern, lowercase);
            lowercase=!lowercase;
            print_pattern(pattern, hstr->promptY, basex);
            selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;
            break;
        case K_CTRL_H:
        case K_BACKSPACE:
        case KEY_BACKSPACE:
            if(hstr_strlen(pattern)>0) {
                hstr_chop(pattern);
                x--;
                print_pattern(pattern, hstr->promptY, basex);
            }

            // TODO why I make selection if it's done in print_selection?
            if(strlen(pattern)>0) {
                hstr_make_selection(pattern, hstr->history, maxHistoryItems);
            } else {
                hstr_make_selection(NULL, hstr->history, maxHistoryItems);
            }
            result=hstr_print_selection(maxHistoryItems, pattern);

            move(hstr->promptY, basex+hstr_strlen(pattern));
            break;
        case KEY_UP:
        case K_CTRL_K:
        case K_CTRL_P:
            previousSelectionCursorPosition=selectionCursorPosition;
            if(selectionCursorPosition>0) {
                if(hstr->promptBottom) {
                    if(selectionCursorPosition <= (int)(hstr->promptItems-hstr->selectionSize)) {
                        selectionCursorPosition=hstr->promptItems-1;
                    } else {
                        selectionCursorPosition--;
                    }
                } else {
                    selectionCursorPosition--;
                }
            } else {
                if(hstr->promptBottom) {
                    selectionCursorPosition=hstr->promptItems-1;
                } else {
                    selectionCursorPosition=hstr->selectionSize-1;
                }
            }
            highlight_selection(selectionCursorPosition, previousSelectionCursorPosition, pattern);
            move(hstr->promptY, basex+strlen(pattern));
            break;
        case KEY_PPAGE:
            previousSelectionCursorPosition=selectionCursorPosition;
            if(selectionCursorPosition>=PG_JUMP_SIZE) {
                selectionCursorPosition=selectionCursorPosition-PG_JUMP_SIZE;
            } else {
                selectionCursorPosition=0;
            }
            highlight_selection(selectionCursorPosition, previousSelectionCursorPosition, pattern);
            move(hstr->promptY, basex+strlen(pattern));
            break;
        case KEY_DOWN:
        case K_CTRL_J:
        case K_CTRL_N:
            if(selectionCursorPosition==SELECTION_CURSOR_IN_PROMPT) {
                if(hstr->promptBottom) {
                    selectionCursorPosition=hstr->promptItems-hstr->selectionSize;
                } else {
                    selectionCursorPosition=previousSelectionCursorPosition=0;
                }
            } else {
                previousSelectionCursorPosition=selectionCursorPosition;
                if(hstr->promptBottom) {
                    if(selectionCursorPosition<hstr->promptItems-1) {
                        selectionCursorPosition++;
                    } else {
                        selectionCursorPosition=hstr->promptItems-hstr->selectionSize;
                    }
                } else {
                    if((selectionCursorPosition+1) < (int)hstr->selectionSize) {
                        selectionCursorPosition++;
                    } else {
                        selectionCursorPosition=0;
                    }
                }
            }
            if(hstr->selectionSize) {
                highlight_selection(selectionCursorPosition, previousSelectionCursorPosition, pattern);
            }
            move(hstr->promptY, basex+strlen(pattern));
            break;
        case KEY_NPAGE:
            if(selectionCursorPosition==SELECTION_CURSOR_IN_PROMPT) {
                selectionCursorPosition=previousSelectionCursorPosition=0;
            } else {
                previousSelectionCursorPosition=selectionCursorPosition;
                if((selectionCursorPosition+PG_JUMP_SIZE) < (int)hstr->selectionSize) {
                    selectionCursorPosition = selectionCursorPosition+PG_JUMP_SIZE;
                } else {
                    selectionCursorPosition=hstr->selectionSize-1;
                }
            }
            if(hstr->selectionSize) {
                highlight_selection(selectionCursorPosition, previousSelectionCursorPosition, pattern);
            }
            move(hstr->promptY, basex+strlen(pattern));
            break;
        case K_ENTER:
        case KEY_ENTER:
            executeResult=TRUE;
            if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
                result=getResultFromSelection(selectionCursorPosition, hstr, result);
                if(hstr->view==HSTR_VIEW_FAVORITES) {
                    favorites_choose(hstr->favorites,result);
                }
            }
            else {
                if (hstr->selectionSize > 0) {
                    result=hstr->selection[0];
                }
            }
            done=TRUE;
            break;
        case KEY_LEFT:
            fixCommand=TRUE;
            executeResult=TRUE;
            if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
                result=getResultFromSelection(selectionCursorPosition, hstr, result);
                if(hstr->view==HSTR_VIEW_FAVORITES) {
                    favorites_choose(hstr->favorites,result);
                }
            } else {
                result=pattern;
            }
            done=TRUE;
            break;
        case K_TAB:
        case KEY_RIGHT:
            if(!isSubshellHint) {
                editCommand=TRUE;
            } else {
                // Not setting editCommand to TRUE here,
                // because else an unnecessary blank line gets emitted before returning to prompt.
            }
            if(selectionCursorPosition!=SELECTION_CURSOR_IN_PROMPT) {
                result=getResultFromSelection(selectionCursorPosition, hstr, result);
                if(hstr->view==HSTR_VIEW_FAVORITES) {
                    favorites_choose(hstr->favorites,result);
                }
            } else {
                result=pattern;
            }
            done=TRUE;
            break;
        case K_CTRL_G:
        case K_ESC:
            result=NULL;
            history_mgmt_clear_dirty();
            done=TRUE;
            break;
        case K_CTRL_X:
            result=NULL;
            done=TRUE;
            break;
        default:
            LOGKEYS(Y_OFFSET_HELP, c);
            LOGCURSOR(Y_OFFSET_HELP);
            LOGUTF8(Y_OFFSET_HELP,pattern);
            LOGSELECTION(Y_OFFSET_HELP,getmaxy(stdscr),hstr->selectionSize);

            if(c>K_CTRL_Z) {
                selectionCursorPosition=SELECTION_CURSOR_IN_PROMPT;

                if(strlen(pattern)<(width-basex-1)) {
                    strcat(pattern, (char*)(&c));
                    print_pattern(pattern, hstr->promptY, basex);
                    cursorX=getcurx(stdscr);
                    cursorY=getcury(stdscr);
                }

                result = hstr_print_selection(maxHistoryItems, pattern);
                move(cursorY, cursorX);
                refresh();
            }
            break;
        }
    }
    hstr_curses_stop(hstr->keepPage);

    if(result!=NULL) {
        if(fixCommand) {
            fill_terminal_input("fc \"", FALSE);
        }
        fill_terminal_input(result, editCommand);
        if(fixCommand) {
            fill_terminal_input("\"", FALSE);
        }
        if(executeResult) {
            fill_terminal_input("\n", FALSE);
        }
    }
}

// TODO protection from line overflow (snprinf)
void hstr_assemble_cmdline_pattern(int argc, char* argv[], int startIndex)
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

void hstr_interactive(void)
{
    hstr->history=prioritized_history_create(hstr->bigKeys, hstr->blacklist.set);
    if(hstr->history) {
        history_mgmt_open();
        if(hstr->interactive) {
            loop_to_select();
        } else {
            stdout_history_and_return();
        }
        history_mgmt_flush();
    } // else (no history) handled in create() method

    hstr_exit(EXIT_SUCCESS);
}

void hstr_getopt(int argc, char **argv)
{
    int option_index = 0;
    int option = getopt_long(argc, argv, "fkVhnszb", long_options, &option_index);
    if(option != -1) {
        switch(option) {
        case 'f':
            hstr->view=HSTR_VIEW_FAVORITES;
            break;
        case 'n':
            hstr->interactive=false;
            break;
        case 'k':
            if(history_mgmt_remove_last_history_entry(hstr->verboseKill)) {
                hstr_exit(EXIT_SUCCESS);
                break;
            } else {
                hstr_exit(EXIT_FAILURE);
                break;
            }
        case 'b':
            blacklist_load(&hstr->blacklist);
            blacklist_dump(&hstr->blacklist);
            hstr_exit(EXIT_SUCCESS);
            break;
        case 'V':
            printf("%s", VERSION_STRING);
            hstr_exit(EXIT_SUCCESS);
            break;
        case 'h':
            printf("%s", HELP_STRING);
            hstr_exit(EXIT_SUCCESS);
            break;
        case 'z':
            printf("%s", INSTALL_ZSH_STRING);
            hstr_exit(EXIT_SUCCESS);
            break;
        case 's':
            // ZSH_VERSION is not exported by zsh > detected by parent process name
            if(isZshParentShell()) {
                printf("%s", INSTALL_ZSH_STRING);
            } else {
                printf("%s", INSTALL_BASH_STRING);
            }
            hstr_exit(EXIT_SUCCESS);
            break;
        case '?':
        default:
            printf("%s", HELP_STRING);
            hstr_exit(EXIT_SUCCESS);
        }
    }

    if(optind < argc) {
        hstr_assemble_cmdline_pattern(argc, argv, optind);
    }
}

int hstr_main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    hstr=malloc(sizeof(Hstr));
    hstr_init();

    hstr_get_env_configuration();
    hstr_getopt(argc, argv);
    favorites_get(hstr->favorites);
    blacklist_load(&hstr->blacklist);
    // hstr cleanup is handled by hstr_exit()
    hstr_interactive();

    return EXIT_SUCCESS;
}
