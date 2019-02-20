#include "libc.h"

void RTC_Clear(){
    rRTCCON = ~(0x1F);
}

void Set_ALM_Time(unsigned char hour, unsigned char min, unsigned char sec){
    Set_ALM_Hour(hour);
    Set_ALM_Minute(min);
    Set_ALM_Second(sec);
}

void Set_ALM_Hour(unsigned char hour){
    ALMHOUR.HOUR_10 = hour / 10;
    ALMHOUR.HOUR_1 = hour % 10;
}

void Set_ALM_Minute(unsigned char min){
    ALMMIN.MIN_10 = min / 10;
    ALMMIN.MIN_1 = min % 10;
}

void Set_ALM_Second(unsigned char sec){
    ALMSEC.SEC_10 = sec / 10;
    ALMSEC.SEC_1 = sec % 10;
}

void Set_BCD_Time(unsigned short year, unsigned char mon, unsigned char date, 
                    unsigned char hour, unsigned char min, unsigned char sec)
{
    Set_BCD_Year(year);
    Set_BCD_Month(mon);
    Set_BCD_Date(date);
    Set_BCD_Hour(hour);
    Set_BCD_Minute(min);
    Set_BCD_Second(sec);
}

void Set_BCD_Year(unsigned short year){
    year %= 100;
    Uart_Printf("real_year = %d\n", year);
    BCDYEAR.YEAR_10 = year / 10;
    BCDYEAR.YEAR_1 = year % 10;
}

void Set_BCD_Month(unsigned char mon){
    BCDMON.MON_10 = mon / 10;
    BCDMON.MON_1 = mon % 10;
}

void Set_BCD_Date(unsigned char date){
    BCDDATE.DATE_10 = date / 10;
    BCDDATE.DATE_1 = date % 10;
}

void Set_BCD_Hour(unsigned char hour){
    BCDHOUR.HOUR_10 = hour / 10;
    BCDHOUR.HOUR_1 = hour % 10;
}

void Set_BCD_Minute(unsigned char min){
    BCDMIN.MIN_10 = min / 10;
    BCDMIN.MIN_1 = min % 10;
}

void Set_BCD_Second(unsigned char sec){
    sec += 1;
    BCDSEC.SEC_10 = sec / 10;
    BCDSEC.SEC_1 = sec % 10;
}