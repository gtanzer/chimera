all:	
	gcc chimera.c -o chimera
	gcc patcher.c -o patcher
	gcc main.c -rdynamic -shared -fPIC -o main.so
	ld main.so --filter chimera
	gcc patch.c -rdynamic -shared -fPIC -o patch.so
	