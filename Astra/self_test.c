#include "st7735.h"
#include "timer.h"
#include "calibration.h"
#include "pins.h"
#include "scanner.h"
#include "my_adc.h"	
#include "uv.h"
#include "as.h"
#include "st7735.h"
#include <stdio.h> 
#include <string.h>
#include "flash.h"
#include "usart.h"
#include "insert.h"
#include "motor.h"
#include "get_image.h"
#include "menu.h"
#include "motor.h"
#include "asrcv.h"
#include "self_test.h"
#include "rtc.h"


void WaitKeyMenu(void)
{
	int i=0;
	
	while(i<3) {
		delay_ms(10);
		if(BUT1==0) { i++; }
		else {i=0;}
	}
}


void WaitKeyEnter(void)
{
	int i=0;
	
	while(i<3) {
		delay_ms(10);
		if(BUT2==0) { i++; }
		else {i=0;}
	}
}

	
unsigned int SelfTest(void)
{
	int flag=0;
	
	while(1) {
		if(CheckMotorFirst()==0) { break; }
		flag=1;
		WaitOff();
		WaitAnyKey();	
		if(BUT1==0) {						// if "Menu/Next" was pressed, enter MENU
			ButtonsHandler();
			
			
			
ClearLcdMemory();	
LCD_ShowString(10,10,"ошибка 1");
LCD_Refresh();
while(1);
			
			
			NVIC_SystemReset();	
		}
	}

	while(1){	
		if(CheckPath()==0) { break; }
		flag=1;
		WaitOff();
		WaitAnyKey();
		if(BUT1==0) {						// if "Menu/Next" was pressed, enter MENU
			ButtonsHandler();
			
			
			
			ClearLcdMemory();	
LCD_ShowString(10,10,"ошибка 2");
LCD_Refresh();
while(1);
			
			
			NVIC_SystemReset();	
		}
		RejectNote();
	}

	while(1){		
		if(IsNoteHere()==0)	{ break; }
		flag=1;
		ClearLcdMemory();	
		LCD_ShowString(10,10,"ошибка:");
		LCD_ShowString(3,30,"датчик подачи банкнот");
		LCD_Refresh();
		WaitOff();
		WaitAnyKey();
		if(BUT1==0) {						// if "Menu/Next" was pressed, enter MENU
			ButtonsHandler();
			
			
			
			
			
			ClearLcdMemory();	
LCD_ShowString(10,10,"ошибка 3");
LCD_Refresh();
while(1);
			
			
			
			NVIC_SystemReset();	
		}
		CleanPath();
	}
	
	while(1){		
		if(EXIT_JAM_SENSOR==0)	{ break; }
		flag=1;
		ClearLcdMemory();	
		LCD_ShowString(10,10,"ошибка:");
		LCD_ShowString(3,30,"датчик замятия на");
		LCD_ShowString(3,50,"выходе валидатора");
		LCD_Refresh();
		WaitOff();
		WaitAnyKey();
		if(BUT1==0) {						// if "Menu/Next" was pressed, enter MENU
			ButtonsHandler();
			
			
			ClearLcdMemory();	
LCD_ShowString(10,10,"ошибка 4");
LCD_Refresh();
while(1);
			
			
			
			NVIC_SystemReset();	
		}
		CleanPath();
	}
	
	if (flag==1) {
		ClearLcdMemory();	
		LCD_ShowString(10,10,"стало лучше :)");
		LCD_Refresh();
		delay_ms(1000);
	}
	return 0;
}
		

void TestError(void)
{
	LcdDrawRectangle(0,159,96,128,red);
	LCD_SetFont(Arial_15x17,white);	
	LCD_ShowString(53,105,"ERROR");
	LCD_Refresh();	
	while(1);
}


void TestNext(void)
{
	LcdDrawRectangle(0,159,96,128,green);
	LCD_ShowString(30,105,"press ENTER");
	LCD_Refresh();	
	WaitKeyEnter();	
	WaitOff();
}


void TestButtons(void)
{
	SetClutzColor(CLUTZ_WHITE);
	ClearLcdMemory();	
	LcdDrawRectangle(0,159,0,32,white);
	LcdDrawRectangle(0,159,32,64,red);
	LcdDrawRectangle(0,159,64,96,green);
	LcdDrawRectangle(0,159,96,128,blue);
	LCD_ShowString(30,10,"FIRST START");
	LCD_Refresh();
	delay_ms(300);
	LCD_SetFont(Arial_15x17,white);	
	LCD_ShowString(35,42,"press MENU");
	LCD_Refresh();
	WaitKeyMenu();											// test MENU button

	LcdDrawRectangle(0,159,32,64,red);	
	LCD_Refresh();
	delay_ms(100);
	LCD_SetFont(Arial_15x17,black);	
	LCD_ShowString(35,74,"press ENTER");
	LCD_Refresh();
	WaitKeyEnter();											// test ENTER button
}


