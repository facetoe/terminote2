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

#include "helperFunctions.h"
#include "linkedList.h"
#include "terminoteFunctions.h"
#include "defines.h"
#include <signal.h>
#include <getopt.h>

/* Prints menu message */
void menuMessage(void) {
	printf("\nWelcome to Terminote interactive:\n"
			"(w) to insert new note\n"
			"(a) and (d) to navigate through notes\n"
			"(e) to print all notes\n"
			"(g) to delete all notes\n"
			"(f) to search for notes\n"
			"(m) to print this message\n"
			"(q) to quit\n");
}

/* Prints usage */
void printUsage(FILE *outStream) {
	fprintf(outStream,
			"Terminote version %.1f - a command line note tool.\n\n"
					"Terminote reacts differently depending on how you call it.\n"
					"If you pipe data to it, or supply a command line argument, Terminote runs in non-interactive mode.\n"
					"If you call terminote with no arguments from the shell, terminote will enter interactive mode.\n\n"
					"ARGUMENTS: Arguments in capitals are destructive, lower case leaves notes intact.\n"
					"	-h: Print this message and quit.\n"
					"	-P: Prints the last note only, no path or number, then deletes it.\n"
					"	-F: Prints the last note with full info (path/time/num) then deletes it.\n"
					"	-N: Prints the note at the supplied note number and deletes it, if it exists. Requires an integer argument. \n"
					"	-D: Deletes the note at supplied note number, if it exists. Requires an integer argument.\n"
					"	-R: Deletes all notes.\n"
					"	-p: Prints the note at the supplied note number, leaving it intact. Requires an integer argument.\n"
					"	-l: Prints all the notes leaving them intact.\n"
					"	-f: Searches for notes containing supplied sub string and prints them, leaving them intact. Requires a string argument.\n"
					"	-a: Appends a note to the list. Requires a string argument.\n\n"
					"CONTACT:\n"
					"  Please email any bugs, requests or hate mail to facetoe@ymail.com, or file a bug at https://github.com/facetoe/terminote2\n",
			VERSION_NUM);
}

/* Prints all notes. This is meant for non-interactive */
void printAll(FILE *outStream, node *currP, node *head) {
	currP = head;
	while ((currP = currP->next) != NULL )
		fprintf(outStream, "NoteNum: %d"
				"\nPath: %s"
				"\nTime: %s"
				"\n\n%s\n\n", currP->note_num, currP->path, currP->time,
				currP->message);
}

/* Prints current note */
void printCurrent(node *currP) {
	if (currP == NULL || currP->note_num == 0)
		fprintf(stdout, "Nothing to print.\n");
	else {
		printf("NoteNum: %d"
				"\nPath: %s"
				"\nTime: %s"
				"\n\n%s\n", currP->note_num, currP->path, currP->time,
				currP->message);
	}
}

/* Prints the current node's message only */
void printMessage(FILE *outStream, node *currP) {
	if (currP == NULL || currP->note_num == 0)
		return;

	else {
		fprintf(outStream, "%s", currP->message);
	}
}

/* Prints the full message with path/time info included */
void printMessageAll(FILE *outStream, node *currP) {
	if (currP == NULL || currP->note_num == 0)
		return;
	else {
		fprintf(outStream, "NoteNum: %d"
				"\nPath: %s"
				"\nTime: %s"
				"\n\n%s\n", currP->note_num, currP->path, currP->time,
				currP->message);
	}
}

/* Prints note number n */
void printN(FILE *outStream, node *currP, node *head, int n) {
	currP = searchByNoteNum(currP, head, n);
	if (currP && currP->note_num == n)
		printMessageAll(outStream, currP);
	else
		fprintf(stderr, "No note number: %d\n", n);
}

/* Pops a note off the list. In other words, prints last note then deletes it. If there are no notes to print, send an error to stderr. */
/* This function only prints the message, no path or number. */
void popNote(FILE *outStream, node *currP, node *head) {
	currP = lastNode(currP, head);

	if (currP) {
		printMessage(outStream, currP);
		deleteCurrent(currP, head);
		saveList(head);
	} else {
		fprintf(stderr, "No notes to pop\n");
	}
}

/* Pops a not off the list with full path and time info */
void popNoteInfo(FILE *outStream, node *currP, node *head) {
	currP = lastNode(currP, head);

	if (currP) {
		printMessageAll(outStream, currP);
		deleteCurrent(currP, head);
		saveList(head);
	} else {
		fprintf(stderr, "No notes to pop\n");
	}
}

