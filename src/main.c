#include <stdio.h>
#include "uart.h"
#include "hdlc_process.h"
#include "game_business.h"

int main(void)
{
   printf("Starting simple terminal game\n");

   uart_init();

   while ( !uart_end_of_file() )
   {
      game_process();
   }

   struct point boardPosition = game_get_point();
   printf("End position is ( %d, %d )\n", boardPosition.x, boardPosition.y );

   return 0;
}