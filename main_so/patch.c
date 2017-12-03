#include <stdio.h>

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
	return x + x;
}
