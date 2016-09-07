/********************************************************************
 *	ChessMovs.e -- Written in ClassC by Paul Kirkaas --
 *
 *	ChessMovs shows all legal chess moves for any given
 *	chess board configuration.
 *	The chess board and each piece are objects.  The pieces
 *	examine the board and the other pieces on it, and generate
 *	their own legal moves based on their information about themselves
 *	and the state of the board.
 **********************************************************************/

#include "List.h"
class Board
{  Piece square[8][8];  // array of Pieces
   display();
   setup();
   char  sect[8][8][9];
   object moves; // Set of moves -- gets set/initialized by the piece
   void add();
   delete();
};
class Pos { int x;
	    int y;
	    char * eval();
	    object copy();
	   };
class Piece
{
  object pos; /* position object, of class Pos */
  char colr;
  void showmoves();
  object genmoves(); /* Returns an object of class Set, of moves */
  char* eval();
  delete();
  init();
  char id();
 };
object Piece::genmoves() {}
char Piece::id() {}
class Rook : Piece
{ 
 char id();
 object genmoves(); /* Returns an object of class Set, of moves */
 object Vmoves();
 };
class Bishop : Piece
{char id();
 object genmoves();
 object DiagonalMoves();
 };


class Queen : Rook : Bishop

// An example of multiple inheritence -- the queen has characteristics of
// both the rook and bishop.

{char id() ;
 object genmoves();
 };

extern Board board;
Board::delete()
{ int i,j;
  for ( i=0; i < 8; i++) for ( j=0; j < 8; j++) square[i][j]:>delete();
  moves:>delete();
}


Board::init()
  {
   int i,j;
   for (i=0; i<8; i++) for (j=0;j<8;j++)square[i][j] = 0;
  }
void Board::add(pP, i, j) Piece pP; int i,j;
{square[i][j] = pP;}
Board::display()
{ setup();
  char line[99];
  strcpy(line,
        "_________________________________________________________________\n");
  printf("[H%s",line); //Go Home & draw 1st line.
  int i,j;
  for (j=0;j<8;j++)
     {
     for (i=0;i<8;i++) //printf("|%s",sect[i][7-j]);
     //printf("|\n%s",line);
	;
     }
};
   
Board::setup() 
  { int i,j;
    for(i=0;i<8;i++) for (j=0;j<8;j++)
      { strcpy (sect[i][j],"       ");
        if (square[i][j])
          {
            sect[i][j][1] = square[i][j]:>colr;
            sect[i][j][3] = (square[i][j]):>id();
          }
      } 
    if (moves)
    {
    moves:>rewind();
    while (moves:>place) 
       { sect[moves:>current():>(int)x][moves:>current():>(int)y][5] = 'X';
	moves:>next();
       }
    }
  };

/***********************************************************************/

char * Pos::eval()
{static char  PosEvalRet[2][99];
 static int ix = 0;
 ix = !ix;
 sprintf(PosEvalRet[ix],"Pos:x=%d; y=%d :: ",x,y);
 return PosEvalRet[ix];
}
object Pos::copy()
{ object retobj = Pos:>new();
  retobj:>(int)x = x;
  retobj:>(int)y = y;
  return retobj;
}
Piece::delete() { pos :> delete(); }
Piece::init(cl,x,y)char cl;int x; int y ;
  {
   pos = Pos:>new();
   colr = cl;
   extern object board;
   board:>add(self,x,y);
   pos:>(int)x=x;
   pos:>(int)y=y;
}

char* Piece:: eval()
    {char outS [2][99];
     static int ix = 0;
     ix = !ix;
     sprintf(outS[ix]," Piece:: %c [%d,%d]\n",colr,
		pos:>(int)x,pos:>(int)y);
     return outS[ix];
    }

void Piece::showmoves(){//HERE IS WHERE WE NEED GARBAGE COLLECTION
                    board :> moves  =genmoves();
		    board :> display();
                  };

char Rook::id() {return 'R';};

object Rook::genmoves()
{ return Vmoves();}

object Rook::Vmoves()
{ Pos tstpos;
  object pieceP=0;
  object moves = Set:>new();
  tstpos = pos:>copy();

//Rook movement in Positive x direction.

  for ( (tstpos:>x) = pos:>(int)x+1;
	(tstpos:>x) < 8; (tstpos:>x)++)
     { pieceP =(board:>square[tstpos:>x][tstpos:>y]); 
      if (pieceP)
        { if (pieceP:>(char)colr != colr) moves:>add(tstpos:>copy());
          break;
        }
      moves:>add(tstpos:>copy());
     }

tstpos = pos:>copy();
//Rook movement in Negative x direction.

