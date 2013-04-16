
#include "defines.h"
#include "helperFunctions.h"
#include "linkedList.h"
#include "dynamicArray.h"
#include "nonInteractive.h"
#include "defines.h"
#include "gui.h"


volatile bool needsRefresh = false;
volatile int NCOLS = 0;
volatile int NROWS = 0;
volatile GUIDATA *gd;

char *choices[]  = {
		"Choice 1", "Choice 2", "Choice 3", "Choice 4", "Choice 5",
		"Choice 6",(char *)NULL,
};


int main (int argc, char *argv[])
{

	gd = NULL;
	gd = malloc(sizeof(gd));

	initSigaction();
	initNcurses();
	initMenu(choices);
	showWins(gd);
	guiLoop(gd);
	quit();
	free(gd);

	return 0;
}

