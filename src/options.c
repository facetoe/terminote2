/*
 * options.c
 *
 *  Created on: 09/05/2013
 *      Author: facetoe
 */


/* Initialize options struct */

#include "options.h"
#include "nonInteractive.h"

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

    opts->version = 0;

    opts->searchNotes = 0;
    opts->searchTerm = NULL;

    opts->outputToFile = 0;
    opts->append = 0;
    opts->usage = 0;
    opts->size = 0;
    opts->interactive = 0;
    opts->popA = 0;

    return opts;
}

/* Parse command line options */
void options_parse( OPTIONS *options, int argc, char **argv ) {
    char opt;

    char *nArg, *dArg, *pArg, *fArg, *oArg;
    nArg = dArg = pArg = fArg = oArg = NULL;

    while ( ( opt = getopt( argc, argv, "sivhPFN:D:Rp:lf:a:o:" ) ) != -1 ) {
        switch ( opt ) {

        case 's':
            options->size = 1;
            break;

        case 'i':
            options->interactive = 1;
            break;

        case 'v':
            options->version = 1;
            break;

        case 'h':
            options->usage = 1;
            break;

            /* Pop Note ( note only )*/
        case 'P':
            options->pop = 1;
            break;

            /* Pop note ( with time/path info ) */
        case 'F':
            options->popA = 1;
            break;

            /* Pop n */
        case 'N':
            nArg = optarg;
            break;

            /* Delete n */
        case 'D':
            dArg = optarg;
            break;

            /* Delete all notes */
        case 'R':
            options->delA = 1;
            break;

            /* Print n */
        case 'p':
            pArg = optarg;
            break;

            /* Print all notes */
        case 'l':
            options->printA = 1;
            break;

            /* Search for notes containing sub string */
        case 'f':
            options->searchNotes = 1;
            options->searchTerm = optarg;
            break;

            /* Append note to list */
        case 'a':
            options->append = 1;
            options->appendStr = optarg;
            break;

        case 'o':
            options->outputToFile = 1;
            options->outFile = optarg;
            break;

        case '?':
            printf( "?\n" );
            exit( 1 );
            break;

        default:
            fprintf( stderr,
                    "When strange things happen, are you going round the twist? Aborting.\n" );
            abort();
            break;
        }
    }

    /* Ensures -N option is an integer */
    if ( nArg ) {
        if ( !isInteger( nArg ) ) {
            fprintf( stderr, "Error: -N requires an integer\n" );
            exit( 1 );
        } else {
            sscanf( nArg, "%d", &options->popN );
        }
    }

    /* Ensures -D option is an integer */
    if ( dArg ) {
        if ( !isInteger( dArg ) ) {
            fprintf( stderr, "Error: -D requires an integer\n" );
            exit( 1 );
        } else {
            sscanf( dArg, "%d", &options->delN );
        }
    }
    /* Ensures -p option is an integer */
    if ( pArg ) {
        if ( !isInteger( pArg ) ) {
            fprintf( stderr, "Error: -p requires an integer\n" );
            exit( 1 );
        } else {
            sscanf( pArg, "%d", &options->printN );
        }
    }

    //validateOptions(options);
}

/* Print options for debugging */
void options_print(OPTIONS *opts) {
    printf("\npopNote: %d"
            "\npopN: %d"
            "\ndelN: %d"
            "\ndeleteAll: %d"
            "\nprintN: %d"
            "\nprintAll: %d"
            "\nsearchNotes: %d"
            "\nsearchTerm: %s\n", opts->pop, opts->popN, opts->delN, opts->delA,
            opts->printN, opts->printA, opts->searchNotes, opts->searchTerm);
}

/* Executes options then destroys the list */
void options_execute( OPTIONS *opts ) {

    if ( opts->usage ) {
        printUsage( stdout );
        exit( 0 );
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
        printf( "** Run Interactive **\n" );
    } else if ( opts->popA ) {
        nonInteractive_pop( stdout, msg, "nptm", list_length( msg ) );
    } else if (opts->size) {
        fprintf(outStream, "%d\n", msg->root->totalMessages);
    }

    if ( opts->outputToFile )
        fclose( outStream );

    /* Clean up */
    list_save( msg );
    list_destroy( &msg );
}




