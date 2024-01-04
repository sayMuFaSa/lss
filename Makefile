all:debug
debug:
	cc -g -o ls main.c
release:
	cc -O1 -o ls main.c
clean:
	rm ls
