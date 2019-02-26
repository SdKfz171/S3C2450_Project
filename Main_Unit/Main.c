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

uint8_t Old_Sec = 0;

int Menu = 0;

bool Alarm_State = false;

uint8_t S_Buffer[14];
uint8_t A_Buffer[4];

#pragma region Prototype
void __attribute__((interrupt("IRQ"))) RTC_TICK(void);
void __attribute__((interrupt("IRQ"))) RTC_ALARM(void);
void __attribute__((interrupt("IRQ"))) DMA_Handler(void);
void __attribute__((interrupt("IRQ"))) TIMER2_Handler(void);
void __attribute__((interrupt("IRQ"))) EINT0_Handler(void);
void __attribute__((interrupt("IRQ"))) EINT4_7_Handler(void);
void __attribute__((interrupt("IRQ"))) EINT8_23_Handler(void);
void __attribute__((interrupt("IRQ"))) UART2_RX_Handler(void);
#pragma endregion
#pragma region Init_Function
void gpio_init(){
    // LED INIT
    GPGCON.GPIO_PIN_2 = OUTPUT;
    GPGCON.GPIO_PIN_4 = OUTPUT;
    GPGCON.GPIO_PIN_5 = OUTPUT;
    GPGCON.GPIO_PIN_6 = OUTPUT;
    GPGCON.GPIO_PIN_7 = OUTPUT; 

    GPBCON.GPIO_PIN_1 = OUTPUT;
    GPBCON.GPIO_PIN_2 = OUTPUT;
    GPBDAT.GPIO_PIN_1 = HIGH;

    GPFCON.GPIO_PIN_0 = INPUT;
    GPFCON.GPIO_PIN_1 = INPUT;
    GPFCON.GPIO_PIN_3 = INPUT;
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

    // Set_BCD_Time(year, month, date, hour, minute, second);
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
    // Uart_Printf("TIME SHOW\r\n");
    if(Old_Sec != rBCDSEC) {
        Lcd_Printf(10, 10, BLACK, WHITE, 4, 3, "%d/%2d/%2d %s", 2000 + BCDYEAR.YEAR_10 * 10 + BCDYEAR.YEAR_1, BCDMON.MON_10 * 10 + BCDMON.MON_1, BCDDATE.DATE_10 * 10 + BCDDATE.DATE_1, wdays[get_weekday(2000 + BCDYEAR.YEAR_10 * 10 + BCDYEAR.YEAR_1, BCDMON.MON_10 * 10 + BCDMON.MON_1, BCDDATE.DATE_10 * 10 + BCDDATE.DATE_1)]);
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

    Uart_Send_String("TEMP");

    RxBuffer[0] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[0]);
    RxBuffer[1] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[1]);
    RxBuffer[2] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[2]);
    RxBuffer[3] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[3]);
    RxBuffer[4] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[4]);
    RxBuffer[5] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[5]);
    
    // Uart2_Send_String("TEMP");

    // Uart_Printf("TEMP IN\r\n");

    // Uart_Printf("\r\n");

    // Uart_Printf("TEMP DATA IN\r\n");

    // Uart_Printf("%s\r\n", RxBuffer);
    
    TaskBar_Time_Show();
    Lcd_Printf(10, 50, BLACK, WHITE, 4, 5, "TEMP : %2dC", RxBuffer[4]);
    Lcd_Printf(10, 150, BLACK, WHITE, 4, 5, "HUMI : %2d%%", RxBuffer[1]);
}

void Dust_Show(){
    uint8_t RxBuffer[6];
    uint8_t i = 0;

    Uart_Send_String("DUST");

    RxBuffer[0] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[0]);
    RxBuffer[1] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[1]);
    RxBuffer[2] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[2]);
    RxBuffer[3] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[3]);
    RxBuffer[4] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[4]);
    RxBuffer[5] = Uart_Get_Char();
    // Uart_Printf("%x", RxBuffer[5]);

    // Uart_Printf("DUST IN\r\n");

    Uart_Printf("\r\n");

    // Uart_Printf("DUST DATA IN\r\n");

    TaskBar_Time_Show();
    Lcd_Printf(10, 60, BLACK, WHITE, 4, 5, "DUST : ");
    Lcd_Printf(60, 140, BLACK, WHITE, 4, 5, "%3d.%d%d ug/m2", RxBuffer[1] * 100 + RxBuffer[2] * 10 + RxBuffer[3], RxBuffer[4], RxBuffer[5]);
}

void Alarm_Show(){
    // Uart_Printf("GPFDAT GP0 : %d\r\n", GPFDAT.GPIO_PIN_0);
    if(GPFDAT.GPIO_PIN_0 || GPFDAT.GPIO_PIN_1){
        Lcd_Printf(10, 80, BLACK, WHITE, 7, 4, "ALARM ON");
        Viberate_On();
        Sound(C6, 1);
    } 
}
#pragma endregion

