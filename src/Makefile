SOURCES := helperFunctions.c linkedList.c line.c options.c nonInteractive.c terminote.c 
HEADERS := defines.h helperFunctions.h linkedList.h options.h line.h structures.h nonInteractive.h
BINARY := terminote-dev
CFLAGS := -g -std=c99 -Wall -pedantic -Wextra
LIBS := -lncurses -lmenu

$(BINARY): $(SOURCES) $(HEADERS) Makefile
	gcc $(CFLAGS) -o $(BINARY) $(SOURCES) $(LIBS)

clean:
	rm $(BINARY)
