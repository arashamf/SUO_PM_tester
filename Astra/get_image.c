// Image capturing
// Both scanners - by the encoder's interruption
// MG - by the timer5, each 100us
// UV - by the encoder's interruption
// AS - every 4th encoder interruption or when the UART received the AS data
// Idle scan (no banknote inside) is done by the timer4 interruption

#include "get_image.h"
#include "timer.h"
#include "pins.h"
#include "scanner.h"
#include "my_adc.h"	
#include "magnet.h"
#include "uv.h"
#include "string.h"
#include "calibration.h"
#include "usart.h"
#include "as.h"
#include "motor.h"
#include "insert.h"
#include "st7735.h"
#include "jam_sensors.h"
#include "protocol.h"

unsigned int encCnt,imgCnt,edgeCnt;
volatile unsigned int getImgState=1;
static unsigned char isNote;
volatile unsigned int minIdleTime;		// minimal time between the image capturing interrupt handlers (time when the CPU is free)
volatile unsigned int idleTime;
volatile unsigned int suspectAF;			// =1: the note is longer than it should be. Suspect fishing.

volatile unsigned int error_enc=0;
volatile unsigned int vl=0;

// start the image capturing routine:
// prepare all the variables, turn on timer and encoder interruptions
// turn off the idle scan mode
void GetImage (void)
{
	/*
	MOTOR_FORWARD;
	StartMotorControl();								// we should call it before the EXTI (encoder iterruptions) enabled
	minIdleTime = 0xffff;								// min idle time is maximum :)
	encCnt=0;
	imgCnt=0;
	edgeCnt=BANKNOTE_FREE_RUN;					// edgeCnt = distance the banknote will run after the TOP scanner
	getImgState=1;
	suspectAF = 0;
	StartMG();
	uvBackground=uvCnt=0;
//	memset(img.buf,0,sizeof(img.buf)); 	// clean the previous image data
	uartCnt=3;													// for AS data only
	asResult1=asResult2=0;
	error_enc=0;
	doubleCnt=0;
	doubleSum=0;
*/
	DisableIdleScan();									// disable idle scan mode
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	__HAL_GPIO_EXTI_CLEAR_IT(ENC_Pin);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);	// EXTI (encoder pulses) interruptions are enabled
	
	init_timer6(50000);
	Timer6_reset();	
}


void DisableImageInterruptions(void)
{
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);	// disable EXTI (encoder pulses)
	DisableTimer5();											// disable timer5 (for MG only)
	deInit_timer7();											// disable timer7 (for motor speed control)
	deInit_timer6();
}


void SaveImg(unsigned char *buf)
{
	unsigned char i;
	
	for (i=0;i<22;i++) {
		buf[i] = resultBuf[i];
	}
}




// Image capturing state machine (encoder interrupt handler)
void EXTI15_10_IRQHandler()
{	
	HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	HAL_GPIO_EXTI_IRQHandler(ENC_Pin);			// throw the callback
}


// encoder interruption callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	unsigned int tmp=0;
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
	if(GPIO_Pin!=ENC_Pin) {return;}
	
	if(test_motor){error_motor=0;}
	
	tmp=GetTim6Value();
	if(tmp<ENCODER_SAVE_TIME){
		error_enc++;
		return;
	}
	Timer6_reset();

	MotorSpeedControl();			// PI regulator (motor speed stabilizer)
	/*
	switch (getImgState) {
		case 1:									// scan and save at [0] (banknote has not come yet)
			encCnt++;
//			GetSample(0);
			if(isNote) 	{					// if the banknote is under the top scanner
				if(encCnt<10) {			// if it was too early
					MOTOR_IDLE;				// stop scan, return
					DisableImageInterruptions();
					getImgState = ERR_CHAIN;
					return;
				}
				else {							// if it's Ok, go to the next case
					PrepareMG();			// averaged MG level is ready, temporary values are reset
					PrepareUV();			// averaged UV level is ready
					getImgState = 2;
					return;
				}
			}
			if(encCnt>500) {			// if the banknote has not come within 500mm
				MOTOR_IDLE;					// stop scan, return
				DisableImageInterruptions();
				getImgState = ERR_FEEDING;
				return;
			}
			else { 								// if everything's Ok,
				GetUvBackground();	// measure UV level without a banknote
				return; 						// let's wait for some more
			}			
			
		case 2:									// scan and save at [imgCnt] (banknote is here)
			imgCnt++;
//			GetSample(imgCnt);
			SaveDoubleSensorInfo();
			
			if(imgCnt>180) {			// if the banknote is too long
				MOTOR_IDLE;					// stop scan, return
				DisableImageInterruptions();
				getImgState = ERR_LONG_NOTE;
				return;
			} 
			if(isNote) 	{					// if the banknote is still under the top scanner,
				return;							// it's Ok, just continue capturing
			}
			else {								// if the banknote is over,
				getImgState = 3;		// go to the next case
				return;
			}
			
		case 3:									// scan the rest of the banknote (banknote is about to leave the scanner)
			imgCnt++;
			edgeCnt--;
			GetSample(imgCnt);
		
			if(edgeCnt<=0) {			// if the banknote went the path successfuly
				MOTOR_BREAK;				// stop scan, return
				DisableImageInterruptions();
				getImgState = IMG_OK;
				if(suspectAF) {getImgState = ERR_FISHING;}
				return;
			}
			else { return; }			// if the banknote is still on its way, let's wait more
	}*/
}


unsigned int CheckPath(void)
{
	/*unsigned int i=0;
	
	DisableIdleScan();	
	for(i=0;i<10;i++) {
		ScanTop(TOP_IR,cal.table.topItime);						
		ApplyCal(cal.table.topIRW,cal.table.topIRB);
		delay_ms(1);
	}		
	if((resultBuf[5]>MIN_LEVEL)||(resultBuf[10]>MIN_LEVEL)||(resultBuf[15]>MIN_LEVEL)) {	// the banknote is detected by the TOP IR image
		ClearLcdMemory();		
		LCD_ShowString(10,10,"ошибка:");
		LCD_ShowString(10,30,"верхний сканер");
		LCD_Refresh();			
		return 1;
	}
	
	for(i=0;i<10;i++) {
		ScanBottom(BOTTOM_IR,cal.table.botItime);						
		ApplyCal(cal.table.botIRW,cal.table.botIRB);
		delay_ms(1);
	}		
	if((resultBuf[5]>MIN_LEVEL)||(resultBuf[10]>MIN_LEVEL)||(resultBuf[15]>MIN_LEVEL)) {	// the banknote is detected by the TOP IR image
		ClearLcdMemory();		
		LCD_ShowString(10,10,"ошибка:");
		LCD_ShowString(10,30,"нижний сканер");
		LCD_Refresh();			
		return 1;
	}*/
	return 0;
}


// check insert sensor, jam sensor and both scanners if the banknote is in the path
void AreScannersClean(void)
{

}


// IDLE SCAN:
// when there's no banknote inside, we still need to use scanners and UV sensor
// to prevent the phototransistors extra charging
void TIM3_IRQHandler(void) // Idle scan routine
{
	TIM3->SR &= ~TIM_SR_UIF;
	/*
	ScanTop(TOP_RED,cal.table.topRtime);
	ScanBottom(BOTTOM_RED,cal.table.botRtime);
	GetUv(cal.table.uvTime);*/
}


