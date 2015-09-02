/*
 test_*.c       HSTR test

 Copyright (C) 2014  Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "../../src/include/hashset.h"
#include "../../src/include/hstr_utils.h"

void testBlacklist() {
    const char* commandBlacklist[] = { "a","b","c","d","e" };
    HashSet blacklist;
    int i;
    hashset_init(&blacklist);
    for (i = 0; i < 5; i++) {
        hashset_add(&blacklist, commandBlacklist[i]);
    }
    for (i = 0; i < 5; i++) {
        printf("match %d\n", hashset_contains(&blacklist, hstr_strdup(commandBlacklist[i])));
    }
}

void testGetKeys() {
    const char* commandBlacklist[] = { "a","b","c","d","e" };
    HashSet blacklist;
    int i;
    hashset_init(&blacklist);
    for (i = 0; i < 5; i++) {
        hashset_add(&blacklist, commandBlacklist[i]);
    }

    char **keys=hashset_keys(&blacklist);
    if(keys) {
        for(i=0; i<hashset_size(&blacklist); i++) {
            printf("\nKey: %s", keys[i]);
        }
    }
}

int main(int argc, char *argv[])
{
    testGetKeys();
}
