#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int a(int a, int b, int c, int d);
int b(int a, int b, int c);
int c(int a, int b);
int d(int a);

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

int a(int a, int b, int c, int d) {
	printf("a2 ");
	return vt.b(a,b,c) * vt.b(b,c,d);
}

int b(int a, int b, int c) {
	printf("b2 ");
	return vt.c(a,b) * vt.c(b,c);
}

int c(int a, int b) {
	printf("c2 ");
	return vt.d(a) * vt.d(b);
}

int d(int a) {
	printf("d2 ");
	return a + a;
}
