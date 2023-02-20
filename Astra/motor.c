// motor control unit

#include "motor.h"
#include "timer.h"
#include "usart.h"
#include "pins.h"
#include "get_image.h"
#include "insert.h"
#include "st7735.h"
#include "get_image.h"
#include "protocol.h"

#ifdef MOTOR_DEBUG_ENABLED
unsigned int motorParams[3][500];
#endif

unsigned int motorCnt=0;

signed int motor_integr;
signed int motor_proport;
signed int motor_pi;

volatile unsigned int error_motor=0;
volatile unsigned int test_motor=0;


void StartMotorControl(void)
{
	init_timer7(50000);
	motorCnt=0;
	motor_integr=0;
}


void MotorSpeedControl(void)
{
	unsigned int tmp, idle;
	signed int speedDelta;
	
	tmp = GetTim7Value();						// get the encoder period
	idle = tmp - idleTime;
	
	if(tmp!=0) {tmp = 1000000/tmp;}	// transform period into speed (us => mm per second)
	
	if(motorCnt<500) {
#ifdef MOTOR_DEBUG_ENABLED
		motorParams[0][motorCnt] = tmp;
#endif
		motorCnt++;
	}
	if(motorCnt==1) {return;}				// first entrance may be dummy due to unpredictable encoder position
	
	Timer7_reset();									// reset and reload the timer
	ReLoadTimer7(50000);

	speedDelta = MAIN_MOTOR_TARGET_SPEED - tmp;	// PI controller calculations
	motor_integr += speedDelta*MAIN_MOTOR_KI/100;
	motor_proport = speedDelta*MAIN_MOTOR_KP/100;
	motor_pi = motor_integr + motor_proport;
	
	if(motor_pi<0) {motor_pi = 0;}
	if(motor_pi>100) {motor_pi = 100;}
	
	MOTOR_PWM(motor_pi);						// set the motor power 0%..100%

#ifdef MOTOR_DEBUG_ENABLED
	motorParams[1][motorCnt] = motor_proport;
	motorParams[2][motorCnt] = motor_integr;
#endif
	
	if(idle<minIdleTime) {minIdleTime = idle;}
}


// outputs:
// main motor speed and its PI regulator parameters
// min CPU idle time
void SendMotorReport(void)
{
#ifdef MOTOR_DEBUG_ENABLED	
	char tmpBuf[50];
	unsigned int i;

	USART1_SendByte('@');				// start token '@' for "Serial Port Plotter"
	for (i=1;i<motorCnt;i++) {	// the first measurement may be wrong, skip it
		sprintf(tmpBuf,"$%d %d %d;",motorParams[0][i],motorParams[1][i],motorParams[2][i]);
		USART1_SendBuf(tmpBuf);
	}
	sprintf(tmpBuf,"min CPU idle time = %dus\r\n",minIdleTime);	// min CPU free time
	USART1_SendBuf(tmpBuf);
#endif
}


// let's clean up the banknote path
unsigned char CleanPath(void)
{
	unsigned int i;
	
	MOTOR_REVERSE;
	delay_ms(100);
	CheckUartDMA();
	delay_ms(100);
	CheckUartDMA();
	delay_ms(100);
	CheckUartDMA();
	for(i=0;i<100;i++) {
		CheckUartDMA();
		delay_ms(10);
		if(IsNoteHere()==0) {
			MOTOR_IDLE;
			return 0;						
		}
	}
	MOTOR_IDLE;
	return 1;
}


// drive motor back and forth depending on time
void DriveMotor(int time)
{
	if(time<1500) 								{ MOTOR_FORWARD; }
	if((time>1500)&&(time<1600)) 	{ MOTOR_IDLE; }
	if((time>1600)&&(time<1700)) 	{ MOTOR_REVERSE; }
	if((time>1700)&&(time<1800)) 	{ MOTOR_IDLE; }
	if((time>1800)&&(time<2200)) 	{ MOTOR_FORWARD; }
	if((time>2200)&&(time<2300)) 	{ MOTOR_IDLE; }
	if((time>2300)&&(time<2400)) 	{ MOTOR_REVERSE; }
	if((time>2400)&&(time<2500)) 	{ MOTOR_IDLE; }
	if((time>2500)&&(time<2900)) 	{ MOTOR_FORWARD; }
	if((time>2900)&&(time<3000)) 	{ MOTOR_IDLE; }
	if((time>3000)&&(time<3100)) 	{ MOTOR_REVERSE; }
	if((time>3100)&&(time<3200)) 	{ MOTOR_IDLE; }
	if((time>3200)&&(time<4000)) 	{ MOTOR_FORWARD; }
}


