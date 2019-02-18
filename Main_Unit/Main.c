/*
 * =====================================================================
 * NAME         : Main.c
 *
 * Descriptions : Main routine for S3C2450
 *
 * IDE          : GCC-4.1.0
 *
 * Modification
 *    s
 * =====================================================================
 */
#pragma region Includes
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <time.h>
#include <math.h>

#include "2450addr.h"
#include "libc.h"
#include "option.h"
#pragma endregion

#pragma region Definition
#define LCD_XSIZE 		(480)	
#define LCD_YSIZE 		(272)

#define BLACK   0x0000
#define WHITE   0xFFFE
#define BLUE    0x003E
#define GREEN   0x07C0
#define RED     0xF800
#define YELLOW  0xFFC0
#define WHITE_P 0xFFFF
#pragma endregion

uint8_t Viberation_duty = 4;
uint8_t Old_Sec = 0;

bool PWM_Flag = true;
bool Lcd_Refresh = false;
bool Alarm_State = true;

static unsigned long src = 0x33000000;
static unsigned long dst = 0x33100000;
static unsigned int size = 6;
static unsigned long pattern;

#pragma region Prototype
void __attribute__((interrupt("IRQ"))) RTC_TICK(void);
void __attribute__((interrupt("IRQ"))) RTC_ALARM(void);
void __attribute__((interrupt("IRQ"))) DMA_Handler(void);
void __attribute__((interrupt("IRQ"))) TIMER2_Handler(void);
void __attribute__((interrupt("IRQ"))) EINT0_Handler(void);
void __attribute__((interrupt("IRQ"))) EINT4_7_Handler(void);
void __attribute__((interrupt("IRQ"))) EINT8_23_Handler(void);
#pragma endregion
#pragma region Init_Function
void gpio_init(){
    // LED INIT
    GPGCON.GPIO_PIN_1 = ALTERNATIVE_0;  // 메뉴 전환 인터럽트 핀
    GPGCON.GPIO_PIN_2 = OUTPUT;
    GPGCON.GPIO_PIN_4 = OUTPUT;
    GPGCON.GPIO_PIN_5 = OUTPUT;
    GPGCON.GPIO_PIN_6 = OUTPUT;
    GPGCON.GPIO_PIN_7 = OUTPUT; 

    GPBCON.GPIO_PIN_1 = OUTPUT;
    GPBCON.GPIO_PIN_2 = OUTPUT;
    GPBDAT.GPIO_PIN_1 = HIGH;

    GPFCON.GPIO_PIN_0 = ALTERNATIVE_0;
    GPFCON.GPIO_PIN_4 = ALTERNATIVE_0;
}

void EXTI_Init(){
    rINTMOD1 = IRQ;

    rSRCPND1 |= BIT_EINT0 | BIT_EINT4_7 | BIT_EINT8_23;
    
    rINTPND1 |= BIT_EINT0 | BIT_EINT4_7 | BIT_EINT8_23;

    rINTMSK1 &= ~(BIT_EINT0 | BIT_EINT4_7 | BIT_EINT8_23); 

    // rEXTINT0 = (rEXTINT0 & ~(0x7 << 4)) | (FALLING_EDGE << 4);
    // rEXTINT0 |= (rEXTINT0 & ~(0x7 << 0)) | (FALLING_EDGE << 0);
    // rEXTINT1 = (rEXTINT1 & ~(0x7 << 1)) | (RISING_EDGE << 1);

    EXTINT0.EINT0 = FALLING_EDGE;
    EXTINT0.EINT4 = FALLING_EDGE;
    EXTINT1.EINT9 = RISING_EDGE;
    
    rEINTPEND = (0x1 << 9);
    rEINTPEND |= (0x1 << 4); 
    // EINTPEND.EINT4 = 1;
    // EINTPEND.EINT9 = 1;

    rEINTMASK &= ~((0x1 << 4) | (0x1 << 9));
    // EINTMASK.EINT4 = 0;
    // EINTMASK.EINT9 = 0;

    pISR_EINT0 = (unsigned)EINT0_Handler;
    pISR_EINT4_7 = (unsigned)EINT4_7_Handler;
    pISR_EINT8_23 = (unsigned)EINT8_23_Handler;
}

