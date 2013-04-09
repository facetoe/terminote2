/*
 * linkedList.h
 *
 *  Created on: Mar 31, 2013
 *      Author: fragmachine
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "defines.h"

struct node {
	char ch;
	int index;

	struct node *next;
	struct node *prev;
} node;

typedef struct node noteNode;

struct message {
	int num;
	int size;

	char time[MAX_TIME_SIZE];
	char path[MAX_PATH_SIZE];

	struct message *next;
	struct message *rootM;

	noteNode *rootN;
	noteNode *message;
} message;

typedef struct message listNode;

/* Allocates memory for initial nodes and sets default values. */
void list_init(listNode **root);

/* Allocates memory for a new node, sets values and returns initialized node */
listNode *list_getNode(listNode *ln);

/* Returns the length of the list */
int list_length(listNode *ln);

/* Returns the message length */
int list_messageLength(noteNode *msg);

/* Inserts a string into a msgList node */
void list_insertString(listNode *ln, char *str);

/* Appends message to the end of the list */
void list_appendMessage(listNode *ln, char *str);

/* Prints current note according to args. Args are:
 * n: Note number
 * p: Path
 * t: Time
 * m: Message */
void list_printMessage(FILE *outStream, char *args, listNode *ln);

/* Prints all messages with all information */
void list_printAll(FILE *outStream, listNode *ln);

/* Destroys the list freeing all memory */
void list_destroy(listNode *ln);

/* Moves list pointer to last node in the list.
 * If it is already the last node then leaves the pointer unchanged. */
void list_lastNode(listNode **ln);

/* Moves list pointer to the first node in the list.
 * If the first node is the root node then leaves the pointer unchanged. */
void list_firstNode(listNode **ln);

/* Moves the list pointer to the next node in the list. If it is the last node,
 * moves the pointer to the first node: ie, head->next to skip root node.
 * If there is only one node in the list (not counting root) it returns currP unchanged. */
void list_next(listNode **ln);

/* Moves the list pointer to the previous node in the list.
 * If there is only one node (not counting root) leaves the pointer unchanged.  */
void list_previous(listNode **ln);

/* Searches for node with noteNum.
 * Returns node if found, otherwise returns NULL. */
listNode *list_searchByNoteNum(listNode *ln, int noteNum);

/* Reorders the noteNums */
void list_orderList(listNode *ln);

/* Delete a node by noteNum */
void list_deleteNode(listNode *ln, int noteNum);

/* Writes the note structs to a file */
void list_writeBinary(FILE *fp, listNode *ln);

/* Reads the note data from a file and places in struct */
void list_readBinary(FILE *fp, listNode *ln);

/* Deletes all notes freeing all the memory. */
void list_deleteAll(listNode **ln);

/* Attempts to read a saved list from path. If no file is found, attempts to create one.*/
/* Returns true on success or false on failure. */
bool list_load(listNode *ln);

/* Attempts to save the list at path. */
/* Returns true on success or false on failure. */
bool list_save(listNode *ln);

/* Searches the listNode's message for substring. Returns true if it does,
 * false if not. */
bool list_messageHasSubstring(listNode *ln, char *subStr);

#endif /* LINKEDLIST_H_ */
