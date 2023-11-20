CC = gcc
CFLAGS = -Wall -Wextra -std=c11
DEBUG_FLAGS = -DDEBUG -g
SOURCES = expression.c stack.c exp_semantic.c token.c scanner.c str.c symtable.c parser.c main.c gen.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all debug exp clean

all: exp

debug: CFLAGS += $(DEBUG_FLAGS)
debug: exp

exp: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) exp