void DMA_Init(){
    Uart2_DMA_Init(115200);
    DMA0_UART_Init();
    
    rINTSUBMSK &= ~(0x1 << 18);
    rINTMSK1 &= ~(0x1 << 17);

    pISR_DMA0 = (unsigned)DMA_Handler;

    DMA0_HW_Start();
}

void RTC_Init(){
    uint16_t year = 19;
    uint8_t month = 2;
    uint8_t date = 15;
    uint8_t hour = 12;
    uint8_t minute = 29;
    uint8_t second = 5;

    TICNT0.TICK_INT_EN = 1;
    TICNT0.TICK_TIME_CNT0 = 0;
    TICNT1.TICK_TIME_CNT1 = 1;
    TICNT2.TICK_TIME_CNT2 = 0;

    RTC_Clear();
    
    RTCCON.TICSEL2 = 14;
    RTCCON.TICSEL = 0;
    RTCCON.CLKRST = 0;
    RTCCON.CNTSEL = 0;
    RTCCON.CLKSEL = 0;
    RTCCON.RTCEN = 1;

    //Uart_Printf("RTCCON = %x\r\n", RTCCON);

    Set_BCD_Time(year, month, date, hour, minute, second);
}

void RTC_Tick_Init(){
    INTMOD1.INT_TICK = IRQ;
    
    SRCPND1.INT_TICK = 1;
    INTPND1.INT_TICK = 1;

    INTMSK1.INT_TICK = 0;

    pISR_TICK = (unsigned)RTC_TICK;
}

void ALARM_Init(int hour, int min){
    RTCALM.ALMEN = 1;
    RTCALM.HOUREN = 1;
    RTCALM.MINEN = 1;
    RTCALM.SECEN = 1;

    Set_ALM_Time(hour, min, 0);
    // rALMHOUR = ((hour / 10) << 4) + (hour % 10);
    // rALMMIN = ((min / 10) << 4) + (min % 10);
    // rALMSEC = (0 << 4) + (0 % 10);
}

void ALARM_Int_Init(){
    INTMOD1.INT_RTC = IRQ;

    SRCPND1.INT_RTC = 1;
    INTPND1.INT_RTC = 1;

    INTMSK1.INT_RTC = 0;

    pISR_RTC = (unsigned)RTC_ALARM;
}

void timer0_init(){
    TCFG0.PRESCALER0 = (33 - 1);
    TCFG1.MUX0 = 0;
    rTCNTB0 = (0xFFFF);
    TCON.TIM0_MANUAL_UPDATE = 1;

    TCON.TIM0_AUTO_RELOAD = 1;
    TCON.TIM0_MANUAL_UPDATE = 0;
    TCON.TIM0_START = 1;
}

void Timer2_Init(){
    // PWM 진동모터
    TCFG0.PRESCALER1 = (128 - 1);
    TCFG1.MUX2 = 0;
    rTCNTB2 = (PCLK / 128 / 2) / 20 - 1;
    TCON.TIM2_MANUAL_UPDATE = 1;
    
    TCON.TIM2_AUTO_RELOAD = 1;
    TCON.TIM2_MANUAL_UPDATE = 0;
    TCON.TIM2_START = 0;
}

void Timer2_Int_Init(){
    INTMOD1.INT_TIMER2 = IRQ;

    SRCPND1.INT_TIMER2 = 1;
    INTPND1.INT_TIMER2 = 1;    

    INTMSK1.INT_TIMER2 = 0;

    pISR_TIMER2 = (unsigned)TIMER2_Handler;
}
#pragma endregion
#pragma region PV_Function
void delay_us(int us){
    volatile unsigned long now, last, diff;
    now = rTCNTO0;
    while(us > 0){
        last = now;
        now = rTCNTO0;
        if(now > last){
            diff = last + (0xFFFF - now) + 1;
        } else {
            diff = last - now;
        }
        us -= diff;
    }
}

void delay_ms(int ms){
    delay_us(ms * 1000);
}

void WT_Delay(int duration){
    rWTCON = (37 << 8) | (3 << 3);
    rWTDAT = duration + 10;
    rWTCNT = duration + 10;
    rWTCON |= (1 << 5);

    while(rWTCNT > 10);
    rWTCON = 0;   
}

