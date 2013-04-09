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
void list_init(listNode **root) {
	if (DEBUG)
		printf("Initializing list\n");

	noteNode *tmpNode = malloc(sizeof(noteNode));
	listNode *tmpMsg = malloc(sizeof(listNode));

	if (!tmpNode || !tmpMsg) {
		fprintf(stderr, "Unable to allocate memory\n");
		exit(1);
	}
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

	*root = tmpMsg;
}

/* Allocates memory for a new node, sets values and returns initialized node */
listNode *list_getNode(listNode *ln) {
	listNode *newMsg = NULL;
	newMsg = malloc(sizeof(listNode));
	if ( !newMsg ) {
		fprintf(stderr, "Failed to allocate memory in msgList_getNode()\n");
		exit(1);
	}

	newMsg->num = ln->num;
	newMsg->rootN = NULL;
	newMsg->rootM = ln->rootM;
	newMsg->message = NULL;
	newMsg->next = NULL;
	strncpy(newMsg->time, "\0", MAX_TIME_SIZE);
	strncpy(newMsg->path, "\0", MAX_PATH_SIZE);

	return newMsg;
}

/* Returns the length of the list */
int list_length(listNode *ln) {
	int listSize = 0;
	ln = ln->rootM;
	if ( ln->next ) {
		ln = ln->next; // Don't count root node.
		for (; ln ; ln = ln->next, listSize++);
	}
	return listSize;
}

/* Returns the message length */
int list_messageLength(noteNode *msg) {
	int len = 0;
	for (; msg; msg = msg->next, len++);
	return len;
}


/* Inserts a string into a msgList node */
void list_insertString(listNode *ln, char *str) {
	noteNode *prevNode = NULL;
	int cnt = 0;
	int num = ln->num;

	ln->message = malloc(sizeof(noteNode));
	ln->num = ++num;

	if (!ln->message) {
		fprintf(stderr,
				"Failed to allocate memory in msgList_insertString()\n");
		exit(1);
	}

	(ln->message)->ch = *str;
	(ln->message)->index = cnt;
	(ln->message)->prev = NULL;
	(ln->message)->next = NULL;
	ln->rootN = ln->message;
	ln->next = NULL;

	prevNode = ln->message;

	for (char *s = str + 1; *s; s++) {
		(ln->message)->next = malloc(sizeof(noteNode));
		if (!(ln->message)->next) {
			fprintf(stderr,
					"Failed to allocate memory in msgList_insertString()\n");
			exit(1);
		}
		ln->message = (ln->message)->next;
		(ln->message)->prev = prevNode;
		(ln->message)->next = NULL;
		(ln->message)->index = ++cnt;
		(ln->message)->ch = *s;
		prevNode = ln->message;
	}
	ln->message = ln->rootN;
	ln->size = list_messageLength(ln->message);
	if (DEBUG)
		printf("Added: %s\n", str);
}

/* Appends message to the end of the list */
void list_appendMessage(listNode *ln, char *str) {

	for (; ln->next; ln = ln->next)
		;

	/* Allocate memory for next node */
	ln->next = list_getNode(ln);

	/* Move to node */
	ln = ln->next;

	/* Insert string */
	list_insertString(ln, str);

	/* Get and store path information */
	char *path = getcwd(NULL, 0);
	if (path == NULL ) {
		fprintf(stderr, "Unable to retrieve path\n");
	} else {
		size_t path_len = strlen(path);
		strncpy(ln->path, path, path_len);
		free(path);
	}

	/* Get and store time information */
	char *time = current_time();
	if (time == NULL ) {
		perror("Unable to retrieve time\n");
	} else {

		strip_newline(time);
		size_t time_len = strlen(time);
		strncpy(ln->time, time, time_len);
	}
}

/* Prints current note according to args. Args are:
 * n: Note number
 * p: Path
 * t: Time
 * m: Message */