/* Pops note number n and deletes it. */
/* This function only prints the message, no path or number. */
void popN(FILE *outStream, node *currP, node *head, int n) {
	currP = searchByNoteNum(currP, head, n);
	if (currP) {
		printMessage(outStream, currP);
		deleteCurrent(currP, head);
		orderList(currP);
		saveList(head);
	} else {
		fprintf(stderr, "No note number: %d\n", n);
	}
}

/* Asks user for search term then prints all notes that contain it. */
void printAllWithSubStringInteractive(node *currP, node *head) {
	int found = 0;
	currP = head;
	FLUSH_STDIN(Junk);
	printf("Enter search term:\n> ");
	getInput(inputBuffer, MAX_MESSAGE_SIZE);

	/* Don't check root node */
	if (currP->note_num == 0)
		currP = currP->next;

	while (currP) {
		if (hasSubstring(currP, inputBuffer)) {
			found++;
			printCurrent(currP);
		}
		currP = currP->next;
	}

	if (found == 0)
		printf("Nothing found.\n");

	FLUSH_STDIN(Junk);
}

/* Prints all notes that contain substring */
void printAllWithSubString(node *currP, char *subString) {

	int found = 0;
	/* Don't check root node */
	if (currP->note_num == 0 && currP->next != NULL )
		currP = currP->next;

	if (currP->note_num == 0) {
		fprintf(stderr, "No notes to search\n");
		return;
	}

	while (currP) {
		if (hasSubstring(currP, subString)) {
			found++;
			printCurrent(currP);
		}
		currP = currP->next;
	}

	if (found == 0)
		fprintf(stderr, "Nothing found.\n");

}

/* Prompts user for y or n question. */
/* Returns true for y and false for anything else */
bool promtUserChoice(char *prompt) {
	char choice;
	printf("%s\n> ", prompt);
	scanf("%c", &choice);

	if (tolower(choice) == 'y')
		return true;
	else
		return false;
}

/* Prompts user for input, appends input to list.
 * Returns pointer to the new node. */
node *appendNoteInteractive(char inputBuffer[], int buffSize, node *currP,
		node *head) {
	printf("Enter Note\n> ");
	FLUSH_STDIN(Junk);
	if (getInput(inputBuffer, buffSize) >= MAX_MESSAGE_SIZE)
		fprintf(stderr, "Input too large, truncated\n");
	else
		printf("Added note.\n");
	FLUSH_STDIN(Junk);
	currP = head;
	return append(currP, inputBuffer);
}

/* Asks user if they want to delete all notes. If so, deletes them */
void deleteAllNotesInteractive(node *currP, node *head) {
	FLUSH_STDIN(Junk);
	if (listLength(currP) == 0) {
		printf("Nothing to delete.\n");
		FLUSH_STDIN(Junk);
		return;

	} else if (promtUserChoice("Delete all notes [y/n]?")) {
		currP = head;
		deleteAll(currP);
		if (listLength(currP) == 0)
			printf("Deleted.\n");
	} else {
		printf("Nothing deleted.\n");
	}
	/* Reset to head otherwise we are sitting in a NULL node */
	currP = head;

	FLUSH_STDIN(Junk);
}

/* Deletes current currP. Returns pointer to head if successful or currP if not. This version asks the user if they want to delete and deletes if the answer is y */
node *deleteCurrentInteractive(node *currP, node *head) {
	FLUSH_STDIN(Junk);
	if (currP->note_num == 0 && currP->next == NULL ) {
		printf("Nothing to delete.\n");
		return head;
	}

	if (promtUserChoice("Delete current note[y/n]?")) {
		deleteNode(currP, head, currP->note_num);
		printf("Deleted.\n");
		currP = head;
		orderList(currP);
		FLUSH_STDIN(Junk);
		return head;
	} else {
		FLUSH_STDIN(Junk);
		return currP;
	}
}

/* Deletes current currP.
 * WARNING: This version is for non-interactive, it doesn't return anything, prompt the user or clean up after itself. */
void deleteCurrent(node *currP, node *head) {
	if (currP->note_num == 0 && currP->next == NULL ) {
		return;
	}
	deleteNode(currP, head, currP->note_num);
}

/* Deletes note number n */
void deleteN(node *currP, node *head, int n) {
	currP = searchByNoteNum(currP, head, n);

	if (currP) {
		deleteCurrent(currP, head);
		currP = head;
		orderList(currP);
		saveList(head);
	} else {
		fprintf(stderr, "No note number: %d\n", n);
	}
}

/* Deletes all notes. Non interactive. */
void deleteA(node *currP, node *head) {
	currP = head;
	if (listLength(currP)) {
		deleteAll(currP);
		saveList(head);
	} else {
		fprintf(stderr, "Nothing to delete\n");
	}
}

