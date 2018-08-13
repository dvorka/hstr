/*
 hstr_history.h     header file for loading and processing of BASH history

 Copyright (C) 2014-2018  Martin Dvorak <martin.dvorak@mindforger.com>

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

#ifndef _HSTR_HISTORY_H_
#define _HSTR_HISTORY_H_

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
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
    char** items;
    unsigned count;
    // raw history
    char** rawItems;
    unsigned rawCount;
} HistoryItems;

HistoryItems* get_prioritized_history(int optionBigKeys, HashSet* blacklist);

HistoryItems* get_history_items(void);
void free_history_items(void);

HistoryItems* prioritize_history(HistoryItems* historyFileItems);
void free_prioritized_history(void);

void history_mgmt_open(void);
void history_clear_dirty(void);
int history_mgmt_remove_from_system_history(char* cmd);
bool history_mgmt_remove_last_history_entry(bool verbose);
int history_mgmt_remove_from_raw(char* cmd, HistoryItems* history);
int history_mgmt_remove_from_ranked(char* cmd, HistoryItems* history);
void history_mgmt_flush(void);

#endif
