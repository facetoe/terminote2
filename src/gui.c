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

volatile GUIDATA *gd;
volatile int NCOLS;
volatile int NROWS;

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
	if ( !gd->topWin )
		gd->topWin = newwin(1, NCOLS, 0, 0);

	/* Turn colors on */
	wattron(gd->topWin,COLOR_PAIR(1));
	wbkgd(gd->topWin,COLOR_PAIR(1));

	char noteStr[100];
	sprintf(noteStr, "Note #%d", NCOLS);

	// win stuff was here
	wprintw(gd->topWin, "%d %d", NROWS, NCOLS);

	wnoutrefresh(gd->topWin);
}

/* Setup and print the middle window to screen */
void showMidWin() {
	if (!gd->midWin)
		gd->midWin = newwin(NROWS - 2, NCOLS, 1, 0);
	wmove(gd->midWin, 0, 0);
	wprintw(gd->midWin, "FUCKFUYCKFYCJ");
	wnoutrefresh(gd->midWin);
}

/* Setup and print the middle window to screen */
void showBotWin() {
	if (!gd->botWin)
		gd->botWin = newwin(1, NCOLS, NROWS- 1, 0);
	wattron(gd->botWin,COLOR_PAIR(2));
	wbkgd(gd->botWin,COLOR_PAIR(2));

	wnoutrefresh(gd->botWin);
	keypad(gd->botWin, true);
}


/* Show the windows (and remove botWin if it exists) */
void showWins() {
	getScrnSize();
	resizeterm(NROWS, NCOLS);
	showTopWin(gd);
	showMidWin(gd);
	wrefresh(curscr); // This redraws everything
}

/* Initialize the menu but don't show it */
void initMenu(char *choices[]) {
	gd->menuItems = (ITEM **)calloc(NUM_CHOICES, sizeof(ITEM *));
	for(int i = 0; (char*)choices[i] != (char*)NULL; ++i)
		gd->menuItems[i] = new_item(choices[i], choices[i]);

	/* Create menu */
	gd->footerMenu = new_menu((ITEM **)gd->menuItems);
}


/* Setup and show the menu */
void setMenu() {
	/* Set menu option not to show the description */
	menu_opts_off(gd->footerMenu, O_SHOWDESC);

	/* Set main window and sub window */
	set_menu_win(gd->footerMenu, gd->botWin);
	set_menu_sub(gd->footerMenu, derwin(gd->botWin, 1, NCOLS, 1, 1));
	set_menu_format(gd->footerMenu, 0, 6);

	/* Get key events from the bottom window */
	keypad(gd->botWin, TRUE);

	/* Post the menu */
	post_menu(gd->footerMenu);
	wnoutrefresh(gd->botWin);
}


/* Shows the menu along the bottom of the screen */
void showMenu() {
	getScrnSize(NCOLS, NROWS);
	resizeterm(NROWS, NCOLS);
	setMenu(gd);
	showBotWin(gd);
	wrefresh(curscr);
}

/* Hides the menu at the bottom of the screen */
void hideMenu() {
	unpost_menu(gd->footerMenu);
	wrefresh(gd->botWin);
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
	showBotWin(gd);
	showMenu(gd);
	wnoutrefresh(gd->botWin);
	doupdate();

	int ch;
	ITEM *currItem;
	bool keepGoing = true;
	while ( keepGoing ) {
		ch = wgetch(gd->botWin);
		switch (ch) {
		case KEY_LEFT:
			menu_driver(gd->footerMenu, REQ_PREV_ITEM);
			break;

		case KEY_RIGHT:
			menu_driver(gd->footerMenu, REQ_NEXT_ITEM);
			break;

		case 13: /* Enter */
			currItem = current_item(gd->footerMenu);
			hideMenu(gd);
			showWins(gd);

			keepGoing = false;
			break;

		default:
			/* Hide the menu and break out of the loop */
			hideMenu(gd);
			showWins(gd);
			needsRefresh = true;
			keepGoing = false;
			break;
		}
	}
}

/* Free all memory and quit */
void quit() {
	unpost_menu(gd->footerMenu);
	for(int i = 0; (char*)gd->menuItems[i] != (char*)NULL ; ++i)
		free_item(gd->menuItems[i]);
	free_menu(gd->footerMenu);
	free(gd->menuItems);

	endwin();
}

/* Set up the SIGWINCH handler */
void initSigaction() {
	memset (&sa, '\0', sizeof(sa));
	sa.sa_handler = hndSIGWINCH;
}

/* run main GUI loop */
void guiLoop(GUIDATA  *gd) {

	int ch;

	while ( (ch = wgetch(gd->midWin) ) != 'q') {

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
			doMenu(gd);
			break;

		default:
			needsRefresh = true;
			break;
		}

		if (needsRefresh) {
			showWins(gd);
			needsRefresh = false;
		}
	}

}
