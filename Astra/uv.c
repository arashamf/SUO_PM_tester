// UV data capturing

#include "timer.h"
#include "pins.h"
#include "scanner.h"
#include "my_adc.h"	
#include "uv.h"
#include "calibration.h"

unsigned int uvResult,uv1,uv2;
unsigned int uvBackground,uvCnt;


// set the phototransistor's emitter as an analog input (High-Z) + ADC
void SetUvPinAnalog(void) 
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = UV_ADC_Pin;		// Analog input
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	ADC1_ChannelSelect(ADC_UV); 
}


// set the phototransistor's emitter as a digital output with logical 0.
void SetUvPinLowLevel(void) 
{	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = UV_ADC_Pin;	// GPIO output push-pull
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOC, UV_ADC_Pin, GPIO_PIN_RESET);	// set output = 0
}


// get UV sample routine
void GetUv(unsigned int chargeTime)
{
	UV_Collector(0);			// release
	delay_us(30);	
	SetUvPinAnalog();			// set the phototransistor's emitter pin as analog input (High-Z)
	delay_us(30);	
	
	UV_Collector(1);			// UV level reading#1
	delay_us(20);
	uv1 = ADC1_getValue();
	UV_Collector(0);
	
	UV_LED(1);						// exposure
	delay_us(chargeTime);
	UV_LED(0);
	
	UV_Collector(1);			// UV level reading#2
	delay_us(20);
	uv2 = ADC1_getValue();
	SetUvPinLowLevel();		// set the phototransistor's emitter pin as digital output, low level (discharge)
	
	if(uv2>=uv1) { uvResult = uv2-uv1; }
	else {uvResult = 0;}
}


// capture UV data before the banknote is under the UV sensor
// this makes virtual zero level for the UV for each banknote feeding
void GetUvBackground(void)
{
	GetUv(cal.table.uvTime);
	uvBackground+= uvResult;
	uvCnt++;
}


void PrepareUV(void)
{
	uvBackground = uvBackground/uvCnt;
}


// the UV result is 0..255
unsigned int ApplyUvCal(void) 
{
	signed int tmpUv;
	
	if(uvResult<uvBackground) {uvResult=uvBackground;}
	tmpUv = ((uvResult-uvBackground)*256)/(cal.table.uvW-cal.table.uvB);	
	if(tmpUv<0) {tmpUv = 0;}
	if(tmpUv>255) {tmpUv=255;}		
	return tmpUv;								
}


// get averaged UV data
void GetAvgUv(unsigned int tmpChargeTime)
{
	unsigned int i,avg1=0,avg2=0,avgRes=0;	
	
	for (i=0;i<UV_AVG_INTERVAL;i++) {
		GetUv(tmpChargeTime);
		delay_ms(1);
		avg1+=uv1;
		avg2+=uv2;
		avgRes+=uvResult;
	}
	uv1 = avg1/UV_AVG_INTERVAL;
	uv2 = avg2/UV_AVG_INTERVAL;
	uvResult = avgRes/UV_AVG_INTERVAL;
}


