/*
 ============================================================================
 Name        : hstr_history.h
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Loading and processing of BASH history
 ============================================================================
*/

#ifndef _HSTR_HISTORY_H_
#define _HSTR_HISTORY_H_

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <readline/history.h>
#include <unistd.h>
#include <stdbool.h>
#include "hstr_utils.h"
#include "hashset.h"
#include "radixsort.h"

#define ENV_VAR_USER "USER"
#define ENV_VAR_HOME "HOME"
#define ENV_VAR_HISTFILE "HISTFILE"

#define FILE_HISTORY ".bash_history"

typedef struct {
	char **items;
	char **raw;
	unsigned count;
	unsigned rawCount;
} HistoryItems;

HistoryItems *get_prioritized_history();

HistoryItems *get_history_items();
void free_history_items();

HistoryItems *prioritize_history(HistoryItems *historyFileItems);
void free_prioritized_history();

void history_mgmt_open();
int history_mgmt_remove(char *cmd);
void history_mgmt_close();

#endif
