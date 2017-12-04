all: chimera patcher main.so patch.so
clean:
	@$(RM) chimera patcher *.so *.o

chimera: 
	gcc chimera.c -ldl -pthread -o chimera

patcher:
	gcc patcher.c -o patcher

main.so:
	gcc main.c -rdynamic -shared -fPIC -Xlinker "-soname=chimera" -Xlinker "--filter" -Xlinker "chimera" -o main.so

patch.so:
	gcc patch.c -rdynamic -shared -fPIC -Xlinker "-soname=chimera" -Xlinker "--filter" -Xlinker "chimera" -o patch.so
	