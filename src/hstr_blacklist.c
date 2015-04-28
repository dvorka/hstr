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

static const char *implicitCommandBlacklist[] = {
        "ls", "pwd", "cd", "cd ..", "hh", "mc",
        "ls ", "pwd ", "cd ", "cd .. ", "hh ", "mc "
};

void blacklist_init(Blacklist *blacklist)
{
    blacklist->loaded=false;
    blacklist->implicit=false;
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

void blacklist_get(Blacklist *blacklist)
{
    if(!blacklist->loaded) {
        char* fileName = favorites_get_filename();
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
                char *p=strchr(file_contents,'\n');
                while (p!=NULL) {
                    p=strchr(p+1,'\n');
                }
                char *pb=file_contents, *pe, *s;
                pe=strchr(file_contents, '\n');
                while(pe!=NULL) {
                    *pe=0;
                    if(!hashset_contains(blacklist->set,pb)) {
                        s=hstr_strdup(pb);
                        hashset_add(blacklist->set,s);
                    }
                    pb=pe+1;
                    pe=strchr(pb, '\n');
                }
                free(file_contents);
            }
        } else {
            // blacklist not found - use default one (flushed on exit)
            blacklist->loaded=true;
            blacklist->implicit=true;
            int i, length=sizeof(implicitCommandBlacklist)/sizeof(implicitCommandBlacklist[0]);
            for(i=0; i<length; i++) {
                hashset_add(blacklist->set, implicitCommandBlacklist[i]);
            }
        }
        free(fileName);
    }
}

bool blacklist_in(Blacklist *blacklist, char *cmd)
{
    return hashset_contains(blacklist->set, cmd);
}

void blacklist_destroy(Blacklist *blacklist)
{
    if(blacklist) {
        if(blacklist->implicit) {
            // TODO save blacklist to file if implicit
        }
        hashset_destroy(blacklist->set, false);
        free(blacklist->set);
        free(blacklist);
    }
}
