#David Kuntz
#Fall 2022
#Lab 9 GAS from AST


all: lab9

lab9: lab9.l lab9.y ast.c ast.h symtable.c symtable.h emit.c emit.h
	yacc lab9.y
	lex lab9.l
	gcc -o lab9 lex.yy.c y.tab.c ast.c symtable.c emit.c

clean: lab9
	rm -f lab9
