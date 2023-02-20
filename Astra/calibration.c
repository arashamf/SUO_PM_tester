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
#include "motor.h"
#include "get_image.h"
#include "menu.h"
#include "motor.h"



union calUnion cal;


// whait until the button is pressed
// returns after the button is released
void WaitButton2(void)
{
	unsigned char i=0;
	
	while(BUT2);
	while(i<5) {
		delay_ms(10);
		i++;
		if(BUT2==0) {i=0;}
	}
}


void WaitButton1(void)
{
	unsigned char i=0;
	
	while(BUT1);
	while(i<5) {
		delay_ms(10);
		i++;
		if(BUT1==0) {i=0;}
	}
}


// Calibration main routine
void Calibration(void)
{	
	WaitButton2();	
	ClearLcdMemory();
	
	LCD_CalibrateWhiteSheet();
	WaitButton1();

	SetupDD();
	WaitButton1();
	
	cal.table.topRtime = SetupWhite(TOP_RED);
	SaveWhiteCal(cal.table.topRedW);
	cal.table.topGtime = SetupWhite(TOP_GREEN);
	SaveWhiteCal(cal.table.topGreenW);
	cal.table.topBtime = SetupWhite(TOP_BLUE);
	SaveWhiteCal(cal.table.topBlueW);
	cal.table.topItime = SetupWhite(TOP_IR);
	SaveWhiteCal(cal.table.topIRW);
	
	cal.table.botRtime = SetupWhite(BOTTOM_RED);
	SaveWhiteCal(cal.table.botRedW);
	cal.table.botGtime = SetupWhite(BOTTOM_GREEN);
	SaveWhiteCal(cal.table.botGreenW);
	cal.table.botBtime = SetupWhite(BOTTOM_BLUE);
	SaveWhiteCal(cal.table.botBlueW);
	cal.table.botItime = SetupWhite(BOTTOM_IR);
	SaveWhiteCal(cal.table.botIRW);
	
	LCD_CalibrateBlackSheet();
	WaitButton1();
	
	SetupBlack(TOP_RED,cal.table.topRtime);
	SaveBlackCal(cal.table.topRedB, cal.table.topRedW);
	SetupBlack(TOP_GREEN,cal.table.topGtime);
	SaveBlackCal(cal.table.topGreenB, cal.table.topGreenW);
	SetupBlack(TOP_BLUE,cal.table.topBtime);
	SaveBlackCal(cal.table.topBlueB, cal.table.topBlueW);
	SetupBlack(TOP_IR,cal.table.topItime);
	SaveBlackCal(cal.table.topIRB, cal.table.topIRW);
	
	SetupBlack(BOTTOM_RED,cal.table.botRtime);
	SaveBlackCal(cal.table.botRedB, cal.table.botRedW);
	SetupBlack(BOTTOM_GREEN,cal.table.botGtime);
	SaveBlackCal(cal.table.botGreenB, cal.table.botGreenW);
	SetupBlack(BOTTOM_BLUE,cal.table.botBtime);
	SaveBlackCal(cal.table.botBlueB, cal.table.botBlueW);
	SetupBlack(BOTTOM_IR,cal.table.botItime);
	SaveBlackCal(cal.table.botIRB, cal.table.botIRW);
	
	LCD_CalibrateYellowSheet();
	WaitButton1();
	
	cal.table.uvTime = SetupUvW();
	cal.table.uvW = uvResult;
	
	LCD_ShowString(10,40,"remove yellow sheet");
	LCD_ShowString(10,60,"and press Next");
	LCD_Refresh();
	WaitButton1();
	
	SetupUvB(cal.table.uvTime);
	cal.table.uvB = uvResult;
	
	cal.table.asTime = 100;
	
	SetupAF();
	WaitButton1();
	
	ClearLcdMemory();
	LCD_ShowString(10,40,"Calibration is done!");
	LCD_Refresh();
	delay_ms(1000);
		
	SaveCalValues(cal.buf,CAL_LENGTH);
}


