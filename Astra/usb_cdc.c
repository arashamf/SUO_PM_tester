#include "usb_cdc.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "timer.h"
#include "adc.h"
#include "pins.h"
#include "scanner.h"	
#include "rtc.h"

#define SizeLenNote 4
#define SizeErrNote 4
#define SizeTypeNote 4
#define SizeOrientationNote 4
#define SizeDispNote 4
#define UvMedLevel 4
#define AsState 4
#define SizeResult 4


char buf_rec[100];
unsigned int counter_buf=0;
unsigned int flag_receive=0;

void CDC_SendImage(void)
{
	int answer=0;
	//int i=0;

//	for(i=0;i<sizeof(img.buf);i++)	{		// replace all "0" with "1" for synchronization
//		if(img.buf[i]==0){img.buf[i]=1;}
//	}
//	img.buf[0]=0x00;	// package start token byte
	
	if(hUsbDeviceFS.dev_state==USBD_STATE_CONFIGURED) {	// send the packet
	//	answer=CDC_Transmit_FS(img.buf, sizeof(img.buf));	
		if(answer!=USBD_OK){}
		else{}
	}
	else {}	// LED_RED(1);
}


void CDC_SendData(unsigned char *buf, int len)
{
	int i,answer=0;
	unsigned char mybuf[256];
	
	mybuf[0] = 0;
	mybuf[1] = len;
	for(i=0;i<len;i++) {
		mybuf[i+2] = buf[i];
	}

	if(hUsbDeviceFS.dev_state==USBD_STATE_CONFIGURED) {	// send the packet
		answer=CDC_Transmit_FS(mybuf,len+2);	
		if(answer!=USBD_OK){}
	}
}


unsigned int CDC_RecData (unsigned char data)
{
	int len=0;
	if(data){				
		if(data==0x02){		
			counter_buf=1;
			buf_rec[0]=data;
			return 0;
		}
		if(counter_buf>0){
			buf_rec[counter_buf]=data;
			counter_buf++;
		}
		if(data==0x03){				
			len=counter_buf;counter_buf=0;	
			flag_receive=1;
			return(CDC_CheckMessage(len));
		}
	}		
	return 0;
}


//Перевод из двух ascii в одно шестадцатиричное
//'A''5' в 0хA5
unsigned char atoh (unsigned char x,unsigned char y)
{
	if(x>'9'){x=(x-0x37)<<4;}
	else{x=(x-0x30)<<4;}

	if(y>'9'){y=y-0x37;}
	else{y=y-0x30;}

	return x+y;
}


unsigned int CDC_CheckMessage (unsigned int len)
{
	unsigned char crc=0;
	unsigned char i=0,y=0;

	//Проверка начала и конца сообщения
	if((buf_rec[0]!=0x02)||(buf_rec[len-1]!=0x03)){flag_receive=0;CDC_ClearRecBuf();return 0;}
	
	//подсчет и проверка контрольной суммы пришедшего сообщения
	for(i=1;i<len-3;i++)
	{crc=crc^buf_rec[i];y++;}
	i=atoh(buf_rec[len-3],buf_rec[len-2]);	
	if(i!=crc){flag_receive=0;CDC_ClearRecBuf();return 0;}

	flag_receive=len;
	
	return 0;
}

/*
void CDC_ParseMessage (void)
{
	char command[4]={0,0,0,0};
	
	if(flag_receive==0) {return;}																	// no data received, exit.
	
	command[0]=buf_rec[4];
	command[1]=buf_rec[5];
	command[2]=buf_rec[6];		
	if (strstr (command, "STI")){																	// команда установки времени
		SystemTime.Day=(buf_rec[8]-0x30)*10+(buf_rec[9]-0x30);			// Day
		SystemTime.Month=(buf_rec[11]-0x30)*10+(buf_rec[12]-0x30);	// Month
		SystemTime.Year=(buf_rec[14]-0x30)*1000+(buf_rec[15]-0x30)*100+(buf_rec[16]-0x30)*10+(buf_rec[17]-0x30); //Year
		SystemTime.H=(buf_rec[19]-0x30)*10+(buf_rec[20]-0x30);			// H
		SystemTime.M=(buf_rec[22]-0x30)*10+(buf_rec[23]-0x30);			// M
		SystemTime.S=(buf_rec[25]-0x30)*10+(buf_rec[26]-0x30);			// S
		RTC_SetTime(SystemTime.Year-2000,SystemTime.Month,SystemTime.Day,SystemTime.H,SystemTime.M,SystemTime.S,0);
		
		CDC_SendData("OK",3);
	}
	flag_receive=0;
	CDC_ClearRecBuf();
}
*/

void CDC_ClearRecBuf (void)
{
	unsigned int i=0;
	for(i=0;i<sizeof(buf_rec);i++) buf_rec[i]=0;
}

