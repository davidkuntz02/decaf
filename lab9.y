%{
/*
 *			**** DECAF ****
 *
 * This YACC routine will function like a programming language
 *
 */

/* 
   Shaun Cooper
    January 2015
   
   David Kuntz
   Fall 2022
   Lab 9 GAS from AST
*/


	/* begin specs */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ast.h" //abstract syntax tree methods included
#include "symtable.h" //symbol table included
#include "emit.h" //emitter included

int yylex(); //warning fix

extern int lineindex;	//lineindex from lex

int level = 0; //static scope level counter for indentation
int offset = 0; //for memory allocation
int maxoffset; //offset limit
int goffset; //global offset
int gtemp = 0; //one up value for temp node

//global ast pointer
struct ASTnodetype * Program;

void yyerror (s)  /* Called by yyparse on error */
     char *s;
{
     printf("%s on line %d\n",s,lineindex); //prints line where syntax error occurred
}


%}
/*  defines the start symbol, what values come back from LEX and how the operators are associated  */
%start Program

%union	{
	int value;
	char* string;
	//AST node elements added
	struct ASTnodetype * node;
	enum AST_Decaf_Types type;
	enum AST_Operators operator;
}

/*DECAF TOKENS LIST*/
%token T_AND            //&&
%token T_ASSIGN         //=
%token T_BOOLTYPE       //bool
%token T_BREAK          //break
%token <string> T_CHARCONSTANT   //char_lit (see section on Character literals)
%token T_CONTINUE       //continue
%token T_DOT            //.
%token T_ELSE           //else
%token T_EQ             //==
%token T_EXTERN         //extern
%token <value> T_FALSE          //false
%token T_FOR            //for
%token T_FUNC           //func
%token T_GEQ            //>=
%token T_GT             //>
%token T_LT		//<	
%token <string> T_ID             //identifier (see section on Identifiers)
%token T_IF             //if
%token <value> T_INTCONSTANT    //int_lit (see section on Integer literals)
%token T_INTTYPE        //int
%token T_LEFTSHIFT      //<<
%token T_LEQ            //<=
%token T_NEQ            //!=
%token T_NULL           //null
%token T_OR             //||
%token T_PACKAGE        //package
%token T_RETURN         //return
%token T_RIGHTSHIFT     //>>
%token <string> T_STRINGCONSTANT //string_lit (see section on String literals)
%token T_STRINGTYPE     //string
%token <value> T_TRUE           //true
%token T_VAR            //var
%token T_VOID           //void
%token T_WHILE          //while

%type <node> Externs ExternDefn ExternTypeList ExternTypeList1 FieldDecl FieldDecls
%type <node> ArrayType MethodDecl MethodDecls TypeList TypeList1 Block
%type <node> VarDecl VarDecls Constant BoolConstant Statement Statements
%type <node> BreakStmt ReturnStmt Expr Term Additiveexpression Simpleexpression Factor
%type <node> WhileStmt IfStmt ContinueStmt AssignStmt Lvalue MethodCall
%type <node> MethodArgList MethodArgList1 MethodArg

%type <type> MethodType Type ExternType

%type <operator> Multop Addop Relop

%%	/* end specs, begin rules */

Program : Externs T_PACKAGE T_ID'{'FieldDecls MethodDecls'}'
	{//symbol check not required
	Program = ASTCreateNode(A_PROGRAM);
	Program->S1 = $1; //externs
	Program->S2 = ASTCreateNode(A_PACKAGE); //package
	Program->S2->name = $3; //name
	Program->S2->S1 = $5; //fielddecls
	Program->S2->S2 = $6; //methoddecls
	
	//check ID with symtable
	if(Search($3,level,0)!=NULL){
		yyerror("Duplicate extern definition");
		yyerror($3);
		exit(1);
	}
	//add package to symbol table
	//variable name, type, subtype, level, size, offset, formal parameters
	Program->symbol = Insert($3,A_Decaf_VOID,ID_Sub_Type_Package,level,0,0,NULL);
	}
	;