void Sound(uint16_t scale, int duration){
    while(duration > 0){
        GPBDAT.GPIO_PIN_1 = 0;
        WT_Delay(10000 / scale);
        GPBDAT.GPIO_PIN_1 = 1;
        WT_Delay(10000 / scale);

        duration -= ((10000 / scale / 10) * 2) * 2;
    }
    delay_ms(1);
}

void Viberate_On(){
    GPGDAT.GPIO_PIN_2 = 1;
}

void Viberate_Off(){
    GPGDAT.GPIO_PIN_2 = 0;
}

void Time_Show(){
    if(Old_Sec != rBCDSEC) {
        Lcd_Printf(10, 10, BLACK, WHITE, 4, 3, "%d/%2d/%2d %s", 2000 + BCDYEAR.YEAR_10 * 10 + BCDYEAR.YEAR_1, BCDMIN.MIN_10 * 10 + BCDMIN.MIN_1, BCDDATE.DATE_10 * 10 + BCDDATE.DATE_1, wdays[get_weekday(2000 + ((rBCDYEAR >> 4) * 10) + (rBCDYEAR & (0xF)), ((rBCDMON >> 4) * 10) + (rBCDMON & (0xF)), ((rBCDDATE >> 4) * 10) + (rBCDDATE & (0xF)))]);
        Lcd_Printf(60, 60, BLACK, WHITE, 6, 9, "%02x : %02x", rBCDHOUR, rBCDMIN);
        Lcd_Printf(400, 150, BLACK, WHITE, 3, 2, "%02x", rBCDSEC);

        Old_Sec = rBCDSEC;
    }
}

void TaskBar_Time_Show(){
    if(Old_Sec != rBCDSEC) {
        Lcd_Printf(10, 10, BLACK, WHITE, 2, 2, "%d/%2d/%2d %s %02x : %02x", 2000 + BCDYEAR.YEAR_10 * 10 + BCDYEAR.YEAR_1, BCDMON.MON_10 * 10 + BCDMON.MON_1, BCDDATE.DATE_10 * 10 + BCDDATE.DATE_1, wdays[get_weekday(2000 + ((rBCDYEAR >> 4) * 10) + (rBCDYEAR & (0xF)), ((rBCDMON >> 4) * 10) + (rBCDMON & (0xF)), ((rBCDDATE >> 4) * 10) + (rBCDDATE & (0xF)))], rBCDHOUR, rBCDMIN);
        Lcd_Printf(370, 25, BLACK, WHITE, 1, 1, "%02x", rBCDSEC);

        Old_Sec = rBCDSEC;
    }
}

void Temp_Show(){
    uint8_t RxBuffer[6];
    uint8_t i = 0;

    Uart2_Send_String("TEMP");
    
    Uart_Printf("TEMP IN\r\n");

    while(Uart2_Get_Char() != 'H');
    RxBuffer[0] = 'H';
    Uart_Printf("%x", RxBuffer[0]);
    for(i = 1; i < 6; i++){
        RxBuffer[i] = Uart2_Get_Char();
        Uart_Printf("%x", RxBuffer[i]);
    }
    
    Uart_Printf("\r\n");

    Uart_Printf("TEMP DATA IN\r\n");

    Uart_Printf("%s\r\n", RxBuffer);
    
    TaskBar_Time_Show();
    Lcd_Printf(10, 50, BLACK, WHITE, 4, 5, "TEMP : %dC", RxBuffer[4]);
    Lcd_Printf(10, 150, BLACK, WHITE, 4, 5, "HUMI : %d%%", RxBuffer[1]);
}