void CheckMg(void) 
{
	int cnt=0,x,y,tmp[6],n,flag;
	char txtBuf[4];
	int mgBuf[6];
	
	mgBuf[0]=mgBuf[1]=mgBuf[2]=mgBuf[3]=mgBuf[4]=mgBuf[5]=0;
	
	while(cnt<4) {
		ClearLcdMemory();
		LcdDrawRectangle(0,34,0,17,red);	
		LCD_ShowString(5,0,"MG"); 
		LCD_ShowString(45,0,"USD1");
 		EnableIdleScan();
		txtBuf[0] = '<';		
		txtBuf[1] = 'A' + cnt;
		txtBuf[2] = '>';
		txtBuf[3] = 0;
		LCD_ShowString(90,0,txtBuf);
		
		if(IsNoteHere()) { 									
			GetImage();
			while(getImgState<5);
			AcceptNote();
			EnableIdleScan();	
			tmp[0]=tmp[1]=tmp[2]=tmp[3]=tmp[4]=tmp[5]=n=0;
			for(x=10;x<150;x++) {
				tmp[0]+=img.table.mg[x][0];
				tmp[1]+=img.table.mg[x][1];
				tmp[2]+=img.table.mg[x][2];
				tmp[3]+=img.table.mg[x][3];
				tmp[4]+=img.table.mg[x][4];
				tmp[5]+=img.table.mg[x][5];
				n++;
			}
			for(y=0;y<6;y++) {		// calculate average value
				tmp[y] = tmp[y]/n;		
				mgBuf[y] = (mgBuf[y]*cnt + tmp[y]) / (cnt+1);	
			}
			cnt++;	
		}
		LcdDrawMgGraph(mgBuf, MG_MIN_LEVEL, MG_MAX_LEVEL);
		LCD_Refresh();
	}
	DisableIdleScan();
	
	flag=0;
	for(y=0;y<6;y++) {
		if((mgBuf[y]<MG_MIN_LEVEL) || (mgBuf[y]>MG_MAX_LEVEL)) {flag=1;}
	}
	
	if(flag==0) { LCD_ShowString(75,15,"Ok"); }
	else {LCD_ShowString(10,15,"ошибка МГ датчика");}
	LCD_ShowString(30,30,"press Next");
	LCD_Refresh();
	WaitButton1();	
}


void SetupDD (void)
{
	unsigned int pre_result=0,delta=0,pre_delta=0;
	int i=0;
	char tmpBuf[50];
	insert_t tmpIns;
	
	for(i=0;i<1000;i++) {
		LED_PWM(i);
		delay_ms(5);
		tmpIns = ReadInsertSensors();
		
		if(!(i%10))	{
			ClearLcdMemory();
			LCD_ShowString(0,0,"Double note detection"); 
			sprintf(tmpBuf,"PWM = %d,%d%%",i/10,i%10);
			LCD_ShowString(0,20,tmpBuf);
			sprintf(tmpBuf,"DD value= %d mV",tmpIns.ins0);
			LCD_ShowString(0,40,tmpBuf);
			LCD_Refresh();
		}
		
		if(tmpIns.ins0<=DD_TARGET_LEVEL) {
									
			delta = DD_TARGET_LEVEL - tmpIns.ins0; 
			
			if(pre_result < DD_TARGET_LEVEL) { delta = DD_TARGET_LEVEL - pre_result; }
			else { pre_delta = pre_result - DD_TARGET_LEVEL; }
			
			if(delta<pre_delta){cal.table.ddDuty=i;}
			else{cal.table.ddDuty=i-1;}
			
			ClearLcdMemory();
			LCD_ShowString(0,0,"Double note detection"); 
			sprintf(tmpBuf,"PWM = %d,%d%%",cal.table.ddDuty/10,cal.table.ddDuty%10);
			LCD_ShowString(0,20,tmpBuf);
			sprintf(tmpBuf,"DD value= %d mV",tmpIns.ins0);
			LCD_ShowString(0,40,tmpBuf);
			LCD_ShowString(0,60,"OK. Press Next");
			LCD_Refresh();
			
			break;
		}
		pre_result=tmpIns.ins0;
	}
}


