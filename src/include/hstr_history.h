#ifndef _HSTR_HISTORY_H_
#define _HSTR_HISTORY_H_

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <readline/history.h>
#include <unistd.h>
#include "hstr_utils.h"

//#define GET_HISTORY_FROM_FILE
#define GET_HISTORY_USING_LIBRARY

#define ENV_VAR_USER "USER"
#define ENV_VAR_HOME "HOME"
#define ENV_VAR_HISTFILE "HISTFILE"

#define FILE_HISTORY ".bash_history"

char** get_history_items();
int get_history_items_size();
void free_history_items();

#endif
