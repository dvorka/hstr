/*
 ============================================================================
 Name        : hstr_history.h
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Loading and processing of BASH history
 ============================================================================
*/

#include "include/hstr_history.h"
#include "include/hashset.h"
#include "include/hashmap.h"
#include "include/radixsort.h"
#include <stdbool.h>

#include "include/hstr_utils.h"

#define NDEBUG
#include <assert.h>

typedef struct {
	char *item;
	unsigned rank;
} RankedHistoryItem;

static HistoryItems *history;
static HistoryItems *prioritizedHistory;
static bool dirty;

unsigned history_ranking_function(unsigned rank, unsigned newOccurenceOrder, unsigned lng) {
	rank+=newOccurenceOrder/10 + lng;
	return rank;
}

char *get_history_file_name() {
	char *historyFile=getenv(ENV_VAR_HISTFILE);
	if(!historyFile || strlen(historyFile)==0) {
		char *home = getenv(ENV_VAR_HOME);
		historyFile = (char*) malloc(strlen(home) + 1 + strlen(FILE_HISTORY) + 1);
		strcat(strcat(strcpy(historyFile, home), "/"), FILE_HISTORY);
	}
	return historyFile;
}

void dump_prioritized_history(HistoryItems *ph) {
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

HistoryItems *prioritize_history(HistoryItems *historyFileItems) {
	HashMap rankmap;
	hashmap_init(&rankmap);

	HashSet blacklist;
	char *blackCommands[] = {"ls", "pwd", "cd", "hh"};
	int i;
	hashset_init(&blacklist);
	for(i=0; i<4; i++) {
		hashset_add(&blacklist, blackCommands[i]);
	}

	RadixSorter rs;
	radixsort_init(&rs);

	RankedHistoryItem *r;
	RadixItem *radixItem;
	for(i=0; i<historyFileItems->count; i++) {
		if(hashset_contains(&blacklist, historyFileItems->items[i])) {
			continue;
		}
		if((r=hashmap_get(&rankmap, historyFileItems->items[i]))==NULL) {
			r=(RankedHistoryItem *)malloc(sizeof(RankedHistoryItem));
			r->rank=history_ranking_function(0, i, strlen(historyFileItems->items[i]));
			r->item=historyFileItems->items[i];

			hashmap_put(&rankmap, historyFileItems->items[i], r);

			radixItem=(RadixItem *)malloc(sizeof(RadixItem));
			radixItem->key=r->rank;
			radixItem->data=r;
			radixItem->next=NULL;
			radixsort_add(&rs, radixItem);
		} else {
			radixItem=radix_cut(&rs, r->rank, r);

			assert(radixItem);

			if(radixItem) {
				r->rank=history_ranking_function(r->rank, i, strlen(historyFileItems->items[i]));
				radixItem->key=r->rank;
				radixsort_add(&rs, radixItem);
			}
		}
	}

	RadixItem **prioritizedRadix=radixsort_dump(&rs);
	prioritizedHistory=(HistoryItems *)malloc(sizeof(HistoryItems));
	prioritizedHistory->count=rs.size;
	prioritizedHistory->items=malloc(rs.size * sizeof(char*));
	for(i=0; i<rs.size; i++) {
		if(prioritizedRadix[i]->data) {
			prioritizedHistory->items[i]=((RankedHistoryItem *)(prioritizedRadix[i]->data))->item;
		}
	}

	radixsort_destroy(&rs);

	return prioritizedHistory;
}

void free_prioritized_history() {
	// TODO free(prioritizedHistory->items);
	// TODO free(prioritizedHistory);
}

HistoryItems *get_history_items() {
	history=(HistoryItems *)malloc(sizeof(HistoryItems));

	using_history();

	char *historyFile = get_history_file_name();
	if(read_history(historyFile)!=0) {
		fprintf(stderr, "\nUnable to read history file from '%s'!\n",historyFile);
		exit(EXIT_FAILURE);
	}
	HISTORY_STATE *historyState=history_get_history_state();

	if(historyState->length > 0) {
		history->count=historyState->length;
		history->items=(char**)malloc(history->count * sizeof(char*));

		HIST_ENTRY **historyList=history_list();
		int i;
		char *entry, *item;
		for(i=0; i<history->count; i++) {
			entry=historyList[i]->line;
			if(entry!=NULL) {
				item=malloc(strlen(entry)+1);
				strcpy(item, entry);
			} else {
				item=malloc(2);
				strcpy(item, " ");
			}
			history->items[i]=item;
		}
	} else {
		history->count=0;
		history->items=NULL;
	}

	return history;
}

void free_history_items() {
	free(history->items);
	free(history);
}

HistoryItems *get_prioritized_history() {
	HistoryItems *fileItems=get_history_items();
	return prioritize_history(fileItems);
}

void history_mgmt_open() {
	dirty=false;
}

void history_mgmt_remove(char *cmd) {
	get_history_items();

	int offset=history_search_pos(cmd, 0, 0);
	while(offset>=0) {
		free_history_entry(remove_history(offset));
		offset=history_search_pos(cmd, 0, ++offset);
	}

	write_history(get_history_file_name());
	dirty=true;
}

void history_mgmt_close() {
	if(dirty) {
		fill_terminal_input("history -r\n");
	}
}
