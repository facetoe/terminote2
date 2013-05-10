/*
 * line.h
 *
 *  Created on: 10/05/2013
 *      Author: facetoe
 */

#ifndef LINE_H_
#define LINE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structures.h"

/* Allocates memory for a new LINE node and sets default values */
LINE *line_getLine();

/* Returns the requested lineNode or NULL if it doesn't exist (needs to be cleaned up) */
LINE *line_getLineNode( MESSAGE *msg, int nodeNum );

/* Inserts a line after nodeNum */
void line_insertAfter( MESSAGE *msg, int nodeNum, char *str );

/* Inserts a line before nodeNum */
void line_insertBefore( MESSAGE *msg, int nodeNum, char *str );

/* Deletes nodeNum */
void line_deleteNode( MESSAGE *msg, int nodeNum );

#endif /* LINE_H_ */
