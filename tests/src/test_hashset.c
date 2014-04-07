/*
 ============================================================================
 Name        : test_hashset.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : A test
 ============================================================================
*/

#include "../src/include/hashset.h"

void testBlacklist() {
	const char* commandBlacklist[] = { };
	HashSet blacklist;
	int i;
	hashset_init(&blacklist);
	for (i = 0; i < 5; i++) {
		hashset_add(&blacklist, commandBlacklist[i]);
	}
	for (i = 0; i < 5; i++) {
		printf("match %d\n",
				hashset_contains(&blacklist, strdup(commandBlacklist[i])));
	}
}

int main(int argc, char *argv[])
{
	testBlacklist();
}

