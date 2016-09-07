#include "Piece.h"
/*
void Piece::showmoves()
   {delete (board->moves);
    board->moves = genmoves();
   }
*/

 char * Pos::eval() { char  * str;
//                      str = (char *) malloc (99);
                      str = new char[99];

                      sprintf(str," [%d,%d] ",x,y);
                      return str;
                    }; 

void Piece::showmoves(){delete (board -> moves);
                   (board -> moves) = genmoves();
		    board->display();
                  };

cSet * Rook::genmoves()
{ Pos tstpos;
  Piece * pieceP;
  cSet * moves = new cSet;
  tstpos = pos;

//Rook movement in Positive x direction.

  for ( (tstpos.x) = pos.x+1; (tstpos.x) < 8; (tstpos.x)++)
     { pieceP = (Piece *) (board->square[tstpos.x][tstpos.y]); 
      if (pieceP)
        { if (pieceP->colr != colr)  moves->add(tstpos.copy());
          break;
        }
      moves->add(tstpos.copy());
     }
tstpos = pos;

//Rook movement in Negative x direction.

  for ( (tstpos.x) = pos.x-1; (tstpos.x) >= 0; (tstpos.x)--)
      {pieceP = (Piece *) (board->square[tstpos.x][tstpos.y]); 
      if (pieceP)
        { if (pieceP->colr != colr)  moves->add(tstpos.copy());
          break;
        }
      moves->add(tstpos.copy());
     }

tstpos = pos;

//Rook movement in Positive Y direction.

  for ( (tstpos.y) = pos.y+1; (tstpos.y) < 8; (tstpos.y)++)
      {pieceP = (Piece *) (board->square[tstpos.x][tstpos.y]); 
      if (pieceP)
        { if (pieceP->colr != colr)  moves->add(tstpos.copy());
          break;
        }
      moves->add(tstpos.copy());
     }
tstpos = pos;

//Rook movement in Negative Y direction.

  for ( (tstpos.y) = pos.y-1; (tstpos.y) >= 0; (tstpos.y)--)
      {pieceP = (Piece *) (board->square[tstpos.x][tstpos.y]); 
      if (pieceP)
        { if (pieceP->colr != colr)  moves->add(tstpos.copy());
          break;
        }
      moves->add(tstpos.copy());
     }
return moves;
}

cSet * Bishop::genmoves()
{ Pos tstpos;
  Piece * pieceP;
  cSet * moves = new cSet;
  tstpos = pos;
/* Generate diagonal move for Bishop in Positive X & Positive Y */
  for ( (tstpos.x) = pos.x+1,(tstpos.y) = pos.y+1; 
        ((tstpos.x) < 8) && ((tstpos.y) < 8); 
        (tstpos.x)++, (tstpos.y)++)
     { pieceP = (Piece *) (board->square[tstpos.x][tstpos.y]); 
      if (pieceP)
        { if (pieceP->colr != colr)  moves->add(tstpos.copy());
          break;
        }
      moves->add(tstpos.copy());
     }


tstpos = pos;
/* Generate diagonal move for Bishop in Negative X & Negative Y */

  for ( (tstpos.x) = pos.x-1,(tstpos.y) = pos.y-1; 
        ((tstpos.x) >= 0) && ((tstpos.y) >= 0); 
        (tstpos.x)--, (tstpos.y)--)
     { pieceP = (Piece *) (board->square[tstpos.x][tstpos.y]); 
      if (pieceP)
        { if (pieceP->colr != colr)  moves->add(tstpos.copy());
          break;
        }
      moves->add(tstpos.copy());
     }


/* Generate diagonal move for Bishop in Negative X & Positive Y */
tstpos = pos;
  for ( (tstpos.x) = pos.x-1,(tstpos.y) = pos.y+1; 
        ((tstpos.x) >= 0) && ((tstpos.y) < 8); 
        (tstpos.x)--, (tstpos.y)++)
     { pieceP = (Piece *) (board->square[tstpos.x][tstpos.y]); 
      if (pieceP)
        { if (pieceP->colr != colr)  moves->add(tstpos.copy());
          break;
        }
      moves->add(tstpos.copy());
     }


/* Generate diagonal move for Bishop in Positive X & Negative Y */
tstpos = pos;
  for ( (tstpos.x) = pos.x+1,(tstpos.y) = pos.y-1; 
        ((tstpos.x) < 8) && ((tstpos.y) >= 0); 
        (tstpos.x)++, (tstpos.y)--)
     { pieceP = (Piece *) (board->square[tstpos.x][tstpos.y]); 
      if (pieceP)
        { if (pieceP->colr != colr)  moves->add(tstpos.copy());
          break;
        }
      moves->add(tstpos.copy());
     }
return moves;
};



