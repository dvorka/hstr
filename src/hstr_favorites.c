/*
 ============================================================================
 Name        : hstr_favorites.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Favorite commands.
 ============================================================================
*/

#include "include/hstr_favorites.h"

FavoriteItems *favorites;

FavoriteItems *favorites_init() {
	favorites=malloc(sizeof(FavoriteItems));
	return favorites;
}

FavoriteItems *favorites_load() {
	// TODO fake initialization
	favorites->count=3;
	favorites->items=malloc(sizeof(char *)*favorites->count);
	favorites->items[0]="a";
	favorites->items[1]="b";
	favorites->items[2]="c";

	return favorites;
}

void favorites_add(char *newFavorite) {
	favorites->items=realloc(favorites->items, sizeof(char *)*favorites->count);
	favorites->items[favorites->count++]=newFavorite;
}

void favorites_save() {
}

void favorites_close() {
	if(favorites) {
		if(favorites->count) {
			int i;
			for(i=0; i<favorites->count; i++) {
				free(favorites->items[i]);
			}
		}
	}
}
