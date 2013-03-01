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

#ifndef DEFINES_H_
#define DEFINES_H_

#define MAX_CWDBUFF_SIZE 300
#define MAX_MESSAGE_SIZE 1000
#define TIME_SIZE 30
#define MAX_PATH_SIZE 200
#define OPT_NUM 9
#define DEBUG 0
#define VERSION_NUM 1.0

#define DATA_FILE ".terminote.data"

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
