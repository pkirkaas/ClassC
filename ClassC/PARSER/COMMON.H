/*             %M%    %I%  %H%  %G%                */
#include <sys/types.h>
#include <stdio.h>
#include <varargs.h>
#include <string.h>
/*
#define NOVARARGS 1
*/
/*
#define stral(str) vwrite(0,str)
*/
#ifdef SUN
#define _mct __mct
#endif SUN
char errstr[199];
char * stral();
struct ref_el	 { char * name; int offst; };
struct sym_entry { 
		   struct sym_entry *next; 
		   char *name;
		   char * spare; /* used for full name in class decs */
		   char * typestr; 
		   int blk_no;
		   char * declaration;
		   char * abstract_dec;
		   int objQ;
		 };
struct struct_entry {
			struct struct_entry *next;
			char * name;
			struct sym_entry * sym_table;
		    } ;
struct class_entry {
			struct class_entry *next;
			char * name;
			/*
			char * typestr; 
			*/
			struct sym_entry * sym_table;
			struct sym_entry * fnc_tbl;
			char   * ancList; /* Ancestor list */
			char * initQ;	/* Is there a user defined 
					 * init() function for this class?
					 * If so, what is it? */
		    } ;

#define _NEW(x) (x *) calloc (1,sizeof (x) )
#define StrEq(str1,str2,rslt) /* If str1 == str2 rslt =1; else rslt = 0 */\
  { \
   char * QQ_a = (str1), * QQ_b = (str2);\
   rslt = 1;\
   while (*QQ_a || *QQ_b ) \
       { if(*QQ_a != *QQ_b) {rslt = 0; break;}\
	 QQ_a++; QQ_b++;\
       }\
  }	
#define YYSTYPE stack_type
int pkdbg;
typedef struct { char * r; char * t; } stack_type;
	/* stak_type -- type of value stakc to be used in yacc.  
	 * "r" ==  textual representation
	 * "t" ==  type string for expression, statement or variable.
	 */
extern int derefQ; /* Determines if derefrencing by function or expression */
extern char yytext[];
extern int nout; /* No output flag -- set on detection of error */
extern char * inlprep;
extern char * drfprep;
extern FILE * 	outFP;
extern char   	currfile[];
extern char   	* classList;
extern int 	currline;
extern struct class_entry 	* Ktable;
extern struct sym_entry		* Ttable;
char * stripStore();
char * vmerge();
char * nmerge();
char * vvmerge();
char * zmerge();
char ** Zmerge();
char * vwrite();
char * nwrite();
char * ztwrite();
char * pline();
char * namext();
char * cdr();
char * icon();
char * getype();
char * getmemtype();
char * build_class_entries();
char * build_class_func_decs();
char * build_deref_arr();
char * build_inline_arr();
char * setup();
char * buildClassList();
char * pkstrrpbrk();
char * storeStrip();
char * mkRefAr();
char * mkNum();
char * addPatList();
char * intostr();
char * mkcast();
struct sym_entry * get_sym_tbl();
struct sym_entry * find();
struct class_entry * find_cl();