Externs :/*empty*/ {$$ = NULL;}
	| ExternDefn Externs 
	{$$ = $1;
	$$->next = $2;}
	;
ExternDefn : T_EXTERN T_FUNC T_ID '('ExternTypeList')' MethodType ';'
	{//check ID with symtable
	if(Search($3,level,0)!=NULL){
		yyerror("Duplicate extern definition");
		yyerror($3);
		exit(1);
	}
	$$=ASTCreateNode(A_EXTERN);
	$$->name = $3; //name
	$$->S1 = $5; //branch externtypelist
	$$->decl_type = $7; //method type
	//variable name, type, subtype, level, size, offset, formal parameters
	$$->symbol = Insert($3,$$->decl_type,ID_Sub_Type_Extern_Method,level,0,0,$5);
	}
	;
ExternTypeList	:/*empty*/ {$$=NULL;}
		| ExternTypeList1 {$$ = $1;}
		;
ExternTypeList1: ExternType 
	{$$ = ASTCreateNode(A_EXTERNTYPE);
	$$->decl_type = $1;
	}
	|ExternTypeList1 ',' ExternType
	{$$ = ASTCreateNode(A_EXTERNTYPE);
	$$->decl_type = $3;
	$$->next = $1;
	}
	;
FieldDecls :/*empty*/ {$$=NULL;}
	| FieldDecl FieldDecls
	{$$ = $1;
	$$->next = $2;}
	;
FieldDecl  : T_VAR T_ID Type ';'
	   {
	   //check ID with symbtab
	   if(Search($2,level,0) != NULL){
		yyerror("Variable name already used");
		yyerror($2);
		exit(1);
	   }
	   $$ = ASTCreateNode(A_VARDEC);
	   $$->name = $2;
	   $$->decl_type = $3;
	   $$->symbol = Insert($2,$$->decl_type,ID_Sub_Type_Scalar,level,1,offset,NULL);
	   offset += 1; //add to offset
	   }
	   
	   | T_VAR T_ID ArrayType ';'
	   {
	   //check ID with symbtab
	   if(Search($2,level,0) != NULL){
		yyerror("Variable name already used");
		yyerror($2);
		exit(1);
	   }
	   $$ = ASTCreateNode(A_VARDEC);
	   $$->name = $2;
	   $$->S1 = $3; //array size
	   $$->decl_type = $3->decl_type; //actual array type
	   $$->symbol = Insert($2,$$->decl_type,ID_Sub_Type_Array,level,$3->value,offset,NULL);
	   offset += $3->value; //allocate for array size
	   }
	   
	   | T_VAR T_ID Type T_ASSIGN Constant ';' 
	   {
	   //check ID with symbtab
	   if(Search($2,level,0) != NULL){
		yyerror("Variable name already used");
		yyerror($2);
		exit(1);
	   }
	   $$ = ASTCreateNode(A_VARDEC);
	   $$->name = $2;
	   $$->decl_type = $3;
	   $$->S2 = $5; //constant value assigned
	   //variable name, type, subtype, level, size, offset, formal parameters
	   $$->symbol = Insert($2,$$->decl_type,ID_Sub_Type_Scalar,level,1,offset,NULL);
	   offset += 1;
	   }
	   ;
MethodDecls :/*empty*/ {$$=NULL;}
	| MethodDecl MethodDecls
	{$$ = $1;
	$$->next = $2;}
	;
MethodDecl : T_FUNC T_ID {goffset = offset; offset = 0; maxoffset = offset;}
		'('TypeList')' MethodType
	   {
	   //check ID with symbtab before we get to block
	   if(Search($2,level,0) != NULL){
			yyerror("Duplicate method name");
			yyerror($2);
			exit(1);
	   	}
	   Insert($2,$7,ID_Sub_Type_Method,level,0,0,$5);
	   }
	   Block
	   {
	   $$ = ASTCreateNode(A_METHODDEC);
	   $$->name = $2;
	   $$->decl_type = $7; //method type
	   $$->S1 = $5; //typelist
	   $$->S2 = $9; //block
	   $$->symbol = Search($2,level,0);
	   $$->size = maxoffset; //size of method
	   $$->symbol->mysize = maxoffset;
	   offset = goffset;
	   }
	   ;
