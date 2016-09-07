/*            ce.y    1.14  6/17/88  6/16/88                */
%{
/*
 *		ClassC -- Object Oriented C Language
 *	Copyright (c) 1988 by Paul Kirkaas
 *
 *	This program may be freely used and/or modified, 
 *	with the following provisions:
 *	1. This notice and the above copyright notice must remain intact.
 *	2. Neither this program, nor any modification of it,
 *	   may be sold for profit without written consent of the author.
 *
 */
#define isObject(str) ( (*str == 'k') || (*str == 'o' ) )
#define YYMAXDEPTH 10000
#define PKDEBUG 1
#include "common.h"	
char * funame;  /* For deref debugging */
int derefQ=1;	/* Flag for old/deref() style dereferencing. */
		/* Default to oldstyle function deref; slower, but hardier */
int debugQ=0;	/* Flag for debugging stuff */
int indepth = 0; /* Depth of object indirection within a single statement.
		  * This is used because all object funtions take the object 
		  * itself as an argument, hence an object function at the end
		  * of a chain of objects takes the entire chain as an argument;
		  * if there are functions in the chain, they get re-evalutated
		  * etc.
		  */
int objdepth = 0; /* similar to indepth above */
int fdefQ = 0; /* Flag -- 0== in Variable declaration; 1 == Function dec. */
int ArgQ  = 0;  /* Flag -- Is this declaration section for formal parameters? */
int mdefQ = 0; /* Flag -- We are defining a class member function */
int tdefQ = 0; /* Flag -- Are we defining a new type w. typedef? */
int CdefQ = 0; /* Flag -- Are we defining a new Class ?? */
int InitQ = 0; /* Flag -- Are we defining Class Init function ? */
int mainQ = 0; /* Flag -- Compiling the "main()" module?  Put in definitions */
int garbQ = 0; /* Flag -- If 1, compile for garbage collection -- else not */
int storeQ = 0; /* Flag -- Is the storage class specified ? */
extern int nout; /* Flag -- 1 == Error has been detected - no output */
extern char * libstr; /* This is a super string containing library functions*/
char * classList=0; /* List of class names separated by ":" made from 1st pass*/
char * vtype[99]; /* Single character representation of basic type --
	    i = int/char;		v = void;	d = float/double;
	    s = struct;	u = union;	k = class;	e = enum;
	    BUT: for aggregate types, full vtype is, eg, s<struct name>
	    */
char * indrstr;
/*  Referencing string for basic types -- 
 *  p = pointer;  v = vector (array);  f = function
 *  EX:  "unsigned *(*x[22])();"  -> vpfpI  ==
 *  x is an array of 22 pointers to funtions returning pointers to unsigned int
 *  !! FOR STRUCTURES, CLASSES, UNIONS & ENUMS !!
 *  struct list *x[5] = vprlist = "x is array of 5 pointers to struct "list"
 *  -- in general r,u,k&e followed by name ...r<structname>
 */
char * vname; /* Holds variable name at declarations to enter in symbol table */
char * cname; /* Holds class name */
/* Five symbol tables - 
 * -- functions, variables, structures, typedef's, and Classes  --
 * Variable, Function and Typedef tables use the same format (sym_entry); 
 * Structure table uses struct_entry format.
 * Class table uses class_entry format
 * Ininitialize all five here:
 */

struct sym_entry    Vtail	= 	{ (struct sym_entry *) 0, "VarT" }, 
		    Ftail	= 	{ (struct sym_entry *) 0, "FncT" }, 
		   *Vtable	= 	&Vtail,
		   *Ftable	=	&Ftail,
		   *Ttable	=	&Vtail,
		   *Tmp_Vtbls[9],	/* Structure symbol table stack 
					 * for nested structure declarations */
		   *Tmp_Ftbls[9];	/* Structure function table stack */

/* Structure symbol table initialization */
struct struct_entry Stail	=	{ (struct struct_entry *) 0 },
		   *Stable	=	&Stail;

/* Class symbol table initialization */
struct class_entry	 Ktail	=	{ (struct class_entry *) 0},
			*Ktable	=	&Ktail;

struct class_entry	*thisClass = 0;

/* sym_table_list -- A list built by the parser when creating a derived class.
 *	Each ancestor of the derived class will have an equivalent entry in
 * the sym_table_list consisting of their symbols.  The derived class will be
 * constructed from this, with all duplicates removed.  The first definition
 * of a name has the priority.
 */

struct sym_tabl_list 
	{  struct sym_tabl_list * next;
	   struct sym_entry 	* sym_table;
	   struct sym_entry 	* fnc_tbl;
	};
struct sym_tabl_list	 sym_tabl_tail	= { 0 },
			*sym_tabl_tabl	= &sym_tabl_tail;

int struct_cnt = 0; /*  Count to assign names to unamed structures */
int blk_no = 0; /* Block count for scoping; count increases with every '{';
		 * and decrements with every '}' */
int tmpint[99];
int decdpth=0; /* The stack pointer for nested structure/class declarations --
		* allowed here to a nesting depth of 99.tmpint[],vtype[decdpth],
		*/
char tmpstr[99]; 
char *tmppc =0; /* Very temporary character pointer */
char *tmppc2=0; /* Very temporary character pointer */
char * type_str=0;/* Contains the text of a C - type name (ex, "unsigned int")*/
char * dec_str=0;  /* Contains declarator string */
char * cast_str=0; /* Contains declarator string WITHOUT identifier, for casts*/
char * abst_dec_str = 0; /* cast_str with type prepended for true casting */
char * class_funcs = 0;
char * classDecs = 0; /* Class Declarations should all float to the top */
		      /* (27Jan89 -- classDecs includes typedef's also) */
char * patList = 0;    /* List of ancestors */
char * func_list = 0;  /* Contains all the customized functions built by
			* this parser.  Will be prepended to the output */
char *(stat_stk[99]);
char *(dec_stk [99]);
/* statement & declaration stacks: used to re-arrange statements/declarations
 * within a block so that all declarations float to the top.  Variables must
 * still be declared before use, but can otherwise be intermixed with 
 * executable statements.  */
%}
%token AND
%token ASSIGN
%token BREAK
%token CASE
%token CHAR
%token CHARACTER
%token CHRL
%token CLASS
%token CLREF
%token CONTINUE
%token DEFAULT
%token DINC
%token DO
%token ELSE
%token ENUM
%token EQ
%token FLOAT
%token FOR
%token GEQ
%token GOTO
%token IDENT
%token IF
%token INT
%token INTEGER
%token KNAME
%token LEQ
%token LONG
%token LSH
%token NAME_SCOPER
%token NEQ
%token NEW
%token NOISE
%token OBJ
%token OR
%token PAPA
%token REAL
%token RETURN
%token RSH
%token SCOPER
%token SHORT
%token SIZEOF
%token SP
%token STORE
%token STR
%token STRUCT
%token SWITCH
%token SYNTH
%token TYPEDEF
%token UNION
%token UNSGN
%token VOID
%token WHILE

