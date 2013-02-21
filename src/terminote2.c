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

	append(currP, "string");
	append(currP, "STring");
	append(currP, "Party");
	append(currP, "So many party time");

	//currP = head;

	for (int i = 0; i < 5; ++i) {
		currP=previous(head, currP);
		printf("%d\n", currP->note_num);
	}


	printf("Head: %d\n", head->note_num);


	destroy(head);

	return 0;
}
