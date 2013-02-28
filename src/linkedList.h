/*
 * linkedList.h
 *
 *  Created on: Feb 22, 2013
 *      Author: fragmachine
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "defines.h"

struct Node {
	int note_num;
	char message[MAX_MESSAGE_SIZE];
	char time[TIME_SIZE];
	char path[MAX_PATH_SIZE];

	struct Node *next;
};

typedef struct Node node;

/* Initialize all the elements to prevent problems later */
void init(node *currP);

/* Creates and initializes the list */
void create_list(node **head, node **currP);

/* Appends a note to the end of the list */
node *append(node *currP, char *noteText);

/* Returns the last node in the list. If currP is the root node, returns NULL */
node *lastNode(node *currP, node *head);

/* Returns the first node in the list. If the first node is the root node, returns NULL */
node *firstNode(node *currP, node *head);

/* Returns the next node in the list. If currP is the last node,
 * returns a pointer to the first node: ie, head->next to skip root node.
 * If there is only one node in the list (not counting root) it returns currP unchanged. */
node *next(node *head, node *currP);

/* Returns the previous node in the list. If there is only one node (not counting root) returns currP unchanged.  */
node *previous(node *head, node *currP);

/* Searches for node with noteNum.
 * Returns node if found, otherwise returns NULL. */
node *searchByNoteNum(node *currP, node *head, int noteNum);

/* Returns the length of the list */
int listLength(node *currP);

/* Reorders the noteNums */
void orderList(node *currP);

/* Delete a node by noteNum */
/* Warning: this function leaves the list unordered,
 * you need to call orderList() after using it. */
void deleteNode(node *currP, node *head, int noteNum);

/* Deletes all notes */
/* Note: This function affects currP, so after using it you need to reset currP to head */
void deleteAll(node *currP);

/* Destroys the list freeing all memory */
void destroy(node *head);

/* Prints every note */
void printList(node *currP);

/* Writes the note structs to a file */
void writeBinary(FILE *fp, node *head);

/* Reads the note data from a file and places in struct */
void readBinary(FILE *fp, node *head);

/* Attempts to read a saved list from path. If no file is found, attempts to create one.*/
/* Returns true on success or false on failure. */
bool loadList(node *head);

/* Attempts to save the list at path. 			*/
/* Returns true on success or false on failure. */
bool saveList(node *head);

/* Searches node->message for substring. Returns true if it does, false if not */
bool hasSubstring( node *currP, char subString[]);
#endif /* LINKEDLIST_H_ */
