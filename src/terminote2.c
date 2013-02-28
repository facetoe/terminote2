/*
 * terminote.c
 *
 *  Created on: Feb 21, 2013
 *      Author: fragmachine
 */

#include "linkedList.h"
#include "helperFunctions.h"
#include "terminoteFunctions.h"
#include "defines.h"

/* Global variable for run loop */
volatile int keepRunning = 1;

int main(int argc, char **argv) {
	Options options;

	if (isatty(STDIN_FILENO) && argc == 1) {
		runInteractive();
	} else {
		runNonInteractive(&options, argc, argv);
	}

	return 0;
}

