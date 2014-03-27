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

	lazy loading (boolean indicator to FavoriteItems)

	// TODO load from file

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
	} else {
		fprintf(stderr,"\nHistory file not found: %s\n",fileName);
	}

	if(file_contents) {
		split & process & initilize favorites
	}
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
	create/update history file

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
