#include "2450addr.h"
#include <stdarg.h>
#include "libc.h"
#include "option.h"

#define DMA	1


// Function Declaration
void Uart_Init(int baud);
void Uart2_Init(int baud);

void Uart_Printf(char *fmt,...);
void Uart2_Printf(char *fmt,...);

void Uart_Send_String(char *pt);
void Uart2_Send_String(char *pt);

void Uart_Send_Byte(int data);
void Uart2_Send_Byte(int data);

char Uart_Get_Char();
char Uart2_Get_Char();

extern int vsprintf(char *, const char *, va_list);

void Uart_Init(int baud)
{
	int pclk;
	pclk = PCLK;
	
	// PORT GPIO initial
	rGPHCON &= ~(0xf<<4);
	rGPHCON |= (0xa<<4);	

	
	rUFCON1 = 0x0;
	rUMCON1 = 0x0;
	
	/* TODO : Line Control(Normal mode, No parity, One stop bit, 8bit Word length */
	rULCON1 = 0x03;

	/* TODO : Transmit & Receive Mode is polling mode  */
#if !DMA
	rUCON1 = 0x05;
#elif DMA
	rUCON1 = 0x0B;
#endif
	/* TODO : Baud rate ����  */		
	rUBRDIV1=0x22;
	rUDIVSLOT1=0xDFDD;
}

void Uart2_Init(int baud)
{
   int pclk;
   pclk = PCLK;
   
   // PORT GPIO initial
   rGPHCON &= ~(0xf<<8);
   rGPHCON |= (0xa<<8);   

   
   rUFCON2= 0x0;
   rUMCON2 = 0x0;
   
   /* TODO : Line Control(Normal mode, No parity, One stop bit, 8bit Word length */
   rULCON2 = 0x3;

      /* TODO : Transmit & Receive Mode is polling mode  */
      rUCON2  = (0x5);      // fix
      //rUMCON2 = (0<<0) | (1<<4) | (0x7 << 5);
      /* TODO : Baud rate ¼³Á¤  */      
      rUBRDIV2= ((unsigned int)(PCLK/16./baud+0.5)-1 ); 
      //(*(volatile unsigned *)0x50000802c )= 0xDFDD;
}

void Uart_Printf(char *fmt,...)
{
    va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    Uart_Send_String(string);
    va_end(ap);		
}

void Uart2_Printf(char *fmt,...)
{
	va_list ap;
	char string[256];

	va_start(ap, fmt);
	vsprintf(string, fmt, ap);
	Uart2_Send_String(string);
	va_end(ap);
}

void Uart_Send_String(char *pt)
{
	while(*pt)
	{
		/* TODO : ���� �ϳ��� ������Ű�鼭 ���ڿ� ���  */
		/*YOUR CODE HERE*/
	
		Uart_Send_Byte(*pt);
		pt++;
	}	
}

void Uart2_Send_String(char *pt)
{
	while(*pt){
		Uart2_Send_Byte(*pt);
		pt++;
	}
}

void Uart_Send_Byte(int data)
{
	if(data=='\n')
	{
		while(!(rUTRSTAT1 & 0x2));
		WrUTXH1('\r');
	}
	/* TODO : UTRSTAT1�� ���� Ȯ���Ͽ� TX ���۰� ��������� ���� ���   */	
	/*YOUR CODE HERE*/
    while(!(rUTRSTAT1 &  0x2));	
	  	WrUTXH1(data);
}

void Uart2_Send_Byte(int data)
{
	if(data == '\n')
	{
		while(!(rUTRSTAT2 & 0x2));
		WrUTXH2('\r');
	}

	while(!(rUTRSTAT2 & 0x2));
	WrUTXH2(data);
}

char Uart_Get_Char()
{
	/* TODO : UTRSTAT1�� ���� Ȯ���Ͽ� ���ڿ� �Է�   */	
	/*YOUR CODE HERE*/
	  while(!(rUTRSTAT1 & 0x1));

	  return RdURXH1();
}

char Uart2_Get_Char()
{
	while(!(rUTRSTAT2 & 0x1));

	return RdURXH2();
}
