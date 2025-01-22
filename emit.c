//  Subprograms which output GAS code.
//
//   Shaun Cooper Spring 2015
/// Updated Spring 2017 to utilize the data segement portion of MIPS instead of advancing the Stack and Global pointers
/// to accomodate global variables.
//  Shaun Cooper December 2020 updated for DECAF format

//   David Kuntz
//   Fall 2022
//   Lab 9 GAS from AST
//	Adapted for GAS instead of MIPS
//	~scooper/lab9 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> //required to do math :'(

#include "ast.h"
#include "emit.h"


char * CURRENT_FUNC; //need to know Current Function for return on main


// A data structure linked list which allows to keep track of current
// labels for break and continue
struct LabelList {
	char *s;
	struct LabelList *next;
};

// global variables to maintain break and continue targets
struct LabelList *BreakList = NULL;
struct LabelList *ContinueList = NULL;


// used to maintain the containeing FOR/WHILE labels
//pre: pointer l is not null
//post: label list node pushed to break list stack
struct LabelList * PushLabelBreakList(char *l)
{  	//create new node
	struct LabelList *myNode;
	//allocate space for new node
	myNode = (struct LabelList*)malloc(sizeof(struct LabelList)); 
	myNode->s = l; //assign label value
	myNode->next = BreakList; //next connect to break list
	return myNode; 
}// of PushLabelBreakList


// used to maintain the containeing FOR/WHILE labels
//pre: pointer l is not null
//post: label list node pushed to continue list stack
struct LabelList * PushLabelContinueList(char *l)
{  	//create new node
	struct LabelList *myNode;
	//allocate space for new node
	myNode = (struct LabelList*)malloc(sizeof(struct LabelList)); 
	myNode->s = l; //assign label value
	myNode->next = ContinueList; //next connect to continue list
	return myNode;
}// of PushLabelContinueList


// remove inner most Break label
//pre: break stack list is not empty
//post: first node in list is popped
void PopLabelBreakList()
{  
	BreakList = BreakList->next;
}


// remove inner most continue lable
//pre: continue stack list is not empty
//post: first node in list is popped
void PopLabelContinueList()
{  
	ContinueList = ContinueList->next;
}


static int GLABEL=0;  // global label counter

//creates one_up labels of the form _L%d
//  PRE:  none
//  POST:  new copy of string of the form _L% and GLABEL is incremented
char * genlabel()
{  char s[100]; //char holder
   //char *S1; //char pointer?
   sprintf(s,"_L%d",GLABEL);
   GLABEL += 1;
   return(strdup(s));
}

//------------------------------------BEGIN EMITTER METHODS------------------------------------


//pre: ast and file pointers are not null
//post: GAS code for global variables to be placed in the .data section
void Emit_Globals(ASTnode *p, FILE *fp)
{
	char s[100]; //char holder

	//if pointer is null get out
	if (p==NULL) return;
	
	//check for vardec at level 0
  	if((p->type == A_VARDEC)&&(p->symbol->level == 0)){
  		//check if variable is initialized or not
  		if(p->S2==NULL){ //not initialized global var
  			sprintf(s,".comm %s,%d,%d",p->name,p->symbol->mysize*WSIZE,p->symbol->mysize*WSIZE);
  			emit(fp,"",s,"global variable defined");
  			emit(fp,"",".align 8","");
  		}else{ //initialized global var
  			sprintf(s,".long %d ",p->S2->value);
  			char l[100]; //char holder for label
  			emit(fp,p->name,s,"initialized global variable defined");
  			emit(fp,"",".align 8","");
  		}
  	}
  	
  	//navigate the whole tree
  	Emit_Globals(p->S1, fp);
  	Emit_Globals(p->S2, fp);
	Emit_Globals(p->next, fp);
}


