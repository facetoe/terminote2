/*
 * linkedList.h
 *
 *  Created on: Feb 22, 2013
 *      Author: fragmachine
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAX_TITLE_SIZE 200
#define MAX_MESSAGE_SIZE 1000
#define TIME_SIZE 30
#define MAX_PATH_SIZE 200
#define DEBUG 1

struct Node {
	int note_num;
	char title[MAX_TITLE_SIZE];
	char message[MAX_MESSAGE_SIZE];
	char time[TIME_SIZE];
	char path[MAX_PATH_SIZE];

	struct Node *next;
};

typedef struct Node node;

void init(node *head, node *currP);
void create_list(node **head, node **currP);
void append(node *currP, char *noteText);
node *next(node *head, node *currP);
node *previous(node *head, node *currP);
int length(node *currP);
void orderList(node *currP);
void deleteNote(node *currP, int noteNum);
void deleteAll(node *head);
void destroy(node *head);
void printList(node *currP);
void writeBinary(FILE *fp, node *head);
void readBinary(FILE *fp, node *head);
bool loadList(node *head, char *path);
bool saveList(node *head, char *path);

#endif /* LINKEDLIST_H_ */
