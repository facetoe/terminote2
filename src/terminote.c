/*
 ============================================================================
 Name        : listRewrite.c
 Author      : facetoe
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#define _POSIX_SOURCE // You need this here to get rid of the waring with sigaction
#include "helperFunctions.h"
#include "linkedList.h"
#include "options.h"
#include "defines.h"
#include "nonInteractive.h"
#include "ui.h"

#include "ncurses.h"
#include "menu.h"
#include "assert.h"

#include <sys/ioctl.h> // For terminal size
#include <signal.h> // For sigaction
char *path;
const char *dataFile = "/.terminote.data";

OPTIONS *opts = NULL;


int main( int argc, char **argv ) {
    getDataPath();

    if ( isatty( STDIN_FILENO ) && argc == 1 ) {
        run();
    } else {
        opts = options_new();
        nonInteractive_run( opts, argc, argv );
        free( opts );
    }

    return EXIT_SUCCESS;
}
