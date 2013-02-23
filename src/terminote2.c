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

	uiLoop(currP, head);

	currP=head;
	saveList(currP, path);
	destroy(currP);

	return 0;
}

