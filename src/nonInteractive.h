/*
 * nonInteractive.h
 *
 *  Created on: 13/04/2013
 *      Author: facetoe
 */

#ifndef NONINTERACTIVE_H_
#define NONINTERACTIVE_H_

#include "defines.h"
#include "linkedList.h"
#include "helperFunctions.h"
#include "dynamicArray.h"


typedef struct{
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

	/* Output to file instead of stdout */
	int outputToFile;
	char *outFile;

}Options;

/* Initialize options struct */
void initOptions(Options *opts);

/* Parse command line options */
void parseOptions(Options *options, int argc, char **argv);

/* Print options for debugging */
void printOpts(Options *opts);

/* Ensures options make sense */
void validateOptions(Options *opts);

/* Executes options then destroys the list */
void executeOptions(Options *opts, listNode *currP);

/* Pops noteNum note and prints with args sections then deletes note */
void nonInteractive_pop(FILE *outStream, listNode *currP, char *args, int noteNum);

/* Searches through messages printing them if they contain searchTerm */
void nonInteractive_search(FILE *outStream, listNode *ln, char *searchTerm);

/* Prints usage */
void printUsage(FILE *outStream);

/* Reads from stdin until EOF growing the buffer as needed */
void nonInteractive_readPipe(dArr *buffer);

/* Run in non-interactive mode */
void nonInteractive_run(Options *opts, int argc, char **argv);

#endif /* NONINTERACTIVE_H_ */
