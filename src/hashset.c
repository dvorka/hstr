#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/hashset.h"

unsigned int hash( const char *str ) {
    int i;
    unsigned int result = 0;

    for( i = 0; str[ i ] != '\0'; i++ )
        result = result * 31 + str[ i ];

    return result % TABLE_SIZE;
}

void hs_init( HashSet * hs ) {
    int i;
    hs->currentSize = 0;
    for( i = 0; i < TABLE_SIZE; i++ )
        hs->lists[ i ] = NULL;
}

int hs_contains( const HashSet * hs, const char *key ) {
    int listNum = hash( key );
    struct HashNode *ptr = hs->lists[ listNum ];

    while( ptr != NULL && strcmp( ptr->key, key ) != 0 )
        ptr = ptr->next;

    return ptr != NULL;
}

int hs_add( HashSet * hs, const char *key ) {
    struct HashNode *newNode;
    int listNum;

    if( hs_contains( hs, key ) )
        return 0;

    listNum = hash( key );


    newNode = (struct HashNode *) malloc( sizeof ( struct HashNode ) );
    if( newNode == NULL ) {
        fprintf( stderr, "Error allocating node" );
        return 0;
    }

    newNode->key = strdup( key );
    newNode->next = hs->lists[ listNum ];
    hs->lists[ listNum ] = newNode;
    hs->currentSize++;

    return 1;
}

int hs_remove( HashSet * hs, const char *key ) {
    struct HashNode *curr;
    struct HashNode *prev = NULL;
    int listNum;

    if( !hs_contains( hs, key ) )
        return 0;

    listNum = hash( key );
    curr = hs->lists[ listNum ];
    while( strcmp( curr->key, key ) != 0 ) {
        prev = curr;
        curr = curr->next;
    }

    if( prev == NULL ) // first item
        hs->lists[ listNum ] = curr->next;
    else      // middle of list
        prev->next = curr->next;

    // cleanup node curr
    free( curr->key );
    free( curr );

    hs->currentSize--;
    return 1;
}

int hs_size(const HashSet * hs) {
    return hs->currentSize;
}

void hs_print( const HashSet * hs ) {
    int i;
    struct HashNode *ptr;

    for( i = 0; i < TABLE_SIZE; i++ )
        for( ptr = hs->lists[ i ]; ptr != NULL; ptr = ptr->next )
            printf( "%s\n", ptr->key );
}
