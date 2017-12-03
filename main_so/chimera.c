#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>

static void **cur_plt;

void *patch_daemon(void *arg);

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    
    void *dll = dlopen("./main.so", RTLD_NOW | RTLD_GLOBAL);
    if(dll == NULL) {
        printf("dlopen() failed: %s\n", dlerror());
        return -1;
    }
    
    int (*nmain)(int, char **) = dlsym(dll, "main");
    if(nmain == NULL) {
        printf("dlsym() failed to find new main: %s\n", dlerror());
        return -1;
    }
    
    pthread_t tid;
    int err = pthread_create(&tid, NULL, patch_daemon, NULL);
    
    cur_plt = (void **) nmain;
    nmain(argc, argv);
    
    return 0;
}

// --- patch daemon -------------------------------------------

#define UDS_NAME "patch_uds"
#define HEADER_LEN sizeof(size_t)
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
    unlink(UDS_NAME);                  // make sure UDS_NAME is available
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
            char buf[HEADER_LEN];
            size_t len;
        } header;
        
        int n = read(conn, header.buf, HEADER_LEN);
        if(n < HEADER_LEN) {
            perror("header read() failed\n");
            close(conn);
            continue;
        }
        
        if(n == HEADER_LEN) {
            char *name = malloc(header.len);
        
            n = read(conn, name, header.len);
            if(n < header.len) {
                perror("body read() failed\n");
                free(name);
                close(conn);
                continue;
            }
        
            err = apply_patch(name);
            if(err < 0) {
                perror("apply_patch() failed\n");
                _exit(1);
            }
            
            free(name);
        }
        else {
            perror("header read() failed\n");
        }
        
        close(conn);
    }
    
    close(fd);
    unlink(UDS_NAME);
    
    return NULL;
}

int apply_patch(const char *name) {
    void *dll = dlopen(name, RTLD_NOW | RTLD_GLOBAL);
    if(dll == NULL) {
        printf("%s\n", dlerror());
        return -1;
    }

    // hopefully --filter will make this work automatically
    
    int (*a)(int, char **) = dlsym(dll, "a");
    if(a == NULL) {
        perror("dlsym() failed to find new a\n");
        return -1;
    }
    printf("%p\n", a);
    
    // replace old PLT maybe
    // make sure globals are working properly
    
    // we could keep dll somewhere
    // and dlclose() if no stack frame
    // uses that version
    
    return 0;
}
