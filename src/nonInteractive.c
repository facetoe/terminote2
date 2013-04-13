/*
 * nonInteractive.c
 *
 *  Created on: 09/04/2013
 *      Author: facetoe
 */

#include "nonInteractive.h"
#include "linkedList.h"

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

	} else if (opts->popN) {
		printf("popN");
	} else if (opts->delA) {
		printf("delA");
	} else if (opts->delN) {
		printf("delN");
	} else if (opts->printN) {
		printf("printN");
	} else if (opts->printA) {
		printf("printA");
	} else if (opts->searchNotes) {
		printf("search");
	} else if (opts->append) {
		printf("append");
	} else if (opts->version) {
		printf("%.1f\n", VERSION_NUM);
	} else if (opts->usage) {
		printf("version");
	} else if (opts->popA) {
		printf("popA");
	}

	fclose(outStream);
	list_destroy(currP);
}

void pop_nonInteractive(FILE *outStream, listNode *currP, char *args, int noteNum) {

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

