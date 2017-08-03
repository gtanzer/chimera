//  webserver.cpp

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <pthread.h>

#define BUFFER_SIZE 4096
#define PORT 8888
#define QUEUE 3

struct sockinfo {
	int fd;
	struct sockaddr_in addr;
	socklen_t addrlen;
};


// idk what exactly would go in here, but
// that's not too important for now
struct httpdata {
	int a, b, c;
};


// ---- SWAPPABLE CODE (MANAGED OR UNMANAGED) -------------------------------------------------------------


struct httpdata parse_packet(char *buffer) {
	std::cout << buffer;
	
	// do nothing (would use BUFFER_SIZE for bounds checking)
	
	return (struct httpdata) {1,2,3};
}


struct httpdata read_packet(int fd) {
	char *buffer = (char *) calloc(BUFFER_SIZE, 1);
	int sz = read(fd, buffer, BUFFER_SIZE);
	
	struct httpdata packet = parse_packet(buffer);
	
	free(buffer);
	return packet;
}


void make_response(struct httpdata packet, char *response) {
	(void) packet;
	
	const char *dummy = "\\\nHTTP/1.1 200 OK\n\nHello, World!";
	std::cout << dummy;
	
	memcpy(response, dummy, strlen(dummy));	// obviously not final
}


void send_packet(int fd, const char *response) {
	send(fd, response, strnlen(response, BUFFER_SIZE), 0);
	std::cout << "Hello message sent\n";
}


void handle_client(struct sockinfo conn) {
	// conn.addr and conn.addrlen unused, but maybe they could be used in the future

	// more complicated logic to read packets in multiple parts is probably needed
	struct httpdata packet = read_packet(conn.fd);
	
	char *buffer = (char *) calloc(BUFFER_SIZE, 1);
	
	make_response(packet, buffer);
	send_packet(conn.fd, buffer);
	
	free(buffer);
	close(conn.fd);
}


struct sockinfo socket_setup() {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == 0) {
		perror("socket() failed\n");
		_exit(1);
	}
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);
	socklen_t addrlen = (socklen_t) sizeof(addr);
	
	int err = bind(fd, (struct sockaddr *)&addr, addrlen);
	if(err < 0) {
		perror("bind() failed\n");
		_exit(1);
	}
	
	err = listen(fd, QUEUE);
	if(err < 0) {
		perror("listen() failed\n");
		_exit(1);
	}
	
	struct sockinfo sock = { .fd = fd, .addr = addr, .addrlen = addrlen };
	return sock;
}


struct sockinfo wrapped_accept(struct sockinfo sock) {
	int fd = accept(sock.fd, nullptr, nullptr);
	if(fd < 0) {
		perror("accept() failed\n");
	}
	struct sockinfo conn = { .fd = fd, .addr = sock.addr, .addrlen = sock.addrlen };
	return conn;
}


// ---- UNSWAPPABLE CODE (UNMANAGED ONLY) -----------------------------------------------------------------


void *thread_init(void *arg) {
	struct sockinfo *heap_conn = (struct sockinfo *) arg;
	struct sockinfo conn = { .fd = heap_conn->fd, .addr = heap_conn->addr, .addrlen = heap_conn->addrlen };
	free(heap_conn);

	handle_client(conn);
	
	return nullptr;
}


int main(int argc, char **argv) {
	(void) argc;
	(void) argv;
	
	struct sockinfo sock = socket_setup();

	while(1) {
		struct sockinfo conn = wrapped_accept(sock);
		if(conn.fd < 0) {
			continue;
		}
		struct sockinfo *heap_conn = (struct sockinfo *) malloc(sizeof(struct sockinfo));
		memcpy(heap_conn, &conn, sizeof(struct sockinfo));
		
		pthread_t tid;
		int err = pthread_create(&tid, nullptr, thread_init, heap_conn);
		if(err != 0) {
			// presumably max threads have been spawned
			perror("pthread_create() failed\n");
			continue;
		}
		
		err = pthread_detach(tid);
		if(err != 0) {
			// this failure should not be possible with correct code
			perror("pthread_detach() failed\n");
			_exit(1);
		}
	}
}
