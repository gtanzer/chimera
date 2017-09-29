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
	
	// --- send patch -----------------------------------------
	
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(fd < 0) {
		perror("socket() failed\n");
		_exit(1);
	}
	
	struct sockaddr_un server;
	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, UDS_NAME); // don't make NAME too long
	
	int err = connect(fd, (struct sockaddr *) &server, sizeof(struct sockaddr_un));
	if(err < 0) {
		close(fd);
		perror("connect() failed\n");
		_exit(1);
	}
	
	char *name;
	if(argc == 2)
		name = "patch.so";
	else
		name = "patch.so";
	size_t len = strlen(name);
	
	err = write(fd, (void *) &len, sizeof(size_t));
	if(err < 0) {
		close(fd);
		perror("header write() failed\n");
		_exit(1);
	}
	
	err = write(fd, name, len);
	if(err < 0) {
		close(fd);
		perror("body write() failed\n");
		_exit(1);
	}
	
	close(fd);
	
	return 0;
}
