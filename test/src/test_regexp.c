/*
 test_*.c       A test for regular expression based matching

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

#include <regex.h>
#include <stdio.h>
#include <stdbool.h>

#define REGEXP_MATCH_BUFFER_SIZE 10

void main() {
    bool caseSensitive=false;

    char *regexp="^b";
    char *text="This is a command that I want to match: go.sh there";

    regex_t compiled;
    int compilationFlags=(caseSensitive?0:REG_ICASE);
    printf("Regular expressions matching:\n  '%s'\n  '%s'",text,regexp);
    int compilationStatus=regcomp(&compiled, regexp, compilationFlags);
    printf("\nCompilation: %d",compilationStatus);

    int matches=REGEXP_MATCH_BUFFER_SIZE;
    regmatch_t matchPtr[REGEXP_MATCH_BUFFER_SIZE];
    int matchingFlags=0;
    int matchingStatus=regexec(&compiled, text, matches, matchPtr, matchingFlags);
    printf("\nMatching (status/matches): %s %d",(!matchingStatus?"match":"no-match"),matches);

    if(!matchingStatus) {
        int i;
        for(i=0; i<REGEXP_MATCH_BUFFER_SIZE; i++) {
            printf("\n  %d %d",matchPtr[i].rm_so, matchPtr[i].rm_eo);
            if(matchPtr[i].rm_so != -1) {
                printf("\n* %d %d",matchPtr[i].rm_so,matchPtr[i].rm_eo);
            }
        }
    } else {
        regerror(matchingStatus);
    }

    printf("\n");
}
