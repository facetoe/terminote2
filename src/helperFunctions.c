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

#include "helperFunctions.h"

/* Returns a pointer to a string containing the current time */
char *current_time() {
    time_t t;
    time( &t );
    return ctime( &t );
}

/* Returns current Unix time */
int currentUnixTime() {
    return ( int ) time( NULL );
}

/* Strip trailing newline and replace with NULL terminator */
void strip_newline( char *string ) {
    int len = strlen( string ) - 1;
    string[len] = '\0';
}

/* Determines if a file exists.
 * Returns true for success, false for failure.
 */
int file_exists( char *filename ) {
    struct stat buffer;
    return ( stat( filename, &buffer ) == 0 );
}

/* Searches string for substring.
 * Returns a true if found, false otherwise.
 */bool findSubstring( char *haystack, char *needle ) {
    char *pntr;

    if ( ( pntr = strstr( haystack, needle ) ) == NULL ) {
        return false;
    } else {
        return true;
    }
}

/* Places lowercase version of str in newString */
void stringToLower( char str[], char newString[], int len ) {
    for ( int i = 0; i <= len; ++i ) {
        newString[i] = tolower( str[i] );
    }
    newString[len] = '\0';
}

/* Determines if str is an integer *.
 * Returns true if is false if it's not. */bool isInteger( char *str ) {
    for ( char *sPntr = str; *sPntr; sPntr++ ) {
        if ( !isdigit(*sPntr) )
            return false;
    }
    return true;
}

/* Gets the path of users home directory and concatenates it with the data filename*/
void getDataPath() {
    char *homeDir;
    if ( ( homeDir = getenv( "HOME" ) ) ) {
        int buffSize = strlen( homeDir ) + strlen( dataFile ) + 1;
        char *pathBuffer = malloc( buffSize * sizeof(char) );
        snprintf( pathBuffer, buffSize, "%s%s", homeDir, dataFile );
        path = pathBuffer;
    }
}

