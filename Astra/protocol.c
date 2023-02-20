
#include "timer.h"
#include "pins.h"
#include "protocol.h"
#include "st7735.h"
#include <stdio.h> 
#include <string.h>
#include "flash.h"
#include "usart.h"
#include "protocol.h"
//#include "globalVars.h"
#include "rtc.h"

#define SIZE_DMA_BUF 100
unsigned char dma_buf_rec[SIZE_DMA_BUF];
unsigned char dma_buf_send[SIZE_DMA_BUF];

volatile unsigned int dma_cnt=0;
unsigned int dma_state=0;
char mes_len[3];
char mes_ses[3];
char mes_command[3];
char mes_salt[8];
char mes_checksum[8];
char mes_data[SIZE_DMA_BUF];
unsigned int  mes_data_cnt=0;
int saltCnt=0;
int crcCnt=0;
int crc=0;

unsigned int rub_10=0;
unsigned int rub_50=0;
unsigned int rub_100=0;
unsigned int rub_200=0;
unsigned int rub_500=0;
unsigned int rub_1000=0;
unsigned int rub_2000=0;
unsigned int rub_5000=0;
unsigned int rub_total=0;

unsigned char allow_notes[8]={1,1,1,1,1,1,1,1};

unsigned int currentAstraState=ASTRA_STATE_READY; 	// this state is to be sent to host PC

unsigned int NextByte (void);

enum
{
	STATUS=0,
	VERSION,
	SUM_REPORT,
	CLEAR_REPORT,
	ALLOW_NOTE,
	CLUTZ_LED,
	COUNTER_NOTE,                                                                                                        
	RESET_POWER,
	GET_TIME,
	SET_TIME,
	CURRENCY
};


void ClearDmaBuffer (void)
{
	unsigned int i=0;
	
	for(i=0;i<sizeof(dma_buf_rec);i++) {
		dma_buf_rec[i]=0x30;
	}
}


void StartReceiveCommand (void)
{
	ClearDmaBuffer();
	dma_cnt=0;
	dma_state=0;
	mes_data_cnt=0;
	HAL_UART_DMAStop(&huart1);
	HAL_UART_Receive_DMA(&huart1, dma_buf_rec, SIZE_DMA_BUF);
}


void CheckCommand (void)
{
	int i=0;
	if (!memcmp (mes_command, "STA",3)) {SendCommand(STATUS);}
	if (!memcmp (mes_command, "VER",3)) {SendCommand(VERSION);}
	if (!memcmp (mes_command, "SUM",3)) {SendCommand(SUM_REPORT);}
	if (!memcmp (mes_command, "CLR",3)) {
		rub_10=0;rub_50=0;rub_100=0;rub_200=0;rub_500=0;rub_1000=0;rub_2000=0;rub_5000=0;rub_total=0;
		SendCommand(CLEAR_REPORT);
	}	
	if (!memcmp (mes_command, "ALL",3)) {
		for(i=0;i<8;i++) {
			if(mes_data[i+3]=='1') {allow_notes[i]=1;}
			else{allow_notes[i]=0;}
		}
		SendCommand(ALLOW_NOTE);
	}
	if (!memcmp (mes_command, "CLU",3)) {
		if(mes_data[0]=='1') {CLUTZ_R(1);} else {CLUTZ_R(0);}
		if(mes_data[1]=='1') {CLUTZ_G(1);} else {CLUTZ_G(0);}
		if(mes_data[2]=='1') {CLUTZ_B(1);} else {CLUTZ_B(0);}
		SendCommand(CLUTZ_LED);
	}
	if (!memcmp (mes_command, "CNT",3)) {SendCommand(COUNTER_NOTE);}
	if (!memcmp (mes_command, "RST",3)) {
		SendCommand(RESET_POWER);
		delay_ms(10);
		
		
		
		ClearLcdMemory();	
LCD_ShowString(10,10,"ошибка 6");
LCD_Refresh();
while(1);
		
		
		
		NVIC_SystemReset();
	}	
	
	if (!memcmp (mes_command, "STI",3)) {
		SystemTime.Day=(mes_data[0]-0x30)*10+(mes_data[1]-0x30);			// Day
		SystemTime.Month=(mes_data[2]-0x30)*10+(mes_data[3]-0x30);		// Month
		SystemTime.Year=(mes_data[4]-0x30)*1000+(mes_data[5]-0x30)*100+(mes_data[6]-0x30)*10+(mes_data[7]-0x30); //Year
		SystemTime.H=(mes_data[9]-0x30)*10+(mes_data[10]-0x30);				// H
		SystemTime.M=(mes_data[11]-0x30)*10+(mes_data[12]-0x30);			// M
		SystemTime.S=(mes_data[13]-0x30)*10+(mes_data[14]-0x30);			// S
		RTC_SetTime(SystemTime.Year-2000,SystemTime.Month,SystemTime.Day,SystemTime.H,SystemTime.M,SystemTime.S,0);
		SendCommand(SET_TIME);
	}	
	if (!memcmp (mes_command, "GTI",3)) {SendCommand(GET_TIME);}
	if (!memcmp (mes_command, "CUR",3)) {SendCommand(CURRENCY);}
}


