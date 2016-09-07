/*             %M%    %I%  %H%  %G%                */
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
/* file FUNCTIONS.C -- Functions for the ClassC compiler by Paul Kirkaas */

#include <stdio.h>
#include "common.h"
#define isObject(str) ( (*str == 'k') || (*str == 'o' ) )


struct RefEl {char * name; char * offset; };
extern int fdefQ;
extern int mdefQ;
extern int tdefQ;
extern int CdefQ;
extern int mainQ;
extern int storeQ;
extern int nout;
extern char * libstr;
extern char * vtype[99];
extern char * indrstr;
extern char * vname;
extern char *(stat_stk[9]), *(dec_stk[9]);
extern struct sym_entry    Vtail,
		    Ftail,
		   *Vtable,
		   *Ftable,
		   *Ttable,
		   *Tmp_Vtbls[9],
		   *Tmp_Ftbls[9];
extern struct struct_entry Stail,
		   *Stable;
extern struct class_entry	 Ktail,
			*Ktable;
struct membr_fncs  
		   {	struct membr_fncs * next;
			char * name;
			struct sym_entry * fnc_entry;
		    };
struct sym_tabl_list 
	{  struct sym_tabl_list * next;
	   struct sym_entry 	* sym_table;
	   struct sym_entry 	* fnc_tbl;
	};
struct sym_tabl_list	 sym_tabl_tailt,
			*sym_tabl_tabl;
extern int struct_cnt;
extern int blk_no;
extern int tmpint[99];
extern int decdpth;
extern char tmpstr[99]; 
extern char *tmppc;
extern char * type_str;
extern char * dec_str;
extern char * cast_str;
extern char * abst_dec_str;
extern char * func_list;

/*****************************************************************************
 * char * mystrtok(list,sep) char * list, *sep;
 *	Like the C strtok()  function.  Only works with one list at a time,
 *	and I need two.
 ----------------------------------------------------------------------------*/
char * mystrtok(list,sep) char * list, *sep;
	{ static char *next;
	  char * start;
	  if (list) next = list;
	  if (! next) return 0;
	  next += strspn(next,sep);
	  if (! *next) return next = 0;
	  start = next;
	  next = strpbrk(next,sep);
	  if (next)  *next = 0; ++next;
	  return start;
	}


/*****************************************************************************
 * unwind(table,n) struct sym_entry **table; int n;
 *	Scoping function.  When a block is exited, all the variables
 *	defined in that block are gone and removed from the symbol table.
 *	Unwind symbol table down TO block number n 
 ----------------------------------------------------------------------------*/
unwind(table,n) struct sym_entry **table; int n;
{ struct sym_entry * pst;
  while ((*table)->next)
    { if ((*table)->blk_no <= n) break;
      pst = *table;
      *table = (*table)-> next;
      free (pst);
    }
}
      
/*****************************************************************************
 * char * getype(table,name) struct sym_entry *table; char * name;
 *	Gets the type of a variable or function from the symbol appropriate
 *	symbol table.
 ----------------------------------------------------------------------------*/
char * getype(table,name) struct sym_entry *table; char * name;
{
 extern struct sym_entry    *Ftable;
 name = namext (name);
 while (table->next)
   {if(strcmp(name,table->name)==0) return free(name), stral(table->typestr);
    table = table->next;
   }
  if (strcmp (table->name,"FncT") == 0) 
      {if (0) fprintf(stderr,
	    "Warning: Function %s used before declared; default to int\n",name);
       Uenter(name,"i",dec_str,0,&Ftable,"int");
       free (name);
       return (stral("i"));
      }
  else
    {
     table = Ftable; /* Search Ftable, if found, IDENT is a function ptr */
     while (table->next)
      {  if(strcmp(name,table->name)==0)
		return free(name), vwrite(stral("p"),table->typestr);
         table = table->next;
      }
     sprintf(errstr,"%s used before declared\n",name);
     yyerror(errstr);
    }
  free (name);
  return 0;
}


/*****************************************************************************
 *cdr(typestr,derefstr) -- returns a malloc'ed string equal to the
 *	tail of the input "typestr" with the initial sequence found in
 *	"derefstr" removed.  If "deref" is not an initial sequence of 
 *	"typestr", return an error.
 ----------------------------------------------------------------------------*/
char * cdr(type,deref) char * type; char deref;
{char * pc;
 if (*type == deref ) 
     { pc = stral(&type[1]); free (type); return pc; }
 sprintf(errstr,"Illegal dereference <%s>\n",type);
 yyerror(errstr);
}

