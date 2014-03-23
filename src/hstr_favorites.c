/*
 ============================================================================
 Name        : hstr_favorites.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Favorite commands.
 ============================================================================
*/

#include <stdbool.h>
#include <string.h>
#include "include/hstr_favorites.h"

#define FAVORITE_SEGMENT_SIZE 10

void favorites_init(FavoriteItems *favorites)
{
	favorites->items=NULL;
	favorites->count=0;
}

void favorites_load(FavoriteItems *favorites)
{
	// TODO fake initialization instead of .hhrc load
	favorites->count=2;
	favorites->items=malloc(sizeof(char *)*favorites->count);
	favorites->items[0]=malloc(2);
	strcpy(favorites->items[0],"a");
	favorites->items[1]=malloc(2);
	strcpy(favorites->items[1],"b");
}

void favorites_add(FavoriteItems *favorites, char *newFavorite)
{
	favorites->items=realloc(favorites->items, sizeof(char *) * ++favorites->count);
	favorites->items[favorites->count-1]=newFavorite;

	favorites_choose(favorites, newFavorite);
	favorites_save(favorites);
}

void favorites_choose(FavoriteItems *favorites, char *choice)
{
	if(favorites->count && choice) {
		int i;
		char *b=0, *next;
		for(i=0; i<favorites->count; i++) {
			if(!strcmp(favorites->items[i],choice)) {
				favorites->items[0]=favorites->items[i];
				if(b) {
					favorites->items[i]=b;
				}
				return;
			}
			next=favorites->items[i];
			favorites->items[i]=b;
			b=next;
		}
	}
	favorites_save(favorites);
}

void favorites_remove(FavoriteItems *favorites, char *almostDead)
{
	// TODO: keept slot you have, just change count

	favorites_save(favorites);
}

void favorites_save(FavoriteItems *favorites)
{
}

void favorites_destroy(FavoriteItems *favorites)
{
	if(favorites) {
		int i;
		for(i=0; i<favorites->count; i++) {
			free(favorites->items[i]);
		}
		free(favorites);
	}
}
