## NMSU CS 370 (COSC 3700) Decaf to GAS Assembly Compiler Project

David Kuntz Fall 2022

This program compiles Decaf files to GAS assembly using (f)Lex, Bison Yacc, and gcc.

### Dependencies:
###### Ensure lex and yacc are installed in your Linux environment.
###### Installing on ubuntu:
    $ sudo apt install flex
    $ sudo apt install bison

### To compile Decaf to GAS:
    $ make		    
    $ ./lab9 -o filename < filename.decaf -d
(-d debug flag optional to display abstact syntax tree and symbol table data)

### To compile and run resulting GAS file:
    $ gcc filename.s
    $ ./a.out

### Don't forget to clean up after yourself 
    $ make clean
