/*  Symbol Table --linked list
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

    
#include <string.h>
#include "ast.h"
#include "symtable.h"


int GTEMP = 0; //temp counter

//pre: it is called...
//post: returns a string name for a temporary value
char * CREATE_TEMP(){
	char s[100];
	sprintf(s,"_t%d",GTEMP);
	GTEMP++;
	return(strdup(s));
}

struct SymbTab * first=NULL;



/* Simple Insert into the symbol table with the size, type level that the name is being inserted into */
//precondition: Symbtab struct pointer, variable name, type, subtype, level, size, offset, formal parameters 
//postcondition: new struct Symbtab element inserted into symbol table
struct SymbTab * Insert(char *name, enum AST_Decaf_Types Type, enum ID_Sub_Type subtype , int  level, int mysize, int offset , ASTnode * fparms)

{
  struct SymbTab * n;
    n=Search(name,level, 0);
    if(n!=NULL)
      {
      printf("\n\tThe name %s exists at level %d already in the symbol table\n\tDuplicate can.t be inserted",name, level);
      return (NULL);
      }
    else
    {
      struct SymbTab *p;
      p=malloc(sizeof(struct SymbTab));
      p->name=name;
      p->offset=offset;  /* assign the offset */
      p->level=level;  /* assign the level */
      p->mysize=mysize;  /* assign the size */
      p->Type=Type;  /* assign the Type */
      p->SubType=subtype;  /* assign the Function  */
      p->fparms=fparms;  /* assign the Method parameter list  */
      p->next=NULL;

   /* Check on how many elements we have in the symbol table */
      if(first==NULL)
      {
        first=p;
      }
      else
      {
        p->next=first;
        first=p;
      }
      return (p);
 
    }
     
  printf("\n\tLabel inserted\n");
}

/* print out a single symbol table entry -- for debugging */
void PrintSym(struct SymbTab *s)
{
	char *string,*display_type; 
	 switch(s->SubType) {
		 case ID_Sub_Type_Scalar: string = "Scalar " ; break;
		 case ID_Sub_Type_Method: string = "Method " ; break;
		 case ID_Sub_Type_Array: string = "Array " ; break;
		 case ID_Sub_Type_Package: string = "Package " ; break;
		 case ID_Sub_Type_Extern_Method: string = "Extern Method " ; break;
		 default:  string ="Unknown";
	 }
	 switch(s->Type) { //int bool void or string
		 case A_Decaf_INT: display_type = "INT " ; break;
		 case A_Decaf_BOOL: display_type = "BOOL " ; break;
		 case A_Decaf_VOID: display_type = "VOID " ; break;
		 case A_Decaf_STRING: display_type = "STRING " ; break;
		 default:  string ="Unknown Declared TYPE in print symbol";
	 }

         printf("\t%s\t\t%d\t%d\t%d\t%s\t%s\n",s->name,s->offset,s->mysize, s->level,display_type,string);

}


/*  General display to see what is our symbol table */
//pre: symbol table exists
//post: displays elements in symbol table at level where it is called
void Display()
{
   int i;
   struct SymbTab *p;
   p=first;
   printf("\n\tLABEL\t\tOFFSET\tSIZE\tLEVEL\tTYPE\tSUBTYPE\n");
      while (p!=NULL)
      {
         PrintSym(p);
         p=p->next;
      }
}

/*  Search for a symbol name at level or below.  We have to do multiple passes into the symbol table because we have to find
   the name closest to us 


  If recur is non-zero, then we look through all of the levels, otherwise, only our level 
   We return a pointer to a SymbolTab structure so that we can use other functions/methods to get the attributes */

//precondition: Symbtab struct pointer, variable name, variable level, int recur counter 
//postcondition: returns specified struct Symbtab element from symbol table if it exists, returns NULL otherwise
struct SymbTab * Search(char name[], int level, int recur)
{
   int i,flag=0;
   struct SymbTab *p;

  /* for each level, try to find our symbol */
   while (level >= 0)
    {
       p=first;
       while (p!=NULL)
        {
         if((strcmp(p->name,name)==0) && (p->level == level))
           return p;
         p=p->next;
        }
       if (recur == 0) return (NULL);   /* we did not find it at our level */
       level--; /* check the next level up */
    }


   return  NULL;  /* did not find it, return 0 */
}

/* Remove all enteries that have the indicated level
   We need to take care about updating first pointer into the linked list when we are deleting edge elements */

//pre: offset > 0
//post: removes all symbol entries at specified level and returns the offset value they occupied
int Delete(int level)
{
    struct SymbTab *p,*f=NULL;  /* we follow with pointer f */
    int SIZE=0;
    p=first;

    
    
  /* cruise through the list */

    while (p != NULL)
      {
        /* do we match? */
        if (p->level>= level )
        { /* if it is the first in the list we have to update first, we know this by f being NULL */
           SIZE+=p->mysize;
           if ( f==NULL) first=p->next;
           else /* not the first element */
              {f->next=p->next;
              }
            p=p->next;
           
        }
        else
         {
               /* update follow pointer, move the p pointer */
                f=p;
                p=p->next;
          }

      }
    return(SIZE);
}

