SOURCES := linkedList.c helperFunctions.c terminoteFunctions.c terminote2.c
HEADERS := defines.h helperFunctions.h linkedList.h terminoteFunctions.h
BINARY := terminote
CFLAGS := -std=c99 -Wall -pedantic -Wextra

$(BINARY): $(SOURCES) $(HEADERS) Makefile
	gcc $(CFLAGS) -o $(BINARY) $(SOURCES)

