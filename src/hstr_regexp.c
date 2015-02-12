/*
 ============================================================================
 Name        : hstr_regexp.c
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : Simplified regexp that suits HSTR needs - matching and caching
 ============================================================================
*/

#include "include/hstr_regexp.h"

#include "include/hstr_utils.h"

#define REGEXP_MATCH_BUFFER_SIZE 1

void hstr_regexp_init(HstrRegexp *hstrRegexp)
{
    hashset_init(&hstrRegexp->cache);
}

bool hstr_regexp_match(
        HstrRegexp *hstrRegexp,
        const char *regexp,
        const char *text,
        regmatch_t *match,
        char *errorMessage,
        const size_t errorMessageSize)
{
    regex_t* compiled;
    if(hashset_contains(&hstrRegexp->cache,regexp)) {
        compiled=hashset_get(&hstrRegexp->cache, regexp);
    } else {
        compiled=malloc(sizeof(regex_t));
        int compilationFlags=(hstrRegexp->caseSensitive?0:REG_ICASE);
        int compilationStatus=regcomp(compiled, regexp, compilationFlags);
        if(!compilationStatus) {
            hashset_put(&hstrRegexp->cache, hstr_strdup(regexp), compiled);
        } else {
            regerror(compilationStatus, compiled, errorMessage, errorMessageSize);
            free(compiled);
            return false;
        }
    }

    int matches=REGEXP_MATCH_BUFFER_SIZE;
    regmatch_t matchPtr[REGEXP_MATCH_BUFFER_SIZE];
    int matchingFlags=0;
    int matchingStatus=regexec(compiled, text, matches, matchPtr, matchingFlags);
    if(!matchingStatus) {
        if(matchPtr[0].rm_so != -1) {
            match->rm_so=matchPtr[0].rm_so;
            match->rm_eo=matchPtr[0].rm_eo;
            return true;
        }
    }
    return false;
}

void hstr_regexp_destroy(HstrRegexp *hstrRegexp)
{
    hashset_destroy(&hstrRegexp->cache, true);
}

int regexp_compile(regex_t *regexp, const char *regexpText)
{
    return regcomp(regexp, regexpText, 0);
    //return regcomp(regexp, regexpText, REG_NEWLINE);
}

int regexp_match(regex_t *regexp, const char *text)
{
    const char *p = text;
    // TODO study multi matches and make this safe
    const int n_matches = 100;
    regmatch_t m[n_matches];
    return regexec(regexp, p, n_matches, m, 0);
}
