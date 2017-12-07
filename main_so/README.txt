// ------ INSTRUCTIONS -----------------------------------------------------

This folder has a toy program that I used while implementing live patching, and later AddressSanitizer support across instrumented vs. uninstrumented versions of the same files.

You can build everything with 'make', then run:

./chimera ./libname.so

You can patch in a function (in this case, a, b, c, or d) with:

./patcher ./new_libname.so func_name

You'll see the printout in each function change, as well as the overall result of the computation.

If you send the same library multiple times, it just refers to the original instance of it, so you can splice in multiple functions from the same shared object if you just call ./patcher again.

// ------ FILES ------------------------------------------------------------

chimera.c: dll management and live patching from socket (note: the file name is sent over the socket, not the actual object file)
patcher.c: program to send instructions to install a new patch

plthook-elf.h: a library for interposing on the PLT, specifically for ELF files
imalloc.h: malloc/free interception from before I decided to use AddressSanitizer

main.c: the intended base version of the program, though you can use the patches as a base version without loss of generality
patch.c: one sample patch
patch2.c: another sample patch

Makefile: lots of flag magic (U_FORTIFY_SOURCE is so gcc doesn't try to inline instrumented functions inside of uninstrumented functions)
README.txt: wow im so meta
