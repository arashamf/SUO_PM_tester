#include "flash.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "scanner.h"	
#include "st7735.h"
#include "timer.h"
#include "calibration.h"

#define NOTE_COUNTER_ADDRESS 0x081C0000
#define NOTE_COUNTER_SECTOR FLASH_SECTOR_22

#define DEVICE_ADDRESS 0x081E0000
#define DEVICE_SECTOR FLASH_SECTOR_23

// writes [nWords] words into the flash
void SaveCalValues(unsigned int *buf, unsigned int nWords)
{
	unsigned int i;
	
	ClearLcdMemory();
	LCD_SetFont(Arial_15x17,black);
	LCD_ShowString(10,40,"Saving data");
	LCD_ShowString(10,60,"to FLASH ...");
	LCD_Refresh();
	
	__disable_irq();
	HAL_FLASH_Unlock();
	FLASH_Erase_Sector(DEVICE_SECTOR, FLASH_VOLTAGE_RANGE_3);

	for (i=0;i<nWords;i++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, DEVICE_ADDRESS+i*4, buf[i]);
	}

	HAL_FLASH_Lock();
	__enable_irq();
	
	ClearLcdMemory();
	LCD_SetFont(Arial_15x17,black);
	LCD_ShowString(30,40,"DONE!");
	LCD_ShowString(30,60,"Reload....");
	LCD_Refresh();
	delay_ms(1000);
}

// reads [nWords] words from the flash
// return 1 if the device has been calibrated ("fist time" mode)
int LoadCalValues(unsigned int *buf, unsigned int nWords)
{
	unsigned int * src;
	unsigned int * dest;
	unsigned int res=0;
	unsigned int word=0;
	
	src  = (unsigned int *)DEVICE_ADDRESS;
	dest = (unsigned int *)buf;
	
	while (nWords){
		*dest++ = word = *src++;
		if(word!=0xFFFFFFFF){res=1;}	// if Astra was calibrated
		nWords--;
	}
	return res;
}


unsigned int ReadNoteCounter (void)
{
	unsigned int i=0,y=0;
	unsigned int counter=0;
	unsigned int * src;
	unsigned int word=0;
	unsigned int b=0;
	
	src  = (unsigned int *)NOTE_COUNTER_ADDRESS;
	for(i=0;i<0x8000;i++)	{
		word=*src++;
		if(word==0x00000000) {counter+=32;}
		else{			
			b=0x00000001;
			for(y=0;y<32;y++)	{
				if(word&b) {return counter;}
				else {counter++;}
				b=b<<1;
			}		
		}
	}
	return counter;
}


void WriteGlobalNoteCounter (void)
{
	unsigned int count;
	unsigned int bit=0,word=0;
	unsigned int i=0,y=0xFFFFFFFF;
	
	count=ReadNoteCounter();
	count++;
	
	word=count/32;
	bit=count%32;
	if(bit){
		for(i=0;i<bit;i++){y=y<<1;}
	}
	else{
		word--;
		y=0;
	}
	
	__disable_irq();
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, NOTE_COUNTER_ADDRESS+word*4, y);
	HAL_FLASH_Lock();
	__enable_irq();
}


void ResetNoteCounter(void)
{
	__disable_irq();
	HAL_FLASH_Unlock();
	FLASH_Erase_Sector(NOTE_COUNTER_SECTOR, FLASH_VOLTAGE_RANGE_3);
	HAL_FLASH_Lock();
	__enable_irq();
}


void ResetCalValues(void)
{
	__disable_irq();
	HAL_FLASH_Unlock();
	FLASH_Erase_Sector(DEVICE_SECTOR, FLASH_VOLTAGE_RANGE_3);
	HAL_FLASH_Lock();
	__enable_irq();
}


void FillNoteCounter(void)
{
	unsigned int i=0;
	__disable_irq();
	HAL_FLASH_Unlock();
	for(i=0;i<112500;i++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, NOTE_COUNTER_ADDRESS+i, 0);
	}
	HAL_FLASH_Lock();
	__enable_irq();
}