// helper routine which outputs label, action, and comment lines
//pre: fp is not null; label, action, comment pointers can be empty
//post: output formatted to proper GAS code syntax
void emit( FILE *fp, char *label, char *action, char *comment){

 char s[100]; //char holder
 
 //all permutations of label, action, and comments
 if(strcmp(label,"")==0){
 	if(strcmp(comment,"")==0){
 		if(strcmp(action,"")==0){ 
 			fprintf(fp,"\n");			//nothing
 		}else{ 
 			fprintf(fp,"\t%s\n",action);		//action
 		}
 	}else if(strcmp(action,"")==0){ 
 		fprintf(fp,"\t\t# %s\n",comment);			//comment
 	}else{ 
 		fprintf(fp,"\t%s\t\t# %s\n",action,comment);	//comment action
 	}
 }
 //label
 else if(strcmp(comment,"")==0){
 	if(strcmp(action,"")==0){ 
 			fprintf(fp,"%s:\t\n",label);		//label
 		}else{ 
 			fprintf(fp,"%s:\t%s\n",label,action);	//label action
 		}
 }
 //label comment
 else if(strcmp(action,"")==0){ 
 		fprintf(fp,"%s:\t# %s\n",label,comment);		//label comment
 }else{ 
 	fprintf(fp,"%s:\t%s\t\t# %s\n",label,action,comment);	//label action comment
 }

}//of emit


// Given a PTR to a function call, handle it

//pre: pointers not null, p points to method call
//post: GAS code for method calls, %rax is set by the method
void Emit_Method_Call(ASTnode *p, FILE *fp){
	char s[100]; //char holder

	//check if its a method call
	if(p->type == A_METHODCALL){
		//check if external or internal method
		if(p->symbol->SubType == ID_Sub_Type_Extern_Method){
			Emit_Extern_Method_Call(p,fp);
			return;
		}else{//its an internal method
			//carve out space for method call
			//sprintf(s,"subq $%d, %rsp",p->symbol->mysize*WSIZE);
        		//emit(fp,"",s,"carve out space for method call");
        		
        		//assign values to actual parameters
        		ASTnode *params = p->S1;
        		//run through argument list
        		while(params != NULL){
        			emit_expr(params->S1,fp); //get value
				sprintf(s,"mov %%rax, %d(%%rsp)",params->symbol->offset*WSIZE);
				emit(fp,"",s,"stor arg value in our runtime stack");
				params = params->next;
			} 
			//reassign params
			params = p->S1;
			int count = WSIZE;
			//put arguments in parameter registers
			while(params != NULL){
				sprintf(s,"mov %d(%%rsp), %%rax",params->symbol->offset*WSIZE);
				emit(fp,"",s,"load argument into RAX");
				sprintf(s,"mov %%rax,  %%r%d",count); //backward?
				emit(fp,"",s,"load argument into RAX");
				count++;
				params = params->next;
			} 
			//call the function
			sprintf(s,"CALL %s",p->name);
			emit(fp,"",s,"call to function");
			fprintf(fp,"\n");
		};
	}
}

//pre: ast and file pointers are not null
//post: GAS code for extern method calls, %rax is set by the method
void Emit_Extern_Method_Call(ASTnode *p, FILE *fp)
{
  char s[100]; //char holder
  
   //check for "print_string"  
   if (strcmp(p->name,"print_string") == 0)
   {
	sprintf(s,"mov $%s, %%rdi", p->S1->S1->label);
        emit(fp,"",s,"RDI is the label address");
        emit(fp,"","mov $0, %%rax", "RAX needs to be zero");
	emit(fp,"","call printf","print a string");
	fprintf(fp,"\n");
        return;
   } 
   
   //check for "print_int"
   if (strcmp(p->name,"print_int") == 0)
   {
   	emit_expr(p->S1->S1,fp); //mov value to %rax

	emit(fp,"","mov %%rax, %%rsi","RSI needs the value to print");
	emit(fp,"","mov $percentD, %%rdi","RDI needs to be the int format");
	emit(fp,"","mov $0, %%rax","RAX needs to be 0");
	emit(fp,"","call printf","print a number from expression");
	fprintf(fp,"\n");
	return;
   }
   
   //check for "read_int"
   if (strcmp(p->name,"read_int") == 0)
   {
	
	emit(fp,"","mov $_SCANFHOLD, %%rsi","read in a number");
	emit(fp,"","mov $percentD , %%rdi","rdi has integer format for scanf");
	emit(fp,"","mov $0 , %%rax","No other parameters for scanf");
	emit(fp,"","call  __isoc99_scanf","call read");
	emit(fp,"","mov _SCANFHOLD, %%rax","bring value on STACK into RAX register for default value");
	
	fprintf(fp,"\n");
	return;
   }
   
    // any other extern method calls are not handled
   fprintf(stderr,"Extern Method %s is unknown\n",p->name);	
	
} //of emit extern method call


