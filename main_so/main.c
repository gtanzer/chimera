#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int a(int x, int y, int z, int w);
int b(int x, int y, int z);
int c(int x, int y);
int d(int x);

int a(int x, int y, int z, int w) {
	printf("a1 ");
	return b(x,y,z) + b(y,z,w);
}

int b(int x, int y, int z) {
	printf("b1 ");
	return c(x,y) + c(y,z);
}

int c(int x, int y) {
	printf("c1 ");
	return d(x) + d(y);
}

int d(int x) {
	printf("d1 ");
	return x * x;
}

// --- main ---------------------------------------------------

int main(int argc, char **argv) {
	printf("argc: %d\n", argc);
	printf("argv[0]: %s\n", argv[0]);
	
	while(1) {
		printf("%d\n", a(1,2,3,4));
		sleep(1);
	}
	
	return 0;
}
