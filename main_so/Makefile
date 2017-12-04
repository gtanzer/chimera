CC=gcc

all: chimera patcher main.so patch.so
clean:
	@$(RM) chimera patcher *.so *.o

chimera: chimera.c plthook-elf.h
	$(CC) chimera.c -ldl -pthread -o chimera

patcher: patcher.c
	$(CC) patcher.c -o patcher

main.so: main.c
	$(CC) main.c -rdynamic -shared -fpic -o main.so
	objcopy --weaken main.so

patch.so: patch.c
	$(CC) patch.c -rdynamic -shared -fpic -o patch.so
	objcopy --weaken patch.so