%left ','
%left DUMMY
%right '=' ASSIGN 
%right '?' ':'
%left OR
%left AND
%left '|'
%left '^'
%left '&'
%left EQ NEQ
%left '<' '>' LEQ GEQ
%left LSH RSH
%left '+' '-'
%left '*' '/' '%'
%right SIZEOF DINC '!' '~'
%right POSTFIX  /* Another dummy placeholder for %prec casting */
%left SP '.' CLREF

%start file
%%
file :	sets
	    {
	     tmppc = derefQ ? drfprep : inlprep;
/*
	     fprintf(outFP,"%s%s%s%s\n\n",tmppc,classDecs,$1.r,func_list);
*/
	     fprintf(outFP,"%s%s%s\n\n",tmppc,classDecs,$1.r);
	    }
     ;
sets :	set
     |	sets set
	    {$$.r = vmerge($1.r,$2.r); }
     ;
set  :  definition 
		{$$.r = vwrite($1.r,"\n");}
     |  declaration	/* Function declarations and un-initialized variables*/
     ;

statement
    :	simple_statement 
		{
		 int i;
		 tmppc=stral("\n");
		 for (i=0; i< blk_no; i++) tmppc = vwrite(tmppc,"  ");
		 $$.r = vwrite(vmerge (tmppc,$1.r), "\n"); 
		 tmppc = 0;
		 indepth = 0;
		 objdepth = 0;
		}
    |	block
		{$$.r = nmerge(3,stral("\n"),$1.r,stral("\n"));}
    |	error ';'
	    { yyerror(" Syntax error\n");
	      yyerrok;
	    }
    ;

/* Declarations */
declaration
    :	Type opt_declarator_list ';'
	    {
	      $$.r=nmerge(3,$1.r,$2.r, stral(";\n"));
	      storeQ = 0;
	    }
    |	TYPEDEF		{tdefQ = 1; /* IS a typedef */ }
	type opt_declarator_list ';'
	    { classDecs=nmerge(6,classDecs,$1.r,$3.r,$4.r,$5.r,stral("\n"));
/*
	    { $$.r=nmerge(5,$1.r,$3.r,$4.r,$5.r,stral("\n"));
*/
	      storeQ = 0;
	      tdefQ = 0; /* Return to normal state */
		$$.r = 0;
	    }
    ;
opt_declarator_list
    :	/*empty */	{($$).r = 0; ($$).t = 0; }
    |	declarator_list
    ;
declarator_list 
    :	pi_declarator
    |	declarator_list ',' pi_declarator
	    { $$.r=nmerge(3,$1.r,$2.r,$3.r); }
    ;
pi_declarator 
    :	declarator opt_initialization
	    { $$.r = vmerge($1.r,$2.r); }
    ;

/* Variable declarations */
/*******  HERE WE BUILD SYMBOL TABLE  *********/
declarator 
    :	v_declarator
	    {
	      indrstr = vwrite(indrstr,vtype[decdpth]);
	      dec_str = vwrite(stral(type_str),($1).r);
	      abst_dec_str = vwrite(vwrite(abst_dec_str,type_str),cast_str);
	      /* make/check entry in symbol table.  */
	      /*if Typedef, and NOT in a structure definition */
	      if (tdefQ && !decdpth)
		    Uenter(vname,indrstr,dec_str,0, &Ttable); 
	      else
		    Uenter(vname,indrstr,dec_str,blk_no, &Vtable,abst_dec_str);
	      if (indrstr) {free (indrstr); indrstr = 0;}
	      if (cast_str) {free (cast_str); cast_str = 0;}
	      if (abst_dec_str) {free (abst_dec_str); abst_dec_str = 0; }
	      if (dec_str) {free (dec_str); dec_str = 0; }
	    }
    |	f_declarator
	    { indrstr = vwrite(indrstr,vtype[decdpth]);
	      dec_str = vwrite(stral(type_str),($1).r);
	      abst_dec_str = vwrite(vwrite(abst_dec_str,type_str),cast_str);
	      /* make/check entry in symbol table.  */
	      Uenter (vname, indrstr,dec_str, 0, &Ftable,abst_dec_str); 
	      free (vname); free (indrstr); free (abst_dec_str); free(dec_str);
	      vname = 0; indrstr = 0; abst_dec_str = 0; dec_str = 0;
	      if (cast_str) {free (cast_str); cast_str = 0;}
	    }
    ;
f_declarator 
    :	IDENT '(' opt_ident_list ')'
	    { if(vname) free (vname);
	      vname = namext(($1).r); /* capture variable name */
	      /*  See if we are compiling the "main()" module */
	      if (strcmp(namext($1.r),"main") == 0) mainQ = 1;
	      if (cname && CdefQ) $1.r = ztwrite(0,"_",cname,"_",$1.r,0);
	      $$.r = nmerge(4,$1.r,$2.r,$3.r,$4.r);
	      fdefQ = 1; /* This is a function (not variable) declaration */
	      indrstr = 0; /* Re-intialize the indirection string to 0 */
	      /* cast_str builds up abstract type declarator for deref */
	      if (cast_str) {free (cast_str); cast_str = 0; }
	    }
/* Must parse for class member function defintion here */
    |	NAME_SCOPER IDENT '(' opt_ident_list ')'
	    { 
	      if (tmppc = strchr($1.r,':')) *tmppc  =0;
	      if (!(thisClass=(struct class_entry *)find(namext($1.r),Ktable)))
		      errpt("Failed to find %s in class table\n",$1.r);
	      /* capture variable name */
	      if(vname) free (vname);
	      if (!strcmp(namext($2.r),"init")) InitQ=1;
	      else InitQ=0;
	      vname = nmerge(4,stral("_"),namext($1.r),stral("_"),namext($2.r));
	      /* If there is an argument list ... */
	      if (! CdefQ)
		  {
		    if ($4.r) $4.r = vmerge(stral("self, "),$4.r);
		    else $4.r = stral("self"); 
		  }
	      $1.r = stral(vname);
	      $$.r = nmerge(4,$1.r,$3.r,$4.r,$5.r);
	      fdefQ = 1; /* This is a function (not variable) declaration */
	      mdefQ = 1; /* This is a member function(not independent)def'n */
	      indrstr = 0; /* Re-intialize the indirection string to 0 */
	    }
    |	'(' f_declarator ')'
	    { $$.r=nmerge(3,$1.r,$2.r,$3.r); 
	      cast_str= vwrite(vwrite($1.r,cast_str),$3.r);
	    }
    |	'(' f_declarator ')' '(' ')'
	    { $$.r=nmerge(5,$1.r,$2.r,$3.r,$4.r,$5.r);
	      cast_str= nwrite(3,0,"(",cast_str,")()");
	      cast_str= vwrite(vwrite(0,"("),cast_str,")()");
	      indrstr = vwrite (indrstr,"f"); /* "Function returning ..." */
	    }
    |	f_declarator '[' opt_expr ']'
	    { $$.r=nmerge(4,$1.r,$2.r,$3.r,$4.r); 
	      cast_str= nwrite (3,cast_str,"[", $3.r, "]");
	      indrstr = vwrite(indrstr,"p"); /*"Array (vector) of..."*/
	    }
    |	'*' f_declarator
	    { $$.r = vmerge($1.r,$2.r); 
	      cast_str = nwrite(2,0,"*",cast_str);
	      indrstr = vwrite(indrstr,"p"); /* " pointer to ... " */
	    }
    ;
