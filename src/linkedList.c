/*
 * linkedList.c
 *
 *  Created on: 09/05/2013
 *      Author: facetoe
 */

#include "linkedList.h"
#include <assert.h>



/* Allocates memory for a MESSAGE list and initializes default values */
void list_init( MESSAGE **msg ) {
    MESSAGE *tmp = NULL;
    tmp = calloc( 1, sizeof(MESSAGE) );

    if ( !tmp ) {
        fprintf( stderr, "Unable to allocate memory in lineData_init.\n" );
        abort();
    }

    tmp->numLines = 0;
    tmp->numChars = 0;
    tmp->messageNum = 0;
    tmp->totalMessages = 0;
    tmp->first = NULL;
    tmp->last = NULL;
    tmp->next = NULL;
    tmp->prev = NULL;
    tmp->root = tmp;

    *msg = tmp;
}

/* Returns a new MESSAGE node */
MESSAGE *list_getNode( MESSAGE *msg ) {
    MESSAGE *tmp = NULL;
    tmp = calloc( 1, sizeof(MESSAGE) );

    if ( !tmp ) {
        fprintf( stderr, "Unable to allocate memory in lineData_init.\n" );
        abort();
    }

    tmp->numLines = 0;
    tmp->numChars = 0;
    tmp->messageNum = 0;
    tmp->totalMessages = 0;
    tmp->first = NULL;
    tmp->last = NULL;
    tmp->next = NULL;
    tmp->prev = NULL;
    tmp->root = msg->root;

    return tmp;
}

/* Get and store path information */
void list_setPath( MESSAGE *msg ) {
    char *path = getcwd( NULL, 0 );
    if ( path == NULL ) {
        fprintf( stderr, "Unable to retrieve path\n" );
    } else {
        size_t path_len = strlen( path );
        strncpy( msg->path, path, path_len );
        free( path );
    }
}

/* Get and store time information */
void list_setTime( MESSAGE *msg ) {
    char *time = current_time();
    if ( time == NULL ) {
        perror( "Unable to retrieve time\n" );
    } else {
        strip_newline( time );
        size_t time_len = strlen( time );
        strncpy( msg->time, time, time_len );
    }
}

/* Inserts a line into a LINE struct */
void insertLine( LINE **l, char *s, int lineLen, int numLines ) {

    LINE *line = *l;
    line->text = malloc( lineLen + 1 );

    /* s - lineLen is the start of the line. Copy lineLen characters into the waiting string,
     * ie, from the start to the end of the line */
    memcpy( line->text, s - lineLen, lineLen );
    line->text[lineLen] = 0;

    /* Update statistics */
    line->lNum = numLines;
    line->lSize = lineLen;
    *l = line;
}

/* Parses a string into individual lines and inserts into the MESSAGE's LINE struct */
void list_insertString( MESSAGE *msg, char *str ) {
    char *s = NULL;
    int lineLen, totChars, numLines;
    lineLen = totChars = numLines = 0;

    LINE *line, *prev;
    line = prev = NULL;
    line = line_getLine();

    for ( s = str; *s; s++ ) {

        /* Increment lineLen until we hit a NULL or a newline */
        if ( *s != '\0' && *s != '\n' ) {
            lineLen++;
        } else {

            if ( s - lineLen == str ) {
                msg->first = line;
            }

            numLines++;

            /* Insert this line */
            insertLine( &line, s, lineLen, numLines );

            /* Set and update the prev pointer */
            line->prev = prev;
            prev = line;

            /* Get a new line and move to it */
            line->next = line_getLine();
            line = line->next;

            /* It's lineLen + 1 because when we write the line we append a newline. */
            /* This could be done in the writeBinary function but I think it's easier this way */
            totChars += lineLen + 1;
            lineLen = 0;
        }
    }

    /* If there is no newline at the end of the last line it won't get caught above, so deal with it here */
    if ( lineLen > 0 ) {
        if ( s - lineLen == str ) {
            msg->first = line;
        }
        numLines++;
        insertLine( &line, s, lineLen, numLines );

        /* Set and update the prev pointer */
        line->prev = prev;
        prev = line;

        /* Get a new line and move to it */
        line->next = line_getLine();
        line = line->next;

        totChars += lineLen + 1;
    }

    /* Update MESSAGE statistics for this message */
    msg->last = prev;
    msg->pageTop = msg->first;
    msg->currentLine = msg->first;

    msg->numChars = totChars;
    msg->numLines = numLines;
    msg->messageNum = msg->root->totalMessages + 1;
    msg->root->totalMessages++;
    msg->root->numLines += numLines;
}

