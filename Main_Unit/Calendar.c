#include "libc.h"

int mdays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };   
char wdays[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

int leap_check(int year)
{
	if(year%4!=0) return 0;
	else if(year%100!=0) return 1;
	else if(year%400==0) return 1;
	else return 0;
}

int get_days(int year, int month)
{
	int days =0;
	days=mdays[month-1];
	if(leap_check(year) && month==2) ++days;
	return days;
}

int get_weekday(int year, int month, int date)
{
	int i, tot=0;

	for(i=1; i<year; i++){
		if(leap_check(i)) tot+=366;
		else tot+=365;
	}

	for(i=0; i<month-1; i++){
		tot+=mdays[i];
	}

	if(leap_check(year) && month>=3) tot++;

	tot += date;

	return tot%7;
}

void disp_calendar(int year, int month)
{
	int i=0;
	int cnt=0;
	int start;
	int days; 

	start = get_weekday(year, month, 1);
	days =get_days(year, month);

	Uart_Printf("\n============< %4d. %2d >============\n\n", year, month);
	Uart_Printf("------------------------------------\n");
	Uart_Printf("%5s%5s%5s%5s%5s%5s%5s\n", "Sun","Mon","Tue","Wed","Thu","Fri","Sat");
	Uart_Printf("------------------------------------\n");

	for(i=0; i<start; i++){
		Uart_Printf("%5c", ' ');
		cnt++;
	}

	for(i=1; i<=days; i++){
		Uart_Printf("%5d", i);
		cnt++;
		if(cnt%7==0) Uart_Printf("\n");
	}
	Uart_Printf("\n");
	Uart_Printf("------------------------------------\n");

}
