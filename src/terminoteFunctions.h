/*
 * terminoteFunctions.h
 *
 *  Created on: Feb 23, 2013
 *      Author: fragmachine
 */

#ifndef TERMINOTEFUNCTIONS_H_
#define TERMINOTEFUNCTIONS_H_

typedef struct{
	/* Pop last note */
	int pop;
	/* Pop n note */
	int popN;

	/* Delete n note */
	int delN;
	/* Delete all notes */
	int delA;

	/* Print n note */
	int printN;
	/* Print all notes */
	int printA;

	/* Search notes for search term */
	int searchNotes;
	char searchTerm[MAX_MESSAGE_SIZE-1];

	/* Append note */
	int append;
	char *appendStr;

}Options;

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
void deleteAllNotesInteractive(node *currP, node *head);

/* Deletes current currP. Returns pointer to head. This version asks the user if they want to delete and deletes if the answer is y */
node *deleteCurrentInteractive(node *currP, node *head);

/* Deletes current currP.
 * WARNING: This version is for non-interactive, it doesn't return anything, prompt the user or clean up after itself. */
void deleteCurrent(node *currP, node *head);

/* Pops a note off the list. In other words, prints last note then deletes it. If there are no notes to print, send an error to stdout. */
void popNote(node *currP, node *head);

/* Pops note n */
void popN(node *currP, node *head, int n);

/* Asks user for search term then prints all notes that contain it. */
void printAllWithSubStringInteractive(node *currP, node *head);

/* Enters UI loop */
void uiLoop(node *currP, node *head);

/* Handles SIGINT by killing run loop */
void sigintHandler(int sig);

/* Runs Terminote in interactive mode */
void runInteractive();

/* Runs Terminote in pipe mode */
void runNonInteractive(Options *options, int argc, char **argv);

/* Initialize options struct */
void initOptions(Options *opts);

/* Print options for debugging */
void printOpts(Options *opts);

/* Parse command line options */
void parseOptions(Options *options, int argc, char **argv);

/* Returns true if any commandline options were set */
bool hasOptions( Options *opts );

/* Ensures options make sense */
void validateOptions(Options *opts);

/* Execute the command line options */
void executeOptions(Options *opts, node *currP, node *head);

#endif /* TERMINOTEFUNCTIONS_H_ */
