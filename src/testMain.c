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

MESSAGE *msg = NULL;

#define OPT_NUM 10

char *path = "/home/facetoe/.terminote.data";






OPTIONS *opts = NULL;

int main( int argc, char **argv ) {

    opts = options_new();
    options_parse( opts, argc, argv );
    options_print( opts );

    return EXIT_SUCCESS;
}
