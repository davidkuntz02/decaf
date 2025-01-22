/*   Abstract syntax tree code

     This code is used to define an AST node, 
    routine for printing out the AST
    defining an enumerated type so we can figure out what we need to
    do with this.  The ENUM is basically going to be every non-terminal
    and terminal in our language.

    Shaun Cooper February 2020
    
    David Kuntz
    Fall 2022
    Lab 9 GAS from AST

*/

#include<stdio.h>
#include<malloc.h>
#include "ast.h" 


/* uses malloc to create an ASTnode and passes back the heap address of the newley created node */
ASTnode *ASTCreateNode(enum AST_Tree_Element_Type mytype)
{
    ASTnode *p;
    if (mydebug) fprintf(stderr,"Creating AST Node \n");
    p=(ASTnode *)malloc(sizeof(ASTnode)); // get head data
    p->type=mytype; // set up the Element type
    p->S1=NULL;
    p->S2=NULL;  // set default values
    p->value=0;
    return(p);
}

/*  Helper function to print tabbing */

//pre: int howmany >= 0
//post: prints howmany spaces
void PT(int howmany)
{
	for(int i=0;i<howmany;i++){
		printf(" ");
	}
}

//method ASTprintType(type)
//pre: enum AST_Decaf_Types type
//post: print string text of corresponding type 
void ASTprintType(enum AST_Decaf_Types type){
	switch (type) {
		case A_Decaf_INT : printf("INT "); return; break;
		case A_Decaf_BOOL : printf("BOOL "); return; break;
		case A_Decaf_VOID : printf("VOID "); return; break;
		case A_Decaf_STRING : printf("STRING "); return; break;
	default: printf("UNKNOWN TYPE "); return; break;
	}
}//end ASTprintType

