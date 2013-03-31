/*
 * linkedList.c

 *
 *  Created on: Mar 31, 2013
 *      Author: fragmachine
 */
#include "helperFunctions.h"
#include "linkedList.h"
#include "defines.h"


/* Allocates memory for initial nodes and sets default values. */
void msgList_init(msgList **root) {
	if (DEBUG)
		printf("Initializing list\n");

	node_t *tmpNode = malloc(sizeof(node_t));
	msgList *tmpMsg = malloc(sizeof(msgList));

	if (!tmpNode || !tmpMsg) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	} else {
		tmpNode->ch = '\0';
		tmpNode->index = -1;
		tmpNode->next = NULL;
		tmpNode->prev = NULL;

		strncpy(tmpMsg->time, "\0", MAX_TIME_SIZE);
		strncpy(tmpMsg->path, "\0", MAX_PATH_SIZE);
		tmpMsg->num = 0;
		tmpMsg->size = 0;
		tmpMsg->rootN = tmpNode;
		tmpMsg->rootM = tmpMsg;
		tmpMsg->message = tmpNode;
		tmpMsg->next = NULL;
	}
	*root = tmpMsg;
}

/* Allocates memory for a new node, sets values and returns initialized node */
msgList *msgList_getNode(msgList *ml) {
	msgList *newMsg = NULL;
	newMsg = malloc(sizeof(msgList));
	if ( !newMsg) {
		fprintf(stderr, "Failed to allocate memory in msgList_getNode()\n");
		exit(1);
	}

	newMsg->num = ml->num;
	newMsg->rootN = NULL;
	newMsg->rootM = ml->rootM;
	newMsg->message = NULL;
	newMsg->next = NULL;
	strncpy(newMsg->time, "\0", MAX_TIME_SIZE);
	strncpy(newMsg->path, "\0", MAX_PATH_SIZE);
	return newMsg;
}

/* Returns the length of the list */
int listLength(msgList *ml) {
	int listSize = 0;
	ml = ml->rootM;
	if ( ml->next ) {
		ml = ml->next; // Don't count root node.
		for (; ml ; ml = ml->next, listSize++);
	}
	return listSize;
}

/* Returns the message length */
int messageLength(node_t *msg) {
	int len = 0;
	for (; msg; msg = msg->next, len++);
	return len;
}


/* Inserts a string into a msgList node */
void msgList_insertString(msgList *ml, char *str) {
	node_t *prevNode = NULL;
	int cnt = 0;

	ml->message = malloc(sizeof(node_t));
	ml->num = ++ml->num;

	if (!ml->message) {
		fprintf(stderr,
				"Failed to allocate memory in msgList_insertString()\n");
		exit(1);
	}

	(ml->message)->ch = *str;
	(ml->message)->index = cnt;
	(ml->message)->prev = NULL;
	(ml->message)->next = NULL;
	ml->rootN = ml->message;
	ml->next = NULL;

	prevNode = ml->message;

	for (char *s = str + 1; *s; s++) {
		(ml->message)->next = malloc(sizeof(node_t));
		if (!(ml->message)->next) {
			fprintf(stderr,
					"Failed to allocate memory in msgList_insertString()\n");
			exit(1);
		}
		ml->message = (ml->message)->next;
		(ml->message)->prev = prevNode;
		(ml->message)->next = NULL;
		(ml->message)->index = ++cnt;
		(ml->message)->ch = *s;
		prevNode = ml->message;
	}
	ml->message = ml->rootN;
	ml->size = messageLength(ml->message);
	if (DEBUG)
		printf("Added: %s\n", str);
}

/* Appends message to the end of the list */
void msgList_appendMessage(msgList *ml, char *str) {

	for (; ml->next; ml = ml->next)
		;

	ml->next = msgList_getNode(ml);
	ml = ml->next;
	msgList_insertString(ml, str);

	/* Get and store path information */
	char *path = getcwd(NULL, 0);
	if (path == NULL ) {
		fprintf(stderr, "Unable to retrieve path\n");
	} else {
		size_t path_len = strlen(path);
		strncpy(ml->path, path, path_len);
		free(path);
	}

	/* Get and store time information */
	char *time = current_time();
	if (time == NULL ) {
		perror("Unable to retrieve time\n");
	} else {

		strip_newline(time);
		size_t time_len = strlen(time);
		strncpy(ml->time, time, time_len);
	}
}

/* Prints current note according to args. Args are:
 * n: Note number
 * p: Path
 * t: Time
 * m: Message */
void printMessage(FILE *outStream, char *args, msgList *ml) {
	if (ml == NULL || ml->num == 0)
		fprintf(outStream, "Nothing to print.\n");
	else {
		for (char *s = args; *s ; s++) {
			switch (*s) {
			case 'n':
				fprintf(outStream, "Note Number: %d\n", ml->num);
				break;
			case 'p':
				fprintf(outStream, "Path: %s\n", ml->path);
				break;
			case 't':
				fprintf(outStream, "Time: %s\n", ml->time);
				break;
			case 'm':
				fprintf(outStream, "Message:\n");
				node_t *message = ml->message;
				for (; message; message = message->next)
					fprintf(outStream, "%c", message->ch);
				fprintf(outStream, "\n\n");
				break;
			default:
				break;
			}
		}
	}
}

