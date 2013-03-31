SOURCES := helperFunctions.c testMain.c
HEADERS := defines.h helperFunctions.h 
BINARY := terminote-dev
CFLAGS := -std=c99 -Wall -pedantic -Wextra

$(BINARY): $(SOURCES) $(HEADERS) Makefile
	gcc $(CFLAGS) -o $(BINARY) $(SOURCES)

clean:
	rm $(BINARY)
