/*
 * terminote.c
 *
 *  Created on: Feb 21, 2013
 *      Author: fragmachine
 */

#include "linkedList.h"

int main(void) {

	node *head, *currP;
	create_list(&head, &currP);

	append(currP, "String 1");
	append(currP, "String 2");
	append(currP, "String 3");
	append(currP, "Final String");

	currP = head;

	deleteNote(currP, 3);
	orderList(currP);
	printList(currP);

	deleteAll(currP);
	printList(currP);
	destroy(head);

	return 0;
}
