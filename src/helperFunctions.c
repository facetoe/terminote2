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

	if (psStruct == NULL ) {
		perror("Error retrieving home directory\n");
		return NULL ;
	}

	char *dirP = psStruct->pw_dir;

	return dirP;
}

/* Gets the path of users home directory and concatenates it into the buffer 		*/
/* Returns true on success or false if the buffer wasn't big enough, or other error */
bool getDataPath( char buffer[], int buffLen, char *fileName ) {
	int charsRead;
	char *homePath = getHomDir();
	if (homePath) {
		charsRead = snprintf(buffer, buffLen, "%s/%s", homePath, fileName);
		return charsRead < buffLen;

	} else {
		return false;
	}
}

/* Get input from stdin. Reads until newline or bufferLen.
 * Returns 0 if input overflows buffer, otherwise
 *  returns the number of characters read. */
int getInput(char buffer[], int bufferLen) {
	char ch;
	int charsRead = 0;

	while ((ch = getchar()) != '\n') {
		/* If input overflows buffer */
		if (charsRead >= bufferLen) {
			/* Terminate */
			buffer[charsRead - 1] = '\0';

			/* And return error */
			return charsRead;
		}
		buffer[charsRead] = ch;
		charsRead++;
	}
	buffer[charsRead] = '\0';
	return charsRead;
}

/* Get input from stdin. Reads up to bufferLen or EOF.
 * If more chars are entered then bufferLen returns 0,
 * otherwise returns the number of characters read. */
int getInputPipe(char buffer[], int bufferLen) {
	char ch;
	int charsRead = 0;

	while ((ch = getchar()) != EOF) {
		/* If input overflows buffer */
		if (charsRead >= bufferLen) {
			/* Terminate */
			buffer[charsRead - 1] = '\0';

			/* And return error */
			return charsRead;
		}
		buffer[charsRead] = ch;
		charsRead++;
	}
	buffer[charsRead] = '\0';
	return charsRead;
}

/* Searches string for substring.
 * Returns a pointer to the substring if found,
 * otherwise returns NULL.
 */
char *findSubstring(char *haystack, char *needle) {
	char *pntr;

	if ((pntr = strstr(haystack, needle)) == NULL ) {
		return NULL ;

	} else {

		return pntr;
	}
}

/* Places lowercase version of str in newString */
void stringToLower(char str[], char newString[], int len) {
	for (int i = 0; i <= len; ++i) {
		newString[i] = tolower(str[i]);
	}
	newString[len] = '\0';
}

/* Determines if str is an integer *.
 * Returns true if is false if it's not. */
bool isInteger(char *str) {
	for (unsigned int i = 0; i < strlen(str); ++i)
		if (str[i] == '.' || !isdigit(str[i]))
			return false;

	return true;
}

