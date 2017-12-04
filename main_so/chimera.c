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

static void *cur_dll;

void *patch_daemon(void *arg);

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    
    void *dll = dlopen("./main.so", RTLD_NOW | RTLD_GLOBAL);
    if(dll == NULL) {
        printf("dlopen() failed: %s\n", dlerror());
        return -1;
    }
    cur_dll = dll;
    
    int (*nmain)(int, char **) = dlsym(dll, "main");
    if(nmain == NULL) {
        printf("dlsym() failed to find new main: %s\n", dlerror());
        return -1;
    }
    
    pthread_t tid;
    int err = pthread_create(&tid, NULL, patch_daemon, NULL);
    
    nmain(argc, argv);
    
    return 0;
}

// --- patch daemon -------------------------------------------

#define UDS_NAME "patch_uds"
#define HEADER_LEN 2*sizeof(size_t)
#define QUEUE 1

int apply_patch(const char *so_name, const char *fn_name);

void *patch_daemon(void *arg) {

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd < 0) {
        perror("socket() failed");
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
        perror("listen() failed");
        _exit(1);
    }
    
    while(1) {
    
        int conn = accept(fd, NULL, NULL);
        if(conn < 0) {
            perror("accept() failed");
            continue;
        }
        
        union {
            char buf[HEADER_LEN];
            size_t lens[2];
        } header;
        
        int n = read(conn, header.buf, HEADER_LEN);
        if(n != HEADER_LEN) {
            perror("header read() failed");
            close(conn);
            continue;
        }
        
        if(n == HEADER_LEN) {
            char *so_name = malloc(header.lens[0]);
            char *fn_name = malloc(header.lens[1]);
        
            n = read(conn, so_name, header.lens[0]);
            if(n < header.lens[0]) {
                perror("so_name read() failed");
                goto cleanup;
            }

            n = read(conn, fn_name, header.lens[0]);
            if(n < header.lens[1]) {
                perror("fn_name read() failed");
                goto cleanup;
            }
        
            err = apply_patch(so_name, fn_name);
            if(err < 0) {
                printf("apply_patch() failed\n");
                goto cleanup;
            }
            
        cleanup:
            free(so_name);
            free(fn_name);

        }
        
        close(conn);
    }
    
    close(fd);
    unlink(UDS_NAME);
    
    return NULL;
}

int apply_patch(const char *so_name, const char *fn_name) {
    void *dll = dlopen(so_name, RTLD_NOW | RTLD_GLOBAL);
    if(dll == NULL) {
        printf("%s\n", dlerror());
        return -1;
    }

    printf("%s\n", fn_name);
    
    void *fn = dlsym(dll, fn_name);
    if(fn == NULL) {
        printf("dlsym() failed to find new fn: %s\n", dlerror());
        return -1;
    }
    printf("%p\n", fn);

    /*void *plt = dlsym(dll, "_GLOBAL_OFFSET_TABLE_");
    if(plt == NULL) {
        printf("dlsym() failed to find new plt: %s\n", dlerror());
        return -1;
    }
    printf("%p\n", plt);*/

    cur_dll = dll;
    
    // replace old PLT maybe
    // make sure globals are working properly
    
    // we could keep dll somewhere
    // and dlclose() if no stack frame
    // uses that version
    
    return 0;
}
