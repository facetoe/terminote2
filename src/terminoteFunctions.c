#include "helperFunctions.h"
#include "linkedList.h"
#include "terminoteFunctions.h"
#include "defines.h"

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
void printUsage()
{
	printf("Fill this in\n");
}

/* Prints current note */
void printCurrent(node *currP) {

	if (currP == NULL || currP->note_num == 0)
		fprintf(stdout, "Nothing to print.\n");
	else {
		printf("NoteNum: %d"
				"\nPath: %s"
				"\nTime: %s"
				"\nMessage: %s\n\n", currP->note_num, currP->path, currP->time,
				currP->message);
	}
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
node *appendNote(char inputBuffer[], int buffSize, node *currP, node *head) {
	printf("Enter Note\n> ");
	FLUSH_STDIN(Junk);
	getInput(inputBuffer, buffSize);
	FLUSH_STDIN(Junk);
	currP = head;
	return append(currP, inputBuffer);
}

/* Asks user if they want to delete all notes. If so, deletes them */
void deleteAllNotes(node *currP, node *head) {
	FLUSH_STDIN(Junk);
	if (promtUserChoice("Delete all notes [y/n]?")) {
		currP = head;
		deleteAll(currP);
	} else {
		printf("Nothing deleted.\n");
	}
	/* Reset to head otherwise we are sitting in a NULL node */
	currP = head;

	FLUSH_STDIN(Junk);
}

/* Deletes current currP. Returns pointer to head. This version asks the user if they want to delete and deletes if the answer is y */
node *deleteCurrentInteractive(node *currP, node *head) {
	FLUSH_STDIN(Junk);
	if (currP->note_num == 0 && currP->next == NULL ) {
		printf("Nothing to delete.\n");
		return head;
	}

	if (promtUserChoice("Delete current note?")) {
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
	FLUSH_STDIN(Junk);
	if (currP->note_num == 0 && currP->next == NULL ) {
		return;
	}
	deleteNode(currP, head, currP->note_num);
}

/* Pops a note off the list. In other words, prints last note then deletes it. If there are no notes to print, send an error to stderr. */
void popNote(node *currP, node *head, char *path)
{
	currP = lastNode(currP, head);

	if ( currP )
	{
		printCurrent(currP);
		deleteCurrent(currP, head);
		saveList(head, path);
	} else {
		fprintf(stderr, "No notes to pop\n");
	}

}

/* Asks user for search term then prints all notes that contain it. */
void printAllWithSubString(node *currP, node *head) {
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

/* Enters UI loop */
void uiLoop(node *currP, node *head) {
	char opt;
	int lastNoteNum = 0;
	menuMessage();

	while (keepRunning) {

		/* Loop until we get something other then a newline */
		while( ( opt = getchar() ) == '\n' );

		/* Prevents the switch being excecuted one last time on SIGINT */
		if ( keepRunning == 0 )
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
			currP = appendNote(inputBuffer, MAX_MESSAGE_SIZE, currP, head);
			break;

			/* Print */
		case 'e':
			printList(currP);
			break;

			/* Delete all notes */
		case 'g':
			deleteAllNotes(currP, head);
			currP = head;
			break;

			/* Find and print all notes containing search term */
		case 'f':
			printAllWithSubString(currP, head);
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
void sigintHandler(int sig)
{
	printf("\nSIGINT received, press any key to quit.\n");
	keepRunning = 0;
}

/* Runs Terminote in interactive mode */
void runInteractive()
{
	node *head, *currP;
	create_list(&head, &currP);

	char *path;
	if ( getDataPath(pathBuffer, MAX_PATH_SIZE, "terminote.data") )
		path = pathBuffer;
	else
	{
		fprintf(stderr, "Error retrieving path\nAborting\n");
		exit(1);
	}

	loadList(currP, path);

	signal(SIGINT, sigintHandler);

	uiLoop(currP, head);

	currP=head;
	saveList(head, path);
	destroy(currP);
}

/* Runs Terminote in non interactive mode */
void runNonInteractive(Options *options, int argc, char **argv)
{
	char *path;
	if ( getDataPath(pathBuffer, MAX_PATH_SIZE, "terminote.data") )
		path = pathBuffer;
	else
	{
		fprintf(stderr, "Error retrieving path\nAborting\n");
		exit(1);
	}

	/* Set up the list */
	node *head, *currP;
	create_list(&head, &currP);
	loadList(head, path);


	 /* If there are no arguments we'll go ahead and add the data to the list */
	if (argc <= 1) {
		/* Read data from the pipe */
		getInputPipe(inputBuffer, MAX_MESSAGE_SIZE);

		/* If there is only a newline in the buffer then don't add anything */
		if (strlen(inputBuffer) == 1 && (int) inputBuffer[0] == 10) {
			fprintf(stderr, "No Input\n");
		} else {
			/* Otherwise append to list and save */
			append(currP, inputBuffer);
			saveList(head, path);
		}

	} else {
		Options options;
		parseOptions(&options, argc, argv);
		printOpts(&options);
	}


}

/* Initialize options struct */
void initOptions(Options *opts)
{
	opts-> popNote = 0;
	opts-> popN = 0;

	opts-> delN = 0;
	opts-> deleteAll = 0;

	opts-> printN = 0;
	opts-> printAll = 0;

	opts-> searchNotes = 0;
	opts->searchTerm = "";
}

/* Print options for debugging */
void printOpts(Options *opts)
{
	printf(
			"\npopNote: %d"
			"\npopN: %d"
			"\ndelN: %d"
			"\ndeleteAll: %d"
			"\nprintN: %d"
			"\nprintAll: %d"
			"\nsearchNotes: %d"
			"\nsearchTerm: %s\n",
			opts->popNote,
			opts->popN,
			opts->delN,
			opts->deleteAll,
			opts->printN,
			opts->printAll,
			opts->searchNotes,
			opts->searchTerm
			);
}

/* Parse command line options */
void parseOptions(Options *options, int argc, char **argv)
{
	char opt;
	initOptions(options);

	char *nArg, *dArg, *sArg, *fArg;
	nArg=dArg=sArg=fArg=NULL;

	while ( (opt = getopt(argc, argv, "hpn:d:rs:cf:") ) != -1)
	{
		switch (opt) {

			case 'h':
				printf("Print help\n");
				exit(0);
				break;

			/* Pop Note */
			case 'p':
				options->popNote = 1;
				break;

			/* Pop n */
			case 'n':
				nArg = optarg;
				break;

			/* Delete n */
			case 'd':
				dArg = optarg;
				break;

			/* Delete all notes */
			case 'r':
				options->deleteAll = 1;
				break;

			/* Print n */
			case 's':
				sArg = optarg;
				options->searchNotes = 1;
				break;

			/* Print all notes */
			case 'c':
				options->printAll = 1;
				break;

			case 'f':
				options->searchNotes = 1;
				options->searchTerm = optarg;
				break;

			case '?':
				if ( optopt == 'n')
				{
					printf("Error: -n requires a an argument (integer)\n");
					exit(1);
				} else if ( optopt == 'd' )
				{
					printf("Error: -d requires a an argument (integer)\n");
					exit(1);
				} else if ( optopt == 's')
				{
					printf("Error: -s requires an argument (integer)\n");
					exit(1);
				} else if ( optopt == 'f' )
				{
					printf("Error: -f requires an argument (string)\n");
					exit(1);
				}

				break;

			default:
				printf("Strange thing happened, aborting\n");
				abort();
				break;
		}
	}

	/* Ensures -n option is an integer */
	if ( nArg )
	{
		if ( !isInteger(nArg) )
		{
			fprintf(stderr, "Error: -n requires an integer\n");
			exit(1);
		} else {
			sscanf(nArg, "%d", &options->popN);
		}
	}

	/* Ensures -d option is an integer */
	if ( dArg )
	{
		if ( !isInteger(dArg) )
		{
			fprintf(stderr, "Error: -d requires an integer\n");
			exit(1);
		} else {
			sscanf(dArg, "%d", &options->delN);
		}
	}
	/* Ensures -s option is an integer */
	if ( sArg )
	{
		if ( !isInteger(sArg) )
		{
			fprintf(stderr, "Error: -s requires an integer\n");
			exit(1);
		} else {
			sscanf(sArg, "%d", &options->printN);
		}
	}


}

