/*
 hashset.c      hash set implementation

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

#include "include/hashset.h"

unsigned int hashmap_hash(const char *str)
{
    int i;
    unsigned int result=5381;

    for(i=0; str[i]!='\0'; i++) {
        result=result*33+str[i];
    }
    result = result^(result>>16);

    return result%HASH_MAP_SIZE;
}

void hashset_init(HashSet * hs)
{
    int i;
    hs->currentSize = 0;
    for(i = 0; i<HASH_MAP_SIZE; i++) {
        hs->lists[i] = NULL;
    }
}

void *hashset_get(const HashSet * hs, const char *key)
{
    int listNum = hashmap_hash( key );
    struct HashSetNode *ptr = hs->lists[ listNum ];

    while(ptr != NULL && strcmp(ptr->key, key)!= 0) {
        ptr = ptr->next;
    }

    return (ptr!=NULL?ptr->value:NULL);
}

int hashset_contains(const HashSet * hs, const char *key)
{
    return (hashset_get(hs, key) != NULL);
}

// key is COPIED, value is REFERENCED
int hashset_put(HashSet *hs, const char* key, void* value)
{
    if(hashset_get(hs, key)) {
        return 0;
    } else {
        int listNum = hashmap_hash( key );
        struct HashSetNode* newNode=(struct HashSetNode *)malloc(sizeof(struct HashSetNode));
        if(newNode == NULL) {
            fprintf(stderr,"Unable to allocate hashset entry!");
            return 0;
        }

        newNode->key=malloc(strlen(key)+1);
        strcpy(newNode->key, key);
        newNode->value=value;
        newNode->next=hs->lists[listNum];
        hs->lists[listNum]=newNode;
        hs->currentSize++;

        return 1;
    }
}

int hashset_add(HashSet * hs, const char *key)
{
    return hashset_put(hs, key, "nil");
}

int hashset_size(const HashSet *hs)
{
    return hs->currentSize;
}

void hashset_stat(const HashSet *hs)
{
    int i;
    struct HashSetNode *ptr;
    for(i=0; i<HASH_MAP_SIZE; i++) {
        for(ptr=hs->lists[i]; ptr!=NULL; ptr=ptr->next) {
            printf("%s\n",ptr->key);
        }
    }
}

char** hashset_keys(const HashSet *hs)
{
    if(hs->currentSize) {
        char **result=malloc(sizeof(char*) * hs->currentSize);
        int i=0, j=0;
        struct HashSetNode *p;
        for(i=0; i<HASH_MAP_SIZE; i++) {
            p=hs->lists[i];
            while(p && p->next) {
                result[j++]=hstr_strdup(p->key);
                p=p->next;
            }
            if(p) {
                result[j++]=hstr_strdup(p->key);
            }
        }
        return result;
    } else {
        return NULL;
    }
}

void hashset_destroy(HashSet *hs, const bool freeValues)
{
    // only hashset keys (and possibly values) are freed - caller must free hashset itself
    if(hs && hs->currentSize) {
        int i=0;
        struct HashSetNode *p, *pp;
        for(i=0; i<HASH_MAP_SIZE; i++) {
            p=hs->lists[i];
            while(p && p->next) {
                if(p->key) {
                    free(p->key);
                    if(freeValues && p->value) free(p->value);
                }
                pp=p;
                p=p->next;
                free(pp);
            }
            if(p && p->key) {
                free(p->key);
                if(freeValues && p->value) free(p->value);
                free(p);
            }
        }
    }
}
