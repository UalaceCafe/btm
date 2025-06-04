all: compile

CC = gcc
SOURCES = src/main.c src/*/*.c
CFLAGS = -std=c11 -O2 -Wall -pedantic -Werror
DEFS = -DTAPE_SIZE=11 -DSTEPS

.PHONY: clean

clean:
	rm -f bin/*

compile:
	$(CC) $(CFLAGS) $(SOURCES) $(DEFS) -o bin/tml
