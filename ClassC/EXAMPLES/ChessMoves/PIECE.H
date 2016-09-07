#ifndef PCE
#define PCE 1
#include "Board.h"


struct Pos : Object { int x; int y; 
                      int xv() {return x;};
                      int yv() {return y;};
                      Object * copy() {Pos * dup = new Pos; 
                                       *dup = *this;
                                       return (Object *) dup;};
                      char * eval(); 
                    };

struct Piece : Game 
{
  static Board * board;
  Pos pos;
  char colr;
  virtual cSet * genmoves(){return (cSet *) 0;};
  void showmoves();
  Piece(char cl=0,int x=0,int y=1) {pos.x=x; pos.y=y; colr = cl;
                                    if (! board) board = new Board;
                                    board->add((Game *)this,x,y);};
  void scolor(char c) { colr = c;};
  char gcolor() { return colr;};
  void spos (Pos ps) {pos = ps;};
  Pos gpos() {return pos;};
  char* eval()
    {char * outS = new char[99];
     sprintf(outS," Piece:: %c [%d,%d]\n",colr,pos.x,pos.y);
     return outS;
    }
 };


struct Rook : Piece
{char id() {return 'R';};
 Rook(char clr, int x, int y) : (clr,x,y){};// Calls the Base Class constructor
 cSet * genmoves();
 };

struct Bishop : Piece
{char id() {return 'B';};
 Bishop(char clr, int x, int y) : (clr,x,y){};//Calls Base Class constructor
 cSet * genmoves();
 };


struct King : Piece
{char id() {return 'K';};
 King(char clr, int x, int y) : (clr,x,y){};//Calls Base Class constructor
 cSet * genmoves();
 };


struct Queen : Piece
{char id() {return 'Q';};
 Queen(char clr, int x, int y) : (clr,x,y){};//Calls Base Class constructor
 cSet * genmoves();
 };

/*
struct Knight : Piece
{char id() {return 'N';};
 Knight(char clr, int x, int y) : (clr,x,y){};//Calls Base Class constructor
 cSet * genmoves();
 };


struct Pawn : Piece
{char id() {return 'P';};
 Pawn(char clr, int x, int y) : (clr,x,y){};//Calls Base Class constructor
 cSet * genmoves();
 };
*/
