/*
 * ui.h
 *
 *  Created on: 17/04/2013
 *      Author: facetoe
 */

#ifndef UI_H_
#define UI_H_

#define _POSIX_SOURCE // You need this here to get rid of the warning with sigaction

#include <ncurses.h>
#include <menu.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "linkedList.h"

struct line {
	int lNum;
	int lSize;
	char *line;
	struct line *next;
};
typedef struct line LINE;

/* Get the size of the terminal screen */
void getScrnSize();

/* Setup and print the top window to screen */
void showTopWin();

/* Parses a listNode and separates each line into a LINE node for displaying in ncurses */
void parseMessage();

/* Free all memory in the LINE struct */
void destroyLineData();

/* Refresh the middle window */
void refreshMidwin();

/* Setup and print the middle window to screen */
void showMidWin();

/* Setup and print the bottom window to screen */
void showBotWin();

/* Show the windows */
void showWins();

/* Initialize the menu but don't show it */
void initMainMenu();

/* Setup and show the menu */
void setMainMenu();

/* Shows the menu along the bottom of the screen */
void showMainMenu();

/* Hides the menu at the bottom of the screen */
void hideMainMenu();

/* Handles screen resizes */
void hndSIGWINCH(int sig);

/* Setup ncurses */
void initNcurses();

/* Free all memory and quit */
void quit();

/* Set up the SIGWINCH handler */
void initSigaction();

/* Select and execute options from the menu */
void doMenu();

/* run main GUI loop */
void guiLoop();


#endif /* UI_H_ */
