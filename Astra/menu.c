
#include "timer.h"
#include "pins.h"
#include "scanner.h"
#include "my_adc.h"	
#include "uv.h"
#include "as.h"
#include "calibration.h"
#include "st7735.h"
#include <stdio.h> 
#include <string.h>
#include "flash.h"
#include "usart.h"
#include "insert.h"
#include "rtc.h"
#include "usb_cdc.h"
#include "menu.h"
#include "protocol.h"


int saveImageOnSdCard=1;

const char menu0[] = 	{"Тест"};
const char menu1[] = 	{"Калибровка"};
const char menu2[] = 	{"Настройки"};
const char menu3[] = 	{"Об устройстве"};
const char menu4[] = 	{"Выход"};
const char * menuItem[] = {menu0,menu1,menu2,menu3,menu4};


void LcdShowMenu(int state) 
{
	int i;
	
	lcdDrawHLine(0,159,0,black);
	lcdDrawHLine(0,159,25+1,grey);
	lcdDrawHLine(0,159,50+1,grey);
	lcdDrawHLine(0,159,75+1,grey);
	lcdDrawHLine(0,159,100+1,grey);
	lcdDrawHLine(0,159,125+1,black);
	lcdDrawHLine(0,159,127,black);
	
	for(i=0;i<5;i++) {
		CheckUartDMA();	
		if(i==state) {		// highlighted item
			LcdDrawRectangle(0,160,i*25+1,(i+1)*25+1,red);
			LCD_SetFont(Arial_22x23,white);	
			LCD_ShowString(6,3+i*25,(char*)menuItem[i]); 
		}
		else {						// regular item
			LCD_SetFont(Arial_15x17,black);	
			LCD_ShowString(8,7+i*25,(char*)menuItem[i]); 
		}			
	}
}


void LcdShowExit(int state, int progress)
{
	LcdShowMenu(state);
	LcdDrawRectangle(28,139,28,100,black);
	LcdDrawRectangle(30,137,30,98,white);
	LCD_SetFont(Arial_22x23,black);	
	LCD_ShowString(45,40,"ВЫХОД"); 
	LcdDrawRectangle(33,33+progress,70,75,green);
	LCD_Refresh();
}


void WaitBut1off(void)
{
	int i=0; 
	
	while(i<5) {
		CheckUartDMA();	
		delay_ms(10);
		if(BUT1==0) { i=0; }
		else {i++;}
	}
}


void WaitBut2off(void)
{
	int i=0; 
	
	while(i<5) {
		CheckUartDMA();	
		delay_ms(10);
		if(BUT2==0) { i=0; }
		else {i++;}
	}
}


void WaitOff(void)
{
	int i=0;
	
	while(i<5) {
		CheckUartDMA();	
		delay_ms(10);
		if((BUT1==0)||(BUT2==0)) { i=0; }
		else {i++;}
	}
}


void WaitAnyKey(void)
{
	int i=0;
	
	while(i<5) {
		CheckUartDMA();	
		delay_ms(10);
		if((BUT1==0)||(BUT2==0)) { i++; }
		else {i=0;}
	}
}


void LcdShowVersion(void) 
{
	ClearLcdMemory();
	LCD_SetFont(Arial_36x37,black);	
	LCD_ShowString(25,40,"ASTRA"); 
	LCD_SetFont(Arial_15x17,black);	
	LCD_ShowString(18,10,"валидатор банкнот"); 
	LCD_ShowString(22,80,"версия ПО: ");
//	LCD_ShowString(110,80,OFFICIAL_VERSION);
	LCD_SetFont(Arial_15x17,red);	
	LCD_ShowString(32,110,"www.docash.de");
	LCD_Refresh();
	WaitOff();
}


void LcdShowSecretVersion(void) 
{
	int i=0,s=0;
	char tmpBuf[50];
	
	LcdShowVersion();
	while(1) {
		delay_ms(10);
		if((BUT1!=0)&&(BUT2!=0)) {i=0; s=0;}
		if((BUT1==0)||(BUT2==0)) {i++;}
		if((BUT1==0)&&(BUT2==0)) {i=0; s++;}
		if(i>3) 	{return;}
		if(s>300) {break;}
	}
	ClearLcdMemory();

	LCD_ShowString(30,20,"версия сборки: ");
//	LCD_ShowString(60,40,SECRET_VERSION);
	
	LCD_ShowString(25,70,"счетчик пробега: ");
	sprintf(tmpBuf,"%07d банкнот",ReadNoteCounter());
	LCD_ShowString(25,90,tmpBuf);
	
	LCD_Refresh();
	WaitOff();
	WaitAnyKey();
}


