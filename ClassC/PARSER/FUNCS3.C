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
#include <fcntl.h>
#include <stdio.h>
#include "y.tab.h"
#include "common.h"
/*   Global definitions   */
FILE *	outFP,inFP;
int 	currline=1;
extern	int	nout; /* No output flag  --  set on detection of error */
char  	currfile[99] = "Degoba";
extern  char * prep;
extern  FILE * yyout, * yyin;
/*****************************************************************************/

/* Print a cpp line control statement  -- Unused in present implementation.
char * pline()
{char * line;
 line = (char *) calloc (72,1);
 sprintf(line,"\n#line %d \"%s\"\n",currline,currfile);
 return line;
}
*/

/***************************************************************************
 * char * vwrite(char * vfile, char * str); -- returns a REALLOC
 *	char * with the contents of vfile & str. Assumes vfile has been
 *	dynamically allocated; and that str is not.  Should be called by
 *	"vfile = vwrite(vfile,str);".  If called with vwrite(0,str),
 *	creates a new vfil and returns it.
 ---------------------------------------------------------------------------*/
char * vwrite(mfil,sstr)  char * mfil, *sstr;
{ if (mfil && sstr)
      mfil=strcat((char *)realloc(mfil,strlen(mfil)+strlen(sstr)+1),sstr);
  else if (sstr)
       mfil = strcpy ((char *) malloc (strlen (sstr) + 1), sstr);
  return mfil;
}


/***************************************************************************
 * char * vmerge(char * vfile, char * vfilem); -- like vwrite above, except
 *	assumes that the second argument (vfilem) was allocated by a malloc
 *	and should now be freed.
 ---------------------------------------------------------------------------*/
char * vmerge(mfil,mstr)  char * mfil, *mstr;
{ if (mfil && mstr)
      {mfil = (char *) realloc(mfil, strlen(mfil) + strlen (mstr) +6);
      strcat(mfil,mstr);
      free (mstr);
      mstr = 0;
      }
  if (mfil) return mfil;
  return mstr;
}

/***************************************************************************
 * yyerror(s) char * s; -- Reports error message with file name and line
 *	number and sets flag for no output file.
 ---------------------------------------------------------------------------*/
yyerror(s) char * s;
{nout = 1; /* nout -- Flag meaning an error in parsing, no output file is made*/
 fprintf (stderr,"<%s>:%d %s\n",currfile,currline,s);
}

/***************************************************************************
 * yywarn(s) char * s; -- Reports warning message with file name and line
 *	number but allows continued compilation and creation of output file.
 ---------------------------------------------------------------------------*/
yywarn(s) char * s;
{
 fprintf (stderr,"<%s>:%d %s\n",currfile,currline,s);
}

/***************************************************************************
 * pkdebug() -- Debug funtion -- prints the text read at the current line
 ---------------------------------------------------------------------------*/
pkdebug()
{printf("    <%s>:%d:YYTEXT=\"%s\"\n",currfile,currline,yytext);}


/***************************************************************************
 * namext (str) -- Takes a string argument that may have whitespace and
 *	info in it and returns the last string of non-whitespace characters.
 *	Works only with whitespace -- not other non-identifier characters.
 *	Considers "\002" as whitespace.
 ---------------------------------------------------------------------------*/
char * namext (str) char * str; 
{char * symbol;
 char c;
 int i;
 if (str <  (char *) 0x100) return (char *) 0;
 i = pkstrrspn(str," \002\t\n")+1;
 c = str[i];
 str [i] = 0; /*Truncates whitespace*/
 symbol = pkstrrpbrk(str," \002\t\n");
 symbol  ? symbol++  :  (symbol = str);
 symbol = vwrite(0,symbol);
 str [i] = c; /* Restores whitespace*/
 symbol[strcspn(symbol,"(")] = 0; /* This extracts Function names */
 return symbol;
}


/***************************************************************************
 * Uenter (name, typstr, decstr, blkn, table, abst_dec)  
 *	Symbol table entry routine.  Used for both function and variable
 *	symbol tables.
 ---------------------------------------------------------------------------*/
Uenter (name, typstr, decstr, blkn, table, abst_dec)  
int blkn; char * name, *typstr, *decstr, * abst_dec; struct sym_entry **table;
{    struct sym_entry *newentry = _NEW (struct sym_entry);
     extern int fdefQ, tdefQ;
     struct sym_entry *thumb = *table;
     int existsP = 0, conflictsP = 0;
     name = namext(name);
    /* Eliminate storage class specifiers from abst_dec's */
     abst_dec = stripStore (abst_dec);
     while (thumb->next && thumb->blk_no == blkn)
	{if (strcmp(thumb->name,name) == 0) 
		{existsP = 1;
		 if (strcmp (thumb->typestr,typstr) != 0) conflictsP = 1;
		}
	 thumb = thumb-> next;
	}
     if (! existsP)
	 {newentry->name = vwrite(0,name);
	  newentry->typestr= vwrite(0,typstr);
	  newentry->declaration = vwrite(0,decstr);
	  newentry->blk_no = blkn;
	  newentry->next = *table;
	  newentry->abstract_dec = vwrite(0,abst_dec);
	  *table = newentry;
	 }
/*   error routine if (existsP)  */
  if (existsP && conflictsP) 
    {if (fdefQ)
	errpt("Conflicting redeclaration of function <%s>\n",name);
     else if (tdefQ) errpt ("Conflicting redeclaration of typedef <%s>\n",name);
     else errpt ("Conflicting redeclaration of variable <%s>\n",name);
    }
  free (name);
}



