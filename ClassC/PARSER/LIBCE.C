
/*             %M%    %I%  %H%  %G%                */
/* file libce.c -- Library functions for ClassC by Paul Kirkaas */
#include <stdio.h>
#define hex unsigned long
#define INITSIZE 999

/*****************************************************************************
 * FAST StrEq -- Compares string 1 & string 2.  rslt==1 if same; else 0.
 ----------------------------------------------------------------------------*/
#define StrEqf(str1,str2,QQ_a,QQ_b,rslt) \
   QQ_a = (str1);\
   QQ_b = (str2);\
   rslt = 1;\
   while ( *QQ_a || *QQ_b )  if(*QQ_a ++ != *QQ_b ++) \
	{ rslt = 0; \
	  break; \
	}

char * _whome[99];
int _mct = 0;
int _indArr[99]; /* This is to allow deep indirection of objects
		    ( obj1:>objf1():>objf2(); etc */

struct ref_el	{	char * name;
			int offset;
		};

struct _TblEntry 
 {	int	ref_cnt;
	int 	self; /*Really an undefined struct pointer */
	int	dirtyQ;	/* Flag -- is this entry free? */
	int *	hofV;   /* Table of offsets */
	int *	hofF;   /* Table of offsets */
	int	hashV; /* The hash value for the entry */
	int	hashF; /* The hash value for the entry */
 };

struct FreeEntry  /*  A linked list of freed entries */
     {	struct FreeEntry * next;	
	hex index;	
     };

static struct FreeEntry  freeEntryRoot = {0};
static struct FreeEntry  * freeEntryList = &freeEntryRoot;
static int tblSz = 0;
static int NoOfTableEntries = 0;
struct _TblEntry * _objTbl = 0;
struct _TblEntry * _objV[99];

/*****************************************************************************
 * _new (size,tableV,hashV,tableF,hashF)
 *	creates a new object instance of a class.  Non - garbage collected
 *	version.
 ----------------------------------------------------------------------------*/
_new (size,tableV,hashV,tableF,hashF)
int * tableF,* tableV,hashF,hashV,size;
{
 NoOfTableEntries++;
 if (NoOfTableEntries >= tblSz - 2) grow_table();
 _objTbl[NoOfTableEntries].self = (hex)calloc(1,size);
 _objTbl[NoOfTableEntries].ref_cnt = 0;
 _objTbl[NoOfTableEntries].hofV = tableV;
 _objTbl[NoOfTableEntries].hofF = tableF;
 _objTbl[NoOfTableEntries].dirtyQ = 1;
 _objTbl[NoOfTableEntries].hashV = hashV;
 _objTbl[NoOfTableEntries].hashF = hashF;
 return NoOfTableEntries;
}
   
/*************************************************************************
 * Garbage collection version of "new()" 
 *-----------------------------------------------------------------------*/
int CEGthreshold = 1000;
int CEGfrequency = 100;
/* These global integers may be modified by user programs to tune
 * the garbage collector.  Garbage collection is only done when the
 * number of entries in the object table exceeds CEGthreshold.
 * After this threshold is achieved, garbage collection is performed
 * only once every CEGfrequency number of calls to "new".
 */
__new (size,tableV,hashV,tableF,hashF)
int * tableF,* tableV,hashF,hashV,size;
{int i;
 static int cycle = 0;
 if (!(i=pop_free()))
     {	if ((NoOfTableEntries >= CEGthreshold )
	&& !( cycle = ( cycle + 1) % CEGfrequency ))
	    { 
	      collect();
	      if (!(i = pop_free())) 
		{  
		   collect();
		   if (!(i = pop_free())) i = ++NoOfTableEntries;
		}
	    }
       else i = ++NoOfTableEntries;
     }
 if (i > (tblSz - 4)) grow_table();
 _objTbl[i].self = (int)calloc(1,size);
 _objTbl[i].ref_cnt = 0;
 _objTbl[i].hashV = hashV;
 _objTbl[i].hashF = hashF;
 _objTbl[i].hofV = tableV;
 _objTbl[i].hofF = tableF;
 _objTbl[i].dirtyQ = 1;
 return i;
}


/*****************************************************************************
 * push & pop free -- Dynamically allocated stack to keep track of 
 *	freed object table indicies
 ----------------------------------------------------------------------------*/
static int * freeEntryStack = 0;  /* Pointer to base of free entry stack --
				   * the stack to keep track of released
				   * object indicies
				   */
static int   fESP = 0;	/*   Free entry stack POINTER (depth of stack)
			 *   points to the CURRENTLY available entry; or -1
			 */
static int   fESize = 0;	/* FES size -- size allocated to the stack */

push_free(freed) int freed; /* Pushes freed object index back on free stack*/
{  
  if (!freeEntryStack) /* If this is the first time called */
      { fESize = 200;
	freeEntryStack = (int *) calloc (1,fESize * sizeof (int) + 4);
      }
  if ((fESize - fESP) < 4) 
      {	fESize *= 1.5;
	freeEntryStack= (int *)realloc(freeEntryStack,fESize*sizeof(int)+4);
      }
  freeEntryStack[++ fESP ] = freed;
}

pop_free() /* Returns index of the next free object table entry, or 0 */
{  
   if ( fESP >= 0) return (freeEntryStack[ fESP --]);
   return 0;
}
   
