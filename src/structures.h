/*
 * structures.h
 *
 *  Created on: 10/05/2013
 *      Author: facetoe
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#define MAX_TIME_SIZE 30
#define MAX_PATH_SIZE 200

#include <stdbool.h>

/* List to hold lines */
struct line {
    int lNum;
    int lSize;
    char *text;
    struct line *next;
    struct line *prev;
};

typedef struct line LINE;

/* List to hold parsed messages */
struct message {

    int totalMessages;
    int numLines;
    int messageNum;
    long numChars;
    int unixTime;

    /* Whether we need to save any changes */
    bool hasChanged;

    char path[MAX_PATH_SIZE];
    char time[MAX_TIME_SIZE];

    /* Current line of the message */
    LINE *currentLine;

    /* Top line of the message */
    LINE *first;

    /* Bottom line of the message */
    LINE *last;

    /* Top line of the current page */
    LINE *pageTop;

    /* Bottom line of the current page */
    LINE *pageBot;

    struct message *next;
    struct message *prev;
    struct message *root;

};

typedef struct message MESSAGE;

/* Terminal coordinates and scrollable message variables */
typedef struct {
    int NCOLS;
    int NROWS;
    int cursorRow;
    int cursorCol;
    MESSAGE *currMsg;
} DISPLAY_DATA;


#endif /* STRUCTURES_H_ */
