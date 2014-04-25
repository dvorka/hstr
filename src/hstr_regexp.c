/*
 ============================================================================
 Name        : hstr_regexp.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : Simplified regexp that suits HSTR needs - matching and caching
 ============================================================================
*/

#include "include/hstr_regexp.h"

#define REGEXP_MATCH_BUFFER_SIZE 1

void hstr_regexp_init(HstrRegexp *hstrRegexp)
{
	hashset_init(&hstrRegexp->cache);
}

bool hstr_regexp_match(HstrRegexp *hstrRegexp, char *regexp, char *text, regmatch_t *match)
{
	regex_t* compiled=malloc(sizeof(regex_t));
	if(hashset_contains(&hstrRegexp->cache,regexp)) {
		compiled=hashset_get(&hstrRegexp->cache, regexp);
	} else {
		int compilationFlags=(hstrRegexp->caseSensitive?0:REG_ICASE);
		//printf("Regular expressions matching:\n  '%s'\n  '%s'",text,regexp);
		int compilationStatus=regcomp(compiled, regexp, compilationFlags);
		//printf("\nCompilation: %d",compilationStatus);
		if(!compilationStatus) {
			hashset_put(&hstrRegexp->cache, strdup(regexp), compiled);
		} else {
			free(compiled);
			// TODO error handling: regerror() to turn error codes to messages
			return false;
		}
	}

	int matches=REGEXP_MATCH_BUFFER_SIZE;
	regmatch_t matchPtr[REGEXP_MATCH_BUFFER_SIZE];
	int matchingFlags=0;
	int matchingStatus=regexec(compiled, text, matches, matchPtr, matchingFlags);
	//printf("\nMatching (status/matches): %s %d",(!matchingStatus?"match":"no-match"),matches);

	if(!matchingStatus) {
		//printf("\n  %d %d",matchPtr[0].rm_so, matchPtr[0].rm_eo);
		if(matchPtr[0].rm_so != -1) {
			//printf("\n* %d %d",matchPtr[0].rm_so,matchPtr[0].rm_eo);
			match->rm_so=matchPtr[0].rm_so;
			match->rm_eo=matchPtr[0].rm_eo;
			return true;
		}
	}
	return false;
}

void hstr_regexp_destroy(HstrRegexp *hstrRegexp)
{
	// TODO hashset_destroy();
}

