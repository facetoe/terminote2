
#include "ui.h"
#include "linkedList.h"
#include "dynamicArray.h"
#include "lineData.h"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define SIGWINCH 28 // Needed because OSX doesn't recognize SIGWINCH
struct sigaction sa;

struct winsize wSize; // I'm using this instead of getmaxyx() because it didn't seem to always work.


/* Variables for ncurses */
char *mainMenuStrings[] = { "New", "Browse", "Edit", "Search", "Quit", "Help", (char *) NULL, };
static WINDOW *topWin, *midWin, *botWin;
static ITEM **mainMenuItems;
static MENU *footerMenu;

/* Line data pointers */
static LINEDATA *lineData = NULL;

/* Whether the screen needs to be refreshed */
bool needsRefresh = false;

/* Whether we're in a message that can be scrolled (For screen resizes) */
bool inScrollMessage;

/* Terminal coordinates and scrollable message variables */
int NCOLS, NROWS;
static int nlines = 0;
static int cursorRow = 0;
static int cursorCol = 0;


/* External variable to hold the list. You should probably move this in here... */
extern listNode *list;

/* Get the size of the terminal screen */
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

	/* If the we are not in the root node then update the top window with time, number and path info*/
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




/* Refresh the middle window */
void refreshMidwin() {

	/* If we're in a scroll message, we need to repaint the right part of the message (You need to fix this so it works properly) */
	if( inScrollMessage )
	{
		getScrnSize();
		resizeterm(NROWS, NCOLS);
		showTopWin();
		showBotWin();
		midWin = newwin(NROWS - 2, NCOLS, 1, 0);

		printRange(lineData, midWin, nlines, (nlines+NROWS)-2);

		keypad(midWin, true);
		wmove(midWin, cursorRow, cursorCol);
		wrefresh(midWin);
	} else {

		/* Otherwise just repaint everything */
		showWins();
		return;
	}



}

/* Setup and print the middle window to screen */
void showMidWin() {
	midWin = newwin(NROWS - 2, NCOLS, 1, 0);

	if( list->num == 0 ) {
		char str[30];
		sprintf(str, "You had %d stored notes.", list->size);
		mvwprintw(midWin, 0, (NCOLS / 2) - (strlen(str) / 2), str);
		wrefresh(midWin);
		return;
	}

	/* Parse the message into the lineData struct */
	lineData_parseMessage(list, &lineData);

	/* Print the message to the screen */
	printRange(lineData, midWin, 0, NROWS -2);

	/* Position the cursor at the start of the message */
	wmove(midWin, 0, 0);

	wrefresh(midWin);
	keypad(midWin, true);

}

/* Setup and print the bottom window to screen */
void showBotWin() {
	botWin = newwin(1, NCOLS, NROWS - 1, 0);
	wattron(botWin, COLOR_PAIR(2));
	wbkgd(botWin, COLOR_PAIR(2));
	wnoutrefresh(botWin);
}

/* Show the windows */
void showWins() {
	getScrnSize(NCOLS, NROWS);
	resizeterm(NROWS, NCOLS);
	showTopWin();
	showBotWin();
	if( inScrollMessage )
		refreshMidwin();
	else
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



/* Show the menu along the bottom of the screen */
void showMainMenu() {
	getScrnSize(NCOLS, NROWS);
	showBotWin();
	setMainMenu();
}

/* Hide the menu at the bottom of the screen */
void hideMainMenu() {
	unpost_menu(footerMenu);
	wrefresh(botWin);
}

/* Handles screen resizes */
void hndSIGWINCH(int sig) {
	if( sig == SIGWINCH )
		refreshMidwin();
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
	lineData_init(&lineData);
}

/* Free all memory and quit */
void quit() {
	int n = ARRAY_SIZE(mainMenuStrings);
	for (int i = 0; i < n; ++i)
		free_item(mainMenuItems[i]);
	free_menu(footerMenu);

	endwin();

	lineData_destroy(&lineData);

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
				keepGoing = false;
				break;
			} else if (!strcmp(item_name(currItem), "Help")) {
				keepGoing = false;
				break;
			}

			keepGoing = false;
			break;

		default:
			/* Hide the menu and break out of the loop */
			hideMainMenu();
			return;
			break;
		}
	}
}

