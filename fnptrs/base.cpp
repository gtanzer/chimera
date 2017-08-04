#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int a(int a, int b, int c, int d);
int b(int a, int b, int c);
int c(int a, int b);
int d(int a);

// --- vtable -------------------------------------------------

struct vtable {
	int (*a)(int, int, int, int);
	int (*b)(int, int, int);
	int (*c)(int, int);
	int (*d)(int);
};

struct vtable vt = { &a, &b, &c, &d };

// --- base versions ------------------------------------------

int a(int a, int b, int c, int d) {
	printf("a1 ");
	return vt.b(a,b,c) + vt.b(b,c,d);
}

int b(int a, int b, int c) {
	printf("b1 ");
	return vt.c(a,b) + vt.c(b,c);
}

int c(int a, int b) {
	printf("c1 ");
	return vt.d(a) + vt.d(b);
}

int d(int a) {
	printf("d1 ");
	return a * a;
}

// --- main ---------------------------------------------------

void *daemon(void *arg) {
	while(1) {
		usleep(100);
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