/* Appends message to the end of the list */
void list_appendMessage( MESSAGE *msg, char *str ) {

    assert(msg != NULL);

    MESSAGE *prev = NULL;

    /* Loop to the end of the message */
    msg = msg->root;

    for ( ; msg->next; msg = msg->next )
        ;

    prev = msg;

    /* Allocate and move to new node */
    msg->next = list_getNode( msg );
    msg->prev = prev;
    msg = msg->next;

    /* Get and store path and time information */
    list_setPath( msg );
    list_setTime( msg );

    /* Insert the message */
    list_insertString( msg, str );
}

/* Writes the MESSAGE structs to a file */
void list_writeBinary( FILE *fp, MESSAGE *msg ) {
    assert( msg != NULL && fp != NULL );
    msg = msg->root;
    /* Don't write root node */
    if ( msg->messageNum == 0 )
        msg = msg->next;

    while ( msg ) {
        if ( DEBUG )
            printf( "Writing Note #%d\n", msg->messageNum );

        long first = msg->numChars;
        int len;

        /* Write numChars first so we know how many to read later */
        fwrite( &first, sizeof( first ), 1, fp );

        /* Write each line */
        for ( LINE *line = msg->first; line->next; line = line->next ) {
            fwrite( line->text, sizeof(char), line->lSize, fp );

            /* Write a newline so we can seperate the lines later */
            fwrite( "\n", sizeof(char), 1, fp );
        }

        /* Write path */
        len = strlen( msg->path ) + 1;
        fwrite( &len, sizeof(int), 1, fp );
        fwrite( msg->path, sizeof(char), len, fp );

        /* Write time */
        len = strlen( msg->time ) + 1;
        fwrite( &len, sizeof(int), 1, fp );
        fwrite( msg->time, sizeof(char), len, fp );

        msg = msg->next;
    }
}

/* Reads the note data from a file and places in struct */
void list_readBinary( FILE *fp, MESSAGE *msg ) {
    assert( msg != NULL && fp != NULL );

    long first;
    int len, note_num;
    MESSAGE *previous = NULL;
    note_num = 0;

    char path[MAX_PATH_SIZE];
    char time[MAX_TIME_SIZE];

    while ( fread( &first, sizeof( first ), 1, fp ) ) {
        note_num++;

        if ( DEBUG )
            printf( "Reading Note #%d\n", note_num );

        /* Allocate memory for new MESSAGE node */
        msg->next = list_getNode( msg );
        previous = msg;

        /* Move to new node */
        msg = msg->next;
        msg->prev = previous;

        /* We got the first length in the loop condition.... */
        char *buffer = malloc( first * sizeof(char) + 1 );
        fread( buffer, sizeof(char), first, fp );
        buffer[first] = '\0';

        /* Update char stats. It's -1 as we don't count the terminator. */
        msg->root->numChars += first-1;

        /* Insert the message */
        list_insertString( msg, buffer );

        /* Add path */
        fread( &len, sizeof( len ), 1, fp );
        fread( path, sizeof(char), len, fp );
        memcpy( msg->path, path, len );
        msg->path[len] = 0;

        /* Add time */
        fread( &len, sizeof( len ), 1, fp );
        fread( time, sizeof(char), len, fp );
        memcpy( msg->time, time, len );
        msg->time[len] = 0;

        free( buffer );
    }
}

/* Free all memory in the LINEDATA list */
void list_destroy( MESSAGE **message ) {
    assert( message != NULL );
    LINE *line, *tmpLine;
    MESSAGE *msg, *tmpMsg;
    msg = *message;
    msg = msg->root;
    while ( msg ) {
        line = msg->first;
        while ( line ) {
            tmpLine = line->next;
            free( line->text );
            free( line );
            line = tmpLine;
        }
        tmpMsg = msg->next;
        free( msg );
        msg = tmpMsg;
    }
    free( line );
    free( msg );
    *message = NULL;
}

/* Returns the length of the list */
int list_length( MESSAGE *msg ) {
    assert( msg != NULL );
    int listSize = 0;
    msg = msg->root;
    if ( !msg )
        return 0;

    if ( msg->next ) {
        msg = msg->next; // Don't count root node.
        for ( ; msg; msg = msg->next, listSize++ )
            ;
    }
    return listSize;
}

