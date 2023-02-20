#include "my_main.h"
//#include "adc.h"
#include "gpio.h"
#include "pins.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "usb_cdc.h"
#include "timer.h"
//#include "motor.h"
#include "..//GetDeNom//GetDeNom.h"
#include "st7735.h"

note_type current_note;


void AllStop (void)
{
	//MOTOR_REVERSE;
	//delay_ms(30);
	MOTOR_BREAK;	
	encoder_stop=0;
	moving_step=0;
	encoder_exist=encoder_counter;
	encoder_counter=0;
	encoder_counter_full=0;
	Disable_IRQ_ENC();
	DisableTimer6();
	DisableTimer7();
	RED_TOP(0);BLUE_TOP(0);GREEN_TOP(0);IR_TOP(0);UV_TOP(0);
	RED_BOT(0);BLUE_BOT(0);GREEN_BOT(0);IR_BOT(0);UV_BOT(0);
	AS_LED(0);AS_LED(0);UV_LED(0);
}


//void MakeResult (void)
//{
//	//check errors
//	if((current_note.err==0)&&(uv_note_level<200)&&(as>0)){result=1;}
//	else{result=0;}
//}


void Moving (void)
{
//	int tmp=0;
	switch(moving_step)
	{
		case 0: 
			if(INSERT_OUT_1==1)
			{
				MOTOR_FORWARD;
				Enable_IRQ_ENC();
				moving_step=1;
				//ClearUvBuffer();
				as=0;
				
			}
			break;
		case 1:
					
			if(encoder_counter_full>1000){
				AllStop();
				if(hUsbDeviceFS.dev_state==USBD_STATE_CONFIGURED) {
					CDC_Transmit_FS("Rolling\r\n", 9);
				}
				delay_ms(500);
				return;
			}
			
			if(end_note==1)
			{
				end_note=0;

				//EnableTimer5(100);
				//as_state=1;
				//delay_ms(100);
				//DisableTimer5();
				//while(as_state!=5){}

				AllStop();	
				
				//GetDeNom(&current_note);
					
				FindLen();
	
				CorrectUVData();			
				
				//LCD_OutInfo();
						
				CDC_SendData();
				as=0;
				
				delay_ms(100);
				
				delay_ms(100);

				MOTOR_FORWARD;
				delay_ms(200);
				MOTOR_BREAK;
				
				
				/*sprintf(buf,"%06d\r\n",tick_stop);
				if(hUsbDeviceFS.dev_state==USBD_STATE_CONFIGURED) {
					CDC_Transmit_FS(buf, 8);
				}*/
				
				
				//moving_step=2;
			}
			
			/*if(encoder_counter==400){	
				AllStop();
				CDC_SendData();			
			}*/
				
				
				
			break;
		case 2:
			MOTOR_REVERSE;
			moving_step=3;
			break;
		case 3:
			if(INSERT_OUT_1==0)
			{
				moving_step=4;
			}
			break;
		case 4:
			if(INSERT_OUT_1==1)
			{
			MOTOR_BREAK;
				
			HAL_Delay(1000);
			moving_step=0;
			}
			break;		
		default: 
			break;
	}
}




void my_main (void)
{
	Moving();
}



