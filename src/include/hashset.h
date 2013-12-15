/*
 ============================================================================
 Name        : hashset.h
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Hash set
 ============================================================================
*/

#ifndef _HASHSET_H
#define _HASHSET_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TABLE_SIZE 10007

struct HashNode {
    char *key;
    struct HashNode *next;
};

typedef struct {
    struct HashNode * lists[TABLE_SIZE];
    int currentSize;
} HashSet;

void hashset_init( HashSet *hs );
int hashset_contains( const HashSet *hs, const char *key );
int hashset_add( HashSet *hs, const char *key );
int hashset_remove( HashSet *hs, const char *key );
int hashset_size( const HashSet *hs );
void hashset_print( const HashSet *hs );

#endif
