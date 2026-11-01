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
#include <locale.h>

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
// history file variables are restored after each test as tests may change them
static char* originalHistFile=NULL;
static char* originalHstrHistFile=NULL;

static char* env_save(const char* name)
{
    char* value=getenv(name);
    return value ? hstr_strdup(value) : NULL;
}

static void env_restore(const char* name, char** value)
{
    if(*value) {
        setenv(name, *value, 1);
        free(*value);
        *value=NULL;
    } else {
        unsetenv(name);
    }
}

void setUp(void)
{
    // code to be run before each test
    originalHome=env_save(ENV_VAR_HOME);
    originalHistFile=env_save(ENV_VAR_HISTFILE);
    originalHstrHistFile=env_save(ENV_VAR_HSTR_HISTFILE);

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

    env_restore(ENV_VAR_HOME, &originalHome);
    env_restore(ENV_VAR_HISTFILE, &originalHistFile);
    env_restore(ENV_VAR_HSTR_HISTFILE, &originalHstrHistFile);
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
    hstr_strelide(buffer, sizeof(buffer), "0123456789", 20);
    printf("%s\n", buffer);
    TEST_ASSERT_EQUAL_STRING("0123456789", buffer);

    // do nothing - string too short
    hstr_strelide(buffer, sizeof(buffer), "012", 1);
    printf("%s\n", buffer);
    TEST_ASSERT_EQUAL_STRING("012", buffer);

    hstr_strelide(buffer, sizeof(buffer), "0123456789", 6);
    printf("%s\n", buffer);
    TEST_ASSERT_EQUAL_STRING("01...9", buffer);


    hstr_strelide(buffer, sizeof(buffer), "0123456789", 10);
    TEST_ASSERT_EQUAL_STRING("0123456789", buffer);
    printf("%s\n", buffer);

    hstr_strelide(buffer, sizeof(buffer), "0123456789", 9);
    TEST_ASSERT_EQUAL_STRING("012...789", buffer);
    printf("%s\n", buffer);

    hstr_strelide(buffer, sizeof(buffer), "0123456789", 8);
    TEST_ASSERT_EQUAL_STRING("012...89", buffer);
    printf("%s\n", buffer);
}

// pattern typed on a terminal wider than the pattern buffer must not overflow it
void test_strcat_char(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN a small buffer guarded by canary bytes
    char memory[8];
    memset(memory, '#', sizeof(memory));
    char* buffer=memory+1;
    const size_t size=sizeof(memory)-2;
    buffer[0]=0;

    // WHEN the buffer is filled beyond its capacity
    unsigned i, appended=0;
    for(i=0; i<2*size; i++) {
        if(hstr_strcat_char(buffer, size, 'a'+i)) {
            appended++;
        }
    }

    // THEN only size-1 characters are appended, terminated and canaries are intact
    TEST_ASSERT_EQUAL_UINT(size-1, appended);
    TEST_ASSERT_EQUAL_STRING("abcde", buffer);
    TEST_ASSERT_EQUAL_INT('#', memory[0]);
    TEST_ASSERT_EQUAL_INT('#', memory[sizeof(memory)-1]);

    // GIVEN an empty buffer
    char pattern[8]={0};

    // WHEN UTF-8 bytes, curses key codes and invalid values are appended
    bool utf8First=hstr_strcat_char(pattern, sizeof(pattern), 0xC3);
    bool utf8Second=hstr_strcat_char(pattern, sizeof(pattern), 0xA1);
    bool keyCode=hstr_strcat_char(pattern, sizeof(pattern), KEY_LEFT);
    bool zero=hstr_strcat_char(pattern, sizeof(pattern), 0);
    bool negative=hstr_strcat_char(pattern, sizeof(pattern), -1);

    // THEN only bytes are appended
    TEST_ASSERT_TRUE(utf8First);
    TEST_ASSERT_TRUE(utf8Second);
    TEST_ASSERT_FALSE(keyCode);
    TEST_ASSERT_FALSE(zero);
    TEST_ASSERT_FALSE(negative);
    TEST_ASSERT_EQUAL_STRING("\xC3\xA1", pattern);

    // GIVEN invalid buffers
    char unterminated[4]={'a', 'b', 'c', 'd'};
    char empty[1]={0};

    // WHEN a character is appended
    bool nullAppended=hstr_strcat_char(NULL, 8, 'x');
    bool unterminatedAppended=hstr_strcat_char(unterminated, sizeof(unterminated), 'x');
    bool emptyAppended=hstr_strcat_char(empty, sizeof(empty), 'x');
    bool zeroSizeAppended=hstr_strcat_char(pattern, 0, 'x');

    // THEN nothing is written
    TEST_ASSERT_FALSE(nullAppended);
    TEST_ASSERT_FALSE(unterminatedAppended);
    TEST_ASSERT_FALSE(emptyAppended);
    TEST_ASSERT_FALSE(zeroSizeAppended);
    TEST_ASSERT_EQUAL_MEMORY("abcd", unterminated, sizeof(unterminated));
    TEST_ASSERT_EQUAL_INT(0, empty[0]);
    printf("DONE\n");
}