/*****************************************************************************
 * char * icon(t1,t2,op,aopQ) char *t1, *t2, *op; int aopQ;
 *	Implicit conversion of types -- float + int -> float --- etc.
 *	t1 & t2 are the type strings of the operation arguments.
 *	op is the lex returned string that represents the operand for error
 *	reporting purposes.
 *	aopQ is an add op? flag, which is set if pointer/integer
 *	ops are allowed.
 ----------------------------------------------------------------------------*/
char * icon(t1,t2,op,aopQ) char *t1, *t2, *op; int aopQ;
{
 if (strcmp(t1,t2) == 0)  /* Both arguments have same type */
     { /* If that type is Int or Float, return it */
	if( (*t1 == 'i') || (*t1 == 'd') || (aopQ && (*t1 == 'p'))) return t1; 
	sprintf(errstr,"Illegal combination of types: \"%s %s %s\"\n",t1,op,t2);
	yyerror(errstr);
	return 0;
     }
 if ((*t1 == 'd') && (*t2 == 'i'))		return t1; 
 if ((*t2 == 'd') && (*t1 == 'i')) 		return t2;
 if (aopQ && (*t1 == 'p') && (*t2 == 'i'))		return t1; 
 if (aopQ && (*t2 == 'p') && (*t1 == 'i'))		return t2; 
 sprintf(errstr,"Illegal combination of types: \"%s %s %s\"\n",t1,op,t2);
 yyerror(errstr);
 return 0;
}


/*****************************************************************************
 * char * getmemtype(ag,el) char *ag, *el;
 *	Gets the type of the member of an aggregate (struct, union, etc)
 ----------------------------------------------------------------------------*/
char * getmemtype(ag,el) char *ag, *el;
{ 
  struct sym_entry * stable;
  struct struct_entry * atable;
  char *typestr;
  if ( (*ag != 's') && (*ag != 'u' ) && (*ag != 'o') )
	{ sprintf(errstr,"Incorrect type for aggregate dereference\n");
	  yyerror(errstr);
	  return 0;
	}
  ag = namext(&ag[1]);
  el = namext(el);
  atable = Stable;
  while (atable->next)
    {if (strcmp(atable->name,ag)== 0)
       {typestr = find(el,atable->sym_table) -> typestr;
        if (! typestr)
		{
		sprintf(errstr,"Symbol %s not found in aggregate %s\n",el,ag);
		yyerror(errstr);
		}
	free (ag); free (el);
        return stral(typestr);
       }
    atable = atable->next;
    }
  sprintf(errstr,"Aggregate %s not defined\n",ag);
  yyerror(errstr);
  return 0;
}

 
/*****************************************************************************
 * struct sym_entry * get_sym_tbl(name,Ktbl)
 *	Gets symbol information for the given symbol name of the class
 *	member.
 ----------------------------------------------------------------------------*/
struct sym_entry * get_sym_tbl(name,Ktbl)
	char * name;	struct class_entry * Ktbl;
{
 struct sym_entry * tmp_seP;
 name = namext(name);
 while (Ktbl->next)
     { if (! strcmp (Ktbl->name,name)) return Ktbl->sym_table;
       Ktbl = Ktbl->next;
     }
 return 0;
}



/*****************************************************************************
 * make_class_entry(name,Vtbl,Ftbl,sym_tbl_tbl,Ktbl,patList)
 *	Makes a new entry in the class table
 ----------------------------------------------------------------------------*/
