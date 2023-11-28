#include "hdlc_process.h"
#include "uart.h"
#include "yahdlc.h"

#include <string.h>
#include <stdio.h>

#define MAX_HDLC_INFORMATION_SIZE   1
#define MAX_HDLC_FRAME_SIZE         7
#define REQUEST_MOVE                1

typedef enum
{
   HDLC_FRAME_PROCESSING,
   HDLC_FRAME_VALID_RECEIVED,
   HDLC_FRAME_INVALID_RECEIVED
} hdlc_frame_status_e;

typedef enum
{
   HDLC_STATE_REQUEST_COMMAND,
   HDLC_STATE_WAITING_FOR_ACK,
   HDLC_STATE_WAITING_FOR_DATA,
   HDLC_STATE_SENDING_ACK,
} hdlc_states_e;

char src_buffer[MAX_HDLC_FRAME_SIZE];
char dst_buffer[MAX_HDLC_INFORMATION_SIZE];

yahdlc_control_t controlReceiver;
yahdlc_control_t controlSender;

static hdlc_states_e state = HDLC_STATE_REQUEST_COMMAND;

hdlc_frame_status_e hdlc_wait_for_ack();
hdlc_frame_status_e hdlc_wait_for_information( char* data, unsigned int* size );
bool hdlc_receive_frame( char* data, unsigned int* size );
int hdlc_send_req();
int hdlc_send_ack();

int hdlc_process()
{
   int retVal = 0;
   hdlc_frame_status_e frameStatus = 0;
   char data[MAX_HDLC_FRAME_SIZE];
   unsigned int size;

   switch (state)
   {
   case HDLC_STATE_REQUEST_COMMAND:
      if ( hdlc_send_req() ) 
      {
         state = HDLC_STATE_WAITING_FOR_ACK;
      }
      break;

   case HDLC_STATE_WAITING_FOR_ACK:
      frameStatus = hdlc_wait_for_ack();
      if ( frameStatus == HDLC_FRAME_VALID_RECEIVED ) 
      {
         state = HDLC_STATE_WAITING_FOR_DATA;
      } 
      else if ( frameStatus == HDLC_FRAME_INVALID_RECEIVED )
      {
         state = HDLC_STATE_REQUEST_COMMAND;
      }
      break;

   case HDLC_STATE_WAITING_FOR_DATA:
      frameStatus = hdlc_wait_for_information( data, &size );
      if ( frameStatus == HDLC_FRAME_VALID_RECEIVED ) 
      {
         if ( size > 0 )
         {
            // we always receive one byte only
            retVal = data[0];
         }
         state = HDLC_STATE_SENDING_ACK;
      }
      else if ( frameStatus == HDLC_FRAME_INVALID_RECEIVED )
      {
         state = HDLC_STATE_REQUEST_COMMAND;
      }
      break;

   case HDLC_STATE_SENDING_ACK:
      if ( hdlc_send_ack() ) 
      {
         state = HDLC_STATE_REQUEST_COMMAND;
      }
      break;
   
   default:
      // DO NOTHING - never happens.
      break;
   }

   return retVal;
}


hdlc_frame_status_e hdlc_wait_for_ack()
{
   hdlc_frame_status_e retVal = HDLC_FRAME_PROCESSING;
   char data;
   unsigned int size;
   if ( hdlc_receive_frame( &data, &size ) )
   {
      if ( ( controlReceiver.frame == YAHDLC_FRAME_ACK ) && 
           ( controlReceiver.seq_no == (++controlSender.seq_no) ) )
      {
         retVal = HDLC_FRAME_VALID_RECEIVED;
      }
      else
      {
         retVal = HDLC_FRAME_INVALID_RECEIVED;
      }
   }
   return retVal;
}


hdlc_frame_status_e hdlc_wait_for_information( char* data, unsigned int* size )
{
   hdlc_frame_status_e retVal = HDLC_FRAME_PROCESSING;
   if ( hdlc_receive_frame( data, size ) )
   {
      if ( ( controlReceiver.frame == YAHDLC_FRAME_DATA ) && 
           ( controlReceiver.seq_no == (++controlSender.seq_no) ) )
      {
         retVal = HDLC_FRAME_VALID_RECEIVED;
      }
      else
      {
         retVal = HDLC_FRAME_INVALID_RECEIVED;
      }
   }
   return retVal;
}

bool hdlc_receive_frame( char* data, unsigned int* size )
{
   bool retVal = false;
   int readBytes = 0; 
   static uint8_t bytesReceived = 0;

   src_buffer[ bytesReceived++ ] = uart_get_char();

   readBytes = yahdlc_get_data( &controlReceiver, src_buffer, bytesReceived, dst_buffer, size );

   if ( readBytes >= 0 )
   {
      retVal = true;
      bytesReceived = 0;

      if ( *size > 0 && *size <= MAX_HDLC_FRAME_SIZE)
      {
         memcpy( data, dst_buffer, *size );
      }
   }
   else
   {
      // Should handle errors but for now I assume only valid frames are received.
   }

   return retVal;
}


int hdlc_send_ack()
{
   yahdlc_control_t controlSender = {  .frame = YAHDLC_FRAME_ACK,
                                       .seq_no = controlReceiver.seq_no + 1 };

   char frame[MAX_HDLC_FRAME_SIZE];
   unsigned int length = 0;
   uint8_t sendBytes = 0;
   if ( 0 == yahdlc_frame_data( &controlSender, NULL, 0, frame, &length ) )
   {
      sendBytes = uart_put_chars( frame, length );
   }
   
   return sendBytes;
}


int hdlc_send_req()
{
   controlSender.frame = YAHDLC_FRAME_DATA;
   controlSender.seq_no = 0;

   char request = REQUEST_MOVE;
   char frame[MAX_HDLC_FRAME_SIZE];
   unsigned int length = 0;
   uint8_t sendBytes = 0;
   if ( 0 == yahdlc_frame_data( &controlSender, &request, sizeof(request), frame, &length ) )
   {
      sendBytes = uart_put_chars( frame, length );
   }

   return sendBytes;
}