v_declarator 
    :	IDENT 
	    { fdefQ = 0; /* This is a variable (not function) dec */
	      indrstr = 0; /* String of type derivations */
	      if (vname) free(vname);
	      vname = stral(($1).r); /* capture variable name */
	      /* cast_str builds up abstract type declarator for deref */
	      if (cast_str) {free (cast_str); cast_str = 0; }
	    }
    |	'(' v_declarator ')'
	    { 
	      cast_str= nwrite(3,0,$1.r,cast_str,$3.r);
	      $$.r=nmerge(3,$1.r,$2.r,$3.r); }
    |	'(' v_declarator ')' '(' ')'
	    { 
	      cast_str= nwrite(3,0,"(",cast_str,")()");
	      $$.r=nmerge(5,$1.r,$2.r,$3.r,$4.r,$5.r);
	      indrstr = vwrite (indrstr,"f"); /* "Function returning ..." */
	    }
    |	v_declarator '[' opt_expr ']'
	    { 
	      cast_str= nwrite (3,cast_str,"[", $3.r, "]");
	      $$.r=nmerge(4,$1.r,$2.r,$3.r,$4.r); 
	      indrstr = vwrite(indrstr,"p"); /*"Array (vector) of..."*/
	    }
    |	'*' v_declarator
	    { 
	      cast_str = nwrite(2,0,"*",cast_str);
	      $$.r = vmerge($1.r,$2.r); 
	      indrstr = vwrite(indrstr,"p"); /* " pointer to ... " */
	    }
    ;
opt_initialization 

    :		/* empty */
	    { if (isObject(vtype[decdpth]) &&
		  !ArgQ && !storeQ && !fdefQ && !decdpth)
		{
		    $$.r = stral(" = 0");
		    $$.t = "i";
		}
	      else ($$).r=0, $$.t = 0;
	    }
    |	'='  initialization
	    {
	      /* If initialization is a class or object type -- */
	      if ( isObject (vtype[decdpth]) )
		{ if (!blk_no) /* That is, if at global level -- */
	 		yyerror("Initialization of global object illegal\n");
		  else
		    {if (garbQ)
			stat_stk[blk_no]= ztwrite(stat_stk[blk_no],
			    "(*_decref(&",vname, "))= _incref (",$2.r,");\n",0);
		     else /* NOT garbage collected */ 
			 stat_stk[blk_no]=
			    ztwrite(stat_stk[blk_no],vname,"=",$2.r,";\n",0);
		     free($1.r); free($2.r);
		     $$.r = stral (" = 0");
		    }
		}
	      else
		    $$.r = vmerge($1.r,$2.r); 
	    }
    ;
initialization
    :	expr
    | '{' initialization_list '}'
		    { $$.r=nmerge(3,$1.r,$2.r,$3.r); }
    | '{' initialization_list ',' '}'
		    { $$.r=nmerge(4,$1.r,$2.r,$3.r,$4.r); }
	       ;
initialization_list 
    :	initialization
    |	initialization_list ',' initialization %prec DUMMY
		    { $$.r=nmerge(3,$1.r,$2.r,$3.r); }
    ;

/* Definitions */
Type :	type
	    { type_str = vwrite(stral(stripStore($1.r))," "); }
     |	STORE
	    {storeQ = 1;
	    }
	type
	    {
	      type_str = vwrite(stral($3.r)," ");
	      $$.r = vmerge ($1.r,$3.r);
	     }
     |	/* empty */
	     { $$.r = 0;
	       type_str = stral("int ");
	       vtype[decdpth] = stral("i");
	     }
     ;
definition 
    :	Type f_declarator 
	    { indrstr = vwrite(indrstr,vtype[decdpth]);
	      /* make/check entry in symbol table.  */
	      if(!fdefQ)yyerror("Bad function definition");
	      if(blk_no)yyerror("Function define illegal inside block");
	      abst_dec_str = vwrite(vwrite(abst_dec_str,type_str),cast_str);
	      /* make/check entry in symbol table.  */
	      Uenter (vname, indrstr,dec_str, 0, &Ftable,abst_dec_str); 
	      if (derefQ)funame=nwrite(3,0,"_whome[_mct++]=\"", vname,"\";\n");
	      free (vname); free (indrstr);
	      vname = 0; indrstr = 0;
	      blk_no ++;
	      /* Normally, object declarations get converted to
		 static and initialized to 0 if inside a block;
		 but if the object is an argument (formal parameter)
		 to a function, we set the flag ArgQ to prevent this
	       */
	      ArgQ = 1;
	      if (indrstr) {free (indrstr); indrstr = 0;}
	      if (cast_str) {free (cast_str); cast_str = 0;}
	      if (abst_dec_str) {free (abst_dec_str); abst_dec_str = 0; }
	      if (dec_str) {free (dec_str); dec_str = 0; }
	    }
	opt_dec_list 
	    {
	      ArgQ = 0;
	      if (mdefQ) 
		{Uenter("self","o","int self",blk_no, &Vtable,"int ");
		 $4.r = vwrite(vmerge(stral("int self;"),$4.r),"\n");
		}
	      blk_no --; 
	    }
	block 
	    { 
	      if (InitQ) 
		 {  *strrchr($6.r,'}')=0;/* Removes last bracket*/
		    $6.r = vwrite($6.r,"\treturn self;\n}");
	         }
	      $$.r=nmerge(6,$1.r,stral("\n"),$2.r,stral("\n"),$4.r,$6.r);
	      InitQ = 0;
	      mdefQ = 0;
	    }
	;
opt_dec_list
    :	/* empty */ {($$).r=0;}
    |	dec_list
    ;
dec_list 
    :	declaration
    |	dec_list  declaration
	    { $$.r = vmerge($1.r,$2.r); }
    ;
