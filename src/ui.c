
#include "ui.h"
#include "linkedList.h"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define SIGWINCH 28 // Needed because OSX doesn't recognize SIGWINCH
struct sigaction sa;

struct winsize wSize; // I'm using this instead of getmaxyx() because it didn't seem to always work.

char *mainMenuStrings[] = { "Edit", "Choice 2", "Choice 3", "Choice 4", "Choice 5",
		"Choice 6", (char *) NULL, };

char *startMenuStrings[] = { "New", "Search", "Help", "Quit", (char *)NULL};

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

/* Allocate memory for data struct */
void init_topWin() {
	win = malloc(sizeof(_topWin));
}

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

/* Setup and print the middle window to screen */
void showBotWin() {
	if(botWin) {
		if ( startMenu )
			unpost_menu(startMenu);
		delwin(botWin);
	}
	botWin = newwin(1, NCOLS, NROWS - 1, 0);
	wattron(botWin, COLOR_PAIR(2));
	wbkgd(botWin, COLOR_PAIR(2));

	/* If we are in the root node then we are at the opening screen, show startMenu */
	if ( list->num == 0 ) {
		doStartMenu();
	}

	wnoutrefresh(botWin);
}

/* Show the windows (and remove botWin if it exists) */
void showWins() {
	getScrnSize(NCOLS, NROWS);
	resizeterm(NROWS, NCOLS);
	showTopWin();
	showBotWin();
	showMidWin();
	wrefresh(curscr);
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

	/* Set main window and sub window */
	set_menu_win(footerMenu, botWin);
	set_menu_sub(footerMenu, derwin(botWin, 1, NCOLS, 1, 1));
	set_menu_format(footerMenu, 0, 6);

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
	nItems = ARRAY_SIZE(mainMenuStrings);
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

	/* Set main window and sub window */
	set_menu_win(startMenu, botWin);
	set_menu_sub(startMenu, derwin(botWin, 1, NCOLS, 1, 1));
	set_menu_format(startMenu, 0, 4);

	/* Get key events from the bottom window */
	keypad(botWin, TRUE);

	/* Post the menu */
	post_menu(startMenu);
	wrefresh(botWin);
}



/* Shows the menu along the bottom of the screen */
void showStartMenu() {
	getScrnSize(NCOLS, NROWS);
	setStartMenu();
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
			strcpy(win->longString, item_name(currItem));
			hideMainMenu();
			showWins();

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
	setStartMenu();
	wnoutrefresh(botWin);
	doupdate();

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
			strcpy(win->longString, item_name(currItem));
			wprintw(midWin, "Harro");
			hideStartMenu();
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

/* Free all memory and quit */
void quit() {
	int s = ARRAY_SIZE(mainMenuStrings);
	for (int i = 0; i < s; ++i)
		free_item(mainMenuItems[i]);
	free_menu(footerMenu);
	for (int i = 0; i < s; ++i) {
		free_item(startMenuItems[i]);
	}
	free_menu(startMenu);
	endwin();
	exit(0);
}

/* Set up the SIGWINCH handler */
void initSigaction() {
	memset(&sa, '\0', sizeof(sa));
	sa.sa_handler = hndSIGWINCH;
}

/* run main GUI loop */
void guiLoop() {

	int ch;
	while ((ch = wgetch(midWin)) != 'q') {

		if (sigaction(SIGWINCH, &sa, NULL) == -1)
			printw("SADSADAS");

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
