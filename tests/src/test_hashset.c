/*
 ============================================================================
 Name        : test_hashset.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : A test
 ============================================================================
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
