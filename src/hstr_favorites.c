/*
 ============================================================================
 Name        : hstr_favorites.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Favorite commands.
 ============================================================================
*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "include/hstr_favorites.h"

#define FAVORITE_SEGMENT_SIZE 10

void favorites_init(FavoriteItems *favorites)
{
	favorites->items=NULL;
	favorites->count=0;
	favorites->loaded=false;
}

void favorites_get(FavoriteItems *favorites)
{
	if(!favorites->loaded) {
		char *home = getenv(ENV_VAR_HOME);
		char *fileName=(char*)malloc(strlen(home)+1+strlen(FILE_HH_RC)+1);
		strcpy(fileName,home);
		strcat(fileName,"/");
		strcat(fileName,FILE_HH_RC);

		char *file_contents=NULL;
		if(access(fileName, F_OK) != -1) {
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

			if(file_contents && strlen(file_contents)) {
				favorites->count = 0;
				char *p=strchr(file_contents,'\n');
				while (p!=NULL) {
					favorites->count++;
					p=strchr(p+1,'\n');
				}

			    favorites->items = malloc(sizeof(char*) * favorites->count);
				int i = 0;
				char *pb=file_contents, *pe;
				pe=strchr(file_contents, '\n');
				while(pe!=NULL) {
					favorites->items[i]=pb;
					*pe=0;
					favorites->items[i]=strdup(pb);
					pb=pe+1;
					pe=strchr(pb, '\n');
					i++;
				}
				free(file_contents);
			}
		} else {
			// favorites file not found > favorites don't exist yet
			favorites->loaded=true;
			return;
		}
	}
}

void favorites_add(FavoriteItems *favorites, char *newFavorite)
{
	if(favorites->count) {
		favorites->items=realloc(favorites->items, sizeof(char *) * ++favorites->count);
		favorites->items[favorites->count-1]=strdup(newFavorite);
		favorites_choose(favorites, newFavorite);
	} else {
		favorites->items=malloc(sizeof(char*));
		favorites->items[0]=strdup(newFavorite);
		favorites->count=1;
	}

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
	// TODO: keep slot you have, just change count

	favorites_save(favorites);
}

void favorites_save(FavoriteItems *favorites)
{
	if(favorites->count) {
		// TODO shrink file and rewrite it (can be shorter)
	}
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
