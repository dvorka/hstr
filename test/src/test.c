/*
 test.c     UNIT tests main for HSTR shell history completion utility

 Copyright (C) 2014-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <regex.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

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

void test_getopt(void)
{
    int argc = 3;
    char* argv[argc];
    argv[0] = "hstr";
    argv[1] = "--create";
    argv[2] = "_getopt";

    int c;
    int digit_optind = 0;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
                {"add",     required_argument, 0,  0 },
                {"append",  no_argument,       0,  0 },
                {"delete",  required_argument, 0,  0 },
                {"verbose", no_argument,       0,  0 },
                {"create",  required_argument, 0, 'c'},
                {"file",    required_argument, 0,  0 },
                {0,         0,                 0,  0 }
        };

        c = getopt_long(argc, argv, "abc:d:012", long_options, &option_index);
        if (c == -1)
            break;


        switch (c) {
        case 0:
            printf("option %s", long_options[option_index].name);
            if (optarg)
                printf(" with arg %s", optarg);
            printf("\n");
            break;

        case '0':
        case '1':
        case '2':
            if (digit_optind != 0 && digit_optind != this_option_optind)
                printf("digits occur in two different argv-elements.\n");
            digit_optind = this_option_optind;
            printf("option %c\n", c);
            break;

        case 'a':
            printf("option a\n");
            break;

        case 'b':
            printf("option b\n");
            break;

        case 'c':
            printf("option c with value '%s'\n", optarg);
            break;

        case 'd':
            printf("option d with value '%s'\n", optarg);
            break;

        case '?':

            break;

        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }

        TEST_ASSERT_EQUAL('c', c);
    }

    if(optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc) {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
        TEST_FAIL_MESSAGE("Thist unit test is supposed to succeed with 'c' option");
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

void test_regexp(void)
{
    unsigned REGEXP_MATCH_BUFFER_SIZE = 10;

    bool caseSensitive=false;

    char *regexp="^b";
    char *text="This is a command that I want to match: go.sh there";

    regex_t precompiled;
    int compilationFlags=(caseSensitive?0:REG_ICASE);
    printf("Regular expressions matching:\n  '%s'\n  '%s'",text,regexp);
    int compilationStatus=regcomp(&precompiled, regexp, compilationFlags);
    printf("\nCompilation: %d",compilationStatus);
    TEST_ASSERT_FALSE(compilationStatus);

    int matches=REGEXP_MATCH_BUFFER_SIZE;
    regmatch_t matchPtr[REGEXP_MATCH_BUFFER_SIZE];
    int matchingFlags=0;
    int matchingStatus=regexec(&precompiled, text, matches, matchPtr, matchingFlags);
    printf("\nMatching (status/matches): %s %d",(!matchingStatus?"match":"no-match"), matches);
    TEST_ASSERT_EQUAL(10, matches);

    if(!matchingStatus) {
        unsigned i;
        for(i=0; i<REGEXP_MATCH_BUFFER_SIZE; i++) {
            printf("\n  %d %d",matchPtr[i].rm_so, matchPtr[i].rm_eo);
            if(matchPtr[i].rm_so != -1) {
                printf("\n* %d %d",matchPtr[i].rm_so,matchPtr[i].rm_eo);
            }
        }
    } else {
        char message[100];
        regerror(matchingStatus, &precompiled, message, 100);
        printf("\nRegexp failed with %s\n", message);
    }

    printf("\n");
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

    s = hstr_main(argc, argv);
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

    s = hstr_main(argc, argv);
    TEST_ASSERT_FALSE(s);
}

void test_string_elide()
{
    char buffer[1000];

    // do nothing - string fits to screen
    hstr_strelide(buffer, "0123456789", 20);
    printf("%s\n", buffer);
    TEST_ASSERT_EQUAL_STRING("0123456789", buffer);

    // do nothing - string too short
    hstr_strelide(buffer, "012", 1);
    printf("%s\n", buffer);
    TEST_ASSERT_EQUAL_STRING("012", buffer);

    hstr_strelide(buffer, "0123456789", 6);
    printf("%s\n", buffer);
    TEST_ASSERT_EQUAL_STRING("01...9", buffer);


    hstr_strelide(buffer, "0123456789", 10);
    TEST_ASSERT_EQUAL_STRING("0123456789", buffer);
    printf("%s\n", buffer);

    hstr_strelide(buffer, "0123456789", 9);
    TEST_ASSERT_EQUAL_STRING("012...789", buffer);
    printf("%s\n", buffer);

    hstr_strelide(buffer, "0123456789", 8);
    TEST_ASSERT_EQUAL_STRING("012...89", buffer);
    printf("%s\n", buffer);
}
