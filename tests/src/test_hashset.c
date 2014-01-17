/*
 * test_hstr.c
 *
 *  Created on: Jan 10, 2014
 *      Author: dvorka
 */

#include "../src/include/hashset.h"

int main(int argc, char *argv[])
{
	const char *commandBlacklist[] = {"ls", "pwd", "cd", "hh", "mc"};
	HashSet blacklist;
	int i;
	hashset_init(&blacklist);
	for(i=0; i<5; i++) {
		hashset_add(&blacklist, commandBlacklist[i]);
	}

	for(i=0; i<5; i++) {
		printf("match %d\n", hashset_contains(&blacklist, strdup(commandBlacklist[i])));
	}
}

