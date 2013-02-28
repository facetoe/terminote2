/*
 * defines.h
 *
 *  Created on: Feb 23, 2013
 *      Author: fragmachine
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#define MAX_CWDBUFF_SIZE 300
#define MAX_MESSAGE_SIZE 1000
#define TIME_SIZE 30
#define MAX_PATH_SIZE 200
#define OPT_NUM 9
#define DEBUG 0
#define VERSION_NUM 1.0

/* Flushes junk characters in the input buffer */
#define FLUSH_STDIN(x) {if(x[strlen(x)-1]!='\n'){do fgets(Junk,16,stdin);while(Junk[strlen(Junk)-1]!='\n');}else x[strlen(x)-1]='\0';}
char Junk[16];

/* Buffers for input */
char inputBuffer[MAX_MESSAGE_SIZE];
char pathBuffer[MAX_PATH_SIZE];
char cwdBuffer[MAX_CWDBUFF_SIZE];

/* Location of data file */
char *path;

/* Condition for run loop */
extern volatile int keepRunning;

#endif /* DEFINES_H_ */
