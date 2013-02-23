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
	int lastNoteNum;
	menuMessage();

	while (loop) {
		scanf("\n%c", &opt);
		lastNoteNum = currP->note_num;

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
				/* Don't print the note if nothing changed. */
				if( lastNoteNum != currP->note_num )
					printCurrent(currP);
			}
			break;

		/* Previous */
		case 'a':
			if ( listLength(currP) )
			{
				currP=previous(head, currP);
				/* Don't print the note if nothing changed. */
				if ( lastNoteNum != currP->note_num )
					printCurrent(currP);
			}
			break;

		/* Get input and append to list */
		case 'w':
			appendNote(inputBuffer, MAX_MESSAGE_SIZE, currP, head);
		break;

		/* Print */
		case 'f':
			printList(currP);
			break;

		/* Delete all notes */
		case 'g':
			deleteAllNotes(currP, head);
			break;

		/* Find and print all notes containing search term */
		case 'e':
			printAllWithSubString(currP, head, inputBuffer);
			break;

		case 'm':
			menuMessage();
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

