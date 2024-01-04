#include <stdio.h>
#include <unistd.h>

int main(void) {
	char buf[100] = {0};
	read(0, buf, 100);
	for (int i = 0; i < 100; i++) {
		printf("%d ", buf[i]);
	}
	putchar('\n');
}
