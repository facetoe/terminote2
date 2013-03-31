/*
 * defines.h
 *
 *  Created on: Mar 31, 2013
 *      Author: fragmachine
 */

#ifndef DEFINES_H_
#define DEFINES_H_

/* Buffers and buffer sizes */
#define MAX_CWDBUFF_SIZE 300
#define MAX_TIME_SIZE 30
#define MAX_PATH_SIZE 200

/* This is the number of options that exclude any other options, not the total of all options. */
#define OPT_NUM 11

#define DEBUG 1
#define VERSION_NUM 1.0

#define DATA_FILE ".terminote.data"

/* Flushes junk characters in the input buffer */
#define FLUSH_STDIN(x) {if(x[strlen(x)-1]!='\n'){do fgets(Junk,16,stdin);while(Junk[strlen(Junk)-1]!='\n');}else x[strlen(x)-1]='\0';}
char Junk[16];

/* Buffers for input */
char pathBuffer[MAX_PATH_SIZE];
char cwdBuffer[MAX_CWDBUFF_SIZE];

/* Location of data file */
char *path;

/* Condition for run loop */
extern volatile int keepRunning;


#endif /* DEFINES_H_ */
