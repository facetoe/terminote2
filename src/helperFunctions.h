/*
 * helperFunctions.h
 *
 *  Created on: Feb 22, 2013
 *      Author: fragmachine
 */

#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

char *current_time();
void strip_newline(char *string);
int file_exists(char *filename);
char *getHomDir();
bool getDataPath(char buffer[], int buffLen, char *fileName);

#endif /* HELPERFUNCTIONS_H_ */
