/*
 hstr_regexp.h      header file for simplified regexp that suits HSTR needs

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

#ifndef HSTR_REGEXP_H
#define HSTR_REGEXP_H

#include <regex.h>

#include "hashset.h"

typedef struct {
    bool caseSensitive;
    HashSet cache;
} HstrRegexp;

void hstr_regexp_init(HstrRegexp* hstrRegexp);
bool hstr_regexp_match(
        HstrRegexp* hstrRegexp,
        const char* regexp,
        const char* text,
        regmatch_t* match,
        char* errorMessage,
        const size_t errorMessageSize);
void hstr_regexp_destroy(HstrRegexp* hstrRegexp);

int regexp_compile(regex_t* regexp, const char* regexpText);
int regexp_match(regex_t* regexp, const char* text);

#endif