/* Prints current note according to args. Args are:
 * n: Note number
 * p: Path
 * t: Time
 * m: Message */
void list_printMessage( FILE *outStream, char *args, MESSAGE *msg ) {
    assert( msg != NULL );

    if ( !msg ) {
        fprintf( stdout, "Nothing to print\n" );
        return;
    }

    else {
        for ( char *s = args; *s; s++ ) {
            switch ( *s ) {
            case 'n':
                fprintf( outStream, "Note Number: %d\n", msg->messageNum );
                break;
            case 'p':
                fprintf( outStream, "Path: %s\n", msg->path );
                break;
            case 't':
                fprintf( outStream, "Time: %s\n", msg->time );
                break;
            case 'm':
                fprintf( outStream, "Message:\n" );
                LINE *lines = msg->first;
                for ( ; lines->next; lines = lines->next )
                    fprintf( outStream, "%s\n", lines->text );
                fprintf( outStream, "\n\n" );
                break;
            default:
                break;
            }
        }
    }
}

/* Prints all messages with all information */
void list_printAll( FILE *outStream, MESSAGE *msg ) {

    if ( !msg ) {
        fprintf( stderr, "Nothing to print\n" );
        return;
    }

    msg = msg->root;

    if ( !msg->next ) {
        fprintf( stdout, "Nothing to print\n" );
        return;
    }

    for ( msg = msg->next; msg; msg = msg->next ) {
        list_printMessage( outStream, "nptm", msg );
    }

}

/* Moves list pointer to last node in the list.
 * If it is already the last node then leaves the pointer unchanged. */
void list_lastNode( MESSAGE **msg ) {
    assert( *msg != NULL );
    MESSAGE *tmp = *msg;
    if ( tmp->next == NULL )
        return;
    for ( ; tmp->next; tmp = tmp->next )
        ;
    *msg = tmp;
}

/* Moves list pointer to the first node in the list.
 * If the first node is the root node then leaves the pointer unchanged. */
void list_firstNode( MESSAGE **msg ) {
    assert( *msg != NULL );
    MESSAGE *tmp = *msg;
    tmp = tmp->root;
    if ( tmp->next )
        tmp = tmp->next;
    *msg = tmp;
}

/* Moves the list pointer to the next node in the list. If it is the last node,
 * moves the pointer to the first node: ie, head->next to skip root node.
 * If there is only one node in the list (not counting root) it returns currP unchanged. */
void list_next( MESSAGE **msg ) {
    assert( *msg != NULL );
    MESSAGE *tmp = *msg;
    if ( tmp->next ) {
        tmp = tmp->next;
    } else if ( tmp->root->next ) {
        tmp = tmp->root->next;
    }
    *msg = tmp;
}

/* Moves the list pointer to the previous node in the list.
 * If there is only one node (not counting root) leaves the pointer unchanged.  */
void list_previous( MESSAGE **msg ) {
    assert( *msg != NULL );
    MESSAGE *tmp = *msg;
    int noteNum = tmp->messageNum;
    if ( noteNum == 1 ) {
        /* We are at the start of the list, so grab the last node */
        list_lastNode( &tmp );
    } else {
        /* We are not at the start, so grab the first node */
        list_firstNode( &tmp );

        /* And loop through to noteNum -1, ie the previous node */
        for ( ; tmp->next != NULL && tmp->messageNum != noteNum - 1;
                tmp = tmp->next )
            ;
    }
    *msg = tmp;
}

/* Searches for node with noteNum.
 * Returns node if found, otherwise returns NULL. */
MESSAGE *list_searchByNoteNum( MESSAGE *msg, int noteNum ) {
    assert( msg != NULL );

    /* Reset to first node */
    msg = msg->root;

    /* Nothing to search if the list is empty */
    if ( list_length( msg ) == 0 )
        return NULL;

    /* don't check root node */
    msg = msg->next;

    for ( ; msg->messageNum != noteNum && msg->next; msg = msg->next )
        ;

    if ( msg->messageNum == noteNum )
        return msg;
    else
        return NULL;
}

