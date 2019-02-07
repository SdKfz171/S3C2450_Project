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

#define SLAVE_ADDR 0x0000

#define FREQ 1000000

uint16_t year = 19;
uint8_t month = 2;
uint8_t date = 8;
uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;

void __attribute__((interrupt("IRQ"))) RTC_TICK(void);

void gpio_init(){
    // LED INIT
    GPGCON.GPIO_PIN_4 = OUTPUT;
    GPGCON.GPIO_PIN_5 = OUTPUT;
    GPGCON.GPIO_PIN_6 = OUTPUT;
    GPGCON.GPIO_PIN_7 = OUTPUT; 

    rGPECON = 0x55555555;
    GPECON.GPIO_PIN_14 = OUTPUT;
    // GPECON.GPIO_PIN_13 = 0x2;
    // GPECON.GPIO_PIN_12 = 0x2;
    // GPECON.GPIO_PIN_11 = 0x2;
}

void RTC_Init(){
    // rRTCCON = (0x1C0);

    rTICNT0 = (0x80);
    rTICNT1 = (0x1);
    rTICNT2 = (0x0);

    rRTCCON = ~(0x1F);
    rRTCCON = (0x1C1);
    rBCDYEAR = ((year / 10) << 4) + (year % 10);
    rBCDMON = ((month / 10) << 4) + (month % 10);
    rBCDDATE = ((date / 10) << 4) + (date % 10);
    rBCDHOUR = ((hour / 10) << 4) +(hour % 10);
    rBCDHOUR = ((minute / 10) << 4) +(minute % 10);
    rBCDHOUR = ((second / 10) << 4) +(second % 10);
    //rRTCCON = (0x0);
}

void RTC_Tick_Init(){
    rINTMOD1 = (0x0);
    
    rINTMSK1 = BIT_ALLMSK;
    
    rSRCPND1 = (BIT_TICK);
    rINTPND1 = (BIT_TICK);

    rINTMSK1 = ~(BIT_TICK);

    pISR_TICK = (unsigned)RTC_TICK;
}


void timer0_init(){
    rTCFG0 = (rTCFG0 & ~0xFF) | (50 - 1);   // 66,000,000 / 33
    rTCFG1 = (rTCFG1 & ~0xF);               // 66,000,000 / 33 / 2
    rTCNTB0 =  (0xFFFF);                    // Max size of buffer 
    rTCON |= (0x02);                        // update TCNTB0
    rTCON = (rTCON & ~(0xf) | (1 << 3) | (1 << 0)); // auto reload & start timer0
}

void delay_us(int us){
    volatile unsigned long now, last, diff;
    now = rTCNTO0;
    while(us > 0){
        last = now;
        now = rTCNTO0;
        if(now > last){ // timer reloaded
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

void Main(){
    Uart_Init(115200);
    timer0_init();
    RTC_Init();
    RTC_Tick_Init();
    gpio_init();

    Uart_Printf("Program Started!!\r\n");
    
    while(1){
        GPGDAT.GPIO_PIN_4 = LOW;
        delay_ms(1000);
        GPGDAT.GPIO_PIN_4 = HIGH;
        delay_ms(1000);
        Uart_Printf("%x %x %x\r\n", rBCDHOUR, rBCDMIN, rBCDSEC);
    }
}

void __attribute__((interrupt("IRQ"))) RTC_TICK(void){
    ClearPending1(BIT_TICK);
    Uart_Printf("TICK INT\r\n");
}
