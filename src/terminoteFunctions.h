/*
 * terminoteFunctions.h
 *
 *  Created on: Feb 23, 2013
 *      Author: fragmachine
 */

#ifndef TERMINOTEFUNCTIONS_H_
#define TERMINOTEFUNCTIONS_H_

/* Prints menu message */
void menuMessage(void);


/* Prints current note */
void printCurrent(node *currP);


/* Prompts user for y or n question. */
/* Returns true for y and false for anything else */
bool promtUserChoice(char *prompt);


/* Prompts user for input, appends input to list.
 * Returns pointer to the new node. */
node *appendNote(char inputBuffer[], int buffSize, node *currP, node *head);


/* Asks user if they want to delete all notes. If so, deletes them. */
void deleteAllNotes(node *currP, node *head);


/* Deletes current currP. Returns pointer to head. */
node *deleteCurrent(node *currP, node *head);


/* Asks user for search term then prints all notes that contain it. */
void printAllWithSubString(node *currP, node *head);

/* Enters UI loop */
void uiLoop(node *currP, node *head);

/* Handles SIGINT by killing run loop */
void sigintHandler(int sig);

/* Runs Terminote in interactive mode */
void runInteractive();

#endif /* TERMINOTEFUNCTIONS_H_ */
