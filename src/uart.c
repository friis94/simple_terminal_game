#include <stdio.h>
#include "uart.h"

FILE *fp;

void uart_init()
{
   fp = fopen("../transmission.bin", "rb");

   if ( NULL == fp )
   {
      printf( "Could not open file\n" );
   }
   else
   {
      rewind(fp);
   }
}

uint8_t uart_get_char()
{
   uint8_t byte;
   
   if ( NULL != fp )
   {
      byte = fgetc(fp);
   }

   return byte;
}


uint8_t uart_put_chars( const char* byte, uint8_t length )
{
   // This is a fake, it should put the char on the UART
   // but this task is does nothing 
   // (only the return number of send bytes)
   return length;
}

bool uart_end_of_file()
{
   bool retVal = false;
   if ( NULL != fp && feof(fp) )
   {
      retVal = true;
      fclose(fp);
   }
   return retVal;
}