/*  Symbol Table --linked list  headers
    Used for Compilers class

    Modified Spring 2015 to allow for name to pointed to by symtable, instead of copied, since the name is copied
    into the heap

    Modified to have levels.  A level 0 means global variable, other levels means in range of the function.  We
    start out our offsets at 0 (from stack pointer) for level 1,,,when we enter a functional declaration.
    We increment offset each time we insert a new variable.  A variable is considered to be valid if it is found in
    the symbol table at our level or lesser level.  If at 0, then it is global.  

    We return a pointer to the symbol table when a variable matches our creteria.

    We add a routine to remove variables at our level and above.
    
    David Kuntz
    Fall 2022
    Lab 9 GAS from AST
*/

    

#include "ast.h"

#ifndef _SYMTAB 
#define _SYMTAB

//pre: symbol table exists
//post: displays elements in symbol table at level where it is called
void Display();

//pre: offset > 0
//post: removes all symbol entries at specified level and returns the offset value they occupied
int Delete(int level);

//pre: idk what this one does
//int FetchAddr (char *lab);

// The types an ID can have
enum ID_Sub_Type {
	ID_Sub_Type_Scalar, 
	ID_Sub_Type_Method,
	ID_Sub_Type_Array,
	ID_Sub_Type_Package,
	ID_Sub_Type_Extern_Method
};

struct SymbTab
{
     char *name;
     int offset; /* from activation record boundary */
     int mysize;  /* number of words this item is 1 or more */
     int level;  /* the level where we found the variable */
     enum AST_Decaf_Types Type;  /* the type of the symbol */
     enum ID_Sub_Type  SubType;  /* the element is a function */
     ASTnode * fparms; /* pointer to parameters of the function in the AST */

     struct SymbTab *next;
};

//precondition: Symbtab struct pointer, variable name, type, subtype, level, size, offset, formal parameters 
//postcondition: new struct Symbtab element inserted into symbol table
struct SymbTab * Insert(char *name, enum AST_Decaf_Types Type, enum ID_Sub_Type subtype, int  level, int mysize, int offset, ASTnode * fparms );

//precondition: Symbtab struct pointer, variable name, variable level, int recur counter 
//postcondition: returns specified struct Symbtab element from symbol table if it exists, returns NULL otherwise
struct SymbTab * Search(char name[], int level, int recur);

//pre: um
//post: creates a name for a temporary value
char * CREATE_TEMP();

#endif
