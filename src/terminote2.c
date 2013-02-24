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
#include <getopt.h>

volatile int keepRunning = 1;


int main(int argc, char **argv) {

	Options options;

	if (isatty(STDIN_FILENO)) {
		runInteractive();
	} else {

		runNonInteractive(&options, argc, argv);
	}

	return 0;
}

