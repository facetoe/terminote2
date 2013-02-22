/*
 * terminote.c
 *
 *  Created on: Feb 21, 2013
 *      Author: fragmachine
 */

#include "linkedList.h"
#include <fcntl.h>

int main(void) {


	int pathBuffLen = 200;
	char pathBuff[pathBuffLen];
	char *path;

	for (int i = 0; i < 2000; ++i) {


	if ( getDataPath(pathBuff, pathBuffLen, "terminote") )
		path = pathBuff;
	}

	printf("%s\n", path);

	return 0;
}
