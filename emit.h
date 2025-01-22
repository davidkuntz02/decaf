//  Subprograms which output NASM code.
//
//   Shaun Cooper Spring 2017
/// Updated Spring 2017 to utilize the data segement portion of NASM instead of advancing the Stack and Global pointers
/// to accomodate global variables.

// NASM is called on linux as
//  gcc foo.s 

//   David Kuntz
//   Fall 2022
//   Lab 9 GAS from AST

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef EMIT_H
#define EMIT_H

#include "ast.h"
#include "symtable.h"
#define WSIZE 8  // number of bytes in a word
#define LOGWSIZE 2  // number of shifts to get to WSIZE
FILE *fp; //file pointer

char *genlabel(); //needed in YACC for T_STRING_LIT

//wow so many methods...

//pre: ast and file pointers are not null
//post: a complete, correct assembly program written to output file
void EMIT(ASTnode *p,FILE *fp);

//pre: pointers not null, p points to AST node
//post: GAS code for each AST instruction
void EMITAST(ASTnode *p,FILE *fp);

//pre: ast and file pointers are not null
//post: GAS code for method declaration to be placed in the .text section
void Emit_Method_Decl(ASTnode *p, FILE *fp);

//pre: ast and file pointers are not null
//post: GAS code that sets %rax to the result of an expression
void emit_expr(ASTnode *p, FILE *fp);

//pre: fp is not null; label, action, comment pointers can be empty
//post: output formatted to proper GAS code syntax
void emit( FILE *fp, char *label, char *action, char *comment);

//pre: file pointer is not null, can handle p being null
//post: GAS code for strings to be placed in the .rodata section
void Emit_Strings(ASTnode *p, FILE *fp);

//pre: pointers are not null, p points to a variable id
//post: GAS code to set %rax to address of id
void emit_ident(ASTnode *p, FILE *fp);

//pre: pointers not null, p points to method call
//post: GAS code for method calls, %rax is set by the method
void Emit_Method_Call(ASTnode *p, FILE *fp);

//pre: ast and file pointers are not null
//post: GAS code for extern method calls, %rax is set by the method
void Emit_Extern_Method_Call(ASTnode *p, FILE *fp);

//pre: file pointer is not null, ast pointer to return or null
//post: GAS code for returning from a method
void Emit_Return(ASTnode *p, FILE *fp);

//pre: pointers not null, p points to assign
//post: GAS code that handles the assignment
void emit_assign(ASTnode *p, FILE *fp);

//pre: pointers not null, p points to if statement
//post: GAS code that handles an if statement
void emit_if(ASTnode *p, FILE *fp);

//pre: pointers not null, p points to while statememt
//post: GAS code that handles the while statement
void emit_while(ASTnode *p, FILE *fp);

//pre: ast and file pointers are not null
//post: GAS code for global variables to be placed in the .data section
void Emit_Globals(ASTnode *p, FILE *fp);

//pre: pointers not null, p points to break statement inside a while loop
//after PushLabelBreakList() is called
//post: GAS code that handles a break statement
void emit_break(ASTnode *p, FILE *fp);

//pre: pointers not null, p points to continue statement inside a while loop
//after PushLabelContinueList() is called
//post: GAS code that handles a continue statement
void emit_continue(ASTnode *p, FILE *fp);


#endif  // of EMIT.h
