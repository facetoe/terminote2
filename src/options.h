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
#include "linkedList.h"
#include "defines.h"

typedef struct {
    /* Pop last note */
    int pop;
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
    /* Print last note */
    int printL;

    /* Search notes for search term */
    int searchNotes;
    int grep;
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

    /* Copy data from clipboard */
    int copyFromClip;

    /* Number of messages */
    int stats;

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

/* Executes options then destroys the list */
void options_execute( OPTIONS *opts );




#endif /* OPTIONS_H_ */
