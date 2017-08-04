#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int a1(int a, int b, int c, int d);
int a2(int a, int b, int c, int d);
int b1(int a, int b, int c);
int b2(int a, int b, int c);
int c1(int a, int b);
int c2(int a, int b);
int d1(int a);
int d2(int a);

// --- vtable -------------------------------------------------

struct vtable {
	int (*a)(int, int, int, int);
	int (*b)(int, int, int);
	int (*c)(int, int);
	int (*d)(int);
};

struct vtable vt = { &a1, &b1, &c1, &d1 };

void flip_vtable(void) {
	vt.a = (vt.a == &a1) ? &a2 : &a1;
	vt.b = (vt.b == &b1) ? &b2 : &b1;
	vt.c = (vt.c == &c1) ? &c2 : &c1;
	vt.d = (vt.d == &d1) ? &d2 : &d1;
}

// --- alt versions -------------------------------------------

int a1(int a, int b, int c, int d) {
	printf("a1 ");
	return vt.b(a,b,c) + vt.b(b,c,d);
}

int a2(int a, int b, int c, int d) {
	printf("a2 ");
	return vt.b(a,b,c) * vt.b(b,c,d);
}

int b1(int a, int b, int c) {
	printf("b1 ");
	return vt.c(a,b) + vt.c(b,c);
}

int b2(int a, int b, int c) {
	printf("b2 ");
	return vt.c(a,b) * vt.c(b,c);
}

int c1(int a, int b) {
	printf("c1 ");
	return vt.d(a) + vt.d(b);
}

int c2(int a, int b) {
	printf("c2 ");
	return vt.d(a) * vt.d(b);
}

int d1(int a) {
	printf("d1 ");
	return a * a;
}

int d2(int a) {
	printf("d2 ");
	return a + a;
}

// --- main ---------------------------------------------------

void *daemon(void *arg) {
	while(1) {
		flip_vtable();
		// (╯°□°)╯︵ V┻━┻
	}
}

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;
	
	pthread_t tid;
	int err = pthread_create(&tid, NULL, daemon, NULL);
	
	while(1) {
		printf("%d\n", vt.a(1,2,3,4));
	}
	
	return 0;
}
