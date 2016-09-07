

typedef struct { int a; float b; } MS;

class SAMP { MS memval; int g; };

struct { object c; } ;

main()
{ SAMP myObject;
  myObject:>memval;
}
