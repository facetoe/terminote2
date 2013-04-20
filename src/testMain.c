
#include "defines.h"
#include "helperFunctions.h"
#include "linkedList.h"
#include "dynamicArray.h"
#include "nonInteractive.h"
#include "ui.h"


listNode *list = NULL;



int main(int argc, char **argv) {

	Options options;

	if (getDataPath(pathBuffer, MAX_PATH_SIZE, DATA_FILE))
		path = pathBuffer;
	else {
		/* If we can't load the save file then the program is crippled, just exit */
		fprintf(stderr, "Failed to load data.\n");
		exit(1);
	}

	if (isatty(STDIN_FILENO) && argc == 1) {
		list_init(&list);
		list_load(list);
		initSigaction();
		initNcurses();
		initMainMenu();
		initStartMenu();
		guiLoop(list);
		quit();
	} else {
		nonInteractive_run(&options, argc, argv);
	}
	return 0;
}
