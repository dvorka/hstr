#include "include/hashmap.h"

unsigned int hashmap_hash( const char *str ) {
    int i;
    unsigned int result = 0;

    for( i = 0; str[ i ] != '\0'; i++ )
        result = result * 31 + str[ i ];

    return result % HASH_MAP_SIZE;
}

void hashmap_init( HashMap * hs ) {
    int i;
    hs->currentSize = 0;
    for( i = 0; i < HASH_MAP_SIZE; i++ ) {
        hs->lists[ i ] = NULL;
    }
}

void *hashmap_get( const HashMap * hs, const char *key ) {
    int listNum = hashmap_hash( key );
    struct HashMapNode *ptr = hs->lists[ listNum ];

    while( ptr != NULL && strcmp( ptr->key, key ) != 0 )
        ptr = ptr->next;

    return (ptr != NULL? ptr->value : NULL);
}

int hashmap_put( HashMap * hs, const char *key, void *value) {
    struct HashMapNode *newNode;
    int listNum;

    if( hashmap_get( hs, key ) )
        return 0;

    listNum = hashmap_hash( key );


    newNode = (struct HashMapNode *) malloc( sizeof ( struct HashMapNode ) );
    if( newNode == NULL ) {
        fprintf( stderr, "Error allocating node" );
        return 0;
    }

    newNode->key = strdup( key );
    newNode->value = value;
    newNode->next = hs->lists[ listNum ];
    hs->lists[ listNum ] = newNode;
    hs->currentSize++;

    return 1;
}

int hashmap_size(const HashMap * hs) {
    return hs->currentSize;
}

void hashmap_print( const HashMap * hs ) {
    int i;
    struct HashMapNode *ptr;

    for( i = 0; i < HASH_MAP_SIZE; i++ )
        for( ptr = hs->lists[ i ]; ptr != NULL; ptr = ptr->next )
            printf( "%s\n", ptr->key );
}
