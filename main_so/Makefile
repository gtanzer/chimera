CC=gcc
CFLAGS=-O2 -fsanitize=address

all: chimera patcher main.so patch.so patch2.so
clean:
	@$(RM) chimera patcher *.so *.o

chimera: chimera.c plthook-elf.h imalloc.h
	$(CC) $(CFLAGS) chimera.c -U_FORTIFY_SOURCE -ldl -pthread -o chimera

patcher: patcher.c
	$(CC) patcher.c -o patcher

main.so: main.c
	$(CC) $(CFLAGS) -U_FORTIFY_SOURCE main.c -rdynamic -shared -fpic -o main.so
	objcopy --weaken main.so

patch.so: patch.c
	$(CC) $(CFLAGS) -U_FORTIFY_SOURCE patch.c -rdynamic -shared -fpic -o patch.so
	objcopy --weaken patch.so

patch2.so: patch2.c
	$(CC) $(CFLAGS) -U_FORTIFY_SOURCE patch2.c -rdynamic -shared -fpic -o patch2.so
	objcopy --weaken patch2.so