#define _POSIX_SOURCE // You need this here to get rid of the warning with sigaction
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <menu.h>

#include <sys/types.h>
#include <signal.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define MIDWINPAD 2
#define BOTWINPAD 1

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
		" ", "Choice 1", "Choice 2", "Choice 3", "Choice 4", "Choice 5",
		"Choice 6",
		(char *)NULL,
};

int x, y;
WINDOW *topWin, *midWin, *botWin;
_topWin *win;
ITEM **menuItems;
MENU *footerMenu;
bool needsRefresh = false;
int x, y;


void init_topWin(_topWin **win) {
	_topWin *p = NULL;
	p = malloc(sizeof(_topWin));
	p->pathLen = 0;
	p->timeLen = 0;
	p->noteNum = 0;

	*win = p;
}

void showTopWin(){
	/* Kill the window and delete it. Otherwise you get ugly effects */
	if (topWin)
		delwin(topWin);
	endwin();

	/* Get the screen coordinates and recreate window*/
	topWin = newwin(1, x, 0, 0);

	/* Turn colors back on */
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	wattron(topWin,COLOR_PAIR(1));
	wbkgd(topWin,COLOR_PAIR(1));

	char noteStr[100];
	sprintf(noteStr, "Note #%d", win->noteNum);

	/* Print heading text to screen */
	mvwprintw(topWin, 0, (x/2)-(win->pathLen/2), win->path); // Middle
	mvwprintw(topWin, 0, 0, noteStr); // Left
	mvwprintw(topWin, 0, x-(win->timeLen+2), win->time); // Right
	wrefresh(topWin);
}

void showMidWin() {
	if (midWin)
		delwin(midWin);

	endwin();
	midWin = newwin(y - MIDWINPAD, x, 1, 0);
	mvwprintw(midWin, 0, 0, win->longString);
	wmove(midWin, 0, 0);
	wrefresh(midWin);
}

void showBotWin() {
	botWin = newwin(4, x, y-BOTWINPAD, 0);
	mvwprintw(midWin, 0, 0, win->longString);
}

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

void setMenu() {
	unpost_menu(footerMenu);

	/* Set menu option not to show the description */
	menu_opts_off(footerMenu, O_SHOWDESC);

	/* Set main window and sub window */
	set_menu_win(footerMenu, botWin);
	set_menu_sub(footerMenu, derwin(botWin, 1, x, 1, 1));
	set_menu_format(footerMenu, 0, 6);

	/* Post the menu */
	post_menu(footerMenu);
	keypad(botWin, TRUE);
	wrefresh(botWin);
}

void showWins() {
	getmaxyx(stdscr, y, x);
	showTopWin();
	showMidWin();
}

void showMenu() {
	getmaxyx(stdscr, y, x);
	showTopWin();
	showBotWin();
	setMenu();
	showMidWin();
}

static void hndSIGWINCH (int sig)
{
	showWins();
}

void initNcurses() {
	initscr();
	raw();
	nonl();
	noecho();
	cbreak();
	//keypad(stdscr, TRUE);
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

		case KEY_LEFT:
			menu_driver(footerMenu, REQ_PREV_ITEM);
			break;

		case KEY_RIGHT:
			menu_driver(footerMenu, REQ_NEXT_ITEM);
			break;

		case 13: /* Enter */
		{	ITEM *cur;

			cur = current_item(footerMenu);
			strcpy(win->longString, item_name(cur));
			pos_menu_cursor(footerMenu);
			needsRefresh = true;
			break;
		}
		break;


		case 6:
			strcpy(win->longString, "You Pressed the modify key");
			showMenu();
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

	int s = ARRAY_SIZE(choices);
	for(int i = 0; i < s; ++i)
		free_item(menuItems[i]);
	free_menu(footerMenu);
	free(win);
	endwin();
	return 0;
}
