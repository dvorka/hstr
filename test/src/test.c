/*
 main.c     UNIT tests main for HSTR shell history completion utility

 Copyright (C) 2014-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <string.h>

// HSTR uses Unity C test framework: https://github.com/ThrowTheSwitch/Unity
#include "unity/src/c/unity.h"

#include "../../src/include/hashset.h"
#include "../../src/include/hstr_utils.h"
#include "../../src/include/hstr_favorites.h"
#include "../../src/include/hstr.h"

/*
 * IMPORTANT: make sure to run TEST RUNNER GENERATOR script after any change to this file!
 *
 */

void setUp(void)
{
}

void tearDown(void)
{
}

void test_args(void)
{
    const unsigned  LINELNG = 500;
    int argc = 3;
    char* argv[argc];
    argv[0] = "hstr";
    argv[1] = "-o";
    argv[2] = "_args";

    if(argc>0) {
        int i;
        char line[LINELNG];
        line[0]=0;
        for(i=0; i<argc; i++) {
            if((strlen(line)+strlen(argv[i])*2) > LINELNG) break;
            printf("%d %s\n", i, argv[i]);
            if(strstr(argv[i], " ")) {
                strcat(line, "\"");
            }
            strcat(line, argv[i]);
            if(strstr(argv[i], " ")) {
                strcat(line, "\"");
            }
            strcat(line, " ");
        }

        printf("#%s#\n", line);
        TEST_ASSERT_EQUAL_STRING("hstr -o _args ", line);
    } else {
        TEST_FAIL_MESSAGE("There must be an argument");
    }
}

void test_locate_char_in_string_overflow(void)
{
    TEST_ASSERT_EQUAL_STRING(0, strchr("a\nb",1));

    printf("%s\n",strchr("a\nb",98));
    TEST_ASSERT_EQUAL_STRING("b", strchr("a\nb",98));

    printf("%s\n",strchr("a\nb",10));
    TEST_ASSERT_EQUAL_STRING("\nb", strchr("a\nb",10));
}

void test_favorites(void)
{
    FavoriteItems favoriteItems;

    favorites_init(&favoriteItems);
    favorites_get(&favoriteItems);

    char* favorite = "UNIT-TEST-ENTRY";
    unsigned count = favoriteItems.count;

    TEST_ASSERT_EQUAL(count, favoriteItems.count);
    favorites_add(&favoriteItems, favorite);
    TEST_ASSERT_EQUAL(count+1, favoriteItems.count);

    favorites_choose(&favoriteItems, favorite);
    TEST_ASSERT_EQUAL_STRING(favorite, favoriteItems.items[0]);

    favorites_remove(&favoriteItems, favorite);
    TEST_ASSERT_EQUAL(count, favoriteItems.count);

    favorites_choose(&favoriteItems, favorite);
    TEST_ASSERT_NOT_EQUAL(favorite, favoriteItems.items[0]);
}

void test_hashset_blacklist()
{
    const char* commandBlacklist[] = { "a","b","c","d","e" };
    HashSet blacklist;
    int i;
    hashset_init(&blacklist);
    for (i = 0; i < 5; i++) {
        TEST_ASSERT_TRUE(hashset_add(&blacklist, commandBlacklist[i]));
    }
    for (i = 0; i < 5; i++) {
        printf("match %d\n", hashset_contains(&blacklist, hstr_strdup(commandBlacklist[i])));
        TEST_ASSERT_TRUE(hashset_contains(&blacklist, hstr_strdup(commandBlacklist[i])));
    }
}

void test_hashset_get_keys()
{
    const char* commandBlacklist[] = { "a","b","c","d","e" };
    HashSet blacklist;
    int i;
    hashset_init(&blacklist);
    for (i = 0; i < 5; i++) {
        TEST_ASSERT_TRUE(hashset_add(&blacklist, commandBlacklist[i]));
    }

    char **keys=hashset_keys(&blacklist);
    if(keys) {
        for(i=0; i<hashset_size(&blacklist); i++) {
            printf("Key: %s\n", keys[i]);
            TEST_ASSERT_NOT_EQUAL(NULL, keys[i]);
        }
    } else {
        TEST_FAIL_MESSAGE("Inserted keys are missing in hashset");
    }
}

void test_help_long(void)
{
    TEST_IGNORE_MESSAGE("Tests exits the program");

    char* ARG_HSTR = "hstr";
    char* ARG_HELP = "--help";
    int argc = 1+1;
    char* argv[argc];
    argv[0] = ARG_HSTR;
    argv[1] = ARG_HELP;
    int s;

    s = hstrMain(argc, argv);
    TEST_ASSERT_FALSE(s);
}

void test_help_short(void)
{
    TEST_IGNORE_MESSAGE("Tests exits the program");

    char* ARG_HSTR = "hstr";
    char* ARG_H = "-h";
    int argc = 1+1;
    char* argv[argc];
    argv[0] = ARG_HSTR;
    argv[1] = ARG_H;
    int s;

    s = hstrMain(argc, argv);
    TEST_ASSERT_FALSE(s);
}
