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


struct RefEl {char * name; char * offset; };
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

/*****************************************************************************
 * char * build_deref_arr(Ktbl)
 *	Builds the dereferencing arrays used by _deref() to get the
 *	correct component of the class.
 *		This is the original "_deref" version.
 ----------------------------------------------------------------------------*/
char * build_deref_arr(Ktbl)
	 struct class_entry * Ktbl;
{ 
  struct sym_entry	* tmp_seP = Ktbl -> sym_table;
  struct sym_entry	* tmp_feP = Ktbl -> fnc_tbl;
  char * class_name = namext (Ktbl -> name);
  char * tmp_cP=ztwrite(0,"\nstatic struct _ref_el _",
	class_name, "Arr [] =\n\t{",0);
  /* This implementation assumes that member functions are not pointers but
   * externally defined funtions.  Thus, the ref_array is divided into two
   * parts.  The first is the dereferenceing for all the member variables.
   * This section is delimited by the null string "". (change to 0?) The
   * section section returns pointers to functions (cast as integers)
   * and is delimited by a null pointer (0).
   */
  while ( tmp_seP -> next )
    {
      tmp_cP=ztwrite( tmp_cP," { \"",tmp_seP->name, "\",\t(int)&((struct ",
		class_name, " * ) 0) ->", tmp_seP-> name, " },\n\t ",0);
      tmp_seP = tmp_seP->next;
    }
  tmp_cP=vwrite(tmp_cP, " { 0 , 0 },\n\t ");
  /* Add the class name here, like a function, so returns absolute address */
  tmp_cP=ztwrite(tmp_cP, " { \"_name\" , (int)\"",class_name,"\" },\n\t ",0);
  while ( tmp_feP -> next )
    {
      tmp_cP=ztwrite( tmp_cP," { \"",tmp_feP->name, "\",\t(int) ",
		tmp_feP-> spare, " },\n\t ",0);
      tmp_feP = tmp_feP->next;
    }
  tmp_cP=vwrite(tmp_cP, " {  0 , 0 }\n\t};\n");
  return tmp_cP;
}

/*****************************************************************************
 * char * build_inline_arr(Ktbl)
 *	Builds the dereferencing arrays used by in inline dereferencing
 	Sends generated array to mkRefAr, which then returns the
 * 	string to be returned.
 ----------------------------------------------------------------------------*/
char * build_inline_arr(Ktbl)
	 struct class_entry * Ktbl;
{ 
  struct RefEl	* refElV = (struct RefEl *) calloc(1,sizeof (struct RefEl));
  struct RefEl	* refElF = (struct RefEl *) calloc(1,sizeof (struct RefEl));
  struct sym_entry	* tmp_seP = Ktbl -> sym_table;
  struct sym_entry	* tmp_feP = Ktbl -> fnc_tbl;
  char * class_name = namext (Ktbl -> name);
  int tableSize=0;
  refElV->name=class_name;
  while ( tmp_seP -> next )
    {
      refElV = (struct RefEl *)
		realloc (refElV,(tableSize++ +4) * sizeof (struct RefEl) );
      refElV[tableSize].name = tmp_seP-> name;
      refElV[tableSize].offset =
      vwrite(vwrite(vwrite(vwrite (0,"(int)&((struct "),
		class_name), " * ) 0) ->"), tmp_seP-> name);
      tmp_seP = tmp_seP->next;
    }
  refElV[tableSize+1].name=0;
  refElV[tableSize+1].offset=0;
/* First improvement in version 2 - separate tables for functions and
   variables -- better use of space, as waste grows geometrically */
  tableSize=-1;
  while ( tmp_feP -> next )
    {
      refElF = (struct RefEl *)
		realloc (refElF,(tableSize++ +4) * sizeof (struct RefEl) );
      refElF[tableSize].name = namext (tmp_feP-> name);
      refElF[tableSize].offset = 
	  vwrite(vwrite (0,"(int) "), tmp_feP-> spare);
      tmp_feP = tmp_feP->next;
    }
  refElF[tableSize+1].name=0;
  refElF[tableSize+1].offset=0;
/*
{ char * tmp;
  tmp = mkRefAr(refElV,refElF);
printf("Returned array = \n%s\n\n",tmp);
  return tmp;
}
*/
 return mkRefAr(refElV,refElF);
}
#define MAXMASKRATIO 16
#define CharacterToNumber(c) 						\
    ((unsigned long)(( (c >= 'A') && (c <= 'Z') ) ?			\
	c - 'A' :( ((c >= 'a') && (c <= 'z')) ?				\
		c - 'a' + 26 : ( ( (c >= '0') &&  (c <= '9') ) ?	\
			c - '0' + 52 : ( (c == '_') ? 62 : 63)) )))

