/*
 * linkedList.h
 *
 *  Created on: 09/05/2013
 *      Author: facetoe
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "defines.h"
#include "helperFunctions.h"
#include "structures.h"

#include <unistd.h> // getcwd


extern char * path;



/* Allocates memory for a new LINE node and sets default values */
LINE *line_getLine();

/* Allocates memory for a MESSAGE list and initializes default values */
void list_init( MESSAGE **msg );

/* Returns a new MESSAGE node */
MESSAGE *list_getNode( MESSAGE *msg );

/* Get and store path information */
void list_setPath(MESSAGE *msg);

/* Get and store time information */
void list_setTime(MESSAGE *msg);

/* Inserts a line into a LINE struct */
void insertLine( LINE **l, char *s, int lineLen, int numLines );

/* Inserts a string into a MESSAGE struct */
void list_insertString( MESSAGE *msg, char *str );

/* Reads the note data from a file and places in struct */
void list_readBinary( FILE *fp, MESSAGE *msg );

/* Writes the MESSAGE structs to a file */
void list_writeBinary( FILE *fp, MESSAGE *msg );

/* Free all memory in the LINEDATA list */
void list_destroy( MESSAGE **message );

/* Returns the length of the list */
int list_length( MESSAGE *msg );

/* Appends message to the end of the list */
void list_appendMessage( MESSAGE *msg, char *str );

/* Prints current note according to args. Args are:
 * n: Note number
 * p: Path
 * t: Time
 * m: Message */
void list_printMessage( FILE *outStream, char *args, MESSAGE *msg );

/* Prints all messages with all information */
void list_printAll( FILE *outStream, MESSAGE *msg );

/* Moves list pointer to last node in the list.
 * If it is already the last node then leaves the pointer unchanged. */
void list_lastNode( MESSAGE **msg );

/* Moves list pointer to the first node in the list.
 * If the first node is the root node then leaves the pointer unchanged. */
void list_firstNode( MESSAGE **msg );

/* Moves the list pointer to the next node in the list. If it is the last node,
 * moves the pointer to the first node: ie, head->next to skip root node.
 * If there is only one node in the list (not counting root) it returns currP unchanged. */
void list_next( MESSAGE **msg );

/* Moves the list pointer to the previous node in the list.
 * If there is only one node (not counting root) leaves the pointer unchanged.  */
void list_previous( MESSAGE **msg );

/* Searches for node with noteNum.
 * Returns node if found, otherwise returns NULL. */
MESSAGE *list_searchByNoteNum( MESSAGE *msg, int noteNum );

/* Reorders the noteNums */
void list_orderList( MESSAGE *msg );

/* Delete a node by noteNum */
void list_deleteNode( MESSAGE *msg, int noteNum );

/* Deletes all nodes except for the root node */
void list_deleteAll( MESSAGE **message );

/* Attempts to read a saved list from path. If no file is found, attempts to create one.*/
void list_load( MESSAGE *msg );

/* Attempts to save the list at path. */
void list_save( MESSAGE *msg );

/* Searches the listNode's message for substring. Returns true if it does,
 * false if not. */
bool list_messageHasSubstring( MESSAGE *msg, char *subStr );


#endif /* LINKEDLIST_H_ */