TypeList:/*empty*/ {$$=NULL;}
	| TypeList1
	{$$ = $1;}
	;
TypeList1: T_ID Type
	{
	//check ID with symbtab
	if(Search($1,level+1,0) != NULL){
		yyerror("Duplicate method arg name");
		yyerror($1);
		exit(1);
	}
	$$ = ASTCreateNode(A_PARAM); 
	$$->name = $1;
	$$->decl_type = $2;
	$$->symbol = Insert($1,$$->decl_type,ID_Sub_Type_Scalar,level+1,offset,0,NULL);
	offset += 1;
	}
	
	|T_ID Type ',' TypeList1
	{
	//check ID with symbtab
	if(Search($1,level+1,0) != NULL){
		yyerror("Duplicate method arg name");
		yyerror($1);
		exit(1);
	}
	$$ = ASTCreateNode(A_PARAM);
	$$->name = $1;
	$$->decl_type = $2;
	$$->next = $4;
	$$->symbol = Insert($1,$$->decl_type,ID_Sub_Type_Scalar,level+1,1,offset,NULL);
	offset += 1;
	}
	;
	
Block : '{' {level++;} VarDecls Statements '}'
	{$$ = ASTCreateNode(A_BLOCK);
	$$->S1 = $3; //variable declarations
	$$->S2 = $4; //statements
	//display all symbols used in block
	if(mydebug == 1) Display();
	//apply maxoffset limit
	if(offset > maxoffset) maxoffset = offset;
	//delete nodes when leaving block
	offset -= Delete(level);
	level--;
	}
	;

VarDecls :/*empty*/ {$$=NULL;}
	| VarDecl VarDecls
	{$$ = $1;
	$$->next = $2;}
	;
VarDecl : T_VAR T_ID Type';' 
	{
	//check ID with symbtab
	if(Search($2,level,0) != NULL){
		yyerror("Variable already defined at this level");
		yyerror($2);
		exit(1);
	}
	$$ = ASTCreateNode(A_VARDEC);
	$$->name = $2;
	$$->decl_type = $3;
	$$->symbol = Insert($2,$$->decl_type,ID_Sub_Type_Scalar,level,1,offset,NULL);
	offset += 1;
	}
	
	| T_VAR T_ID ArrayType';'  
	{
	//check ID with symbtab
	if(Search($2,level,0) != NULL){
		yyerror("Variable already defined at this level");
		yyerror($2);
		exit(1);
	}
	$$ = ASTCreateNode(A_VARDEC);
	$$->name = $2;
	$$->S1 = $3; //array size
	$$->decl_type = $3->decl_type; //array type
	//pass array size into symbtab
	//variable name, type, subtype, level, size, offset, formal parameters
	$$->symbol = Insert($2,$$->decl_type,ID_Sub_Type_Array,level,$3->value,offset,NULL); 
	offset += $3->value; //allocate for array
	}
	;
Statements :/*empty*/ {$$=NULL;}
	| Statement Statements
	{$$ = $1;
	$$->next = $2;}
	;
	
Statement : Block {$$ = $1;}
	| AssignStmt ';' {$$ = $1;}
	| MethodCall ';' {$$ = $1;}
	| IfStmt {$$ = $1;} 
	| WhileStmt {$$ = $1;} 
	| ReturnStmt {$$ = $1;} 
	| BreakStmt {$$ = $1;} 
	| ContinueStmt {$$ = $1;}
	;
