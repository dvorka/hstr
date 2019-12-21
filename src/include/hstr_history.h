/*
 hstr_history.h     header file for loading and processing of BASH history

 Copyright (C) 2014-2020  Martin Dvorak <martin.dvorak@mindforger.com>

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

#ifndef HSTR_HISTORY_H
#define HSTR_HISTORY_H

#include <fcntl.h>
#include <limits.h>
// do NOT remove stdio.h include - must be present on certain system before readline to compile
#include <stdio.h>
#include <readline/history.h>

#include "hstr_utils.h"
#include "hstr_regexp.h"
#include "radixsort.h"
#include "hstr_favorites.h"

#define ENV_VAR_HISTFILE "HISTFILE"

#define FILE_DEFAULT_HISTORY ".bash_history"
#define FILE_ZSH_HISTORY ".zsh_history"
#define FILE_ZSH_ZHISTORY ".zhistory"

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

HistoryItems* prioritized_history_create(int optionBigKeys, HashSet* blacklist);
void prioritized_history_destroy(HistoryItems* h);

void history_mgmt_open(void);
void history_mgmt_clear_dirty(void);
bool history_mgmt_load_history_file(void);
int history_mgmt_remove_from_system_history(char* cmd);
bool history_mgmt_remove_last_history_entry(bool verbose);
int history_mgmt_remove_from_raw(char* cmd, HistoryItems* history);
int history_mgmt_remove_from_ranked(char* cmd, HistoryItems* history);
void history_mgmt_flush(void);

#endif