void SetupAF (void)
{
	unsigned int i, result,success=0;
	char tmpBuf[50];
	
	for(i=0;i<100;i++) {
		AF_PWM(i);
		delay_ms(5);
		result=ReadAFSensor();
		
		if(result<AF_TARGET_LEVEL){	
			success=1;
			i=i+i/10;
			cal.table.afDuty=i;
		}
		
		ClearLcdMemory();		
		LCD_ShowString(0,0,"Anti fishing"); 
		sprintf(tmpBuf,"AF PWM = %d%%",i);
		LCD_ShowString(0,20,tmpBuf);
		sprintf(tmpBuf,"AF value= %d mV",result);
		LCD_ShowString(0,40,tmpBuf);
		LCD_Refresh();
		
		if(success) { break;}
	}
	
	if(i<2) {
		LCD_ShowString(0,60,"error: high AF");
	}
	else {
		if(success==0) {
			LCD_ShowString(0,60,"error: low AF");
		}
		else {
			LCD_ShowString(0,60,"OK. Press Next");
		}
	}
	LCD_Refresh();
}


// setup scanner's optimal exposure time for the light source selected
unsigned int SetupWhite(unsigned char lightSource) 
{	
	unsigned int tmpChargeTime, avg, delta, prevDelta, result;
	unsigned char i,j;
	unsigned int tmpResultBuf[22];
	char tmpBuf[50];

	tmpChargeTime = 1;
	delta = 0;
	prevDelta = 0xffff;
	
	for(i=0;i<10;i++) {	// dummy readings to stabilize the scanner
		if(lightSource<BOTTOM_RED) { ScanTop(lightSource,tmpChargeTime); }
		else { ScanBottom(lightSource,tmpChargeTime); }
		ShowScan(lightSource,tmpChargeTime);
	}
	
	while(tmpChargeTime++<MAX_EXPOSURE_TIME) {	// try various exposure times in order to get optimal result
		if(lightSource<BOTTOM_RED) { ScanTop(lightSource,tmpChargeTime); }
		else { ScanBottom(lightSource,tmpChargeTime); }
		ShowScan(lightSource,tmpChargeTime);
		avg = GetAvg();
		if(avg < AVG_SCAN_LEVEL) { delta = AVG_SCAN_LEVEL - avg; }
		else { delta = avg - AVG_SCAN_LEVEL; }
		if(delta > prevDelta) {		// we just found the closest value
			tmpChargeTime--;
			if(lightSource<BOTTOM_RED) { ScanTop(lightSource,tmpChargeTime); }
			else { ScanBottom(lightSource,tmpChargeTime); }
			ShowScan(lightSource,tmpChargeTime);
			avg = GetAvg();
			break;
		}
		prevDelta = delta;
	}
	
	if(avg<(AVG_SCAN_LEVEL-AVG_SCAN_TOLERANCE)) {		// calibration failed - average ADC level is too low
		LCD_ShowString(0,18,"too low level");
		LCD_Refresh();
		WaitButton1();
		ClearLcdMemory();
		result = tmpChargeTime;
	}
	else {											
		if(avg>(AVG_SCAN_LEVEL+AVG_SCAN_TOLERANCE)) {	// calibration failed - average ADC level is too high
			sprintf(tmpBuf,"AVG=%d",avg);
			LCD_ShowString(0,36,tmpBuf);
			LCD_ShowString(0,18,"too high level");
			LCD_Refresh();
			WaitButton1();
			ClearLcdMemory();
			result = tmpChargeTime;
		}
		else { 																				// calibration is successful
			for(j=0;j<22;j++) {tmpResultBuf[j] = 0;}
			
			for(i=0;i<10;i++) {	// scan 10 times for better averaging
				if(lightSource<BOTTOM_RED) { ScanTop(lightSource,tmpChargeTime); }
				else { ScanBottom(lightSource,tmpChargeTime); }
				ShowScan(lightSource,tmpChargeTime);
				for(j=0;j<22;j++) {tmpResultBuf[j] += resultBuf[j];}
			}
			for(j=0;j<22;j++) {resultBuf[j] = tmpResultBuf[j]/10;}
				
			LCD_ShowString(0,18,"OK");
			LCD_Refresh();
			//WaitButton1();
			ClearLcdMemory();
			delay_ms(500);
			result = tmpChargeTime;
		}
	}
	return result;
}


