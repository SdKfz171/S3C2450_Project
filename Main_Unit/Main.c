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
    rRTCCON = (0x1C0);

    rTICNT0 = (0x80);
    rTICNT1 = (0x1);
    rTICNT2 = (0x0);
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

// void I2C_Init(){
//     // I2C SCL, SDA PIN Init
//     rGPECON = (0xA0000000);
//     rGPEUDP = (0xC000);

//     rIICCON = (1 << 7) | (0 << 6) | (1 << 5) |  (0xE); // 송신 클럭 값 = 66000000 / 16 / 15 = 275000
//     rIICSTAT = (0x10);
//     rIICADD = SLAVE_ADDR;
// }

// void I2C_SEND(uint8_t data){
//     int i;
//     rIICSTAT = (0xF0);
//     if(rIICCON & 0x10){
//         rIICDS = data;
//         for(i = 0; i < 10; i++);
//         rIICCON = (0xAE);
//     }
// }

// void I2C_RECEIVE(){
//     rIICADD = ();

//     rIICSTAT = (0xB0);
//     rIICCON = (0xAE);

// }

void SPI_Init(){
    CH_CFG0 = 0x4F;
    CLK_CFG0 = 0x101;
}

void SPI_Transmit(uint8_t data){
    SPI_TX_DATA0 = data;
}

void Main(){
    Uart_Init(115200);
    timer0_init();
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
        rGPFDAT = 0x00;
        rGPGDAT = 0x00;
        rGPEDAT = 0xFF;
        // GPGDAT.GPIO_PIN_4 = LOW;
        // delay_ms(1000);
        // GPGDAT.GPIO_PIN_4 = HIGH;
        // delay_ms(1000);
        
    }
}

void __attribute__((interrupt("IRQ"))) RTC_TICK(void){
    ClearPending1(BIT_TICK);
    // Uart_Printf("%3d sec\t", ++tick);
}
