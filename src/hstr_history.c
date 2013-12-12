#include "include/hstr_history.h"

static int historyItemsCount;
static char** historyItems;

char* getHistoryFile() {
	char *home = getenv(ENV_VAR_HOME);
	char *fileName = (char*) malloc(
			strlen(home) + 1 + strlen(FILE_HISTORY) + 1);
	strcpy(fileName, home);
	strcat(fileName, "/");
	strcat(fileName, FILE_HISTORY);
	return fileName;
}



#ifdef GET_HISTORY_FROM_FILE

static char *historyAsString;

char *load_history_file() {
	char* fileName = getHistoryFile();
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

char** get_history_items() {
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


void flushHistory() {
	const char* filename = "history";
	char *const args[1] = {"-a"};
	execvp(filename, args);
}

char** get_history_items() {
	flushHistory();
	using_history();

	char *historyFile = getenv(ENV_VAR_HISTFILE);
	if(historyFile==NULL || strlen(historyFile)==0) {
		historyFile=getHistoryFile();
	}

	if(read_history(historyFile)!=0) {
		fprintf(stderr, "\nUnable to read history file from '%s'!\n",historyFile);
		exit(EXIT_FAILURE);
	}
	HISTORY_STATE *historyState=history_get_history_state();

	if(historyState->length > 0) {
		historyItemsCount=historyState->length;

		historyItems=(char**)malloc(historyItemsCount*sizeof(char*));
		HIST_ENTRY **historyList=history_list();
		int i;
		char *entry, *item;
		for(i=0; i<historyItemsCount; i++) {
			entry=historyList[i]->line;
			if(entry!=NULL) {
				item=malloc(strlen(entry)+1);
				strcpy(item, entry);
			} else {
				item=malloc(2);
				strcpy(item, " ");
			}
			historyItems[i]=item;
		}
	} else {
		historyItemsCount=0;
		historyItems=NULL;
	}

	return historyItems;
}

int get_history_items_size() {
	return historyItemsCount;
}

void free_history_items() {
	free(historyItems);
}


#endif
