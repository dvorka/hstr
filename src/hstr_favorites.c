/*
 ============================================================================
 Name        : hstr_favorites.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Favorite commands.
 ============================================================================
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "include/hstr_favorites.h"
#include "include/hstr_utils.h"

#define FAVORITE_SEGMENT_SIZE 10

void favorites_init(FavoriteItems *favorites)
{
    favorites->items=NULL;
    favorites->count=0;
    favorites->loaded=false;
    favorites->set=malloc(sizeof(HashSet));
    hashset_init(favorites->set);
}

void favorites_show(FavoriteItems *favorites)
{
    printf("\n\nFavorites (%d):", favorites->count);
    if(favorites->count) {
        int i;
        for(i=0;i<favorites->count;i++) {
            printf("\n%s",favorites->items[i]);
        }
    }
    printf("\n");
}

char* favorites_get_filename()
{
    char *home = getenv(ENV_VAR_HOME);
    char *fileName = (char*) malloc(strlen(home) + 1 + strlen(FILE_HH_FAVORITES) + 1);
    strcpy(fileName, home);
    strcat(fileName, "/");
    strcat(fileName, FILE_HH_FAVORITES);
    return fileName;
}

void favorites_get(FavoriteItems *favorites)
{
    if(!favorites->loaded) {
        char* fileName = favorites_get_filename();
        char *fileContent=NULL;
        if(access(fileName, F_OK) != -1) {
            long inputFileSize;

            FILE *inputFile = fopen(fileName, "rb");
            fseek(inputFile, 0, SEEK_END);
            inputFileSize = ftell(inputFile);
            rewind(inputFile);
            fileContent = malloc((inputFileSize + 1) * (sizeof(char)));
            if(fread(fileContent, sizeof(char), inputFileSize, inputFile)==-1) {
                exit(EXIT_FAILURE);
            }
            fclose(inputFile);
            fileContent[inputFileSize] = 0;

            if(fileContent && strlen(fileContent)) {
                favorites->count = 0;
                char *p=strchr(fileContent,'\n');
                while (p!=NULL) {
                    favorites->count++;
                    p=strchr(p+1,'\n');
                }

                favorites->items = malloc(sizeof(char*) * favorites->count);
                favorites->count = 0;
                char *pb=fileContent, *pe, *s;
                pe=strchr(fileContent, '\n');
                while(pe!=NULL) {
                    *pe=0;
                    if(!hashset_contains(favorites->set,pb)) {
                        s=hstr_strdup(pb);
                        favorites->items[favorites->count++]=s;
                        hashset_add(favorites->set,s);
                    }
                    pb=pe+1;
                    pe=strchr(pb, '\n');
                }
                free(fileContent);
            }
        } else {
            // favorites file not found > favorites don't exist yet
            favorites->loaded=true;
            return;
        }
        free(fileName);
    }
}

void favorites_save(FavoriteItems *favorites)
{
    char *fileName=favorites_get_filename();

    if(favorites->count) {
        FILE *output_file = fopen(fileName, "wb");
        rewind(output_file);
        int i;
        for(i=0; i<favorites->count; i++) {
            if(fwrite(favorites->items[i], sizeof(char), strlen(favorites->items[i]), output_file)==-1) {
                exit(EXIT_FAILURE);
            }
            if(fwrite("\n", sizeof(char), strlen("\n"), output_file)==-1) {
                exit(EXIT_FAILURE);
            }
        }
        fclose(output_file);
    } else {
        if(access(fileName, F_OK) != -1) {
            FILE *output_file = fopen(fileName, "wb");
            fclose(output_file);
        }
    }
    free(fileName);
}

void favorites_add(FavoriteItems *favorites, char *newFavorite)
{
    if(favorites->count) {
        favorites->items=realloc(favorites->items, sizeof(char *) * ++favorites->count);
        favorites->items[favorites->count-1]=hstr_strdup(newFavorite);
        favorites_choose(favorites, newFavorite);
    } else {
        favorites->items=malloc(sizeof(char*));
        favorites->items[0]=hstr_strdup(newFavorite);
        favorites->count=1;
    }

    favorites_save(favorites);
    hashset_add(favorites->set, newFavorite);
}

void favorites_choose(FavoriteItems *favorites, char *choice)
{
    if(favorites->count && choice) {
        int r;
        char *b=NULL, *next;
        for(r=0; r<favorites->count; r++) {
            if(!strcmp(favorites->items[r],choice)) {
                favorites->items[0]=favorites->items[r];
                if(b) {
                    favorites->items[r]=b;
                }
                favorites_save(favorites);
                return;
            }
            next=favorites->items[r];
            favorites->items[r]=b;
            b=next;
        }
    }
}

bool favorites_remove(FavoriteItems *favorites, char *almostDead)
{
    if(favorites->count) {
        int r, w, count;
        count=favorites->count;
        for(r=0, w=0; r<count; r++) {
            if(!strcmp(favorites->items[r], almostDead)) {
                favorites->count--;
            } else {
                if(w<r) {
                    favorites->items[w]=favorites->items[r];
                }
                w++;
            }
        }
        favorites_save(favorites);
        // kept in set and removed/freed on favs destroy
        return true;
    } else {
        return false;
    }
}

void favorites_destroy(FavoriteItems *favorites)
{
    if(favorites) {
        // TODO hashset destroys keys - no need to destroy items!
        int i;
        for(i=0; i<favorites->count; i++) {
            free(favorites->items[i]);
        }
        // TODO hashset destroys keys - no need to destroy items!
        hashset_destroy(favorites->set, false);
        free(favorites->set);
        free(favorites);
    }
}