AssignStmt: Lvalue T_ASSIGN Expr 
	  {
	  //check if both sides of T_ASSIGN are the same
	  if($1->decl_type != $3->decl_type){
	  	yyerror("Type mismatch on assignment");
	  	exit(1);
	  }
	  $$ = ASTCreateNode(A_ASSIGNSTMT);
	  $$->S1 = $1; //lefthand value
	  $$->S2 = $3; //expression
	  $$->decl_type = $1->decl_type; //set declared type of assignment
	  
	  //create temp value
	  $$->name = CREATE_TEMP(); //temp value name
	  //variable name, type, subtype, level, size, offset, formal parameters
	  $$->symbol = Insert($$->name,$$->decl_type,ID_Sub_Type_Scalar,level,1,offset,NULL);
	  offset++;
	  }
	  ;
Lvalue    : T_ID 
	  {
	  //check ID with symbtab
	  struct SymbTab *p;
	  p = Search($1,level,1);
	  if(p == NULL){ //not defined
		yyerror("Left variable not defined");
		yyerror($1);
		exit(1);
	  }
	  //check if subtype is scalar
	  if(p->SubType != ID_Sub_Type_Scalar){
	  	yyerror("Invalid subtype; left value is not scalar");
		yyerror($1);
		exit(1);
	  }
	  $$ = ASTCreateNode(A_VAR_LVAL);
	  $$->name = $1;
	  $$->S1 = NULL; //no array index/reference
	  $$->symbol = p; 
	  $$->decl_type = p->Type;
	  }
	  
	  | T_ID '[' Expr ']' 
	  {
	  //check ID with symbtab
	  struct SymbTab *p;
	  p = Search($1,level,1);
	  if(p == NULL){ //not defined
		yyerror("Left variable not defined");
		yyerror($1);
		exit(1);
	  }
	  //check if subtype is array
	  if(p->SubType != ID_Sub_Type_Array){
	  	yyerror("Invalid subtype; left value is not array");
		yyerror($1);
		exit(1);
	  }
	  $$ = ASTCreateNode(A_VAR_LVAL);
	  $$->name = $1;
	  $$->S1 = $3; //array index/reference
	  $$->symbol = p;
	  $$->decl_type = p->Type;
	  }
	  ;

MethodCall : T_ID '('MethodArgList')' 
	   {
	   //check ID with symbtab
	   struct SymbTab *p;
	   p = Search($1,level,1);
	   if(p == NULL){ //not defined
		yyerror("Method called not defined");
		yyerror($1);
		exit(1);
	   }
	   //check if subtype is method
	   if((p->SubType != ID_Sub_Type_Method)&&(p->SubType != ID_Sub_Type_Extern_Method)){
	  	yyerror("Invalid subtype; value is not a method");
		yyerror($1);
		exit(1);
	   }
	   //check parameters
	   if(checkParam(p->fparms, $3)==0){ //mismatch
	   	yyerror("Formal/actual parameter mismatch");
		yyerror($1);
		exit(1);
	   }
	   $$ = ASTCreateNode(A_METHODCALL);
	   $$->name = $1;
	   $$->S1 = $3; //method arguments
	   $$->symbol = p;
	   $$->decl_type = p->Type;
	   }
	   ;
MethodArgList:/*empty*/ {$$=NULL;}
	| MethodArgList1 {$$ = $1;}
	;
MethodArgList1: MethodArg {$$ = $1;}
	|MethodArg ',' MethodArgList1
	{$$ = $1;
	$$->next = $3;}
	;
