/*
 * ui.h
 *
 *  Created on: 21/07/2013
 *      Author: facetoe
 */

#ifndef UI_H_
#define UI_H_

#define _POSIX_SOURCE // You need this here to get rid of the waring with sigaction
#include "helperFunctions.h"
#include "linkedList.h"
#include "options.h"
#include "defines.h"
#include "nonInteractive.h"

#include "ncurses.h"
#include "menu.h"
#include "assert.h"

#include <sys/ioctl.h> // For terminal size
#include <sys/types.h> // for pid_t
#include <sys/wait.h> // for waitpid
#include <signal.h> // For sigaction

/* Allocate and initialize a POSITION struct */
void initPosition( DISPLAY_DATA **disp );

/* Get the size of the terminal screen */
void getScrnSize( DISPLAY_DATA *disp );

/* Setup ncurses */
void initNcurses();

void initWins( DISPLAY_DATA *disp );

/* Setup and print the top window to screen */
void showTopWin( DISPLAY_DATA *disp );

/* Print the current page */
void printPage( DISPLAY_DATA *disp, int numRows );

/* Print the start of the message */
void printTop( DISPLAY_DATA *disp );

/* Print the bottom of the message */
void printBot( DISPLAY_DATA *disp );

/* Scrolls the page up  */
void scrollUpPage( DISPLAY_DATA *disp, WINDOW *win );

/* Scrolls the page down */
void scrollDownPage( DISPLAY_DATA *disp, WINDOW *win );

/* Setup and print the middle window to screen */
void showMidWin( DISPLAY_DATA *disp );

/* Setup and print the bottom window to screen */
void showBotWin();

/* Show the windows */
void showWins( DISPLAY_DATA *disp );

/* Initialize the menu but don't show it */
void initMainMenu();

/* Setup and show the menu */
void setMainMenu( DISPLAY_DATA *disp );

/* Show the menu along the bottom of the screen */
void showMainMenu( DISPLAY_DATA *disp );

/* Hide the menu at the bottom of the screen */
void hideMainMenu();

/* Resize the terminal screen */
void resizeScreen( DISPLAY_DATA *disp );

/* Sets the RECIEVED_SIGWINCH flag to true so we can deal with resizing the screen */
void hndSIGWINCH( int sig );

/* Set up the SIGWINCH handler */
void initSigaction();

/* Prints the help message */
void printHelp();

/* Free all memory and quit */
void quit( MESSAGE *msg );

/* Select and execute options from the menu */
void doMenu( DISPLAY_DATA *disp );

void clearPosition( DISPLAY_DATA *disp );

/* run main GUI loop */
void run();



#endif /* UI_H_ */
