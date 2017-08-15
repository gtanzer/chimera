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

int apply_patch(const char *buf, size_t len);
void *daemon(void *arg);

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
	int err = pthread_create(&tid, NULL, daemon, NULL);
	
	while(1) {
		printf("%d\n", vt.a(1,2,3,4));
		sleep(1);
	}
	
	return 0;
}

// --- update daemon ------------------------------------------

#define UDS_NAME "patch_uds"
#define HEADER_LEN sizeof(size_t)
#define QUEUE 1

int apply_patch(const char *buf, size_t len);

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
			
			err = apply_patch(buf, len);
			if(err < 0) {
				perror("apply_patch() failed\n");
				munmap(buf, len);
				close(conn);
				continue;
			}
		}
		else {
			perror("header read() failed\n");
		}
		
		close(conn);
	}
	
	close(fd);
	unlink(UDS_NAME);
}

// https://opensource.apple.com/source/cctools/cctools-795/include/mach-o/loader.h

#include <mach-o/loader.h>
#include <mach-o/nlist.h>

int apply_patch(const char *buf, size_t len) {
	
	struct mach_header_64 *h = (struct mach_header_64 *) buf;
	
	if(h->magic != MH_MAGIC_64) {
		perror("Must be native-endian 64-bit Mach-O file\n");
		return -1;
	}
	
	if(h->filetype != MH_OBJECT) {
		perror("Not a relocatable object file\n");
		return -1;
	}
	
	size_t offset = sizeof(struct mach_header_64);

	uint64_t *fns = NULL;
	int f = 0;
	void *segment_start = NULL;
	
	for(int i = 0; i < h->ncmds; i++) {
		uint32_t *cmd = (uint32_t *) &buf[offset];
		uint32_t cmdsize = cmd[1];
		
		switch(*cmd) {
			case LC_SYMTAB: {
				struct symtab_command *c = (struct symtab_command *) cmd;
				
				fns = (uint64_t *) malloc(sizeof(uint64_t) * c->nsyms);
				if(fns == NULL) {
					perror("fns malloc() failed\n");
					return -1;
				}
				
				size_t soffset = c->symoff;
				for(int j = 0; j < c->nsyms; j++) {
					struct nlist_64 *n = (struct nlist_64 *) &buf[soffset];
					
					if(n->n_sect == 1) {		// __text
						fns[f] = n->n_value;
						f++;
					}
					
					soffset += sizeof(struct nlist_64);
				}
				
				break;
			}
			case LC_DYSYMTAB:
				break;
			case LC_SEGMENT_64: {
				struct segment_command_64 *c = (struct segment_command_64 *) cmd;
			
				size_t soffset = offset + sizeof(segment_command_64);
				for(int j = 0; j < c->nsects; j++) {
					struct section_64 *s = (struct section_64 *) &buf[soffset];
					
					if(strcmp(s->sectname, SECT_TEXT) == 0) {
						segment_start = (void *) &buf[s->offset];
					}
					
					soffset += sizeof(struct section_64);
				}
				break;
			}
			default:
				break;
		}

		offset += cmdsize;
	}
	
	if(fns == NULL) {
		perror("Broken symbol table\n");
		return -1;
	}
	if(segment_start == NULL) {
		perror("Broken segment command\n");
		free(fns);
		return -1;
	}
	
	printf("\nBefore:\n");
	printf("vt.a: %p\n", vt.a);
	printf("vt.b: %p\n", vt.b);
	printf("vt.c: %p\n", vt.c);
	printf("vt.d: %p\n", vt.d);
	
	int err = mprotect((void *) buf, len, PROT_READ | PROT_EXEC);
	if(err < 0) {
		perror("mprotect() failed\n");
		free(fns);
		return -1;
	}
	
	for(int i = 0; i < f; i++) {			// fill the old vtable without worrying about types
		void **p = &((void **) &vt)[i];
		*p = (void *) ((uintmax_t) segment_start + (uintmax_t) fns[i]);
	}
	
	printf("\nAfter:\n");
	printf("vt.a: %p\n", vt.a);
	printf("vt.b: %p\n", vt.b);
	printf("vt.c: %p\n", vt.c);
	printf("vt.d: %p\n", vt.d);
	
	free(fns);
	
	//mprotect((void *) buf, len, PROT_EXEC);
	
	return 0;
}


