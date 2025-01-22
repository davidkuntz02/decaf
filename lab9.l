%{
/*                 Small LEX routine which returns many DECAF tokens
 
                    This LEX definition is the companion to the lab6 YACC routine which
                   is a DECAF parser

                    Shaun Cooper
                    January 2015
                    
                    David Kuntz
                    September 2022
                    
                    Lab 9 GAS from AST
*/

static int mydebug=0;
int lineindex=1; //line count
#include "ast.h" //abstract syntax tree methods included
#include "y.tab.h"

//lab5 new method hextoint
//precondition: char pointer hex is a hexadecimal representation of an integer
//postcondition: returns a equivalent integer value
int aHexToInt(char * hex){
	int num=0; //initialize return value
	
	//iterate through characters
	for (int i = 2; hex[i] != '\0'; ++i){ 
		if (hex[i] >= '0' && hex[i] <= '9' ) //find characters 1-9
			num = num*16 + hex[i] - '0'; //add decimal value
		if (hex[i] >= 'a' && hex[i] <= 'f' ) //find characters a-f 
			num = num*16 + (hex[i] - 'a'+10); //add decimal value
		if (hex[i] >= 'A' && hex[i] <= 'F' ) //find characters A-F
			num = num*16 + (hex[i] - 'A'+10); //add decimal value
	}
	return num;
} 

%}

%%
"bool"		{return (T_BOOLTYPE);}
"break"		{return (T_BREAK);}
"continue" 	{return (T_CONTINUE);} 
"else" 		{return (T_ELSE);}  
"extern"	{return (T_EXTERN);}  
"false" 	{return (T_FALSE);}  
"for"  		{return (T_FOR);}   
"func"		{return (T_FUNC);}   
"if"   		{return (T_IF);}     
"int"  		{return (T_INTTYPE);}  
"null" 		{return (T_NULL);}   
"package"	{return (T_PACKAGE);} 
"return" 	{return (T_RETURN);} 
"string" 	{return (T_STRINGTYPE);} 
"true"		{return (T_TRUE);}     
"var"		{return (T_VAR);}    
"void"  	{return (T_VOID);}  
"while" 	{return (T_WHILE);}
"=" 		{return (T_ASSIGN);} //was not implemented in lab 5
"<<"  		{return (T_LEFTSHIFT);}
">>"  		{return (T_RIGHTSHIFT);}
">"  		{return (T_GT);}
"<"		{return (T_LT);}
"<="  		{return (T_LEQ);}
">="  		{return (T_GEQ);}
"=="  		{return (T_EQ);}
"!="  		{return (T_NEQ);}
"&&"  		{return (T_AND);}
"||"  		{return (T_OR);}
"."		{return (T_DOT);}
\'.\'		{yylval.value = yytext[1]; return (T_CHARCONSTANT);} //chars
\".*\"		{yylval.string = strdup(yytext); return (T_STRINGCONSTANT);} //strings
\/\/.*\n	{lineindex++;}
[a-zA-Z][a-zA-Z0-9_]*	{if (mydebug) fprintf(stderr,"Letter found\n"); 
                       yylval.string=strdup(yytext); return(T_ID);}
[0-9][0-9]*	{if (mydebug) fprintf(stderr,"Digit found\n"); 
                       yylval.value=atoi((const char *)yytext); return(T_INTCONSTANT);}
0x[0-9a-fA-F][0-9a-fA-F]*	{if (mydebug) fprintf(stderr,"Hex found\n"); 		//hex input
                       yylval.value=aHexToInt(yytext); return(T_INTCONSTANT);}
[ \t]		{if (mydebug) fprintf(stderr,"Whitespace found\n");}
[\[\]{}()=\-+*\/%,!;] { if (mydebug) fprintf(stderr,"return a token %c\n",*yytext); 
                       return (*yytext);}
\n		{ if (mydebug) fprintf(stderr,"carriage return %c\n",*yytext); 
                       lineindex++;}

%%

int yywrap(void)
{
return 1;
}



