// IR/Visible light scanner

#include "st7735.h"
#include "scanner.h"	
#include "timer.h"	
#include <stdio.h> 
#include <string.h>
#include "my_adc.h"
#include "flash.h"
#include "uv.h"
#include "calibration.h"

#define delaySCK delay(100);

static unsigned int ADCbuf1[25],ADCbuf2[25];
unsigned int resultBuf[25];


// scanns 1 string of 22 phototransistors on TOP side
// scanning consists of the first scan, exposure light, second scan and subtraction scan2-scan1.
void ScanTop(unsigned char lightSource, unsigned int chargeTime)
{
}


// scanns 1 string of 22 phototransistors on BOTTOM side
// scanning consists of the first scan, exposure light, second scan and subtraction scan2-scan1.
void ScanBottom(unsigned char lightSource, unsigned int chargeTime)
{
	}


// shows the scan result on the TFT display
void ShowScan(unsigned char lightSource, unsigned int chargeTime)
{	
	char tmpBuf[40];

	ClearLcdMemory();		//700us	
	LCD_SetFont(Arial_15x17,white);
	
	if(lightSource==TOP_RED) {
		LcdDrawRectangle(0,34,0,17,red);	
		LCD_ShowString(0,0,"TopR"); 
	}	
	if(lightSource==TOP_GREEN) {	
		LcdDrawRectangle(0,34,0,17,green);
		LCD_ShowString(0,0,"TopG");
	}	
	if(lightSource==TOP_BLUE) {	
		LcdDrawRectangle(0,34,0,17,blue);
		LCD_ShowString(0,0,"TopB"); 
	}	
	if(lightSource==TOP_IR) {	
		LcdDrawRectangle(0,35,0,17,black);
		LCD_ShowString(0,0,"TopIR"); 
	}	
	
	if(lightSource==BOTTOM_RED) {
		LcdDrawRectangle(0,34,0,17,red);	
		LCD_ShowString(0,0,"BotR"); 
	}	
	if(lightSource==BOTTOM_GREEN) {	
		LcdDrawRectangle(0,34,0,17,green);
		LCD_ShowString(0,0,"BotG");
	}	
	if(lightSource==BOTTOM_BLUE) {	
		LcdDrawRectangle(0,34,0,17,blue);
		LCD_ShowString(0,0,"BotB"); 
	}	
	if(lightSource==BOTTOM_IR) {	
		LcdDrawRectangle(0,35,0,17,black);
		LCD_ShowString(0,0,"BotIR"); 
	}	
	
	LCD_SetFont(Arial_15x17,black);	
	sprintf(tmpBuf,"chrg=%dus,avg=%d", chargeTime, GetAvg());
	
	LCD_ShowString(37,0,tmpBuf);
	
	//LcdDrawGraphSimple(resultBuf, BLUE);
	LcdDrawGraph(ADCbuf1,resultBuf,ADCbuf2);	
	LCD_Refresh();		//100ms
}


// calculate the averaged value of the buffer
unsigned int GetAvg(void) 
{
	unsigned int avg=0, i;
	
	for(i=0;i<22;i++) {		
		avg+=resultBuf[i];
	}
	avg = avg/22;
	return avg;
}