int AcceptNote(void)
{
	int time_ms=0,tmp;
	
	while(1) {
		CheckUartDMA();
		delay_ms(10);
		time_ms+=10;
		DriveMotor(time_ms);
		if(time_ms>5000) {		// we've given up. No way to drop the banknote
			MOTOR_IDLE;
			return NOTE_ABORTED;	// we can comment it to exclude any timeout
		}
		if(PATH_OPEN==0) {		// check the insert sensor
			MOTOR_IDLE;
			return NOTE_ABORTED;
		}
		if(IsNoteHere()) {		// check the insert sensor, a new banknote may come
			MOTOR_BREAK;
			delay_ms(20);	
			MOTOR_IDLE;
			CheckUartDMA();
			time_ms = 0;
			currentAstraState |= ASTRA_STATE_INSERT_JAM;		
			LCD_SetFont(Arial_15x17,black);	
			ClearLcdMemory();	
			LCD_ShowString(10,10,"цепочка банкнот!");
			LCD_ShowString(10,25,"извлеките банкноту");
			LCD_ShowString(10,40,"из приемника");
			LCD_Refresh();
			tmp = 0;
			while(IsNoteHere()) {
				CheckUartDMA();
				tmp++;
				delay_ms(5);
				if(tmp<20) { SetClutzColor(CLUTZ_RED); }
				else { SetClutzColor(CLUTZ_BLACK); }
				if(tmp>40) { tmp = 0; }
				if(PATH_OPEN==0) {
					MOTOR_IDLE;
					SetClutzColor(CLUTZ_BLACK);
					return NOTE_ABORTED;
				}
			}
			SetClutzColor(CLUTZ_BLACK);
		}
		currentAstraState &= ~ASTRA_STATE_INSERT_JAM;	
		if((EXIT_JAM_SENSOR==0)&&(PATH_OPEN!=0)) {
			MOTOR_BREAK;
			delay_ms(20);	
			MOTOR_IDLE;
			return NOTE_DROPPED_OK;
		}		
	}
}
	

// reject the banknote, returns 1 if jammed
int RejectNote(void)
{
	int i,fJam=1;
	
	MOTOR_REVERSE;
	
	delay_ms(100);
	CheckUartDMA();
	delay_ms(100);
	CheckUartDMA();
	delay_ms(100);
	CheckUartDMA();
	for(i=0;i<1000;i++) {
		CheckUartDMA();
		delay_ms(1);
		if(IsNoteHere()==0) {
			fJam=0;
			break;						
		}
	}
	MOTOR_BREAK;
	delay_ms(20);	
	MOTOR_IDLE;
	
	return fJam;
}




unsigned int CheckMotorFirst (void)
{
	unsigned int i=0;
	test_motor=1;
	
	MOTOR_REVERSE;
	StartMotorControl();								// we should call it before the EXTI (encoder iterruptions) enabled
	getImgState=1;

	DisableIdleScan();									// disable idle scan mode
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	__HAL_GPIO_EXTI_CLEAR_IT(ENC_Pin);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);	// EXTI (encoder pulses) interruptions are enabled
		
	for(i=0;i<100;i++) {
		delay_ms(5);
		error_motor++;
	}

	MOTOR_IDLE;					// stop scan, return
	DisableImageInterruptions();
	EnableIdleScan();
	test_motor=0;
	
	if(error_motor>10) {	
		error_motor=0;
		ClearLcdMemory();		
		LCD_ShowString(10,10,"ошибка:");
		LCD_ShowString(10,30,"энкодер");
		LCD_Refresh();	
		return 1;
	}	
	return 0;
}

