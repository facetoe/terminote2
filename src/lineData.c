/*
 * lineData.c
 *
 *  Created on: 25/04/2013
 *      Author: facetoe
 */


#include "lineData.h"
#include "defines.h"


/* Allocates memory for a new LINE node and sets default values */
LINE *line_getLine() {
	LINE *tmp = NULL;
	tmp = malloc(sizeof(LINE));
	if(!tmp) {
		fprintf(stderr, "Failed to allocate memory in line_getline\n");
		abort();
	}

	tmp->lNum = 0;
	tmp->lSize = 0;
	tmp->currLine = NULL;
	tmp->next = NULL;
	tmp->prev = NULL;

	return tmp;
}

/* Allocates memory for a LINEDATA list and initializes defualt values */
void lineData_init(LINEDATA **ld) {
	LINEDATA *tmp = NULL;
	tmp = malloc(sizeof(LINEDATA));

	if ( !tmp ) {
		fprintf(stderr, "Unable to allocate memory in lineData_init.\n");
		abort();
	}

	tmp->numLines = 0;
	tmp->numChars = 0;
	tmp->messageNum = 0;
	tmp->first = NULL;
	tmp->last = NULL;
	tmp->next = NULL;
	tmp->prev = NULL;
	tmp->root = tmp;
	tmp->lines = NULL;

	*ld = tmp;
}

/* Returns a new LINEDATA node */
LINEDATA *lineData_getNode(LINEDATA *ld) {
	LINEDATA *tmp = NULL;
	tmp = malloc(sizeof(LINEDATA));

	if ( !tmp ) {
		fprintf(stderr, "Unable to allocate memory in lineData_init.\n");
		abort();
	}

	tmp->numLines = 0;
	tmp->numChars = 0;
	tmp->messageNum = 0;
	tmp->first = NULL;
	tmp->last = NULL;
	tmp->next = NULL;
	tmp->prev = NULL;
	tmp->lines = NULL;
	tmp->root = ld->root;

	return tmp;
}

/* Checks if we have already parsed this message into the LINEDATA list */
bool lineData_hasMessage(LINEDATA *ld, int msgNum) {
	ld = ld->root;

	/* If there is no next then we are in the root node */
	if( !ld->next )
		return false;

	ld = ld->next;

	for (; ld ; ld = ld->next) {
		if( ld->messageNum == msgNum )
			return true;
	}
	return false;
}

/* Updates the ld pointer to point to a specific message.
 * If the message doesn't exist leaves the pointer unchanged */
void lineData_getMessage(LINEDATA **ld, int msgNum) {
	LINEDATA *tmp = *ld;
	tmp = tmp->root;

	if( !tmp->next )
		return;

	tmp = tmp->next;
	for (; tmp; tmp = tmp->next) {
		if( tmp->messageNum == msgNum ) {
			*ld = tmp;
			return;
		}
	}

}

/* Parses a listNode into a LINEDATA list.
 * If the message already exists in the list it does nothing */
void lineData_parseMessage(listNode *list, LINEDATA **ld) {
	if( list->num == 0 || lineData_hasMessage(*ld, list->num))
		return;

	LINEDATA *lineData = *ld;

	/* Create an initialize a dynamic array for the line buffer */
	dArr *lineBuffer;
	dArr_init(&lineBuffer);

	/* Grab a new node and move to it */
	lineData->next = lineData_getNode(*ld);
	lineData = lineData->next;

	noteNode *currMsg = list->message;

	/* Get and set the new LINE structure */
	LINE *line = line_getLine();
	LINE *prevLine = NULL;
	lineData->first = line;
	lineData->lines = line;
	lineData->messageNum = list->num;

	int lineCnt = 0;
	long chars = 0;

	for (; currMsg ; currMsg = currMsg->next) {
		/* If the current character isn't a newline or null terminator then add to buffer */
		if (currMsg->ch != '\n' && currMsg->ch != '\0') {
			chars++;
			dArr_add(lineBuffer, currMsg->ch);
		} else {

			/* Add a newline and terminator so the line will display correctly in ncurses */
			dArr_add(lineBuffer, '\n');
			dArr_add(lineBuffer, '\0');

			chars++;
			lineCnt++;

			/* Allocate enough memory to accomodate the line waiting in the lineBuffer and copy it into currLine */
			line->currLine = malloc(sizeof(char) * lineBuffer->currSize);
			strcpy(line->currLine, lineBuffer->contents);
			if( !line->currLine ) {
				fprintf(stderr, "Unable to allocate memory for new line in lineData_parseMessage\n");
				abort();
			}

			/* Set the line variables */
			line->lSize = lineBuffer->currSize;
			line->lNum = lineCnt;
			line->prev = prevLine;
			prevLine = line;

			/* Get a new LINE node and move to it */
			line->next = line_getLine();
			line = line->next;

			/* Clear the buffer for the next iteration */
			dArr_clear(&lineBuffer);
		}
	}

	/* Set the last of the LINEDATA variables */
	lineData->last = prevLine;
	lineData->numLines = lineCnt;
	lineData->numChars = chars;

	/* Free the line buffer */
	dArr_destroy(&lineBuffer);

	/* Done */
	*ld = lineData;
}

/* Free all memory in the LINEDATA list */
void lineData_destroy(LINEDATA **ld) {
	LINE *line, *tmpLine;
	LINEDATA *lineData, *tmpLD;
	lineData = *ld;
	lineData = lineData->root;
	while (lineData) {
		line = lineData->first;
		while (line) {
			tmpLine = line->next;
			free(line->currLine);
			free(line);
			line = tmpLine;
		}
		tmpLD = lineData->next;
		free(lineData);
		lineData = tmpLD;
	}
	free(line);
	free(lineData);
	*ld = NULL;
}

/* Print stats for debugging */
void printLineStats(LINEDATA *ld) {
	ld = ld->root;
	if( !ld->next )
		return;

	ld = ld->next;
	for (; ld ; ld = ld->next)
		printf("Lines: %d\nChars %ld\nNum: %d\n\n", ld->numLines, ld->numChars, ld->messageNum);
}

void printRange(LINEDATA *ld, WINDOW *win,  int start, int end) {
	LINE *tmp = ld->lines;

	if( !tmp )
	{
		fprintf(stderr, "Error in print range: No lines...");
	}

	if( start == 0 )
		start = 1;

	if( tmp->lNum >= start ) {
		for (; tmp->lNum >= start && tmp->prev ; tmp = tmp->prev);

		for (; tmp->lNum <= end && tmp->next; tmp = tmp->next) {
			waddstr(win, tmp->currLine);
		}

	} else {
		for (; tmp->lNum <= start && tmp->next; tmp = tmp->next);

		for (; tmp->lNum <= end && tmp->next; tmp = tmp->next) {
			waddstr(win, tmp->currLine);
		}
	}
	wrefresh(win);
	ld->lines = tmp;
}
