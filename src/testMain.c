
#include "defines.h"
#include "helperFunctions.h"
#include "linkedList.h"
#include "dynamicArray.h"
//#include "nonInteractive.h"
//#include "ui.h"

struct line {
	int lNum;
	int lSize;
	char *currLine;
	struct line *next;
	struct line *prev;
};

typedef struct line LINE;

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


listNode *list = NULL;

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

bool lineData_hasMessage(LINEDATA *ld, int msgNum) {
	ld = ld->root;
	if( !ld->next )
		return false;

	ld = ld->next;

	for (; ld ; ld = ld->next) {
		if( ld->messageNum == msgNum )
			return true;
	}
	return false;
}

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

/* Parses a listNode and separates each line into a LINE node for displaying in ncurses */
void lineData_parseMessage(LINEDATA **ld) {
	if( list->num == 0 || lineData_hasMessage(*ld, list->num))
		return;

	LINEDATA *lineData = *ld;

	/* Create an initialize a dynamic array for the line buffer */
	dArr *lineBuffer;
	dArr_init(&lineBuffer);

	lineData->next = lineData_getNode(*ld);
	lineData = lineData->next;

	noteNode *currMsg = list->message;

	LINE *prevLine = NULL;
	LINE *line = line_getLine();
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

			line->currLine = malloc(sizeof(char) * lineBuffer->currSize);
			strcpy(line->currLine, lineBuffer->contents);
			if( !line->currLine ) {
				fprintf(stderr, "Unable to allocate memory for new line in lineData_parseMessage\n");
				abort();
			}
			line->lSize = lineBuffer->currSize;
			line->lNum = lineCnt;
			line->next = line_getLine();
			line->prev = prevLine;
			prevLine = line;

			line = line->next;

			/* Clear the buffer for the next iteration */
			dArr_clear(&lineBuffer);
		}
	}
	lineData->last = prevLine;
	lineData->numLines = lineCnt;
	lineData->numChars = chars;
	/* Free the line buffer */
	dArr_destroy(&lineBuffer);

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

void printLineStats(LINEDATA *ld) {
	ld = ld->root;
	if( !ld->next )
		return;

	ld = ld->next;
	for (; ld ; ld = ld->next)
		printf("Lines: %d\nChars %ld\nNum: %d\n\n", ld->numLines, ld->numChars, ld->messageNum);
}

void printRange(LINEDATA *ld, int start, int end) {
	LINE *tmp = ld->lines;

	if( !tmp )
	{
		fprintf(stderr, "Error in print range: No lines...");
	}

	if( start == 0 )
		start = 1;

	if( tmp->lNum > start ) {
		for (; tmp->lNum >= start && tmp->prev ; tmp = tmp->prev);

		for (; tmp->lNum <= end && tmp->next; tmp = tmp->next) {
			printf("%s", tmp->currLine);
		}

	} else {
		for (; tmp->lNum <= start && tmp->next; tmp = tmp->next);

		for (; tmp->lNum <= end && tmp->next; tmp = tmp->next) {
			printf("%s", tmp->currLine);
		}
	}
	ld->lines = tmp;
}

int main(int argc, char **argv) {

	path = "/home/facetoe/.terminote.data";
	list_init(&list);
	list_load(list);

	LINEDATA *lineData, *root;

	lineData_init(&lineData);
	root = lineData;
	list_next(&list);
	lineData_parseMessage(&lineData);

	list_next(&list);
	lineData_parseMessage(&lineData);
	list_next(&list);
	lineData_parseMessage(&lineData);
	list_next(&list);
	lineData_parseMessage(&lineData);
	list_next(&list);
	lineData_parseMessage(&lineData);
	list_next(&list);
	lineData_parseMessage(&lineData);
	list_next(&list);
	lineData_parseMessage(&lineData);
	list_next(&list);
	lineData_parseMessage(&lineData);
	list_next(&list);
	lineData_parseMessage(&lineData);
	list_next(&list);
	lineData_parseMessage(&lineData);
	list_next(&list);
	lineData_parseMessage(&lineData);
	list_next(&list);
	lineData_parseMessage(&lineData);
	list_next(&list);
	lineData_parseMessage(&lineData);

	if(lineData_hasMessage(lineData, 4))
		printf("Yes");

	printLineStats(lineData);

	lineData_getMessage(&lineData, 4);

	printRange(lineData, 0, 50);

	lineData_destroy(&root);


	return 0;
}