/* Enters UI loop */
void uiLoop(node *currP, node *head) {
	char opt;
	int lastNoteNum = 0;
	menuMessage();

	while (keepRunning) {

		/* Loop until we get something other then a newline */
		while ((opt = getchar()) == '\n')
			;

		/* Prevents the switch being executed one last time on SIGINT */
		if (keepRunning == 0)
			break;

		lastNoteNum = currP->note_num;

		switch (opt) {

		/* Exit */
		case 'q':
			keepRunning = 0;
			break;

			/* Next */
		case 'd':
			if (listLength(currP)) {
				currP = next(head, currP);
				/* Don't print the note if nothing changed. */
				if (lastNoteNum != currP->note_num && lastNoteNum != 0)
					printCurrent(currP);
			}
			break;

			/* Previous */
		case 'a':
			if (listLength(currP)) {
				currP = previous(head, currP);
				/* Don't print the note if nothing changed. */
				if (lastNoteNum != currP->note_num && lastNoteNum != 0)
					printCurrent(currP);
			}
			break;

			/* Get input and append to list */
		case 'w':
			currP = appendNoteInteractive(inputBuffer, MAX_MESSAGE_SIZE, currP,
					head);
			break;

			/* Print */
		case 'e':
			printList(currP);
			break;

			/* Delete all notes */
		case 'g':
			deleteAllNotesInteractive(currP, head);
			currP = head;
			break;

			/* Find and print all notes containing search term */
		case 'f':
			printAllWithSubStringInteractive(currP, head);
			break;

			/* Print menu message */
		case 'm':
			menuMessage();
			break;

			/* Delete Current note */
		case 'c':
			currP = deleteCurrentInteractive(currP, head);
			break;

		default:
			printf("Invalid Entry\n");
			break;
		}
	}
}

/* Handles SIGINT by killing run loop */
void sigintHandler(int sig) {
	printf("\nSIGINT (%d) received, press any key to quit.\n", sig);
	keepRunning = 0;
}

/* Runs Terminote in interactive mode */
void runInteractive() {

	node *head, *currP;
	create_list(&head, &currP);

	loadList(currP);

	signal(SIGINT, sigintHandler);

	uiLoop(currP, head);

	saveList(head);
	destroy(currP);
}

/* Runs Terminote in non interactive mode */
void runNonInteractive(Options *options, int argc, char **argv) {

	/* If there are no arguments we'll go ahead and add the data to the list */
	if (argc <= 1) {
		/* Set up the list */
		node *head, *currP;
		create_list(&head, &currP);
		loadList(head);

		/* Read data from the pipe */
		if (getInputPipe(inputBuffer, MAX_MESSAGE_SIZE) >= MAX_MESSAGE_SIZE)
			fprintf(stderr, "Input too large, truncated\n");

		/* If there is only a newline in the buffer then don't add anything */
		if (strlen(inputBuffer) == 1 && (int) inputBuffer[0] == 10) {
			fprintf(stderr, "No Input\n");
		} else {
			/* Otherwise append to list and save */
			append(currP, inputBuffer);
			saveList(head);
		}

	} else {
		/* If we get here then there are command line arguments */
		/* Parse the arguments and make sure they are valid */
		parseOptions(options, argc, argv);

		/* Set up the list */
		node *head, *currP;
		create_list(&head, &currP);
		loadList(head);

		/* Execute options and quit */
		executeOptions(options, currP, head);
		exit(0);
	}

}

/* Initialize options struct */
void initOptions(Options *opts) {
	opts->pop = 0;
	opts->popN = 0;

	opts->delN = 0;
	opts->delA = 0;

	opts->printN = 0;
	opts->printA = 0;

	opts->searchNotes = 0;
	strncpy(opts->searchTerm, "\0", MAX_MESSAGE_SIZE-1);

	opts->outputToFile = 0;
	opts->append = 0;
	opts->usage = 0;
	opts->popA = 0;
}

/* Print options for debugging */
void printOpts(Options *opts) {
	printf("\npopNote: %d"
			"\npopN: %d"
			"\ndelN: %d"
			"\ndeleteAll: %d"
			"\nprintN: %d"
			"\nprintAll: %d"
			"\nsearchNotes: %d"
			"\nsearchTerm: %s\n", opts->pop, opts->popN, opts->delN, opts->delA,
			opts->printN, opts->printA, opts->searchNotes, opts->searchTerm);
}

