/*
 ============================================================================
 Name        : hstr_favorites.h
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Favorite commands.
 ============================================================================
*/

#ifndef _HSTR_FAVORITES_H_
#define _HSTR_FAVORITES_H_

#include <stdlib.h>
#include <stdbool.h>
#include "hashset.h"

#define ENV_VAR_USER "USER"
#define ENV_VAR_HOME "HOME"

#define FILE_HH_FAVORITES ".hh_favorites"

typedef struct {
    char **items;
    unsigned count;
    bool loaded;
    HashSet *set;
} FavoriteItems;

void favorites_init(FavoriteItems *favorites);
void favorites_get(FavoriteItems *favorites);
void favorites_add(FavoriteItems *favorites, char *favorite);
void favorites_choose(FavoriteItems *favorites, char *choice);
bool favorites_remove(FavoriteItems *favorites, char *almostDead);
void favorites_destroy(FavoriteItems *favorites);

#endif
