/*
 * line.c
 *
 *  Created on: 10/05/2013
 *      Author: facetoe
 */


#include "line.h"

#define LINE_FIRST(msg) msg->first
#define LINE_LAST(msg) msg->last

/* Allocates memory for a new LINE node and sets default values */
LINE *line_getLine() {
    LINE *tmp = NULL;
    tmp = malloc( sizeof(LINE) );
    if ( !tmp ) {
        fprintf( stderr, "Failed to allocate memory in line_getline\n" );
        abort();
    }

    tmp->lNum = 0;
    tmp->lSize = 0;
    tmp->text = NULL;
    tmp->next = NULL;
    tmp->prev = NULL;

    return tmp;
}

/* Returns the requested lineNode or NULL if it doesn't exist (needs to be cleaned up) */
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

/* Inserts a line after nodeNum */
void line_insertAfter( MESSAGE *msg, int nodeNum, char *str ) {

    LINE *oldLine = NULL;

    if ( ( oldLine = line_getLineNode( msg, nodeNum ) ) == NULL ) {
        fprintf( stderr, "Unable to retrieve nodeNum %d\n", nodeNum );
        return;
    } else {

        LINE *newLine = line_getLine();

        /* Add the new line */
        int strLen = strlen( str );
        newLine->text = malloc( strLen );
        memmove( newLine->text, str, strLen );
        newLine->text[strLen] = '\0';

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

/* Inserts a line before nodeNum */
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
        newLine->text = malloc( strLen );
        memmove( newLine->text, str, strLen );
        newLine->text[strLen] = '\0';

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

/* Deletes nodeNum */
void line_deleteNode( MESSAGE *msg, int nodeNum ) {
    LINE *nodeToBeDeleted = NULL;

    if ( ( nodeToBeDeleted = line_getLineNode( msg, nodeNum ) ) == NULL ) {
        fprintf( stderr, "Unable to delete nodeNum %d\n", nodeNum );
        return;
    } else {

        if ( !nodeToBeDeleted->prev ) {
            msg->first = nodeToBeDeleted->next;
        } else {
            nodeToBeDeleted->prev->next = nodeToBeDeleted->next;
        }

        if ( !nodeToBeDeleted->next ) {
            msg->last = nodeToBeDeleted->prev;
        } else {
            nodeToBeDeleted->next->prev = nodeToBeDeleted->prev;
        }
    }

    /* Update statistics */
    msg->numLines--;
    msg->numChars -= nodeToBeDeleted->lSize + 1;

    /* Free the memory */
    free( nodeToBeDeleted->text );
    free( nodeToBeDeleted );
}

