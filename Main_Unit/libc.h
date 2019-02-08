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
    unsigned char GPIO_PIN_10  : 1;
    unsigned int RESERVED          : 21;
} GPIOB; 

enum OUTPUT_LEVEL {
    HIGH = 1,
    LOW = 0
};

enum IO_MODE {
    INPUT = (0x0),
    OUTPUT = (0x1),
    ALTERNATIVE_0 = (0x2),
    ALTERNATIVE_1 = (0x3)
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

typedef struct {
    uint8_t  PRESCALER0 : 8;
    uint8_t  PRESCALER1 : 8;
    uint8_t  DEAD_ZONE_LEN : 8;
    uint8_t RESERVED : 8;
} TCFG0_;

typedef struct 
{
    uint8_t MUX0 : 4;
    uint8_t MUX1 : 4;
    uint8_t MUX2 : 4;
    uint8_t MUX3 : 4;
    uint8_t MUX4 : 4;
    uint8_t DMA_MODE : 4;
    uint8_t RESERVED : 8;
} TCFG1_;

typedef struct 
{
    uint8_t TIM0_START : 1;
    uint8_t TIM0_MANUAL_UPDATE : 1;
    uint8_t TIM0_OUTPUT_INVERTER : 1;
    uint8_t TIM0_AUTO_RELOAD : 1;

    uint8_t DEAD_ZONE_ENABLE : 1;
    uint8_t RESERVED : 3;

    uint8_t TIM1_START : 1;
    uint8_t TIM1_MANUAL_UPDATE : 1;
    uint8_t TIM1_OUTPUT_INVERTER : 1;
    uint8_t TIM1_AUTO_RELOAD : 1;
    
    uint8_t TIM2_START : 1;
    uint8_t TIM2_MANUAL_UPDATE : 1;
    uint8_t TIM2_OUTPUT_INVERTER : 1;
    uint8_t TIM2_AUTO_RELOAD : 1;
    
    uint8_t TIM3_START : 1;
    uint8_t TIM3_MANUAL_UPDATE : 1;
    uint8_t TIM3_OUTPUT_INVERTER : 1;
    uint8_t TIM3_AUTO_RELOAD : 1;

    uint8_t TIM4_START : 1;
    uint8_t TIM4_MANUAL_UPDATE : 1;
    uint8_t TIM4_AUTO_RELOAD : 1;
} TCON_;

typedef enum {
    C4 = 262,
    C4_ = 278,
    D4 = 294,
    D4_ = 311,
    E4 = 330,
    F4 = 349,
    F4_ = 370,
    G4 = 392,
    G4_ = 415,
    A4 = 440,
    A4_ = 466, 
    B4 = 494,

    C5 = 523,
    C5_ = 554,
    D5 = 587,
    D5_ = 622,
    E5 = 659,
    F5 = 699,
    F5_ = 740,
    G5 = 784,
    G5_ = 831,
    A5 = 880,
    A5_ = 932, 
    B5 = 988,
} SCALE;

#define GPGCON    (*(volatile GPCON *)0x56000060)
#define GPGDAT    (*(volatile GPIOG *)0x56000064)

#define GPFCON (*(volatile GPCON *)0x56000050)
#define GPFDAT (*(volatile GPIOF *)0x56000054)

#define GPECON (*(volatile GPCON *)0x56000040)
#define GPEDAT (*(volatile GPIOE *)0x56000044)

#define GPBCON (*(volatile GPCON *)0x56000010)
#define GPBDAT (*(volatile GPIOB *)0x56000014)

#define TCFG0 (*(volatile TCFG0_ *)0x51000000)
#define TCFG1 (*(volatile TCFG1_ *)0x51000004)
#define TCON (*(volatile TCON_ *)0x51000008)


#endif