//pre: ast and file pointers are not null
//post: GAS code that sets %rax to the result of an expression
void emit_expr(ASTnode *p, FILE *fp){

   char s[100]; //char holder

//check if expression value is constant, predefined variable, or method call
switch (p->type) { 
	case A_CONSTANT_INT:
		sprintf(s,"mov $%d, %%rax",p->value);
		emit(fp,"",s,"expr load number");
		return;
		break;
	case A_CONSTANT_BOOL:
		sprintf(s,"mov $%d, %%rax",p->value);
		emit(fp,"",s,"expr load bool");
		return;
		break;
	case A_METHODCALL:
		Emit_Method_Call(p,fp);
		return;
		break;
	case A_VAR_RVAL:
		emit_ident(p,fp); //rax has memory address
		emit(fp,"","mov (%%rax), %%rax","read in memory value from rhs");
		return;
		break;
	default: break; //not initialized; we have to do math now :'(
} //of switch for base cases

// we have an expression whose type and value need to be synthesized

     emit_expr(p->S1,fp);     //evaluate the lhs
     
     //if there is a rhs, push rax to stack
     if(p->S2!=NULL){
     	sprintf(s,"mov %%rax, %d(%%rsp)",p->symbol->offset*WSIZE);
     	emit(fp,"",s,"STORE LHS of expression to memory"); 
     	emit_expr(p->S2,fp);     //evaluate rhs
     	//mov rax to rbx
     	emit(fp,"","mov %%rax, %%rbx","right hand side needs to be set");
     	//restore lhs to rax
     	sprintf(s,"mov %d(%%rsp), %%rax",p->symbol->offset*WSIZE);
     	emit(fp,"",s,"FETCH LHS of expression from memory"); 
     }
     
//now execute the specified operation in GAS
    switch(p->operator) {
    		/*--integer math--*/
   		case A_PLUS : 
	              emit(fp,"","add %%rbx, %%rax", "EXPR ADD");
	              fprintf(fp,"\n");
                      break;
                case A_MINUS : 
	              emit(fp,"","sub %%rbx, %%rax", "EXPR SUBTRACT");
	              fprintf(fp,"\n");
                      break; 
		case A_TIMES : 
	              emit(fp,"","imul %%rbx, %%rax", "EXPR MULT RAX has lower word");
	              fprintf(fp,"\n");
                      break;  
                case A_DIVIDE: 
                      emit(fp,"","mov $0,%%rdx","EXPR upper word 0 DIV");
	              emit(fp,"","idiv %%rbx", "EXPR DIV");
	              fprintf(fp,"\n");
                      break; 
                case A_MOD: 
                      emit(fp,"","mov $0,%%rdx","EXPR upper word 0 DIV");
	              emit(fp,"","idiv %%rbx", "EXPR MOD");
	              emit(fp,"","mov %%rdx, %%rax", "MOD needs to be in RAX");
	              fprintf(fp,"\n");
                      break;
                case A_UMINUS: //multiply by -1 haha
                	//rax is S1 value
                	emit(fp,"","mov $-1, %%rbx", "set rbx to -1");//set rbx to -1
                	emit(fp,"","imul %%rbx, %%rax", "EXPR UMINUS");//multiply by -1
                	fprintf(fp,"\n");
                      break;
                /*--boolean math--*/
                case A_NOT: //set a reg to 1 and xor
                	//rax is S1 value
                	emit(fp,"","mov $1, %%rbx", "set rbx to 1");//set rbx to 1
                	emit(fp,"","xor %%rbx, %%rax", "EXPR NOT");//flip it
                	fprintf(fp,"\n");
                      break;
                case A_AND : 
	              emit(fp,"","and %%rbx, %%rax", "EXPR AND");
	              fprintf(fp,"\n");
                      break;
                case A_OR : 
	              emit(fp,"","or %%rbx, %%rax", "EXPR OR");
	              fprintf(fp,"\n");
                      break; 
                case A_LT : 
	              emit(fp,"","cmp %%rbx, %%rax", "EXPR LESSTHAN");
	              emit(fp,"","setl %al", "EXPR LESSTHAN");
	              emit(fp,"","mov $1,%%rbx", "set rbx to one to filter rax");
	              emit(fp,"","and %%rbx, %%rax", "filter RAX");
	              fprintf(fp,"\n");
                      break;   
                case A_GT : 
	              emit(fp,"","cmp %%rbx, %%rax", "EXPR GREATERTHAN");
	              emit(fp,"","setg %al", "EXPR GREATERTHAN");
	              emit(fp,"","mov $1,%%rbx", "set rbx to one to filter rax");
	              emit(fp,"","and %%rbx, %%rax", "filter RAX");
	              fprintf(fp,"\n");
                      break; 
                case A_LEQ : 
	              emit(fp,"","cmp %%rbx, %%rax", "EXPR LESSEQUAL");
	              emit(fp,"","setle %al", "EXPR LESSEQUAL");
	              emit(fp,"","mov $1,%%rbx", "set rbx to one to filter rax");
	              emit(fp,"","and %%rbx, %%rax", "filter RAX");
	              fprintf(fp,"\n");
                      break;   
                case A_GEQ : 
	              emit(fp,"","cmp %%rbx, %%rax", "EXPR GREATEREQUAL");
	              emit(fp,"","setge %al", "EXPR GREATEREQUAL");
	              emit(fp,"","mov $1,%%rbx", "set rbx to one to filter rax");
	              emit(fp,"","and %%rbx, %%rax", "filter RAX");
	              fprintf(fp,"\n");
                      break;
                case A_NEQ : 
	              emit(fp,"","cmp %%rbx, %%rax", "EXPR NOTEQUAL");
	              emit(fp,"","setne %al", "EXPR NOTEQUAL");
	              emit(fp,"","mov $1,%%rbx", "set rbx to one to filter rax");
	              emit(fp,"","and %%rbx, %%rax", "filter RAX");
	              fprintf(fp,"\n");
                      break;   
                case A_EQ : 
	              emit(fp,"","cmp %%rbx, %%rax", "EXPR EQUAL");
	              emit(fp,"","sete %al", "EXPR EQUAL");
	              emit(fp,"","mov $1,%%rbx", "set rbx to one to filter rax");
	              emit(fp,"","and %%rbx, %%rax", "filter RAX");
	              fprintf(fp,"\n");
                      break;
		default: printf("im scared and confused and shaking and crying\n");
			break;
         }
}//of emit expr


