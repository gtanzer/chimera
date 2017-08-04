#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define UDS_NAME "patch_uds"

int main(int argc, char **argv) {

	// --- load patch -----------------------------------------

	int fd = open("patch.o", O_RDONLY);
	if(fd < 0) {
		perror("Open failed\n");
		_exit(1);
	}
	
	struct stat s;
	int err = fstat(fd, &s);
	if(err < 0) {
		perror("fstat failed\n");
		_exit(1);
	}
	
	size_t len = s.st_size;
	char *buf = (char *) malloc(len);
	err = read(fd, buf, len);
	if(err < 0) {
		perror("read failed\n");
		_exit(1);
	}
	close(fd);
	
	// --- send patch -----------------------------------------
	
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(fd < 0) {
		perror("socket() failed\n");
		_exit(1);
	}
	
	struct sockaddr_un server;
	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, UDS_NAME); // don't make NAME too long
	
	err = connect(fd, (struct sockaddr *) &server, sizeof(struct sockaddr_un));
	if(err < 0) {
		close(fd);
		perror("connect() failed\n");
		_exit(1);
	}
	
	err = write(fd, (void *) &len, sizeof(size_t));
	if(err < 0) {
		close(fd);
		perror("header write() failed\n");
		_exit(1);
	}
	
	err = write(fd, buf, len);
	if(err < 0) {
		close(fd);
		perror("body write() failed\n");
		_exit(1);
	}
	
	close(fd);
	free(buf);
	
	return 0;
}