static bool is_in_test_home(const char* fileName)
{
    size_t length=strlen(testHome);
    return fileName && !strncmp(testHome, fileName, length) && fileName[length]=='/';
}

// HSTR_HISTFILE takes precedence over HISTFILE, shell history file ignores it
void test_history_file_name(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN no history file variables
    unsetenv(ENV_VAR_HSTR_HISTFILE);
    unsetenv(ENV_VAR_HISTFILE);

    // WHEN
    char* defaultFile=get_history_file_name();
    char* defaultShellFile=get_shell_history_file_name();

    // THEN shell specific default in HOME is used (bash or zsh - depends on parent process)
    TEST_ASSERT_TRUE(is_in_test_home(defaultFile));
    TEST_ASSERT_EQUAL_STRING(defaultFile, defaultShellFile);
    free(defaultFile);
    free(defaultShellFile);

    // GIVEN HISTFILE and empty HSTR_HISTFILE
    setenv(ENV_VAR_HISTFILE, "/tmp/hstr-shell-history", 1);
    setenv(ENV_VAR_HSTR_HISTFILE, "", 1);

    // WHEN
    char* histFile=get_history_file_name();
    char* histShellFile=get_shell_history_file_name();

    // THEN empty HSTR_HISTFILE is ignored
    TEST_ASSERT_EQUAL_STRING("/tmp/hstr-shell-history", histFile);
    TEST_ASSERT_EQUAL_STRING("/tmp/hstr-shell-history", histShellFile);
    free(histFile);
    free(histShellFile);

    // GIVEN both HISTFILE and HSTR_HISTFILE
    setenv(ENV_VAR_HSTR_HISTFILE, "/tmp/hstr-global-history", 1);

    // WHEN
    char* hstrFile=get_history_file_name();
    char* hstrShellFile=get_shell_history_file_name();

    // THEN HSTR uses HSTR_HISTFILE, shell history file is still HISTFILE
    TEST_ASSERT_EQUAL_STRING("/tmp/hstr-global-history", hstrFile);
    TEST_ASSERT_EQUAL_STRING("/tmp/hstr-shell-history", hstrShellFile);
    free(hstrFile);
    free(hstrShellFile);

    // GIVEN HSTR_HISTFILE without HISTFILE
    unsetenv(ENV_VAR_HISTFILE);

    // WHEN
    char* onlyHstrFile=get_history_file_name();
    char* onlyHstrShellFile=get_shell_history_file_name();

    // THEN shell history file falls back to the default in HOME
    TEST_ASSERT_EQUAL_STRING("/tmp/hstr-global-history", onlyHstrFile);
    TEST_ASSERT_TRUE(is_in_test_home(onlyHstrShellFile));
    free(onlyHstrFile);
    free(onlyHstrShellFile);
    printf("DONE\n");
}

// switches to a UTF-8 locale (as HSTR does w/ setlocale(LC_ALL, "") in a UTF-8 terminal),
// returns the original locale to be restored or NULL if no UTF-8 locale is available
static char* utf8_locale_set(void)
{
    char* current=setlocale(LC_ALL, NULL);
    char* original=current ? hstr_strdup(current) : NULL;
    const char* locales[]={"C.UTF-8", "C.utf8", "en_US.UTF-8", "en_US.utf8"};
    unsigned i;
    for(i=0; i<sizeof(locales)/sizeof(locales[0]); i++) {
        if(setlocale(LC_ALL, locales[i])) {
            return original;
        }
    }
    free(original);
    return NULL;
}

static void utf8_locale_restore(char* original)
{
    setlocale(LC_ALL, original ? original : "C");
    free(original);
}

