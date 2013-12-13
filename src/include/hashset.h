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

struct HashSetStruct {
    struct HashNode * lists[TABLE_SIZE];
    int currentSize;
};

typedef struct HashSetStruct HashSet;

void hashset_init( HashSet * hs );
int hashset_contains( const HashSet * hs, const char *key );
int hashset_add( HashSet * hs, const char *key );
int hashset_remove( HashSet * hs, const char *key );
int hashset_size( const HashSet * hs );
void hashset_print( const HashSet * hs );

#endif
