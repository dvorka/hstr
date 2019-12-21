/*
 hstr_blacklist.c       commands to be skipped from history

 Copyright (C) 2014-2020  Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "include/hstr_blacklist.h"

static const char *defaultCommandBlacklist[] = {
        "ls", "pwd", "cd", "cd ..", "hstr", "mc",
        "ls ", "pwd ", "cd ", "cd .. ", "hstr ", "mc " // trailing space
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
    char *fileName = (char*) malloc(strlen(home) + 1 + strlen(FILE_HSTR_BLACKLIST) + 1);
    strcpy(fileName, home);
    strcat(fileName, "/");
    strcat(fileName, FILE_HSTR_BLACKLIST);
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
                if(!fread(fileContent, sizeof(char), fileSize, file)) {
                    if(ferror(file)) {
                        exit(EXIT_FAILURE);
                    }
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
                free(keys[i]);
            }
            if(keys) {
                free(keys);
            }
            return;
        }
    }
    printf("Command blacklist is empty\n");
}

void blacklist_destroy(Blacklist *blacklist, bool freeBlacklist)
{
    if(blacklist) {
        if(blacklist->useFile) {
            char* fileName = blacklist_get_filename();
            int size=hashset_size(blacklist->set);
            if(size) {
                FILE *outputFile = fopen(fileName, "wb");
                rewind(outputFile);
                int i;
                char **keys=hashset_keys(blacklist->set);
                for(i=0; i<size; i++) {
                    if(!fwrite(keys[i], sizeof(char), strlen(keys[i]), outputFile)) {
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
                    FILE *outputFile = fopen(fileName, "wb");
                    fclose(outputFile);
                }
            }
            free(fileName);
        }
        hashset_destroy(blacklist->set, false);
        free(blacklist->set);
        if(freeBlacklist) {
            free(blacklist);
        }
    }
}
