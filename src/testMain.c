
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

WINDOW *topWin, *midWin, *botWin;
_topWin *win;
ITEM **menuItems;
MENU *footerMenu;

char *choices[]  = {
		"Choice 1", "Choice 2", "Choice 3", "Choice 4", "Choice 5",
		"Choice 6", (char *)NULL,
};


int main (int argc, char *argv[])
{
	initSigaction();
	initNcurses();
	showWins();
	initMenu(choices);
	initWins();
	guiLoop();
	quit(choices);

	return 0;
}





