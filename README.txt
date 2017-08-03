# chimera

// --- fnptrs -----------------------------------------

make
./fnptrs

Different versions of the same function are mixed in the same call stack, so output varies, but state seems to be kept consistent.

// --- webserver --------------------------------------

I've been testing with:

g++ webserver.cpp -o webserver
./webserver

then going to 

http://localhost:8888/

in Chrome