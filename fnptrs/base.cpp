#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <string.h>
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

// --- update daemon ------------------------------------------

#define UDS_NAME "patch_uds"
#define HEADER_LEN sizeof(size_t)
#define QUEUE 1

void *daemon(void *arg) {

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
		
		union {
			char header[HEADER_LEN];
			size_t len;
		};
		
		int n = read(conn, header, HEADER_LEN);
		if(n < HEADER_LEN) {
			perror("header read() failed\n");
			close(conn);
			continue;
		}
		
		if(n == HEADER_LEN) {
		
			char *buf = (char *) mmap(
										NULL,							// no preferred address
										len,
										PROT_READ | PROT_WRITE,
										MAP_PRIVATE | MAP_ANONYMOUS,	// no file backing
										-1,								// MAP_ANONYMOUS
										0								// MAP_ANONYMOUS
									 );
			if(buf == NULL) {
				perror("mmap() failed\n");
				close(conn);
				continue;
			}
			
			n = read(conn, buf, len);
			if(n < len) {
				perror("body read() failed\n");
				munmap(buf, len);
				close(conn);
				continue;
			}
			
			// asymmetric encryption magic might go here
			
			for(int i = 0; i < len; i++) {
				printf("%x ", buf[i]);
			}
			
			munmap(buf, len);
		}
		else {
			perror("header read() failed\n");
		}
		
		close(conn);
	}
	
	close(fd);
	unlink(UDS_NAME);
}

// --- main ---------------------------------------------------

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;
	
	pthread_t tid;
	int err = pthread_create(&tid, NULL, daemon, NULL);
	
	while(1) {
		//printf("%d\n", vt.a(1,2,3,4));
	}
	
	return 0;
}
