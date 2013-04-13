
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
WINDOW *topWin;
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
	wclear(topWin);
	char str[100];
	int xt, yt;
	getmaxyx(stdscr, yt, xt);
	sprintf(str, "Coords: x: %d y: %d", xt, yt);
	wborder(topWin, ' ', ' ', '-', '-', '+', '+', '+', '+');
	mvwprintw(topWin, 1, (xt/2)-1, str);
	wrefresh(topWin);
}

static void hdl (int sig)
{
	draw();
}



int main (int argc, char *argv[])
{
	struct sigaction sa;
	memset (&sa, '\0', sizeof(sa));
	sa.sa_handler = hdl;

	initscr();
	raw();
	nonl();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);


	char str[100];
	char *sprintfKey = "Coords: x: %d y: %d";

	getmaxyx(stdscr, y, x);
	topWin = newwin(4, x, 0, 0);
	int len = strlen(sprintfKey);
	sprintf(str, "Coords: x: %d y: %d", x, y);
	mvwprintw(topWin, 1, (x/2)-(len/2), str);
	wborder(topWin, ' ', ' ', '-', '-', '+', '+', '+', '+');
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
