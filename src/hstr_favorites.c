/*
 hstr_favorites.c       favorite commands

 Copyright (C) 2014-2026 Martin Dvorak <martin.dvorak@mindforger.com>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include "include/hstr_favorites.h"

#define FAVORITE_SEGMENT_SIZE 10

void favorites_init(FavoriteItems* favorites)
{
    favorites->items=NULL;
    favorites->count=0;
    favorites->loaded=false;
    favorites->reorderOnChoice=true;
    favorites->skipComments=false;
    favorites->set=malloc(sizeof(HashSet));
    hashset_init(favorites->set);
}

void favorites_show(FavoriteItems *favorites)
{
    printf("\n\nFavorites (%d):", favorites->count);
    if(favorites->count) {
        unsigned i;
        for(i=0; i<favorites->count; i++) {
            printf("\n%s",favorites->items[i]);
        }
    }
    printf("\n");
}

char* favorites_get_filename(void)
{
    char* home = getenv(ENV_VAR_HOME);
    char* fileName = (char*) malloc(strlen(home) + 1 + strlen(FILE_HSTR_FAVORITES) + 1);
    strcpy(fileName, home);
    strcat(fileName, "/");
    strcat(fileName, FILE_HSTR_FAVORITES);
    return fileName;
}

void favorites_get(FavoriteItems* favorites)
{
    if(!favorites->loaded) {
        char* fileName = favorites_get_filename();
        char* fileContent = NULL;
        if(access(fileName, F_OK) != -1) {
            long inputFileSize;

            FILE* inputFile = fopen(fileName, "rb");
            fseek(inputFile, 0, SEEK_END);
            inputFileSize = ftell(inputFile);
            rewind(inputFile);
            fileContent = malloc((inputFileSize + 1) * (sizeof(char)));
            if(!fread(fileContent, sizeof(char), inputFileSize, inputFile)) {
                if(ferror(inputFile)) {
                    exit(EXIT_FAILURE);
                }
            }
            fclose(inputFile);
            fileContent[inputFileSize] = 0;

            if(fileContent && strlen(fileContent)) {
                favorites->count = 0;
                char* p=strchr(fileContent,'\n');
                while (p!=NULL) {
                    favorites->count++;
                    p=strchr(p+1,'\n');
                }

                favorites->items = malloc(sizeof(char*) * favorites->count);
                favorites->count = 0;
                char* pb=fileContent, *pe, *s;
                pe=strchr(fileContent, '\n');
                while(pe!=NULL) {
                    *pe=0;
                    if(!hashset_contains(favorites->set,pb)) {
                        if(!favorites->skipComments || !(strlen(pb) && pb[0]=='#')) {
                            s=hstr_strdup(pb);
                            favorites->items[favorites->count++]=s;
                            hashset_add(favorites->set,s);
                        }
                    }
                    pb=pe+1;
                    pe=strchr(pb, '\n');
                }
                free(fileContent);
            }
        } else {
            // favorites file not found > favorites don't exist yet
            favorites->loaded=true;
        }
        free(fileName);
    }
}

void favorites_save(FavoriteItems* favorites)
{
    char* fileName=favorites_get_filename();

    if(favorites->count) {
        FILE* outputFile = fopen(fileName, "wb");
        rewind(outputFile);
        unsigned i;
        for(i=0; i<favorites->count; i++) {
            if(!fwrite(favorites->items[i], sizeof(char), strlen(favorites->items[i]), outputFile)) {
                if(ferror(outputFile)) {
                    exit(EXIT_FAILURE);
                }
            }
            if(!fwrite("\n", sizeof(char), strlen("\n"), outputFile)) {
                if(ferror(outputFile)) {
                    exit(EXIT_FAILURE);
                }
            }
        }
        fclose(outputFile);
    } else {
        if(access(fileName, F_OK) != -1) {
            FILE *output_file = fopen(fileName, "wb");
            fclose(output_file);
        }
    }
    free(fileName);
}

static int favorites_index_of(FavoriteItems* favorites, const char* favorite)
{
    unsigned i;
    for(i=0; i<favorites->count; i++) {
        if(!strcmp(favorites->items[i], favorite)) {
            return (int)i;
        }
    }
    return -1;
}

// moves the item to the head and keeps the order of the other items
static void favorites_move_to_head(FavoriteItems* favorites, unsigned index)
{
    char* item=favorites->items[index];
    memmove(&favorites->items[1], &favorites->items[0], sizeof(char*) * index);
    favorites->items[0]=item;
}

void favorites_add(FavoriteItems* favorites, char* newFavorite)
{
    if(!newFavorite) {
        return;
    }
    if(favorites_index_of(favorites, newFavorite) >= 0) {
        // already a favorite - adding it again is choosing it
        favorites_choose(favorites, newFavorite);
        return;
    }

    char* item=hstr_strdup(newFavorite);
    char** items=item ? realloc(favorites->items, sizeof(char*) * (favorites->count+1)) : NULL;
    if(!items) {
        // out of memory - favorites are left intact
        free(item);
        return;
    }
    favorites->items=items;
    favorites->items[favorites->count++]=item;
    if(favorites->reorderOnChoice) {
        favorites_move_to_head(favorites, favorites->count-1);
    }

    favorites_save(favorites);
    hashset_add(favorites->set, newFavorite);
}

void favorites_choose(FavoriteItems* favorites, char* choice)
{
    if(!favorites->reorderOnChoice || !choice) {
        return;
    }
    int index=favorites_index_of(favorites, choice);
    if(index > 0) {
        favorites_move_to_head(favorites, (unsigned)index);
        favorites_save(favorites);
    }
}

// favorites are unique (deduplicated on load and add) - at most one item is removed
bool favorites_remove(FavoriteItems* favorites, char* almostDead)
{
    int index=almostDead ? favorites_index_of(favorites, almostDead) : -1;
    if(index < 0) {
        return false;
    }

    free(favorites->items[index]);
    favorites->count--;
    memmove(&favorites->items[index], &favorites->items[index+1], sizeof(char*) * (favorites->count-index));
    favorites_save(favorites);
    return true;
}

void favorites_destroy(FavoriteItems* favorites)
{
    if(favorites) {
        // TODO hashset destroys keys - no need to destroy items!
        unsigned i;
        for(i=0; i<favorites->count; i++) {
            free(favorites->items[i]);
        }
        free(favorites->items);
        hashset_destroy(favorites->set, false);
        free(favorites->set);
        free(favorites);
    }
}