MethodArg  : Expr 
		{
		$$ = ASTCreateNode(A_METHODARG);
		$$->S1 = $1; //branch
		$$->decl_type = $1->decl_type;
		
		//create temp value
	 	$$->name = CREATE_TEMP(); //temp value name
	 	//variable name, type, subtype, level, size, offset, formal parameters
	 	$$->symbol = Insert($$->name,$$->decl_type,ID_Sub_Type_Scalar,level,1,offset,NULL);
	 	offset++;
		}
		
	   | T_STRINGCONSTANT 
	   	{
	   	$$ = ASTCreateNode(A_METHODARG);
	   	$$->S1 = ASTCreateNode(A_CONSTANT_STRING); 
	   	$$->S1->name = $1; 
	   	$$->S1->label = genlabel();
	   	$$->S1->decl_type = A_Decaf_STRING;
	   	$$->decl_type = $$->S1->decl_type; 
	   
	   	//create temp value
	 	$$->name = CREATE_TEMP(); //temp value name
	 	//variable name, type, subtype, level, size, offset, formal parameters
	 	$$->symbol = Insert($$->name,A_Decaf_STRING,ID_Sub_Type_Scalar,level,1,offset,NULL);
	 	offset++;
	   }
	   ;
IfStmt :  T_IF '(' Expr ')' Block T_ELSE Block
	  {$$ = ASTCreateNode(A_IFSTMT);
	  $$->S1 = $3; //expr
	  $$->S2 = ASTCreateNode(A_ELSESTMT); //else
	  $$->S2->S1 = $5; //if block
	  $$->S2->S2 = $7; //else block
	  } 
	  | T_IF '(' Expr ')' Block 
	  {$$ = ASTCreateNode(A_IFSTMT);
	  $$->S1 = $3; //expr
	  $$->S2 = ASTCreateNode(A_ELSESTMT); //else
	  $$->S2->S1 = $5; //if block
	  $$->S2->S2 = NULL; //no else block
	  } 
	  ;
	  
WhileStmt : T_WHILE '(' Expr ')' Block 
	  {$$ = ASTCreateNode(A_WHILESTMT);
	  $$->S1 = $3; //expr
	  $$->S2 = $5; //block
	  }
	  ;
ReturnStmt : T_RETURN ';' 
		{$$ = ASTCreateNode(A_RETURN);}
		| T_RETURN '(' ')' ';' 
		{$$ = ASTCreateNode(A_RETURN);}
	  	| T_RETURN '(' Expr ')' ';' {
	  	$$ = ASTCreateNode(A_RETURN);
	  	$$->S1 = $3; //return expression
	  	}
	  	;
BreakStmt : T_BREAK ';' {$$ = ASTCreateNode(A_BREAK);}
	  ;
ContinueStmt : T_CONTINUE ';' {$$ = ASTCreateNode(A_CONTINUE);};


Expr : Simpleexpression {$$ = $1;};

Simpleexpression : Additiveexpression {$$ = $1;}
                | Simpleexpression Relop Additiveexpression
		{
		//check if both sides of relop are the same
		if(($1->decl_type != $3->decl_type)){
	 		yyerror("Type mismatch on simple expression");
	 		exit(1);
	 	}
		$$ = ASTCreateNode(A_EXPR);
		$$->S1 = $1; //simple expr
		$$->operator = $2;
		$$->decl_type = A_Decaf_BOOL; //returns true or false
		$$->S2 = $3; //addi expr
		
		//create temp value
	 	$$->name = CREATE_TEMP(); //temp value name
	 	//variable name, type, subtype, level, size, offset, formal parameters
	 	$$->symbol = Insert($$->name,$$->decl_type,ID_Sub_Type_Scalar,level,1,offset,NULL);
	 	offset++;
		}
                  ;
Relop : T_LEQ {$$ = A_LEQ;}
	  | T_LT {$$ = A_LT;}
	  | T_GT {$$ = A_GT;}
	  | T_GEQ {$$ = A_GEQ;}
	  | T_EQ {$$ = A_EQ;}
	  | T_NEQ {$$ = A_NEQ;}
      ;
