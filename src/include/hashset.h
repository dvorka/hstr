#ifndef HASHSET_H
#define HASHSET_H 1

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


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

void hs_init( HashSet * hs );
int hs_contains( const HashSet * hs, const char *key );
int hs_add( HashSet * hs, const char *key );
int hs_remove( HashSet * hs, const char *key );
int hs_size( const HashSet * hs );
void hs_print( const HashSet * hs );


#ifdef __cplusplus
}
#endif

#endif
