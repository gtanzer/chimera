#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int a(int x, int y, int z, int w);
int b(int x, int y, int z);
int c(int x, int y);
int d(int x);

// --- alt versions -------------------------------------------

int a(int x, int y, int z, int w) {
	printf("a2 ");
	return b(x,y,z) * b(y,z,w);
}

int b(int x, int y, int z) {
	printf("b2 ");
	return c(x,y) * c(y,z);
}

int c(int x, int y) {
	printf("c2 ");
	return d(x) * d(y);
}

int d(int x) {
	printf("d2 ");
	if(x < 1) {
		return x;
	}
	return x + d(x-1);
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