/* Prints all messages with all information */
void printAll(FILE *outStream, msgList *ml) {
	if( DEBUG )
		if ( !ml )
			fprintf(stderr, "Null pointer passed to printAll\n");

	ml = ml->rootM;
	if (!ml->next) {
		printf("Nothing to print.\n");
	} else {
		ml = ml->next;
		for (; ml; ml = ml->next) {
			printMessage(outStream, "nptm", ml);
		}
	}
}

/* Destroys the list freeing all memory */
void msgList_destroy(msgList *ml) {
	if( DEBUG )
		if ( !ml )
			fprintf(stderr, "Null pointer passed to msgList_destroy\n");

	msgList *tmpList = NULL;
	node_t *tmpMsg = NULL;
	node_t *currP = NULL;
	ml = ml->rootM;
	while ( ml ) {
		if( DEBUG )
			printf("Destroying: %d\n", ml->num);

		if (ml->message) {
			currP = ml->message;
			while ( currP ) {
				tmpMsg = currP->next;
				free(currP);
				currP = tmpMsg;
			}
		}
		ml->message = NULL;
		tmpList = ml->next;
		free(ml);
		ml = tmpList;
	}
}

/* Moves list pointer to last node in the list.
 * If it is already the last node then leaves the pointer unchanged. */
void lastNode(msgList **ml) {
	msgList *tmp = *ml;
	if (tmp->next == NULL )
		return;
	for (; tmp->next; tmp = tmp->next)
		;
	*ml = tmp;
}

/* Moves list pointer to the first node in the list.
 * If the first node is the root node then leaves the pointer unchanged. */
void firstNode(msgList **ml) {
	msgList *tmp = *ml;
	tmp = tmp->rootM;
	if (tmp->next)
		tmp = tmp->next;
	*ml = tmp;
}

/* Moves the list pointer to the next node in the list. If it is the last node,
 * moves the pointer to the first node: ie, head->next to skip root node.
 * If there is only one node in the list (not counting root) it returns currP unchanged. */
void next(msgList **ml) {
	msgList *tmp = *ml;
	if (tmp->next != NULL ) {
		tmp = tmp->next;
	} else if ((tmp->rootM)->next){
		tmp = (tmp->rootM)->next;
	}
	*ml = tmp;
}

/* Moves the list pointer to the previous node in the list.
 * If there is only one node (not counting root) leaves the pointer unchanged.  */
void previous(msgList **ml) {
	msgList *tmp = *ml;
	int noteNum = tmp->num;
	if (noteNum == 1) {
		/* We are at the start of the list, so grab the last node */
		lastNode(&tmp);
	} else {
		/* We are not at the start, so grab the first node */
		firstNode(&tmp);

		/* And loop through to noteNum -1, ie the previous node */
		for (;tmp->next != NULL && tmp->num != noteNum - 1; tmp = tmp->next);
	}
	*ml = tmp;
}

/* Searches for node with noteNum.
 * Returns node if found, otherwise returns NULL. */
msgList *searchByNoteNum(msgList *ml, int noteNum) {
	/* Reset to first node */
	ml = ml->rootM;

	/* Nothing to search if the list is empty */
	if (listLength(ml) == 0)
		return NULL ;

	/* don't check root node */
	ml = ml->next;

	for (;ml->num != noteNum && ml->next; ml = ml->next );

	if (ml->num == noteNum)
		return ml;
	else
		return NULL ;
}

/* Reorders the noteNums */
void orderList(msgList *ml) {
	int nNum = 1;
	/* Don't count root node */
	ml = ml->rootM;

	if( ml->next ) {
		ml = ml->next;
		for (; ml; ml = ml->next, nNum++) {
			ml->num = nNum;
		}
	}
	if (DEBUG)
		printf("Ordered list\n");
}

/* Delete a node by noteNum */
void deleteNode(msgList *ml, int noteNum) {
	ml = ml->rootM;
	/* Don't delete root node */
	if (noteNum == 0)
		return;

	msgList *tmpList;
	node_t *tmpMsg, *currP;

	while (ml) {
		/* Go to the node before the one to be deleted */
		if (ml->num == noteNum - 1) {

			/* tmpList points to node to be deleted */
			tmpList = ml->next;

			/* currP points to the message to be deleted */
			currP = (ml->next)->message;
			if (DEBUG)
				printf("Deleting note #%d\n", tmpList->num);

			/* Loop through the message freeing all the memory */
			while ( currP ) {
				if ( DEBUG )
					printf("RM: %c\n", currP->ch);
				tmpMsg = currP->next;
				free(currP);
				currP = tmpMsg;
			}

			/* Link ml to node after tmpList, leaving a hole */
			ml->next = tmpList->next;

			/* Free tmpList */
			if (tmpList != NULL )
				free(tmpList);

			/* Reorder the list */
			orderList(ml);
			return;
		} else {
			tmpList = ml;
			ml = ml->next;
		}
	}
}

