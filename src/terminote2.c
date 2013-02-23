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

	char *path;
	if ( getDataPath(pathBuffer, MAX_PATH_SIZE, "terminote.data") )
		path = pathBuffer;
	else
		fprintf(stderr, "Error loading note data\n");

	loadList(currP, path);

	char opt;
	int loop = 1;
	int lastNoteNum = 0;
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
				if( lastNoteNum != currP->note_num && lastNoteNum != 0)
					printCurrent(currP);
			}
			break;

		/* Previous */
		case 'a':
			if ( listLength(currP) )
			{
				currP=previous(head, currP);
				/* Don't print the note if nothing changed. */
				if ( lastNoteNum != currP->note_num && lastNoteNum != 0)
					printCurrent(currP);
			}
			break;

		/* Get input and append to list */
		case 'w':
			currP=appendNote(inputBuffer, MAX_MESSAGE_SIZE, currP, head);
		break;

		/* Print */
		case 'e':
			printList(currP);
			break;

		/* Delete all notes */
		case 'g':
			deleteAllNotes(currP, head);
			currP=head;
			break;

		/* Find and print all notes containing search term */
		case 'f':
			printAllWithSubString(currP, head);
			break;

		/* Print menu message */
		case 'm':
			menuMessage();
			break;

		case 'c':
			currP=deleteCurrent(currP, head);
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

