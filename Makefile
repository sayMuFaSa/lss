include config.mk
CC ?= gcc
CFLAGS ?=

all:debug
debug:
	$(CC) $(CFLAGS) -Wall -g -o ls main.c
release:
	$(CC) $(CFLAGS) -Wall -O1 -s -o ls main.c
clean:
	rm ls
