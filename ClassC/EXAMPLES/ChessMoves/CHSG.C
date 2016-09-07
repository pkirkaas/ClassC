#include "Object.h"
#include "Board.h"
#include "Piece.h"

void out_of_store() {printf("Out of store\n"); exit(1);}
extern void (* set_new_handler(...)) ();
main()
{
system("clear");
set_new_handler(&out_of_store);
int i;
Piece * WhiteRook = new Rook('w',4,5);
Piece * BlackRook = new Rook('b',5,5);
Piece * WhiteBishop = new Bishop('w',2,3);
Piece * WhiteQueen = new Queen('w',2,5);
for (i=0; i<100; i++) {
WhiteRook->showmoves();
//WhiteRook->board->display();
WhiteQueen->showmoves();
//WhiteBishop->showmoves();
//WhiteRook->board->display();
}}
