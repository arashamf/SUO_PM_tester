//Insert/Double sensors

#include "pins.h"
#include "my_adc.h"	
#include "insert.h"
#include "calibration.h"
#include "st7735.h"
#include "get_image.h"
#include "timer.h"

unsigned int doubleCnt=0;  //for double sensor 
unsigned int doubleSum=0; 


int IsNoteHere(void)
{
	insert_t tmpIns;
	
	if(PATH_OPEN==0) {return 0;}
	__disable_irq();
	tmpIns = ReadInsertSensors();
	__enable_irq();
	if((tmpIns.ins0>NOTE_INSERT_LEVEL)||(tmpIns.ins1>NOTE_INSERT_LEVEL)) {return 1;}
	else {return 0;}
}


insert_t ReadInsertSensors(void)
{
	insert_t tmpIns;	
	int tmp0, tmp1;
	
	ADC1_ChannelSelect(ADC_INSERT_0);
	tmp0=ADC1_getValue();
	ADC1_ChannelSelect(ADC_INSERT_1);
	tmp1=ADC1_getValue();

	tmpIns.ins0=(tmp0*3300)/1023;
	tmpIns.ins1=(tmp1*3300)/1023;
	
	return tmpIns;
}


// read anti fishing sensor value
unsigned int ReadAFSensor(void)
{
	int tmp;
	
	ADC1_ChannelSelect(ADC_AF);
	tmp=ADC1_getValue();
	tmp=(tmp*3300)/1023;
	return tmp;
}


void SaveDoubleSensorInfo(void)
{
	insert_t tmpIns;
	tmpIns = ReadInsertSensors();
	
	if(tmpIns.ins0>NOTE_INSERT_LEVEL){
		doubleCnt++;
		doubleSum=doubleSum+tmpIns.ins0; 
	}
//	img.table.dd=(doubleSum/doubleCnt)>>4;
}


void ShowDoubleSensors(void)
{	
	char tmpBuf[40];

	insert_t tmpIns;

	ClearLcdMemory();		
		
	tmpIns = ReadInsertSensors();
	
	LCD_SetFont(Arial_15x17,black);
	sprintf(tmpBuf,"DD test, pwm: %d,%d%%", cal.table.ddDuty/10,cal.table.ddDuty%10);
	LCD_ShowString(5,0,tmpBuf); 
	
	sprintf(tmpBuf,"Sensor 0 = %d mV", tmpIns.ins0);
	LCD_ShowString(5,15,tmpBuf);

	sprintf(tmpBuf,"Sensor 1 = %d mV", tmpIns.ins1);
	LCD_ShowString(5,30,tmpBuf);
	
	sprintf(tmpBuf,"AF test, pwm: %d%%",cal.table.afDuty);
	LCD_ShowString(5,50,tmpBuf);
	
	sprintf(tmpBuf,"AF sensor = %d mV", ReadAFSensor());
	LCD_ShowString(5,65,tmpBuf);
	
	if(EXIT_JAM_SENSOR){LCD_ShowString(5,80,"JAM Sensor: close");}
	else{LCD_ShowString(5,85,"JAM Sensor: open");}
	
	if(PATH_OPEN){LCD_ShowString(5,100,"Path state: close");}
	else{LCD_ShowString(5,105,"Path state: open");}
	
	LCD_Refresh();	
}

