/*
 * terminote.c
 *
 *  Created on: Feb 21, 2013
 *      Author: fragmachine
 */

#include "linkedList.h"
#include "helperFunctions.h"
#include "terminoteFunctions.h"
#include "defines.h"


int main(void) {

	node *head, *currP;
	create_list(&head, &currP);

	currP = head;

	int pathBuffLen = 200;
	char pathBuff[pathBuffLen];
	char *path;
	if ( getDataPath(pathBuff, pathBuffLen, "terminote.data") )
		path = pathBuff;
	else
		fprintf(stderr, "Error loading note data\n");

	loadList(currP, path);



	char opt;
	int loop = 1;
	menuMessage();

	while (loop) {
		scanf("\n%c", &opt);

		switch (opt) {
		/* Exit */
		case 'q':
			loop = 0;
			break;

		/* Next */
		case 'd':
			if ( listLength(currP) )
			{
				currP=next(head, currP);
				printCurrent(currP);
			}
			break;

		/* Previous */
		case 'a':
			if ( listLength(currP) )
			{
				currP=previous(head, currP);
				printCurrent(currP);
			}
			break;

		/* Get input and append to list */
		case 'w':
			appendNote(inputBuffer, MAX_MESSAGE_SIZE, currP, head);
		break;

		case 'f':
			printList(currP);
			break;

		/* Delete all notes */
		case 'g':
			deleteAllNotes(currP, head);
			break;

		case 't':
			printCurrent(currP);
			break;

		default:
			printf("Invalid Entry\n");
			break;
		}
	}

	currP=head;
	saveList(currP, path);
	destroy(currP);

	return 0;
}

