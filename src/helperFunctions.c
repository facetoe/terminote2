/*
 * helperFunctions.c
 *
 *  Created on: Feb 21, 2013
 *      Author: fragmachine
 */


#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include "helperFunctions.h"
#include "linkedList.h"

/* Returns a pointer to a string containing the current time */
char *current_time() {
	time_t t;
	time(&t);
	return ctime(&t);
}

/* Strip trailing newline and replace with NULL terminator */
void strip_newline(char *string) {
	int len = strlen(string) - 1;
	string[len] = '\0';
}

/* Determines if a file exists.
 * Returns true for success, false for failure.
 */
int file_exists(char *filename) {
	struct stat buffer;
	return (stat(filename, &buffer) == 0);
}

/* Attempts to get the users home directory. Returns NULL on failure or
 * a pointer to the directory string on success.
 *
 * Note: Valgrind reports this function as leaking memory, however
 * this - http://stackoverflow.com/questions/12919653/valgrind-reports-getpwuid-leaks-in-c-with-ubuntu
 * answer on stack overflow says that it's not a "real" leak... So there isn't much I can do.
 * I tested it in a loop a few thousand times and the amount of memory lost doesn't increase.
 */
char *getHomDir() {
	struct passwd *psStruct = NULL;
	uid_t uid = getuid();
	psStruct = getpwuid(uid);

	if (psStruct == NULL) {
		perror("Error retrieving home directory\n");
		return NULL;
	}

	char *dirP = psStruct->pw_dir;

	return dirP;
}

/* Gets the path of users home directory and concatenates it into the buffer 		*/
/* Returns true on success or false if the buffer wasn't big enough, or other error */
bool getDataPath(char buffer[], int buffLen, char *fileName) {
	int charsRead;
	char *homePath = getHomDir();
	if (homePath) {
		charsRead = snprintf(buffer, buffLen, "%s/%s", homePath, fileName);
		return charsRead < buffLen;

	} else {
		return false;
	}
}

