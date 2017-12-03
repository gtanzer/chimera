all: chimera patcher main.so patch.so
clean:
	@$(RM) chimera patcher *.so *.o

chimera: 
	gcc chimera.c -ldl -pthread -o chimera

patcher:
	gcc patcher.c -o patcher

main.so:
	gcc main.c -rdynamic -shared -fPIC -o main.so
	#ld -o main.so -shared main.o --filter chimera

patch.so:
	gcc patch.c -rdynamic -shared -fPIC -o patch.so
	#ld -o patch.so -shared patch.o --filter chimera
	