void CheckUartDMA (void)
{
	unsigned int dma_len=0,count=0;

	count=(hdma_usart1_rx.Instance->NDTR);
	dma_len= SIZE_DMA_BUF - count;
	
	while(dma_cnt<dma_len) {
		if(NextByte()) {
			CheckCommand();
			StartReceiveCommand();
			return;
		}
	}
}


// take next byte and put it into a specified variable or array
unsigned int NextByte (void)
{
	unsigned int tmp,len,i,crc_mes,result;
	
	if(dma_cnt>(SIZE_DMA_BUF-5)) {			// nothing to parse (5 is a magic number)
		StartReceiveCommand();
		return 0;
	}
	tmp=dma_buf_rec[dma_cnt++];
	
	if((tmp<0x20)||(tmp>0x7F)){ 
		if(tmp==0x02){										// STX byte received
			dma_state=1;
			saltCnt = 0;
			crcCnt = 0;
			crc = 0;
		}
		return 0;
	}
	if(dma_state==0){return 0;}
	dma_state++;		
	if((dma_state>1)&&(dma_state<5))	{
		if((tmp<'0')||(tmp>'9')) {				// if non-digit character in the "len" field was found
			dma_state=0;
			return 0;
		}
		crc+=tmp;
		mes_len[dma_state-2]=tmp;					// receive length
		return 0;
	}
	len=(mes_len[0]-0x30)*100+(mes_len[1]-0x30)*10+(mes_len[2]-0x30); 						// extract the length
	if((dma_state>4)&&(dma_state<8))	{mes_ses[dma_state-5]=tmp;			crc+=tmp; return 0;}	// session number
	if((dma_state>7)&&(dma_state<11))	{mes_command[dma_state-8]=tmp;	crc+=tmp; return 0;}	// command name
	if(dma_state<(len+2-16)){																											// dma_state >= 11
		if((dma_state-11)>=SIZE_DMA_BUF) {return 0;}																// check the array size
		mes_data[dma_state-11]=tmp;
		mes_data_cnt++;
		crc+=tmp;
		return 0;
	}
	if(saltCnt<8) {											// receive salt
		mes_salt[saltCnt++]=tmp;	
		return 0;
	}		
	if(crcCnt<7) {											// receive checksum
		mes_checksum[crcCnt++]=tmp;	
		return 0;
	}		
	mes_checksum[crcCnt]=tmp;
	
	crc_mes = 0;
	for(i=0;i<8;i++) {
		if(mes_checksum[i]>'9'){tmp=mes_checksum[i]-0x37;}		// capital letters only
		else{tmp=mes_checksum[i]-0x30;}
		crc_mes += tmp<<((7-i)*4);
	}
	if(crc==crc_mes) {result = 1;}
	else {
		dma_state=0;
		result = 0;
	}
/*
	char tmpBuf[50];
	ClearLcdMemory();								// Show packet
	LCD_SetFont(Arial_15x17,black);	
	LCD_ShowString(10,0,"len: ");LCD_ShowStringSize(40,0,mes_len,3);								
	LCD_ShowString(10,15,"ses: ");LCD_ShowStringSize(40,15,mes_ses,3);
	LCD_ShowString(10,30,"com: ");LCD_ShowStringSize(40,30,mes_command,3);
	LCD_ShowString(10,45,"data: ");LCD_ShowStringSize(40,45,mes_data,mes_data_cnt);
	LCD_ShowString(10,60,"salt: ");LCD_ShowStringSize(40,60,mes_salt,8);
	LCD_ShowString(10,75,"crc: ");LCD_ShowStringSize(40,75,mes_checksum,8);
	if(result == 1){sprintf(tmpBuf,"crc validation: ok");}
	else{sprintf(tmpBuf,"crc %X =! %X",crc,crc_mes);}   
	LCD_ShowString(10,90,tmpBuf);
	LCD_Refresh();	
	*/
	return result;
}


