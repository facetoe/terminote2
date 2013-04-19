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



typedef struct topWin {
	WINDOW *win;
	int pathLen;
	int timeLen;
	int noteNum;
	char path[100];
	char time[30];
	char longString[500];
	int longStrLen;
} _topWin;

/* Allocate memory for data struct */
void init_topWin();

void getScrnSize();

/* Setup and print the top window to screen */
void showTopWin();

/* Setup and print the middle window to screen */
void showMidWin();

/* Setup and print the middle window to screen */
void showBotWin();

/* Show the windows (and remove botWin if it exists) */
void showWins();

/* Initialize the menu but don't show it */
void initMainMenu();

/* Setup and show the menu */
void setMainMenu();

/* Shows the menu along the bottom of the screen */
void showMainMenu();

/* Hides the menu at the bottom of the screen */
void hideMainMenu();

/* Initialize the menu but don't show it */
void initStartMenu();

/* Setup and show the menu */
void setStartMenu();

/* Shows the menu along the bottom of the screen */
void showStartMenu();

/* Hides the menu at the bottom of the screen */
void hideStartMenu();

/* Handles screen resizes */
static void hndSIGWINCH(int sig);

/* Setup ncurses */
void initNcurses();

/* Select and execute options from the menu */
void doMenu();

/* Select and execute options from the menu */
void doStartMenu();

/* Free all memory and quit */
void quit();

/* Set up the SIGWINCH handler */
void initSigaction();

/* run main GUI loop */
void guiLoop();



#endif /* UI_H_ */