make_class_entry(name,Vtbl,Ftbl,sym_tbl_tbl,Ktbl,patList)
	char * name;		struct sym_tabl_list * sym_tbl_tbl; 
	struct sym_entry *Vtbl, *Ftbl;	struct class_entry ** Ktbl;
	char * patList;
{ struct sym_entry	* tmp_seP, * chkseP;
  struct sym_entry	* tmp_feP = Ftbl;
  struct sym_tabl_list	* tmp_stlP;
  struct class_entry	* tmp_ceP;
  char * tmp_cP = 0;
  char * tmp_var_name = 0;
  name = namext(name);
  /* First make the initial function table from Ftbl.  This differs from
   * Ftbl only by having an extra string which is the local (non-qualified)
   * name of the function.  Ex  - if the class is Gaylord and the function
   * is sing(), then the local name is sing() and the real name is
   * _Gaylord_sing().  If any other class descend from Gaylord, we must
   * know both the local and the real name of the functions. */
  while (tmp_feP->next)
    {tmp_feP->spare = 
	nmerge(4,stral("_"),namext(name),stral("_"),namext(tmp_feP->name));
     tmp_feP = tmp_feP->next;
    }
  while (sym_tbl_tbl->next)
    { /*  Build up the variabe table */
      while (sym_tbl_tbl -> sym_table -> next)
	{ if ( ! (chkseP = find (sym_tbl_tbl -> sym_table -> name, Vtbl) ) )
			/* If not find this variable declared in an ancestor */
	    { tmp_seP = Vtbl;
	      Vtbl = _NEW(struct sym_entry);
	      *Vtbl = * (sym_tbl_tbl -> sym_table);
	      Vtbl -> next = tmp_seP;
	    }
	  else /* Check to see that the types are the same */
	    { if ( ! isObject( chkseP->typestr) &&
/** XXX should really check that descendent declaration descendent type (isa) */
		  (strcmp (chkseP->typestr, sym_tbl_tbl->sym_table->typestr)))
	      /* Types are different ...*/
		{
		    sprintf(errstr,
		      "Error -- member <%s> re-declared a different type %s\n",
		      chkseP->name,"from ancestor");
		    yyerror(errstr);
		}
	    }
	  sym_tbl_tbl -> sym_table = sym_tbl_tbl -> sym_table -> next;
	}
	/* Now build up the function table */
      while (sym_tbl_tbl -> fnc_tbl -> next)
	{ if ( !( chkseP = find (sym_tbl_tbl -> fnc_tbl -> name, Ftbl) ) )
	    { tmp_feP = Ftbl;
	      Ftbl = _NEW(struct sym_entry);
	      *Ftbl = * (sym_tbl_tbl -> fnc_tbl);
	      Ftbl -> next = tmp_feP;
	    }
	  else /* Check to see that the types are the same */
	    { if ( ! isObject( chkseP->typestr) &&
/** XXX should really check that descendent declaration descendent type (isa) */
	          (strcmp (chkseP->typestr, sym_tbl_tbl->fnc_tbl  ->typestr)))
	      /* Types are different ...*/
		{
		  sprintf(errstr,
			"Member <%s> re-declared a different type %s\n",
			chkseP->name,"from ancestor");
		  yyerror(errstr);
		}
	    }
	  sym_tbl_tbl -> fnc_tbl = sym_tbl_tbl -> fnc_tbl -> next;
	}
      sym_tbl_tbl = sym_tbl_tbl -> next;
    }
  tmp_ceP = *Ktbl;
  *Ktbl = _NEW (struct class_entry);
  (*Ktbl) ->next = tmp_ceP;
  (*Ktbl) -> sym_table = Vtbl;
  (*Ktbl) -> fnc_tbl = Ftbl;
  (*Ktbl) -> name = name;
  (*Ktbl) -> ancList = ( patList ?  stral(patList) : 0 );
   if ( tmp_feP = (struct sym_entry *) find ("init", Ftbl) )
	(*Ktbl) -> initQ = tmp_feP->spare;
}


/*****************************************************************************
 * char * build_class_func_decs(Ftbl) struct sym_entry * Ftbl;
 *	Makes all the function declarations for member functions declared in
 *	a class entry.  This entails prepending the class name to the 
 *	member function name.
 ----------------------------------------------------------------------------*/
char * build_class_func_decs(Ftbl) struct sym_entry * Ftbl;
{ char * tmp_cP = stral("\n");
  struct sym_entry * tmp_feP = Ftbl;
  while (tmp_feP->next)
     {
#if NOVARARGS
	tmp_cP = vwrite(vwrite(tmp_cP,tmp_feP->declaration),";\n");
#else
	tmp_cP = ztwrite(tmp_cP,tmp_feP->declaration,";\n",0);
#endif
	tmp_feP = tmp_feP->next;
     }
  if (pkdbg) printf ("In bldclsfncdcs; cP = <%s>\n",tmp_cP);
  return tmp_cP;
}

/*****************************************************************************
 * char * build_class_entries(Ktbl)	struct class_entry * Ktbl;
 *	Builds the structure corresponding to the class given.
 *	This means declaring all the variable members of the class,
 *	those declared explicitly to be part of the class as well as
 *	those member variables inherited from ancestor classes
 ----------------------------------------------------------------------------*/
