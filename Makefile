

PROJECT = lircE161
SOURCES = lircE161.c
CC = gcc -g -Wall -Wstrict-prototypes 
#--pedantic-errors # error in include/lirc/lirc_client.h:34
INCLUDE = 
LIB = -llirc_client

default: $(SOURCES)
	$(CC) $(INCLUDE) $(LIB) $(SOURCES) -o $(PROJECT)

clean: 
	rm -f $(PROJECT)
