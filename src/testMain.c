/*
 ============================================================================
 Name        : listRewrite.c
 Author      : facetoe
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helperFunctions.h"

#include "linkedList.h"

MESSAGE *msg = NULL;

char *path = "/home/facetoe/.terminote.data";

int main( void ) {

    list_init( &msg );
    FILE * fp = fopen( path, "rb" );
    list_load( msg );
    fclose( fp );

    fp = fopen( "/home/facetoe/.terminote.data", "wb" );
    if ( fp == NULL )
        printf( "FUCK" );

    list_appendMessage(msg, "hey there baby");

    //MESSAGE *tmp = list_searchByNoteNum(msg, 1);
    msg = msg->next;


    list_printAll( stdout, msg );

    list_save( msg );
    fclose( fp );
    list_destroy( &msg );

    return EXIT_SUCCESS;
}
