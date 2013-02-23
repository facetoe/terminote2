#include "helperFunctions.h"
#include "linkedList.h"
#include "defines.h"
#include <ctype.h>

void menuMessage(void)
{
printf("Wecome to Terminote interactive:\n"
"(w) to insert new note\n"
"(a) and (d) to navigate through notes\n"
"(f) to print all notes\n"
"(g) to delete all notes\n"
"(m) to print this message\n"
"(q) to quit\n");
}

/* Prints current note */
void printCurrent(node *currP)
{

	if ( currP  == NULL || currP->note_num == 0)
		fprintf(stdout, "Nothing to print.\n");
	else {
		printf("NoteNum: %d"
				"\nPath: %s"
				"\nTime: %s"
				"\nMessage: %s\n\n",
				currP->note_num,
				currP->path,
				currP->time,
				currP->message
				);
	}
}

/* Prompts user for y or n question. */
/* Returns true for y and false for anything else */
bool promtUserChoice(char *prompt)
{
	char choice;
	printf("%s\n> ", prompt);
	scanf("%c", &choice);

	if ( tolower(choice) == 'y' )
		return true;
	else
		return false;
}

/* Prompts user for input. Appends input to list */
void appendNote(char inputBuffer[], int buffSize, node *currP, node *head)
{
	printf("Enter Note\n> ");
	FLUSH_STDIN(Junk);
	getInput(inputBuffer, buffSize);
	currP=head;
	append(currP, inputBuffer);
	FLUSH_STDIN(Junk);
}

/* Asks user if they want to delete all notes. If so, deletes them */
void deleteAllNotes(node *currP, node *head)
{
	FLUSH_STDIN(Junk);
	if ( promtUserChoice("Delete all notes [y/n]?") )
	{
		currP = head;
		deleteAll(currP);
	} else {
		printf("Nothing deleted.\n");
	}
	/* Reset to head otherwise we are sitting in a NULL node */
	currP = head;

	FLUSH_STDIN(Junk);
}

