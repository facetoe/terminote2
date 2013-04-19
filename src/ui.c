
#include "ui.h"
#include "linkedList.h"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define SIGWINCH 28 // Needed because OSX doesn't recognize SIGWINCH
struct sigaction sa;

struct winsize wSize; // I'm using this instead of getmaxyx() because it didn't seem to always work.

char *mainMenuStrings[] = { "New", "Browse", "Edit", "Search", "Quit", "Help", (char *) NULL, };
char *startMenuStrings[] = { "New", "Browse", "Search", "Quit", "Help", (char *)NULL, };

/* Global variables */
WINDOW *topWin, *midWin, *botWin;
_topWin *win;
ITEM **mainMenuItems;
ITEM **startMenuItems;
MENU *startMenu;
MENU *footerMenu;
bool needsRefresh = false;
int NCOLS, NROWS;

extern listNode *list;

void getScrnSize() {
	ioctl(0, TIOCGWINSZ, &wSize);
	NCOLS = wSize.ws_col;
	NROWS = wSize.ws_row;
}

/* Setup and print the top window to screen */
void showTopWin() {
	/* Create the window*/
	topWin = newwin(1, NCOLS, 0, 0);

	/* Turn colors on */
	wattron(topWin, COLOR_PAIR(1));
	wbkgd(topWin, COLOR_PAIR(1));

	/* If the we are not in the root node then update the top window */
	if ( list->num > 0 ) {
		char noteStr[100];
		snprintf(noteStr, 100, "Note #%d", list->num);
		mvwprintw(topWin, 0, (NCOLS / 2) - (strlen(noteStr) / 2), noteStr);
		mvwprintw(topWin, 0, 0, list->path);
		mvwprintw(topWin, 0, (NCOLS - strlen(list->time)) - 2, list->time);
	} else {
		/* Otherwise just print the title and version */
		char title[50];
		sprintf(title, "Terminote %.1f", VERSION_NUM);
		mvwprintw(topWin, 0, (NCOLS / 2) - (strlen(title) / 2), title);
	}
	wnoutrefresh(topWin);
}

void showHelpScreen() {
	/* Create the windows*/
	topWin = newwin(1, NCOLS, 0, 0);
	midWin = newwin(NROWS - 2, NCOLS, 1, 0);
	botWin = newwin(1, NCOLS, NROWS - 1, 0);

	/* Turn colors on */
	wattron(topWin, COLOR_PAIR(1));
	wbkgd(topWin, COLOR_PAIR(1));
	char title[50];
	sprintf(title, "Terminote %.1f Help", VERSION_NUM);
	mvwprintw(topWin, 0, (NCOLS / 2) - (strlen(title) / 2), title);

	char *helpHeading = "Terminote Interactive Help:";

	/* Print the help message */
	mvwprintw(midWin, 0, (NCOLS / 2) - (strlen(helpHeading) / 2), helpHeading);
	waddstr(midWin, "\n\nTerminote interactive allows viewing, editing and creating notes in an interative manner.");

	/* Hide whichever menu got us here */
	unpost_menu(startMenu);
	unpost_menu(footerMenu);

	/* Refresh all the windows */
	wnoutrefresh(botWin);
	wnoutrefresh(topWin);
	wnoutrefresh(midWin);
	doupdate();
	/* Wait for input on help screen so it doesn't instantly vanish */
	wgetch(botWin);
}

/* Setup and print the middle window to screen */
void showMidWin() {
	midWin = newwin(NROWS - 2, NCOLS, 1, 0);
	noteNode *msg = NULL;
	msg = list->message;
	if ( !msg ) {
		fprintf(stderr, "Error displaying message in showMidWin()");
		abort();
	}

	/* If we are not in the root node then print the message in the middle window */
	if ( list->num > 0 )
		for (int i = 0; msg ; msg = msg->next, ++i)
			waddch(midWin, msg->ch);

	wnoutrefresh(midWin);
}

/* Setup and print the bottom window to screen */
void showBotWin() {
	if(botWin) {
		if ( startMenu )
			unpost_menu(startMenu);
		delwin(botWin);
	}
	botWin = newwin(1, NCOLS, NROWS - 1, 0);
	wattron(botWin, COLOR_PAIR(2));
	wbkgd(botWin, COLOR_PAIR(2));

	/* If we are in the root node then we are at the opening screen so show the startMenu */
	if ( list->num == 0 ) {
		doStartMenu();
	}
	wnoutrefresh(botWin);
}

/* Show the windows */
void showWins() {
	getScrnSize(NCOLS, NROWS);
	resizeterm(NROWS, NCOLS);
	showTopWin();
	showBotWin();
	showMidWin();
	doupdate();
}

/* Initialize the menu but don't show it */
void initMainMenu() {
	/* Create items */
	int nItems;
	nItems = ARRAY_SIZE(mainMenuStrings);
	mainMenuItems = (ITEM **) calloc(nItems, sizeof(ITEM *));
	for (int i = 0; i < nItems; ++i)
		mainMenuItems[i] = new_item(mainMenuStrings[i], mainMenuStrings[i]);

	/* Create menu */
	footerMenu = new_menu((ITEM **) mainMenuItems);
}

/* Setup and show the menu */
void setMainMenu() {
	/* Set menu option not to show the description */
	menu_opts_off(footerMenu, O_SHOWDESC);

	int nItems = ARRAY_SIZE(mainMenuStrings);

	/* Set main window and sub window */
	set_menu_win(footerMenu, botWin);
	set_menu_sub(footerMenu, derwin(botWin, 1, NCOLS, 1, 1));
	set_menu_format(footerMenu, 0, nItems);

	/* Get key events from the bottom window */
	keypad(botWin, TRUE);

	/* Post the menu */
	post_menu(footerMenu);
	wrefresh(botWin);
}