arg_list
    :	expr
    |	arg_list ',' expr
	    {$$.r = nmerge(3,$1.r,$2.r,$3.r); }
    ;
opt_arg_list
    :	/* empty */
	    {$$.r=0;}
    |	arg_list
    ;
/*********************  Statements & Expressions ************************/
comop_expr
    :	expr
    |	comop_expr ',' expr
	    { $$.r=nmerge(3,$1.r,$2.r,$3.r); }
    ;
expr :	ni_expr		/* Non Identifier expression... */
     |  IDENT
	    { struct sym_entry * tmpseP;
	      if ((mdefQ) && (tmpseP = find(namext($1.r),thisClass->sym_table)))
	      {
		tmppc2 = ztwrite (0,"(",tmpseP -> abstract_dec,")",0);
		tmppc2 = mkcast(tmppc2,0);

		if( derefQ)
		{
		      $$.r = ztwrite(0,
				"(*",tmppc2,
				")_deref(self, \"",
				tmpseP->name,
				"\"))",
				0);
		}
		else
		{
		      $$.r = ztwrite(0,
			    "\n(*",
			    tmppc2,
			    ")((_objV[",
			    intostr(++objdepth),
			    "]=_objTbl+self)->self + _objV[",
			    intostr(objdepth),
			    "]->hofV[",
			    mkNum(tmpseP->name),
			    "&_objV[",
			    intostr(objdepth),
			    "]->hashV]))\n",
			    0);
		}
		  $$.t = stral (tmpseP -> typestr);
	      }
	      else ($$).t = getype(Vtable,($1).r);
	    }
ni_expr /* non - identifier expression */
     :	literal
     |	fnc_call
     |	KNAME CLREF NEW '(' opt_arg_list ')' /* Create & initialize an object */

	    {   
	    if (garbQ)
		    tmppc = stral("__new ( sizeof(struct ");
	    else
		    tmppc = stral("_new ( sizeof(struct ");
	    if (derefQ)
		{
		tmppc = nmerge(5,
				tmppc,
				namext($1.r),
				stral("), _"),
				namext($1.r),
				stral("Arr,0,0,0 )")
				) ;
		}
	    else
		{
		tmppc = nmerge(11,
		    tmppc,
		    namext($1.r),
		    stral("), _"),
		    namext($1.r),
		    stral("ArV, _"),
		    namext($1.r),
		    stral("HsV, _"),
		    namext($1.r),
		    stral("ArF, _"),
		    namext($1.r),
		    stral("HsF)")
		    ) ;
		}
		if (tmppc2=((struct class_entry *)find($1.r,Ktable))->initQ)
		    /*  There is an INIT function for this class  -- BUT
		     *  Must check to see if arglist exists or is empty
		     */
		    {
		      if ( $5.r ) $5.r = nmerge(3,tmppc,stral(", "),$5.r);
		      else $5.r = tmppc;
		      $$.r = nmerge(4,stral(tmppc2),stral("("),$5.r,stral(")"));
		    }

		 else if ( $5.r )
		      errpt("Error - Arguments used in call to%s%s",
			    " <new> but no init declared for class ", $1.r);
		 else $$.r = tmppc;
		 $$.t = vwrite(vwrite(0,"k"),namext($1.r));
	    }
     |	expr '[' expr ']'
	    { 
	      if ( *$3.t != 'i' ) 
		{
		  sprintf(errstr,"Non-integer in array reference: <%s>\n",$3.t);
		  yywarn(errstr);
		}
	      $$.t = cdr(($1).t,'p');
	      $$.r=nmerge(4,$1.r,$2.r,$3.r,$4.r); 
	    }

/* An object dereference works like this:
 *   1) an object is an integer variable, say OB;
 *   2) Each class has associated with it an array of strings and
 *	offsets for each component member.  The first part of the
 *	array represents DATA members and is delimited by an empty
 *	string ("").  The array continues, with the remainder of
 *	the entries matching strings to function addresses (cast as
 *	integers).  This is terminated by a null pointer.
 *	The array is called "_<CName>Arr" (of type "struct _ref_el []);
 *	and it is passed to a single, general dereferencing function
 *	(_deref()) --
 *		_deref(selfP, mem, class )
 *		int selfP; char * mem; struct _ref_el * class;
 *   3) NOW a reference to a memeber of OB; say 
 *		OB:>printself( args );
 *	gets translated to:
 *		*(_deref(OB,"printself") (OB,args );
 */

