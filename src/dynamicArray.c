/*
 ============================================================================
 Name        : Darr.c
 Author      : William Coe
 Version     :
 Copyright   :
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "dynamicArray.h"

/* Initialize the array */
bool dArr_init(dArr **arr) {
	dArr *tmp = NULL;
	tmp = malloc( sizeof(dArr) );
	tmp->contents = (char *) malloc(sizeof(char) * DYNA_SIZE);
	if( tmp->contents ) {
		tmp->maxSize = DYNA_SIZE;
		tmp->currSize = 0;
		*arr = tmp;
		return true;
	} else {
		fprintf(stderr, "Malloc failed in dArr_init()\n");
		return false;
	}
}

/* Print the contents */
void dArr_print(dArr *arr) {
	for (int i = 0; i < arr->currSize; ++i) {
		printf("%c", arr->contents[i]);
	}
	printf("\n");
}

/* Check if full */
bool dArr_isFull(dArr *arr) {
	return arr->currSize >= arr->maxSize;
}

/* Grow the array as necessary */
bool dArr_grow(dArr *arr) {
	char *tmp = NULL;
	arr->maxSize *= 2;
	tmp = (char *) realloc(arr->contents, sizeof(char) * arr->maxSize);
	if( tmp ) {
		arr->contents = tmp;
		return true;
	} else {
		fprintf(stderr, "Realloc failed in dArr_grow()\n");
		return false;
	}
}

/* Add an item to the array */
void dArr_add(dArr *arr, char ch) {
	if( !dArr_isFull(arr) ) {
		arr->contents[arr->currSize] = ch;
		arr->currSize++;
	} else if ( dArr_grow(arr) ) {
		arr->contents[arr->currSize] = ch;
		arr->currSize++;
	} else {
		fprintf(stderr, "Failed to grow array, aborting\n");
		abort();
	}
}

/* Destroy the array freeing all memory */
void dArr_destroy(dArr **arr) {
	dArr *tmp = *arr;
	free(tmp->contents);
	free(tmp);
	tmp = NULL;
	*arr = tmp;
}

