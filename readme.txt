David Kuntz Fall 2022 CS370 Lab 9

This program takes a decaf program file and compiles it into GAS assembly
in a Linux environment using the GNU compiler.

Dependencies:
-------------------------------------------------------
Ensure the following software are installed:

lex
yacc

Installing on ubuntu:
$ sudo apt install flex
$ sudo apt install bison


to run in terminal:
-------------------------------------------------------
            (small)		    
$ ./lab9 -o filename < filename.decaf -d 
(-d debug flag optional to display abstact syntax tree and symbol table data)


to compile and run resulting GAS file:
-------------------------------------------------------
$ gcc small.s
$ ./a.out


to delete lab9 executable and small.s: 
-------------------------------------------------------
make clean
