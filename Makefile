include config.mk
CC ?= gcc
CFLAGS ?=

all:debug
debug:
	$(CC) $(CFLAGS) -Wall -g -o ls main.c
release:
	$(CC) $(CFLAGS) -O1 -o ls main.c
clean:
	rm ls
