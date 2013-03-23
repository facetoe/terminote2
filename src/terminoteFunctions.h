/*
    Terminote - a simple, terminal based note program.
    Copyright (C) 2013  Facetoe.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef TERMINOTEFUNCTIONS_H_
#define TERMINOTEFUNCTIONS_H_

typedef struct{
	/* Pop last note ( only the message )*/
	int pop;
	/* Pop the last note ( with time and path info ) */
	int popA;
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

	/* Print the version */
	int version;

	/* Print Usage */
	int usage;

	/* Output to file instead of stdout */
	int outputToFile;
	char *outFile;

}Options;

/* Prints menu message */
void menuMessage(void);

/* Prints usage */
void printUsage( FILE *outStream );

/* Prints current note */
void printCurrent(FILE *outStream, char *args, node *currP);

/* Prints note number n */
void printN(FILE *outStream, node *currP, int n);

/* Pops a note off the list. In other words, prints last note then deletes it. If there are no notes to print, send an error to stderr. */
/* This function only prints the message, no path or number. */
void popNote(FILE *outStream, node *currP);

/* Pops note number n and deletes it. */
/* This function only prints the message, no path or number. */
void popN(FILE *outStream, node *currP, int n);

/* Asks user for search term then prints all notes that contain it. */
void printAllWithSubStringInteractive(node *currP);

/* Prints all notes that contain substring */
void printAllWithSubString(node *currP, char *subString);

/* Prompts user for y or n question. */
/* Returns true for y and false for anything else */
bool promtUserChoice(char *prompt);

/* Prompts user for input, appends input to list.
 * Returns pointer to the new node. */
node *appendNoteInteractive(char inputBuffer[], int buffSize, node *currP);

/* Asks user if they want to delete all notes. If so, deletes them */
void deleteAllNotesInteractive(node *currP);

/* Deletes current currP. Returns pointer to head if successful or currP if not. This version asks the user if they want to delete and deletes if the answer is y */
node *deleteCurrentInteractive(node *currP);

/* Deletes current currP.
 * WARNING: This version is for non-interactive, it doesn't return anything, prompt the user or clean up after itself. */
void deleteCurrent(node *currP);

/* Deletes note number n */
void deleteN(node *currP, int n);

/* Deletes all notes. Non interactive. */
void deleteA(node *currP);

/* Enters UI loop */
void uiLoop(node *currP);

/* Handles SIGINT by killing run loop */
void sigintHandler(int sig);

/* Runs Terminote in interactive mode */
void runInteractive();

/* Runs Terminote in non interactive mode */
void runNonInteractive(Options *options, int argc, char **argv);

/* Initialize options struct */
void initOptions(Options *opts);

/* Print options for debugging */
void printOpts(Options *opts);

/* Parse command line options */
void parseOptions(Options *options, int argc, char **argv);

/* Ensures options make sense */
void validateOptions(Options *opts);

/* Executes options then destroys the list */
void executeOptions(Options *opts, node *currP);

#endif /* TERMINOTEFUNCTIONS_H_ */