void LcdShowIntro(void) 
{
	int i;
	
	ClearLcdMemory();
	LCD_SetFont(Arial_22x23,black);	
	LCD_ShowString(32,23,"ВНЕСИТЕ"); 
	LCD_ShowString(27,50,"БАНКНОТУ");
	for(i=0;i<26;i++) {												// red triangle
		lcdDrawHLine(30+i*2,130-i*2,82+i,red);
	}
	LCD_Refresh();
}


void LcdShowSetup(int focus)
{
	ClearLcdMemory();
	LCD_SetFont(Arial_15x17,black);	
	LCD_ShowString(50,6,"Настройки"); 
	lcdDrawHLine(20,139,25,grey);
	LCD_ShowString(5,40,"Запись образов:"); 
	if(focus==0) { 
		LCD_SetFont(Arial_22x23,white); 
		LcdDrawRectangle(115,158,40,60,red);
	}
	else { 
		LCD_SetFont(Arial_15x17,black); 
	}
	if(saveImageOnSdCard==0) { LCD_ShowString(117,40,"OFF"); }
	else { LCD_ShowString(117,40,"ON"); }
	
	if(focus==1) { 
		LCD_SetFont(Arial_22x23,white); 
		LcdDrawRectangle(2,157,100,122,red);
		LCD_ShowString(8,100,"Сохр. и выйти"); 
	}
	else { 
		LCD_SetFont(Arial_15x17,black); 
		LCD_ShowString(30,100,"Сохр. и выйти"); 
	}
	LCD_Refresh();
}


void MenuSetup(void)
{
	int focus=0;
	
	while(1) {
		CheckUartDMA();	
		LcdShowSetup(focus);
		WaitOff();
		WaitAnyKey();
		if(BUT1==0) {
			focus++;
			if(focus>=2) {focus=0;}
		}
		if(BUT2==0) {
			if(focus==0) {
				if(saveImageOnSdCard==0) {saveImageOnSdCard=1;}
				else {saveImageOnSdCard=0;}
			}
			if(focus==1) {
				return;
			}
		}		
	}
}


#define MENU_RETURN 0
#define MENU_EXIT 	1

int HandleBut2 (int state)
{
	CheckUartDMA();	
	switch (state) {
		case 0: 		// test mode
			ShowSensors();
			return MENU_RETURN;
		case 1:			// calibration mode
			Calibration(); 
		
		
		
		ClearLcdMemory();	
LCD_ShowString(10,10,"ошибка 7");
LCD_Refresh();
while(1);
		
		
		
			NVIC_SystemReset();
		case 2:			// "setup"
			MenuSetup();
			WaitOff();
			return MENU_RETURN;
		case 3:			// "about"
			LcdShowSecretVersion();
			WaitAnyKey();
			WaitOff();
			return MENU_RETURN;
		case 4:			// exit
			LcdShowIntro();
			WaitOff();
			return MENU_EXIT;
	}
	return MENU_RETURN;
}


// enter Menu if 2 buttons are pressed
void ButtonsHandler(void)
{
	unsigned int res,state=0;
	
	if(BUT1!=0) { return; }							// MENU/ESC is not pressed - return immediately
	
	currentAstraState = ASTRA_STATE_USER_MENU;
	
	DisableIdleScan();									// disable idle scan mode
	SetClutzColor(CLUTZ_BLACK);
	
	ClearLcdMemory();
	LcdShowMenu(state);
	LCD_Refresh();
	WaitBut1off();
	while(1) {
		CheckUartDMA();	
		if(BUT1==0) {											// next MENU item
			state++;
			if(state>=5) {state=0;}
			ClearLcdMemory();
			LcdShowMenu(state);
			LCD_Refresh();
			WaitBut1off();
		}
		if(BUT2==0) { 										// item is selected
			res = HandleBut2(state); 				// handle it
			if(res==MENU_EXIT) { break;}		// exit the menu
			if(res==MENU_RETURN) { 					// return to menu
				ClearLcdMemory();
				LcdShowMenu(state);
				LCD_Refresh();
			}
		}
	}
	LCD_SetFont(Arial_15x17,black);	
	EnableIdleScan();										// enter idle scan mode 
	SetClutzColor(CLUTZ_GREEN);
	currentAstraState = ASTRA_STATE_READY;
}	


