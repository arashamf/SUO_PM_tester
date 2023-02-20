// AntiStokes data processing

#include "timer.h"
#include "pins.h"
#include "scanner.h"
#include "my_adc.h"	
#include "as.h"
#include "calibration.h"
#include "st7735.h"
#include "usart.h"

unsigned int asResult1,asResult2;


// light the IR LEDs
void AsSendPulse(unsigned int chargeTime)
{
}


// extract the AS data out of UART buffer
void GetAs(void)
{
	unsigned char pattern1,pattern2;

	if((!(uartBuf[1]&0x80))&&(uartBuf[2]&0x80)) {					// check for valid data (1st and 2nd channels)
		asResult1 = asResult2 = 0;
		pattern1 = (uartBuf[1]&0x70)>>4;
		if(pattern1==2) {asResult1 =  (uartBuf[1]&0x0f)*16;}	// max AS signal delay (OpAmp's overload release time): 15*32us = 480us => 15*16 = 240units
		pattern2 = (uartBuf[2]&0x70)>>4;
		if(pattern2==2) {asResult2 =  (uartBuf[2]&0x0f)*16;}
	}
	uartCnt=0;
}


// shows the AS result on the TFT display
void ShowAs(unsigned int AsChargeTime)
{
	char tmpBuf[40];
	
	uartCnt = 0;
	
	while (uartCnt!=3);

	ClearLcdMemory();		
	LCD_SetFont(Arial_15x17,black);
	LCD_ShowString(5,0,"AS"); 
	/*
	sprintf(tmpBuf,"pattern1=%d,delay=%dms",((uartBuf[1]&0x70)>>4),(uartBuf[1]&0x0f)*32);
	LCD_ShowString(0,40,tmpBuf);
	sprintf(tmpBuf,"pattern2=%d,delay=%dms",((uartBuf[2]&0x70)>>4),(uartBuf[2]&0x0f)*32);
	LCD_ShowString(0,70,tmpBuf);
	*/
	GetAs();
	
	sprintf(tmpBuf,"chrg=%dus", AsChargeTime);
	LCD_ShowString(37,0,tmpBuf);
	sprintf(tmpBuf,"AS1=%d", asResult1);
	LCD_ShowString(20,20,tmpBuf);
	sprintf(tmpBuf,"AS2=%d", asResult2);
	LCD_ShowString(95,20,tmpBuf);
	
	LcdDrawASGraph(asResult1,asResult2);	
	LCD_Refresh();	
}
