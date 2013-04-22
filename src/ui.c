
#include "ui.h"
#include "linkedList.h"
#include "dynamicArray.h"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define SIGWINCH 28 // Needed because OSX doesn't recognize SIGWINCH
struct sigaction sa;

struct winsize wSize; // I'm using this instead of getmaxyx() because it didn't seem to always work.

char *mainMenuStrings[] = { "New", "Browse", "Edit", "Search", "Quit", "Help", (char *) NULL, };

/* Variables for ncurses */
static WINDOW *topWin, *midWin, *botWin;
static ITEM **mainMenuItems;
static MENU *footerMenu;

/* Line data pointers */
static LINE *lineData = NULL;
static LINE *lineRoot;

/* Whether the screen needs to be refreshed */
bool needsRefresh = false;

/* Whether we're in a message that can be scrolled (For screen resizes) */
static bool inScrollMessage = false;

/* Terminal coordinates and scrollable message variables */
int NCOLS, NROWS;
static int nlines = 0;
static int cursorPos = 0;

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

/* Parses a listNode and separates each line into a LINE node for displaying in ncurses */
void parseMessage() {
	/* Create an initialize a dynamic array for the line buffer */
	dArr *lineBuffer;
	dArr_init(&lineBuffer);

	/* Set inMessage to true so we can deal with screen resizes */
	inScrollMessage = true;

	/* If lineData isn't NULL then something is wrong and we will have memory leaks.
	 * Abort so we have a backtrace to debug with.*/
	if( lineData )
		abort();

	/* Allocate memory for the lineData struct and set as root node */
	lineData = malloc(sizeof(LINE));
	lineRoot = lineData;

	/* Assign the listNode message to a temporary variable */
	noteNode *currMsg = NULL;
	currMsg = list->message;

	/* If there is no message then something is wrong. Abort */
	if ( !currMsg ) {
		fprintf(stderr, "Error displaying message in showMidWin()");
		abort();
	}

	int totLines = 0;

	for (; currMsg ; currMsg = currMsg->next) {
		if (currMsg->ch != '\n' && currMsg->ch != '\0') {
			/* If the current character isn't a newline or null then add to buffer */
			dArr_add(lineBuffer, currMsg->ch);
		} else {
			/* Keep track of line count  */
			totLines++;

			/* Add a newline and terminator so the line will display correctly in ncurses */
			dArr_add(lineBuffer, '\n');
			dArr_add(lineBuffer, '\0');

			/* Allocate memory for the new LINE node */
			lineData->next = malloc(sizeof(LINE));
			if( !lineData->next )
				abort();

			/* Allocate enough memory to lineData->line to accommodate the string waiting in the buffer */
			lineData->line = malloc( sizeof(char) *  lineBuffer->currSize  );
			if( !lineData->line )
				abort();

			/* Copy the contents of the buffer into the line */
			strcpy(lineData->line, lineBuffer->contents);

			/* Set the size and note number */
			lineData->lSize = lineBuffer->currSize;
			lineData->lNum = totLines;

			/* Move to the next node and initialize values */
			lineData = lineData->next;
			lineData->next = NULL;
			lineData->line = NULL;
			lineData->lSize = 0;
			lineData->lNum = 0;

			/* Clear the buffer for the next iteration */
			dArr_clear(&lineBuffer);
		}
	}
	/* Free the line buffer */
	dArr_destroy(&lineBuffer);
}

/* Free all memory in the LINE struct */
void destroyLineData() {
	LINE *tmp;
	lineData = lineRoot;
	while (lineData) {
		tmp = lineData->next;
		if(lineData->line)
			free(lineData->line);
		free(lineData);
		lineData = tmp;
	}
	free(lineData);
	lineData = NULL;
}

