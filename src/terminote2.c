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

	char *path = "/Users/fragmachine/Desktop/terminoteData";

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
	saveList(currP, path);
	destroy(currP);







	return 0;
}