void Dust_Show(){
    uint8_t RxBuffer[6];
    uint8_t i = 0;

    Uart2_Send_String("DUST");
    
    Uart_Printf("DUST IN\r\n");

    while(Uart2_Get_Char() != 'D');
    RxBuffer[0] = 'D';
    for(i = 1; i < 6; i++){
        RxBuffer[i] = Uart2_Get_Char();
        Uart_Printf("%x", RxBuffer[i]);
    }
    Uart_Printf("\r\n");
    Uart_Printf("%s\r\n", RxBuffer);
    Uart_Printf("DUST DATA IN\r\n");

    TaskBar_Time_Show();
    Lcd_Printf(10, 60, BLACK, WHITE, 4, 5, "DUST : ");
    Lcd_Printf(60, 140, BLACK, WHITE, 4, 5, "%3d.%d%d ug/m2", RxBuffer[1] * 100 + RxBuffer[2] * 10 + RxBuffer[3], RxBuffer[4], RxBuffer[5]);
}
#pragma endregion
#pragma region Main
void Main(){
    Uart_Init(115200);
    Uart2_Init(115200);
    timer0_init();
    // Timer2_Init();
    // Timer2_Int_Init();
    RTC_Init();
    // RTC_Tick_Init();
    ALARM_Init(17, 20);
    ALARM_Int_Init();
    gpio_init();
    EXTI_Init();
    Graphic_Init();

    Uart_Printf("Program Started!!\r\n");
    
    Lcd_Clr_Screen(WHITE);
    Lcd_Select_Frame_Buffer(0);

    // Buzzer Test 
    // Sound(C3, 500);
    
    // Vibration Motor Test
    Viberate_On();  
    delay_ms(500);
    Viberate_Off();
    delay_ms(500);

    uint8_t Menu = 1;

    while(1){
        if(GPGDAT.GPIO_PIN_1){
            while(GPGDAT.GPIO_PIN_1);
            Uart_Printf("MENU COUNTED\r\n");
            Menu = 1;
            // Menu++;
            if(Menu == 3){
                Lcd_Clr_Screen(WHITE);
                Menu = 0;
            }
            else if(Menu == 1){
                Lcd_Clr_Screen(WHITE);
            }
            else {
                Lcd_Part_Clr_Screen(0, 45, LCD_XSIZE, LCD_YSIZE, WHITE);
            }
            // Lcd_Select_Frame_Buffer(0);
        }
        switch (Menu)
        {
            case 0:
                Time_Show();
                break;

            case 1:
                Temp_Show();
                break;
            
            case 2:
                Dust_Show();
                break;
        }
        
        // if(Alarm_State){
        //     Viberate_On();
        //     delay_ms(500);
        //     Viberate_Off();
        //     delay_ms(500);
        // }
        // Uart_Printf("%d %d %d\r\n", BCDHOUR.HOUR_10 * 10 + BCDHOUR.HOUR_1, BCDMIN.MIN_10 * 10 + BCDMIN.MIN_1, BCDSEC.SEC_10 * 10 + BCDSEC.SEC_1);
    }
}
#pragma endregion
#pragma region INT_HANDLER

void __attribute__((interrupt("IRQ"))) DMA_Handler(void){
    rINTSUBMSK |= (0x1 << 18);
    rINTMSK1 |= (0x1 << 17);

    rSUBSRCPND |= (0x1 << 18);
    rSRCPND1 |= (0x1 << 17);
    rINTPND1 |= (0x1 << 17);

    Uart_Printf("DMA 실행 결과\r\n");
    MemDump(dst, 15);

    rINTSUBMSK &= ~(0x1 << 18);
    rINTMSK1 &= ~(0x1 << 17);
}

void __attribute__((interrupt("IRQ"))) TIMER2_Handler(void){
    ClearPending1(BIT_TIMER2);
    Uart_Printf("TIM2 INT\r\n");
    
}

