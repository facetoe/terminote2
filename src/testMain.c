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
/* Prints usage */
void printUsage( FILE *outStream ) {
    fprintf( outStream,
            "Terminote version %.1f - a command line note tool.\n\n"
                    "Terminote reacts differently depending on how you call it.\n"
                    "If you pipe data to it, or supply a command line argument, Terminote runs in non-interactive mode.\n"
                    "If you call terminote with no arguments from the shell, terminote will enter interactive mode.\n\n"
                    "ARGUMENTS: Arguments in capitals are destructive, lower case leaves notes intact.\n"
                    " -h: Print this message and quit.\n"
                    " -P: Prints the last note only, no path or number, then deletes it.\n"
                    " -F: Prints the last note with full info (path/time/num) then deletes it.\n"
                    " -N: Prints the note at the supplied note number and deletes it, if it exists. Requires an integer argument. \n"
                    " -D: Deletes the note at supplied note number, if it exists. Requires an integer argument.\n"
                    " -R: Deletes all notes.\n"
                    " -p: Prints the note at the supplied note number, leaving it intact. Requires an integer argument.\n"
                    " -l: Prints all the notes leaving them intact.\n"
                    " -f: Searches for notes containing supplied sub string and prints them, leaving them intact. Requires a string argument.\n"
                    " -a: Appends a note to the list. Requires a string argument.\n\n"
                    "CONTACT:\n"
                    " Please email any bugs, requests or hate mail to facetoe@ymail.com, or file a bug at https://github.com/facetoe/terminote2\n",
            VERSION );
}

/* Pops noteNum note and prints with args sections then deletes note */
void nonInteractive_pop( FILE *outStream, MESSAGE *msg, char *args,
        int noteNum ) {

    if ( msg->root->totalMessages == 0 || noteNum == 0 ) {
        fprintf( stderr, "Nothing to print\n" );
        return;
    }

    MESSAGE *tmpMsg = NULL;
    if ( ( tmpMsg = list_searchByNoteNum( msg, noteNum ) ) == NULL ) {
        fprintf( outStream, "Nothing found\n" );
    } else {
        list_printMessage( outStream, args, tmpMsg );
        list_deleteNode( msg, noteNum );
    }
}

/* Searches through messages printing them if they contain searchTerm */
void nonInteractive_printAllWithSubString( FILE *outStream, MESSAGE *msg,
        char *searchTerm ) {

    msg = msg->root->next;
    if ( !msg ) {
        fprintf( outStream, "Nothing Found\n" );
    }

    for ( ; msg; msg = msg->next ) {
        if ( list_messageHasSubstring( msg, searchTerm ) )
            list_printMessage( outStream, "nptm", msg );
    }
}

/* Reads from stdin until EOF growing the buffer as needed */
void nonInteractive_appendMessage( MESSAGE *msg ) {

    /* Loop to the end of the message */
    msg = msg->root;
    for ( ; msg->next; msg = msg->next )
        ;

    /* Allocate and move to new node */
    msg->next = list_getNode( msg );
    msg = msg->next;

    /* Get and set path and time information */
    list_setPath( msg );
    list_setTime( msg );

    int ch, lineLen, totChars, numLines, buffSize;
    ch = lineLen = totChars = numLines = 0;

    char *tmp, *buffer;
    tmp = buffer = NULL;

    buffSize = 1024;
    buffer = malloc( buffSize * sizeof(char) );

    if ( !buffer ) {
        fprintf( stderr,
                "Failed to allocate memory for message buffer in nonInteractive_appendMessage\n" );
        exit( 1 );
    }

    LINE *line, *prev;
    line = prev = NULL;
    line = line_getLine();

    while ( ( ch = getchar() ) != EOF ) {

        /* If we've outgrown the buffer then allocate some more memory */
        if ( lineLen >= buffSize ) {
            buffSize *= 2;
            tmp = realloc( buffer, buffSize * sizeof(char *) );

            if ( !tmp ) {
                fprintf( stderr,
                        "Failed to allocate %d bytes in nonInteractive_appendMessage\n",
                        buffSize );
                exit( 1 );
            } else {
                buffer = tmp;
            }
        }

        if ( ch != '\n' && ch != '\0' ) {
            buffer[lineLen] = ch;
            lineLen++;
        } else {

            numLines++;
            buffer[lineLen] = 0;

            /* You pass buffer + lineLen because insertLine expects the pointer to be at the end of the string */
            insertLine( &line, buffer + lineLen, lineLen, numLines );

            if ( numLines == 1 ) {
                msg->first = line;
            }

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

    /* Update MESSAGE statistics for this message */
    msg->last = line->prev;
    msg->numChars = totChars;
    msg->numLines = numLines;
    msg->messageNum = msg->root->totalMessages + 1;
    msg->root->totalMessages++;
    free( buffer );
}

/* Executes options then destroys the list */
void options_execute( OPTIONS *opts ) {

    if ( opts->usage ) {
        printUsage(stdout);
        exit(0);
    }

    FILE *outStream = NULL;
    outStream = stdout;

    if ( opts->outputToFile )
        outStream = fopen( opts->outFile, "w" );

    MESSAGE *msg = NULL;
    list_init( &msg );
    list_load( msg );

    if ( opts->pop ) {
        nonInteractive_pop( stdout, msg, "nptm", msg->root->totalMessages );
    } else if ( opts->popN ) {
        nonInteractive_pop( stdout, msg, "nptm", opts->popN );
    } else if ( opts->delA ) {
        list_deleteAll( &msg );
    } else if ( opts->delN ) {
        if ( list_length( msg ) >= opts->delN )
            list_deleteNode( msg, opts->delN );
        else
            fprintf( stderr, "Nothing to delete at position: %d\n",
                    opts->delN );

    } else if ( opts->printN ) {
        MESSAGE *tmp = NULL;
        if ( ( tmp = list_searchByNoteNum( msg, opts->printN ) ) )
            list_printMessage( outStream, "nptm", tmp );
        else
            fprintf( stderr, "Nothing to print at position: %d\n",
                    opts->printN );

    } else if ( opts->printA ) {
        list_printAll( outStream, msg );
    } else if ( opts->searchNotes ) {
        nonInteractive_printAllWithSubString( outStream, msg,
                opts->searchTerm );
    } else if ( opts->append ) {
        list_appendMessage( msg, opts->appendStr );
    } else if ( opts->version ) {
        printf( "%.1f\n", VERSION );
    } else if ( opts->interactive ) {
        printf("** Run Interactive **\n");
    } else if ( opts->popA ) {
        nonInteractive_pop( stdout, msg, "nptm", list_length( msg ) );
    }

    if ( opts->outputToFile )
        fclose( outStream );

    /* Clean up */
    list_save( msg );
    list_destroy( &msg );
}

/* Run in non-interactive mode */
void nonInteractive_run( OPTIONS *opts, int argc, char **argv ) {

    /* If there are no arguments we'll just print usage */
    if ( argc <= 1 ) {
        printUsage(stdout);
        exit(0);
    } else {
        /* If we get here there are command line arguments, parse and execute them */
        options_parse( opts, argc, argv );
        options_execute( opts );
    }
}

char *path = "/home/facetoe/.terminote.data";

OPTIONS *opts = NULL;

int main( int argc, char **argv ) {

    opts = options_new();
    nonInteractive_run( opts, argc, argv );
    free( opts );
    return EXIT_SUCCESS;
}
