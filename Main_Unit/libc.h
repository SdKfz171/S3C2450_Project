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

// Lcd.c
extern void Graphic_Init(void);
extern void Lcd_Wait_Blank(void);
extern void Lcd_Copy(unsigned from, unsigned int to);
extern void Lcd_Select_Frame_Buffer(unsigned int id);
extern void Lcd_Display_Frame_Buffer(unsigned int id);
extern void Lcd_Set_Tran_Mode(int mode);
extern void Lcd_Put_Pixel(int x,int y,int c);
extern void Lcd_Clr_Screen(unsigned long color);
extern void Lcd_Hline(int y, int x1, int x2, int color);
extern void Lcd_Vline(int x, int y1, int y2, int color);
extern void Lcd_Line(int x1,int y1,int x2,int y2,int color);
extern void Lcd_Rectangular(int x1, int y1, int x2, int y2, int color);
extern void Lcd_Fill_Rectangular(int x1, int y1, int x2, int y2, int color);
extern void Lcd_Printf(int x, int y, int color, int bkcolor, int zx, int zy, char *fmt,...);
extern void Lcd_Get_Info_BMP(int * x, int  * y, const unsigned char *fp);
extern void Lcd_Draw_BMP(int x, int y, const unsigned char *fp);

extern int mdays[12];
extern char wdays[7][4];

extern int leap_check(int year);
extern int get_days(int year, int month);
extern int get_weekday(int year, int month, int date);
extern void disp_calendar(int year, int month);

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
    unsigned char  PRESCALER0 : 8;
    unsigned char  PRESCALER1 : 8;
    unsigned char  DEAD_ZONE_LEN : 8;
    unsigned char RESERVED : 8;
} TCFG0_;

typedef struct 
{
    unsigned char MUX0 : 4;
    unsigned char MUX1 : 4;
    unsigned char MUX2 : 4;
    unsigned char MUX3 : 4;
    unsigned char MUX4 : 4;
    unsigned char DMA_MODE : 4;
    unsigned char RESERVED : 8;
} TCFG1_;

typedef struct 
{
    unsigned char TIM0_START : 1;
    unsigned char TIM0_MANUAL_UPDATE : 1;
    unsigned char TIM0_OUTPUT_INVERTER : 1;
    unsigned char TIM0_AUTO_RELOAD : 1;

    unsigned char DEAD_ZONE_ENABLE : 1;
    unsigned char RESERVED : 3;

    unsigned char TIM1_START : 1;
    unsigned char TIM1_MANUAL_UPDATE : 1;
    unsigned char TIM1_OUTPUT_INVERTER : 1;
    unsigned char TIM1_AUTO_RELOAD : 1;
    
    unsigned char TIM2_START : 1;
    unsigned char TIM2_MANUAL_UPDATE : 1;
    unsigned char TIM2_OUTPUT_INVERTER : 1;
    unsigned char TIM2_AUTO_RELOAD : 1;
    
    unsigned char TIM3_START : 1;
    unsigned char TIM3_MANUAL_UPDATE : 1;
    unsigned char TIM3_OUTPUT_INVERTER : 1;
    unsigned char TIM3_AUTO_RELOAD : 1;

    unsigned char TIM4_START : 1;
    unsigned char TIM4_MANUAL_UPDATE : 1;
    unsigned char TIM4_AUTO_RELOAD : 1;
} TCON_;

/*
    옥타브 	    0 	    1 	    2 	    3 	    4 	    5 	    6 	    7 	    8
    도(C) 	    16      33      65 	    131     262     523     1047    2093 	4186
    도#(C#)     17      35      69 	    139     278     554     1109 	2218 	4435
    레(D) 	    18      37      73 	    147     294     587     1175 	2349 	4699
    레#(D#)     20      39      78 	    156     311     622     1245 	2489 	4978
    미(E)       21      41      82 	    165     330     659     1319 	2637 	5274
    파(F)       22      44      87 	    175     349     699     1397 	2794 	5588
    파#(F#)     23      46      93 	    185     370     740     1475 	2960 	5920
    솔(G) 	    25      49      98 	    196     392     784     1568 	3136 	6272
    솔#(G#)     26      52      104     208     415     831     1661 	3322 	6645
    라(A) 	    28      55      110     220     440     880     1760 	3520 	7040
    라#(A#)     29      58      117     233     466     932     1865 	3729 	7459
    시(B)       31      62      124     247     494     988     1976 	3951 	7902
*/

typedef enum {
    C0 = 16,    C1 = 33,    C2 = 65,    C3 = 131,   C4 = 262,   C5 = 523,   C6 = 1047,  C7 = 2093,  C8 = 4186,
    C0_ = 17,   C1_ = 35,   C2_ = 69,   C3_ = 139,  C4_ = 278,  C5_ = 554,  C6_ = 1109, C7_ = 2218, C8_ = 4435,
    D0 = 18,    D1 = 37,    D2 = 73,    D3 = 147,   D4 = 294,   D5 = 587,   D6 = 1175,  D7 = 2349,  D8 = 4699,
    D0_ = 20,   D1_ = 39,   D2_ = 78,   D3_ = 156,  D4_ = 311,  D5_ = 622,  D6_ = 1245, D7_ = 2489, D8_ = 4978,
    E0 = 21,    E1 = 41,    E2 = 82,    E3 = 165,   E4 = 330,   E5 = 659,   E6 = 1319,  E7 = 2637,  E8 = 5274,
    F0 = 22,    F1 = 319,   F2 = 87,    F3 = 175,   F4 = 349,   F5 = 699,   F6 = 1397,  F7 = 2794,  F8 = 5588,
    F0_ = 23,   F1_ = 370,  F2_ = 93,   F3_ = 185,  F4_ = 370,  F5_ = 740,  F6_ = 1475, F7_ = 2960, F8_ = 5920,
    G0 = 25,    G1 = 392,   G2 = 98,    G3 = 196,   G4 = 392,   G5 = 784,   G6 = 1568,  G7 = 3136,  G8 = 6272,
    G0_ = 26,   G1_ = 115,  G2_ = 104,  G3_ = 208,  G4_ = 415,  G5_ = 831,  G6_ = 1661, G7_ = 3322, G8_ = 6645,
    A0 = 28,    A1 = 110,   A2 = 110,   A3 = 220,   A4 = 440,   A5 = 880,   A6 = 1760,  A7 = 3520,  A8 = 7040,
    A0_ = 29,   A1_ = 166,  A2_ = 117,  A3_ = 233,  A4_ = 466,  A5_ = 932,  A6_ = 1865, A7_ = 3729, A8_ = 7459,
    B0 = 31,    B1 = 191,   B2 = 124,   B3 = 247,   B4 = 494,   B5 = 988,   B6 = 1976,  B7 = 3951,  B8 = 7902
} SCALE;

#define GPGCON  (*(volatile GPCON *)0x56000060)
#define GPGDAT  (*(volatile GPIOG *)0x56000064)

#define GPFCON  (*(volatile GPCON *)0x56000050)
#define GPFDAT  (*(volatile GPIOF *)0x56000054)

#define GPECON  (*(volatile GPCON *)0x56000040)
#define GPEDAT  (*(volatile GPIOE *)0x56000044)

#define GPBCON  (*(volatile GPCON *)0x56000010)
#define GPBDAT  (*(volatile GPIOB *)0x56000014)

#define TCFG0   (*(volatile TCFG0_ *)0x51000000)
#define TCFG1   (*(volatile TCFG1_ *)0x51000004)
#define TCON    (*(volatile TCON_ *)0x51000008)

#endif