/*
 ============================================================================
 Name        : hstr_history.h
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : Loading and processing of BASH history
 ============================================================================
*/

#ifndef _HSTR_HISTORY_H_
#define _HSTR_HISTORY_H_

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <readline/history.h>
#include <unistd.h>
#include <stdbool.h>

#include "hstr_favorites.h"
#include "hstr_utils.h"
#include "hashset.h"
#include "radixsort.h"

#define ENV_VAR_HISTFILE "HISTFILE"

#define FILE_DEFAULT_HISTORY ".bash_history"
#define FILE_ZSH_HISTORY ".zsh_history"

#define ZSH_HISTORY_ITEM_OFFSET 15
#define BASH_HISTORY_ITEM_OFFSET 0

typedef struct {
    // ranked history
    char **items;
    unsigned count;
    // raw history
    char **rawItems;
    unsigned rawCount;
} HistoryItems;

HistoryItems *get_prioritized_history();

HistoryItems *get_history_items();
void free_history_items();

HistoryItems *prioritize_history(HistoryItems *historyFileItems);
void free_prioritized_history();

void history_mgmt_open();
void history_clear_dirty();
int history_mgmt_remove_from_system_history(char *cmd);
int history_mgmt_remove_from_hh_raw(char *cmd, HistoryItems *historyItems);
void history_mgmt_flush();

#endif