void SendCommand (unsigned int command)
{
	unsigned int len=4,i=0,y=0;
	unsigned int crc=0;
	unsigned char checksum[8];
	char tmpBuf[50];

	dma_buf_send[0]=0x03;		//write STX
	memcpy(&dma_buf_send[4],mes_ses,3);	//write Number of session
	len=len+6;

	switch(command)	//write command
	{
		case STATUS:
			memcpy(&dma_buf_send[7],"STA",3);
			memcpy(&dma_buf_send[10],"READY",5);
			len=len+5;
			break;
		case VERSION:
			memcpy(&dma_buf_send[7],"VER",3);
//			memcpy(&dma_buf_send[10],SECRET_VERSION,8);
			len=len+8;
			break;
		case SUM_REPORT:
			memcpy(&dma_buf_send[7],"SUM",3);
			/*memcpy(&dma_buf_send[10],"RUB",3);len=len+3;
			WriteNomToBuf(rub_10,13);len=len+3;dma_buf_send[len++]=',';
			WriteNomToBuf(rub_50,17);len=len+3;dma_buf_send[len++]=',';
			WriteNomToBuf(rub_100,21);len=len+3;dma_buf_send[len++]=',';
			WriteNomToBuf(rub_200,25);len=len+3;dma_buf_send[len++]=',';
			WriteNomToBuf(rub_500,29);len=len+3;dma_buf_send[len++]=',';
			WriteNomToBuf(rub_1000,33);len=len+3;dma_buf_send[len++]=',';
			WriteNomToBuf(rub_2000,37);len=len+3;dma_buf_send[len++]=',';
			WriteNomToBuf(rub_5000,41);len=len+3;*/
	
			sprintf(tmpBuf,"RUB%03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d",rub_10,rub_50,rub_100,rub_200,rub_500,rub_1000,rub_2000,rub_5000);
			memcpy(&dma_buf_send[10],tmpBuf,34);
			len=len+34;
			break;
		case CLEAR_REPORT:
			memcpy(&dma_buf_send[7],"CLR",3);
			break;
		case ALLOW_NOTE:
			memcpy(&dma_buf_send[7],"ALL",3);
			break;
		case CLUTZ_LED:
			memcpy(&dma_buf_send[7],"CLU",3);
			break;
		case COUNTER_NOTE:
			memcpy(&dma_buf_send[7],"CNT",3);
			sprintf(tmpBuf,"%07d",ReadNoteCounter());
			memcpy(&dma_buf_send[10],tmpBuf,7);
			len=len+7;
			break;
		case RESET_POWER:
			memcpy(&dma_buf_send[7],"RST",3);
			break;
		case GET_TIME:
			memcpy(&dma_buf_send[7],"GTI",3);			
			RTC_GetTime();	
			sprintf(tmpBuf,"%02d%02d%04d_%02d%02d%02d",date.Date,date.Month,date.Year+2000,time.Hours,time.Minutes,time.Seconds);
			memcpy(&dma_buf_send[10],tmpBuf,15);
			len=len+15;
			break;
		case SET_TIME:
			memcpy(&dma_buf_send[7],"STI",3);
			break;
		case CURRENCY:
			memcpy(&dma_buf_send[7],"CUR",3);
			break;
		default:
			memcpy(&dma_buf_send[7],"UNK",3);
			break;
	}
	
	memcpy(&dma_buf_send[len],"11111111",8);	//write salt
	sprintf(tmpBuf,"%03d",len-1); 						//write len	
	memcpy(&dma_buf_send[1],tmpBuf,3);
	for(i=1;i<len+8;i++){crc=crc+dma_buf_send[i];}	//write crc

	for(i=0;i<8;i++) {
		checksum[i]=(crc>>(28-y))&0x0F;
		if(checksum[i]>9){checksum[i]=checksum[i]+0x37;}
		else{checksum[i]=checksum[i]+0x30;}
		y=y+4;
	}
	memcpy(&dma_buf_send[len+8],checksum,8);
	USART1_SendBuf((char*)dma_buf_send);	
}


void SaveNotesCounter(void)
{
/*	if(note.detectionResult!=GENUINE) {return;}
	
	if(((note.denomYear)==0)&&(allow_notes[0]==1))	{rub_10++;}
	if(((note.denomYear)==1)&&(allow_notes[1]==1))	{rub_50++;}
	if(((note.denomYear)==2)&&(allow_notes[2]==1))	{rub_100++;}
	if(((note.denomYear)==3)&&(allow_notes[3]==1))	{rub_200++;}
	if(((note.denomYear)==4)&&(allow_notes[4]==1))	{rub_500++;}
	if(((note.denomYear)==5)&&(allow_notes[4]==1))	{rub_500++;}
	if(((note.denomYear)==6)&&(allow_notes[5]==1))	{rub_1000++;}
	if(((note.denomYear)==7)&&(allow_notes[5]==1))	{rub_1000++;}
	if(((note.denomYear)==8)&&(allow_notes[6]==1))	{rub_2000++;}
	if(((note.denomYear)==9)&&(allow_notes[7]==1))	{rub_5000++;}
	if(((note.denomYear)==10)&&(allow_notes[7]==1))	{rub_5000++;}*/
}
		

