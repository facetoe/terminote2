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

/* List to hold lines */
struct line {
    int lNum;
    int lSize;
    char *currLine;
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

    char path[MAX_PATH_SIZE];
    char time[MAX_TIME_SIZE];

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

#endif /* STRUCTURES_H_ */
