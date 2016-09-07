#ifndef BRD
#define BRD 1
#include "Object.h"
#include "Set.h"

struct Game : Object
{virtual char gcolor(){return 'R';};
 virtual void showmoves(){};
 virtual char id(){return 'V';};
 //virtual Pos gpos() {};
};

struct Board : Object
{ char  sect[8][8][9];
  Game * square[8][8];
  cSet * moves;
  void add(Game * pP, int i, int j) {square[i][j] = pP;};
  void display();
  void setup();
  Board();
};
#endif
