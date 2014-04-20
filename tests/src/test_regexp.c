/*
 ============================================================================
 Name        : test_regexp.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : A test for regular expression based matching
 ============================================================================
*/

#include <regex.h>
#include <stdio.h>
#include <stdbool.h>

void main() {
	bool caseSensitive=false;

	char *regexp="a";
	char *text="This is a command that I want to match: go.sh there";

	regex_t compiled;
	int compilationFlags=(caseSensitive?0:REG_ICASE);
	printf("Regular expressions matching:\n  '%s'\n  '%s'",text,regexp);
	int compilationStatus=regcomp(&compiled, regexp, compilationFlags);
	printf("\nCompilation: %d",compilationStatus);

	int matches;
	regmatch_t **matchPtr;
	int matchingFlags=0;
	int matchingStatus=regexec(&compiled, text, matches, matchPtr, matchingFlags);
	printf("\nMatching (status/matches/ptr): %d %d %p",matchingStatus,matches,matchPtr);

	printf("\n");
}