/***************************************************************************
 * StrEnter(name,sym_table) char * name; struct sym_entry * sym_table;
 *	Structure symbol table entry routine
 ---------------------------------------------------------------------------*/
extern struct struct_entry * Stable;
/* Structure definition entry table */
StrEnter(name,sym_table) char * name; struct sym_entry * sym_table;
{
  int existsP = 0;
  extern struct struct_entry * Stable;
  struct struct_entry * new_entry = _NEW (struct struct_entry);
  struct struct_entry * scan;
  name = namext(name);
  scan = Stable;
  while (scan->next)
      {if (strcmp(scan->name,name) == 0) existsP = 1;
       scan = scan->next;
      }
  if (! existsP) 
    { new_entry->next = Stable;
      new_entry->sym_table = sym_table;
      new_entry->name = vwrite(0,name);
      Stable = new_entry;
    }
  else 
    {
    sprintf(errstr,"Redefinition of struct %s\n",name);
    yyerror(errstr);
    }
  free (name);
}


/***************************************************************************
 *  find() -- Returns pointer to the entry of the named element in given table;
 *	or 0.  Will work with any type table structure that has as
 *	its first two components { struct * next; char * name; ... }
 *	CAST return value of find() to the proper table type.
 ---------------------------------------------------------------------------*/
struct sym_entry * find (symbol, table) char * symbol; struct sym_entry * table;
{
 symbol = namext(symbol);
 while (table->next)
     {
      if (strcmp (table->name,symbol) == 0) 
			{free (symbol);
			 return table;
			}
      table = table-> next;
     }
 free (symbol);
 return 0;
}


/***************************************************************************
 * find_cl() -- Takes string argument & returns pointer to class entry, or 0
 ---------------------------------------------------------------------------*/
struct class_entry * cl_find (symbol) char * symbol;
{struct class_entry * table = Ktable;
 symbol = namext(symbol);
 while (table->next)
     {
      if (strcmp (table->name,symbol) == 0) 
			{free (symbol);
			 return table;
			}
      table = table-> next;
     }
 free (symbol);
 return 0;
}

#ifndef NOVARARGS

/***************************************************************************
 * char * nmerge(int n, [char ** str ]*n ); -- 
 *	Takes pointers as arguments -- merges them into a
 *	single string & frees each of the original strings (hence assumes
 *	that they were dynamically allocated w. malloc).  Called with
 *	"str = vmerge (3, str, str2, str3);"
 ---------------------------------------------------------------------------*/
char * nmerge(va_alist)  va_dcl
{ 
  va_list argP;
  char * mfil, *mstr;
  int i, argN;
  va_start(argP);
  argN = va_arg(argP,int);
  if (argN < 2) printf("Bad call to nmerge -- argN = %d\n",argN);
  mfil = va_arg(argP, char *);
  for (i = 1; i < argN; i++)
  {
      mstr = va_arg(argP, char *);
      if (mfil && mstr)
	  {mfil = (char *) realloc(mfil, strlen(mfil) + strlen (mstr) +6);
	  strcat(mfil,mstr);
	  free (mstr);
	  }
      else if (mstr) mfil = mstr;
   }
   return mfil;
}
#endif
/***************************************************************************
 * nwrite ( int no_args, char * ~~~) -- vararg function.  Combines many strings
 *	into one.  The first argument is the count.  The second argument
 *	is either a previously DYNAMICALLY allocated string, in which
 *	case it is realloced, or it is ZERO, in which case new memory
 *	is malloced.  The remainder of the character strings are left
 *	unmollested
 ---------------------------------------------------------------------------*/
char * nwrite(va_alist)  va_dcl
{ 
  va_list argP;
  char * mfil, *mstr, *pc;
  int i, argN;
  va_start(argP);
  argN = va_arg(argP,int);
  mfil = va_arg(argP,char *);
  mstr = va_arg(argP, char *);
  while (argN--)
     {if (mstr)
	{ if (mfil) mfil=(char *)realloc(mfil, strlen(mfil) + strlen (mstr) +6);
	  else mfil = (char *) calloc (1, strlen(mstr) + 4);
	  strcat(mfil,mstr);
	}
      mstr = va_arg(argP, char *);
      }
   va_end(argP);
   return mfil ;
}

