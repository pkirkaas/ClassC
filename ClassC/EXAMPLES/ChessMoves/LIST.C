#include "List.h"
#include <stream.h>

void List::reset()
   { while (remove(1)); }

Object * List::next ()
   { if (now = now->next) return now->elemV;
     return (Object *) 0;
   };
int List::remove(int k) // Takes index of object as argument & removes it.
   {int n = 0; 
    now = head;
    while (n < k) {n++; if ( ! next()) return 0;}
    if (now->prev) now->prev->next = now->next;
    if (now->next) now->next->prev = now->prev;
    delete now->elemV;
    delete now;
    now = head;
    size--;
    return k;
   };
Object * List::copy(){rewind();
    List * newL = new List;
    Object * tst;
    while (tst = next()) newL->add(tst->copy());
    return newL;
   };
char * List::eval() // Returns a string which identifies the List.
{ char * outS = new char[999];
  ostream ostS(999,outS); 
  rewind();
  int n=0;
  Object * linkOV;
  ostS << "List:: ";
  char * elemstr; // Allocated by linkOV->eval(); released at this level
  while (linkOV = next()) 
     { elemstr = linkOV->eval();
       ostS << ++n << ")" << elemstr << " "; 
       delete elemstr;
     }
  ostS << "# ";
  return outS;
};
void List::merge(List *lst)
{ff();
 lst->rewind();
 Object* tst;
 while (tst = lst->next()) add(tst);
};

int List::valmember(Object * p) // Is the value of the object in the list?
{ rewind();                     //   If so, return its poistion.
  int n=0;
  while (next()) { n++; if (eqv(now->elemV, p))  return n;}
  return 0;
};
int List::refmember(Object * p)// Is the object itself; that is , its address
{ rewind(); 		       // included in the list?
  int n=0;
  while (next()) { n++; if (now->elemV == p)  return n;}
  return 0;
};
  

