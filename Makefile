include config.mk
CC ?= gcc

all:debug
debug:
	$(CC) $(CFLAGS) -g -o ls main.c
release:
	$(CC) $(CFLAGS) -O1 -o ls main.c
clean:
	rm ls