// setup the scanner's signals for the black calibration sheet for the light source selected
void SetupBlack(unsigned char lightSource, unsigned int chargeTime) 
{	
	unsigned char i,j;	
	unsigned int tmpResultBuf[22];
	
	for(i=0;i<5;i++) {	// dummy readings to stabilize the scanner
		if(lightSource<BOTTOM_RED) { ScanTop(lightSource,chargeTime); }
		else { ScanBottom(lightSource,chargeTime); }
		ShowScan(lightSource,chargeTime);
	}
		
	for(j=0;j<22;j++) {tmpResultBuf[j] = 0;}
	
	for(i=0;i<10;i++) {
		if(lightSource<BOTTOM_RED) { ScanTop(lightSource,chargeTime); }
		else { ScanBottom(lightSource,chargeTime); }
		ShowScan(lightSource,chargeTime);	
		for(j=0;j<22;j++) {tmpResultBuf[j] += resultBuf[j];}
	}
	for(j=0;j<22;j++) {resultBuf[j] = tmpResultBuf[j]/10;}
	
	LCD_ShowString(0,18,"OK");
	LCD_Refresh();
	//WaitButton1();
	ClearLcdMemory();
	delay_ms(500);
}


// save scanners's calibration factors for BLACK sheet
void SaveBlackCal(unsigned int *bufBlack, unsigned int *bufWhite) 
{
	unsigned char i;
	
	for(i=0;i<22;i++) {
		bufBlack[i] = resultBuf[i];
		if((bufWhite[i] - bufBlack[i])!=0) {
			bufWhite[i] = 65536 / (bufWhite[i] - bufBlack[i]);
		}
	}
}


// save scaner's calibration factors for WHITE sheet
void SaveWhiteCal(unsigned int *buf) 
{
	unsigned char i;
	
	for(i=0;i<22;i++) {
		buf[i] = resultBuf[i];		// save white level
	}
}


// apply calibration factors to the raw data of the scanners
void ApplyCal(unsigned int *bufWhite, unsigned int *bufBlack) 
{
	unsigned char i;
	signed int tmp;
	
	for(i=0;i<22;i++) {
		tmp = resultBuf[i] - bufBlack[i];
		if(tmp<0) { tmp = 0;}
		tmp = tmp*bufWhite[i];
		tmp = (tmp>>8);	// divide by 255		
		if(tmp>255) {tmp = 255;}
		resultBuf[i] = tmp;
	}
}


// setup white level for UV sensor (yellow color "SBM shading UV" sheet)
unsigned int SetupUvW(void)
{
	unsigned int i, tmpChargeTime=0, optimChargeTime=1, delta, minDelta=0xffff;
	
	for (i=0;i<100;i++) {
		GetUv(1);
		delay_ms(1);
	}
	
	while(tmpChargeTime++<50) {
		GetAvgUv(tmpChargeTime);
		ShowUv(tmpChargeTime);
		if(uvResult>IDEAL_UV) {delta = uvResult - IDEAL_UV;}
		else {delta = IDEAL_UV - uvResult;}
		if(delta<minDelta) {
			optimChargeTime = tmpChargeTime;
			minDelta = delta;
		}
	}
	GetAvgUv(optimChargeTime);
	ShowUv(optimChargeTime);
	if(uvResult<(IDEAL_UV-TOLERANCE_UV)) {
		LCD_ShowString(0,18,"too low level");
		LCD_Refresh();
		WaitButton1();
		ClearLcdMemory();
		return 1;
	}
	if(uvResult>(IDEAL_UV+TOLERANCE_UV)) {
		LCD_ShowString(0,18,"too high level");
		LCD_Refresh();
		WaitButton1();
		ClearLcdMemory();
		return 1;
	}
	if((uv1>MAX_UV)||(uv2>MAX_UV)) {
		LCD_ShowString(0,18,"UV sensor is failed");
		LCD_Refresh();
		WaitButton2();
		ClearLcdMemory();;
		return 1;
	}
	LCD_ShowString(0,18,"OK. Press Next");
	LCD_Refresh();
	WaitButton1();
	ClearLcdMemory();
	return optimChargeTime;
}


