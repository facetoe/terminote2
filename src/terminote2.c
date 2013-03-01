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

