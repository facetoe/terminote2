/*
 ============================================================================
 Name        : linkedList.c
 Author      : Facetoe
 Description : Linked list library for terminote
 ============================================================================
 */

#include "linkedList.h"
#include "helperFunctions.h"


/* Initialize all the elements to prevent problems later */
void init(node *head, node *currP) {
	if (DEBUG)
		printf("Initializing list\n");

	currP->note_num = 0;

	strcpy(currP->time, "");
	strcpy(currP->message, "");
	strcpy(currP->path, "");

	currP->next = NULL;
}

/* Creates and initializes the list */
void create_list(node **head, node **currP) {
	if (DEBUG)
		printf("Creating list\n");

	/* Allocate memory for the head */
	*head = (node *) calloc(1, sizeof(node));
	*currP = *head;

	/* Initialize all the elements */
	init(*head, *currP);
}

/* Appends a note to the end of the list */
node *append(node *currP, char *noteText) {
	int old_note_num;

	while (currP->next != NULL ) {
		currP = currP->next;
	}

	/* Allocate memory for new node */
	currP->next = (node *) calloc(1, sizeof(node));

	/* Store old note_num */
	old_note_num = currP->note_num;

	/* Move to newly created node */
	currP = currP->next;

	/* Update note_num */
	currP->note_num = ++old_note_num;

	/* Get and store path information */
	char *path = getcwd(NULL, 0);
	if (path == NULL ) {
		fprintf(stderr, "Unable to retrieve path\n");
	} else {
		size_t path_len = strlen(path);
		strncpy(currP->path, path, path_len);
		free(path);
	}

	/* Get and store time information */
	char *time = current_time();
	if (time == NULL ) {
		perror("Unable to retrieve time\n");
	} else {

		strip_newline(time);
		size_t time_len = strlen(time);
		strncpy(currP->time, time, time_len);
	}

	if (noteText) {
		size_t note_len = strlen(noteText);
		if (note_len >= MAX_MESSAGE_SIZE) {
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
	if (DEBUG)
		printf("Inserted <%s> in %d\n", currP->message, currP->note_num);
	return currP;
}

/* Returns the last node in the list. If currP is the root node, returns NULL */
node *lastNode(node *currP, node *head)
{
	currP = head;
	if ( currP->next == NULL)
		return NULL;

	while ( currP->next )
		currP = currP->next;
	return currP;
}

/* Returns the first node in the list. If the first node is the root node, returns NULL */
node *firstNode(node *currP, node *head)
{
	currP = head;
	if ( currP->next )
		return currP->next;
	else
		return NULL;
}

/* Returns the next node in the list. If currP is the last node,
 * returns a pointer to the first node: ie, head->next to skip root node.
 * If there is only one node in the list (not counting root) it returns currP unchanged. */
node *next(node *head, node *currP) {
	if (currP->next != NULL ) {
		if (DEBUG)
			printf("Next to: %d\n", (currP->next)->note_num);
		return currP->next;
	} else {
		if (DEBUG)
			printf("Next to: %d\n", (head->next)->note_num);
		return head->next;
	}
}

/* Returns the previous node in the list. If there is only one node (not counting root) returns currP unchanged.  */
node *previous(node *head, node *currP) {
	int noteNum = currP->note_num;

	if (noteNum == 1) {
		/* We are at the start of the list, so loop to the end */
		while (currP->next != NULL )
			currP = currP->next;

		if (DEBUG)
			printf("Previous to: %d\n", currP->note_num);

		/* And return a pointer to the end */
		return currP;
	} else {
		/* We are not at the start, so reset currP to head */
		currP = head;

		/* And loop through to noteNum -1, ie the previous node */
		while (currP->next != NULL && currP->note_num != noteNum - 1)
			currP = currP->next;

		if (DEBUG)
			printf("Previous to: %d\n", currP->note_num);

		/* Return the previous node */
		return currP;
	}
}

/* Searches for node with noteNum.
 * Returns node if found, otherwise returns NULL. */
node *searchByNoteNum(node *currP, node *head, int noteNum)
{
	/* Reset to first node */
	currP = head;

	/* Nothing to search if the list is empty */
	if ( listLength(currP) == 0)
		return NULL;

	/* don't check root node */
	if ( currP->note_num == 0 )
		currP = currP->next;

	while(currP->note_num != noteNum && currP->next != NULL)
		currP = currP->next;

	if (currP->note_num == noteNum)
		return currP;
	else
		return NULL;

	return currP;
}

/* Returns the length of the list */
int listLength(node *currP) {

	/* Don't count the root node */
	if (currP->note_num == 0)
		currP = currP->next;

	int size = 0;
	while (currP) {
		size++;
		currP = currP->next;
	}
	return size;
}

/* Reorders the noteNums */
void orderList(node *currP) {
	/* Don't count root node */
	currP = currP->next;

	int num = 0;
	while (currP) {
		currP->note_num = ++num;
		currP = currP->next;
	}
	if (DEBUG)
		printf("Ordered list\n");
}

/* Delete a node by noteNum */
/* Warning: this function leaves the list unordered,
 * you need to call orderList() after using it. */
void deleteNode(node *currP, node *head, int noteNum) {
	currP=head;
	/* Don't delete root node */
	if (noteNum == 0)
		return;

	node *tmp;

	while (currP) {
		/* Go to the node before the one to be deleted */
		if (currP->note_num == noteNum - 1) {

			/* tmp points to node to be deleted */
			tmp = currP->next;

			if (DEBUG)
				printf("Deleting note #%d\n", tmp->note_num);

			/* Link currP to node after tmp, leaving a hole */
			currP->next = tmp->next;

			/* Free tmp */
			if (tmp != NULL )
				free(tmp);

			return;
		} else {
			tmp = currP;
			currP = currP->next;
		}
	}
}

/* Deletes all notes */
/* Note: This function affects currP, so after using it you need to reset currP to head */
void deleteAll(node *currP) {
	node *tmp;

	/* Don't delete root node */
	tmp = currP->next;
	while (tmp != NULL ) {
		currP->next = tmp->next;
		if (DEBUG)
			printf("Deleting Note #%d\n", tmp->note_num);

		if (tmp)
			free(tmp);
		tmp = currP->next;
	}
}

/* Destroys the list freeing all memory */
void destroy(node *head) {
	node *tmp;

	/* Loop through the list freeing all the memory */
	while (head != NULL ) {
		if (DEBUG)
			printf("Freeing Note #%d\n", head->note_num);

		tmp = head->next;
		if (head)
			free(head);
		head = tmp;
	}

}

/* Prints every note */
void printList(node *currP) {
	/* Don't print root node */
	if (currP->note_num == 0 && currP->next != NULL)
		currP = currP->next;

	if ( currP->note_num == 0 )
	{
		fprintf(stderr, "Nothing to print\n");
		return;
	}

	/* Loop through list printing everything */
	while (currP != NULL ) {
		printf("NoteNum: %d"
				"\nPath: %s"
				"\nTime: %s"
				"\nMessage: %s\n\n", currP->note_num, currP->path, currP->time,
				currP->message);

		currP = currP->next;
	}
}

/* Writes the note structs to a file */
void writeBinary(FILE *fp, node *head) {

	/* Don't write root node */
	if (head->note_num == 0)
		head = head->next;


	while (head) {
		if (DEBUG)
			printf("Writing Note #%d\n", head->note_num);

		/* Get the length +1 (for NULL terminator), write the data. Repeat */
		int len = strlen(head->message) + 1;
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(head->message, sizeof(char), len, fp);

		len = strlen(head->path) + 1;
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(head->path, sizeof(char), len, fp);

		len = strlen(head->time) + 1;
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(head->time, sizeof(char), len, fp);

		head = head->next;
	}
}

/* Reads the note data from a file and places in struct */
void readBinary(FILE *fp, node *head) {

	int len, note_num;
	note_num = 0;

	char message[MAX_MESSAGE_SIZE];
	char path[MAX_PATH_SIZE];
	char time[TIME_SIZE];

	while (fread(&len, sizeof(len), 1, fp)) {
		note_num++;
		if (DEBUG)
			printf("Reading Note #%d\n", note_num);

		/* Allocate memory for new note */
		head->next = (node *) malloc(sizeof(node));

		/* Move to new note and set note->next to NULL */
		head = head->next;
		head->next = NULL;

		/* We got the first length in the loop condition.... */
		fread(message, sizeof(char), len, fp);

		fread(&len, sizeof(len), 1, fp);
		fread(path, sizeof(char), len, fp);

		fread(&len, sizeof(len), 1, fp);
		fread(time, sizeof(char), len, fp);

		/* Insert strings into struct */
		strcpy(head->message, message);
		strcpy(head->path, path);
		strcpy(head->time, time);

		head->note_num = note_num;

	}
}

/* Attempts to read a saved list from path. If no file is found, attempts to create one.*/
/* Returns true on success or false on failure. */
bool loadList(node *head) {
	if (DEBUG)
		printf("Loading list from: %s\n", path);

	FILE *fp;

	if (file_exists(path) && (fp = fopen(path, "rb")) != NULL ) {
		readBinary(fp, head);
		fclose(fp);
		return true;
	} else {
		fprintf(stderr,
				"Error loading data file at: %s\nAttempting to create one...\n",
				path);

		fp = fopen(path, "wb");
		if (fp != NULL)
		{
			fprintf(stderr, "Successfully created file\n");
			fclose(fp);
			return true;

		} else {
			fprintf(stderr, "Failed to create file\n");
			return false;
		}
	}
}

/* Attempts to save the list at path. 			*/
/* Returns true on success or false on failure. */
bool saveList(node *head) {

	if (DEBUG)
		printf("Saving list at: %s\n", path);

	FILE *fp = NULL;
	if (file_exists(path) && (fp = fopen(path, "wb")) != NULL ) {
		writeBinary(fp, head);
		fclose(fp);
		return true;

	} else {
		fprintf(stderr,
				"Error loading data file at: %s\nAttempting to create one...\n",
				path);

		fp = fopen(path, "wb");
		if (fp != NULL)
		{
			fprintf(stderr, "Successfully created file\n");
			writeBinary(fp, head);
			fclose(fp);
			return true;

		} else {
			fprintf(stderr, "Failed to create file\n");
			return false;
		}
	}
}

/* Searches node->message for substring. Returns true if it does, false if not */
bool hasSubstring(node *currP, char subString[])
{
	/* Convert message to lower case */
	int msgLen = strlen(subString);
	char messageLower[msgLen];
	stringToLower(currP->message, messageLower, msgLen);

	/* Convert subString to lower case */
	int sbstrLen = strlen(subString);
	char subLower[sbstrLen];
	stringToLower(subString, subLower, sbstrLen);

	return  ( findSubstring(messageLower, subLower) != NULL );
}
