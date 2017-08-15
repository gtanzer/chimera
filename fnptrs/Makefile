all:
	g++ fnptrs.cpp -o fnptrs
	g++ base.cpp -o base
	g++ patcher.cpp -o patcher
	g++ patch.cpp -c -o patch.o
	
	gcc dlbase.c -rdynamic -o dlbase
	gcc dlpatcher.c -o dlpatcher
	gcc patch.c -shared -fPIC -o patch.dylib
	gcc patch2.c -shared -fPIC -o patch2.dylib