/* 2 types of object dereference statements follow --
 *	object CLREF opt_cast IDENT			(data dereference)
 *	object CLREF opt_cast IDENT ( OPT_ARG_LIST )	(function dereference)
 */
    |	expr CLREF opt_cast IDENT
	    { 
	    int outQ=0;
	    if (*$1.t == 'k') /* Class dereference*/
	      { 
		    char * cname = namext($1.t + 1);
		    struct class_entry * myClass =
				(struct class_entry *)find(cname,Ktable);
		    if (! myClass)
			{
			sprintf(errstr,
			"Woops! ClassC compiler bug -- can't find class <%s>\n",
			cname);
			yyerror(errstr);
			}
		    else
		    { struct sym_entry * tmpseP;
		      if ((tmpseP = find(namext($4.r),myClass->sym_table)))
		      {
			if ($3.r) tmppc2 = $3.r;
			else tmppc2 = ztwrite(0,"(",tmpseP->abstract_dec,")",0);
			tmppc2=mkcast(tmppc2,0);
			$$.t = stral (tmpseP -> typestr);
		      }
		      else 
		      {
			errpt("Member <%s> not declared for class <%s>\n",
				$4.r,cname); 
		      }
		    
		   }
	    }
	    else if (*$1.t == 'o') /* Object dereference*/
	    { 
	      if ($3.r)
		{ tmppc2 = $3.r;
		  tmppc2=mkcast(tmppc2,0);
		  $$.t = $3.t;
		}
	      else
	      	{  tmppc2=stral("(int*");
		   $$.t = stral("o");
		}
	     }
	    else yyerror("Class/Object type required\n"); 
	    if (!nout)
	    {
			if (derefQ)
			{
			    $$.r = ztwrite(0,
				"(*",
				tmppc2,
				")_deref(",
				$1.r,
				", \"",
				namext($4.r),
				"\"))",
				0);
			}
			else
			{
			    $$.r = ztwrite(0,
				"\n(*",
				tmppc2,
				") ((_objV[",
				intostr(++objdepth),
				"]=_objTbl+ ",
				$1.r,
				")->self+ _objV[",
				intostr(objdepth),
				"]->\n\thofV[",
				mkNum(namext($4.r)),
				"&_objV[",
				intostr(objdepth),
				"]->hashV]))",
				0);
			    $$.r = vwrite($$.r,"\n");
		        }
	    }
	  }


	/* The expression before CLREF here will be assigned to the 
	 * runtime variable _indArr[indepth].  This variable instead of
	 * expr itself will be used as the "self" argument for the function
	 * call.  This is in case the previous expression contains a function
	 * which modifies its data - we don't want it to be called twice
	 */

	/* Function member with optional argument list */
     | expr CLREF opt_cast IDENT  '(' opt_arg_list ')' 
	{ 
	  if (!isObject($1.t))
	  {  errpt("Class/Object type required\n");
	     goto FDBLOCKEND;
	  }
	  if(!strcmp (namext($4),"delete")) /* special process deletes */
	  {
	    if ($6.r) $6.r = ztwrite($1.r,", ",$6.r,0);
	    else $6.r = $1.r;
	    $$.r = ztwrite (0,"_delete (",$6.r,")",0);
	    goto FDBLOCKEND;
	  }
	  sprintf(tmpstr,"%d",indepth++);
	  if (*$1.t == 'k') /* Class dereference*/
	  { 
	      struct sym_entry * tmpseP;
	      char * cname = namext($1.t + 1);
	      struct class_entry * myClass =
				(struct class_entry *)find(cname,Ktable);
	      if (! myClass)
	      {
		  errpt("Woops! ClassC compiler bug -- can't find class <%s>\n",
			cname);
	    	  goto FDBLOCKEND;
	      }
	      if (! (tmpseP = find(namext($4.r),myClass->fnc_tbl)))
	      {    errpt("Function <%s> not declared for class <%s>\n",
			    namext($4.r),cname);
		   goto FDBLOCKEND;
	      }
	      if ($3.r)
	      {	  tmppc2 = mkcast($3.r,1);
		  $$.t = $3.t;
	      }
	      else
	      {  tmppc2 = ztwrite (0,"(",tmpseP -> abstract_dec,")",0);
		 tmppc2 = mkcast(tmppc2,1);
		 $$.t = stral (tmpseP -> typestr);
	      }
          }
          else if (*$$.t == 'o')
	  {  
		if ($3.r)
		{  tmppc2 = $3.r;
		   tmppc2 = mkcast(tmppc2,1);
		   $$.t = $3.t;
		}
		else
		{  tmppc2=stral("(int(*)()");
		   $$.t = stral ("o");
		}
	  }  
	  if (derefQ)
	  {
#ifdef REVERSE
		$$.r = ztwrite(0,
			"(",
			tmppc2,
			")(_deref(_indArr[",
			tmpstr,
			"] =",
			$1.r,
			",\"",
			namext($4.r),
			"\"))) (_indArr[",
			tmpstr,
			"]",
			0);
#else
		$$.r = ztwrite(0,
			"(",
			tmppc2,
			")(_deref(_indArr[",
			tmpstr,
			"], \"",
			namext($4.r),
			"\"))) (_indArr[",
			tmpstr,
			"] =",
			$1.r,
			0);
#endif REVERSE
	  }
	  else
	  {
#ifdef REVERSE
	    $$.r = ztwrite(0,
		"\n(",
		tmppc2,
		") ((_objV[",
		intostr(++objdepth),
		"]=_objTbl + (_indArr[",
		tmpstr, 
		"] =",
		$1.r,
		")) ->\n\thofF[",
		mkNum( namext($4.r)),
		"& _objV[",
		intostr(objdepth),
		"]->hashF])) (_indArr[",
		tmpstr,
		"]",
		0);
#else
	    $$.r = ztwrite(0,
		"\n(",
		tmppc2,
		") ((_objV[",
		intostr(++objdepth),
		"]=_objTbl + _indArr[",
		tmpstr, 
		"]",
		") -> hofF[",
		mkNum( namext($4.r)),
		"& _objV[",
		intostr(objdepth),
		"]->hashF]))\n\t((_indArr[",
		tmpstr,
		"] = ",
		$1.r,
		")",
		0);

#endif REVERSE
	    }
	    if ($6.r) $$.r = ztwrite($$.r,",", $6.r,0);
	    $$.r = vwrite($$.r,")\n");
	    FDBLOCKEND:;
	}
     | expr '.' IDENT
	    {
	      if (! strncmp($1.t,"s",1)) /* struct dereference */
		{
		  $$.t = getmemtype(($1).t,($3).r);
		  $$.r=nmerge(3,$1.r,$2.r,$3.r); 
		}
	      else {yyerror("Structure type required\n"); }
	    }
     | expr SP IDENT
	    {
	      if (! strncmp($1.t,"ps",2)) /* struct dereference */
				{ 
				  $$.t = getmemtype(cdr($1.t,'p'),$3.r);
				  $$.r=nmerge(3,$1.r,$2.r,$3.r); 
				}
	      else {yyerror("Structure type required\n"); }
	    }
     | expr DINC  	%prec POSTFIX 
		/* Post - decrement/increment */
				{ $$.r = vmerge($1.r,$2.r); }
     | uop expr		%prec '!' 
		/* ~ and ! and DINC and - */
				{ $$.r = vmerge($1.r,$2.r); 
				  ($$).t = ($2).t;
				}
     | '*' expr		%prec '!'
				{ $$.r = vmerge($1.r,$2.r); 
				  ($$).t = cdr(($2).t,'p');
				}
     | '&' expr		%prec '!'
				{ $$.r = vmerge($1.r,$2.r); 
				  tmppc = stral("p");
				  $$.t = vmerge(tmppc,$2.t);
				}
     | cast expr	%prec '!'
				{ $$.r = vmerge($1.r,$2.r); }
     | SIZEOF expr 
				{ ($$).r = vmerge($1.r,$2.r); 
				  ($$).t = stral("i");
				}
     | SIZEOF abstract_declaration 
				{ $$.r = vmerge($1.r,$2.r); 
				  $$.t = stral("i");
				}
     | SIZEOF cast 
				{ $$.r = vmerge($1.r,$2.r); 
				  $$.t = stral("i");
				}
     | expr mop expr	%prec '%'
				{ 
				  $$.t = icon(($1).t,($3).t,($2).r,0);
				  $$.r=nmerge(3,$1.r,$2.r,$3.r); 
				}
     | expr aop expr
				{ 
				  $$.t = icon($1.t, $3.t, $2.r, 1);
				  $$.r=nmerge(3,$1.r,$2.r,$3.r); 
				}
     | expr shop expr	%prec LSH
				{ $$.r=nmerge(3,$1.r,$2.r,$3.r); 
				 /*shop expression takes type of left operand */
				}
     | expr relop expr	%prec EQ
				{ 
				  $$.t = vwrite (0,"i");
				  $$.r=nmerge(3,$1.r,$2.r,$3.r); 
				}
     | expr bitop expr	%prec '^'
				{ $$.r=nmerge(3,$1.r,$2.r,$3.r); 
				  $$.t = vwrite (0,"i");
				}
     | expr logop expr	%prec AND
				{ 
				  $$.t = vwrite (0,"i");
				  $$.r=nmerge(3,$1.r,$2.r,$3.r); 
				}
     | expr ASSIGN expr	%prec ASSIGN
		{ if ((*$1.t == 'o')  || (*$3.t == 'o'))
		    yyerror("Illegal use of assignment operator on an object");
		  $$.r=nmerge(3,$1.r,$2.r,$3.r); 
		 /* assop expr takes type of left operand */
		}
     | expr '=' expr	%prec ASSIGN
	{ 
	if ( isObject($1.t) && isObject ($3.t))
	  if (! comobj ($1.t, $3.t) )  /* Illegal object assignment */
	    {
	    sprintf(errstr,"Incompatable object assignment - %s to %s\n",
			$3.t+1,$1.t+1);
	    yyerror(errstr);
	    }
	  else /*  Then a legal object op */
	    {if (garbQ)
		$$.r=nmerge(5,stral("*_decref(&("),$1.r,
			stral(")) = _incref(("), $3.r,stral("))"));
	     else /* No garbage collection */ goto nogarbage;
	    }
	 else if ( isObject ($1.t) && (!strcmp (namext($3.r),"0")))
				/* Null object pointer */
	    {if (garbQ)
		$$.r=nmerge(4,stral("*_decref(&("),$1.r,stral(")) ="), $3.r);
	     else /* No garbage collection */ goto nogarbage;
	    }
	 else if (isObject($1.t) || isObject ($3.t))  /*One object, one not*/
		yyerror("Illegal assignment -- object with non-object type\n");
	 else
	 nogarbage:
	     $$.r=nmerge(3,$1.r,$2.r,$3.r); 
	     /* assop expr takes type of left operand */
	}
     | '(' comop_expr ')'
				{ $$.r=nmerge(3,$1.r,$2.r,$3.r); 
				  $$.t = $2.t;
				}
     | expr '?' expr ':' expr	%prec '?'
		      {
		        if(strcmp(($3).t,($5).t) == 0) ($$).t= ($3).t;
			else ($$).t = icon(($3).t,($5).t,($4).r,1);
		       $$.r=nmerge(5,$1.r,$2.r,$3.r,$4.r,$5.r);
		      }
     ;