// bug #533: prompt and pattern w/ multibyte characters must be measured in columns
void test_string_width(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN UTF-8 locale
    char* originalLocale=utf8_locale_set();
    if(!originalLocale) {
        TEST_IGNORE_MESSAGE("UTF-8 locale is not available");
    }

    // WHEN
    int ascii=hstr_strwidth("$ ");
    int arrow=hstr_strwidth("\xE2\x9D\xAF ");             // "❯ " 3 byte character
    int arrows=hstr_strwidth("\xE2\x9D\xAF\xE2\x9D\xAF ");  // "❯❯ "
    int lambda=hstr_strwidth("\xCE\xBB ");               // "λ " 2 byte character
    int cjk=hstr_strwidth("\xE4\xB8\xAD\xE6\x96\x87> ");     // "中文> " double width characters
    int empty=hstr_strwidth("");
    int null=hstr_strwidth(NULL);
    int prefix=hstr_strnwidth("\xE2\x9D\xAF\xE2\x9D\xAF", 3);
    int incomplete=hstr_strwidth("a\xE2\x9D");             // character being typed
    int invalid=hstr_strwidth("a\xFF" "b");

    // THEN
    TEST_ASSERT_EQUAL_INT(2, ascii);
    TEST_ASSERT_EQUAL_INT(2, arrow);
    TEST_ASSERT_EQUAL_INT(3, arrows);
    TEST_ASSERT_EQUAL_INT(2, lambda);
    TEST_ASSERT_EQUAL_INT(6, cjk);
    TEST_ASSERT_EQUAL_INT(0, empty);
    TEST_ASSERT_EQUAL_INT(0, null);
    TEST_ASSERT_EQUAL_INT(1, prefix);
    TEST_ASSERT_EQUAL_INT(1, incomplete);
    TEST_ASSERT_EQUAL_INT(3, invalid);

    utf8_locale_restore(originalLocale);
    printf("DONE\n");
}

void test_string_elide_multibyte(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN UTF-8 locale and strings w/ multibyte characters
    char* originalLocale=utf8_locale_set();
    if(!originalLocale) {
        TEST_IGNORE_MESSAGE("UTF-8 locale is not available");
    }
    char buffer[100];
    const char* greek="\xCE\xB1\xCE\xB2\xCE\xB3\xCE\xB4\xCE\xB5\xCE\xB6\xCE\xB7\xCE\xB8"; // "αβγδεζηθ"
    const char* cjk="\xE4\xB8\x80\xE4\xBA\x8C\xE4\xB8\x89\xE5\x9B\x9B\xE4\xBA\x94";     // "一二三四五" (10 columns)

    // WHEN / THEN fits - not elided
    hstr_strelide(buffer, sizeof(buffer), greek, 8);
    TEST_ASSERT_EQUAL_STRING(greek, buffer);

    // WHEN / THEN elided by columns, not by bytes
    hstr_strelide(buffer, sizeof(buffer), greek, 6);
    TEST_ASSERT_EQUAL_STRING("\xCE\xB1\xCE\xB2...\xCE\xB8", buffer); // "αβ...θ"
    TEST_ASSERT_EQUAL_INT(6, hstr_strwidth(buffer));

    // WHEN / THEN double width characters are not split and the result fits
    hstr_strelide(buffer, sizeof(buffer), cjk, 8);
    TEST_ASSERT_EQUAL_STRING("\xE4\xB8\x80...\xE4\xBA\x94", buffer); // "一...五"
    TEST_ASSERT_TRUE(hstr_strwidth(buffer)<=8);

    // WHEN / THEN no space for "..." - prefix fitting the width (used to overflow the buffer)
    hstr_strelide(buffer, sizeof(buffer), "0123456789", 0);
    TEST_ASSERT_EQUAL_STRING("", buffer);
    hstr_strelide(buffer, sizeof(buffer), "0123456789", 3);
    TEST_ASSERT_EQUAL_STRING("012", buffer);
    hstr_strelide(buffer, sizeof(buffer), cjk, 3);
    TEST_ASSERT_EQUAL_STRING("\xE4\xB8\x80", buffer);

    // WHEN / THEN small buffer is never overflowed
    char small[4];
    hstr_strelide(small, sizeof(small), "0123456789", 20);
    TEST_ASSERT_EQUAL_STRING("012", small);

    utf8_locale_restore(originalLocale);
    printf("DONE\n");
}

