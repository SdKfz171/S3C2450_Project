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

    GPBCON.GPIO_PIN_1 = ALTERNATIVE_0;
    GPBDAT.GPIO_PIN_1 = 1;
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
    TCFG0.PRESCALER0 = (33 - 1);
    TCFG1.MUX0 = 0;
    // rTCFG0 = (rTCFG0 & ~0xFF) | (33 - 1);   // 66,000,000 / 33
    // rTCFG1 = (rTCFG1 & ~0xF);               // 66,000,000 / 33 / 2
    rTCNTB0 =  (0xFFFF);                    // Max size of buffer 
    rTCON |= (0x02);                        // update TCNTB0
    rTCON = (rTCON & ~(0xf) | (1 << 3) | (1 << 0)); // auto reload & start timer0
}

// TOUT1 == BUZZER
void timer1_init(){
    TCFG0.PRESCALER0 = (33 - 1);
    TCFG1.MUX1 = 0;
    rTCNTB1 =  ((PCLK / 33 / 2) / C4) - 1;                    // Max size of buffer 
    rTCMPB1 = rTCMPB1 / 2;
    TCON.TIM1_START = 0;
    TCON.TIM1_AUTO_RELOAD = 1;
    TCON.TIM1_START = 1;
}

void Sound(uint16_t scale, int duration){
    rTCNTB1 = ((PCLK / 33 / 2) / scale) - 1;
    rTCMPB1 = rTCNTB1 / 2;
    delay_ms(duration);
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
    timer1_init();
    RTC_Init();
    RTC_Tick_Init();
    gpio_init();
    SPI_Init();

    Uart_Printf("Program Started!!\r\n");
    
    while(1){
        //Uart_Printf("%c", (uint8_t)SPI_RX_DATA0);
        // SPI_Transmit('0');
        // Uart_Printf("MOSI : %d", GPEDAT.GPIO_PIN_12);
        // Uart_Printf("MISO : %d", GPEDAT.GPIO_PIN_13);
        // rGPFDAT = 0x00;
        // rGPGDAT = 0x00;
        // rGPEDAT = 0xFF;
        //GPGDAT.GPIO_PIN_4 = LOW;
        Sound(C5, 100);
        //delay_ms(1000);
        //GPGDAT.GPIO_PIN_4 = HIGH;
        Sound(A5, 100);
        //delay_ms(1000);
        Uart_Printf("%x %x %x\r\n", rBCDHOUR, rBCDMIN, rBCDSEC);
    }
}

void __attribute__((interrupt("IRQ"))) RTC_TICK(void){
    ClearPending1(BIT_TICK);
    Uart_Printf("TICK INT\r\n");
    // LCD REFRESH
}