Additiveexpression : Term {$$ = $1;}
                    | Additiveexpression Addop Term
			{
			//check if both sides of addop are integers
			if(($1->decl_type != $3->decl_type)||
			   ($1->decl_type != A_Decaf_INT)){
	 			yyerror("Type mismatch on add/sub expr");
	 			exit(1);
	 		}
			$$ = ASTCreateNode(A_EXPR);
			$$->S1 = $1;
			$$->operator = $2;
			$$->S2 = $3;
			$$->decl_type = A_Decaf_INT;
			
			//create temp value
	 		$$->name = CREATE_TEMP(); //temp value name
	 		//variable name, type, subtype, level, size, offset, formal parameters
	 		$$->symbol = Insert($$->name,$$->decl_type,ID_Sub_Type_Scalar,level,1,offset,NULL);
	 		offset++;
			}
                    ;
Addop : '+' {$$ = A_PLUS;}
	  | '-' {$$ = A_MINUS;}
      ;
Term : Factor {$$ = $1;}
     | Term Multop Factor
	 {
	 //check for term factor mismatch
	 if($1->decl_type != $3->decl_type){
	 	yyerror("Type mismatch on multop");
	 	exit(1);
	 }
	 //check for arith operator type mismatch
	 if(($1->decl_type == A_Decaf_BOOL) && 
	 (($2 == A_TIMES) ||
	 ($2 == A_DIVIDE) ||
	 ($2 == A_MOD))){
	 	yyerror("Cannot use booleans in arithmetic operations");
	 	exit(1);
	 }
	 //check for bool operator type mismatch
	 if(($1->decl_type == A_Decaf_INT) && 
	 (($2 == A_AND) ||
	 ($2 == A_OR) ||
	 ($2 == A_LSHIFT) ||
	 ($2 == A_RSHIFT))){
	 	yyerror("Cannot use integers in boolean operations");
	 	exit(1);
	 }
	 $$ = ASTCreateNode(A_EXPR);
	 $$->S1 = $1;
	 $$->operator = $2;
	 $$->decl_type = $1->decl_type;
	 $$->S2 = $3;
	 
	 //create temp value
	 $$->name = CREATE_TEMP(); //temp value name
	 //variable name, type, subtype, level, size, offset, formal parameters
	 $$->symbol = Insert($$->name,$$->decl_type,ID_Sub_Type_Scalar,level,1,offset,NULL);
	 offset++;
	 }
     ;
Multop :  '*' {$$ = A_TIMES;}
		| '/' {$$ = A_DIVIDE;}
		| '%' {$$ = A_MOD;}
		| T_AND {$$ = A_AND;}
		| T_OR {$$ = A_OR;}
		| T_LEFTSHIFT {$$ = A_LSHIFT;}
		| T_RIGHTSHIFT {$$ = A_RSHIFT;}
    	;
