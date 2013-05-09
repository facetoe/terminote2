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

    if ( isatty( STDIN_FILENO ) && argc == 1 ) {
        printf( "isatty\n" );

    } else {
        opts = options_new();
        nonInteractive_run( opts, argc, argv );
        free( opts );
    }

    return EXIT_SUCCESS;
}
