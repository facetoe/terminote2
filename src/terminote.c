/*
 ============================================================================
 Name        : listRewrite.c
 Author      : facetoe
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <getopt.h>

#include "helperFunctions.h"

#include "linkedList.h"
#include "options.h"
#include "defines.h"
#include "nonInteractive.h"

char *path = "/home/facetoe/.terminote.data";

OPTIONS *opts = NULL;

#define LINE_FIRST(msg) msg->first
#define LINE_LAST(msg) msg->last

LINE *line_getLineNode( MESSAGE *msg, int nodeNum ) {

    if ( !msg ) {
        fprintf( stderr, "Null pointer passed to line_getLineNode\n" );
        exit( 1 );
    }
    LINE *tmp = msg->first;
    if ( !tmp ) {
        fprintf( stderr, "Invalid LINE pointer passed to line_getLineNode\n" );
        exit( 1 );
    } else if ( nodeNum > msg->numLines ) {
        fprintf( stderr, "Invalid nodeNum passed to line_getLineNode\n" );
        return NULL;
    }

    for ( ; tmp->next && tmp->lNum < nodeNum; tmp = tmp->next ) {
        ;
    }
    return tmp;
}

void line_insertAfter( MESSAGE *msg, int nodeNum, char *str ) {

    LINE *oldLine = NULL;

    if ( ( oldLine = line_getLineNode( msg, nodeNum ) ) == NULL ) {
        fprintf( stderr, "Unable to retrieve nodeNum %d\n", nodeNum );
        return;
    } else {

        LINE *newLine = line_getLine();

        /* Add the new line */
        int strLen = strlen( str );
        newLine->currLine = malloc( strLen );
        memmove( newLine->currLine, str, strLen );
        newLine->currLine[strLen] = '\0';

        /* Update pointers */
        newLine->next = oldLine->next;
        oldLine->next = newLine;
        newLine->prev = oldLine;
        newLine->next->prev = newLine;

        /* Update statistics */
        newLine->lSize = strLen;
        msg->numChars += newLine->lSize + 1;
        msg->numLines++;
    }
}

void line_insertBefore( MESSAGE *msg, int nodeNum, char *str ) {
    LINE *oldLine, *newLine;
    oldLine = NULL;
    if ( ( oldLine = line_getLineNode( msg, nodeNum ) ) == NULL ) {
        fprintf( stderr, "Unable to retrieve nodeNum %d\n", nodeNum );
        return;
    } else {
        newLine = line_getLine();

        /* Add the new line */
        int strLen = strlen( str );
        newLine->currLine = malloc( strLen );
        memmove( newLine->currLine, str, strLen );
        newLine->currLine[strLen] = '\0';

        /* Update pointers */
        newLine->prev = oldLine->prev;
        newLine->next = oldLine;

        /* if oldLine->prev we are inserting at the beginning */
        if ( !oldLine->prev ) {
            msg->first = newLine;
        } else {
            /* Otherwise insert as normal */
            oldLine->prev->next = newLine;
            oldLine->prev = newLine;
        }

        /* Update statistics */
        newLine->lSize = strLen;
        msg->numChars += newLine->lSize + 1;
        msg->numLines++;
    }
}

void line_deleteNode( MESSAGE *msg, int nodeNum ) {
    LINE *nodeToBeDeleted = NULL;

    if ( ( nodeToBeDeleted = line_getLineNode( msg, nodeNum ) ) == NULL ) {
        fprintf(stderr, "Unable to delete nodeNum %d\n", nodeNum);
        return;
    } else {

        if( !nodeToBeDeleted->prev ) {
            msg->first = nodeToBeDeleted->next;
        } else {
            nodeToBeDeleted->prev->next = nodeToBeDeleted->next;
        }

        if( !nodeToBeDeleted->next ) {
            msg->last = nodeToBeDeleted->prev;
        } else {
            nodeToBeDeleted->next->prev = nodeToBeDeleted->prev;
        }
    }

    /* Update statistics */
    msg->numLines--;
    msg->numChars -= nodeToBeDeleted->lSize + 1;

    /* Free the memory */
    free(nodeToBeDeleted->currLine);
    free(nodeToBeDeleted);
}

int main( int argc, char **argv ) {

    MESSAGE *msg = NULL;
    list_init( &msg );
    list_load( msg );

    msg = msg->next;

    //line_insertBefore( msg, 1, "I am inserted before 1" );
    //line_insertAfter( msg, 0, "I am inserted after 2" );
    line_deleteNode(msg, 1);
    list_printAll( stdout, msg );

    list_save( msg );
    list_destroy( &msg );

    return 0;

    if ( isatty( STDIN_FILENO ) && argc == 1 ) {
        printf( "isatty\n" );

    } else {
        opts = options_new();
        nonInteractive_run( opts, argc, argv );
        free( opts );
    }

    return EXIT_SUCCESS;
}