//routine to do an assignment statement
//pre: pointers not null, p points to assign
//post: GAS code that handles the assignment
void emit_assign(ASTnode *p, FILE *fp){
	
	char s[100]; //char holder

	emit_expr(p->S2,fp); //get rhs value
	//push value in stack
	sprintf(s,"mov %rax, %d(%rsp)",p->symbol->offset*WSIZE);
	emit(fp,"",s,"store RHS value into memory");
	//get lhs id
	emit_ident(p->S1,fp); 
	//pop value into rbx
	sprintf(s,"mov %d(%rsp), %rbx",p->symbol->offset*WSIZE);
	emit(fp,"",s,"get RHS stored value");
	emit(fp,"","mov %rbx, (%rax)","assignstmt final store");//store extracted value in proper memory address
	fprintf(fp,"\n");
}


//pre: pointers are not null, p points to a variable id
//post: GAS code to set %rax to address of id
void emit_ident(ASTnode *p, FILE *fp)
{
     char s[100]; //char holder
     
	//check for array
    if (p->S1 == NULL){   // we have a straight identifier 
	  //check if id is global or local
     	if (p->symbol->level == 0 ){ //global
     		sprintf(s,"mov $%s, %%rax",p->symbol->name); //directly from heap
      		emit(fp,"",s,"Load Global variable address into RAX");
      		fprintf(fp,"\n");
      		
    	}else{ //local
		sprintf(s,"mov $%d, %%rax",p->symbol->offset*WSIZE); //from stack pointer + offset
		emit(fp,"",s,"Load Local variable offset into RAX");
		emit(fp,"","add %rsp, %rax","Add stack pointer to RAX");
		fprintf(fp,"\n");
    	}
    }else{   // this is an array
	  //check if id is global or local
     	if (p->symbol->level == 0 ){ //global
     		emit_expr(p->S1,fp); //get index value
		emit(fp,"","mov %rax, %rbx","store array index in RBX");
		//shl $3 is the same as multiplying by 2^3 or 8, which is WSIZE
		//to accomodate any WSIZE value, we need to use logarithms
		//we have to do math :"()
		sprintf(s,"shl $%.0f, %rbx",log(WSIZE)/log(LOGWSIZE)); //get correct properly formatted float shift value 
		emit(fp,"",s,"multiply wordsize for array reference");

		sprintf(s,"mov $%s, %%rax",p->symbol->name); //directly from heap
      		emit(fp,"",s,"Load Global variable address into RAX");
		emit(fp,"","add %rbx, %rax","move add on %rbx as this is an array reference");
		fprintf(fp,"\n");
		
    	}else{ //local
		emit_expr(p->S1,fp); //get index value
		emit(fp,"","mov %rax, %rbx","store array index in RBX");
		//shl $3 is the same as multiplying by 2^3 or 8, which is WSIZE
		//to accomodate any WSIZE value, we need to use logarithms
		//we have to do math :"()
		sprintf(s,"shl $%.0f, %rbx",log(WSIZE)/log(LOGWSIZE)); //get correct properly formatted float shift value 
		emit(fp,"",s,"multiply wordsize for array reference");

		sprintf(s,"mov $%d, %%rax",p->symbol->offset*WSIZE); //from stack pointer + offset
		emit(fp,"",s,"Load Local variable offset into RAX");
		emit(fp,"","add %rsp, %rax","Add stack pointer to RAX");
		emit(fp,"","add %rbx, %rax","move add on %rbx as this is an array reference");
		fprintf(fp,"\n");
    	}
       
    }
       
} // of function emit_ident


