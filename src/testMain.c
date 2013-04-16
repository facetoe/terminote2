
#include "defines.h"
#include "helperFunctions.h"
#include "linkedList.h"
#include "dynamicArray.h"
#include "nonInteractive.h"
#include "defines.h"
#include "gui.h"


bool needsRefresh = false;
int NCOLS = 0;
int NROWS = 0;
GUIDATA *gd = NULL;

char *choices[]  = {
		"Choice 1", "Choice 2", "Choice 3", "Choice 4", "Choice 5",
		"Choice 6"
};


int main (int argc, char *argv[])
{
	gd = malloc(sizeof(gd));

	initSigaction();
	initNcurses();
	initMenu(choices);
	showWins(gd);
	guiLoop(gd);
	quit();
	//free(gd);

	return 0;
}




