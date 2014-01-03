/*
 ============================================================================
 Name        : hstr_history.h
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Loading and processing of BASH history
 ============================================================================
*/

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
static const char *commandBlacklist[] = {"ls", "pwd", "cd", "hh", "mc"};

#ifdef DEBUG_RADIX
#define DEBUG_RADIXSORT() radixsort_stat(&rs); exit(0)
#else
#define DEBUG_RADIXSORT()
#endif

#define HISTORY_RANKING_FUNCTION(RANK,NEWOCCURENCEORDER,LNG) RANK+NEWOCCURENCEORDER/10+LNG

char *get_history_file_name()
{
	char *historyFile=getenv(ENV_VAR_HISTFILE);
	if(!historyFile || strlen(historyFile)==0) {
		char *home = getenv(ENV_VAR_HOME);
		historyFile = malloc(strlen(home) + 1 + strlen(FILE_HISTORY) + 1);
		strcat(strcat(strcpy(historyFile, home), "/"), FILE_HISTORY);
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

	if(historyState->length > 0) {
		HashSet rankmap;
		hashset_init(&rankmap);

		HashSet blacklist;
		int i;
		hashset_init(&blacklist);
		for(i=0; i<4; i++) {
			hashset_add(&blacklist, commandBlacklist[i]);
		}

		RadixSorter rs;
		radixsort_init(&rs, 100000);

		RankedHistoryItem *r;
		RadixItem *radixItem;
		HIST_ENTRY **historyList=history_list();
		char *line;
		for(i=0; i<historyState->length; i++) {
			line=historyList[i]->line;
			if(hashset_contains(&blacklist, line)) {
				continue;
			}
			if((r=hashset_get(&rankmap, line))==NULL) {
				r=malloc(sizeof(RankedHistoryItem));
				r->rank=HISTORY_RANKING_FUNCTION(0, i, strlen(line));
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
					r->rank=HISTORY_RANKING_FUNCTION(r->rank, i, strlen(line));
					radixItem->key=r->rank;
					radixsort_add(&rs, radixItem);
				}
			}
		}

		DEBUG_RADIXSORT();

		RadixItem **prioritizedRadix=radixsort_dump(&rs);
		prioritizedHistory=malloc(sizeof(HistoryItems));
		prioritizedHistory->count=rs.size;
		prioritizedHistory->items=malloc(rs.size * sizeof(char*));
		for(i=0; i<rs.size; i++) {
			if(prioritizedRadix[i]->data) {
				prioritizedHistory->items[i]=((RankedHistoryItem *)(prioritizedRadix[i]->data))->item;
			}
			free(prioritizedRadix[i]->data);
			free(prioritizedRadix[i]);
		}

		radixsort_destroy(&rs);

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

int history_mgmt_remove(char *cmd)
{
	int offset=history_search_pos(cmd, 0, 0), occurences=0;
	while(offset>=0) {
		occurences++;
		free_history_entry(remove_history(offset));
		offset=history_search_pos(cmd, 0, ++offset);
	}
	if(occurences) {
		write_history(get_history_file_name());
		dirty=true;
	}
	return occurences;
}

void history_mgmt_close()
{
	if(dirty) {
		fill_terminal_input("history -r\n", false);
	}
}
