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

#define NDEBUG
#include <assert.h>

typedef struct {
    char *item;
    unsigned rank;
} RankedHistoryItem;

static HistoryItems *prioritizedHistory;
static bool dirty;

static const char *commandBlacklist[] = {
        "ls", "pwd", "cd", "cd ..", "hh", "mc",
        "ls ", "pwd ", "cd ", "cd .. ", "hh ", "mc "
};

#ifdef DEBUG_RADIX
#define DEBUG_RADIXSORT() radixsort_stat(&rs, false); exit(0)
#else
#define DEBUG_RADIXSORT()
#endif

unsigned history_ranking_function(unsigned rank, int newOccurenceOrder, size_t length) {
    long metrics=rank+(log(newOccurenceOrder)*10.0)+length;
    // alternative metrics:
    //   rank+newOccurenceOrder/10+length
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

void dump_prioritized_history(HistoryItems *ph)
{
    printf("\n\nPrioritized history:");
    int i;
    for(i=0; i<ph->count; i++) {
        if(ph->items[i]!=NULL) {
            printf("\n%s",ph->items[i]); fflush(stdout);
        } else {
            printf("\n %d NULL",i); fflush(stdout);
        }
    }
    printf("\n"); fflush(stdout);
}

HistoryItems *get_prioritized_history()
{
    using_history();

    char *historyFile = get_history_file_name();
    if(read_history(historyFile)!=0) {
        fprintf(stderr, "\nUnable to read history file from '%s'!\n",historyFile);
        exit(EXIT_FAILURE);
    }
    HISTORY_STATE *historyState=history_get_history_state();

    int itemOffset = 0;
    char* zshFileName = ".zsh_history";
    int historyFileLen = strlen(historyFile);
    int zshFileNameLen = strlen(zshFileName);
    if (historyFileLen >= zshFileNameLen) {
        int i = historyFileLen - zshFileNameLen;
        for (int j = 0; i < historyFileLen; i++, j++) {
            if (historyFile[i] != zshFileName[j]) {
                break;
            }
        }
        if (i == historyFileLen) {
            itemOffset = ZSH_HISTORY_ITEM_OFFSET;
        }
    }

    if(historyState->length > 0) {
        HashSet rankmap;
        hashset_init(&rankmap);

        HashSet blacklist;
        int i;
        hashset_init(&blacklist);
        int length=sizeof(commandBlacklist)/sizeof(commandBlacklist[0]);
        for(i=0; i<length; i++) {
            hashset_add(&blacklist, commandBlacklist[i]);
        }

        RadixSorter rs;
        unsigned radixMaxKeyEstimate=historyState->size*1000;
        radixsort_init(&rs, (radixMaxKeyEstimate<100000?100000:radixMaxKeyEstimate));
        rs.optFloorAndInsertBigKeys=true;

        RankedHistoryItem *r;
        RadixItem *radixItem;
        HIST_ENTRY **historyList=history_list();
        char **rawHistory=malloc(sizeof(char*) * historyState->length);
        int rawOffset=historyState->length-1;
        char *line;
        for(i=0; i<historyState->length; i++, rawOffset--) {
            line=historyList[i]->line;
            rawHistory[rawOffset]=line;
            if(hashset_contains(&blacklist, line)) {
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

        DEBUG_RADIXSORT();

        RadixItem **prioritizedRadix=radixsort_dump(&rs);
        prioritizedHistory=malloc(sizeof(HistoryItems));
        prioritizedHistory->count=rs.size;
        prioritizedHistory->rawCount=historyState->length;
        prioritizedHistory->items=malloc(rs.size * sizeof(char*));
        prioritizedHistory->raw=rawHistory;
        for(i=0; i<rs.size; i++) {
            if(prioritizedRadix[i]->data) {
                char* item = ((RankedHistoryItem *)(prioritizedRadix[i]->data))->item;
                item += itemOffset;
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

int history_mgmt_remove(char *cmd)
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

void history_mgmt_flush()
{
    if(dirty) {
        fill_terminal_input("history -r\n", false);
    }
}