struct Table {unsigned long number; int hash; char * offset;};


unsigned long itoj(i,j) int i,j;

{ unsigned long count, retval = 1;
 for (count = 0; count < j; count++ ) retval *= i; 
 return retval;
}


unsigned long nameToNumber(name) char * name;
/*  Note that arithmetic overflow of unsigned integers is guaranteed to
 *  produce the mathematically correct result mod (sizeof (unsigned) ^ 2)
 */
{ int i = 0;
  char c;
  unsigned long retval=0;
  while (c = name[i])
    { retval += CharacterToNumber (c) * itoj(63,i);
      i++;
    }
  return retval;
}

/*****************************************************************************
 * char * mkNum(str) char * str;
 * 	Converts a name to an integer & the integer to a string 
 *---------------------------------------------------------------------------*/
char * mkNum(str) char * str;
{ unsigned long n;
  char * retstr = (char *) malloc(strlen (str) + 30);
  n = nameToNumber(str);
  sprintf(retstr,"/* \"%s\" */ 0x%x ",str,n);
  return retstr;
}
  


cmpTbl(table1,table2) struct Table * table1, * table2;
{ if (table1->hash > table2->hash) return 1;
  if (table1->hash < table2->hash) return -1;
  return 0;
}
      
/* Takes a list of name/offset pairs; the first pair is special and is 
 *  assumed to contain the name of the class.
 *  Converts the names (character strings) to long integers via
 *  "NameToNumber()".  Then the numbers are hashed by one of two methods --
 *	1) Binary Mask -- A binary mask is used and doubled until the
 *  generated numbers are unique.  This is a fast procedure, but can waste a 
 *  lot of space.  Therefore, if the size of the table becomes more than 16
 *  times the number of entries in it, 
 *	2) Modulo -- "mod" starts at the number of entries in the table and
 *  increments by one until all generated numbers are unique.  This is
 *  slower than masking, but takes a fraction of the space for large tables.
 *  	Then, a string is returned which contains C declarations for 3
 *  variables:
 *	1) A flag ("int _<classname>Fl = 0|1;") whose value indicates
 *  whether modulo or masking was used.
 *	2) An integer ("int _<classname>Hs = XX;") whose value is the
 *  mask/mod  value to be used at dereference time.
 *	3) The dereference array itself ("int * _<classname>Ar[] = XX,XX,XX;")
 *  in the form of offsets and zeroes (empty placeholders).
 */

