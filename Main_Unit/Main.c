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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "2450addr.h"
#include "libc.h"
#include "option.h"

#define BLACK   0x0000
#define WHITE   0xFFFE
#define BLUE    0x003E
#define GREEN   0x07C0
#define RED     0xF800
#define YELLOW  0xFFC0
#define WHITE_P 0xFFFF

uint16_t year = 19;
uint8_t month = 2;
uint8_t date = 13;
uint8_t hour = 17;
uint8_t minute = 50;
uint8_t second = 20;

void __attribute__((interrupt("IRQ"))) RTC_TICK(void);
void __attribute__((interrupt("IRQ"))) RTC_ALARM(void);
void __attribute__((interrupt("IRQ"))) EINT8_23_Handler(void);

void gpio_init(){
    // LED INIT
    GPGCON.GPIO_PIN_1 = ALTERNATIVE_0;  // 메뉴 전환 인터럽트 핀
    GPGCON.GPIO_PIN_4 = OUTPUT;
    GPGCON.GPIO_PIN_5 = OUTPUT;
    GPGCON.GPIO_PIN_6 = OUTPUT;
    GPGCON.GPIO_PIN_7 = OUTPUT; 

    GPBCON.GPIO_PIN_1 = OUTPUT;
    GPBDAT.GPIO_PIN_1 = HIGH;
}

void EXTI_Init(){
    INTMOD1.EINT8_23 = IRQ;

    SRCPND1.EINT8_23 = 1;
    INTPND1.EINT8_23 = 1;
    INTMSK1.EINT8_23 = 0;
    
    EXTINT1.EINT9 = RISING_EDGE;
    
    EINTPEND.EINT9 = 1;
    EINTMASK.EINT9 = 0;

    pISR_EINT8_23 = (unsigned)EINT8_23_Handler;
}

void RTC_Init(){
    // rRTCCON = (0x1CF);

    TICNT0.TICK_INT_EN = 1;
    TICNT0.TICK_TIME_CNT0 = 0;
    TICNT1.TICK_TIME_CNT1 = 1;
    TICNT2.TICK_TIME_CNT2 = 0;
    // rTICNT0 = (0x80);
    // rTICNT1 = (0x1);
    // rTICNT2 = (0x0);

    RTCCON.RTCEN = 0;
    RTCCON.CLKSEL = 0;
    RTCCON.CNTSEL = 0;
    RTCCON.CLKRST = 0;
    RTCCON.TICSEL = 0;
    RTCCON.TICSEL2 = 14;
    // rRTCCON = ~(0x1F);
    // rRTCCON = (0x1C1);

    Set_BCD_Time(year, month, date, hour, minute, second);
    // rBCDYEAR = ((year / 10) << 4) + (year % 10);
    // rBCDMON = ((month / 10) << 4) + (month % 10);
    // rBCDDATE = ((date / 10) << 4) + (date % 10);
    // rBCDHOUR = ((hour / 10) << 4) +(hour % 10);
    // rBCDMIN = ((minute / 10) << 4) +(minute % 10);
    // rBCDSEC = ((second / 10) << 4) +(second % 10);
    //rRTCCON = (0x0);
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

void Main(){
    Uart_Init(115200);
    timer0_init();
    RTC_Init();
    RTC_Tick_Init();
    ALARM_Init(17, 20);
    ALARM_Int_Init();
    gpio_init();
    EXTI_Init();
    Graphic_Init();

    Uart_Printf("Program Started!!\r\n");
    
    Lcd_Clr_Screen(WHITE);
    Lcd_Select_Frame_Buffer(0);

    // // Buzzer Test 
    // Sound(C3, 500);
    
    while(1){
        // Uart_Printf("%d %d %d\r\n",  ((rBCDHOUR >> 4) * 10) + (rBCDHOUR & (0xF)), ((rBCDMIN >> 4) * 10) + (rBCDMIN & (0xF)), ((rBCDSEC >> 4) * 10) + (rBCDSEC & (0xF)));
    }
}

void __attribute__((interrupt("IRQ"))) RTC_TICK(void){
    ClearPending1(BIT_TICK);
    Uart_Printf("TICK INT\r\n");
    
    // LCD REFRESH
    // Lcd_Clr_Screen(WHITE);
    // rRTCCON = (0x1C1);
    Lcd_Printf(10, 10, BLACK, WHITE, 4, 3, "%d/%2d/%2d %s", 2000 + ((rBCDYEAR >> 4) * 10) + (rBCDYEAR & (0xF)), ((rBCDMON >> 4) * 10) + (rBCDMON & (0xF)), ((rBCDDATE >> 4) * 10) + (rBCDDATE & (0xF)), wdays[get_weekday(2000 + ((rBCDYEAR >> 4) * 10) + (rBCDYEAR & (0xF)), ((rBCDMON >> 4) * 10) + (rBCDMON & (0xF)), ((rBCDDATE >> 4) * 10) + (rBCDDATE & (0xF)))]);
    Lcd_Printf(60, 60, BLACK, WHITE, 6, 9, "%02x : %02x", rBCDHOUR, rBCDMIN);
    Lcd_Printf(400, 150, BLACK, WHITE, 3, 2, "%02x", rBCDSEC);

    // date++;
    // if(date > mdays[((rBCDMON >> 4) * 10) + (rBCDMON & (0xF)) - 1]){
    //     date = 1;
    //     month++;
    // }
    // if(month > 12){
    //     month = 1;
    //     year++;
    // }

    // rBCDYEAR = ((year / 10) << 4) + (year % 10);
    // rBCDMON = ((month / 10) << 4) + (month % 10);
    // rBCDDATE = ((date / 10) << 4) + (date % 10);
}

void __attribute__((interrupt("IRQ"))) RTC_ALARM(void){
    ClearPending1(BIT_RTC);
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
    Uart_Printf("ALARM INT\r\n");
}

void __attribute__((interrupt("IRQ"))) EINT8_23_Handler(void){
    ClearPending1(BIT_EINT8_23);
    if(EINTPEND.EINT9 == 1){
        EINTPEND.EINT9 = 1;
        // MENU Change

    }
}