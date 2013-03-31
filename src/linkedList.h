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

typedef struct node node_t;

struct message {
	int num;
	int size;

	char time[MAX_TIME_SIZE];
	char path[MAX_PATH_SIZE];

	struct message *next;
	struct message *rootM;

	node_t *rootN;
	node_t *message;
} message;

typedef struct message msgList;

/* Allocates memory for initial nodes and sets default values. */
void msgList_init(msgList **root);

/* Allocates memory for a new node, sets values and returns initialized node */
msgList *msgList_getNode(msgList *ml);

/* Returns the length of the list */
int listLength(msgList *ml);

/* Returns the message length */
int messageLength(node_t *msg);

/* Inserts a string into a msgList node */
void msgList_insertString(msgList *ml, char *str);

/* Appends message to the end of the list */
void msgList_appendMessage(msgList *ml, char *str);

/* Prints current note according to args. Args are:
 * n: Note number
 * p: Path
 * t: Time
 * m: Message */
void printMessage(FILE *outStream, char *args, msgList *ml);

/* Prints all messages with all information */
void printAll(FILE *outStream, msgList *ml);

/* Destroys the list freeing all memory */
void msgList_destroy(msgList *ml);

/* Moves list pointer to last node in the list.
 * If it is already the last node then leaves the pointer unchanged. */
void lastNode(msgList **ml);

/* Moves list pointer to the first node in the list.
 * If the first node is the root node then leaves the pointer unchanged. */
void firstNode(msgList **ml);

/* Moves the list pointer to the next node in the list. If it is the last node,
 * moves the pointer to the first node: ie, head->next to skip root node.
 * If there is only one node in the list (not counting root) it returns currP unchanged. */
void next(msgList **ml);

/* Moves the list pointer to the previous node in the list.
 * If there is only one node (not counting root) leaves the pointer unchanged.  */
void previous(msgList **ml);

/* Searches for node with noteNum.
 * Returns node if found, otherwise returns NULL. */
msgList *searchByNoteNum(msgList *ml, int noteNum);

/* Reorders the noteNums */
void orderList(msgList *ml);

/* Delete a node by noteNum */
void deleteNode(msgList *ml, int noteNum);

/* Writes the note structs to a file */
void writeBinary(FILE *fp, msgList *ml);

/* Reads the note data from a file and places in struct */
void readBinary(FILE *fp, msgList *ml);

/* Deletes all notes freeing all the memory. */
void deleteAll(msgList **ml);

/* Attempts to read a saved list from path. If no file is found, attempts to create one.*/
/* Returns true on success or false on failure. */
bool loadList(msgList *ml);

/* Attempts to save the list at path. */
/* Returns true on success or false on failure. */
bool saveList(msgList *ml);



#endif /* LINKEDLIST_H_ */
