
#include "defines.h"
#include "helperFunctions.h"
#include "linkedList.h"
#include "dynamicArray.h"
#include "nonInteractive.h"


int main(int argc, char **argv) {

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



		Options opts;
		initOptions(&opts);
		parseOptions(&opts, argc, argv);
		executeOptions(&opts, currP);




	return 0;
}