Factor : MethodCall {$$ = $1;}

     |T_ID
	 {
	 //check ID with symbtab
	  struct SymbTab *p;
	  p = Search($1,level,1);
	  if(p == NULL){ //not defined
		yyerror("Right variable not defined");
		yyerror($1);
		exit(1);
	  }
	  //check if subtype is scalar
	  if(p->SubType != ID_Sub_Type_Scalar){
	  	yyerror("Invalid subtype; right value is not scalar");
		yyerror($1);
		exit(1);
	  }
	 $$ = ASTCreateNode(A_VAR_RVAL);
	 $$->name = $1;
	 $$->symbol = p;
	 $$->decl_type = p->Type;
	 }
     
     | T_ID '[' Expr ']'
	 {
	 //check ID with symbtab
	  struct SymbTab *p;
	  p = Search($1,level,1);
	  if(p == NULL){ //not defined
		yyerror("Right value not defined");
		yyerror($1);
		exit(1);
	  }
	  //check if subtype is array
	  if(p->SubType != ID_Sub_Type_Array){
	  	yyerror("Invalid subtype; right value is not array");
		yyerror($1);
		exit(1);
	  }
	 $$ = ASTCreateNode(A_VAR_RVAL);
	 $$->name = $1;
	 $$->S1 = $3; //array size
	 $$->symbol = p;
	 $$->decl_type = p->Type;
	 }
     | Constant {$$ = $1;}
     | '(' Expr ')' {$$ = $2;}
     | '!' Factor
     	 {
     	 //check variable type
	 if($2->decl_type != A_Decaf_BOOL){
	 	yyerror("Type mismatch; expected boolean");
	 	exit(1);
	 }
	 $$ = ASTCreateNode(A_EXPR);
	 $$->operator = A_NOT;
	 $$->S1 = $2;
	 $$->decl_type = A_Decaf_BOOL;
	 
	 //create temp value
	 $$->name = CREATE_TEMP(); //temp value name
	 //variable name, type, subtype, level, size, offset, formal parameters
	 $$->symbol = Insert($$->name,$$->decl_type,ID_Sub_Type_Scalar,level,1,offset,NULL);
	 offset++;
	 }
     | '-' Factor
	 {
     	 //check variable type
	 if($2->decl_type != A_Decaf_INT){
	 	yyerror("Type mismatch; expected integer");
	 	exit(1);
	 }
	 $$ = ASTCreateNode(A_EXPR);
	 $$->operator = A_UMINUS;
	 $$->S1 = $2;
	 $$->decl_type = A_Decaf_INT;
	 
	 //create temp value
	 $$->name = CREATE_TEMP(); //temp value name
	 //variable name, type, subtype, level, size, offset, formal parameters
	 $$->symbol = Insert($$->name,$$->decl_type,ID_Sub_Type_Scalar,level,1,offset,NULL);
	 offset++;
	 }
       ;


ExternType : T_STRINGTYPE {$$ = A_Decaf_STRING;}
	   | Type {$$ = $1;}
	   ;
Type : T_INTTYPE {$$ = A_Decaf_INT;}
     | T_BOOLTYPE {$$ = A_Decaf_BOOL;}
     ;
MethodType : T_VOID {$$ = A_Decaf_VOID;}
	   | Type {$$ = $1;}
	   ;
BoolConstant : T_TRUE {
	 	$$ = ASTCreateNode(A_CONSTANT_BOOL);
	 	$$->value = 1;
	 	$$->decl_type = A_Decaf_BOOL;
	 	}
	     | T_FALSE {
	 	$$ = ASTCreateNode(A_CONSTANT_BOOL);
	 	$$->value = 0;
	 	$$->decl_type = A_Decaf_BOOL;
	 };
ArrayType : '[' Expr ']' Type //changed array to handle expressions
	  {$$ = ASTCreateNode(A_ARRAYTYPE);
	  $$->value = $2->value; //size of array
	  $$->decl_type = $4; //type of array
	  }
	  ;
Constant : T_INTCONSTANT {
	 $$ = ASTCreateNode(A_CONSTANT_INT);
	 $$->value = $1;
	 $$->decl_type = A_Decaf_INT;
	 }
	 | BoolConstant {$$ = $1;};


%%	/* end of rules, start of program */

//main method lab9 overhaul
int main(int argc, char * argv[]){ 

int i=1; //line increment count
FILE *fp; //file pointer
char s[100]; //char holder

while(i<argc){ //process input line
	if(strcmp(argv[i],"-d")==0){ //debug flag
		mydebug = 1; //activate debug
	}
	if(strcmp(argv[i],"-o")==0){
		strcpy(s,argv[i+1]); //copy argv into temp
		strcat(s,".s"); //append assembly file extension
		//attempt to open file
		if((fp=fopen(s,"w"))==NULL){ //no file
			printf("cannot open file");
			exit(1);
		}
	}//of if
	i++;
}//while

yyparse();

if(mydebug==1){ //debug flag
	printf("\nAll symbols: \n");
	Display(); //display symbol table at global level
	printf("Parsing complete.\n\n");
	//at this point Program points to ast
	ASTprint(0,Program); 
	printf("\nAbstract syntax tree print complete\n");
}

//lab9 critical method
EMIT(Program, fp);

}