//pre: pointers not null, p points to if statement
//post: GAS code that handles an if statement
void emit_if(ASTnode *p, FILE *fp)
{ 

  char s[100]; //char holder
  
  //generate then else labels
  char *n = genlabel(); //negative
  char *o = genlabel(); //out
  
  //conditional expression
  emit_expr(p->S1,fp);
  emit(fp,"","cmp $0, %rax","if compare");//check boolean
  
  sprintf(s,"je %s",n);
  emit(fp,"",s,"JUMP else");//jump negative
  
  EMITAST(p->S2->S1,fp); //then block
  
  sprintf(s,"jmp %s",o);
  emit(fp,"",s,"JUMP out");//jump out
  emit(fp,n,"","END then block");//negative label
  
  //check if there's an else statement
  if(p->S2->S2!=NULL){EMITAST(p->S2->S2,fp);} //else block
  emit(fp,o,"","END if");//out label
  fprintf(fp,"\n");
}


// handle the WHILE statement
//pre: pointers not null, p points to while statememt
//post: GAS code that handles the while statement
void emit_while(ASTnode *p, FILE *fp)
{    
  char s[100]; //char holder
  
  //generate loop label
  char *r = genlabel(); //positive
  char *o = genlabel(); //out
  
  //push out label onto break/continue stacks
  BreakList = PushLabelBreakList(o);
  ContinueList = PushLabelContinueList(r);
  
  emit(fp,r,"","while loop");//positive label
 
  //conditional expression
  emit_expr(p->S1,fp);
  
  emit(fp,"","cmp $0, %rax","while compare");//check boolean
  sprintf(s,"je %s",o);
  emit(fp,"",s,"JUMP out");//jump out
  
  EMITAST(p->S2,fp); //while block
  
  sprintf(s,"jmp %s",r);
  emit(fp,"",s,"JUMP while");//jump positive
  emit(fp,o,"","END while");//out label
  fprintf(fp,"\n");
}


