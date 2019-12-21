/*
 hstr_blacklist.h   header file for commands to be skipped from history

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

#ifndef HSTR_BLACKLIST_H
#define HSTR_BLACKLIST_H

#include "hashset.h"

#define ENV_VAR_USER "USER"
#define ENV_VAR_HOME "HOME"

#define FILE_HSTR_BLACKLIST ".hstr_blacklist"

typedef struct {
    bool useFile;
    bool isLoaded;
    bool isDefault;
    HashSet* set;
} Blacklist;

void blacklist_init(Blacklist* blacklist);
void blacklist_load(Blacklist* blacklist);
bool blacklist_in(Blacklist* blacklist, char *cmd);
void blacklist_dump(Blacklist* blacklist);
void blacklist_destroy(Blacklist* blacklist, bool freeBlacklist);

#endif
