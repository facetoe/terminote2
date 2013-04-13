SOURCES := helperFunctions.c linkedList.c dynamicArray.c nonInteractive.c testMain.c
HEADERS := defines.h helperFunctions.h linkedList.h dynamicArray.h nonInteractive.h
BINARY := terminote-dev
CFLAGS := -std=c99 -Wall -pedantic -Wextra

$(BINARY): $(SOURCES) $(HEADERS) Makefile
	gcc $(CFLAGS) -o $(BINARY) $(SOURCES)

clean:
	rm $(BINARY)
