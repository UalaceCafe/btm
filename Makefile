all: compile

CC = gcc
SOURCES = src/main.c src/*/*.c
CFLAGS = -std=c11 -O2 -Wall -pedantic -Werror
DEFS = -DTAPE_SIZE=15 -DSTEPS
TARGET = btm

.PHONY: clean

clean:
	rm -f bin/$(TARGET)

compile:
	$(CC) $(CFLAGS) $(SOURCES) $(DEFS) -o bin/$(TARGET)
