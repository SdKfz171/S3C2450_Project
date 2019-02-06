/*
 * =====================================================================
 * NAME         : libc.h
 *
 * Descriptions : Definition of S3C2450 Library prototype
 *
 * IDE          : GCC-4.1.0
 *
 * Modification
 *	   
 * =====================================================================
 */
#ifndef __LIBC_H__
#define __LIBC_H__

// extern void MemFill(unsigned long ptr, unsigned long pattern, int size);
// extern void MemDump(unsigned long ptr, int size);

// Uart ���� �Լ� 
extern void Uart_Init(int baud);
extern void Uart_Printf(char *fmt,...);
extern void Uart_Send_String(char *pt);
extern void Uart_Send_Byte(int data);
extern char Uart_Get_Char();

// LED ���� �Լ� 
extern void Led_Init();
extern void Led_Display(int data);

// Timer ���� �Լ� 
extern void Timer_Init(void);
extern void Timer_Delay(int msec);

typedef struct {
    unsigned char GPIO_PIN_0    : 1;
    unsigned char GPIO_PIN_1    : 1;
    unsigned char GPIO_PIN_2    : 1;
    unsigned char GPIO_PIN_3    : 1;
    // unsigned char LED           : 4;
    unsigned char GPIO_PIN_4    : 1;
    unsigned char GPIO_PIN_5    : 1;
    unsigned char GPIO_PIN_6    : 1;
    unsigned char GPIO_PIN_7    : 1;
    unsigned char GPIO_PIN_8    : 1;
    unsigned char GPIO_PIN_9    : 1;
    unsigned char GPIO_PIN_10   : 1;
    unsigned char GPIO_PIN_11   : 1;
    unsigned char GPIO_PIN_12   : 1;
    unsigned char GPIO_PIN_13   : 1;
    unsigned char GPIO_PIN_14   : 1;
    unsigned char GPIO_PIN_15   : 1;
} GPIOG; 

typedef struct {
    unsigned char GPIO_PIN_0    : 1;
    unsigned char GPIO_PIN_1    : 1;
    unsigned char GPIO_PIN_2    : 1;
    unsigned char GPIO_PIN_3    : 1;
    unsigned char GPIO_PIN_4    : 1;
    unsigned char GPIO_PIN_5    : 1;
    unsigned char GPIO_PIN_6    : 1;
    unsigned char GPIO_PIN_7    : 1;
    unsigned char res           : 8;
} GPIOF;

typedef struct {
    unsigned char GPIO_PIN_0    : 1;
    unsigned char GPIO_PIN_1    : 1;
    unsigned char GPIO_PIN_2    : 1;
    unsigned char GPIO_PIN_3    : 1;
    unsigned char GPIO_PIN_4    : 1;
    unsigned char GPIO_PIN_5    : 1;
    unsigned char GPIO_PIN_6    : 1;
    unsigned char GPIO_PIN_7    : 1;
    unsigned char GPIO_PIN_8    : 1;
    unsigned char GPIO_PIN_9    : 1;
    unsigned char GPIO_PIN_10   : 1;
    unsigned char GPIO_PIN_11   : 1;
    unsigned char GPIO_PIN_12   : 1;
    unsigned char GPIO_PIN_13   : 1;
    unsigned char GPIO_PIN_14   : 1;
    unsigned char GPIO_PIN_15   : 1;
} GPIOE; 

enum OUTPUT_LEVEL {
    HIGH = 1,
    LOW = 0
};

enum IO_MODE {
    INPUT = (0x0),
    OUTPUT = (0x1),
    EINT = (0x2),
    RESERVED = (0x3)
};

enum EXTI_MODE {
    LOW_LEVEL = (0x0),
    HIGH_LEVEL = (0x1),
    FALLING_EDGE = (0x2),
    RISING_EDGE = (0x4),
    BOTH_EDGE = (0x6)
};

typedef struct {
    unsigned char GPIO_PIN_0   : 2;
    unsigned char GPIO_PIN_1   : 2;
    unsigned char GPIO_PIN_2   : 2;
    unsigned char GPIO_PIN_3   : 2;
    unsigned char GPIO_PIN_4   : 2;
    unsigned char GPIO_PIN_5   : 2;
    unsigned char GPIO_PIN_6   : 2;
    unsigned char GPIO_PIN_7   : 2;
    unsigned char GPIO_PIN_8   : 2;
    unsigned char GPIO_PIN_9   : 2;
    unsigned char GPIO_PIN_10  : 2;
    unsigned char GPIO_PIN_11  : 2;
    unsigned char GPIO_PIN_12  : 2;
    unsigned char GPIO_PIN_13  : 2;
    unsigned char GPIO_PIN_14  : 2;
    unsigned char GPIO_PIN_15  : 2;
} GPCON;

#define GPGCON    (*(volatile GPCON *)0x56000060)
#define GPGDAT    (*(volatile GPIOG *)0x56000064)

#define GPFCON (*(volatile GPCON *)0x56000050)
#define GPFDAT (*(volatile GPIOF *)0x56000054)

#define GPECON (*(volatile GPCON *)0x56000040)
#define GPEDAT (*(volatile GPIOE *)0x56000044)

#endif
