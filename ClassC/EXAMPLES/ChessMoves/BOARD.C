#include "Board.h"

Board::Board()
  {moves = new cSet; 
   int i,j;
   for (i=0; i<8; i++) for (j=0;j<8;j++) square[i][j] = (Game *) 0;
  }
void Board::display()
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
void Board::setup() 
  { int i,j;
    for(i=0;i<8;i++) for (j=0;j<8;j++)
      { strcpy (sect[i][j],"       ");
        if (square[i][j])
          { //printf("Piece [%d,%d] thinks it's at [%d,%d]\n",i,j,
	//	(square[i][j]->pos).x, (square[i][j]->pos).y);
            sect[i][j][1] = (square[i][j])->gcolor();
            sect[i][j][3] = (square[i][j])->id();
          }
      } 
    moves->rewind();
    while (moves->next()) 
                sect[moves->now->elemV->xv()][moves->now->elemV->yv()][5] = 'X';
  };

