#define _POSIX_SOURCE // You need this here to get rid of the warning with sigaction
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <menu.h>

#include <signal.h>

#define SIGWINCH 28


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

typedef struct topWin {
	WINDOW *win;
	int pathLen;
	int timeLen;
	int noteNum;
	char path[100];
	char time[30];
	char longString[500];
	int longStrLen;
}_topWin;

char *choices[] = {
		"Choice 1", "Choice 2", "Choice 3", "Choice 4", "Choice 5",
		"Choice 6",
		(char *)NULL,
};

WINDOW *topWin, *midWin, *botWin;
_topWin *win;
ITEM **menuItems;
MENU *footerMenu;
bool needsRefresh = false;
int x, y;

/* Allocate memory for data struct */
void init_topWin(_topWin **win) {
	_topWin *p = NULL;
	p = malloc(sizeof(_topWin));
	*win = p;
}

/* Setup and print the top window to screen */
void showTopWin(){
	/* If the window already exists, delete it. Otherwise you get ugly effects. */


	/* Create the window*/
	topWin = newwin(1, x, 0, 0);

	/* Turn colors on */
	wattron(topWin,COLOR_PAIR(1));
	wbkgd(topWin,COLOR_PAIR(1));

	char noteStr[100];
	sprintf(noteStr, "Note #%d", win->noteNum);

	/* Print heading text to screen */
	mvwprintw(topWin, 0, (x/2)-(win->pathLen/2), win->path); // Middle
	mvwprintw(topWin, 0, 0, noteStr); // Left
	mvwprintw(topWin, 0, x-(win->timeLen+2), win->time); // Right
	wnoutrefresh(topWin);
}

/* Setup and print the middle window to screen */
void showMidWin() {
	/* If the window already exists, delete it. Otherwise you get ugly effects. */

	midWin = newwin(y - 1, x, 1, 0);
	mvwprintw(midWin, 0, 0, win->longString);
	wmove(midWin, 0, 0);
	wnoutrefresh(midWin);
}

/* Setup and print the middle window to screen */
void showBotWin() {

	botWin = newwin(1, x, y- 1, 0);

	wattron(botWin,COLOR_PAIR(2));
	wbkgd(botWin,COLOR_PAIR(2));
	wnoutrefresh(botWin);
}

/* Initialize the menu but don't show it */
void initMenu() {
	/* Create items */
	int nItems;
	nItems = ARRAY_SIZE(choices);
	menuItems = (ITEM **)calloc(nItems, sizeof(ITEM *));
	for(int i = 0; i < nItems; ++i)
		menuItems[i] = new_item(choices[i], choices[i]);

	/* Create menu */
	footerMenu = new_menu((ITEM **)menuItems);
}

/* Setup and show the menu */
void setMenu() {
	/* Set menu option not to show the description */
	menu_opts_off(footerMenu, O_SHOWDESC);

	/* Set main window and sub window */
	set_menu_win(footerMenu, botWin);
	set_menu_sub(footerMenu, derwin(botWin, 1, x, 1, 1));
	set_menu_format(footerMenu, 0, 6);

	/* Get key events from the bottom window */
	keypad(botWin, TRUE);

	/* Post the menu */
	post_menu(footerMenu);
	wnoutrefresh(botWin);
}

/* Show the windows (and remove botWin if it exists) */
void showWins() {
	getmaxyx(stdscr, y, x);
	showTopWin();
	showMidWin();
	wrefresh(curscr);
}

/* Shows the menu along the bottom of the screen */
void showMenu() {
	getmaxyx(stdscr, y, x);
	showBotWin();
	setMenu();
}

/* Hides the menu at the bottom of the screen */
void hideMenu() {
	unpost_menu(footerMenu);
	wrefresh(botWin);
}

/* Handles screen resizes */
static void hndSIGWINCH (int sig)
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
	showMenu();
	wnoutrefresh(botWin);
	doupdate();

	int ch;
	ITEM *currItem;
	bool keepGoing = true;
	while ( keepGoing ) {
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

void quit() {
	int s = ARRAY_SIZE(choices);
	for(int i = 0; i < s; ++i)
		free_item(menuItems[i]);
	free_menu(footerMenu);
	free(win);
	endwin();
	exit(0);
}

int main (int argc, char *argv[])
{
	struct sigaction sa;
	memset (&sa, '\0', sizeof(sa));
	sa.sa_handler = hndSIGWINCH;

	init_topWin(&win);

	char *str = "This goes in midWin\nNewlines work\nfoo bar bla";

	if(win) {

		win->noteNum = 2;
		strcpy(win->path, "/home/fragmachine/things/yes");
		strcpy(win->time, "03/12/1922 12:34");
		strcpy(win->longString, str);
		win->timeLen = strlen(win->time);
		win->pathLen = strlen(win->path);
		win->longStrLen = strlen(str);

	} else {
		abort();
	}

	initNcurses();
	initMenu();
	showWins();


	int ch;

	while ( (ch = wgetch(midWin) ) != 'q') {

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
			win->noteNum = (int)ch;
			needsRefresh = true;
			break;

		case 6:
			doMenu();
			break;

		default:
			win->noteNum = (int)ch;
			needsRefresh = true;
			break;
		}

		if (needsRefresh) {
			showWins();
			needsRefresh = false;
		}
	}

	quit();

	/* Shouldn't get here... */
	return 0;
}
