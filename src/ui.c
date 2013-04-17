
#include "ui.h"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define SIGWINCH 28 // Needed because OSX doesn't recognize SIGWINCH
struct sigaction sa;

struct winsize wSize; // I'm using this instead of getmaxyx() because it didn't seem to always work.



char *choices[] = { "Choice 1", "Choice 2", "Choice 3", "Choice 4", "Choice 5",
		"Choice 6", (char *) NULL, };

/* Global variables */
WINDOW *topWin, *midWin, *botWin;
_topWin *win;
ITEM **menuItems;
MENU *footerMenu;
bool needsRefresh = false;
int NCOLS, NROWS;

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

	char noteStr[100];
	sprintf(noteStr, "%d %d", NCOLS, NROWS);

	mvwprintw(topWin, 0, (NCOLS / 2) - (win->pathLen / 2), win->path);
	mvwprintw(topWin, 0, 0, noteStr);
	mvwprintw(topWin, 0, (NCOLS - win->timeLen) - 2, win->time);
	wnoutrefresh(topWin);
}

/* Setup and print the middle window to screen */
void showMidWin() {
	midWin = newwin(NROWS - 2, NCOLS, 1, 0);
	mvwprintw(midWin, 0, 0, win->longString);
	wmove(midWin, 0, 0);
	wnoutrefresh(midWin);
}

/* Setup and print the middle window to screen */
void showBotWin() {
	if(botWin)
		delwin(botWin);
	botWin = newwin(1, NCOLS, NROWS - 1, 0);
	wattron(botWin, COLOR_PAIR(2));
	wbkgd(botWin, COLOR_PAIR(2));
	wnoutrefresh(botWin);
}

/* Initialize the menu but don't show it */
void initMenu() {
	/* Create items */
	int nItems;
	nItems = ARRAY_SIZE(choices);
	menuItems = (ITEM **) calloc(nItems, sizeof(ITEM *));
	for (int i = 0; i < nItems; ++i)
		menuItems[i] = new_item(choices[i], choices[i]);

	/* Create menu */
	footerMenu = new_menu((ITEM **) menuItems);
}

/* Setup and show the menu */
void setMenu() {
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

/* Show the windows (and remove botWin if it exists) */
void showWins() {
	getScrnSize(NCOLS, NROWS);
	resizeterm(NROWS, NCOLS);
	showTopWin();
	showMidWin();
	wrefresh(curscr);
	doupdate();
}

/* Shows the menu along the bottom of the screen */
void showMenu() {
	getScrnSize(NCOLS, NROWS);
	showBotWin();
	setMenu();
}

/* Hides the menu at the bottom of the screen */
void hideMenu() {
	unpost_menu(footerMenu);
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
	showMenu();
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
	int s = ARRAY_SIZE(choices);
	for (int i = 0; i < s; ++i)
		free_item(menuItems[i]);
	free_menu(footerMenu);
	free(win);
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
	char *str = "This goes in midWin\nNewlines work\nfoo bar bla";

	if (win) {

		win->noteNum = 2;
		strcpy(win->path, "/home/fragmachine/Diffpath");
		strcpy(win->time, "03/12/1922 01:24");
		strcpy(win->longString, str);
		win->timeLen = strlen(win->time);
		win->pathLen = strlen(win->path);
		win->longStrLen = strlen(str);
		needsRefresh = true;

	} else {
		abort();
	}

	int ch;

	while ((ch = wgetch(midWin)) != 'q') {

		if (sigaction(SIGWINCH, &sa, NULL) == -1)
			printw("SADSADAS");

		switch (ch) {

		case 'd':
			win->noteNum = 2;
			strcpy(win->path, "/home/fragmachine/Diffpath");
			strcpy(win->time, "03/12/1922 01:24");
			strcpy(win->longString, str);
			win->timeLen = strlen(win->time);
			win->pathLen = strlen(win->path);
			win->longStrLen = strlen(str);
			needsRefresh = true;
			break;

		case 'a':
			win->noteNum = 3;
			strcpy(win->path, "IS CHANGING!");
			strcpy(win->time, "04/12/1922 12:34");
			strcpy(win->longString, str);
			win->timeLen = strlen(win->time);
			win->pathLen = strlen(win->path);
			win->longStrLen = strlen(str);
			win->noteNum = (int) ch;
			needsRefresh = true;
			break;

		case 6:
			doMenu();
			break;

		default:
			win->noteNum = (int) ch;
			needsRefresh = true;
			break;
		}

		if (needsRefresh) {
			showWins();
			needsRefresh = false;
		}
	}

}