/*  Print out the abstract syntax tree */
//pre: int level >= 0, ASTnode p exists
//post: prints an abstract syntax tree representation of a decaf program
void ASTprint(int level,ASTnode *p)
{
   int i;
   if (p == NULL ) return;
   else
     { 
       PT(level); /*indent */
       switch (p->type) {
        case A_PROGRAM : printf("PROGRAM\n");
        		ASTprint(level,p->S1); //externs
        		ASTprint(level,p->S2); //package
                     break;
        case A_PACKAGE : printf("PACKAGE %s\n",p->name);
        		ASTprint(level+1,p->S1); //field declarations
        		ASTprint(level+1,p->S2); //method declarations
                     break;
        case A_EXTERN : printf("EXTERN FUNC %s () TYPE: ",p->name);
        		ASTprintType(p->decl_type);
        		printf("\n");
        		ASTprint(level+1,p->S1); //extern types
                     break;
        case A_EXTERNTYPE : printf("PARAMETER TYPE: ");
        		ASTprintType(p->decl_type);
        		printf("\n");
                     break;
        case A_VARDEC :  printf("VARIABLE %s ",p->name);
        		if(p->S1 != NULL){ //if it's an array
        			printf("[");
        			printf("%d",p->S1->value);
        			printf("] ");
        		}
        		ASTprintType(p->decl_type);
        		if(p->S2 != NULL){ //if it's a constant assign
        			printf("= %d",p->S2->value);
        		}
        		printf("\n");
                     break;
        case A_METHODDEC :
                     printf("METHOD %s TYPE: ",p->name);
        	     ASTprintType(p->decl_type);
                     /* print out the parameter list */
                     if (p->S1 == NULL ) //if there are no parameters
		                  { PT(level+2); 
		                  printf (" (NO PARAMETERS) "); }
                     else
                       { 
                       	 printf("\n");
                         PT(level+2);
                         printf( "( \n");
                         ASTprint(level+2, p->S1); //parameters
                         PT(level+2);
                         printf( ") ");
                       }
                     printf("\n");
                     ASTprint(level+2, p->S2); // block
                     break;
        case A_METHODTYPE : //method type list
        		printf("METHOD VARIABLE %s ",p->name);
        		ASTprintType(p->decl_type);
        		printf("\n");
                     break;
        case A_METHODARG:
        		printf("METHOD ARG: \n");
        		ASTprint(level+1,p->S1);
        		break;
        case A_PARAM :  printf("PARAMETER %s ",p->name);
        		ASTprintType(p->decl_type);
                      if (p->value == -1 ) 
                         printf("[]");
                      printf("\n");                  
                     break;
        case A_EXPR :
        	printf("EXPR ");
                     switch(p->operator) {
   		      case A_PLUS : printf(" '+' ");
                           break;
   		      case A_MINUS: printf(" '-' ");
                           break;
                      case A_TIMES: printf(" '*' ");
                           break;
                      case A_DIVIDE: printf(" '/' ");
                           break;
                      case A_MOD: printf(" '%%' ");
                           break;
                      case A_AND: printf(" '&&' ");
                           break;
                      case A_OR: printf(" '||' ");
                           break;
                      case A_LSHIFT: printf(" '<<' ");
                           break;
                      case A_RSHIFT: printf(" '>>' ");
                           break;
                      case A_NOT: printf(" '!' ");
                           break;
                      case A_LT: printf(" '<' ");
                           break;
                      case A_GT: printf(" '>' ");
                           break;
                      case A_LEQ: printf(" '<=' ");
                           break;
                      case A_GEQ: printf(" '>=' ");
                           break;
                      case A_NEQ: printf(" '!=' ");
                           break;
                      case A_EQ: printf(" '==' ");
                           break;
                      case A_UMINUS: printf(" '-' ");
                           break;     
                       }
                     printf("\n");
                     ASTprint(level+1, p->S1);
		     if ((p->operator != A_NOT)&&(p->operator != A_UMINUS)) 
                         ASTprint(level+1, p->S2);
                     break;
        case A_BREAK : printf("BREAK STATEMENT \n");
                     break;
        case A_CONTINUE : printf("CONTINUE STATEMENT \n");
                     break;
        case A_RETURN : printf("RETURN STATEMENT \n");
        	     ASTprint(level+1, p->S1); //return expr
                     break;
        case A_BLOCK :  printf("BLOCK STATEMENT \n");
                     ASTprint(level+1, p->S1); //variable declarations
                     ASTprint(level+1, p->S2); //statements
                     break;
        case A_METHODCALL :  printf("METHOD CALL %s\n",p->name);
        	     PT(level+1);
        	     printf( "( \n");
                     ASTprint(level+2, p->S1); //parameters
                     PT(level+1);
                     printf( ") \n");
                     break;
        case A_ASSIGNSTMT :  printf("ASSIGNMENT STATEMENT \n");
                     ASTprint(level, p->S1); //lefthand variable
                     ASTprint(level+2, p->S2); //expressions
                     break;
        case A_IFSTMT :  printf("IF STATEMENT \n");
                     ASTprint(level+1, p->S1); //expressions
                     ASTprint(level+3, p->S2); //else or block
                     break;
        case A_ELSESTMT : 
        	     printf("\r"); //weird indentation fix through carriage return delimiter
        	     ASTprint(level, p->S1); //if block
                     PT(level);
                     printf("ELSE STATEMENT \n");
                     ASTprint(level, p->S2); //else block
                     break;
        case A_WHILESTMT :  printf("WHILE STATEMENT \n");
                     ASTprint(level+1, p->S1); //expressions
                     ASTprint(level+2, p->S2); //block
                     break;
        case A_CONSTANT_INT :  printf("CONSTANT INT %d\n",p->value);
                     break;
        case A_CONSTANT_BOOL :  printf("CONSTANT BOOLEAN ");
                     if(p->value == 1) printf("TRUE\n");
                     else printf("FALSE\n");
                     break;
        case A_CONSTANT_STRING :  printf("CONSTANT STRING %s\n",p->name);
                     break;
        case A_VAR_RVAL :  printf("VARIABLE %s\n",p->name);
        		if(p->S1 != NULL){ //if it's an array element
        			PT(level);
        			printf("[\n");
        			ASTprint(level+1,p->S1); //array index
        			PT(level);
        			printf("]\n");
        		}
                     break;
        case A_VAR_LVAL :  printf("VARIABLE %s\n",p->name);
        		if(p->S1 != NULL){ //if it's an array element
        			PT(level);
        			printf("[\n");
        			ASTprint(level+1,p->S1); //array index
        			PT(level);
        			printf("]\n");
        		}
        	     break;
        default: printf("unknown type in ASTprint\n");


       }
       ASTprint(level, p->next);
     }
}

//pre: a list of formal parameters and a list of actual parameters (ASTnode * fparms)
//post: returns 1 if lists match, 0 if they don't
int checkParam(ASTnode *formal, ASTnode *actual){
	ASTnode *cursor;
	int parity = 1; //they match by default
	
	//check for nulls
	if((formal == NULL)&&(actual == NULL)) return parity; //both empty
	if((formal == NULL)||(actual == NULL)) return 0; //one empty
	
	//compare node types
	if(formal->decl_type != actual->decl_type) parity = 0; //compare param types
	//recurse
	parity = checkParam(formal->next, actual->next);
	return parity;
}


/* dummy main program so I can compile for syntax error independently   
 
void main()
{ 
}
/* */
