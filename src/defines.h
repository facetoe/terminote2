/*
 * defines.h
 *
 *  Created on: Feb 23, 2013
 *      Author: fragmachine
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#define MAX_TITLE_SIZE 200
#define MAX_MESSAGE_SIZE 1000
#define TIME_SIZE 30
#define MAX_PATH_SIZE 200
#define DEBUG 1

/* Flushes junk characters in the input buffer */
#define FLUSH_STDIN(x) {if(x[strlen(x)-1]!='\n'){do fgets(Junk,16,stdin);while(Junk[strlen(Junk)-1]!='\n');}else x[strlen(x)-1]='\0';}
char Junk[16];

char inputBuffer[MAX_MESSAGE_SIZE];
char pathBuffer[MAX_PATH_SIZE];

/* Condition for run loop */
volatile int keepRunning;

#endif /* DEFINES_H_ */
