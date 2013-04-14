
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
}_topWin;

int x, y;
WINDOW *topWin, *midWin;
_topWin *winStruct;
_topWin *win;


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
	/* Refresh midWin before topWin or the screen gets all messed up */
	wrefresh(midWin);

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

	if(win) {

	win->noteNum = 2;
	strcpy(win->path, "/home/fragmachine/things/yes");
	strcpy(win->time, "03/12/1922 12:34");
	win->timeLen = strlen(win->time);
	win->pathLen = strlen(win->path);
	}
	initNcurses();
	showWins();



	while (wgetch(topWin) != 'q') {

		    if (sigaction(SIGWINCH, &sa, NULL) == -1)
		        printw("SADSADAS");


	}


	free(win);
	endwin();			/* End curses mode		  */
	return 0;
}