/* Refresh the middle window */
void refreshMidwin() {

	/* If we're in a scroll message, we need to repaint the right part of the message */
	if( inScrollMessage )
	{
		getScrnSize();
		resizeterm(NROWS, NCOLS);
		showTopWin();
		showBotWin();
		midWin = newwin(NROWS - 2, NCOLS, 1, 0);

		for (lineData = lineRoot; lineData->lNum <= NROWS-2; lineData = lineData->next) {
			waddstr(midWin, lineData->line);
		}
		keypad(midWin, true);
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
	noteNode *msg = NULL;
	msg = list->message;

	/* If there is no message then something is seriously wrong. Abort and debug */
	if ( !msg ) {
		abort();
	}

	/* If we in the lists root node then we are in the startup screen.
	 * If there are no notes, then say so, otherwise just print a blank screen  */
	if( list->num == 0 ) {
		char str[300];
		sprintf(str, "You have %d notes stored\n", list->rootM->size);
		mvwprintw(midWin, 0, (NCOLS / 2) - (strlen(str) / 2), str);
		wrefresh(midWin);
		return;
	}

	/* There is no need to parse a message that can already fit on the screen. Just show it */
	if ( list->size < (NROWS -2 ) * NCOLS ) {
		inScrollMessage = false;
		for (; msg ; msg = msg->next) {
			waddch(midWin, msg->ch);
		}
		wrefresh(midWin);
		return;
	}

	/* lineData should be NULL here, if it's not then something is wrong and you will have memory leaks. Abort and debug */
	if(lineData)
		abort();

	/* Parse the message into the lineData struct */
	parseMessage();

	/* Print the message to the screen */
	for (lineData = lineRoot; lineData->lNum <= NROWS-2; lineData = lineData->next) {
		waddstr(midWin, lineData->line);
	}

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

/* Handles screen resizes */
static void hndSIGWINCH(int sig) {
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
}

/* Free all memory and quit */
void quit() {
	int n = ARRAY_SIZE(mainMenuStrings);
	for (int i = 0; i < n; ++i)
		free_item(mainMenuItems[i]);
	free_menu(footerMenu);

	endwin();

	if(lineData)
		destroyLineData();

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
			showWins();
			needsRefresh = true;
			keepGoing = false;
			break;
		}
	}
}

/* run main GUI loop */
void guiLoop() {
	unpost_menu(footerMenu);
	showWins();
	int ch;
	keypad(midWin, true);
	cursorPos = 0;
	while ( ( ch = wgetch(midWin) ) ) {

		sigaction(SIGWINCH, &sa, NULL);

		switch (ch) {

		/* Change to next note in list struct */
		case 'd':
			/* Before we change free all the memory in the current lineData struct */
			if(lineData)
				destroyLineData();
			/* And reset counters */
			cursorPos = 0;
			nlines = 0;
			list_next(&list);
			needsRefresh = true;
			break;

			/* Change to previous note in list struct */
		case 'a':
			if(lineData)
				destroyLineData();
			cursorPos = 0;
			nlines = 0;
			list_previous(&list);
			needsRefresh = true;
			break;

			/* Show the menu along the bottom of the screen */
		case 6:
			doMenu();
			needsRefresh = true;
			break;

			/* Scroll up in the message */
		case KEY_UP:
			/* If there is no lineData then there is nothing to scroll */
			if(!lineData)
				break;

			if( cursorPos <= NROWS) {

				if(nlines <= 0)
					nlines = 0;
				else
					nlines--;

				/* Redraw a blank window */
				midWin = newwin(NROWS - 2, NCOLS, 1, 0);
				keypad(midWin, true);

				for (lineData = lineRoot; lineData->lNum <= nlines; lineData = lineData->next);

				for (; lineData->lNum <= nlines+NROWS-2; lineData = lineData->next) {
					waddstr(midWin, lineData->line);
				}
				wrefresh(midWin);
				break;
			}
			wmove(midWin, --cursorPos, 0);
			wrefresh(midWin);
			keypad(midWin, true);
			break;

			/* Scroll down in the message */
		case KEY_DOWN:
			if(!lineData)
				break;
			if ( cursorPos >= NROWS - 2 ) {
				nlines++;
				midWin = newwin(NROWS - 2, NCOLS, 1, 0);
				keypad(midWin, true);
				for (lineData = lineRoot; lineData->lNum <= nlines; lineData = lineData->next);

				for (; lineData->lNum <= nlines+NROWS-2; lineData = lineData->next) {
					waddstr(midWin, lineData->line);
				}
				wrefresh(midWin);
				break;
			}
			wmove(midWin, ++cursorPos, 0);
			wrefresh(midWin);
			break;

			/* Free all memory and exit */
		case 'q':
			quit();
			break;

		default:
			break;
		}

		if (needsRefresh) {
			if(!lineData)
				showWins();
			else
				refreshMidwin();
			needsRefresh = false;
		}
	}

}
