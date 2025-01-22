/*   Abstract syntax tree code


 Header file   
 Shaun Cooper January 2020
 
 David Kuntz
 Fall 2022
 Lab 9 GAS from AST

*/

#include<stdio.h>
#include<malloc.h>

#ifndef AST_H
#define AST_H
static int mydebug;

/* define the enumerated types for the AST.  THis is used to tell us what 
sort of production rule we came across */

enum AST_Tree_Element_Type {
   A_PROGRAM,
   A_PACKAGE,
   A_EXTERN,
   A_EXTERNTYPE,
   A_METHODDEC,
   A_METHODTYPE,
   A_METHODCALL,
   A_ASSIGNSTMT,
   A_WHILESTMT,
   A_IFSTMT,
   A_ELSESTMT,
   A_BLOCK,
   A_EXPR,
   A_PARAM,
   A_VARDEC,
   A_CONSTANT_INT,
   A_CONSTANT_BOOL,
   A_CONSTANT_STRING,
   A_ARRAYTYPE,
   A_VAR_LVAL, //variable lefthand value...
   A_VAR_RVAL, //variable righthand value
   A_BREAK,
   A_CONTINUE,
   A_RETURN,
   A_METHODARG
};


enum AST_Operators {
   A_PLUS,
   A_MINUS,
   A_TIMES,
   A_DIVIDE,
   A_MOD,
   A_AND,
   A_OR,
   A_LSHIFT,
   A_RSHIFT,
   A_NOT,
   A_UMINUS,
   A_LT,
   A_GT,
   A_LEQ,
   A_GEQ,
   A_NEQ,
   A_EQ
};

enum AST_Decaf_Types {
   A_Decaf_INT,
   A_Decaf_BOOL,
   A_Decaf_VOID,
   A_Decaf_STRING
};

/* define a type AST node which will hold pointers to AST structs that will
   allow us to represent the parsed code 
*/
typedef struct ASTnodetype
{
     enum AST_Tree_Element_Type type;
     enum AST_Operators operator;
     char * name;
     char * label;
     int value;
     struct SymbTab *symbol; 	//points to the symbol table
     int size;		//new size helper variable
     enum AST_Decaf_Types decl_type;
     struct ASTnodetype *S1,*S2, *next ;
     /* used for holding IF and WHILE components -- not very descriptive */
} ASTnode;

#include "symtable.h"

/* uses malloc to create an ASTnode and passes back the heap address of the newley created node */
ASTnode *ASTCreateNode(enum AST_Tree_Element_Type mytype);

//pre: int howmany >= 0
//post: prints howmany spaces
void PT(int howmany); 

ASTnode *program;  // Global Pointer for connection between YACC and backend

/*  Print out the abstract syntax tree */
//pre: int level >= 0, ASTnode p exists
//post: prints an abstract syntax tree representation of a decaf program
void ASTprint(int level,ASTnode *p); // prints tree with level horizontal spaceing

//pre: a list of formal parameters and a list of actual parameters (ASTnode * fparms)
//post: returns 1 if lists match, 0 if they don't
int checkParam(ASTnode *formal, ASTnode *actual);

#endif // of AST_H
