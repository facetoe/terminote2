/*
 * helperFunctions.h
 *
 *  Created on: Feb 22, 2013
 *      Author: fragmachine
 */

#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

/* Returns a pointer to a string containing the current time */
char *current_time();


/* Strip trailing newline and replace with NULL terminator */
void strip_newline(char *string);


/* Determines if a file exists.
 * Returns true for success, false for failure.
 */
int file_exists(char *filename);


/* Attempts to get the users home directory. Returns NULL on failure or
 * a pointer to the directory string on success.
 *
 * Note: Valgrind reports this function as leaking memory, however
 * this - http://stackoverflow.com/questions/12919653/valgrind-reports-getpwuid-leaks-in-c-with-ubuntu
 * answer on stack overflow says that it's not a "real" leak... So there isn't much I can do.
 * I tested it in a loop a few thousand times and the amount of memory lost doesn't increase.
 */
char *getHomDir();


/* Gets the path of users home directory and concatenates it into the buffer */
/* Returns true on success or false if the buffer wasn't big enough, or other error */
bool getDataPath(char buffer[], int buffLen, char *fileName);

/* Get input from stdin. Only reads up to buffer_len.
 * Returns 0 if input overflows buffer, otherwise
 *  returns the number of characters read         */
int getInput(char buffer[], int bufferLen);


#endif /* HELPERFUNCTIONS_H_ */
