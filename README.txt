# chimera

// --- fnptrs -----------------------------------------

make
./dlbase
./dlpatcher

The program correctly transitions to a new patch located in the current directory. It currently uses a somewhat-unideal virtual table hopping strategy; implementing the original version as a dll or properly massaging the compiler may fix this problem.


make
./fnptrs

Different versions of the same function are mixed in the same call stack, so output varies, but state seems to be kept consistent.


make
./base
./patcher

Currently, the patcher sends the .o file for the update over a UDS to the original program. Object files on my computer are Mach-O format, so I'll use that format for this. Presumably the final implementation will be portable.

// --- webserver --------------------------------------

I've been testing with:

g++ webserver.cpp -o webserver
./webserver

then going to 

http://localhost:8888/

in Chrome