char * build_class_entries(Ktbl)	struct class_entry * Ktbl;
{char * tmp_cP = 0;
 struct sym_entry	* tmp_seP = Ktbl -> sym_table;
 while (tmp_seP->next)
     {
#if NOVARARGS
      tmp_cP = vwrite(vwrite(vwrite(tmp_cP,"\n\t  "),tmp_seP->declaration),";");
#else
      tmp_cP = ztwrite(tmp_cP,"\n\t  ",tmp_seP->declaration,";",0);
#endif
      tmp_seP = tmp_seP->next;
     }
 tmp_cP = vwrite (tmp_cP,"\n\t");
 return tmp_cP;
}
/*****************************************************************************
 * inList () -- takes a token and a list of tokens separated by ":"'s and 
 *	returns true if the token is in the list; else 0
 ----------------------------------------------------------------------------*/
inList (str, list) char * str; char * list;
{char * here;
 if ((! (str && list)) || ! strlen(list) || ! strlen(str)) return 0;
 str = namext(str);
 list = stral(list);
 here = namext (mystrtok(list,":"));
 if ((!strlen(here))||(!strcmp(here,str)))
	{
	free(list);
	free(str);
	if (here) free(here);
	return 1;
	}
 while (free(here), here = namext(mystrtok(0,":")))
     if (! strcmp(here,str) ) return free(list), free(str), free(here), 1;
 free(list); free(str);
 return 0;
}


/*****************************************************************************
 * char * pkstrrpbrk(str,set) char * str; char * set;
 *	Searches for the last occurrence of a character from SET in STR
 *	and returns character pointer to it; or NULL if not found.
 ----------------------------------------------------------------------------*/
char * pkstrrpbrk(str,set) char * str; char * set;
{char * reg=0, * hold=0;
 int i;
 int setN = strlen(set);
 for (i = 0; i < setN; i++) if (reg < (hold =strrchr(str,set[i]))) reg = hold;
 return reg;
}


/*****************************************************************************
 * pkstrrspn () -- NOT standard c -- but follows c conventions.
 *	strspn(str,set) searches a string and returns the index of
 *	the first character NOT in the set; strrspn(str,set) does the
 *	same, but from the right -- so returns the index of the LAST
 *	character NOT found in the set.  If there is no character in
 *	str not found in set, returns -1
 ----------------------------------------------------------------------------*/
pkstrrspn(str,set) char * str, *set;
{ 
  char * pc;
  pc = str + strlen(str);
  while (pc >= str)
	{ if (!strrchr(set,*pc)) break; pc--; }
  return (int) (pc - str);
}

/* comobj (typ1, typ2) --
 * Are these object assignment compatable?  Either, one of them is of type
 * "object", in which case, yes (forced); or they are the same, in which case
 * yes; or the first is an ancestor of the second  in which
 * case yes. Otherwise, not.
 */
comobj(typ1, typ2) char * typ1; char * typ2; 
{
    struct class_entry * ceP;
    char * k1;
    char * k2;
    char * inc;
    char * list;
    int i=0;
    if ( ! isObject(typ1) || !isObject (typ2) ) return 0;
    if (( *typ1 == 'o') || (*typ2 == 'o' )) return 1;
    if (! strcmp(typ1, typ2)) return 1;
    k1 = namext(typ1 + 1);
    k2 = namext(typ2 + 1);
    if (! (ceP = (struct class_entry *) find (k2,Ktable))) return 0;
    list = stral(ceP->ancList);
    if  (inList (k1,list)) return free (list), 1;
    return 0;
}

/* addPatList () -- not only adds name to pat list, but goes through
 * name's pat list and adds those members that are not already on the list */
char * addPatList(name, patList)
char * name; char * patList;
{
  char * nlist;
  char * tname;
  struct class_entry * ceP;
  name = namext(name);
  ceP = (struct class_entry *) find (name,Ktable);
  if (! ceP)
	{
	  errpt ("Ancestor <%s> not found\n",name);
	  return 0;
	}
  nlist = stral(ceP->ancList);
  tname = strtok(nlist,":");
  if (! inList (tname, patList) )
	patList = ztwrite(patList,tname,":",0);
  while (tname = strtok(0,":"))
      if (! inList (tname, patList) ) patList = ztwrite(patList,tname,":",0);
  if (! inList (name, patList) ) patList = ztwrite(patList,name,":",0);

  free (nlist); free (name); 
  return patList;
}

char * intostr(i) int i;
{ static char  s [99];
  sprintf (s,"%d",i);
  return s;
}
/* ****** Macro's 
cf(sprintf(errstr,

oyyerror(errstr);
 
*/
