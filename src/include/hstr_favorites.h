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

#define HH_RC_FILE ".hhrc"

typedef struct {
	char **items;
	unsigned count;
} FavoriteItems;

FavoriteItems *favorites_init();
FavoriteItems *favorites_load();
void favorites_add();
void favorites_save();
void favorites_close();

#endif