/* Writes the note structs to a file */
void writeBinary(FILE *fp, msgList *ml) {
	ml = ml->rootM;
	/* Don't write root node */
	if (ml->num == 0)
		ml = ml->next;

	while (ml) {
		if (DEBUG)
			printf("Writing Note #%d\n", ml->num);

		/* Get the length +1 (for NULL terminator), write the data. Repeat */
		int len = ml->size;
		fwrite(&len, sizeof(int), 1, fp);

		node_t *tmpMsg = ml->message;
		for (; tmpMsg; tmpMsg = tmpMsg->next)
			fwrite(&tmpMsg->ch, sizeof(char), 1, fp);

		len = strlen(ml->path) + 1;
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(ml->path, sizeof(char), len, fp);

		len = strlen(ml->time) + 1;
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(ml->time, sizeof(char), len, fp);

		ml = ml->next;
	}
}

/* Reads the note data from a file and places in struct */
void readBinary(FILE *fp, msgList *ml) {

	int len, note_num;
	note_num = 0;

	char path[MAX_PATH_SIZE];
	char time[MAX_TIME_SIZE];

	while (fread(&len, sizeof(len), 1, fp)) {
		note_num++;
		if (DEBUG)
			printf("Reading Note #%d\n", note_num);

		/* Allocate memory for new node */
		ml->next = msgList_getNode(ml);

		/* Move to new node */
		ml = ml->next;

		/* We got the first length in the loop condition.... */
		char message[len];
		fread(message, sizeof(char), len, fp);
		message[len] = '\0';

		fread(&len, sizeof(len), 1, fp);
		fread(path, sizeof(char), len, fp);

		fread(&len, sizeof(len), 1, fp);
		fread(time, sizeof(char), len, fp);

		/* Insert strings into struct */
		msgList_insertString(ml, message);
		strcpy(ml->path, path);
		strcpy(ml->time, time);

		ml->num = note_num;

	}
}

/* Deletes all notes freeing all the memory. */
void deleteAll(msgList **ml) {
	msgList *msgListP = *ml;
	msgList *root = msgListP->rootM;
	msgListP = msgListP->rootM;
	if( msgListP->next == NULL ){
		if ( DEBUG )
			printf("Empty list. Nothing to delete\n");
		return;
	}
	msgList *tmpList = NULL;
	node_t *tmpMsg = NULL;
	node_t *currP = NULL;
	msgListP = (msgListP->rootM)->next;
	while ( msgListP ) {
		if( DEBUG )
			printf("Deleting: %d\n", msgListP->num);

		if (msgListP->message) {
			currP = msgListP->message;
			while ( currP ) {
				tmpMsg = currP->next;
				free(currP);
				currP = tmpMsg;
			}
		}
		msgListP->message = NULL;
		tmpList = msgListP->next;
		free(msgListP);
		msgListP = tmpList;
	}
	root->next = NULL;
	*ml = root;
}

/* Attempts to read a saved list from path. If no file is found, attempts to create one.*/
/* Returns true on success or false on failure. */
bool loadList(msgList *ml) {
	if (DEBUG)
		printf("Loading list from: %s\n", path);

	FILE *fp;

	if (file_exists(path) && (fp = fopen(path, "rb")) != NULL ) {
		readBinary(fp, ml);
		fclose(fp);
		return true;
	} else {
		fprintf(stderr,
				"Error loading data file at: %s\nAttempting to create one...\n",
				path);

		fp = fopen(path, "wb");
		if (fp != NULL ) {
			fprintf(stderr, "Successfully created file\n");
			fclose(fp);
			return true;

		} else {
			fprintf(stderr, "Failed to create file\n");
			return false;
		}
	}
}

/* Attempts to save the list at path. */
/* Returns true on success or false on failure. */
bool saveList(msgList *ml) {

	if (DEBUG)
		printf("Saving list at: %s\n", path);

	FILE *fp = NULL;
	if (file_exists(path) && (fp = fopen(path, "wb")) != NULL ) {
		writeBinary(fp, ml);
		fclose(fp);
		return true;

	} else {
		fprintf(stderr,
				"Error loading data file at: %s\nAttempting to create one...\n",
				path);

		fp = fopen(path, "wb");
		if (fp != NULL ) {
			fprintf(stderr, "Successfully created file\n");
			writeBinary(fp, ml);
			fclose(fp);
			return true;

		} else {
			fprintf(stderr, "Failed to create file\n");
			return false;
		}
	}
}



