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
		perror("socket() failed");
		_exit(1);
	}
	
	struct sockaddr_un server;
	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, UDS_NAME); // don't make UDS_NAME too long
	
	int err = connect(fd, (struct sockaddr *) &server, sizeof(struct sockaddr_un));
	if(err < 0) {
		close(fd);
		perror("connect() failed");
		_exit(1);
	}
	
	char *so_name, *fn_name;

	so_name = "./patch.so";
	if(argc == 3) {
		so_name = argv[1];
		fn_name = argv[2];
	}
	else if(argc == 2) {
		fn_name = argv[1];
	}
	else {
		printf("Usage: patcher [patch_name] function_name\n");
		_exit(1);
	}
	size_t lens[2] = { strlen(so_name)+1, strlen(fn_name)+1 };
	
	err = write(fd, (void *) &lens[0], 2*sizeof(size_t));
	if(err < 0) {
		close(fd);
		perror("header write() failed");
		_exit(1);
	}
	
	err = write(fd, so_name, lens[0]);
	if(err < 0) {
		close(fd);
		perror("so_name write() failed");
		_exit(1);
	}
	
	err = write(fd, fn_name, lens[1]);
	if(err < 0) {
		close(fd);
		perror("fn_name write() failed");
		_exit(1);
	}

	close(fd);
	return 0;
}