/* Shows the menu along the bottom of the screen */
void showMainMenu() {
	getScrnSize(NCOLS, NROWS);
	showBotWin();
	setMainMenu();
}

/* Hides the menu at the bottom of the screen */
void hideMainMenu() {
	unpost_menu(footerMenu);
	wrefresh(botWin);
}

/* Initialize the menu but don't show it */
void initStartMenu() {
	/* Create items */
	int nItems;
	nItems = ARRAY_SIZE(startMenuStrings);
	startMenuItems = (ITEM **) calloc(nItems, sizeof(ITEM *));
	for (int i = 0; i < nItems; ++i)
		startMenuItems[i] = new_item(startMenuStrings[i], startMenuStrings[i]);

	/* Create menu */
	startMenu = new_menu((ITEM **) startMenuItems);
}

/* Setup and show the menu */
void setStartMenu() {
	/* Set menu option not to show the description */
	menu_opts_off(startMenu, O_SHOWDESC);

	int nItems = ARRAY_SIZE(startMenuStrings);

	/* Set main window and sub window */
	set_menu_win(startMenu, botWin);
	set_menu_sub(startMenu, derwin(botWin, 1, NCOLS, 1, 1));
	set_menu_format(startMenu, 0, nItems);

	/* Get key events from the bottom window */
	keypad(botWin, TRUE);

	/* Post the menu */
	post_menu(startMenu);
	wnoutrefresh(botWin);
}

/* Shows the menu along the bottom of the screen */
void showStartMenu() {
	getScrnSize(NCOLS, NROWS);
	setStartMenu();
	doupdate();
}

/* Hides the menu at the bottom of the screen */
void hideStartMenu() {
	unpost_menu(startMenu);
	wrefresh(botWin);
}

/* Handles screen resizes */
static void hndSIGWINCH(int sig) {
	showWins();
}

/* Setup ncurses */
void initNcurses() {
	initscr();
	raw();
	nonl();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
}

/* Free all memory and quit */
void quit() {
	int n = ARRAY_SIZE(mainMenuStrings);
	for (int i = 0; i < n; ++i)
		free_item(mainMenuItems[i]);
	free_menu(footerMenu);

	n = ARRAY_SIZE(startMenuStrings);
	for (int i = 0; i < n; ++i)
		free_item(startMenuItems[i]);

	free_menu(startMenu);
	endwin();

	/* Save and destroy the list */
	list_save(list);
	list_destroy(list);
	exit(0);
}

/* Set up the SIGWINCH handler */
void initSigaction() {
	memset(&sa, '\0', sizeof(sa));
	sa.sa_handler = hndSIGWINCH;
}

/* Select and execute options from the menu */
void doMenu() {
	/* Show the menu along the bottom of the screen */
	showBotWin();
	showMainMenu();
	wnoutrefresh(botWin);
	doupdate();

	int ch;
	ITEM *currItem;
	bool keepGoing = true;
	while (keepGoing) {
		ch = wgetch(botWin);
		switch (ch) {
		case KEY_LEFT:
			menu_driver(footerMenu, REQ_PREV_ITEM);
			break;

		case KEY_RIGHT:
			menu_driver(footerMenu, REQ_NEXT_ITEM);
			break;

		case 13: /* Enter */
			currItem = current_item(footerMenu);
			if ( !strcmp(item_name(currItem), "Quit") ) {
				quit();
			} else if ( !strcmp(item_name(currItem), "Browse") ) {
				hideMainMenu();
				list_firstNode(&list);
				showWins();
				keepGoing = false;
				break;
			} else if (!strcmp(item_name(currItem), "Help")) {
				showHelpScreen();
				keepGoing = false;
				break;
			}

			keepGoing = false;
			break;

		default:
			/* Hide the menu and break out of the loop */
			hideMainMenu();
			showWins();
			needsRefresh = true;
			keepGoing = false;
			break;
		}
	}
}

/* Select and execute options from the menu */
void doStartMenu() {
	showStartMenu();

	int ch;
	ITEM *currItem;
	bool keepGoing = true;
	while (keepGoing) {
		ch = wgetch(botWin);
		switch (ch) {
		case KEY_LEFT:
			menu_driver(startMenu, REQ_PREV_ITEM);
			break;

		case KEY_RIGHT:
			menu_driver(startMenu, REQ_NEXT_ITEM);
			break;

		case 13: /* Enter */
			currItem = current_item(startMenu);
			if ( !strcmp(item_name(currItem), "Quit") ) {
				quit();
			} else if( !strcmp(item_name(currItem), "Help") ) {
				showHelpScreen();
			} else if( !strcmp(item_name(currItem), "Browse") ) {
				list_next(&list);
				keepGoing = false;
			}
			hideStartMenu();
			showWins();
			needsRefresh = true;
			keepGoing = false;
			break;

		default:
			break;
		}
	}
}

/* run main GUI loop */
void guiLoop() {
	showWins();
	int ch;
	while ((ch = wgetch(midWin)) != 'q') {

		if (sigaction(SIGWINCH, &sa, NULL) == -1) {
			wprintw(topWin, "I wonder what this means");
			wrefresh(topWin);
		}

		switch (ch) {

		case 'd':
			list_next(&list);
			needsRefresh = true;
			break;

		case 'a':
			list_previous(&list);
			needsRefresh = true;
			break;

		case 6:
			doMenu();
			needsRefresh = true;
			break;

		default:
			break;
		}

		if (needsRefresh) {
			showWins();
			needsRefresh = false;
		}
	}

}