cSet * King::genmoves()
{ Pos tstpos;
  Piece * pieceP;
  cSet * moves = new cSet;
  tstpos = pos;

/* Generate moves for King in Positive X & Positive Y */
  tstpos.x++;  // Relative move from intitial posititon: [+1,0]
  pieceP = (Piece *) (board -> square[tstpos.x][tstpos.y]);
  if ((tstpos.x < 8 && tstpos.x >=0 && tstpos.y < 8 && tstpos.y >=0)
      && (!pieceP || (pieceP->colr != colr))) moves->add(tstpos.copy());

  tstpos.y++;  // Relative move from intitial posititon: [+1,+1]
  pieceP = (Piece *) (board -> square[tstpos.x][tstpos.y]);
  if ((tstpos.x < 8 && tstpos.x >=0 && tstpos.y < 8 && tstpos.y >=0)
      && (!pieceP || (pieceP->colr != colr))) moves->add(tstpos.copy());

  tstpos.x--;  // Relative move from intitial posititon: [0,+1]
  pieceP = (Piece *) (board -> square[tstpos.x][tstpos.y]);
  if ((tstpos.x < 8 && tstpos.x >=0 && tstpos.y < 8 && tstpos.y >=0)
      && (!pieceP || (pieceP->colr != colr))) moves->add(tstpos.copy());

  tstpos.x--;  // Relative move from intitial posititon: [-1,+1]
  pieceP = (Piece *) (board -> square[tstpos.x][tstpos.y]);
  if ((tstpos.x < 8 && tstpos.x >=0 && tstpos.y < 8 && tstpos.y >=0)
      && (!pieceP || (pieceP->colr != colr))) moves->add(tstpos.copy());

  tstpos.y--;  // Relative move from intitial posititon: [-1,0]
  pieceP = (Piece *) (board -> square[tstpos.x][tstpos.y]);
  if ((tstpos.x < 8 && tstpos.x >=0 && tstpos.y < 8 && tstpos.y >=0)
      && (!pieceP || (pieceP->colr != colr))) moves->add(tstpos.copy());

  tstpos.y--;  // Relative move from intitial posititon: [-1,-1]
  pieceP = (Piece *) (board -> square[tstpos.x][tstpos.y]);
  if ((tstpos.x < 8 && tstpos.x >=0 && tstpos.y < 8 && tstpos.y >=0)
      && (!pieceP || (pieceP->colr != colr))) moves->add(tstpos.copy());

  tstpos.x++;  // Relative move from intitial posititon: [0,-1]
  pieceP = (Piece *) (board -> square[tstpos.x][tstpos.y]);
  if ((tstpos.x < 8 && tstpos.x >=0 && tstpos.y < 8 && tstpos.y >=0)
      && (!pieceP || (pieceP->colr != colr))) moves->add(tstpos.copy());

  tstpos.x++;  // Relative move from intitial posititon: [+1,-1]
  pieceP = (Piece *) (board -> square[tstpos.x][tstpos.y]);
  if ((tstpos.x < 8 && tstpos.x >=0 && tstpos.y < 8 && tstpos.y >=0)
      && (!pieceP || (pieceP->colr != colr))) moves->add(tstpos.copy());

return moves;
}


cSet * Queen::genmoves()
{ Rook myRook(colr,pos.x,pos.y);
  Bishop myBishop (colr,pos.x,pos.y);
  cSet * moves = myRook.genmoves();
  cSet * bish_like_moves = myBishop.genmoves();
  moves->merge((List *) bish_like_moves);
  delete bish_like_moves;
  board->add((Game *)this,pos.x,pos.y);
  return moves;
}

