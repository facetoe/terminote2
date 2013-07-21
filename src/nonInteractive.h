/*
 * nonInteractive.h
 *
 *  Created on: 09/05/2013
 *      Author: facetoe
 */

#ifndef NONINTERACTIVE_H_
#define NONINTERACTIVE_H_

#include "linkedList.h"
#include "line.h"
#include "options.h"

/* Prints usage */
void printUsage( FILE *outStream );

/* Searches through messages printing them if they contain searchTerm */
void nonInteractive_printAllWithSubString( FILE *outStream, MESSAGE *msg, char *searchTerm );

/* Reads from stdin until EOF growing the buffer as needed */
void nonInteractive_appendMessage( MESSAGE *msg );

/* Reads the output of command into a MESSAGE struct. */
void nonInteractive_appendClipboardContents( MESSAGE *msg , char *command);

/* Pops noteNum note and prints with args sections then deletes note */
void nonInteractive_pop( FILE *outStream, MESSAGE *msg, char *args, int noteNum );

/* "greps" the messages printing all matches with message and line numbers */
void nonInteractive_grepMessages(FILE *outStream, MESSAGE *msg, char *subString);

/* Prints information on stored messages */
void nonInteractive_printStats(FILE *outStream, MESSAGE *msg);

/* Run in non-interactive mode */
void nonInteractive_run( OPTIONS *opts, int argc, char **argv );


#endif /* NONINTERACTIVE_H_ */
