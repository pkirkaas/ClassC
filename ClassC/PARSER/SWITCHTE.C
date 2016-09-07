int exm(a) int a;
{ return a; }

int eval(fnc,tst) int fnc; int tst;
{return fnc;}

main()

{ int x;
  (( int (*)() ) (eval (exm, x=5 ))) (x = 22);
  printf ("%d",x);
}

