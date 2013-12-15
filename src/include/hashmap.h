#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HASH_MAP_SIZE 10007

struct HashMapNode {
    char *key;
    void *value;
    struct HashMapNode *next;
};

typedef struct {
    struct HashMapNode * lists[HASH_MAP_SIZE];
    int currentSize;
} HashMap;

void hashmap_init( HashMap *hm );
void *hashmap_get( const HashMap *hm, const char *key );
int hashmap_put( HashMap *hm, const char *key, void *value );
int hashmap_size( const HashMap *hm );
void hashmap_print( const HashMap *hm );

#endif /* HASHMAP_H_ */