/*****************************************************************************
 * collect()
 *	The ClassC garbage collection routine.
 ----------------------------------------------------------------------------*/

collect()
{ register int i = 0;
  while (++i <= NoOfTableEntries) if (_objTbl[i].ref_cnt == 0) _delete(i);
}

/*****************************************************************************
 * incref & decref  --  For garbage collection.  
 *	decref takes & returns ADDRESS of object
 *	( so that it can be a modifiable lvalue when dereferenced )  and 
 *	incs/decs the count in the object table (if non 0)
 ----------------------------------------------------------------------------*/
int  _incref ( obj ) int  obj;
{ if (obj) _objTbl[obj].ref_cnt ++;
  return obj;
}

int * _decref ( objP ) int * objP;
{ if (*objP && _objTbl[*objP].ref_cnt) _objTbl[*objP].ref_cnt --;
  return objP;
}
   
/*****************************************************************************
 * _delete( obj) int obj;
 *	The general deletion routine.  _delete() first checks to make sure 
 *	the reference count is Zero and that it hasn't processed this object
 *	already ( in case the class's user defined delete function tries to
 *	delete this object recursively, which is possible because objects
 *	can be connected in cyclic graphs).  It then calls the user's delete
 *	function, frees the memory allocated to the object by (_)_new(), and
 *	resets the object table.
 ----------------------------------------------------------------------------*/
_delete( obj) int obj;
{int (*deleteP) ();
 if (! obj) return;
 if (_objTbl[obj].ref_cnt) _objTbl[obj].ref_cnt--;
 if (( _objTbl[obj].ref_cnt == 0) && _objTbl[obj].dirtyQ)
     {
     /* Catch cycles if class's delete function calls delete recursively -- */
      _objTbl[obj].dirtyQ = 0;
      if (0) {
	deleteP = (int (*)()) ((_objV[0]=_objTbl+obj)-> self + _objV[0] ->
		hofF[/*"delete"*/ 0x1952d415 & _objV[0]->hashF]);
	deleteP(obj);
      }
/* XXX PXK pxk *** MUST BE DEALT WITH !!!
	Was --
	if (deleteP = (int (*)()) _deref(obj,"delete")) deleteP(obj);
	The problem now is to find a way to tell if there is a "delete"
	function defined for the class or not.
*/
      free (_objTbl[obj].self);
      _objTbl[obj].self = 0;
      push_free(obj);
     }
}

/*****************************************************************************
 * grow_table()
 *	Increases size of object table (NoOfTableEntries, tblSz).
 ----------------------------------------------------------------------------*/
static int grow_table()
{
 if (!tblSz) /* If no table has yet been allocated */
    {tblSz = INITSIZE;
     _objTbl=(struct _TblEntry *)calloc(1,tblSz*sizeof(struct _TblEntry )+16);
    }
 else
    { int i;
      int old_size = tblSz;
      tblSz *= 1.5;
      _objTbl = (struct _TblEntry *) 
	    realloc ((char *)_objTbl,tblSz*sizeof(struct _TblEntry)+16);
      for (i = (old_size * sizeof(struct _TblEntry));
			i <= (tblSz * sizeof(struct _TblEntry)); i +=4)
	    {* (long *) (((int)_objTbl) + i) = (long) 0; }
	    /* Makes sure the new table area is zeroed. */
     }
}


/*****************************************************************************
 * int  _deref(selfP, mem) int selfP; register char * mem;
 *	The ClassC dereferencing function.  Takes object table index (selfP)
 *	and the name of the desired element, and returns a pointer to it.
 *	This pointer is found in the "deref array" built by the parser for
 *	the class type.
 ----------------------------------------------------------------------------*/
int  _deref(selfP, mem) int selfP; register char * mem;
{ 
  static int depth = 0;
  register char * tmps1;
  register char * tmps2;
  register int Q;
  register struct ref_el * class;
  if ((selfP > NoOfTableEntries) || !selfP)
  {
     fflush(stdout); fflush(stderr);
     fprintf(stderr,
      "Selfp in deref= %d; no table ent= %d; member name dereferenced: <%s>\
	\n(MAYBE PARSER BUILT BACKWARDS?)\n",
	selfP,NoOfTableEntries,mem);
     fprintf(stderr, "In function:\n",_whome);
     while(  _mct-- &&  _whome[_mct])
	 fprintf(stderr, "\t\t\t<%s>\n",_whome[_mct]);
     exit(1);
     return 0; /* never reached */
  }
  class = (struct ref_el *) _objTbl[selfP].hofV;

  while ( (class->name) )
    {
      StrEqf(mem,class->name,tmps1,tmps2,Q);
      if (Q) return ( _objTbl[selfP].self + class->offset);
      class++;
    }
    class++;
  while ( class->name )
    { 
      StrEqf(mem,class->name,tmps1,tmps2,Q);
      if (Q) return ( class->offset);
      class++;
    }
  if (!strcmp(mem,"delete")) return 0; /* If there is no delete function, 
					* this is not an error-see "_delete"*/
  if (!depth)
  {	depth = 1;
	printf ("Error -- Element <%s> not defined for class %s\n",
			    mem,(char *)_deref (selfP,"_name"));
  }
  depth = 0;
  return  0;
}

