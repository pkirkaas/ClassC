#ifndef LST
#define LST 1
#include "Object.h"
/* class List:  All elements are descendants of class Object.  
   The basic operations supported are:
*/
struct link { Object* elemV; link *next, *prev; 
               link(){elemV=0; next = prev = (link *)0;};};
struct List : Object
  {
    struct link *head, *tail, *now;
    int size;
    Object * next (); //Returns the next Object * on the List.
    int remove(int k); // Takes index of object as argument & removes it.
    int refmember(Object *);// Checks for SAME object & returns index if found.
    int valmember(Object *); // Compares VALUES of objects to see if on list.
    Object * copy();
void reset(); // Empties the list.
virtual char * eval();
virtual void merge(List *);// Adds by reference
virtual void add( Object *elemV)// Add by copying POINTERS onto list -- reference.
        { link *q = new link;
          q->elemV = elemV;
          q->prev = tail;
          tail = tail->next = q;
          size++;
        };
    List ()
        { link *q = new link;
          head =  tail =  now = q;
          size = 0;
        };
    ~List(){now=head;
            link * prev = now;
            while (next()) {delete prev->elemV; delete prev; prev = now;}
            delete prev->elemV;
            delete prev;
           };
    Object *current() { return (now->elemV); };
    void rewind() {now = head;};
    void ff() { now = tail;};
  };

#endif




