
#include "defines.h"
#include "helperFunctions.h"
#include "linkedList.h"
#include "dynamicArray.h"
#include "nonInteractive.h"
#include "defines.h"
#include "ui.h"


int main (int argc, char *argv[])
{
	initSigaction();
	init_topWin();
	initNcurses();
	initMenu();
	showWins();
	guiLoop();
	quit();

	return 0;
}





