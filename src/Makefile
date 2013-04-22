SOURCES := helperFunctions.c linkedList.c dynamicArray.c nonInteractive.c ui.c testMain.c
HEADERS := defines.h helperFunctions.h linkedList.h dynamicArray.h ui.h nonInteractive.h
BINARY := terminote-dev
CFLAGS := -g -pg -std=c99 -Wall -pedantic -Wextra
LIBS := -lncurses -lmenu

$(BINARY): $(SOURCES) $(HEADERS) Makefile
	gcc $(CFLAGS) -o $(BINARY) $(SOURCES) $(LIBS)

clean:
	rm $(BINARY)
