#ifndef SET
#define SET 1
#include "List.h"
// Sets are a modified subclass of Lists, the only difference being
// that the add function will not add duplicates.
//   For pSet, the actual physical ADDRESSES are compared when determining
// if an object is already in the set -- different objects of the same
// value are acceptable.  
//   cSet --
// adresses are not compared, rather the value returned by eval() is tested
// for duplication.
int strcpy(...);
struct pSet : List
{
 void republic(pSet *);
 void add(Object * OV)
       { if ( ! refmember(OV)) List::add(OV); };
 char * eval() {char * evlstr = List::eval();
		strcpy(evlstr,"pSet");
		evlstr[4]=':';
                return evlstr;
	       };
};
struct cSet : List
{
 void republic(cSet *);
 void add(Object * OV)
  { if ( ! valmember(OV)) List::add(OV); };
 char * eval() {char * evlstr = List::eval();
		strcpy(evlstr,"cSet");
		evlstr[4]=':';
                return evlstr;
	       };
};
