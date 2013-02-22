/*
 * terminote.c
 *
 *  Created on: Feb 21, 2013
 *      Author: fragmachine
 */

#include "linkedList.h"
#include <fcntl.h>

int main(void) {

	node *head, *currP;
	create_list(&head, &currP);

	currP = head;

	int pathBuffLen = 200;
	char pathBuff[pathBuffLen];
	char *path;
	if ( getDataPath(pathBuff, pathBuffLen, "terminote.data") )
		path = pathBuff;
	currP = head;

	append(currP, "string1");
	append(currP, "String2");
	append(currP, "String3");
	append(currP, "String4");
	append(currP, "String5");


	saveList(currP, path);
	//deleteAll(currP);
	//loadList(currP, path);
	deleteAll(currP);
	currP=head;
	saveList(currP, path);
	destroy(currP);







	return 0;
}
