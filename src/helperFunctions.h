/*
 * helperFunctions.c
 *
 *  Created on: Feb 21, 2013
 *      Author: fragmachine
 */

#include <time.h>

char *current_time()
{
// Returns a pointer to a string containing the current time
time_t t;
time(&t);
return ctime(&t);
}

void strip_newline(char *string)
{
	/* Strip trailing newline and replace with NULL terminator */
	int len = strlen(string) -1;
	string[len] = '\0';
}