void __attribute__((interrupt("IRQ"))) RTC_TICK(void){
    ClearPending1(BIT_TICK);
#if 0
    //Uart_Printf("TICK INT\r\n");
    // if(Viberation_duty == 4)
    //     Viberate_On();
    // else if(Viberation_duty == 2)
    //     Viberate_Off();
    // else if(Viberation_duty == 0)
    //     Viberation_duty = 5;

    // Viberation_duty--;
    
    // LCD REFRESH
    


    // if(GPGDAT.GPIO_PIN_1){
    //     Lcd_Printf(60, 60, BLACK, WHITE, 4, 7, "TEMP : 20");
    // }
    // else {
    //     Lcd_Printf(10, 10, BLACK, WHITE, 4, 3, "%d/%2d/%2d %s", 2000 + ((rBCDYEAR >> 4) * 10) + (rBCDYEAR & (0xF)), ((rBCDMON >> 4) * 10) + (rBCDMON & (0xF)), ((rBCDDATE >> 4) * 10) + (rBCDDATE & (0xF)), wdays[get_weekday(2000 + ((rBCDYEAR >> 4) * 10) + (rBCDYEAR & (0xF)), ((rBCDMON >> 4) * 10) + (rBCDMON & (0xF)), ((rBCDDATE >> 4) * 10) + (rBCDDATE & (0xF)))]);
    //     Lcd_Printf(60, 60, BLACK, WHITE, 6, 9, "%02x : %02x", rBCDHOUR, rBCDMIN);
    //     Lcd_Printf(400, 150, BLACK, WHITE, 3, 2, "%02x", rBCDSEC);
    // }
    // Lcd_Clr_Screen(WHITE);
    // rRTCCON = (0x1C1);
    
    // Uart_Printf("%d\r\n", GPGDAT.GPIO_PIN_1);
    //Uart_Printf("%d %d %d\r\n", BCDHOUR.HOUR_10 * 10 + BCDHOUR.HOUR_1, BCDMIN.MIN_10 * 10 + BCDMIN.MIN_1, BCDSEC.SEC_10 * 10 + BCDSEC.SEC_1);
#endif
}

void __attribute__((interrupt("IRQ"))) RTC_ALARM(void){
    ClearPending1(BIT_RTC);

    Uart_Printf("ALARM INT\r\n");
}

void __attribute__((interrupt("IRQ"))) EINT0_Handler(void){
    ClearPending1(BIT_EINT0);
    Uart_Printf("EINT 0!!!\r\n");
}

void __attribute__((interrupt("IRQ"))) EINT4_7_Handler(void){
    ClearPending1(BIT_EINT4_7);
    if(rEINTPEND & (0x1 << 4)){
        rEINTPEND = (0x1 << 4);
        Uart_Printf("EINT 4!!!\r\n");
    }
}

void __attribute__((interrupt("IRQ"))) EINT8_23_Handler(void){
    ClearPending1(BIT_EINT8_23);
    if(rEINTPEND & (0x1 << 9)){
        rEINTPEND = 0x1 << 9;
        Uart_Printf("MENU Changed!!\r\n");
        GPGDAT.GPIO_PIN_4 = 0;
    }
}
#pragma endregion

// // Global Variables Declaration
// // žñÀûÁöŽÂ CACHE ¿µ¿ªÀÌ ŸÆŽÒ°Í
// static	unsigned long src=0x33000000;
// static	unsigned long dst=0x33100000;
// static	unsigned int  size = 12; /* word size */
// static	unsigned long pattern;

// static void DMA0_ISR(void)
// {
// 	/* ÀÎÅÍ·ŽÆ® Çã¿ëÇÏÁö ŸÊÀœ on DMA0 */
// 	rINTSUBMSK |= (0x1<<18); 
// 	rINTMSK1 |= (0x1<<17);
	
// 	/* TO DO: Pendng Clear on DMA0 */
// 	rSUBSRCPND |= 0x1<<18;
// 	rSRCPND1 |= 0x1<<17;
// 	rINTPND1 |= 0x1<<17;
		
// 	Uart_Printf("__irq ISRœÇÇà°á°ú  ");	
// 	MemDump(dst, 12); 	
	
// 	/*  TO DO: Stop DMA0 trigger  */
// 	rDMASKTRIG0 |= 0x1<<2;
		
// 	/* Masking Disable on DMA0 */
// 	rINTSUBMSK &= ~(0x1<<18);
// 	rINTMSK1 &= ~(0x1<<17);


// }

// void Main(void)
// {	
// 	char value;

// 	Uart_Init(115200);	
// 	DMA0_SW_Init();
	
// 	/* TODO : ÀÎÅÍ·ŽÆ® º€ÅÍ¿¡ DMA0_ISR ÇÔŒö µî·Ï */
// 	pISR_DMA0 = (unsigned int)DMA0_ISR;
			
// 	/*  ÀÎÅÍ·ŽÆ® Çã¿ë on DMA0 */
// 	rINTSUBMSK &= ~(0x1<<18);	
// 	rINTMSK1 &= ~(0x1<<17);
	
// 	DMA0_SW_Start();
// }