void list_printMessage(FILE *outStream, char *args, listNode *ln) {
	if (ln == NULL || ln->num == 0)
		fprintf(outStream, "Nothing to print.\n");
	else {
		for (char *s = args; *s ; s++) {
			switch (*s) {
			case 'n':
				fprintf(outStream, "Note Number: %d\n", ln->num);
				break;
			case 'p':
				fprintf(outStream, "Path: %s\n", ln->path);
				break;
			case 't':
				fprintf(outStream, "Time: %s\n", ln->time);
				break;
			case 'm':
				fprintf(outStream, "Message:\n");
				noteNode *message = ln->message;
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
void list_printAll(FILE *outStream, listNode *ln) {
	if( DEBUG )
		if ( !ln )
			fprintf(stderr, "Null pointer passed to printAll\n");

	ln = ln->rootM;
	if (!ln->next) {
		printf("Nothing to print.\n");
	} else {
		ln = ln->next;
		for (; ln; ln = ln->next) {
			list_printMessage(outStream, "nptm", ln);
		}
	}
}

/* Destroys the list freeing all memory */
void list_destroy(listNode *ln) {
	if( DEBUG )
		if ( !ln )
			fprintf(stderr, "Null pointer passed to msgList_destroy\n");

	listNode *tmpList = NULL;
	noteNode *tmpMsg = NULL;
	noteNode *currP = NULL;
	ln = ln->rootM;
	while ( ln ) {
		if( DEBUG )
			printf("Destroying: %d\n", ln->num);

		if (ln->message) {
			currP = ln->message;
			while ( currP ) {
				tmpMsg = currP->next;
				free(currP);
				currP = tmpMsg;
			}
		}
		ln->message = NULL;
		tmpList = ln->next;
		free(ln);
		ln = tmpList;
	}
}

/* Moves list pointer to last node in the list.
 * If it is already the last node then leaves the pointer unchanged. */
void list_lastNode(listNode **ln) {
	listNode *tmp = *ln;
	if (tmp->next == NULL )
		return;
	for (; tmp->next; tmp = tmp->next)
		;
	*ln = tmp;
}

/* Moves list pointer to the first node in the list.
 * If the first node is the root node then leaves the pointer unchanged. */
void list_firstNode(listNode **ln) {
	listNode *tmp = *ln;
	tmp = tmp->rootM;
	if (tmp->next)
		tmp = tmp->next;
	*ln = tmp;
}

/* Moves the list pointer to the next node in the list. If it is the last node,
 * moves the pointer to the first node: ie, head->next to skip root node.
 * If there is only one node in the list (not counting root) it returns currP unchanged. */
void list_next(listNode **ln) {
	listNode *tmp = *ln;
	if (tmp->next != NULL ) {
		tmp = tmp->next;
	} else if ((tmp->rootM)->next){
		tmp = (tmp->rootM)->next;
	}
	*ln = tmp;
}

/* Moves the list pointer to the previous node in the list.
 * If there is only one node (not counting root) leaves the pointer unchanged.  */
void list_previous(listNode **ln) {
	listNode *tmp = *ln;
	int noteNum = tmp->num;
	if (noteNum == 1) {
		/* We are at the start of the list, so grab the last node */
		list_lastNode(&tmp);
	} else {
		/* We are not at the start, so grab the first node */
		list_firstNode(&tmp);

		/* And loop through to noteNum -1, ie the previous node */
		for (;tmp->next != NULL && tmp->num != noteNum - 1; tmp = tmp->next);
	}
	*ln = tmp;
}

/* Searches for node with noteNum.
 * Returns node if found, otherwise returns NULL. */
listNode *list_searchByNoteNum(listNode *ln, int noteNum) {
	/* Reset to first node */
	ln = ln->rootM;

	/* Nothing to search if the list is empty */
	if (list_length(ln) == 0)
		return NULL ;

	/* don't check root node */
	ln = ln->next;

	for (;ln->num != noteNum && ln->next; ln = ln->next );

	if (ln->num == noteNum)
		return ln;
	else
		return NULL ;
}

/* Reorders the noteNums */
void list_orderList(listNode *ln) {
	int nNum = 1;
	/* Don't count root node */
	ln = ln->rootM;

	if( ln->next ) {
		ln = ln->next;
		for (; ln; ln = ln->next, nNum++) {
			ln->num = nNum;
		}
	}
	if (DEBUG)
		printf("Ordered list\n");
}

/* Delete a node by noteNum */
void list_deleteNode(listNode *ln, int noteNum) {
	ln = ln->rootM;
	/* Don't delete root node */
	if (noteNum == 0)
		return;

	listNode *tmpList;
	noteNode *tmpMsg, *currP;

	while (ln) {
		/* Go to the node before the one to be deleted */
		if (ln->num == noteNum - 1) {

			/* tmpList points to node to be deleted */
			tmpList = ln->next;

			/* currP points to the message to be deleted */
			currP = (ln->next)->message;
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
			ln->next = tmpList->next;

			/* Free tmpList */
			if (tmpList != NULL )
				free(tmpList);

			/* Reorder the list */
			list_orderList(ln);
			return;
		} else {
			tmpList = ln;
			ln = ln->next;
		}
	}
}

/* Writes the note structs to a file */
void list_writeBinary(FILE *fp, listNode *ln) {
	ln = ln->rootM;
	/* Don't write root node */
	if (ln->num == 0)
		ln = ln->next;

	while (ln) {
		if (DEBUG)
			printf("Writing Note #%d\n", ln->num);

		/* Get the length +1 (for NULL terminator), write the data. Repeat */
		int len = ln->size;
		fwrite(&len, sizeof(int), 1, fp);

		noteNode *tmpMsg = ln->message;
		for (; tmpMsg; tmpMsg = tmpMsg->next)
			fwrite(&tmpMsg->ch, sizeof(char), 1, fp);

		len = strlen(ln->path) + 1;
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(ln->path, sizeof(char), len, fp);

		len = strlen(ln->time) + 1;
		fwrite(&len, sizeof(int), 1, fp);
		fwrite(ln->time, sizeof(char), len, fp);

		ln = ln->next;
	}
}

/* Reads the note data from a file and places in struct */
void list_readBinary(FILE *fp, listNode *ln) {

	int len, note_num;
	note_num = 0;

	char path[MAX_PATH_SIZE];
	char time[MAX_TIME_SIZE];

	while (fread(&len, sizeof(len), 1, fp)) {
		note_num++;
		if (DEBUG)
			printf("Reading Note #%d\n", note_num);

		/* Allocate memory for new node */
		ln->next = list_getNode(ln);

		/* Move to new node */
		ln = ln->next;

		/* We got the first length in the loop condition.... */
		char message[len];
		fread(message, sizeof(char), len, fp);
		message[len] = '\0';

		fread(&len, sizeof(len), 1, fp);
		fread(path, sizeof(char), len, fp);

		fread(&len, sizeof(len), 1, fp);
		fread(time, sizeof(char), len, fp);

		/* Insert strings into struct */
		list_insertString(ln, message);
		strcpy(ln->path, path);
		strcpy(ln->time, time);

		ln->num = note_num;

	}
}

/* Deletes all notes freeing all the memory. */
void list_deleteAll(listNode **ln) {
	listNode *msgListP = *ln;
	listNode *root = msgListP->rootM;
	msgListP = msgListP->rootM;
	if( msgListP->next == NULL ){
		if ( DEBUG )
			printf("Empty list. Nothing to delete\n");
		return;
	}
	listNode *tmpList = NULL;
	noteNode *tmpMsg = NULL;
	noteNode *currP = NULL;
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
	*ln = root;
}

/* Attempts to read a saved list from path. If no file is found, attempts to create one.*/
/* Returns true on success or false on failure. */
bool list_load(listNode *ln) {
	if (DEBUG)
		printf("Loading list from: %s\n", path);

	FILE *fp;

	if (file_exists(path) && (fp = fopen(path, "rb")) != NULL ) {
		list_readBinary(fp, ln);
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
bool list_save(listNode *ln) {

	if (DEBUG)
		printf("Saving list at: %s\n", path);

	FILE *fp = NULL;
	if (file_exists(path) && (fp = fopen(path, "wb")) != NULL ) {
		list_writeBinary(fp, ln);
		fclose(fp);
		return true;

	} else {
		fprintf(stderr,
				"Error loading data file at: %s\nAttempting to create one...\n",
				path);

		fp = fopen(path, "wb");
		if (fp != NULL ) {
			fprintf(stderr, "Successfully created file\n");
			list_writeBinary(fp, ln);
			fclose(fp);
			return true;

		} else {
			fprintf(stderr, "Failed to create file\n");
			return false;
		}
	}
}

/* Searches the listNode's message for substring. Returns true if it does,
 * false if not. */
bool list_messageHasSubstring(listNode *ln, char *subStr) {
    char str[ln->size];
    int cnt = 0;
    noteNode *tmpNode = ln->message;
    for (; tmpNode ; tmpNode = tmpNode->next, cnt++) {
        str[cnt] = tmpNode->ch;
    }
    str[cnt] = '\0';
    return strstr(str, subStr) != NULL;
}




