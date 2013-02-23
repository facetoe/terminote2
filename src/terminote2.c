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

volatile int keepRunning = 1;
int main(void) {

	node *head, *currP;
	create_list(&head, &currP);

	char *path;
	if ( getDataPath(pathBuffer, MAX_PATH_SIZE, "terminote.data") )
		path = pathBuffer;
	else
	{
		fprintf(stderr, "Error retrieving path\nAborting\n");
		return 1;
	}

	loadList(currP, path);

	signal(SIGINT, sigintHandler);

	while(keepRunning)
		uiLoop(currP, head);

	printf("Printed %d\n", keepRunning);

	currP=head;
	saveList(currP, path);
	destroy(currP);

	return 0;
}

