/*
 ============================================================================
 Name        : hashset.c
 Author      : martin.dvorak@midforger.com
 Copyright   : Apache 2.0
 Description : Hash set
 ============================================================================
*/

#include "include/hashset.h"

unsigned int hashmap_hash(const char *str) {
    int i;
    unsigned int result = 5381;

    for( i = 0; str[ i ] != '\0'; i++ )
        result = result * 33 + str[ i ];

    result = result ^ (result >> 16);

    return result % HASH_MAP_SIZE;
}

void hashset_init(HashSet * hs) {
    int i;
    hs->currentSize = 0;
    for( i = 0; i < HASH_MAP_SIZE; i++ ) {
        hs->lists[ i ] = NULL;
    }
}

void *hashset_get(const HashSet * hs, const char *key) {
    int listNum = hashmap_hash( key );
    struct HashSetNode *ptr = hs->lists[ listNum ];

    while( ptr != NULL && strcmp( ptr->key, key ) != 0 )
        ptr = ptr->next;

    return (ptr != NULL? ptr->value : NULL);
}

int hashset_contains(const HashSet * hs, const char *key) {
	return (hashset_get(hs, key) != NULL);
}

int hashset_put(HashSet * hs, const char *key, void *value) {
    struct HashSetNode *newNode;
    int listNum;

    if( hashset_get( hs, key ) )
        return 0;

    listNum = hashmap_hash( key );


    newNode = (struct HashSetNode *) malloc( sizeof ( struct HashSetNode ) );
    if( newNode == NULL ) {
        fprintf( stderr, "Error allocating node" );
        return 0;
    }

    newNode->key = malloc(strlen(key)+1);
    strcpy(newNode->key, key);
    newNode->value = value;
    newNode->next = hs->lists[ listNum ];
    hs->lists[ listNum ] = newNode;
    hs->currentSize++;

    return 1;
}

int hashset_add(HashSet * hs, const char *key) {
	return hashset_put(hs, key, "nil");
}

int hashset_size(const HashSet * hs) {
    return hs->currentSize;
}

void hashset_stat( const HashSet * hs ) {
    int i;
    struct HashSetNode *ptr;

    for( i = 0; i < HASH_MAP_SIZE; i++ )
        for( ptr = hs->lists[ i ]; ptr != NULL; ptr = ptr->next )
            printf( "%s\n", ptr->key );
}
