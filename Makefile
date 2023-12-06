CC = gcc
CFLAGS =
DEBUG_FLAGS = -DDEBUG -g
SOURCES = parser.c symtable.c expression.c stack.c exp_semantic.c error.c scanner.c gen.c buildin.c str.c token.c main.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all debug clean

all: main

debug: CFLAGS += $(DEBUG_FLAGS)
debug: main

main: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) xjakub41.zip main

pack:
	zip xjakub41.zip *.c *.h rozdeleni dokumentace.pdf Makefile
