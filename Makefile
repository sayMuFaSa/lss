include config.mk

all:debug
debug:
	$(CC) $(CFLAGS) -Wall -g -c main.c
	$(CC) $(CFLAGS) -Wall -g -c ls.c
	$(CC) -o ls ls.o main.o
release:
	$(CC) $(CFLAGS) -Wall -O1 -c main.c
	$(CC) $(CFLAGS) -Wall -O1 -c ls.c
	$(CC) -s -o ls ls.o main.o
clean:
	rm ls *.o