//pre: pointers not null, p points to break statement inside a while loop
//after PushLabelBreakList() is called
//post: GAS code that handles a break statement
void emit_break(ASTnode *p, FILE *fp){
	char s[100]; //char holder
	
	sprintf(s,"jmp %s",BreakList->s);
	emit(fp,"",s,"BREAK jump");
	PopLabelBreakList(); //pop used label
}


//pre: pointers not null, p points to continue statement inside a while loop
//after PushLabelContinueList() is called
//post: GAS code that handles a continue statement
void emit_continue(ASTnode *p, FILE *fp){
	char s[100]; //char holder
	
	sprintf(s,"jmp %s",ContinueList->s);
	emit(fp,"",s,"CONTINUE jump");
	PopLabelContinueList(); //pop used label
}

/*  EMIT the Abstract syntax tree */
// PRE: pointers not null, p points to AST node
// POST: GAS code for each AST instruction
void EMITAST(ASTnode *p,FILE *fp)
{
   char s[100]; //char holder
   int i; //um
   
   //if pointer is null get out
   if (p == NULL) return;

   //check what kind of AST node we are pointing at
   //and act accordingly
   switch (p->type) {
        case A_PROGRAM:    EMITAST(p->S1,fp); // externs
			   EMITAST(p->S2,fp); // PACKAGE_SUB
			   break;
			   
	case A_PACKAGE:    EMITAST(p->S1,fp); //global vardecs
			   EMITAST(p->S2,fp); //method decs
			   break;
			   
	case A_EXTERN:     break;
	
	case A_EXTERNTYPE: break;
	
	case A_BLOCK:      EMITAST(p->S2,fp); //statements
			   break;
	
	case A_VARDEC:     break;
			   	
	case A_ASSIGNSTMT: emit_assign(p,fp);
			   break;			 
	
	case A_METHODCALL: Emit_Method_Call(p,fp);
			   break;
	
	case A_METHODDEC:  Emit_Method_Decl(p,fp);
			   break;
			   
	case A_IFSTMT:	   emit_if(p,fp);
			   break;
			   	
	case A_WHILESTMT:  emit_while(p,fp);
			   break;
			   	   
	case A_RETURN:     Emit_Return(p,fp);
			   break;
			   
	case A_BREAK:      emit_break(p,fp);
			   break;
			   
	case A_CONTINUE:   emit_continue(p,fp);
			   break;	
			   
	default: printf("you really don't want to see this one\n\n AST type %d unaccounted for in EMITAST\n",p->type);

       }//of switch
       
   EMITAST(p->next,fp); // always look for the next node

}


//pre: file pointer is not null, ast pointer to return or null
//post: GAS code for returning from a method
//%rax is set to the return value
//if ast pointer is null %rax is set to 0
void Emit_Return(ASTnode *p, FILE *fp){
	
	//if return value is null or undefined, return 0
	if(p == NULL || p->S1 == NULL){
		emit(fp,"","mov $0, %rax","Default return value");
	}else{ //otherwise return the value of the expression
		emit_expr(p->S1,fp);
	} 
   	
   	//explicit method return
	emit(fp,"","leave","leave the function");
	emit(fp,"",".cfi_def_cfa 7,8","STANDARD end function for GAS");
	emit(fp,"","ret","");
	fprintf(fp,"\n");
}