/***************************************************************************
 * errpt(va_alist) va_dcl -- Variable argument version of yyerror -- uses
 *	vprintf();
 ---------------------------------------------------------------------------*/
errpt(va_alist) va_dcl
{va_list ap;
 char * fmt;
 fprintf (stderr,"<%s>:%d ",currfile,currline);
 va_start(ap);
 fmt = va_arg(ap,char *);
 vfprintf( stderr, fmt, ap);
 nout = 1;
}


/***************************************************************************
 * wrrpt(va_alist) va_dcl -- warnint report -- Like errpt() above except
 *	does not disrupt output -- only warns.
 ---------------------------------------------------------------------------*/
wrrpt(va_alist) va_dcl
{va_list ap;
 char * fmt;
 fprintf (stderr,"<%s>:%d Warning: ",currfile,currline);
 va_start(ap);
 fmt = va_arg(ap,char *);
 vfprintf( stderr, fmt, ap);
}



/***************************************************************************
 * ztwrite ( char * ~~~, 0) -- vararg function.  Combines many strings
 *	into one.  The first argument is either a previously DYNAMICALLY
 *	allocated string, in which case it is realloced, or it is ZERO,
 *	in which case new memory is malloced.  The remainder of the
 *	character strings are left unmollested.  NULL (0) terminates the
 *	function.
 ---------------------------------------------------------------------------*/
char * ztwrite(va_alist)  va_dcl
{ 
  va_list argP;
  char * mfil, *mstr, *pc;
  int i, argN;
  va_start(argP);
  mfil = va_arg(argP,char *);
  mstr = va_arg(argP, char *);
  while (mstr)
     {if (mfil) mfil= (char *)realloc(mfil, strlen(mfil) + strlen (mstr) +6);
      else mfil = (char *) calloc (1, strlen(mstr) + 4);
      strcat(mfil,mstr);
      mstr = va_arg(argP, char *);
      }
   va_end(argP);
   return mfil ;
}

/***************************************************************************
 * char * stral(str) char * str;
 *	Saves a string into a dynamically allocated memory.
 ---------------------------------------------------------------------------*/
char * stral(str) char * str;
{
 char * newstr;
/*
 printf("string passed to stral: <%s>\n",str);
*/
 newstr  = (char *) malloc(strlen(str) + 1);
 strcpy (newstr,str);
 return newstr;
}

/***************************************************************************
 * char *buildClassList(str)char * str;
 * 	First pass of the ClassC parrser; just builds a list of all classes
 *	to allow forward references to class names.
 ---------------------------------------------------------------------------*/
char *buildClassList(str)char * str;
{ 
  int j;
  int i;
  while (i = yylex())
  if (i == CLASS)
    { j = yylex();
      if (j == IDENT)
	  str = vwrite(vwrite(str,namext(yytext)),":");
      else if (j == KNAME)
	{
	  sprintf(errstr,"Redefinition of class <%s>\n",yytext);
	  yyerror(errstr);
	}
      else
	{
	  yyerror(errstr,"Bad use of keyword \"class\"");
	  yyerror(errstr);
	}
    }
  return str;
}

/***************************************************************************
 * char * stripStore( str) char * str;
 *	Strips storage class words from type strings.
 ---------------------------------------------------------------------------*/
char * stripStore( str) char * str;
{ 
  while ( (*str == ' ') || (*str == '\t') || (*str == '\n') ) str ++;
  if (( ! strncmp (str, "static",6) ) || ( ! strncmp (str, "extern",6) ) )
	{ str += 6;
	  while ( (*str == ' ') || (*str == '\t') || (*str == '\n') ) str ++;
	}
  return str;

}

/***************************************************************************
 * char * prep -- NOT a function; just a string prepended to all output
 *	files containing necessary declarations.
 ---------------------------------------------------------------------------*/
char * drfprep =
"extern int _indArr[];\n\
 extern char * _whome[99];\n\
 extern int _mct;\n\
 struct _ref_el	{char * name; int offset; };\n\
 int  _deref();\n\
 int * _decref();\n\
 int _incref();\n";

char * inlprep =
"#define hex unsigned long\n\
 extern int _indArr[];\n\
 extern int _mct;\n\
 struct _ref_el	{char * name; int offset; };\n\
 int * _decref();\n\
 int _incref();\n\
 struct _TblEntry \n\
 {	int	ref_cnt;\n\
	int 	self; /*Really an undefined struct pointer */\n\
	int	dirtyQ;	/* Flag -- is this entry free? */\n\
	int *	hofV;   /* Table of offsets */\n\
	int *	hofF;   /* Table of offsets */\n\
	int	hashV; /* The hash value for the entry */\n\
	int	hashF; /* The hash value for the entry */\n\
 };\n\
 extern struct _TblEntry * _objTbl;\n\
 extern struct _TblEntry * _objV[99];\n\
#undef hex\n";
