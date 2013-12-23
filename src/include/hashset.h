/*
 ============================================================================
 Name        : hashset.h
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Hash set
 ============================================================================
*/

#ifndef _HASHSET_H_
#define _HASHSET_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HASH_MAP_SIZE 10007

struct HashSetNode {
    char *key;
    void *value;
    struct HashSetNode *next;
};

typedef struct {
    struct HashSetNode * lists[HASH_MAP_SIZE];
    int currentSize;
} HashSet;

void hashset_init(HashSet *hs);

int hashset_contains(const HashSet *hs, const char *key);
int hashset_add(HashSet *hs, const char *key);
int hashset_size(const HashSet *hs);

void *hashset_get(const HashSet *hm, const char *key);
int hashset_put(HashSet *hm, const char *key, void *value);
int hashset_remove(HashSet *hm, const char *key);

void hashset_stat(const HashSet *hm);

#endif /* HASHSET_H_ */