/* Parse command line options */
void parseOptions(Options *options, int argc, char **argv) {
	char opt;
	initOptions(options);

	char *nArg, *dArg, *pArg, *fArg, *oArg;
	nArg = dArg = pArg = fArg = oArg = NULL;

	while ((opt = getopt(argc, argv, "vhPFN:D:Rp:lf:a:o:")) != -1) {
		switch (opt) {

		case 'v':
			options->version = 1;
			break;

		case 'h':
			options->usage = 1;
			break;

			/* Pop Note ( note only )*/
		case 'P':
			options->pop = 1;
			break;

			/* Pop note ( with time/path info ) */
		case 'F':
			options->popA = 1;
			break;

			/* Pop n */
		case 'N':
			nArg = optarg;
			break;

			/* Delete n */
		case 'D':
			dArg = optarg;
			break;

			/* Delete all notes */
		case 'R':
			options->delA = 1;
			break;

			/* Print n */
		case 'p':
			pArg = optarg;
			break;

			/* Print all notes */
		case 'l':
			options->printA = 1;
			break;

			/* Search for notes containing sub string */
		case 'f':
			options->searchNotes = 1;
			strncpy(options->searchTerm, optarg, MAX_MESSAGE_SIZE-1);
			break;

			/* Append note to list */
		case 'a':
			options->append = 1;
			options->appendStr = optarg;
			break;

		case 'o':
			options->outputToFile = 1;
			options->outFile = optarg;
			break;

		case '?':
			printf("?\n");
			exit(1);
			break;

		default:
			fprintf(stderr,
					"When strange things happen, are you going round the twist? Aborting.\n");
			abort();
			break;
		}
	}

	/* Ensures -N option is an integer */
	if (nArg) {
		if (!isInteger(nArg)) {
			fprintf(stderr, "Error: -N requires an integer\n");
			exit(1);
		} else {
			sscanf(nArg, "%d", &options->popN);
		}
	}

	/* Ensures -D option is an integer */
	if (dArg) {
		if (!isInteger(dArg)) {
			fprintf(stderr, "Error: -D requires an integer\n");
			exit(1);
		} else {
			sscanf(dArg, "%d", &options->delN);
		}
	}
	/* Ensures -p option is an integer */
	if (pArg) {
		if (!isInteger(pArg)) {
			fprintf(stderr, "Error: -p requires an integer\n");
			exit(1);
		} else {
			sscanf(pArg, "%d", &options->printN);
		}
	}

	validateOptions(options);
}

/* Ensures options make sense */
void validateOptions(Options *opts) {
	int optArr[OPT_NUM];
	/* The enum isn't really necessary, just makes it easier to figure out what's going on */
	enum {
		POP, POPN, DELN, DELA, PRINTN, PRINTA, SEARCH, APPEND, VERS, POPA
	};
	optArr[POP] = opts->pop;
	optArr[POPN] = opts->popN;
	optArr[DELN] = opts->delN;
	optArr[DELA] = opts->delA;
	optArr[PRINTN] = opts->printN;
	optArr[PRINTA] = opts->printA;
	optArr[SEARCH] = opts->searchNotes;
	optArr[APPEND] = opts->append;
	optArr[VERS] = opts->version;
	optArr[POPA] = opts->popA;

	/* Only one of the options in optArr at a time make sense, so this just checks if there are more then one arguments.
	 * It should probably be simplified somehow... */
	for (int i = 0; i < OPT_NUM; ++i) {
		if (optArr[i])
			for (int j = i + 1; j < OPT_NUM; ++j) {
				if (optArr[j] && j != OPT_NUM) {
					fprintf(stderr, "Too many arguments.\n");
					exit(1);
				}
			}
	}
}

/* Executes options then destroys the list */
void executeOptions(Options *opts, node *currP, node *head) {
	FILE *outStream = NULL;

	outStream = stdout;

	if (opts->outputToFile)
		outStream = fopen(opts->outFile, "w");

	if (opts->pop) {
		popNote(outStream, currP, head);
	} else if (opts->popN) {
		popN(outStream, currP, head, opts->popN);
	} else if (opts->delA) {
		deleteA(currP, head);
	} else if (opts->delN) {
		deleteN(currP, head, opts->delN);
	} else if (opts->printN) {
		printN(outStream, currP, head, opts->printN);
	} else if (opts->printA) {
		printAll(outStream, currP, head);
	} else if (opts->searchNotes) {
		printAllWithSubString(currP, opts->searchTerm);
	} else if (opts->append) {
		append(currP, opts->appendStr);
		saveList(head);
	} else if (opts->version) {
		printf("%.1f\n", VERSION_NUM);
	} else if (opts->usage) {
		printUsage(outStream);
	} else if (opts->popA) {
		popNoteInfo(outStream, currP, head);
	}

	fclose(outStream);
	destroy(head);
}

