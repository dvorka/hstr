/*
 ============================================================================
 Name        : hstr_favorites.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Favorite commands.
 ============================================================================
*/

#include "include/hstr_favorites.h"

#define FAVORITE_SEGMENT_SIZE 10

void favorites_init(FavoriteItems *favorites) {
	favorites->items=NULL;
	favorites->count=0;
}

void favorites_load(FavoriteItems *favorites) {
	// TODO fake initialization instead of .hhrc load
	favorites->count=3;
	favorites->items=malloc(sizeof(char *)*favorites->count);
	favorites->items[0]="a";
	favorites->items[1]="b";
	favorites->items[2]="c";
}

void favorites_add(FavoriteItems *favorites, char *newFavorite) {
	favorites->items=realloc(favorites->items, sizeof(char *)*favorites->count);
	favorites->items[favorites->count++]=newFavorite;
}

void favorites_save(FavoriteItems *favorites) {
}
