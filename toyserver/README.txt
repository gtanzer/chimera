// ------ INSTRUCTIONS -----------------------------------------------------

Read the README in /main_so first. This folder has the toy web server test case described in the paper.

You can build everything with 'make', then run any of the following: (obviously replace 8080 with any port number)

./toyserver 8080 ./
./chimera ./toyserver.so 8080 ./
./chimera ./toyserver_safe.so 8080 ./

If you run the server through chimera, you can also patch in the other version of the program, e.g.:

./patcher ./toyserver_safe.so web

// ------ TESTING ----------------------------------------------------------

The ApacheBench test I ran was:

ab -k -c 350 -n 5000 http://localhost:8080/

It produces a very nice printout with lots of stats about server latency and bandwidth. You can also run:

python3 overflow.py localhost 8080

to perform a buffer overflow on the server; because it forks its connection handler, a successful attack is a silent crash, whereas a thwarted attack is an AddressSanitizer message. (Sorry, but I didn't make an actual attack that thwarts ASLR to test this.)

// ------ FILES ------------------------------------------------------------

chimera.c: dll management and live patching from socket (note: the file name is sent over the socket, not the actual object file)
patcher.c: program to send instructions to install a new patch
plthook-elf.h: a library for interposing on the PLT, specifically for ELF files

toyserver.c: the toy server I found on GitHub
index.html: the homepage for the website
page2.html: a second page on the website
marx.jpg: the only picture a website could ever need

Makefile: lots of flag magic (U_FORTIFY_SOURCE is so gcc doesn't try to inline instrumented functions inside of uninstrumented functions)
README.txt: wow im so meta
