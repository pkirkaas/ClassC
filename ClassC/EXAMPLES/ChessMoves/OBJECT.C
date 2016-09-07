
#include "Object.h"

  int eqv(Object *aObj, Object *bObj)
    {char *aS = aObj->eval(), *bS = bObj->eval(); // a & b are string represent
     int B = strcmp(aS,bS);//B is boolean test of equality for strings a& b
     delete aS; delete bS; // Free free store
     return (!B); // B=0 if aS==bS, -1 if aS<bS; 1 if aS>bS; we want inverse.
    };

/*
  ostream& operator<<(ostream& s, Object Obj)
    {char * evS = Obj.eval();
     s << evS;
     delete evS;
     return s;
    };
  ostream& operator<<(ostream& s, Object *OV)
    {char * evS = OV->eval();
     s << evS;
     delete evS;
     return s;
    };

*/
/* Super Class Object:  
	Implemented to allow all objects to be manipulated abstractly; --
  that is, to allow for late binding.  All other classes ought to be derived
classes of "Object".  This simulates smalltalk procedure.  Object also allows
for some common functions of sublclasses, such as evaluation and equlity
testing.  These abilities are implemetned as follows:
  1)  Evaluation -- What is the value of the object??  The method here is
recursive, and allows for both the output of the value, and equality testing.
The object is queried via an "eval()" function, and returns a string.
In order to reclaim freestore, a special "<<" operator is defined to delete the
string returned by Obj.eval() after appending it.
*/
