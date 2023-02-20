
#include "softI2C.h"
#include "timer.h"
#include "stm32f4xx_hal.h"

#define DI2C delay_us(1); 

#define I2C_SDA_PORT GPIOB
#define I2C_SCL_PORT GPIOB
#define I2C_SDA_PIN GPIO_PIN_9
#define I2C_SCL_PIN GPIO_PIN_8

#define XSHUT_PIN GPIO_PIN_11	// for VL53x01 sensor only
#define XSHUT_PORT GPIOE

#define SDA1 HAL_GPIO_WritePin(I2C_SDA_PORT,I2C_SDA_PIN,GPIO_PIN_SET)
#define SDA0 HAL_GPIO_WritePin(I2C_SDA_PORT,I2C_SDA_PIN,GPIO_PIN_RESET)
#define SCL1 HAL_GPIO_WritePin(I2C_SCL_PORT,I2C_SCL_PIN,GPIO_PIN_SET) 
#define SCL0 HAL_GPIO_WritePin(I2C_SCL_PORT,I2C_SCL_PIN,GPIO_PIN_RESET) 
#define SDAin HAL_GPIO_ReadPin(I2C_SDA_PORT,I2C_SDA_PIN)  

#define XSHUT1 HAL_GPIO_WritePin(XSHUT_PORT,XSHUT_PIN,GPIO_PIN_SET) 
#define XSHUT0 HAL_GPIO_WritePin(XSHUT_PORT,XSHUT_PIN,GPIO_PIN_RESET)

int isDistanceSensorPresent=1;

void GpioConfig(int pinNumber, int pinMode, int pushPull, int speed, GPIO_TypeDef * port)
{
	GPIO_InitTypeDef GPIO_InitStruct;
		
	GPIO_InitStruct.Pin = pinNumber;
  GPIO_InitStruct.Mode = pinMode;
  GPIO_InitStruct.Pull = pushPull;
  GPIO_InitStruct.Speed = speed;
  HAL_GPIO_Init(port, &GPIO_InitStruct);
}


void I2C_SoftInit(void)
{
  GpioConfig(I2C_SDA_PIN,GPIO_MODE_OUTPUT_OD,GPIO_NOPULL,GPIO_SPEED_FREQ_MEDIUM,I2C_SDA_PORT);
  GpioConfig(I2C_SCL_PIN,GPIO_MODE_OUTPUT_OD,GPIO_NOPULL,GPIO_SPEED_FREQ_MEDIUM,I2C_SCL_PORT);
	GpioConfig(XSHUT_PIN,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_MEDIUM,XSHUT_PORT);
	XSHUT1;
}


void I2C_SoftStart(void)
{
	unsigned int tmp=0;
	SDA1;
	DI2C;
	SCL1;
	DI2C;
	while(!SDAin)	{
		tmp++;
		if(tmp>1000){
			isDistanceSensorPresent=0;
			return;
		}
		SCL0;
		DI2C;
		SCL1;
		DI2C;
	}
	SDA0;
	DI2C;
	SCL0;
	DI2C;
}


void I2C_SoftStop(void)
{
	SDA0;
	DI2C;
	SCL1;
	DI2C;
	SDA1;
	DI2C;
}


unsigned char I2C_SoftWriteByte(unsigned char data)
{
	unsigned char i;
	unsigned char ACK;
	
	for(i=0;i<8;i++) {
		if(data & 0x80)	{
			SDA1;
		}
		else {
			SDA0;
		}
		DI2C;
		SCL1;
		DI2C;        
		SCL0;
		data=data<<1;    
	}
	DI2C;        
	SCL1;
	DI2C;        
	ACK = !SDAin;
	SCL0;
	SDA0;
	return ACK;
}


unsigned char I2C_SoftReadByte(unsigned char ACK)
{
	unsigned char i;
	unsigned char data;
		
	SDA1;
	for(i=0;i<8;i++) {
		DI2C;
		SCL1;
		DI2C;       
		data<<=1;
		if(SDAin)
		data++; 
		SCL0;
	}
	if (ACK) {
		SDA0;
	}
	DI2C;       
	SCL1;
	DI2C;       
	SCL0;
	SDA1;
	return data;
}


