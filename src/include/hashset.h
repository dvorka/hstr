/*
 hashset.h      header file for hash set

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

#ifndef HASHSET_H
#define HASHSET_H

#include "hstr_utils.h"

#define HASH_MAP_SIZE 10007

struct HashSetNode {
    char* key;
    void* value;
    struct HashSetNode* next;
};

typedef struct {
    struct HashSetNode* lists[HASH_MAP_SIZE];
    int currentSize;
} HashSet;

void hashset_init(HashSet* hs);

int hashset_contains(const HashSet* hs, const char* key);
int hashset_add(HashSet* hs, const char* key);
int hashset_size(const HashSet* hs);
char** hashset_keys(const HashSet* hs);

void *hashset_get(const HashSet* hm, const char* key);
int hashset_put(HashSet* hm, const char* key, void* value);
int hashset_remove(const HashSet* hm, const char* key);
void hashset_stat(const HashSet* hm);

void hashset_destroy(HashSet* hs, const bool freeValues);

#endif
