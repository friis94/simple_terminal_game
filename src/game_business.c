#include "hdlc_process.h"
#include "game_business.h"
#include <stdio.h>
#include <stdbool.h>

#define GRID_SIZE 5 // 5-by-5 bottom-left is (0,4)

typedef enum
{
   MOVE_UP = 1,
   MOVE_DOWN,
   MOVE_RIGHT,
   MOVE_LEFT
} move_e;

struct point boardPosition = { .x = 0, .y = 4 };
struct point lastBoardPosition = { .x = 0, .y = 4 };

bool game_discard_moves( uint8_t move );
void game_move_position( uint8_t move );

void game_process()
{
   uint8_t move = hdlc_process();
   if ( move != 0 )
   {
      if ( !game_discard_moves( move ) )
      {
         game_move_position( move );
      }
   }
}

struct point game_get_point()
{
   return boardPosition;
}

bool game_discard_moves( uint8_t move )
{
   bool retVal = false;
   static uint8_t lastMoves[ 2 ];
   if ( move == lastMoves[0] && lastMoves[0] == lastMoves[1] )
   {
      retVal = true;
      boardPosition = lastBoardPosition;

      // reset last moves
      lastMoves[0] = 0;
      lastMoves[1] = 0;
   }
   else if ( move == lastMoves[0] )
   {
      lastMoves[1] = move;
   }
   else
   {
      lastMoves[0] = move;
      lastBoardPosition = boardPosition;
   }

   return retVal;
}


void game_move_position( uint8_t move )
{
   switch (move)
   {
   case MOVE_UP:
      if ( boardPosition.y > 0 )
         boardPosition.y--;
      break;
   
   case MOVE_DOWN:
      if ( boardPosition.y < (GRID_SIZE -  1) )
         boardPosition.y++;
      break;

   case MOVE_LEFT:
      if ( boardPosition.x > 0 )
         boardPosition.x--;
      break;

   case MOVE_RIGHT:
      if ( boardPosition.x < (GRID_SIZE - 1) )
         boardPosition.x++;
      break;
   
   default:
      // Invalid move - do nothing
      printf("Invalid move - do nothing\n");
      break;
   }
}
