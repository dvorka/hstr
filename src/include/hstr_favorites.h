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

#define ENV_VAR_USER "USER"
#define ENV_VAR_HOME "HOME"

#define FILE_HH_RC ".hhrc"

typedef struct {
	char **items;
	unsigned count;
} FavoriteItems;

void favorites_init(FavoriteItems *favorites);
void favorites_load(FavoriteItems *favorites);
void favorites_add(FavoriteItems *favorites, char *favorite);
void favorites_choose(FavoriteItems *favorites, char *choice);
void favorites_remove(FavoriteItems *favorites, char *almostDead);
void favorites_save(FavoriteItems *favorites);
void favorites_destroy(FavoriteItems *favorites);

#endif
