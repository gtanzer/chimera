all:    
	gcc chimera.c -ldl -pthread -o chimera
	gcc patcher.c -o patcher
	gcc main.c -rdynamic -shared -fPIC -o main.so
	ld -o main.so -shared main.so --filter chimera
	gcc patch.c -rdynamic -shared -fPIC -o patch.so
	ld -o patch.so -shared patch.so --filter chimera
	