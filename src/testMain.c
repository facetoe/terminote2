
#include "defines.h"
#include "helperFunctions.h"
#include "linkedList.h"
#include "dynamicArray.h"


int main(void) {

	listNode *currP = NULL;

	list_init(&currP);

		if (currP)
			printf("Initialized list\n");

		if (getDataPath(pathBuffer, MAX_PATH_SIZE, DATA_FILE))
			path = pathBuffer;
		else {
			/* If we can't load the save file then the program is crippled, just exit */
			fprintf(stderr, "Failed to load data.\n");
			exit(1);
		}

		list_appendMessage(currP, "Hello");
		list_appendMessage(currP, "So many words in hear...");
		list_appendMessage(currP, "Hello there yayayayaa");
		list_appendMessage(currP, "Hello");

		for (; currP ; currP=currP->next) {
			if( list_messageHasSubstring(currP, "in") )
				list_printMessage(stdout, "m", currP);
		}

	return 0;
}
