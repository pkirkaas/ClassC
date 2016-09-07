#ifndef OBJ 
#define OBJ 1
int strcmp(char* , char*);
struct Object 
{
  virtual int xv(){return 0;};
  virtual int yv(){return 0;};
  virtual char * eval() {return (char *)0;}; // Redifined in derived classes.
  virtual Object * copy() {return (Object *) 0;};
//  friend ostream& operator<<(ostream&,Object *);
//  friend ostream& operator<<(ostream&,Object);
};

#endif
