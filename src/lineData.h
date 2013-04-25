/*
 * lineData.h
 *
 *  Created on: 25/04/2013
 *      Author: facetoe
 */

#ifndef LINEDATA_H_
#define LINEDATA_H_

#include "defines.h"
#include "dynamicArray.h"
#include "linkedList.h"

/* List to hold lines */
struct line {
	int lNum;
	int lSize;
	char *currLine;
	struct line *next;
	struct line *prev;
};

typedef struct line LINE;

/* List to hold parsed messages */
struct lineData {
	int numLines;
	int messageNum;
	long numChars;

	LINE *first;
	LINE *last;
	LINE *lines;

	struct lineData *next;
	struct lineData *prev;
	struct lineData *root;

};

typedef struct lineData LINEDATA;


/* Allocates memory for a new LINE node and sets default values */
LINE *line_getLine();

/* Allocates memory for a LINEDATA list and initializes defualt values */
void lineData_init(LINEDATA **ld);

/* Returns a new LINEDATA node */
LINEDATA *lineData_getNode(LINEDATA *ld);

/* Checks if we have already parsed this message into the LINEDATA list */
bool lineData_hasMessage(LINEDATA *ld, int msgNum);

/* Updates the ld pointer to point to a specific message.
 * If the message doesn't exist leaves the pointer unchanged */
void lineData_getMessage(LINEDATA **ld, int msgNum);

/* Parses a listNode into a LINEDATA list.
 * If the message already exists in the list it does nothing */
void lineData_parseMessage(listNode *list, LINEDATA **ld);

/* Free all memory in the LINEDATA list */
void lineData_destroy(LINEDATA **ld);

/* Print stats for debugging */
void printLineStats(LINEDATA *ld);

void printRange(LINEDATA *ld, WINDOW *win, int start, int end);



#endif /* LINEDATA_H_ */
