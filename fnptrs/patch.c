#include <stdio.h>

int a(int x, int y, int z, int w);
int b(int x, int y, int z);
int c(int x, int y);
int d(int x);

// --- vtable -------------------------------------------------


// the compiler will need to be aware of the original program vtable address
struct vtable {
	int (*a)(int, int, int, int);
	int (*b)(int, int, int);
	int (*c)(int, int);
	int (*d)(int);
};

struct vtable vt = { &a, &b, &c, &d };

// --- alt versions -------------------------------------------

int a(int x, int y, int z, int w) {
	printf("a2 ");
	return vt.b(x,y,z) * vt.b(y,z,w);
}

int b(int x, int y, int z) {
	printf("b2 ");
	return vt.c(x,y) * vt.c(y,z);
}

int c(int x, int y) {
	printf("c2 ");
	return vt.d(x) * vt.d(y);
}

int d(int x) {
	printf("d2 ");
	return x + x;
}
