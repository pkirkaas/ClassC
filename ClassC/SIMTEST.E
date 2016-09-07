class father { int a; int print(); };
class son:father {print();};

typedef struct { int a; float b; } MS;

class SAMP { MS memval; int g; };

struct { object c; } ;

father::print() {printf ("This is print from father\n");}
son::print(q) int q; {printf ("This is print from son -- q=<%d>\n",q);}

main()
{ object inst;
  inst = son:>new();
  SAMP x=SAMP:>new();
  x:>memval.a=22;
  inst:>print(x:>memval.a);
}