  for ( (tstpos:>x) = pos:>(int)x-1;
	(tstpos:>x) >= 0; (tstpos:>x)--)
     { pieceP =(board:>square[tstpos:>x][tstpos:>y]); 
      if (pieceP)
        { if (pieceP:>(char)colr != colr) moves:>add(tstpos:>copy());
          break;
        }
      moves:>add(tstpos:>copy());
     }


//Rook movement in Positive Y direction.

tstpos = pos:>copy();
  for ( (tstpos:>y) = pos:>(int)y+1;
	(tstpos:>y) < 8; (tstpos:>y)++)
     { pieceP =(board:>square[tstpos:>x][tstpos:>y]); 
      if (pieceP)
        { if (pieceP:>(char)colr != colr) moves:>add(tstpos:>copy());
          break;
        }
      moves:>add(tstpos:>copy());
     }

//Rook movement in Negative Y direction.
tstpos = pos:>copy();

  for ( (tstpos:>y) = pos:>(int)y-1;
	(tstpos:>y) >= 0; (tstpos:>y)--)
     { pieceP =(board:>square[tstpos:>x][tstpos:>y]); 
      if (pieceP)
        { if (pieceP:>(char)colr != colr) moves:>add(tstpos:>copy());
          break;
        }
      moves:>add(tstpos:>copy());
     }

return moves;
}

/***********************************************************************/

object Bishop::genmoves()
{ return DiagonalMoves(); }

char Bishop::id() {return 'B';};

/***********************************************************************/

object Bishop::DiagonalMoves()
{ object tstpos;
  object pieceP;
  object moves = Set:>new();
  tstpos = pos:>copy();
/* Generate diagonal move for Bishop in Positive X & Positive Y */
  for ( (tstpos:>(int)x) = pos:>(int)x+1,(tstpos:>(int)y) = pos:>(int)y+1; 
        ((tstpos:>(int)x) < 8) && ((tstpos:>(int)y) < 8); 
        (tstpos:>(int)x)++, (tstpos:>(int)y)++)
     { pieceP =  board:>square[tstpos:>x][tstpos:>y]; 
      if (pieceP)
        { if (pieceP:>(char)colr != colr)  moves:>add(tstpos:>copy());
          break;
        }
      moves:>add(tstpos:>copy());
     }


tstpos = pos:>copy();
/* Generate diagonal move for Bishop in Negative X & Negative Y */

  for ( (tstpos:>(int)x) = pos:>(int)x-1,(tstpos:>(int)y) = pos:>(int)y-1; 
        ((tstpos:>(int)x) >= 0) && ((tstpos:>(int)y) >= 0); 
        (tstpos:>(int)x)--, (tstpos:>(int)y)--)
     { pieceP =  (board:>square[tstpos:>(int)x][tstpos:>(int)y]); 
      if (pieceP)
        { if (pieceP:>(char)colr != colr)  moves:>add(tstpos:>copy());
          break;
        }
      moves:>add(tstpos:>copy());
     }


/* Generate diagonal move for Bishop in Negative X & Positive Y */
tstpos = pos:>copy();
  for ( (tstpos:>(int)x) = pos:>(int)x-1,(tstpos:>(int)y) = pos:>(int)y+1; 
        ((tstpos:>(int)x) >= 0) && ((tstpos:>(int)y) < 8); 
        (tstpos:>(int)x)--, (tstpos:>(int)y)++)
     { pieceP =  (board:>square[tstpos:>(int)x][tstpos:>(int)y]); 
      if (pieceP)
        { if (pieceP:>(char)colr != colr)  moves:>add(tstpos:>copy());
          break;
        }
      moves:>add(tstpos:>copy());
     }


/* Generate diagonal move for Bishop in Positive X & Negative Y */
tstpos = pos:>copy();
  for ( (tstpos:>(int)x) = pos:>(int)x+1,(tstpos:>(int)y) =
		pos:>(int)y-1; 
        ((tstpos:>(int)x) < 8) && ((tstpos:>(int)y) >= 0); 
        (tstpos:>(int)x)++, (tstpos:>(int)y)--)
     { pieceP =  (board:>square[tstpos:>(int)x][tstpos:>(int)y]); 
      if (pieceP)
        { if (pieceP:>(char)colr != colr)  moves:>add(tstpos:>copy());
          break;
        }
      moves:>add(tstpos:>copy());
     }
return moves;
};



/***********************************************************************/
char Queen::id() {return 'Q';}

object Queen::genmoves()
{ object Rmoves;
  object Bmoves;
  Rmoves = Vmoves();
  Bmoves = DiagonalMoves();
  Rmoves:>merge(Bmoves);
  return Rmoves;
}
/***********************************************************************/
Board board;

main()
{
    free(malloc (1000000));
    system("clear");
    board = Board:>new();
    // Place a piece on the board by declaring it and positioning it as follows:
    object WhiteRook = Rook:>new ('w',4,5);
    object BlackRook = Rook:>new('b',5,5);
    object WhiteBishop = Bishop:>new ('w',2,3);
    object WhiteQueen = Queen:>new ('w',2,5);
    /*
    while(1)
    */
    int i;
    for (i=0; i<100; i++)
    {
	//Each piece will generate its legal moves if so requested:
	WhiteRook:>showmoves();
	WhiteQueen:>showmoves();
	//WhiteBishop:>showmoves();
    }
}
