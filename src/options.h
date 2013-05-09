/*
 * options.h
 *
 *  Created on: 09/05/2013
 *      Author: facetoe
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include "helperFunctions.h"

typedef struct {
    /* Pop last note ( only the message )*/
    int pop;
    /* Pop the last note ( with time and path info ) */
    int popA;
    /* Pop n note */
    int popN;

    /* Delete n note */
    int delN;
    /* Delete all notes */
    int delA;

    /* Print n note */
    int printN;
    /* Print all notes */
    int printA;

    /* Search notes for search term */
    int searchNotes;
    char *searchTerm;

    /* Append note */
    int append;
    char *appendStr;

    /* Print the version */
    int version;

    /* Print Usage */
    int usage;

    /* Run in interactive mode */
    int interactive;

    /* Output to file instead of stdout */
    int outputToFile;
    char *outFile;

} OPTIONS;

/* Initialize options struct */
OPTIONS *options_new();

/* Parse command line options */
void options_parse( OPTIONS *options, int argc, char **argv );

/* Print options for debugging */
void options_print( OPTIONS *opts );

/* Ensures options make sense */
void options_validate( OPTIONS *opts );

/* Print options for debugging */
void options_print(OPTIONS *opts);




#endif /* OPTIONS_H_ */
