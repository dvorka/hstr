#include "include/hstr_history.h"

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

HistoryItems *prioritize_history(HistoryItems *historyFileItems) {
	return historyFileItems;
}

void free_prioritized_history() {
	//free(prioritizedHistory->items);
	//free(prioritizedHistory);
}


#ifdef GET_HISTORY_FROM_FILE

static char *historyAsString;

char *load_history_file() {
	char *fileName = get_history_file();
	if(access(fileName, F_OK) != -1) {
		char *file_contents;
		long input_file_size;

		FILE *input_file = fopen(fileName, "rb");
		fseek(input_file, 0, SEEK_END);
		input_file_size = ftell(input_file);
		rewind(input_file);
		file_contents = malloc((input_file_size + 1) * (sizeof(char)));
		if(fread(file_contents, sizeof(char), input_file_size, input_file)==-1) {
			exit(EXIT_FAILURE);
		}
		fclose(input_file);
		file_contents[input_file_size] = 0;

		return file_contents;
	} else {
	    fprintf(stderr,"\nHistory file not found: %s\n",fileName);
	    exit(EXIT_FAILURE);
	}
}

int count_history_lines(char *history) {
	int i = 0;
	char *p=strchr(history,'\n');
	while (p!=NULL) {
		i++;
		p=strchr(p+1,'\n');
	}
	return i;
}

char **get_tokenized_history(char *history, int lines) {
	char **tokens = malloc(sizeof(char*) * lines);

	int i = 0;
	char *pb=history, *pe;
	pe=strchr(history, '\n');
	while(pe!=NULL) {
		tokens[i]=pb;
		*pe=0;

		pb=pe+1;
		pe=strchr(pb, '\n');
		i++;
	}

	return tokens;
}

char **get_history_items() {
	historyAsString = load_history_file(FILE_HISTORY);
	historyItemsCount = count_history_lines(historyAsString);
	historyItems = get_tokenized_history(historyAsString, historyItemsCount);
	reverse_char_pointer_array(historyItems, historyItemsCount);
	return historyItems;
}

int get_history_items_size() {
	return historyItemsCount;
}

void free_history_items() {
	free(historyAsString);
	free(historyItems);
}



#else


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


#endif