/*  WAS like this -- but shouldn't be.  Will the following work?  XXX PXK
*/
fnc_call	
    : IDENT '(' opt_arg_list ')'
	    { struct sym_entry * tmpseP;
	      if ((mdefQ) && (tmpseP = find(namext($1.r),thisClass->fnc_tbl)))
	      {
		tmppc2 = ztwrite (0,"(",tmpseP -> abstract_dec,")",0);
		tmppc2 = mkcast(tmppc2,1);
		if ( $3.r) /* then there IS an argument list */
		    tmppc = ztwrite(0,"(self,",$3.r,")",0);
		else
		    tmppc = stral("(self)");
		if (derefQ)
		{
		    $$.r = ztwrite(0,
			"(",
			tmppc2,
			")_deref(self, \"",
			tmpseP->name,
			"\"))",
			tmppc,
			0);
		}
		else
		{
		    $$.r = ztwrite(0,
			    "\n(",
			    tmppc2,
			    ")((_objV[",
			    intostr(++objdepth),
			    "]= _objTbl + self)->hofF[",
			    mkNum (tmpseP->name),
			    "& _objV[",
			    intostr(objdepth),
			    "]->hashF]))",
			    tmppc,
			    "\n",
			    0);
		}
		$$.t = stral (tmpseP -> typestr);
	      }
	      else
	      {
		  if ($1.t) /* there is a type for the expr -- check it*/
		  {  if (!strncmp($1.t,"pf",2)) $$.t = stral($1.t+2);
		     else if (!strncmp($1.t,"f",1)) $$.t = stral($1.t+1);
		     else errpt
		     ("Incorrect type for function call dereference <%s>",$$.t);
		  }
		  else ($$).t = getype(Ftable,namext(($1).r));
		  $$.r=nmerge(4,$1.r,$2.r,$3.r,$4.r); 
	      }
	    }
    | ni_expr '(' opt_arg_list ')'
	      {
		  if ($1.t) /* there is a type for the expr -- check it*/
		  {  if (!strncmp($1.t,"pf",2)) $$.t = stral($1.t+2);
		     else if (!strncmp($1.t,"f",1)) $$.t = stral($1.t+1);
		     else errpt
		     ("Incorrect type for function call dereference <%s>",$$.t);
		  }
		  else ($$).t = getype(Ftable,namext(($1).r));
		  $$.r=nmerge(4,$1.r,$2.r,$3.r,$4.r); 
	      }
    ;

opt_cast
    :	/* empty */  {$$.r=0; $$.t=0;}
    |	cast
    ;
cast : '(' abstract_declaration ')'
				{ $$.r=nmerge(3,$1.r,$2.r,$3.r); 
				  ($$).t = ($2).t;
				}
    ;
abstract_declaration
    : type abstract_declarator
			    {if(*vtype[decdpth] == 'o')
				{free($1.r);
				 $1.r = stral("int ");
				}
			     ($$.r) = vwrite( stral(stripStore($1.r)),$2.r);
			     ($$).t = vwrite(indrstr,vtype[decdpth]);
			     indrstr = 0;
			    }
    ;
abstract_declarator 
    :	empty_abstract_declarator
    |	nonempty_abstract_declarator
    ;
empty_abstract_declarator : /* empty */		{($$).r = 0; ($$).t = 0; }
    ;
nonempty_abstract_declarator
    : '(' nonempty_abstract_declarator ')'
		    { $$.r=nmerge(3,$1.r,$2.r,$3.r); 
		    }
    |  abstract_declarator '(' ')'
		{ $$.r=nmerge(3,$1.r,$2.r,$3.r);  
		  indrstr = vwrite (indrstr,"f"); /* "Function returning ..." */
		}
    | abstract_declarator '[' opt_expr ']'
		    { $$.r=nmerge(4,$1.r,$2.r,$3.r,$4.r); 
		      indrstr = vwrite(indrstr,"p"); /*"Array (vector) of..."*/
		    }
    | '*' abstract_declarator
		    { $$.r = vmerge($1.r,$2.r); 
		      indrstr = vwrite(indrstr,"p"); /* " pointer to ... " */
		    }
    ;
