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

#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#include <string.h> //strstr
#include <ctype.h> // tolower

/* Returns a pointer to a string containing the current time */
char *current_time();

/* Strip trailing newline and replace with NULL terminator */
void strip_newline( char *string );

/* Determines if a file exists.
 * Returns true for success, false for failure.
 */
int file_exists( char *filename );

/* Gets the path of users home directory and concatenates it with filename into the buffer 	*/
/* Returns true on success or false if the buffer wasn't big enough, or other error */
bool getDataPath(
        char buffer[], int buffLen, char *fileName );

/* Get input from stdin. Reads until newline or bufferLen.
 * Returns 0 if input overflows buffer, otherwise
 *  returns the number of characters read. */
int getInput( char buffer[], int bufferLen );

/* Get input from stdin. Reads up to bufferLen or EOF.
 * If more chars are entered then bufferLen returns 0,
 * otherwise returns the number of characters read. */
int getInputPipe( char buffer[], int bufferLen );

/* Searches string for substring.
 * Returns a pointer to the substring if found,
 * otherwise returns NULL.
 */bool findSubstring( char *haystack, char *needle );

/* Places lowercase version of str in newString */
void stringToLower( char str[], char newString[], int len );

/* Determines if str is an integer *.
 * Returns true if is false if it's not. */bool isInteger( char *str );

#endif /* HELPERFUNCTIONS_H_ */
