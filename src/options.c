/*
 * options.c
 *
 *  Created on: 09/05/2013
 *      Author: facetoe
 */

/* Initialize options struct */

#include "options.h"
#include "nonInteractive.h"
#include <fcntl.h>

#define OPT_NUM 15

OPTIONS *options_new() {

    OPTIONS *opts = NULL;
    opts = malloc( sizeof(OPTIONS) );
    if ( !opts ) {
        fprintf( stderr, "Unable to allocate memory for options_new\n" );
        exit( 1 );
    }

    opts->pop = 0;
    opts->popN = 0;

    opts->delN = 0;
    opts->delA = 0;

    opts->printN = 0;
    opts->printA = 0;
    opts->printL = 0;

    opts->version = 0;

    opts->searchNotes = 0;
    opts->searchTerm = NULL;

    opts->copyFromClip = 0;

    opts->outputToFile = 0;
    opts->append = 0;
    opts->usage = 0;
    opts->size = 0;
    opts->interactive = 0;

    return opts;
}

int validateInt( char *flag, char *arg ) {
    int result;
    if ( !isInteger( arg ) ) {
        fprintf( stderr, "Error: %s requires an integer\n", flag );
        exit( 1 );
    } else {
        sscanf( optarg, "%d", &result );
    }
    return result;
}

/* Parse command line options */
void options_parse( OPTIONS *options, int argc, char **argv ) {
    char opt;
    int numFlags = 0;

    while ( ( opt = getopt( argc, argv, "n:csivhPFN:D:Rplf:a:o:" ) ) != -1 ) {
        switch ( opt ) {

        case 'c':
            options->copyFromClip = 1;
            numFlags++;
            break;

        case 's':
            options->size = 1;
            numFlags++;
            break;

        case 'i':
            options->interactive = 1;
            numFlags++;
            break;

        case 'v':
            options->version = 1;
            numFlags++;
            break;

        case 'h':
            options->usage = 1;
            numFlags++;
            break;

            /* Pop Note ( note only )*/
        case 'P':
            options->pop = 1;
            numFlags++;
            break;

            /* Pop n */
        case 'N':
            options->popN = validateInt( "-N", optarg );
            numFlags++;
            break;

            /* Delete n */
        case 'D':
            options->delN = validateInt( "-D", optarg );
            numFlags++;
            break;

            /* Delete all notes */
        case 'R':
            options->delA = 1;
            numFlags++;
            break;

            /* Print last */
        case 'p':
            options->printL = 1;
            numFlags++;
            break;

            /* Print n */
        case 'n':
            options->printN = validateInt( "-n", optarg );
            numFlags++;
            break;

            /* Print all notes */
        case 'l':
            options->printA = 1;
            numFlags++;
            break;

            /* Search for notes containing sub string */
        case 'f':
            options->searchNotes = 1;
            options->searchTerm = optarg;
            numFlags++;
            break;

            /* Append note to list */
        case 'a':
            options->append = 1;
            options->appendStr = optarg;
            numFlags++;
            break;

        case 'o':
            options->outputToFile = 1;
            options->outFile = optarg;
            numFlags++;
            break;

        case '?':
            printf( "Strange argument\n" );
            exit( 1 );
            break;

        default:
            fprintf( stderr,
                    "When strange things happen, are you going round the twist? Aborting.\n" );
            abort();
            break;
        }
    }

    /* Only one option at a time makes sense, so if there are more then one print usage and exit */
    if( numFlags > 1 ) {
        printf("Too many arguments.\nUsage: terminote [FLAG] [ARGUMENT]\n");
        exit(1);
    }
}

/* Print options for debugging */
void options_print( OPTIONS *opts ) {
    printf( "\npopNote: %d"
            "\npopN: %d"
            "\ndelN: %d"
            "\ndeleteAll: %d"
            "\nprintN: %d"
            "\nprintAll: %d"
            "\nsearchNotes: %d"
            "\nsearchTerm: %s\n", opts->pop, opts->popN, opts->delN, opts->delA,
            opts->printN, opts->printA, opts->searchNotes, opts->searchTerm );
}

//TODO Clean this function up!
/* Executes options then destroys the list */
void options_execute( OPTIONS *opts ) {

    if ( opts->usage ) {
        printUsage( stdout );
        exit( 0 );
    }

    FILE *outStream = NULL;
    outStream = stdout;

    if ( opts->outputToFile ) {
        printf( "Not implemented\n" ); //outStream = fopen( opts->outFile, "w" );
    }

    MESSAGE *msg = NULL;

    if ( opts->pop ) {
        list_init( &msg );
        list_load( msg );
        nonInteractive_pop( stdout, msg, "nptm", msg->root->totalMessages );
    } else if ( opts->popN ) {
        list_init( &msg );
        list_load( msg );
        nonInteractive_pop( stdout, msg, "nptm", opts->popN );
    } else if ( opts->delA ) {
        int fd = open( path, O_TRUNC, O_WRONLY );
        close( fd );
        exit( 0 );

    } else if ( opts->delN ) {
        list_init( &msg );
        list_load( msg );
        if ( list_length( msg ) >= opts->delN )
            list_deleteNode( msg, opts->delN );
        else
            fprintf( stderr, "Nothing to delete at position: %d\n",
                    opts->delN );

    } else if ( opts->printN ) {
        list_init( &msg );
        list_load( msg );
        MESSAGE *tmp = NULL;
        if ( ( tmp = list_searchByNoteNum( msg, opts->printN ) ) )
            list_printMessage( outStream, "nptm", tmp );
        else
            fprintf( stderr, "Nothing to print at position: %d\n",
                    opts->printN );

    } else if ( opts->printA ) {
        list_init( &msg );
        list_load( msg );
        list_printAll( outStream, msg );
    } else if ( opts->searchNotes ) {
        list_init( &msg );
        list_load( msg );
        nonInteractive_printAllWithSubString( outStream, msg,
                opts->searchTerm );
    } else if ( opts->append ) {
        list_init( &msg );
        list_load( msg );
        list_appendMessage( msg, opts->appendStr );
    } else if ( opts->version ) {
        printf( "%.1f\n", VERSION );
    } else if ( opts->interactive ) {
        printf( "** Run Interactive **\n" );
    } else if ( opts->size ) {
        list_init( &msg );
        list_load( msg );
        fprintf( outStream, "%d stored notes\n", msg->root->totalMessages );
    } else if ( opts->copyFromClip ) {
        list_init( &msg );
        list_load( msg );
        nonInteractive_appendClipboardContents( msg, "xclip -o  2>&1" );
    } else if ( opts->printL ) {
        list_init( &msg );
        list_load( msg );
        MESSAGE *tmp = list_searchByNoteNum( msg, msg->root->totalMessages );
        if ( tmp ) {
            list_printMessage( outStream, "nptm", tmp );
        }
    }

    if ( opts->outputToFile )
        fclose( outStream );

    /* Clean up */
    if ( msg ) {
        list_save( msg );
        list_destroy( &msg );
    }
}