/* The block is a fundimental processing element.  A block count is kept for
 * scoping, and two separate output streams are maintained for each block, to
 * separate out declarations & statements.  In ClassC, statements & declarations
 * can be mixed; in standard c, not.  Each block element is scanned as it is
 * read, and put into either the statement stream or the declaration stream
 * for that block.  At the end of the block, the two streams are combined
 * into the statement stream of the enclosing block one level below.
 * This is done through dynamically allocated strings.
 */
block : '{' 
		      /* Save the source file name & line no */
		    { 
		      /*
		      tmppc = pline();
		      $$.r=vmerge(tmppc,$1.r);
		      */
		      blk_no ++;
		      $$.r=0;
		      dec_stk[blk_no] = 0;
		      stat_stk[blk_no] = 0;
		    }

	 phrase_list
	 '}'
		    /* Merge the declaration & statment streams & close block */
		    { 
		      if ((blk_no==1) && derefQ && debugQ )
		        $$.r=nmerge(5,$1.r,dec_stk[blk_no],funame,
				stat_stk[blk_no],stral("--_mct;\n"));
		      else
		        $$.r=nmerge(3,$1.r,dec_stk[blk_no],stat_stk[blk_no]);
		      /*
		       tmppc = pline();
		      ($$).r=*Zmerge(&($$).r,tmppc);
		      */
		      $$.r=vmerge($$.r,$4.r);
		      blk_no --; 
		      unwind(&Vtable,blk_no);
		    }
      ;
phrase_list : /* empty */ {$$.r = $$.t=dec_stk[blk_no] = stat_stk[blk_no]=0; }
	    | statement
			{stat_stk[blk_no] = vwrite(stat_stk[blk_no],($1).r); }
			/* This allows other parts of the parser to put in
			   statements (eg, from static object initializations)
			   before the first actual statement of the block is
			   parsed
			*/
			     
	    | declaration
		{ dec_stk[blk_no] = $1.r; }
	    | phrase_list statement
			{stat_stk[blk_no]=vmerge(stat_stk[blk_no],$2.r);}
	    | phrase_list declaration
		{ dec_stk[blk_no]=vmerge(dec_stk[blk_no],$2.r);}
	    ;
simple_statement  
    : ';'
    | comop_expr ';'
		{ $$.r=vmerge($1.r,$2.r);
		}
    | label ':' statement
		{ $$.r=nmerge(3,$1.r,$2.r,$3.r); }
    | IF '(' comop_expr ')' statement
	    { $$.r=nmerge(5,$1.r,$2.r,$3.r,$4.r,$5.r);
	    }
    | IF '(' comop_expr ')' statement ELSE statement
	    {$$.r=nmerge(7,$1.r,$2.r,$3.r,$4.r,$5.r,$6.r,$7.r);
	     }
    | WHILE '(' comop_expr ')' statement
	    { $$.r=nmerge(5,$1.r,$2.r,$3.r,$4.r,$5.r);
	    }
    | DO statement WHILE '(' comop_expr ')' ';'
	    {$$.r=nmerge(7,$1.r,$2.r,$3.r,$4.r,$5.r,$6.r,$7.r);
	     }
    | FOR '(' opt_expr ';' opt_expr ';' opt_expr ')' statement
	    { ($$).r=nmerge(9,$1.r,$2.r,$3.r,$4.r,$5.r,$6.r,$7.r,$8.r,$9.r);
	     }
    | SWITCH '(' comop_expr ')' statement
	    { ($$).r=nmerge(5,$1.r,$2.r,$3.r,$4.r,$5.r);
	    }
    | BREAK ';'
		{ $$.r = vmerge($1.r,$2.r); }
    | CONTINUE ';'
		{ $$.r = vmerge($1.r,$2.r); }
    | GOTO label ';'
		{ $$.r=nmerge(3,$1.r,$2.r,$3.r);}
    | RETURN opt_expr ';'
		{
		   if (debugQ)
			$$.r=nmerge(4,$1.r,stral(" --_mct,"),$2.r,$3.r);
		   else
		        $$.r=nmerge(3,$1.r,$2.r,$3.r);
		   if (InitQ) yyerror("Return not allowed in INIT function\n");
		}
    ;
label : IDENT 
      | DEFAULT
      | CASE comop_expr	
				{ $$.r = vmerge($1.r,$2.r); }
      ;
type :	FLOAT
			{vtype[decdpth] = stral("d");}
     |	CHAR
			{vtype[decdpth] = stral("i");}
     |	SHORT
			{vtype[decdpth] = stral("i");}
     |	INT
			{vtype[decdpth] = stral("i");}
     |	LONG
			{vtype[decdpth] = stral("l");}
     |	UNSGN CHAR
			{ ($$).r = vmerge($1.r,$2.r); 
			  vtype[decdpth] = stral("i"); }
     |	UNSGN SHORT
			{ ($$).r = vmerge($1.r,$2.r); 
			  vtype[decdpth] = stral("i"); }
     |	UNSGN 
			{vtype[decdpth] = stral("i");}
     |	UNSGN LONG
			{ ($$).r = vmerge($1.r,$2.r); 
			  vtype[decdpth] = stral("i"); }
     |	VOID
			{vtype[decdpth] = stral("v");}
     |	CLASS 

