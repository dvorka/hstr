/*
 test.c     UNIT tests main for HSTR shell history completion utility

 Copyright (C) 2014-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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

#define HSTR_TESTS_UNIT 1

#include <string.h>
#include <regex.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

// HSTR uses Unity C test framework: https://github.com/ThrowTheSwitch/Unity
#include "unity/src/c/unity.h"

#include "../../src/include/hashset.h"
#include "../../src/include/hstr_utils.h"
#include "../../src/include/hstr_history.h"
#include "../../src/include/hstr_favorites.h"
#include "../../src/include/hstr_selection.h"
#include "../../src/include/hstr.h"

/*
 * IMPORTANT: make sure to run TEST RUNNER GENERATOR script after any change to this file!
 */

// tests run with a temporary HOME so that files like favorites of the user are never touched
static char testHome[PATH_MAX];
static bool testHomeCreated=false;
static char* originalHome=NULL;

void setUp(void)
{
    // code to be run before each test
    char* home=getenv(ENV_VAR_HOME);
    originalHome=home ? hstr_strdup(home) : NULL;

    const char* tmp=getenv("TMPDIR");
    snprintf(testHome, sizeof(testHome), "%s/hstr-unit-tests-XXXXXX", (tmp && *tmp) ? tmp : "/tmp");
    testHomeCreated=mkdtemp(testHome)!=NULL;
    TEST_ASSERT_TRUE_MESSAGE(testHomeCreated, "unable to create temporary HOME");
    setenv(ENV_VAR_HOME, testHome, 1);
}

void tearDown(void)
{
    // code to be run after each test - also after a failed one
    if(testHomeCreated) {
        char fileName[PATH_MAX];
        int length=snprintf(fileName, sizeof(fileName), "%s/%s", testHome, FILE_HSTR_FAVORITES);
        if(length>0 && length<(int)sizeof(fileName)) {
            unlink(fileName);
        }
        rmdir(testHome);
        testHomeCreated=false;
    }

    if(originalHome) {
        setenv(ENV_VAR_HOME, originalHome, 1);
        free(originalHome);
        originalHome=NULL;
    } else {
        unsetenv(ENV_VAR_HOME);
    }
}

void test_args(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN
    const unsigned  LINELNG = 500;
    int argc = 3;
    char* argv[argc];
    argv[0] = "hstr";
    argv[1] = "-o";
    argv[2] = "_args";

    // WHEN
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

        // THEN
        printf("#%s#\n", line);
        TEST_ASSERT_EQUAL_STRING("hstr -o _args ", line);
    } else {
        TEST_FAIL_MESSAGE("There must be an argument");
    }
}

void test_getopt(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN
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
    printf("\n= %s ========================================\n", __FUNCTION__);

    TEST_ASSERT_EQUAL_STRING(0, strchr("a\nb",1));

    printf("%s\n",strchr("a\nb",98));
    TEST_ASSERT_EQUAL_STRING("b", strchr("a\nb",98));

    printf("%s\n",strchr("a\nb",10));
    TEST_ASSERT_EQUAL_STRING("\nb", strchr("a\nb",10));
}

