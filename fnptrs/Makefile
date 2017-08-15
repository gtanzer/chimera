all:
	g++ fnptrs.cpp -o fnptrs
	g++ base.cpp -o base
	g++ patcher.cpp -o patcher
	g++ patch.cpp -c -o patch.o
	
	gcc dlbase.c -o dlbase
	gcc dlpatcher.c -o dlpatcher
	gcc patch.c -shared -o patch.dylib