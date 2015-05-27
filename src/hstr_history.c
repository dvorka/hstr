/*
 ============================================================================
 Name        : hstr_history.h
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : Loading and processing of BASH history
 ============================================================================
*/

#include <stdio.h>
#include <limits.h>
#include <readline/history.h>
#include "include/hstr_history.h"
#include "include/hstr_regexp.h"

#define NDEBUG
#include <assert.h>

typedef struct {
    char *item;
    unsigned rank;
} RankedHistoryItem;

static HistoryItems *prioritizedHistory;
static bool dirty;

#ifdef DEBUG_RADIX
#define DEBUG_RADIXSORT() radixsort_stat(&rs, false); exit(0)
#else
#define DEBUG_RADIXSORT()
#endif

// TODO make this configurable from command line as option
#define METRICS_LOGARITHM(RANK,ORDER,LENGTH) RANK+(log(ORDER)*10.0)+LENGTH
#define METRICS_ADDITIVE(RANK,ORDER,LENGTH)  RANK+ORDER/10+LENGTH

unsigned history_ranking_function(unsigned rank, int newOccurenceOrder, size_t length) {
    long metrics=METRICS_LOGARITHM(rank, newOccurenceOrder, length);
    assert(metrics<UINT_MAX);
    return metrics;
}

char *get_history_file_name()
{
    char *historyFile=getenv(ENV_VAR_HISTFILE);
    if(!historyFile || strlen(historyFile)==0) {
        char *home = getenv(ENV_VAR_HOME);
        historyFile = malloc(strlen(home) + 1 + strlen(FILE_DEFAULT_HISTORY) + 1);
        strcat(strcat(strcpy(historyFile, home), "/"), FILE_DEFAULT_HISTORY);
    }
    return historyFile;
}

void dump_prioritized_history(HistoryItems *historyItems)
{
    printf("\n\nPrioritized history:");
    int i;
    for(i=0; i<historyItems->count; i++) {
        if(historyItems->items[i]!=NULL) {
            printf("\n%s",historyItems->items[i]); fflush(stdout);
        } else {
            printf("\n %d NULL",i); fflush(stdout);
        }
    }
    printf("\n"); fflush(stdout);
}

int get_item_offset()
{
    if(isZshParentShell()) {
        // In zsh history file, the format of item is
        // [:][blank][unix_timestamp][:][0][;][cmd]
        // Such as:
        // : 1420549651:0;ls /tmp/b
        // And the limit of unix timestamp 9999999999 is 2289/11/21,
        // so we could skip first 15 chars in every zsh history item to get the cmd.
        return ZSH_HISTORY_ITEM_OFFSET;
    } else {
        return BASH_HISTORY_ITEM_OFFSET;
    }
}

