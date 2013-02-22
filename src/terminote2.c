/*
 * terminote.c
 *
 *  Created on: Feb 21, 2013
 *      Author: fragmachine
 */

#include "linkedList.h"
#include "helperFunctions.h"

int main(void) {

	node *head, *currP;
	create_list(&head, &currP);

	currP = head;

	int pathBuffLen = 200;
	char pathBuff[pathBuffLen];
	char *path;
	if ( getDataPath(pathBuff, pathBuffLen, "terminote.data") )
		path = pathBuff;

	append(currP, "string1");
	append(currP, "string2");
	append(currP, "string3");
	append(currP, "string4");
	append(currP, "string5");
	append(currP, "string6");



	printf("size = %d\n", length(currP));
	saveList(currP, path);
	destroy(currP);

	create_list(&head, &currP);
	printf("size = %d\n", length(currP));
	loadList(currP, path);

	printList(currP);
	destroy(currP);









	return 0;
}
