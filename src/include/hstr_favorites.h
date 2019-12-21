/*
 hstr_favorites.h       header file for favorite commands

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

#ifndef HSTR_FAVORITES_H
#define HSTR_FAVORITES_H

#include "hashset.h"

#define ENV_VAR_USER "USER"

#define FILE_HSTR_FAVORITES ".hstr_favorites"

typedef struct {
    char** items;
    unsigned count;
    bool loaded;
    bool reorderOnChoice;
    bool skipComments;
    HashSet* set;
} FavoriteItems;

void favorites_init(FavoriteItems* favorites);
void favorites_get(FavoriteItems* favorites);
void favorites_add(FavoriteItems* favorites, char* favorite);
void favorites_choose(FavoriteItems* favorites, char* choice);
bool favorites_remove(FavoriteItems* favorites, char* almostDead);
void favorites_destroy(FavoriteItems* favorites);

#endif
