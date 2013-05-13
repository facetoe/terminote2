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

int main( int argc, char **argv ) {

    MESSAGE *msg = NULL;
    list_init( &msg );
    list_load( msg );

    msg = msg->next;


    //list_deleteAll(&msg);
    //line_insertBefore( msg, 1, "I am inserted before 1" );
    line_insertAfter( msg, 0, "I am inserted after 2" );
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