// setup black level for UV sensor
void SetupUvB(unsigned int tmpChargeTime)
{
	GetAvgUv(tmpChargeTime);	// dummy reading
	GetAvgUv(tmpChargeTime);
	ShowUv(tmpChargeTime);
	
	LCD_ShowString(0,18,"OK. Press Next");
	LCD_Refresh();
	WaitButton1();
	ClearLcdMemory();
}


// shows the scan result on the TFT display
void ShowUv(unsigned int chargeTime)
{	
	char tmpBuf[40];

	ClearLcdMemory();		
	LCD_SetFont(Arial_15x17,black);
	LCD_ShowString(5,0,"UV"); 
	
	sprintf(tmpBuf,"chrg=%dus,UV=%d", chargeTime, uvResult);
	LCD_ShowString(37,0,tmpBuf);
	
	LcdDrawUvGraph(uv1,uvResult,uv2);	
	LCD_Refresh();	
}


// Test mode to review scanner's and UV sensor data
void ShowSensors(void) 
{
	unsigned int color=0;
	
	while(1) {
		if(!BUT1) {
			color++;
			while(!BUT1);
			if(color>12) { color = 0;}
		}
		switch (color) {
			case 0:		// top scanner test
				ScanTop(TOP_RED,cal.table.topRtime);						// 100...150us
				ApplyCal(cal.table.topRedW,cal.table.topRedB);	// 3us
				ShowScan(TOP_RED,cal.table.topRtime);						// 90ms
				break;
			case 1:
				ScanTop(TOP_GREEN,cal.table.topGtime);
				ApplyCal(cal.table.topGreenW,cal.table.topGreenB);
				ShowScan(TOP_GREEN,cal.table.topGtime);
				break;
			case 2:
				ScanTop(TOP_BLUE,cal.table.topBtime);
				ApplyCal(cal.table.topBlueW,cal.table.topBlueB);
				ShowScan(TOP_BLUE,cal.table.topBtime);
				break;
			case 3:
				ScanTop(TOP_IR,cal.table.topItime);
				ApplyCal(cal.table.topIRW,cal.table.topIRB);
				ShowScan(TOP_IR,cal.table.topItime);
				break;
			
			case 4:		// bottom scanner test
				ScanBottom(BOTTOM_RED,cal.table.botRtime);	
				ApplyCal(cal.table.botRedW,cal.table.botRedB);	
				ShowScan(BOTTOM_RED,cal.table.botRtime);						
				break;
			case 5:
				ScanBottom(BOTTOM_GREEN,cal.table.botGtime);
				ApplyCal(cal.table.botGreenW,cal.table.botGreenB);
				ShowScan(BOTTOM_GREEN,cal.table.botGtime);
				break;
			case 6:
				ScanBottom(BOTTOM_BLUE,cal.table.botBtime);
				ApplyCal(cal.table.botBlueW,cal.table.botBlueB);
				ShowScan(BOTTOM_BLUE,cal.table.botBtime);
				break;
			case 7:
				ScanBottom(BOTTOM_IR,cal.table.botItime);
				ApplyCal(cal.table.botIRW,cal.table.botIRB);
				ShowScan(BOTTOM_IR,cal.table.botItime);
				break;
			
			case 8:		// UV test
				GetAvgUv(cal.table.uvTime);
				uvBackground = cal.table.uvB;
				uvResult = ApplyUvCal();
				ShowUv(cal.table.uvTime);
				break;
			
			case 9:		// AS test
				AsSendPulse(cal.table.asTime);
				ShowAs(cal.table.asTime);
				break;
			
			case 10:   // Insert (DD) sensor test
				ShowDoubleSensors();			
				break;
			
			case 11:	 // RTC: show internal time 
				RTC_ShowTime();
				break;
				
			case 12:
				return;
		}
	}
}