/* XXX Was -- But I think it should be this instead --
	    {vtype[decdpth] = stral("o");
*/
	    {vtype[decdpth] = stral("k");
	     class_funcs = 0;
	     CdefQ = 1;
	    }
	KNAME 
	    {
	    decdpth++;
	    Tmp_Vtbls[decdpth] = Vtable;
	    Tmp_Ftbls[decdpth] = Ftable;
	    tmpint[decdpth] = blk_no;
	    Vtable = &Vtail;
	    Ftable = &Ftail;
	    blk_no = 0;
	    cname = namext($3.r);
	    }
	pats '{' opt_dec_list '}'
	    { 
	    class_funcs = build_class_func_decs(Ftable);
	    make_class_entry($3.r,Vtable,Ftable,sym_tabl_tabl,&Ktable,patList);
	    if (patList) {free (patList); patList = 0;}
	    /* Vtable & Ftable contain the symbols from dec_list;
	       sym_tabl_tabl contains the various symbol tables (multiply
	       inherited) from pats. make_class_entry puts them all together
	    */
	    Vtable = Tmp_Vtbls[decdpth];
	    Ftable = Tmp_Ftbls[decdpth];
	    blk_no = tmpint[decdpth];
	    decdpth--;
	    tmppc = namext(($3).r);
	    vtype[decdpth]=vmerge(vtype[decdpth], tmppc);
	    free ($1.r); free ($7.r); free ($6.r);  
	    $6.r = vwrite (0,"\n\t{ struct _ref_el * _ref_arr;");
	    $1.r = stral("\nstruct ");
	    $7.r = build_class_entries(Ktable);
	    if (derefQ)
		tmppc = ztwrite(0,";\n",build_deref_arr (Ktable),0);
	    else
		tmppc = ztwrite(0,";\n",build_inline_arr (Ktable),0);
	    tmppc = vmerge(class_funcs,tmppc);
	    ($$).r= stral("\n");
	    classDecs=nmerge(8, classDecs,$1.r,$3.r,$6.r,$7.r,$8.r,
		stral(";\n"),tmppc);
	     cname = tmppc = class_funcs = 0;
	     CdefQ = 0;
	    }
     |	ag IDENT
	    { 
	     tmppc = namext(($2).r);
	     vtype[decdpth]=vmerge(vtype[decdpth], tmppc);
	     ($$).r = vmerge($1.r,$2.r); 
	    }
     |	ag 		/* ag { dec_list }   (Unnamed struct) */
	    {
	     decdpth++;
	     Tmp_Vtbls[decdpth] = Vtable;
	     tmpint[decdpth] = blk_no;
	     Vtable=_NEW(struct sym_entry);
	     blk_no = 0;
	    } 

	'{' dec_list '}'
	    { 
	      struct_cnt++;/* To name anonamous structs */
	      sprintf(tmpstr,"AnonAg%d",struct_cnt);
	      StrEnter(tmpstr,Vtable);
	      Vtable = Tmp_Vtbls[decdpth];
	      blk_no = tmpint[decdpth];
	      decdpth--;
	      vtype[decdpth]=vwrite(vtype[decdpth],tmpstr);
	      $$.r=nmerge(4,$1.r,$3.r,$4.r,$5.r); 
	    }
     |	ag IDENT  	/* ag IDENT { dec_list } */
	    {
	     decdpth++;
	     Tmp_Vtbls[decdpth] = Vtable;
	     tmpint[decdpth] = blk_no;
	     Vtable=_NEW(struct sym_entry);
	     /* Replace w.  Vtable = &Vtail; someday */
	     blk_no = 0;
	    } /* Attempted parser inheritance */

	'{' dec_list '}'
	    { 
	      StrEnter(($2).r,Vtable);
	      Vtable = Tmp_Vtbls[decdpth];
	      blk_no = tmpint[decdpth];
	      decdpth--;
	      tmppc = namext(($2).r);
	      vtype[decdpth]=vmerge(vtype[decdpth], tmppc);
	      ($$).r=nmerge(5,$1.r,$2.r,$4.r,$5.r,$6.r);
	    }
     | SYNTH	/*In case of a type SYNTH defined by TYPEDEF */
	    { /* Scan typedef symbol table -- if name is found, return
	       * the equivalent type string.  If not found, return 0; 
	       * error condition.
	       */
	      if ( indrstr = find(($1).r,Ttable) -> typestr)
		  vtype[decdpth] = stral(indrstr);
	      else /* name not found in typedef table */
		  {sprintf(errstr,"Unknown type name `%s'\n",($$).r);
		   yyerror(errstr);
		  }
	    }
     |	KNAME	/*Object variables are integers indexing into a table*/
	    { 
	      vtype[decdpth] = vwrite(stral("k"),namext($1.r));
	      /* If there is no explicit storage class specifier, and if we are
	       * not at the top level (that is, if we are inside a block) AND
	       * we are not in a formal paramter declaration (ArgQ ?), default
	       * storage class for object to "static".  This is to retain object
	       * identities between calls to functions. Otherwise, this would be
	       * a back door for garbage collection. The programmer who wants to
	       * can still get around this by explicitly declaring the storage
	       * class to be auto; perhaps to implement a recursive function.
	       */
	      if (garbQ && !storeQ && blk_no && !ArgQ )
			$$.r=stral("static int ");
	      else $$.r = stral( "int ");
	     }
    |	OBJ	/*Object variables are integers indexing into a table*/
	    { 
	      vtype[decdpth] = stral("o");
	      if (garbQ && !storeQ && blk_no && !ArgQ )
			$$.r=stral("static int ");
	      else $$.r = stral( "int ");
	     }
    ;
ag  :	STRUCT
				{vtype[decdpth] = stral("s");}
    |	UNION
				{vtype[decdpth] = stral("u");}
    |	ENUM
				{vtype[decdpth] = stral("e");}
    ;
pats
    :	/* empty */
	    { 
	      struct sym_tabl_list	* tmp_sslP;
	      if (patList) {free (patList); patList = 0;}
	      $$.r = 0; $$.t = 0; 
	      /* Free previous sym_tabl_tabl.  */
	      while (tmp_sslP = (sym_tabl_tabl->next))
		    {free (sym_tabl_tabl);
		     sym_tabl_tabl = tmp_sslP;
		    }
	     }
    |	pats ':' KNAME
	    {			
	    struct sym_tabl_list	* tmp_sslP;
	    struct sym_entry	* tmp_seP;
	    struct sym_entry	* tmp_feP;/* For func tables */
	    /* Find table entry for IDENT */
	    tmppc = namext($3.r);
	    patList = addPatList(tmppc,patList);
	    tmp_seP = get_sym_tbl(tmppc,Ktable);
	    if (!tmp_seP) printf ("Error in \"pat\" table building\n");
	    tmp_feP = ((struct class_entry *) find(tmppc,Ktable))->fnc_tbl;
	    free (tmppc);

	    /* Build inheretance table here. */
	    tmp_sslP = _NEW (struct sym_tabl_list);
	    tmp_sslP->next = sym_tabl_tabl;
	    sym_tabl_tabl = tmp_sslP;
	    sym_tabl_tabl->sym_table = tmp_seP; 
	    sym_tabl_tabl->fnc_tbl = tmp_feP; 
	}
    ;
ident_list
    :	IDENT
    |	ident_list ',' IDENT
	    { ($$).r = nmerge(3,$1.r,$2.r,$3.r);
	    }
    ;
opt_ident_list
    :	/* empty */	 {($$).r=0;}

    |	ident_list
    ;
opt_expr
    :	/* empty */	{($$).r=0;}
    |	comop_expr
    ;
uop :	'!'
    |	'~'
    |	'-'
    |	DINC
    ;
mop :	'*'
    |	'%'
    |	'/'
    ;
bitop 
    :	'^'
    |	'|'
    |	'&'
    ;
aop :	'+'
    |	'-'
    ;
shop 
    :	LSH
    |	RSH
    ;
relop 
    :	'<'
    |	'>'
    |	LEQ
    |	GEQ
    |	EQ
    |	NEQ
    ;
logop 
    :	AND
    |	OR
    ;
literal
    :	INTEGER 		{($$).t = stral("i");}
    | 	REAL			{($$).t = stral("d");}
    |	STR			{($$).t = stral("pi");}
    |	CHARACTER		{($$).t = stral("i");}
    ;

%%
