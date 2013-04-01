
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

		list_appendMessage(currP, "Hello World!");
		list_printAll(stdout, currP);

		dArr *arr = NULL;
		dArr_init(&arr);
		for (char *s = path; *s; s++) {
			dArr_add(arr, *s);
		}

		dArr_print(arr);

	return 0;
}
