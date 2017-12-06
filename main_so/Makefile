CC=gcc
CFLAGS=-O2 -fsanitize=address

all: chimera patcher main.so patch.so patch2.so
clean:
	@$(RM) chimera patcher *.so *.o

chimera: chimera.c plthook-elf.h imalloc.h
	$(CC) $(CFLAGS)  chimera.c -U_FORTIFY_SOURCE -ldl -pthread -o chimera

patcher: patcher.c
	$(CC) patcher.c -o patcher

main.so: main.c
	$(CC) $(CFLAGS) -U_FORTIFY_SOURCE -rdynamic -shared -fpic -o main.so main.c
	objcopy --weaken main.so

patch.so: patch.c
	$(CC) $(CFLAGS) -rdynamic -shared -fpic -o patch.so patch.c
	objcopy --weaken patch.so

patch2.so: patch2.c
	$(CC) $(CFLAGS) -rdynamic -shared -fpic -o patch2.so patch2.c
	objcopy --weaken patch2.so