// backspace must delete the whole multibyte character and never write before the string
void test_chop(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN strings w/ 1, 2, 3 and 4 byte characters and invalid sequences
    char ascii[]="ab";
    char twoBytes[]="a\xCE\xBB";          // "aλ"
    char threeBytes[]="a\xE2\x9D\xAF";    // "a❯"
    char fourBytes[]="a\xF0\x9F\x98\x80"; // "a😀"
    char incomplete[]="a\xE2\x9D";        // character being typed
    char loneHighByte[]="\xE9";           // e.g. Latin-1 input
    char loneContinuation[]="a\x80";
    char empty[]="";

    // WHEN
    hstr_chop(ascii);
    hstr_chop(twoBytes);
    hstr_chop(threeBytes);
    hstr_chop(fourBytes);
    hstr_chop(incomplete);
    hstr_chop(loneHighByte);
    hstr_chop(loneContinuation);
    hstr_chop(empty);
    hstr_chop(NULL);

    // THEN
    TEST_ASSERT_EQUAL_STRING("a", ascii);
    TEST_ASSERT_EQUAL_STRING("a", twoBytes);
    TEST_ASSERT_EQUAL_STRING("a", threeBytes);
    TEST_ASSERT_EQUAL_STRING("a", fourBytes);
    TEST_ASSERT_EQUAL_STRING("a", incomplete);
    TEST_ASSERT_EQUAL_STRING("", loneHighByte);
    TEST_ASSERT_EQUAL_STRING("a", loneContinuation);
    TEST_ASSERT_EQUAL_STRING("", empty);
    printf("DONE\n");
}

// zsh metafies bytes 0x83-0xA2 of multibyte characters: "č" (C4 8D) is stored as C4 83 AD
void test_history_line_equals(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN zsh history lines w/ metafied "čeština" w/ and w/o extended history prefix
    char metafied[]="echo \xC4\x83\xAD" "e\xC5\x83\x81tina";
    char extended[]=": 1730000000:6109;echo \xC4\x83\xAD" "e\xC5\x83\x81tina";
    char plain[]="ls /tmp";
    const char* command="echo \xC4\x8D" "e\xC5\xA1tina";

    // WHEN
    bool metafiedZsh=history_line_equals(metafied, command, true);
    bool extendedZsh=history_line_equals(extended, command, true);
    bool metafiedBash=history_line_equals(metafied, command, false);
    bool plainEquals=history_line_equals(plain, "ls /tmp", true);
    bool plainDiffers=history_line_equals(plain, "ls", true);
    bool nullLine=history_line_equals(NULL, command, true);
    bool nullCommand=history_line_equals(plain, NULL, true);

    // THEN unmetafied only for zsh, history lines are NOT modified
    TEST_ASSERT_TRUE(metafiedZsh);
    TEST_ASSERT_TRUE(extendedZsh);
    TEST_ASSERT_FALSE(metafiedBash);
    TEST_ASSERT_TRUE(plainEquals);
    TEST_ASSERT_FALSE(plainDiffers);
    TEST_ASSERT_FALSE(nullLine);
    TEST_ASSERT_FALSE(nullCommand);
    TEST_ASSERT_EQUAL_STRING("echo \xC4\x83\xAD" "e\xC5\x83\x81tina", metafied);
    printf("DONE\n");
}

void test_remove_from_system_history(void)
{
    printf("\n= %s ========================================\n", __FUNCTION__);

    // GIVEN history w/ Bash timestamp and zsh extended history entry (multi-digit duration)
    char historyFile[PATH_MAX+16];
    int length=snprintf(historyFile, sizeof(historyFile), "%s/history", testHome);
    TEST_ASSERT_TRUE(length>0 && length<(int)sizeof(historyFile));
    setenv(ENV_VAR_HSTR_HISTFILE, historyFile, 1);
    using_history();
    clear_history();
    add_history("#1700000000");
    add_history("ls");
    add_history("pwd");
    add_history(": 1596135828:6109;ls");
    add_history("lsblk");

    // WHEN
    int removed=history_mgmt_remove_from_system_history("ls");
    int missing=history_mgmt_remove_from_system_history("not in history");
    int null=history_mgmt_remove_from_system_history(NULL);

    // THEN both occurrences and the timestamp are removed, other entries are kept
    TEST_ASSERT_EQUAL_INT(2, removed);
    TEST_ASSERT_EQUAL_INT(0, missing);
    TEST_ASSERT_EQUAL_INT(0, null);
    HIST_ENTRY** entries=history_list();
    TEST_ASSERT_NOT_NULL(entries);
    TEST_ASSERT_EQUAL_STRING("pwd", entries[0]->line);
    TEST_ASSERT_EQUAL_STRING("lsblk", entries[1]->line);
    TEST_ASSERT_NULL(entries[2]);
    TEST_ASSERT_EQUAL_INT(0, access(historyFile, F_OK));

    clear_history();
    unlink(historyFile);
    printf("DONE\n");
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
