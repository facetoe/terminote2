
#include "defines.h"
#include "helperFunctions.h"
#include "linkedList.h"
#include "dynamicArray.h"
#include "nonInteractive.h"
#include "ui.h"


listNode *list = NULL;
int main(int argc, char **argv) {

	if (getDataPath(pathBuffer, MAX_PATH_SIZE, DATA_FILE))
		path = pathBuffer;
	else {
		/* If we can't load the save file then the program is crippled, just exit */
		fprintf(stderr, "Failed to load data.\n");
		exit(1);
	}

	list_init(&list);
	if( !list ) {
		fprintf(stderr, "Failed to create list\n");
		abort();
	}

	list_load(list);

	initSigaction();
	init_topWin();
	initNcurses();
	initMainMenu();
	initStartMenu();
	showWins();
	guiLoop(list);
	quit();

	/* Clean up */
	list_save(list);
	list_destroy(list);



	return 0;
}
