
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>


#include <ncurses.h>


typedef struct topWin {
	WINDOW *win;
	int numLines;
	int numCols;
	int noteNum;
	char path[100];
	char time[30];
}_topWin;

int x, y;
WINDOW *topWin, *midWin;
_topWin *winStruct;

void init_topWin(_topWin **win) {
	_topWin *p = NULL;
	p = malloc(sizeof(p));
	p->numLines = 0;
	p->numCols = 0;
	p->noteNum = 0;
	memset(p->path, '\0', 100);
	memset(p->time, '\0', 30);

	*win = p;
}

void draw() {
	endwin();
	if (topWin)
		delwin(topWin);

	char str[100];
	int xt, yt;
	char *sprintfKey = "/home/fragmachine/people/lol/hahahga";
	int len = strlen(sprintfKey);
	getmaxyx(stdscr, yt, xt);
	topWin = newwin(1, xt, 0, 0);

	midWin = newwin(yt, xt, 2, 2);
	wrefresh(midWin);

	start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	wattron(topWin,COLOR_PAIR(1));
	wbkgd(topWin,COLOR_PAIR(1));

	sprintf(str, "Coords: x: %d y: %d", xt, yt);
	mvwprintw(topWin, 0, (xt/2)-(len/2), sprintfKey);
	mvwprintw(topWin, 0, 0, "Note #2");
	char *time = "03/12/23 23:10";
	len = strlen(time);
	mvwprintw(topWin, 0, xt-(len+1), "03/12/23 23:10");
	wrefresh(topWin);
}

static void hndlSigwinch (int sig)
{
	draw();
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
	win->numLines = 4;
}



int main (int argc, char *argv[])
{
	struct sigaction sa;
	memset (&sa, '\0', sizeof(sa));
	sa.sa_handler = hndlSigwinch;


	initNcurses();

	char str[100];
	char *sprintfKey = "/home/facetoe/git/workspace/asss/lol";

	getmaxyx(stdscr, y, x);
	topWin = newwin(1, x, 0, 0);

	start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	wattron(topWin,COLOR_PAIR(1));
	wbkgd(topWin,COLOR_PAIR(1));


	midWin = newwin(y, x, 0, 0);
	wrefresh(midWin);

	int len = strlen(sprintfKey);
	sprintf(str, "Coords: x: %d y: %d", x, y);
	mvwprintw(topWin, 0, (x/2)-(len/2), sprintfKey);
	mvwprintw(topWin, 0, 0, "Note #2");
	mvwprintw(topWin, 0, x-10, "Hello");
	wrefresh(topWin);

	sigemptyset(&sa.sa_mask);
			    sa.sa_flags = SA_RESTART; /* Restart functions if
			                                 interrupted by handler */

	while (wgetch(topWin)) {

		    if (sigaction(SIGWINCH, &sa, NULL) == -1)
		        printw("SADSADAS");


	}



	endwin();			/* End curses mode		  */
	return 0;
}