/* Reorders the noteNums */
void list_orderList( MESSAGE *msg ) {
    assert( msg != NULL );

    int nNum = 1;
    /* Don't count root node */
    msg = msg->root;

    if ( msg->next ) {
        msg = msg->next;
        for ( ; msg; msg = msg->next, nNum++ ) {
            msg->messageNum = nNum;
        }
    }
    if ( DEBUG )
        printf( "Ordered list\n" );
}

/* Delete a node by noteNum */
void list_deleteNode( MESSAGE *msg, int noteNum ) {

    assert( msg != NULL );

    /* Don't delete root node */
    if ( noteNum == 0 )
        return;

    msg = msg->root;

    MESSAGE *nodeToBeDeleted;
    LINE *tmpLine, *line;

    if ( ( nodeToBeDeleted = list_searchByNoteNum( msg, noteNum ) ) == NULL ) {
        fprintf( stderr, "Unable to delete node\n" );
        return;
    } else {

        if ( !nodeToBeDeleted->prev ) {
            msg->root = nodeToBeDeleted->next;
        } else {
            nodeToBeDeleted->prev->next = nodeToBeDeleted->next;
        }

        if ( !nodeToBeDeleted->next ) {
            for (; msg->next ; msg = msg->next)
                ;
            msg = nodeToBeDeleted->prev;
        } else {
            nodeToBeDeleted->next->prev = nodeToBeDeleted->prev;
        }
    }

    line = nodeToBeDeleted->first;
    while (line) {
        tmpLine = line->next;
        free(line->text);
        free(line);
        line = tmpLine;
    }
    free(line);

    msg->root->totalMessages--;

    list_orderList(msg);
}

/* Deletes all nodes except for the root node */
void list_deleteAll( MESSAGE **message ) {
    if ( DEBUG )
        printf( "Deleting all messages\n" );

    assert( message != NULL );

    MESSAGE *tmpMsg, *msg, *root;
    msg = *message;

    root = msg->root;

    /* Don't delete root node! */
    msg = msg->root->next;

    if ( !msg )
        return;

    LINE *tmpLine, *line;

    while ( msg ) {
        line = msg->first;
        if ( DEBUG )
            printf( "Freeing Message #%d\n", msg->messageNum );
        while ( line ) {
            tmpLine = line->next;
            free( line->text );
            free( line );
            line = tmpLine;
        }
        tmpMsg = msg->next;
        free( msg );
        msg = tmpMsg;
    }
    root->next = NULL;
    root->totalMessages = 0;
    *message = root;
}

/* Attempts to read a saved list from path. If no file is found, attempts to create one.*/
/* Returns true on success or false on failure. */
bool list_load( MESSAGE *msg ) {
    if ( DEBUG )
        printf( "Loading list from: %s\n", path );

    assert( msg != NULL );

    FILE *fp;

    if ( file_exists( path ) && ( fp = fopen( path, "rb" ) ) != NULL ) {
        list_readBinary( fp, msg );
        fclose( fp );
        return true;
    } else {
        fprintf( stderr,
                "Error loading data file at: %s\nAttempting to create one...\n",
                path );

        fp = fopen( path, "wb" );
        if ( fp != NULL ) {
            fprintf( stderr, "Successfully created file\n" );
            fclose( fp );
            return true;

        } else {
            fprintf( stderr, "Failed to create file\n" );
            return false;
        }
    }
}

/* Attempts to save the list at path. */
/* Returns true on success or false on failure. */bool list_save( MESSAGE *msg ) {
    assert( msg != NULL );

    if ( DEBUG )
        printf( "Saving list at: %s\n", path );

    FILE *fp = NULL;
    if ( file_exists( path ) && ( fp = fopen( path, "wb" ) ) != NULL ) {
        list_writeBinary( fp, msg );
        fclose( fp );
        return true;

    } else {
        fprintf( stderr,
                "Error loading data file at: %s\nAttempting to create one...\n",
                path );

        fp = fopen( path, "wb" );
        if ( fp != NULL ) {
            fprintf( stderr, "Successfully created file\n" );
            list_writeBinary( fp, msg );
            fclose( fp );
            return true;

        } else {
            fprintf( stderr, "Failed to create file\n" );
            return false;
        }
    }
}

/* Searches the listNode's message for substring. Returns true if it does,
 * false if not. */
bool list_messageHasSubstring( MESSAGE *msg, char *subStr ) {
    LINE *line = msg->first;

    for ( ; line && line->text; line = line->next ) {
        if ( strstr( line->text, subStr ) != NULL )
            return true;
    }
    return false;
}


