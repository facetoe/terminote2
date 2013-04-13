/*
 * nonInteractive.c
 *
 *  Created on: 09/04/2013
 *      Author: facetoe
 */

#include "linkedList.h"
#include "helperFunctions.h"
#include "nonInteractive.h"


/* Initialize options struct */
void initOptions(Options *opts) {
	opts->pop = 0;
	opts->popN = 0;

	opts->delN = 0;
	opts->delA = 0;

	opts->printN = 0;
	opts->printA = 0;

	opts->version = 0;

	opts->searchNotes = 0;
	strncpy(opts->searchTerm, "\0", MAX_SEARCHTERM_SIZE-1);

	opts->outputToFile = 0;
	opts->append = 0;
	opts->usage = 0;
	opts->popA = 0;
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
			strncpy(options->searchTerm, optarg, MAX_SEARCHTERM_SIZE-1);
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

	//validateOptions(options);
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
void executeOptions(Options *opts, listNode *currP) {
	FILE *outStream = NULL;

	outStream = stdout;

	if (opts->outputToFile)
		outStream = fopen(opts->outFile, "w");

	if (opts->pop) {
		nonInteractive_pop(stdout, currP, "m", list_length(currP));
	} else if (opts->popN) {
		nonInteractive_pop(stdout, currP, "nptm", opts->popN);
	} else if (opts->delA) {
		list_deleteAll(&currP);
	} else if (opts->delN) {\
		if ( list_length(currP) >=  opts->delN)
			list_deleteNode(currP, opts->delN);
		else
			fprintf(stderr, "Nothing to delete at position: %d\n", opts->delN);

	} else if (opts->printN) {
		listNode *tmp = NULL;
		if ( (tmp = list_searchByNoteNum(currP, opts->printN))  )
			list_printMessage(outStream, "nptm", tmp);
		else
			fprintf(stderr, "Nothing to print at position: %d\n", opts->printN);

	} else if (opts->printA) {
		list_printAll(outStream, currP);
	} else if (opts->searchNotes) {
		nonInteractive_search(outStream, currP, opts->searchTerm);
	} else if (opts->append) {
		list_appendMessage(currP, opts->appendStr);
	} else if (opts->version) {
		printf("%.1f\n", VERSION_NUM);
	} else if (opts->usage) {
		printUsage(stdout);
	} else if (opts->popA) {
		nonInteractive_pop(stdout, currP, "nptm", list_length(currP));
	}

	if (opts->outputToFile)
		fclose(outStream);
}

/* Pops noteNum note and prints with args sections then deletes note */
void nonInteractive_pop(FILE *outStream, listNode *currP, char *args, int noteNum) {

	if ( list_length(currP) == 0 || noteNum == 0 ) {
		fprintf(stderr, "Nothing to print\n");
		return;
	}

	listNode *tmp = NULL;
	if ( (tmp = list_searchByNoteNum(currP, noteNum) ) == NULL) {
		fprintf(outStream, "Nothing found\n");
	} else {
		list_printMessage(outStream, args, tmp);
		list_deleteNode(currP, noteNum);
	}

}

/* Searches through messages printing them if they contain searchTerm */
void nonInteractive_search(FILE *outStream, listNode *ln, char *searchTerm) {
	for (; ln ; ln = ln->next) {
		if( list_messageHasSubstring(ln, searchTerm) )
			list_printMessage(outStream, "nptm", ln);
	}
}

/* Prints usage */
void printUsage(FILE *outStream) {
	fprintf(outStream,
			"Terminote version %.1f - a command line note tool.\n\n"
			"Terminote reacts differently depending on how you call it.\n"
			"If you pipe data to it, or supply a command line argument, Terminote runs in non-interactive mode.\n"
			"If you call terminote with no arguments from the shell, terminote will enter interactive mode.\n\n"
			"ARGUMENTS: Arguments in capitals are destructive, lower case leaves notes intact.\n"
			" -h: Print this message and quit.\n"
			" -P: Prints the last note only, no path or number, then deletes it.\n"
			" -F: Prints the last note with full info (path/time/num) then deletes it.\n"
			" -N: Prints the note at the supplied note number and deletes it, if it exists. Requires an integer argument. \n"
			" -D: Deletes the note at supplied note number, if it exists. Requires an integer argument.\n"
			" -R: Deletes all notes.\n"
			" -p: Prints the note at the supplied note number, leaving it intact. Requires an integer argument.\n"
			" -l: Prints all the notes leaving them intact.\n"
			" -f: Searches for notes containing supplied sub string and prints them, leaving them intact. Requires a string argument.\n"
			" -a: Appends a note to the list. Requires a string argument.\n\n"
			"CONTACT:\n"
			" Please email any bugs, requests or hate mail to facetoe@ymail.com, or file a bug at https://github.com/facetoe/terminote2\n",
			VERSION_NUM);
}