void TestMotor(void) 
{
	char buf[100];
	int cnt,encState;
	
	ClearLcdMemory();										// Motor test
	LCD_ShowString(10,10,"Motor test...");	
	LCD_Refresh();
	
	MOTOR_FORWARD;
	delay_ms(100);
	init_timer2(50000);									// 50000 is 1 second for Oleg:)
	Timer2_reset();
	cnt=0;
	while(GetTim2Value()<1000) {				// Forward direction for 1 second
		if(ENC_PIN==0) {
			encState=0;
		}
		else if(encState==0) {
			encState=1;
			cnt++;													// rpm counter
		}
	}
	MOTOR_IDLE;
	cnt = cnt*6;												// because the encoder disk has 10 holes, and 60 seconds in a minute 
	sprintf(buf,"forward %drpm",cnt);
	LCD_ShowString(10,25,buf);
	if(cnt>MAX_MOTOR_SPEED) {
		LCD_ShowString(10,40,"too fast");
		TestError();
	}
	else if(cnt<MIN_MOTOR_SPEED) {
		LCD_ShowString(10,40,"too slow");
		TestError();
	}
	else {
		LCD_ShowString(10,40,"Ok");
	}
	LCD_Refresh();
	delay_ms(300);

	MOTOR_REVERSE;
	delay_ms(100);
	Timer2_reset();
	cnt=0;
	while(GetTim2Value()<1000) {				// Forward direction for 1 second
		if(ENC_PIN==0) {
			encState=0;
		}
		else if(encState==0) {
			encState=1;
			cnt++;
		}
	}
	MOTOR_IDLE;
	cnt = cnt*6;												// because the encoder disk has 10 holes, and 60 seconds in a minute 
	sprintf(buf,"reverse %drpm",cnt);
	LCD_ShowString(10,55,buf);
	if(cnt>MAX_MOTOR_SPEED) {
		LCD_ShowString(10,70,"too fast");
		TestError();
	}
	else if(cnt<MIN_MOTOR_SPEED) {
		LCD_ShowString(10,70,"too slow");
		TestError();
	}
	else {
		LCD_ShowString(10,70,"Ok");
	}
	LCD_Refresh();
	TestNext();
}


void ASsensorMcuBurning(void)
{
	ClearLcdMemory();
	LCD_ShowString(10,10,"burning DD2...");
	LCD_Refresh();
	if(AsFirmwareUpload()==0){					// secondary MCU (AS sensor, DD2) is being burning
		LCD_ShowString(10,70,"burning is done");
	}
	else{
		LCD_ShowString(10,70,"burning is failed");
		TestError();	
	}
	LCD_Refresh();
	TestNext();
}


void TestTempSensor(void)
{
	
	TestNext();
}

void TestOpenClose(void)
{
	
	TestNext();
}


void TestExitJam(void)
{
	
	TestNext();
}


void TestBagLevel(void)
{
	
	TestNext();
}


void TestRTC(void)
{
	int sec, oldSec, cnt;
	char buf[50];
	
	cnt=0;
	RTC_GetTime();
	oldSec = time.Seconds;	
	init_timer2(50000);									
	Timer2_reset();
	while(GetTim2Value()<2500) {				// Forward direction for 2.5 second
		RTC_GetTime();
		sec = time.Seconds;
		if(sec!=oldSec) {
			cnt++;
			oldSec=sec;
		}	
		ClearLcdMemory();
		LCD_ShowString(10,10,"testing RTC...");
		sprintf(buf,"RTC seconds: %d",cnt);
		LCD_ShowString(10,30,buf);
		LCD_Refresh();
	}
	if((cnt<2)||(cnt>3)) { TestError(); }	// RTC error
	LCD_ShowString(10,50,"Ok");
	LCD_Refresh();
	TestNext();
}


// First Start: Mass production testing routine
void MassProductionTest(void)
{
	TestButtons();							// check LCD, Clutz and 2 buttons
	ASsensorMcuBurning();				// burn DD2
	TestMotor();								// check motor rpm
	
	//TestTempSensor();					// check temperature sensor
	//TestOpenClose();						// check path open/close state
	//TestExitJam();							// check exit jam sensor
	//TestBagLevel();							// check bag level
	TestRTC();									// check real time clock
	Calibration();							// calibration
	
	
	
	ClearLcdMemory();	
LCD_ShowString(10,10,"ошибка 5");
LCD_Refresh();
while(1);
	
	
	
	NVIC_SystemReset();
}

