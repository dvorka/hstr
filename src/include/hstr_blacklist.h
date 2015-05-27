/*
 ============================================================================
 Name        : hstr_blacklist.h
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Commands to be skipped from history.
 ============================================================================
 */

#ifndef HSTR_BLACKLIST_H_
#define HSTR_BLACKLIST_H_

#include <stdlib.h>
#include <stdbool.h>
#include "hashset.h"

#define ENV_VAR_USER "USER"
#define ENV_VAR_HOME "HOME"

#define FILE_HH_BLACKLIST ".hh_blacklist"

typedef struct {
    bool useFile;
    bool isLoaded;
    bool isDefault;
    HashSet *set;
} Blacklist;

void blacklist_init(Blacklist *blacklist);
void blacklist_load(Blacklist *blacklist);
bool blacklist_in(Blacklist *blacklist, char *cmd);
void blacklist_dump(Blacklist *blacklist);
void blacklist_destroy(Blacklist *blacklist);

#endif
