
#include "ui.h"
#include "linkedList.h"
#include "dynamicArray.h"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define SIGWINCH 28 // Needed because OSX doesn't recognize SIGWINCH
struct sigaction sa;

struct winsize wSize; // I'm using this instead of getmaxyx() because it didn't seem to always work.

char *mainMenuStrings[] = { "New", "Browse", "Edit", "Search", "Quit", "Help", (char *) NULL, };

/* Global variables */
WINDOW *topWin, *midWin, *botWin;
ITEM **mainMenuItems;
MENU *footerMenu;

LINE *lineData = NULL;
LINE *lineRoot;

bool needsRefresh = false;
bool inMessage = false;
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

void parseMessage() {
	dArr *lineBuffer;
	dArr_init(&lineBuffer);
	inMessage = true;

	if( lineData )
		abort();

	lineData = malloc(sizeof(LINE));
	lineRoot = lineData;

	noteNode *currMsg = NULL;
	currMsg = list->message;

	if ( !currMsg ) {
		fprintf(stderr, "Error displaying message in showMidWin()");
		abort();
	}

	int nlines = 0;
	for (; currMsg ; currMsg = currMsg->next) {
		if (currMsg->ch != '\n' && currMsg->ch != '\0') {
			dArr_add(lineBuffer, currMsg->ch);
		} else {
			nlines++;
			dArr_add(lineBuffer, '\n');
			dArr_add(lineBuffer, '\0');
			lineData->next = (LINE*)malloc(sizeof(LINE));
			lineData->line = malloc( sizeof(char) * ( lineBuffer->currSize ) );
			lineData->lSize = lineBuffer->currSize;
			strcpy(lineData->line, lineBuffer->contents);
			lineData->lNum = nlines;
			lineData = lineData->next;
			lineData->next = NULL;
			lineData->line = NULL;
			lineData->lSize = 0;
			lineData->lNum = 0;
			dArr_clear(&lineBuffer);
		}
	}
	dArr_destroy(&lineBuffer);
}

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

void refreshMidwin() {
	if( !inMessage )
	{
		showWins();
		return;
	}

	getScrnSize();
	resizeterm(NROWS, NCOLS);
	if(!lineData)
		parseMessage();

	showTopWin();
	showBotWin();
	midWin = newwin(NROWS - 2, NCOLS, 1, 0);
	for (lineData = lineRoot; lineData->lNum <= NROWS-2; lineData = lineData->next) {
			waddstr(midWin, lineData->line);
		}
	wmove(midWin, 0, 0);
	keypad(midWin, true);
	wrefresh(midWin);

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

	if ( list->size < (NROWS -2 ) * NCOLS ) {
		for (; msg ; msg = msg->next) {
			waddch(midWin, msg->ch);
		}
		wrefresh(midWin);
		return;
	}

	int nlines;
	parseMessage();

	for (lineData = lineRoot; lineData->lNum <= NROWS-2; lineData = lineData->next) {
		waddstr(midWin, lineData->line);
	}
	wrefresh(midWin);
	keypad(midWin, true);
	int ch;
	bool keepGoing = true;
	nlines = 0;
	int cursorPos = 0;
	wmove(midWin, cursorPos, 0);
	while (keepGoing) {
		ch = wgetch(midWin);
		switch (ch) {
		sigaction(SIGWINCH, &sa, NULL);

		case KEY_DOWN:
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

		case KEY_UP:
			if( cursorPos <= NROWS -2 ) {
				if ( nlines <= 0 )
					break;

				nlines--;
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
			break;

		default:
			if(lineData)
				destroyLineData();
			keepGoing = false;
			needsRefresh = true;
			break;
		}
	}
}

/* Setup and print the bottom window to screen */
void showBotWin() {
	botWin = newwin(1, NCOLS, NROWS - 1, 0);
	wattron(botWin, COLOR_PAIR(2));
	wbkgd(botWin, COLOR_PAIR(2));

	/* If we are in the root node then we are at the opening screen so show the startMenu */
	if ( list->num == 0 ) {
		;
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
	while ( ( ch = wgetch(midWin) ) ) {

		sigaction(SIGWINCH, &sa, NULL);

		switch (ch) {

		case 'd':
			if(lineData)
				destroyLineData();
			list_next(&list);
			needsRefresh = true;
			break;

		case 'a':
			if(lineData)
				destroyLineData();
			list_previous(&list);
			needsRefresh = true;
			break;

		case 6:
			//doMenu();
			needsRefresh = true;
			break;

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
