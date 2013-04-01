/*
 * dynamicArray.h
 *
 *  Created on: Apr 1, 2013
 *      Author: fragmachine
 */

#ifndef DYNAMICARRAY_H_
#define DYNAMICARRAY_H_

#include "defines.h"

typedef struct dynamicArray {
	int maxSize;
	int currSize;
	char *contents;
}dArr;

/* Initialize the array */
bool dArr_init(dArr **arr);

/* Print the contents */
void dArr_print(dArr *arr);

/* Check if full */
bool dArr_isFull(dArr *arr);

/* Grow the array as necessary */
bool dArr_grow(dArr *arr);

/* Add an item to the array */
void dArr_add(dArr *arr, char ch);

/* Destroy the array freeing all memory */
void dArr_destroy(dArr **arr);

#endif /* DYNAMICARRAY_H_ */
