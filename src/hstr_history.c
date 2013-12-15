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

typedef struct {
	char *item;
	unsigned rank;
} RankedHistoryItem;

static HistoryItems *history;
static HistoryItems *prioritizedHistory;

char *get_history_file() {
	char *home = getenv(ENV_VAR_HOME);
	char *fileName = (char*) malloc(
			strlen(home) + 1 + strlen(FILE_HISTORY) + 1);
	strcpy(fileName, home);
	strcat(fileName, "/");
	strcat(fileName, FILE_HISTORY);
	return fileName;
}

#define history_ranking_function(RANK, NEWORDEROCCURENCE) (RANK?RANK+NEWORDEROCCURENCE:NEWORDEROCCURENCE)

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
			r->rank=history_ranking_function(0, i);
			r->item=historyFileItems->items[i];

			hashmap_put(&rankmap, historyFileItems->items[i], r);

			radixItem=(RadixItem *)malloc(sizeof(RadixItem));
			radixItem->key=r->rank;
			radixItem->data=r;
			radixItem->next=NULL;
			radixsort_add(&rs, radixItem);
		} else {
			//printf("\n>>> %s ", r->item); fflush(stdout);
			radixItem=radix_cut(&rs, r->rank, r);

			if(radixItem!=NULL) {
				r->rank=history_ranking_function(r->rank, i);
				radixItem->key=r->rank;
				radixsort_add(&rs, radixItem);
			} // TODO else assert
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

void flush_history() {
	const char *filename = "history";
	char *const args[1] = {"-a"};
	execvp(filename, args);
}

HistoryItems *get_history_items() {
	history=(HistoryItems *)malloc(sizeof(HistoryItems));

	flush_history();
	using_history();

	char *historyFile = getenv(ENV_VAR_HISTFILE);
	if(!historyFile || strlen(historyFile)==0) {
		historyFile=get_history_file();
	}

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


