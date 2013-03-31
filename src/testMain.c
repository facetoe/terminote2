
#include "defines.h"
#include "helperFunctions.h"
#include "linkedList.h"


int main(void) {

	msgList *currP = NULL;

		msgList_init(&currP);

		if (currP)
			printf("Initialized list\n");

		if (getDataPath(pathBuffer, MAX_PATH_SIZE, DATA_FILE))
			path = pathBuffer;
		else {
			/* If we can't load the save file then the program is crippled, just exit */
			fprintf(stderr, "Failed to load data.\n");
			exit(1);
		}

		loadList(currP);
		printAll(stdout, currP);
		deleteAll(&currP);
		saveList(currP);
		msgList_destroy(currP);

	return 0;
}
