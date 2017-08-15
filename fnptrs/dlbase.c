#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int a(int x, int y, int z, int w);
int b(int x, int y, int z);
int c(int x, int y);
int d(int x);

void *patch_daemon(void *arg);

// --- vtable -------------------------------------------------

struct vtable {
	int (*a)(int, int, int, int);
	int (*b)(int, int, int);
	int (*c)(int, int);
	int (*d)(int);
};

struct vtable vt = { &a, &b, &c, &d };

// --- base versions ------------------------------------------

int a(int x, int y, int z, int w) {
	printf("a1 ");
	return vt.b(x,y,z) + vt.b(y,z,w);
}

int b(int x, int y, int z) {
	printf("b1 ");
	return vt.c(x,y) + vt.c(y,z);
}

int c(int x, int y) {
	printf("c1 ");
	return vt.d(x) + vt.d(y);
}

int d(int x) {
	printf("d1 ");
	return x * x;
}

// --- main ---------------------------------------------------

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;
	
	pthread_t tid;
	int err = pthread_create(&tid, NULL, patch_daemon, NULL);
	
	while(1) {
		printf("%d\n", vt.a(1,2,3,4));
	}
	
	return 0;
}

// --- patch daemon -------------------------------------------

#define UDS_NAME "patch_uds"
#define QUEUE 1

int apply_patch(const char *name);

void *patch_daemon(void *arg) {

	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(fd < 0) {
		perror("socket() failed\n");
		_exit(1);
	}
	
	struct sockaddr_un server;
	server.sun_family = AF_UNIX;
	unlink(UDS_NAME);				   // make sure UDS_NAME is available
	strcpy(server.sun_path, UDS_NAME); // don't make UDS_NAME too long
	
	int err = bind(fd, (struct sockaddr *) &server, sizeof(struct sockaddr_un));
	if(err < 0) {
		perror("bind() failed\n");
		_exit(1);
	}
	
	err = listen(fd, QUEUE);
	if(err < 0) {
		perror("listen() failed\n");
		_exit(1);
	}
	
	while(1) {
	
		int conn = accept(fd, NULL, NULL);
		if(conn < 0) {
			perror("accept() failed\n");
			continue;
		}
		
		int n = read(conn, NULL, 1);
		if(n > 0) {
			perror("ping read() failed\n");
			close(conn);
			continue;
		}
		
		err = apply_patch("patch.dylib");
		if(err < 0) {
			perror("apply_patch() failed\n");
			_exit(1);
		}
		
		close(conn);
	}
	
	close(fd);
	unlink(UDS_NAME);
}

#include <dlfcn.h>
#define N_FUNCS 4

int apply_patch(const char *name) {

	void *dll = dlopen(name, RTLD_NOW | RTLD_GLOBAL);
	if(dll == NULL) {
		printf("%s\n", dlerror());
		return -1;
	}
	
	void **nvt = dlsym(dll, "vt");
	if(nvt == NULL) {
		perror("dlsym() failed to find new vt\n");
		return -1;
	}
	
	void **ovt = (void **) &vt;
	for(int i = 0; i < N_FUNCS; i++) {			// fill the old vtable without worrying about types
		ovt[i] = nvt[i];
	}
	
	// we could keep dll somewhere
	// and dlclose() if no stack frame
	// uses that version
	
	return 0;
}
