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

void insert(node **head, node *currP, char *noteText)
{
	int old_note_num;

	while( currP->next != NULL)
	{
		currP=currP->next;
	}

	/* Allocate memory for new node */
	currP->next = (node *) malloc(sizeof(node));

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

