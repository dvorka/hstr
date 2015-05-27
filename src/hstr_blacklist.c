/*
 ============================================================================
 Name        : hstr_blacklist.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Commands to be skipped from history.
 ============================================================================
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "include/hstr_blacklist.h"
#include "include/hstr_utils.h"

static const char *defaultCommandBlacklist[] = {
        "ls", "pwd", "cd", "cd ..", "hh", "mc",
        "ls ", "pwd ", "cd ", "cd .. ", "hh ", "mc "
};

void blacklist_init(Blacklist *blacklist)
{
    blacklist->useFile=false;
    blacklist->isLoaded=false;
    blacklist->isDefault=false;
    blacklist->set=malloc(sizeof(HashSet));
    hashset_init(blacklist->set);
}

char* blacklist_get_filename()
{
    char *home = getenv(ENV_VAR_HOME);
    char *fileName = (char*) malloc(strlen(home) + 1 + strlen(FILE_HH_BLACKLIST) + 1);
    strcpy(fileName, home);
    strcat(fileName, "/");
    strcat(fileName, FILE_HH_BLACKLIST);
    return fileName;
}

void blacklist_load_default(Blacklist* blacklist) {
    blacklist->isLoaded = true;
    blacklist->isDefault = true;
    int i, length = sizeof(defaultCommandBlacklist) / sizeof(defaultCommandBlacklist[0]);
    for (i = 0; i < length; i++) {
        hashset_add(blacklist->set, defaultCommandBlacklist[i]);
    }
}

void blacklist_load(Blacklist *blacklist)
{
    if(!blacklist->isLoaded) {
        if(blacklist->useFile) {
            char* fileName = blacklist_get_filename();
            char *fileContent=NULL;
            if(access(fileName, F_OK) != -1) {
                long fileSize;

                FILE *file = fopen(fileName, "rb");
                fseek(file, 0, SEEK_END);
                fileSize = ftell(file);
                rewind(file);
                fileContent = malloc((fileSize + 1) * (sizeof(char)));
                if(fread(fileContent, sizeof(char), fileSize, file)==-1) {
                    exit(EXIT_FAILURE);
                }
                fclose(file);
                fileContent[fileSize] = 0;

                if(fileContent && strlen(fileContent)) {
                    char *p=strchr(fileContent,'\n');
                    while (p!=NULL) {
                        p=strchr(p+1,'\n');
                    }
                    char *pb=fileContent, *pe, *s;
                    pe=strchr(fileContent, '\n');
                    while(pe!=NULL) {
                        *pe=0;
                        if(!hashset_contains(blacklist->set,pb)) {
                            s=hstr_strdup(pb);
                            hashset_add(blacklist->set,s);
                        }
                        pb=pe+1;
                        pe=strchr(pb, '\n');
                    }
                    free(fileContent);
                }
            } else {
                // blacklist file not found > use default in-memory one (flushed on exit)
                blacklist_load_default(blacklist);
            }
            free(fileName);
        } else {
            // don't use file
            blacklist_load_default(blacklist);
        }
    }
}

bool blacklist_in(Blacklist *blacklist, char *cmd)
{
    return hashset_contains(blacklist->set, cmd);
}

void blacklist_dump(Blacklist *blacklist)
{
    if(blacklist) {
        int size=hashset_size(blacklist->set);
        if(size) {
            printf("Command blacklist (%d):\n",size);
            int i;
            char **keys=hashset_keys(blacklist->set);
            for(i=0; i<size; i++) {
                printf("  '%s'\n",keys[i]);
            }
            return;
        }
    }
    printf("Command blacklist is empty\n");
}

void blacklist_destroy(Blacklist *blacklist)
{
    if(blacklist) {
        if(blacklist->useFile) {
            char* fileName = blacklist_get_filename();
            int size=hashset_size(blacklist->set);
            if(size) {
                FILE *output_file = fopen(fileName, "wb");
                rewind(output_file);
                int i;
                char **keys=hashset_keys(blacklist->set);
                for(i=0; i<size; i++) {
                    if(fwrite(keys[i], sizeof(char), strlen(keys[i]), output_file)==-1) {
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
        hashset_destroy(blacklist->set, false);
        free(blacklist->set);
        free(blacklist);
    }
}