char * mkRefAr (Varray,Farray) struct RefEl * Varray, *Farray;
{ int i,j,k;
  char * RetStr = 0;
  int RetStrSz  = 0;
  int hash;
  int maxmask;
  int finalSize;
  int curmax;
  int arraySize;
  int fl;
  char * name = Varray->name;
  struct RefEl * array = Varray + 1;
  struct Table * table =0;
  for (fl = 0; fl <2; fl++)
  {
      arraySize = 0;
      finalSize = curmax= -1;
      hash = 1;
      if (fl) array = Farray;
      while (array[arraySize].name) arraySize ++;
      maxmask = MAXMASKRATIO * arraySize;
      table = (struct Table *) calloc(arraySize+2,sizeof(struct Table));
      for (i=0; i < arraySize; i++) 
	 { 
	    table[i].number = nameToNumber(array[i].name);
	    table[i].offset = array[i].offset;
	 }
      for (i=0; i < arraySize; i++)
      {
	table[i].hash = table[i].number & (hash - 1);
	if (curmax < table[i].hash) curmax = table[i].hash;
	for(j=0; j<i; j++)
	{
	   if (table[i].hash == table[j].hash)
	   {
	     if ( curmax > maxmask ) 
		 fprintf (stderr,
		    "Warning:  Table for class <%s> growing quite large.\n\
		    Consider changing name of component <%s> or <%s>\n",
		    name,array[i].name,array[j].name);
	     hash <<= 1;
	     i = -1;
	     break;
	   }
	}
      }
      for (i=0; i< arraySize; i++) RetStrSz += strlen(table[i].offset);
      qsort (table, arraySize, sizeof (*table) , cmpTbl);
      if (arraySize) finalSize = table[arraySize-1].hash;
      else finalSize = -1;
      if ((finalSize > 0) && (finalSize != curmax))
	{
	    fprintf (stderr,"Something is terribly wrong here...\n");
	    fprintf(stderr,"FinalSize = %d; curmax = %d; arraysz=%d; fl=%d\n",
			finalSize, curmax,arraySize,fl);
	}
      RetStrSz += 99 + 5 * finalSize + 5 * strlen (name);
      if (! RetStr) RetStr = (char *) calloc (1,RetStrSz + 16);
      else RetStr = (char *) realloc (RetStr, RetStrSz + 16);
      if (! RetStr) fprintf(stderr,"Uh oh...\n");
      if (!arraySize) hash=0;
      sprintf (RetStr + strlen(RetStr),
	" static int _%sHs%c = %d;\n static int _%sAr%c[] = {", 
		    name,(fl?'F':'V'), hash-1, name,(fl?'F':'V'));
      for (k=i=0; i<= finalSize; i++)
	{ 
	    if (table[k].hash == i)
	    {
		strcat (strcat (RetStr, table[k].offset),", ");
		k++;
	    }
	    else strcat (RetStr, "0,");
	    if (! (i&7) ) strcat (RetStr,"\n");
	}
	if (! arraySize) strcat (RetStr, "0");
	strcat (RetStr, "};\n");
      if(table) free (table);
    }
  if (Varray) free (Varray);
  if (Farray) free (Farray);
  return (RetStr);
}

/***************************************************************************
 * char * mkcast(castr,fnc) char * castr; int fnc;
 * 	Make an object casting string from a general type cast string
 *
 *
 ***************************************************************************/
char * mkcast(castr,fnc) char * castr; int fnc; /* fnc-flag for function cast */
{
  char * tmppc;
#ifdef DBG
fprintf(stderr,"mkcast 1: castr: %s\n",castr);
#endif DBG
#ifdef ORIG /* This way caused obj:>int[3*4] to produce obj:>int[3*(*)4] */
  if (tmppc = strrchr(castr,'*')) /* Put asterix in middle of cast */
  {  *tmppc = 0;
     tmppc ++;
     *strrchr(tmppc,')') = 0;
     if (!fnc)
	 castr = ztwrite(0,castr,"*(*)",tmppc,0);
     else
	 castr = ztwrite(0,castr,"*(*)()",tmppc,0);
  }
  else if (tmppc = strchr(castr,'[')) /* Put asterix in middle of cast */
  {  if( fnc)
     {
	 yyerror("Illegal cast - function returning array");
     }
     else
     {
	 *tmppc = 0;
	 tmppc ++;
	 *strrchr(tmppc,')') = 0;
	 castr = ztwrite(0,castr,"(*)[",tmppc,0);
     }
#else
  if (tmppc = strchr(castr,'[')) /* Put asterix in middle of cast */
  {  if( fnc)
     {
	 yyerror("Illegal cast - function returning array");
     }
     else
     {
	 *tmppc = 0;
	 tmppc ++;
	 *strrchr(tmppc,')') = 0;
	 castr = ztwrite(0,castr,"(*)[",tmppc,0);
     }
  }
  else if (tmppc = strrchr(castr,'*')) /* Put asterix in middle of cast */
  {  *tmppc = 0;
     tmppc ++;
     *strrchr(tmppc,')') = 0;
     if (!fnc)
	 castr = ztwrite(0,castr,"*(*)",tmppc,0);
     else
	 castr = ztwrite(0,castr,"*(*)()",tmppc,0);
  }
#endif ORIG
  else
  { if (! fnc)
       *strrchr(castr,')') = '*'; /* Else puts an '*' at end of the cast --  */
    else
      {
	  *strrchr(castr,')') = 0;
	  castr = vwrite(castr,"(*)()");
      }
  }
#ifdef DBG
fprintf(stderr,"mkcast 2: castr: %s\n",castr);
#endif DBG
  return castr;
}
