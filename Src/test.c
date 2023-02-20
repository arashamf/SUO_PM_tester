// test procedure
#include "test.h"
#include "st7735.h"
#include "timer.h"	
#include <stdio.h> 
#include <string.h>
#include "my_adc.h"
#include "usart.h"


void WaitInput24V(void)
{
	char buf[100];
	int v=0,cnt=0;
	
	while(cnt<10) {
		if((v>23950)&&(v<24050)) {cnt++;}
		v=avg.in_24v; //(IN_24V);
		ClearLcdMemory();	
		LCD_SetFont(Arial_15x17,black);	
		LCD_ShowString(10,0,"*** Self Test ***");
		LCD_ShowString(10,15,"Please set +24.000V");
		LCD_ShowString(10,30,"Current voltage is:");
		sprintf(buf,"%dmV", v);
		LCD_ShowString(10,45,buf);
		LCD_Refresh();
	}
}


void CheckInput24V(void)
{
	int v=0;
	
	while((v<23800)||(v>24200)) {
		v=avg.in_24v; 
	}
}


void WaitInvertor(void)
{
	int v=0;
	
	while(v<(avg.in_24v-200)) {
		v=avg.in_invertor; 
	}
}


void StopAndShowError(char* text)
{
	SET_ALL_OFF;
	LCD_TerminalAddString(text);
	LCD_TerminalAddString("*** Error. Test aborted");
	while(1) {
		LED_RED(1);
		delay_ms(50);
		LED_RED(0);
		delay_ms(50);
	}
}


void WaitBoardCalibration(void) {
	SetButtonOut();
	while(avg.in_invertor<23800) {
		SET_BUTTON(ON);
		delay_ms(100);
		SET_BUTTON(OFF);
		delay_ms(1000);
	}	
}


void MainTestRoutine(void)
{
	int tmp,i;
	char buf[50];
	
	LCD_TerminalAddString("Check input +24V");
	CheckInput24V();
	LCD_TerminalAddString("Plug +24V via 100R");
	LOAD_R_24V(ON);
	delay_ms(500);
	tmp = avg.out_24v;
	if(tmp<15000) {
		LCD_TerminalAddString("+24V Short circuit");
		sprintf(buf,"found: %05dmV",tmp);
		StopAndShowError(buf);
	}
	LCD_TerminalAddString("Plug +24V directly");
	LOAD_24V(ON);
	delay_ms(500);
	tmp = avg.out_24v;
	if(tmp<(avg.in_24v-400)) {
		LCD_TerminalAddString("+24V High current");
		sprintf(buf,"found: %dmV",tmp);
		StopAndShowError(buf);
	}
	
	delay_ms(500);
	LCD_TerminalAddString("Check +3.3V");
	tmp = avg.in_3_3v;
	if((tmp<3200)||(tmp>3400)) {
		sprintf(buf,"Wrong +3.3V (%dmV)",tmp);
		StopAndShowError(buf);
	}
	
	LCD_TerminalAddString("Pease burn the MCU");
	LCD_TerminalAddString("(wait for invertor)");
	WaitBoardCalibration();	
	
	LCD_TerminalAddString("Test invertor loaded");
	LOAD_INVERTOR(ON);
	delay_ms(300);
	tmp = avg.in_invertor;
	if(tmp<22000) {
		sprintf(buf,"Invertor low (%dmV)",tmp);
		StopAndShowError(buf);
	}
	LOAD_INVERTOR(OFF);
	
	LCD_TerminalAddString("Test Vbat not loaded");
	delay_ms(3000);
	tmp = avg.in_vbat;
	if((tmp<25000)||(tmp>28000)) {
		sprintf(buf,"Vbat N/L(%dmV)",tmp);
		StopAndShowError(buf);
	}
	
	LCD_TerminalAddString("Test Vbat loaded");
	LOAD_CHARGE(ON);
	delay_ms(500);
	tmp = avg.in_vbat;
	LOAD_CHARGE(OFF);
	if((tmp<20000)||(tmp>27000)) {
		sprintf(buf,"Vbat loaded(%dmV)",tmp);
		StopAndShowError(buf);
	}
	
	LCD_TerminalAddString("Test printer not loaded");
	tmp = avg.in_printer;
	if(tmp<23500) {
		sprintf(buf,"Printer N/L(%dmV)",tmp);
		StopAndShowError(buf);
	}
	
	LCD_TerminalAddString("Test printer loaded");
	LOAD_PRINTER(ON);
	delay_ms(500);
	tmp = avg.in_printer;
	LOAD_PRINTER(OFF);
	if(tmp<22000) {
		sprintf(buf,"Printer loaded(%dmV)",tmp);
		StopAndShowError(buf);
	}
	
	LCD_TerminalAddString("Test +12V not loaded");
	tmp = avg.in_12v;
	if((tmp<11500)||(tmp>12300)) {
		sprintf(buf,"+12V N/L (%dmV)",tmp);
		StopAndShowError(buf);
	}
	
	LCD_TerminalAddString("Test +12V loaded");
	LOAD_12V(ON);
	delay_ms(500);
	tmp = avg.in_12v;
	LOAD_12V(OFF);
	if((tmp<11300)||(tmp>12300)) {
		sprintf(buf,"+12V loaded(%dmV)",tmp);
		StopAndShowError(buf);
	}
	
	LCD_TerminalAddString("Test +5V not loaded");
	tmp = avg.in_5v;
	if((tmp<4800)||(tmp>5200)) {
		sprintf(buf,"+5V N/L(%dmV)",tmp);
		StopAndShowError(buf);
	}
	
	LCD_TerminalAddString("Test +5V loaded");
	LOAD_5V(ON);
	delay_ms(500);
	tmp = avg.in_5v;
	LOAD_5V(OFF);
	if((tmp<4700)||(tmp>5200)) {
		sprintf(buf,"+5V loaded(%dmV)",tmp);
		StopAndShowError(buf);
	}
	
	LCD_TerminalAddString("Test main switch");
	LOAD_INVERTOR(ON);
	delay_ms(10);
	LOAD_BATTERY(ON);
	delay_ms(10);
	LOAD_24V(OFF);
	LOAD_R_24V(OFF);
	delay_ms(100);
	//tmp = avg.in_invertor; // там блин бросок тока - он все портил. Ну его, возьмем мгновенные значения
	tmp=0;
	__disable_irq();				// в прерываниях тоже есть АЦП. Там что надо обязательно запрещать прерывания
	for(i=0;i<100;i++) {
		tmp += GetAdc_mV(IN_INVERTOR);
	}
	tmp = tmp/100;
	__enable_irq();
	
	LOAD_24V(ON);
	LOAD_R_24V(ON);
	delay_ms(10);
	LOAD_BATTERY(OFF);
	delay_ms(10);
	LOAD_INVERTOR(OFF);
	if(tmp<22000) {
		sprintf(buf,"main switch(%dmV)",tmp);
		StopAndShowError(buf);
	}
	
	LCD_TerminalAddString("Test UART");
	while(uartState!=2);
	uartBuf[24]=0;
	if(strcmp(uartBuf,"State=main")) {
		sprintf(buf,"<<%s>>",uartBuf);
		LCD_TerminalAddString(buf);
		StopAndShowError("Wrong UART data");
	}
	
	SET_ALL_OFF;
	LCD_TerminalAddString("*** Test OK ***");
	delay_ms(1500);
	LCD_CleanTerminal();
	LCD_TerminalAddString("*** Press START ***");
}

