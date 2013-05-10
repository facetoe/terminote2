/*
 * linkedList.c
 *
 *  Created on: 09/05/2013
 *      Author: facetoe
 */

#include "linkedList.h"

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
    tmp->currLine = NULL;
    tmp->next = NULL;
    tmp->prev = NULL;

    return tmp;
}

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
    line->currLine = malloc( lineLen + 1 );

    /* s - lineLen is the start of the line. Copy lineLen characters into the waiting string,
     * ie, from the start to the end of the line */
    memcpy( line->currLine, s - lineLen, lineLen );
    line->currLine[lineLen] = 0;

    /* Update statistics */
    line->lNum = numLines;
    line->lSize = lineLen;
    *l = line;
}

/* Inserts a string into a MESSAGE struct */
void list_insertString( MESSAGE *msg, char *str ) {
    char *s = NULL;
    int lineLen, totChars, numLines;
    lineLen = totChars = numLines = 0;

    LINE *line, *prev;
    line = prev = NULL;
    line = line_getLine();

    for ( s = str; *s; s++ ) {

        if ( *s != '\0' && *s != '\n' ) {
            lineLen++;
        } else {

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
            lineLen = 0;
        }
    }

    /* If there is no newline at the end of a line it won't get caught above, so deal with it here */
    if ( lineLen > 0 ) {
        if ( s - lineLen == str ) {
            msg->first = line;
        }
        totChars += lineLen + 1;
        numLines++;
        insertLine( &line, s, lineLen, numLines );

        /* Set and update the prev pointer */
        line->prev = prev;
        prev = line;

        /* Get a new line and move to it */
        line->next = line_getLine();
        line = line->next;
    }

    /* Update MESSAGE statistics for this message */
    msg->last = line->prev;
    msg->numChars = totChars;
    msg->numLines = numLines;
    msg->messageNum = msg->root->totalMessages + 1;
    msg->root->totalMessages++;
}

/* Appends message to the end of the list */
void list_appendMessage( MESSAGE *msg, char *str ) {

    /* Loop to the end of the message */
    msg = msg->root;

    for ( ; msg->next; msg = msg->next )
        ;

    /* Allocate and move to new node */
    msg->next = list_getNode( msg );
    msg = msg->next;

    /* Get and store path and time information */
    list_setPath( msg );
    list_setTime( msg );

    /* Insert the message */
    list_insertString( msg, str );
}

/* Reads the note data from a file and places in struct */
void list_readBinary( FILE *fp, MESSAGE *msg ) {

    if ( !fp || !msg ) {
        fprintf( stderr,
                "list_readBinary received a NULL file pointer or message pointer\n" );
        return;
    }

    long first;
    int len, note_num;
    note_num = 0;

    char path[MAX_PATH_SIZE];
    char time[MAX_TIME_SIZE];

    while ( fread( &first, sizeof( first ), 1, fp ) ) {
        note_num++;

        if ( DEBUG )
            printf( "Reading Note #%d\n", note_num );

        /* Allocate memory for new MESSAGE node */
        msg->next = list_getNode( msg );

        /* Move to new node */
        msg = msg->next;

        /* We got the first length in the loop condition.... */
        char *buffer = malloc( first * sizeof(char) + 1 );
        fread( buffer, sizeof(char), first, fp );
        buffer[first] = '\0';

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

/* Writes the MESSAGE structs to a file */
void list_writeBinary( FILE *fp, MESSAGE *msg ) {
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
            fwrite( line->currLine, sizeof(char), line->lSize, fp );
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

/* Free all memory in the LINEDATA list */
void list_destroy( MESSAGE **message ) {
    LINE *line, *tmpLine;
    MESSAGE *msg, *tmpMsg;
    msg = *message;
    msg = msg->root;
    while ( msg ) {
        line = msg->first;
        while ( line ) {
            tmpLine = line->next;
            free( line->currLine );
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
                    fprintf( outStream, "%s\n", lines->currLine );
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
    MESSAGE *tmp = *msg;
    if ( tmp->next != NULL ) {
        tmp = tmp->next;
    } else if ( tmp->root->next ) {
        tmp = tmp->root->next;
    }
    *msg = tmp;
}

/* Moves the list pointer to the previous node in the list.
 * If there is only one node (not counting root) leaves the pointer unchanged.  */
void list_previous( MESSAGE **msg ) {
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
    msg = msg->root;
    /* Don't delete root node */
    if ( noteNum == 0 )
        return;

    MESSAGE *tmpNode;
    LINE *tmpLine, *line;

    while ( msg ) {
        /* Go to the node before the one to be deleted */
        if ( msg->messageNum == noteNum - 1 ) {

            /* tmpNode points to node to be deleted */
            tmpNode = msg->next;

            /* line points to the message to be deleted */
            line = msg->next->first;

            if ( DEBUG )
                printf( "Deleting note #%d\n", tmpNode->messageNum );

            /* Loop through the message freeing each line */
            while ( line ) {
                tmpLine = line->next;
                free( line->currLine );
                free( line );
                line = tmpLine;
            }

            /* Link msg to node after tmpNode, leaving a hole */
            msg->next = tmpNode->next;

            /* Free tmpNode */
            if ( tmpNode != NULL )
                free( tmpNode );

            /* Reorder the list */
            list_orderList( msg );
            return;
        } else {
            tmpNode = msg;
            msg = msg->next;
        }
    }
}

/* Deletes all nodes except for the root node */
void list_deleteAll( MESSAGE **message ) {
    if ( DEBUG )
        printf( "Deleting all messages\n" );

    MESSAGE *tmpMsg, *msg, *root;
    msg = *message;

    root = msg->root;
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
            free( line->currLine );
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
/* Returns true on success or false on failure. */bool list_load( MESSAGE *msg ) {
    if ( DEBUG )
        printf( "Loading list from: %s\n", path );

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
 * false if not. */bool list_messageHasSubstring( MESSAGE *msg, char *subStr ) {
    LINE *line = msg->first;

    for ( ; line && line->currLine; line = line->next ) {
        if ( strstr( line->currLine, subStr ) != NULL )
            return true;
    }
    return false;
}

