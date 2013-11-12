#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#define mainITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define mainITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
#define mainDEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define mainTRCENA          0x01000000

void vPrintString( const char *pcString )
{
  vTaskSuspendAll();
  {
    printf( pcString );
  }
  xTaskResumeAll();
}

void vPrintStringAndNumber( const char *pcString, unsigned long ulValue )
{
  vTaskSuspendAll();
  {
    printf( "%s %u\n", pcString, ulValue );
  }
  xTaskResumeAll();
}

int fputc( int iChar, FILE *pxNotUsed ) 
{

  ( void ) pxNotUsed;

  if( mainDEMCR & mainTRCENA ) 
    {
      while( mainITM_Port32( 0 ) == 0 );
      mainITM_Port8( 0 ) = iChar;
    }

  return( iChar );
}