HistoryItems *get_prioritized_history(int optionBigKeys, HashSet *blacklist)
{
    using_history();

    char *historyFile = get_history_file_name();
    if(read_history(historyFile)!=0) {
        fprintf(stderr, "\nUnable to read history file from '%s'!\n",historyFile);
        exit(EXIT_FAILURE);
    }
    HISTORY_STATE *historyState=history_get_history_state();

    int itemOffset = get_item_offset();

    if(historyState->length > 0) {
        HashSet rankmap;
        hashset_init(&rankmap);

        int i;
        RadixSorter rs;
        unsigned radixMaxKeyEstimate=historyState->size*1000;
        radixsort_init(&rs, (radixMaxKeyEstimate<100000?100000:radixMaxKeyEstimate));
        rs.optionBigKeys=optionBigKeys;

        regex_t regexp;
        // HISTTIMEFORMAT defined > ^#1234567890$
        const char *histtimeformatTimestamp = "^#[[:digit:]]\\{10,\\}$";
        regexp_compile(&regexp, histtimeformatTimestamp);

        RankedHistoryItem *r;
        RadixItem *radixItem;
        HIST_ENTRY **historyList=history_list();
        char **rawHistory=malloc(sizeof(char*) * historyState->length);
        int rawOffset=historyState->length-1, rawTimestamps=0;
        char *line;
        for(i=0; i<historyState->length; i++, rawOffset--) {
            if(!regexp_match(&regexp, historyList[i]->line)) {
                rawHistory[rawOffset]=0;
                rawTimestamps++;
                continue;
            }
            if(historyList[i]->line && strlen(historyList[i]->line)>itemOffset) {
                line=historyList[i]->line+itemOffset;
            } else {
                line=historyList[i]->line;
            }
            rawHistory[rawOffset]=line;
            if(hashset_contains(blacklist, line)) {
                continue;
            }
            if((r=hashset_get(&rankmap, line))==NULL) {
                r=malloc(sizeof(RankedHistoryItem));
                r->rank=history_ranking_function(0, i, strlen(line));
                r->item=historyList[i]->line;

                hashset_put(&rankmap, line, r);

                radixItem=malloc(sizeof(RadixItem));
                radixItem->key=r->rank;
                radixItem->data=r;
                radixItem->next=NULL;
                radixsort_add(&rs, radixItem);
            } else {
                radixItem=radix_cut(&rs, r->rank, r);

                assert(radixItem);

                if(radixItem) {
                    r->rank=history_ranking_function(r->rank, i, strlen(line));
                    radixItem->key=r->rank;
                    radixsort_add(&rs, radixItem);
                }
            }
        }
        if(rawTimestamps) {
            rawOffset=0;
            for(i=0; i<historyState->length; i++) {
                if(rawHistory[i]) {
                    rawHistory[rawOffset++]=rawHistory[i];
                }
            }
        }

        regfree(&regexp);

        DEBUG_RADIXSORT();

        RadixItem **prioritizedRadix=radixsort_dump(&rs);
        prioritizedHistory=malloc(sizeof(HistoryItems));
        prioritizedHistory->count=rs.size;
        prioritizedHistory->rawCount=historyState->length-rawTimestamps;
        prioritizedHistory->items=malloc(rs.size * sizeof(char*));
        prioritizedHistory->rawItems=rawHistory;
        for(i=0; i<rs.size; i++) {
            if(prioritizedRadix[i]->data) {
                char* item = ((RankedHistoryItem *)(prioritizedRadix[i]->data))->item;
                if(strlen(item)>itemOffset) {
                    item += itemOffset;
                }
                prioritizedHistory->items[i]=item;
            }
            free(prioritizedRadix[i]->data);
            free(prioritizedRadix[i]);
        }

        radixsort_destroy(&rs);
        // TODO rankmap (?) and blacklist (?) to be destroyed

        return prioritizedHistory;
    } else {
        return NULL;
    }
}

void free_prioritized_history()
{
    free(prioritizedHistory->items);
    free(prioritizedHistory);
}

void history_mgmt_open()
{
    dirty=false;
}

void history_clear_dirty()
{
    dirty=false;
}

int history_mgmt_remove_from_system_history(char *cmd)
{
    int offset=history_search_pos(cmd, 0, 0), occurences=0;
    char *l;
    HISTORY_STATE *historyState=history_get_history_state();
    while(offset>=0) {
        l=historyState->entries[offset]->line;
        if(offset<historyState->length && !strcmp(cmd, l)) {
            occurences++;
            free_history_entry(remove_history(offset));
        }
        offset=history_search_pos(cmd, 0, ++offset);
    }
    if(occurences) {
        write_history(get_history_file_name());
        dirty=true;
    }
    return occurences;
}

int history_mgmt_remove_from_raw(char *cmd, HistoryItems *history) {
    int occurences=history->rawCount;
    if(history->rawCount) {
        int i, ii;
        for(i=0, ii=0; i<history->rawCount; i++) {
            if(strcmp(cmd, history->rawItems[i])) {
                history->rawItems[ii++]=history->rawItems[i];
            }
        }
        history->rawCount=ii;
    }
    return occurences-history->rawCount;
}

int history_mgmt_remove_from_ranked(char *cmd, HistoryItems *history) {
    int occurences=history->count;
    if(history->count) {
        int i, ii;
        for(i=0, ii=0; i<history->count; i++) {
            if(strcmp(cmd, history->items[i])) {
                history->items[ii++]=history->items[i];
            }
        }
        history->count=ii;
    }
    return occurences-history->count;
}

void history_mgmt_flush()
{
    if(dirty) {
        fill_terminal_input("history -r\n", false);
    }
}
