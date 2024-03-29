#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int a(int x, int y, int z, int w);
int b(int x, int y, int z);
int c(int x, int y);
int d(int x);

#define ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))
 
ATTRIBUTE_NO_SANITIZE_ADDRESS
int a(int x, int y, int z, int w) {
	printf("a1 ");
	return b(x,y,z) + b(y,z,w);
}

ATTRIBUTE_NO_SANITIZE_ADDRESS
int b(int x, int y, int z) {
	printf("b1 ");
	return c(x,y) + c(y,z);
}

ATTRIBUTE_NO_SANITIZE_ADDRESS
int c(int x, int y) {
	printf("c1 ");
	return d(x) + d(y);
}

ATTRIBUTE_NO_SANITIZE_ADDRESS
int d(int x) {
	printf("d1 ");
	return x * x;
}

// --- main ---------------------------------------------------

ATTRIBUTE_NO_SANITIZE_ADDRESS
int main(int argc, char **argv) {
	printf("argc: %d\n", argc);
	printf("argv[0]: %s\n", argv[0]);

	while(1) {
		printf("%d\n", a(1,2,3,4));
		sleep(1);
	}
	
	return 0;
}
