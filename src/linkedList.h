/*
 ============================================================================
 Name        : linkedList.c
 Author      : Facetoe
 Version     :
 Copyright   : copy right!
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "helperFunctions.h"

#define MAX_MESSAGE_SIZE 1000
#define TIME_SIZE 30
#define MAX_PATH_SIZE 200
#define DEBUG 1

struct Node {
	int note_num;
	int *size;
	char message[MAX_MESSAGE_SIZE];
	char time[TIME_SIZE];
	char path[MAX_PATH_SIZE];

	struct Node *next;
};

typedef struct Node node;


/* Initialize all the elements to prevent problems later */
void init(node *head, node *currP) {
	if( DEBUG )
		printf("Initializing list\n");

	currP->note_num = 0;

	int *sizeP = 0;
	currP->size = sizeP;

	strcpy(currP->time, "");
	strcpy(currP->message, "");
	strcpy(currP->path, "");

	currP->next = NULL;
}

/* Creates and initializes the list */
void create_list(node **head, node **currP) {
	if( DEBUG )
		printf("Creating list\n");

	/* Allocate memory for the head */
	*head = (node *) malloc(sizeof(node));
	*currP = *head;

	/* Initialize all the elements */
	init(*head, *currP);
}

/* Appends a note to the end of the list */
void append(node *currP, char *noteText)
{
	int old_note_num;

	while( currP->next != NULL)
	{
		currP=currP->next;
	}

	/* Allocate memory for new node */
	currP->next = (node *) calloc( 1, sizeof(node) );

	/* Store old note_num */
	old_note_num = currP->note_num;

	/* Move to newly created node */
	currP = currP->next;

	/* Update note_num */
	currP->note_num = ++old_note_num;

	/* Get and store path information */
	char *path = getcwd(NULL, 0);
	if (path == NULL)
	{
		perror("Unable to retrieve path\n");
	} else {
		size_t path_len = strlen(path);
		strncpy(currP->path, path, path_len);
		free(path);
	}

	/* Get and store time information */
	char *time = current_time();
	if ( time == NULL)
	{
		perror("Unable to retrieve time\n");
	} else {

		strip_newline(time);
		size_t time_len = strlen(time);
		strncpy(currP->time, time, time_len);
	}

	if (noteText)
	{
		size_t note_len = strlen(noteText);
		if (note_len >= MAX_MESSAGE_SIZE)
		{
			perror("Note too large, truncating.\n");
			char tmp[MAX_MESSAGE_SIZE];

			/* Copy note to tmp with room for terminator */
			strncpy(tmp, noteText, MAX_MESSAGE_SIZE-1);

			/* Terminate and copy to node */
			tmp[MAX_MESSAGE_SIZE] = '\0';
			strncpy(currP->message, tmp, MAX_MESSAGE_SIZE);

		} else {
			strncpy(currP->message, noteText, note_len);
		}


	}
	if( DEBUG )
		printf("Inserted <%s> in %d\n", currP->message, currP->note_num);
}

/* Returns a pointer to the next note in the list. If currP is the last note,
 * returns a pointer to the first note: ie, head->next to skip root node.
 */
node *next(node *head, node *currP)
{
	if ( currP->next != NULL)
		return currP->next;
	else
		return head->next;
}

/* Returns the previous note in the list */
node *previous(node *head, node *currP)
{
	int noteNum = currP->note_num;


	if( noteNum == 1)
	{
		/* We are at the start of the list, so loop to the end */
		while(currP->next != NULL)
			currP = currP->next;

		/* And return a pointer to the end */
		return currP;
	} else {
		/* We are not at the start, so reset currP to head */
		currP = head;

		/* And loop through to noteNum -1, ie the previous note */
		while( currP->next != NULL && currP->note_num != noteNum-1)
			currP = currP->next;

		/* Return the previous note */
		return currP;
	}
}

int length( node *currP )
{
	int size = 0;
	while(currP)
	{
		size++;
		currP = currP->next;
	}
	return size;
}

void orderList(node *currP)
{
	/* Don't count root node */
	currP = currP->next;

	int num = 0;
	while(currP)
	{
		currP->note_num = ++num;
		currP = currP->next;
	}
}

/* Delete a node by noteNum */
/* Note: this function leaves the list unordered,
 * you need to call orderList() on it after using it.
 */
void deleteNote(node *currP, int noteNum)
{
	/* Don't delete root node */
	if ( noteNum == 0 )
		return;

	node *tmp;

	while(currP)
	{
		/* Go to the note before the one to be deleted */
		if (currP->note_num == noteNum-1)
		{
			/* tmp points to node to be deleted */
			tmp = currP->next;

			/* Link currP to node after tmp, leaving a hole */
			currP->next = tmp->next;

			/* Free tmp */
			if (tmp != NULL)
				free(tmp);
			return;

		} else {
			tmp = currP;
			currP = currP->next;
		}
	}
}

void destroy(node *head)
{
	node *tmp;

	/* Loop through the list freeing all the memory */
	while(head != NULL)
	{
		if( DEBUG )
			printf("Freeing Note #%d\n", head->note_num);

		tmp = head->next;
		if(head)
			free(head);
		head = tmp;
	}

}

/* Deletes all notes */
void deleteAll(node *head)
{
	node *tmp;

	/* Don't delete root node */
	tmp = head->next;
	while( tmp != NULL)
	{
		head->next = tmp->next;
		if( DEBUG )
			printf("Deleting Note #%d\n", tmp->note_num);

		if ( tmp )
			free(tmp);
		tmp = head->next;
	}
}

void printList(node *currP)
{
	/* Don't print root node */
	if (currP->note_num == 0)
		currP = currP->next;

	/* Loop through list printing everything */
	while(currP != NULL)
	{
		printf("NoteNum: %d"
				"\nPath: %s"
				"\nTime: %s"
				"\nMessage: %s\n\n",
				currP->note_num,
				currP->path,
				currP->time,
				currP->message);

		currP=currP->next;
	}
}

