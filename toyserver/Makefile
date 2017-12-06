CC=gcc
CFLAGS=-O2 -fsanitize=address

all: chimera patcher toyserver.so
clean:
	@$(RM) chimera patcher *.so *.o

chimera: chimera.c plthook-elf.h
	$(CC) $(CFLAGS) chimera.c -U_FORTIFY_SOURCE -ldl -pthread -o chimera

patcher: patcher.c
	$(CC) patcher.c -o patcher

toyserver.so: toyserver.c
	$(CC) $(CFLAGS) -U_FORTIFY_SOURCE -DATTRIBUTE_NO_SANITIZE_ADDRESS="__attribute__((no_sanitize_address))" -rdynamic -shared -fpic -o toyserver.so toyserver.c
	objcopy --weaken toyserver.so
	$(CC) $(CFLAGS) -DATTRIBUTE_NO_SANITIZE_ADDRESS="" -rdynamic -shared -fpic -o toyserver_safe.so toyserver.c
	objcopy --weaken toyserver_safe.so