//pre: ast and file pointers are not null
//post: GAS code for method declaration to be placed in the .text section
void Emit_Method_Decl(ASTnode *p, FILE *fp){
	char s[100]; //char holder
	
	//standard function header
	fprintf(fp,"\t.globl %s \n",p->name);
	fprintf(fp,"\t.type %s @function \n",p->name);
	fprintf(fp,"%s:\t#Start of Function \n",p->name);
	fprintf(fp,"\t.cfi_startproc	# \tSTANDARD FUNCTION HEADER FOR GAS\n");
	fprintf(fp,"\tpushq   %%rbp	# \tSTANDARD FUNCTION HEADER FOR GAS\n");
	fprintf(fp,"\t.cfi_def_cfa_offset 16	# \tSTANDARD FUNCTION HEADER FOR GAS\n");
	fprintf(fp,"\t.cfi_offset 6, -16	# \tSTANDARD FUNCTION HEADER FOR GAS\n");
	fprintf(fp,"\tmovq    %rsp, %rbp	# \tSTANDARD FUNCTION HEADER FOR GAS\n");
	fprintf(fp,"\t.cfi_def_cfa_register 6	# \tSTANDARD FUNCTION HEADER FOR GAS\n");
	fprintf(fp,"\n");
	
	//line for carving out space on the stack
	sprintf(s,"subq $%d, %rsp",p->symbol->mysize*WSIZE);
	
	emit(fp,"",s,"Carve out stack for method");
	fprintf(fp,"\n");
	
	//handle formal parameters
	ASTnode *params = p->S1;
	int io = WSIZE;
	//run through formal parameter list
	while(params != NULL){
		sprintf(s,"mov %r%d, %d(%rsp)",io,params->symbol->offset*WSIZE); //backward??
		emit(fp,"",s,"copy actual to appropriate slot");
		params = params->next;
		io++;
	}
	
	EMITAST(p->S2,fp); //method block
	
	//implicit method return
	emit(fp,"","mov $0, %rax","Default return value");
	emit(fp,"","leave","leave the function");
	emit(fp,"",".cfi_def_cfa 7,8","STANDARD end function for GAS");
	emit(fp,"","ret","");
	fprintf(fp,"\n");
	
	//end production
	emit(fp,"",".cfi_endproc","STANDARD end function for GAS");
	sprintf(s,".size\t%s, .-%s",p->name,p->name);
	emit(fp,"",s,"STANDARD end function for GAS");
	fprintf(fp,"\n");
}

//pre: file pointer is not null, can handle p being null
//post: GAS code for strings to be placed in the .rodata section
void Emit_Strings(ASTnode *p, FILE *fp){
	
	//if pointer is null get out
	if (p == NULL) return;
	
	//look for strings on all levels
	if(p->type == A_CONSTANT_STRING){
		fprintf(fp,"%s:\t.string  %s\t# global string \n",p->label,p->name);
	}
	
	//search all branches of AST
	Emit_Strings(p->S1,fp);
	Emit_Strings(p->S2,fp);
	Emit_Strings(p->next,fp);
	
}

//main file emit
//pre: ast and file pointers are not null
//post: a complete, correct assembly program written to output file
void EMIT(ASTnode *p,FILE *fp){
	fprintf(fp,"\n");
	//print name of package
	fprintf(fp,"#\tPackage %s\n\n",p->S2->name);
	
	//rodata section header
	fprintf(fp,".section .rodata # read only data section\n");
	fprintf(fp,"percentD:  .string \"%%d\" # ALWAYS needed for print int\n");
	fprintf(fp,"# strings\n");
	//strings
	Emit_Strings(p,fp);
	fprintf(fp,"# end of strings\n\n");
	
	//data segment
	fprintf(fp,".data # start of the DATA section for variables\n");
	fprintf(fp,"\t.comm _SCANFHOLD,8,8\t# MANDATORY space for SCANF reads\n");
	//global variables
	Emit_Globals(p->S2->S1,fp);
	fprintf(fp,"#end of data section\n\n");
	
	//code segment
	fprintf(fp,".text\t#start of code segment\n\n");
	EMITAST(p->S2->S2,fp);
	
	//assembly footer
	emit(fp,"",".ident  \"GCC: (SUSE Linux) 7.5.0\"","");
	emit(fp,"",".section\t.note.GNU-stack,\"\",@progbits","");
	//printf("WAHOO\n"); //don't do this anymore
	
}



