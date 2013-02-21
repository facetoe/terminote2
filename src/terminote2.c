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

	insert(&head, currP, "string");
	insert(&head, currP, "STring");
	insert(&head, currP, "Party");

	destroy(head);

	return 0;
}