#pragma region Main
void Main(){
    Uart_Init(115200);
    timer0_init();
    RTC_Init();
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

    uint8_t i = 0;

    uint16_t set_year = 0;
    uint8_t set_month = 0;
    uint8_t set_date = 0;
    uint8_t set_hour = 0;
    uint8_t set_min = 0;
    uint8_t set_sec = 0;

    uint8_t alm_hour = 0;
    uint8_t alm_min = 0;
    // Time_Show();

    uint8_t first_data;
    uint16_t start = 0;
    uint16_t last = 0;
    while(1){
        if(!GPFDAT.GPIO_PIN_1){
            Viberate_Off();
        }
        if(!GPFDAT.GPIO_PIN_0 && Alarm_State){
            Lcd_Clr_Screen(WHITE);
            Alarm_State = false;
            Viberate_Off();
            Menu = 0;
        }
        if(Alarm_State == true){
            // Lcd_Clr_Screen(WHITE);
            // Alarm_State = false;
            Menu = 3;
        }
        if(rUTRSTAT1 & (0x1)){
            first_data = Uart_Get_Char();
            if(first_data == 'S'){
                for(i = 0; i < 14; i++){
                    S_Buffer[i] = Uart_Get_Char();
                    // Uart_Printf("%x ", S_Buffer[i]);
                }

                set_month = S_Buffer[0] * 10 + S_Buffer[1];
                set_date = S_Buffer[2] * 10 + S_Buffer[3];
                set_year = S_Buffer[4] * 1000 + S_Buffer[5] * 100 + S_Buffer[6] * 10 + S_Buffer[7];
                set_hour = S_Buffer[8] * 10 + S_Buffer[9];
                set_min = S_Buffer[10] * 10 + S_Buffer[11];
                set_sec = S_Buffer[12] * 10 + S_Buffer[13];

                Uart_Printf("Year : %d\n", set_year);
                Uart_Printf("Month : %d\n", set_month);
                Uart_Printf("Date : %d\n", set_date);
                Uart_Printf("Hour : %d\n", set_hour);
                Uart_Printf("Min : %d\n", set_min);
                Uart_Printf("Sec : %d\n", set_sec);

                Set_BCD_Time(set_year, set_month, set_date, set_hour, set_min, set_sec);
            }
            else if(first_data == 'A'){
                for(i = 0; i < 4; i++){
                    A_Buffer[i] = Uart_Get_Char();
                }

                alm_hour = A_Buffer[0] * 10 + A_Buffer[1];
                alm_min = A_Buffer[2] * 10 + A_Buffer[3];

                Uart_Printf("Alarm Hour : %d\n", alm_hour);
                Uart_Printf("Alarm min : %d\n", alm_min);

                if(alm_hour > 23 || alm_min > 59)
                    Lcd_Printf(10, 200, BLACK, WHITE, 4, 4, "ALARM SET FAIL");
                else {
                    Set_ALM_Time(alm_hour, alm_min, 0);
                    Lcd_Printf(10, 200, BLACK, WHITE, 4, 4, "ALARM SETTED!!");
                }
            }
            else if(first_data == 'M'){
                if(Alarm_State){
                    Alarm_State = false;
                    Viberate_Off();
                    Menu = 0;
                }
                // Uart_Printf("MENU COUNTED\r\n");
                Menu++;

                if(Menu == 3){
                    Lcd_Clr_Screen(WHITE);
                    Menu = 0;
                }
                else if(Menu == 1){
                    Lcd_Clr_Screen(WHITE);
                }
                else {
                    // if(Menu == 4){
                    //     Menu = 0;
                    //     Viberate_Off();
                    // }
                    Lcd_Part_Clr_Screen(0, 45, LCD_XSIZE, LCD_YSIZE, WHITE);
                }
                // Lcd_Select_Frame_Buffer(0);
            }
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
            
            case 3:
                Alarm_Show();
                break;
        }
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
}

void __attribute__((interrupt("IRQ"))) RTC_ALARM(void){
    ClearPending1(BIT_RTC);

    Uart_Printf("ALARM INT\r\n");
    Alarm_State = true;
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
        Viberate_Off();
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

void __attribute__((interrupt("IRQ"))) UART2_RX_Handler(void){
    rINTSUBMSK |= (0x1 << 6);
    rINTMSK1 |= (0x1 << 15);

    rSUBSRCPND |= (0x1 << 6);
    rSRCPND1 |= (0x1 << 15);
    rINTPND1 |= (0x1 << 15);

    Uart_Send_String("Uart INT Received\n");

    if(Uart2_Check_Overrun_Error())
        Uart_Send_Byte('*');
        Uart_Send_Byte('\n');

    if(rURXH2 == 'A'){

    }else if(rURXH2 == 'S'){
        
    }
    
    Uart_Printf("%c\r\n", rURXH2);

    rINTSUBMSK &= ~(0x1 << 6);
    rINTMSK1 &= ~(0x1 << 15);
    rUCON2 |= (0x1 << 6);
}

#pragma endregion