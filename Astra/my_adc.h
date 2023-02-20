#include "stm32f4xx_hal.h"

#define ADC_PHOTO_1 	6		// ADC1 inputs
#define ADC_PHOTO_2 	14
#define ADC_MR_0 			2
#define ADC_MR_1 			10
#define ADC_MR_2 			11
#define ADC_MR_3 			12
#define ADC_MR_4 			3
#define ADC_MR_5 			4
#define ADC_UV 				13
#define ADC_INSERT_0  8
#define ADC_INSERT_1	9
#define ADC_AF	      1

#define IN_3_3V 			0		// ADC channels list
#define IN_DRV_MAIN 	1
#define IN_DRV_BAT 		2
#define IN_INVERTOR	 	3
#define IN_VBAT 			4
#define IN_PRINTER 		5
#define IN_12V 				6
#define IN_5V 				7
#define OUT_24V 			8
#define IN_24V 				9
#define IN_LED 				10
#define IN_BUTTON 		11

typedef struct {
	unsigned int in_3_3v;
	unsigned int in_drv_main;
	unsigned int in_drv_bat;
	unsigned int in_invertor;
	unsigned int in_vbat;
	unsigned int in_printer;
	unsigned int in_12v;
	unsigned int in_5v;
	unsigned int out_24v;
	unsigned int in_24v;
	unsigned int in_led;
	unsigned int in_button;	
} avg_t;

extern volatile avg_t avg;

void  ADC1_init(void);
unsigned int ADC1_getValue(void);
void ADC1_ChannelSelect(unsigned int channel);
unsigned int GetAdc_mV(int channel);

