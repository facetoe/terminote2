#define _POSIX_SOURCE // You need this here to get rid of the warning with sigaction
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <menu.h>

#include <signal.h>
#include <sys/ioctl.h>
#include "gui.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))


#define SIGWINCH 28 // Needed because OSX doesn't recognize SIGWINCH
struct sigaction sa;

struct winsize wSize; // I'm using this instead of getmaxyx() because it didn't seem to always work.

int NCOLS;
int NROWS;

extern WINDOW *topWin, *midWin, *botWin;
extern ITEM **menuItems;
extern MENU *footerMenu;

/* Allocate memory for data struct */
void init_topWin(_topWin **win) {
	_topWin *p = NULL;
	p = malloc(sizeof(_topWin));
	*win = p;
}

void getScrnSize() {
	ioctl(0, TIOCGWINSZ, &wSize);
	NCOLS = wSize.ws_col;
	NROWS = wSize.ws_row;
}

/* Setup and print the top window to screen */
void showTopWin(){
	/* Create the window*/
	if ( !topWin )
		topWin = newwin(1, NCOLS, 0, 0);

	/* Turn colors on */
	wattron( topWin,COLOR_PAIR(1));
	wbkgd( topWin,COLOR_PAIR(1));

	char noteStr[100];
	sprintf(noteStr, "Note #%d", NCOLS);

	// win stuff was here
	mvwprintw(topWin, 0, 0, "Harro");

	wnoutrefresh( topWin);
}

/* Setup and print the middle window to screen */
void showMidWin() {
	if (! midWin)
		 midWin = newwin(NROWS - 2, NCOLS, 1, 0);
	wnoutrefresh( midWin);
}

/* Setup and print the middle window to screen */
void showBotWin() {
	if (!botWin)
		 botWin = newwin(1, NCOLS, NROWS- 1, 0);
	wattron( botWin,COLOR_PAIR(2));
	wbkgd( botWin,COLOR_PAIR(2));

	wnoutrefresh( botWin);
	keypad( botWin, true);
}


/* Show the windows (and remove botWin if it exists) */
void showWins() {
	getScrnSize();
	resizeterm(NROWS, NCOLS);
	showTopWin();
	showMidWin();
	doupdate();
}

void initWins() {
	getScrnSize();
	resizeterm(NROWS, NCOLS);
	showTopWin();
	showMidWin();
	showBotWin();
	setMenu();
	hideMenu();
	doupdate();
}

/* Initialize the menu but don't show it */
void initMenu(char *choices[]) {
	menuItems = (ITEM **)calloc(NUM_CHOICES, sizeof(ITEM *));
	for(int i = 0; i < NUM_CHOICES ; ++i) {
		menuItems[i] = new_item(choices[i], choices[i]);
	}

	/* Create menu */
	 footerMenu = new_menu((ITEM **)menuItems);
}


/* Setup and show the menu */
void setMenu() {
	/* Set menu option not to show the description */
	menu_opts_off( footerMenu, O_SHOWDESC);

	/* Set main window and sub window */
	set_menu_win( footerMenu,  botWin);
	set_menu_sub( footerMenu, derwin( botWin, 1, NCOLS, 1, 1));
	set_menu_format( footerMenu, 0, 6);

	/* Get key events from the bottom window */
	keypad( botWin, TRUE);

	/* Post the menu */
	post_menu( footerMenu);
	wnoutrefresh( botWin);
}


/* Shows the menu along the bottom of the screen */
void showMenu() {
	getScrnSize(NCOLS, NROWS);
	resizeterm(NROWS, NCOLS);
	showBotWin();
	setMenu();
	doupdate();
}

/* Hides the menu at the bottom of the screen */
void hideMenu() {
	unpost_menu( footerMenu);
	wrefresh( botWin);
}

/* Handles screen resizes */
void hndSIGWINCH (int sig)
{
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
	showMenu( );
	wnoutrefresh( botWin);
	doupdate();

	int ch;
	ITEM *currItem;
	bool keepGoing = true;
	while ( keepGoing ) {
		ch = wgetch( botWin );
		switch (ch) {
		case KEY_LEFT:
			menu_driver( footerMenu, REQ_PREV_ITEM);
			break;

		case KEY_RIGHT:
			menu_driver( footerMenu, REQ_NEXT_ITEM);
			break;

		case 13: /* Enter */
			currItem = current_item(footerMenu);
			wprintw(midWin, "%s\n", item_name(currItem));
			hideMenu();
			showWins();
			keepGoing = false;
			break;

		default:
			/* Hide the menu and break out of the loop */
			hideMenu();
			showWins();
			needsRefresh = true;
			keepGoing = false;
			break;
		}
	}
}

/* Free all memory and quit */
void quit() {
	unpost_menu( footerMenu);
	for(int i = 0; i < NUM_CHOICES ; ++i)
		free_item(menuItems[i]);
	free_menu( footerMenu);
	endwin();
	exit(0);
}

/* Set up the SIGWINCH handler */
void initSigaction() {
	memset (&sa, '\0', sizeof(sa));
	sa.sa_handler = hndSIGWINCH;
}

/* run main GUI loop */
void guiLoop() {

	int ch;

	while ( (ch = wgetch(midWin) ) != 'q') {

		if (sigaction(SIGWINCH, &sa, NULL) == -1)
			printw("SADSADAS");

		switch (ch) {

		case 'd':

			needsRefresh = true;
			break;

		case 'a':

			needsRefresh = true;
			break;

		case 6:
			doMenu();
			break;

		default:
			needsRefresh = true;
			break;
		}

		if (needsRefresh) {
			showWins();
			needsRefresh = false;
		}
	}

}
