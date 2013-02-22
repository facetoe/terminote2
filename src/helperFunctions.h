/*
 * helperFunctions.c
 *
 *  Created on: Feb 21, 2013
 *      Author: fragmachine
 */

#include <time.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>

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

int file_exists (char *filename)
{
  struct stat buffer;
  return (stat (filename, &buffer) == 0);
}

/* Attempts to get the users home directory. Returns NULL on failure or
 * a pointer to the directory string on success.
 */
char *getHomDir()
{
	struct passwd *psStruct = NULL;
	uid_t uid = getuid();
	psStruct = getpwuid(uid);

	if (psStruct == NULL)
	{
		perror("Error retrieving home directory\n");
		return NULL;
	}

	char *dirP = psStruct->pw_dir;

	return dirP;
}

