
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>


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

int x, y;
WINDOW *topWin, *midWin, *botWin;
_topWin *win;
bool needsRefresh = false;


void init_topWin(_topWin **win) {
	_topWin *p = NULL;
	p = malloc(sizeof(_topWin));
	p->pathLen = 0;
	p->timeLen = 0;
	p->noteNum = 0;

	*win = p;
}

void showWins() {
	/* Kill the window and delete it. Otherwise you get ugly effects */
	if (topWin)
		delwin(topWin);
	endwin();


	/* Get the screen coordinates and recreate windows*/
	int xt, yt;
	getmaxyx(stdscr, yt, xt);
	topWin = newwin(1, xt, 0, 0);
	midWin = newwin(yt, xt, 1, 0);
	mvwprintw(midWin, 0, 0, "THINGSHERE");

	/* Turn colors back on */
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	wattron(topWin,COLOR_PAIR(1));
	wbkgd(topWin,COLOR_PAIR(1));

	char noteStr[100];
	sprintf(noteStr, "Note #%d", win->noteNum);

	/* Print heading text to screen */
	mvwprintw(topWin, 0, (xt/2)-(win->pathLen/2), win->path); // Middle
	mvwprintw(topWin, 0, 0, noteStr); // Left
	mvwprintw(topWin, 0, xt-(win->timeLen+2), win->time); // Right
	wrefresh(topWin);
	mvwprintw(midWin, 0, 0, win->longString);
	wmove(midWin, 0, 0);
	wrefresh(midWin);
}

static void hndlSigwinch (int sig)
{
	showWins();
}

void initNcurses() {
	initscr();
	raw();
	nonl();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
}

void setTop(_topWin *win) {
	win->noteNum = 1;
	win->pathLen = 4;
}


int main (int argc, char *argv[])
{
	struct sigaction sa;
	memset (&sa, '\0', sizeof(sa));
	sa.sa_handler = hndlSigwinch;


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
	showWins();

	char ch;

	while ( (ch = wgetch(topWin) ) != 'q') {

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

	free(win);
	endwin();			/* End curses mode		  */
	return 0;
}