/* run main GUI loop */
void guiLoop() {
	unpost_menu(footerMenu);
	cursorCol = 0;
	cursorRow = 0;
	showWins();
	char str[30];
	int ch;
	keypad(midWin, true);
	while ( ( ch = wgetch(midWin) ) ) {

		sigaction(SIGWINCH, &sa, NULL);

		switch (ch) {

		/* Change to next note in list struct */
		case 'd':
			cursorRow = 0;
			cursorCol = 0;
			nlines = 0;
			list_next(&list);
			inScrollMessage = false;
			needsRefresh = true;
			break;

			/* Change to previous note in list struct */
		case 'a':
			cursorRow = 0;
			cursorCol = 0;
			nlines = 0;
			inScrollMessage = false;
			list_previous(&list);
			needsRefresh = true;
			break;

			/* Show the menu along the bottom of the screen */
		case 6:
			doMenu();
			break;

			/* Scroll up in the message */
		case KEY_UP:

			if( cursorRow >= 0 ) {
				wmove(midWin, --cursorRow, cursorCol);
				wmove(midWin, cursorRow, cursorCol);
				wrefresh(midWin);
				break;
			} else if( cursorRow <= NROWS -2) {

				if (nlines <= 0)
					nlines = 0;
				else
					nlines -= NROWS -2;

				cursorRow = 0;

				/* Redraw a blank window */
				midWin = newwin(NROWS - 2, NCOLS, 1, 0);
				keypad(midWin, true);
				printRange(lineData, midWin, nlines, (nlines+NROWS)-2);
				wmove(midWin, cursorRow, cursorCol);

				printRange(lineData, midWin, nlines, nlines+NROWS-2);
				wrefresh(midWin);
				break;
			}
			wmove(midWin, --cursorRow, 0);
			wrefresh(midWin);
			keypad(midWin, true);
			break;

			/* Scroll down in the message */
		case KEY_DOWN:

			if( lineData->numLines <= NROWS -2 )
				break;

			if( nlines + (NROWS*2)-4 >= lineData->numLines ) {
				midWin = newwin(NROWS - 2, NCOLS, 1, 0);
				keypad(midWin, true);
				printAll(lineData, nlines + NROWS-2,  midWin);
				wmove(midWin, cursorRow, cursorCol);
				wrefresh(midWin);
				break;
			}

			if ( cursorRow >= NROWS - 2 ) {
				nlines += NROWS -2;

				midWin = newwin(NROWS - 2, NCOLS, 1, 0);
				keypad(midWin, true);
				printRange(lineData, midWin, nlines, (nlines+NROWS)-2);
				cursorRow = NROWS-2;
				wmove(midWin, cursorRow, cursorCol);
				wrefresh(midWin);
				break;
			}
			wmove(midWin, ++cursorRow, cursorCol);
			wrefresh(midWin);
			keypad(midWin, true);
			break;

		case KEY_LEFT:
			if( cursorCol <= 0 ) {
				cursorRow--;
				cursorCol = NCOLS;
			} else {
				cursorCol--;
			}
			wmove(midWin, cursorRow, cursorCol);
			wrefresh(midWin);
			keypad(midWin, true);
			break;

		case KEY_RIGHT:
			if( cursorCol >= NCOLS ) {
				cursorRow++;
				cursorCol = 0;
			} else {
				cursorCol++;
			}
			wmove(midWin, cursorRow, cursorCol);
			wrefresh(midWin);
			keypad(midWin, true);
			break;

			/* Free all memory and exit */
		case 'q':
			quit();
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