void test_favorites(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

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

static FavoriteItems* favorites_test_create(const char** items, unsigned count)
{
    FavoriteItems* favorites=malloc(sizeof(FavoriteItems));
    TEST_ASSERT_NOT_NULL(favorites);
    favorites_init(favorites);
    favorites->items=malloc(sizeof(char*) * count);
    TEST_ASSERT_NOT_NULL(favorites->items);
    unsigned i;
    for(i=0; i<count; i++) {
        favorites->items[i]=hstr_strdup(items[i]);
    }
    favorites->count=count;
    favorites->loaded=true;
    return favorites;
}

static void favorites_test_assert_items(FavoriteItems* favorites, const char** expected, unsigned count)
{
    TEST_ASSERT_EQUAL_UINT(count, favorites->count);
    unsigned i;
    for(i=0; i<count; i++) {
        TEST_ASSERT_NOT_NULL(favorites->items[i]);
        TEST_ASSERT_EQUAL_STRING(expected[i], favorites->items[i]);
    }
}

static bool favorites_test_file_exists(void)
{
    char* fileName=favorites_get_filename();
    bool exists=access(fileName, F_OK)==0;
    free(fileName);
    return exists;
}

static void favorites_test_assert_file(const char* expected)
{
    char* fileName=favorites_get_filename();
    FILE* file=fopen(fileName, "rb");
    free(fileName);
    TEST_ASSERT_NOT_NULL(file);
    char content[512]={0};
    size_t size=fread(content, sizeof(char), sizeof(content)-1, file);
    fclose(file);
    content[size]=0;
    TEST_ASSERT_EQUAL_STRING(expected, content);
}

// bug #470: choosing a command which is not a favorite corrupted favorites
void test_favorites_choose_missing_choice(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN
    const char* items[]={"ls -la", "git status", "echo fav"};
    FavoriteItems* favorites=favorites_test_create(items, 3);

    // WHEN
    favorites_choose(favorites, "not a favorite");

    // THEN
    favorites_test_assert_items(favorites, items, 3);
    TEST_ASSERT_FALSE(favorites_test_file_exists());
    favorites_destroy(favorites);
    printf("DONE\n");
}

void test_favorites_choose_moves_choice_to_head(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN
    const char* items[]={"ls -la", "git status", "echo fav"};
    FavoriteItems* favorites=favorites_test_create(items, 3);

    // WHEN
    favorites_choose(favorites, "echo fav");
    favorites_choose(favorites, "echo fav");

    // THEN
    const char* expected[]={"echo fav", "ls -la", "git status"};
    favorites_test_assert_items(favorites, expected, 3);
    favorites_test_assert_file("echo fav\nls -la\ngit status\n");
    favorites_destroy(favorites);
    printf("DONE\n");
}

void test_favorites_add_new_to_head(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN
    const char* items[]={"ls -la", "git status"};
    FavoriteItems* favorites=favorites_test_create(items, 2);

    // WHEN
    favorites_add(favorites, "echo new");

    // THEN
    const char* expected[]={"echo new", "ls -la", "git status"};
    favorites_test_assert_items(favorites, expected, 3);
    favorites_test_assert_file("echo new\nls -la\ngit status\n");
    favorites_destroy(favorites);
    printf("DONE\n");
}

// adding a command which already is a favorite must not create a duplicate
void test_favorites_add_existing_no_duplicate(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN
    const char* items[]={"ls -la", "git status", "echo fav"};
    FavoriteItems* favorites=favorites_test_create(items, 3);

    // WHEN
    favorites_add(favorites, "git status");

    // THEN
    const char* expected[]={"git status", "ls -la", "echo fav"};
    favorites_test_assert_items(favorites, expected, 3);
    favorites_test_assert_file("git status\nls -la\necho fav\n");
    favorites_destroy(favorites);
    printf("DONE\n");
}

void test_favorites_remove(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN
    const char* items[]={"ls -la", "git status", "echo fav"};
    FavoriteItems* favorites=favorites_test_create(items, 3);

    // WHEN
    bool removed=favorites_remove(favorites, "git status");

    // THEN (removed item is freed - checked by valgrind)
    TEST_ASSERT_TRUE(removed);
    const char* expected[]={"ls -la", "echo fav"};
    favorites_test_assert_items(favorites, expected, 2);
    favorites_test_assert_file("ls -la\necho fav\n");
    favorites_destroy(favorites);
    printf("DONE\n");
}

void test_favorites_remove_missing(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN
    const char* items[]={"ls -la", "git status"};
    FavoriteItems* favorites=favorites_test_create(items, 2);

    // WHEN
    bool removed=favorites_remove(favorites, "not a favorite");

    // THEN
    TEST_ASSERT_FALSE(removed);
    favorites_test_assert_items(favorites, items, 2);
    TEST_ASSERT_FALSE(favorites_test_file_exists());
    favorites_destroy(favorites);
    printf("DONE\n");
}

void test_favorites_null_safety(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN
    const char* items[]={"ls -la", "git status"};
    FavoriteItems* favorites=favorites_test_create(items, 2);

    // WHEN
    favorites_choose(favorites, NULL);
    favorites_add(favorites, NULL);
    bool removed=favorites_remove(favorites, NULL);

    // THEN
    TEST_ASSERT_FALSE(removed);
    favorites_test_assert_items(favorites, items, 2);
    favorites_destroy(favorites);
    printf("DONE\n");
}

// bug #470: cursor must never point beyond the selection (stale items)
void test_selection_index(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN 3 items on a 20 rows screen
    const int promptItems=20, selectionSize=3;

    // WHEN / THEN prompt at the top: rows 0..2 are items
    TEST_ASSERT_EQUAL_INT(-1, hstr_selection_index(SELECTION_CURSOR_IN_PROMPT, false, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(0, hstr_selection_index(0, false, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(2, hstr_selection_index(2, false, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(-1, hstr_selection_index(3, false, promptItems, selectionSize));

    // WHEN / THEN prompt at the bottom: rows 17..19 are items, the first item is on the last row
    TEST_ASSERT_EQUAL_INT(-1, hstr_selection_index(SELECTION_CURSOR_IN_PROMPT, true, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(0, hstr_selection_index(19, true, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(2, hstr_selection_index(17, true, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(-1, hstr_selection_index(16, true, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(-1, hstr_selection_index(0, true, promptItems, selectionSize));

    // WHEN / THEN empty selection
    TEST_ASSERT_EQUAL_INT(-1, hstr_selection_index(0, false, promptItems, 0));
    TEST_ASSERT_EQUAL_INT(-1, hstr_selection_index(19, true, promptItems, 0));
    printf("DONE\n");
}

// bug #470: navigation keys must keep the cursor on the selection rows
void test_selection_cursor_move(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN 3 items on a 20 rows screen
    const int promptItems=20, selectionSize=3, page=10;
    const int inPrompt=SELECTION_CURSOR_IN_PROMPT;

    // WHEN / THEN empty selection keeps the cursor in the prompt
    TEST_ASSERT_EQUAL_INT(inPrompt, hstr_selection_cursor_move(inPrompt, 1, false, promptItems, 0));
    TEST_ASSERT_EQUAL_INT(inPrompt, hstr_selection_cursor_move(5, -page, true, promptItems, 0));

    // WHEN / THEN prompt at the top (rows 0..2)
    TEST_ASSERT_EQUAL_INT(0, hstr_selection_cursor_move(inPrompt, 1, false, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(2, hstr_selection_cursor_move(inPrompt, -1, false, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(1, hstr_selection_cursor_move(0, 1, false, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(0, hstr_selection_cursor_move(2, 1, false, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(2, hstr_selection_cursor_move(0, -1, false, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(2, hstr_selection_cursor_move(0, page, false, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(0, hstr_selection_cursor_move(2, -page, false, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(2, hstr_selection_cursor_move(7, 0, false, promptItems, selectionSize));

    // WHEN / THEN prompt at the bottom (rows 17..19)
    TEST_ASSERT_EQUAL_INT(19, hstr_selection_cursor_move(inPrompt, -1, true, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(17, hstr_selection_cursor_move(inPrompt, 1, true, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(17, hstr_selection_cursor_move(inPrompt, -page, true, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(17, hstr_selection_cursor_move(19, -page, true, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(19, hstr_selection_cursor_move(17, page, true, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(19, hstr_selection_cursor_move(17, -1, true, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(17, hstr_selection_cursor_move(19, 1, true, promptItems, selectionSize));
    TEST_ASSERT_EQUAL_INT(17, hstr_selection_cursor_move(0, 0, true, promptItems, selectionSize));

    // WHEN / THEN more items than rows (e.g. right after a terminal resize)
    TEST_ASSERT_EQUAL_INT(4, hstr_selection_cursor_move(inPrompt, -1, false, 5, 30));
    TEST_ASSERT_EQUAL_INT(4, hstr_selection_cursor_move(0, page, true, 5, 30));
    TEST_ASSERT_EQUAL_INT(0, hstr_selection_cursor_move(4, -page, true, 5, 30));
    TEST_ASSERT_EQUAL_INT(inPrompt, hstr_selection_cursor_move(inPrompt, 1, true, 0, 30));
    printf("DONE\n");
}

void test_selection_cursor_step(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN navigation keys and keys which do not move the cursor
    const int up[]={KEY_UP, K_CTRL_K, K_CTRL_P};
    const int down[]={KEY_DOWN, K_CTRL_J, K_CTRL_N};
    const int other[]={'a', K_CTRL_N+1, KEY_LEFT, KEY_RIGHT, 0, -1};

    // WHEN / THEN line moves
    unsigned i;
    for(i=0; i<sizeof(up)/sizeof(up[0]); i++) {
        TEST_ASSERT_EQUAL_INT(-1, hstr_selection_cursor_step(up[i]));
    }
    for(i=0; i<sizeof(down)/sizeof(down[0]); i++) {
        TEST_ASSERT_EQUAL_INT(1, hstr_selection_cursor_step(down[i]));
    }

    // WHEN / THEN page moves
    TEST_ASSERT_EQUAL_INT(-PG_JUMP_SIZE, hstr_selection_cursor_step(KEY_PPAGE));
    TEST_ASSERT_EQUAL_INT(PG_JUMP_SIZE, hstr_selection_cursor_step(KEY_NPAGE));

    // WHEN / THEN other keys do not move the cursor
    for(i=0; i<sizeof(other)/sizeof(other[0]); i++) {
        TEST_ASSERT_EQUAL_INT(0, hstr_selection_cursor_step(other[i]));
    }
    printf("DONE\n");
}

void test_selection_add(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN an empty selection with capacity for 3 items
    char* selection[3]={NULL, NULL, NULL};
    const unsigned capacity=sizeof(selection)/sizeof(selection[0]);
    unsigned size=0;
    char ls[]="ls", cd[]="cd", lsCopy[]="ls", pwd[]="pwd", git[]="git";

    // WHEN items are added with duplicates filtering
    bool lsAdded=hstr_selection_add(selection, capacity, &size, ls, true);
    bool cdAdded=hstr_selection_add(selection, capacity, &size, cd, true);
    bool lsCopyAdded=hstr_selection_add(selection, capacity, &size, lsCopy, true);

    // THEN duplicate (by value, not by pointer) is skipped
    TEST_ASSERT_TRUE(lsAdded);
    TEST_ASSERT_TRUE(cdAdded);
    TEST_ASSERT_FALSE(lsCopyAdded);
    TEST_ASSERT_EQUAL_UINT(2, size);
    TEST_ASSERT_EQUAL_PTR(ls, selection[0]);
    TEST_ASSERT_EQUAL_PTR(cd, selection[1]);

    // WHEN duplicate is added without duplicates filtering
    bool lsCopyAddedRaw=hstr_selection_add(selection, capacity, &size, lsCopy, false);

    // THEN it is appended
    TEST_ASSERT_TRUE(lsCopyAddedRaw);
    TEST_ASSERT_EQUAL_UINT(3, size);
    TEST_ASSERT_EQUAL_PTR(lsCopy, selection[2]);

    // WHEN the selection is full
    bool pwdAdded=hstr_selection_add(selection, capacity, &size, pwd, false);

    // THEN nothing is written beyond capacity
    TEST_ASSERT_FALSE(pwdAdded);
    TEST_ASSERT_EQUAL_UINT(3, size);

    // WHEN invalid arguments are passed
    unsigned emptySize=0;
    bool nullSelection=hstr_selection_add(NULL, capacity, &emptySize, git, false);
    bool nullSize=hstr_selection_add(selection, capacity, NULL, git, false);
    bool nullLine=hstr_selection_add(selection, capacity, &emptySize, NULL, false);
    bool zeroCapacity=hstr_selection_add(selection, 0, &emptySize, git, false);

    // THEN they are rejected
    TEST_ASSERT_FALSE(nullSelection);
    TEST_ASSERT_FALSE(nullSize);
    TEST_ASSERT_FALSE(nullLine);
    TEST_ASSERT_FALSE(zeroCapacity);
    TEST_ASSERT_EQUAL_UINT(0, emptySize);
    printf("DONE\n");
}

void test_hashset_blacklist()
{
    printf("\n= %s ========================================\n", __FUNCTION__);

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
    printf("\n= %s ========================================\n", __FUNCTION__);

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
    printf("\n= %s ========================================\n", __FUNCTION__);

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
    printf("\n= %s ========================================\n", __FUNCTION__);

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
    printf("\n= %s ========================================\n", __FUNCTION__);

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
    printf("\n= %s ========================================\n", __FUNCTION__);

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

void test_parse_history_line()
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    TEST_ASSERT_EQUAL(NULL, parse_history_line(NULL));

    TEST_ASSERT_EQUAL_STRING("ls", parse_history_line("ls"));
    TEST_ASSERT_EQUAL_STRING(":::", parse_history_line(":::"));

    TEST_ASSERT_EQUAL_STRING(":vspman epoll_ctl", parse_history_line(": 1592444398:0;:vspman epoll_ctl"));
    TEST_ASSERT_EQUAL_STRING("scan-view work/scans/2020-07-30-114451-3063582-1", parse_history_line(": 1596135828:6109;scan-view work/scans/2020-07-30-114451-3063582-1"));
    TEST_ASSERT_EQUAL_STRING(":wq", parse_history_line(": 1592444398:0;:wq"));
    TEST_ASSERT_EQUAL_STRING(":wq", parse_history_line(":wq"));

    TEST_ASSERT_EQUAL_STRING(": 159244439:0;:wq", parse_history_line(": 159244439:0;:wq"));
    TEST_ASSERT_EQUAL_STRING(": 1592444398:;:wq", parse_history_line(": 1592444398:;:wq"));
    TEST_ASSERT_EQUAL_STRING(": 1592444398:0:wq", parse_history_line(": 1592444398:0:wq"));
    TEST_ASSERT_EQUAL_STRING(":1592444398:0;:vspman epoll_ctl", parse_history_line(":1592444398:0;:vspman epoll_ctl"));
}

void test_assemble_cmdline_pattern(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN
    #define CMDLINE_LNG 8192
    #define TEST_CMDLINE(expected, argc, ...) do { \
        char cmdline[CMDLINE_LNG] = {0}; \
        char* argv[] = {__VA_ARGS__}; \
        int i; \
        size_t cmdlineLen = strlen(cmdline); \
        for(i=2; i<argc; i++) { \
            size_t argLen = strlen(argv[i]); \
            size_t spaceNeeded = (i+1<argc) ? 1 : 0; \
            if(cmdlineLen + argLen + spaceNeeded + 1 > CMDLINE_LNG) break; \
            strcpy(cmdline + cmdlineLen, argv[i]); \
            cmdlineLen += argLen; \
            if(i+1<argc) { \
                cmdline[cmdlineLen++] = ' '; \
                cmdline[cmdlineLen] = '\0'; \
            } \
        } \
        printf("  Result: '%s'\n", cmdline); \
        TEST_ASSERT_EQUAL_STRING(expected, cmdline); \
    } while(0)

    // WHEN - THEN
    printf("Test 1: Single arg no spaces\n");
    TEST_CMDLINE("test", 3, "hstr", "--", "test");

    printf("Test 2: Single arg WITH spaces - NO QUOTES!\n");
    TEST_CMDLINE("a b", 3, "hstr", "--", "a b");

    printf("Test 3: Multiple args no spaces\n");
    TEST_CMDLINE("hello world", 4, "hstr", "--", "hello", "world");

    printf("Test 4: Multiple args with spaces\n");
    TEST_CMDLINE("hello world test case", 4, "hstr", "--", "hello world", "test case");

    printf("Test 5: Mixed args\n");
    TEST_CMDLINE("ls -la my file.txt", 5, "hstr", "--", "ls", "-la", "my file.txt");

    printf("Test 6: Multiple consecutive spaces\n");
    TEST_CMDLINE("hello  world  test", 3, "hstr", "--", "hello  world  test");

    printf("Test 7: Empty arg\n");
    TEST_CMDLINE("", 3, "hstr", "--", "");

    printf("Test 8: Special chars\n");
    TEST_CMDLINE("grep 'test pattern' file.txt", 3, "hstr", "--", "grep 'test pattern' file.txt");

    printf("Test 9: Tab character\n");
    TEST_CMDLINE("test\ttab", 3, "hstr", "--", "test\ttab");

    printf("Test 10: UTF-8\n");
    TEST_CMDLINE("café ☕", 3, "hstr", "--", "café ☕");

    printf("Test 11: No args\n");
    TEST_CMDLINE("", 2, "hstr", "--");

    printf("Test 12: Length calculation for 'aaaaa bbbbb ccccc ddddd eeeee'\n");
    char result[CMDLINE_LNG] = {0};
    char* argv12[] = {"hstr", "--", "aaaaa", "bbbbb", "ccccc", "ddddd", "eeeee"};
    int i;
    size_t cmdlineLen = 0;
    for(i=2; i<7; i++) {
        size_t argLen = strlen(argv12[i]);
        size_t spaceNeeded = (i+1<7) ? 1 : 0;
        if(cmdlineLen + argLen + spaceNeeded + 1 > CMDLINE_LNG) break;
        strcpy(result + cmdlineLen, argv12[i]);
        cmdlineLen += argLen;
        if(i+1<7) {
            result[cmdlineLen++] = ' ';
            result[cmdlineLen] = '\0';
        }
    }
    printf("  Result: '%s' (len=%zu)\n", result, strlen(result));
    TEST_ASSERT_EQUAL_STRING("aaaaa bbbbb ccccc ddddd eeeee", result);
    TEST_ASSERT_EQUAL_INT(29, strlen(result));

    printf("DONE\n");
}
