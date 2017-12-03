all: chimera base patch
clean:
	@$(RM) chimera patcher *.so *.o

chimera: 
	gcc chimera.c -ldl -pthread -o chimera
	gcc patcher.c -o patcher

base:
	gcc main.c -rdynamic -shared -fPIC -o main.o
	ld -o main.so -shared main.o --filter chimera
	@$(RM) *.o

patch:
	gcc patch.c -rdynamic -shared -fPIC -o patch.o
	ld -o patch.so -shared patch.o --filter chimera
	@$(RM) *.o
	