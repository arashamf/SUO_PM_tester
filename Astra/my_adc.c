#include "my_adc.h"
#include "pins.h"

volatile avg_t avg;


void ADC1_init(void)
{
	ADC1->SQR3 = 1; 														// channel#1 selected
	ADC1->CR2|=0x01;														// ADC1 is ON
}


unsigned int ADC1_getValue (void)
{
	ADC1->CR2 |= (1<<30)|(1<<0);								// SWSTART and ADON to start single ADC conversion
	while(!(ADC1->SR&(1<<1)));
	return (uint16_t) ADC1->DR;
}


void ADC1_ChannelSelect(unsigned int channel)
{
	channel = channel & 0x1f;
	ADC1->SQR3 = channel;
}


unsigned int GetAdc_mV(int channel)
{
	unsigned int v;
	
	ADC1_ChannelSelect(channel);
	v=ADC1_getValue();
	switch (channel) {
		case 0: v = (3300*v*(24+24))/4095/24; break;		// resistor voltage divider is 24k/24k
		case 1: v = (3300*v*(330+24))/4095/24; break;		// resistor voltage divider is 330k/24k
		case 2: v = (3300*v*(330+24))/4095/24; break;		// resistor voltage divider is 330k/24k
		case 3: v = (3300*v*(330+24))/4095/24; break;		// resistor voltage divider is 330k/24k
		case 4: v = (3300*v*(330+24))/4095/24; break;		// resistor voltage divider is 330k/24k
		case 5: v = (3300*v*(330+24))/4095/24; break;		// resistor voltage divider is 330k/24k
		case 6: v = (3300*v*(330+24))/4095/24; break;		// resistor voltage divider is 330k/24k
		case 7: v = (3300*v*(330+24))/4095/24; break;		// resistor voltage divider is 24k/24k
		case 8: v = (3300*v*(330+24))/4095/24; break;		// resistor voltage divider is 330k/24k
		case 9: v = (3300*v*(330+24))/4095/24; break;		// resistor voltage divider is 330k/24k
		case 10: v = (3300*v)/4095; break;							// no resistor voltage divider 
		case 11: v = (3300*v)/4095; break;							// no resistor voltage divider 
		
		default: v=0;
	}
	return v;		
}


void TIM3_IRQHandler(void) // Idle scan routine  exec time: 20us
{
	TIM3->SR &= ~TIM_SR_UIF;
	
	avg.in_3_3v = 		(avg.in_3_3v*9 + GetAdc_mV(IN_3_3V)) / 10;
	avg.in_drv_main = (avg.in_drv_main*9 + GetAdc_mV(IN_DRV_MAIN)) / 10;
	avg.in_drv_bat = 	(avg.in_drv_bat*9 + GetAdc_mV(IN_DRV_BAT)) / 10;
	avg.in_invertor = (avg.in_invertor*9 + GetAdc_mV(IN_INVERTOR)) / 10;
	avg.in_vbat = 		(avg.in_vbat*9 + GetAdc_mV(IN_VBAT)) / 10;
	avg.in_printer = 	(avg.in_printer*9 + GetAdc_mV(IN_PRINTER)) / 10;
	avg.in_12v = 			(avg.in_12v*9 + GetAdc_mV(IN_12V)) / 10;
	avg.in_5v = 			(avg.in_5v*9 + GetAdc_mV(IN_5V)) / 10;
	avg.out_24v = 		(avg.out_24v*9 + GetAdc_mV(OUT_24V)) / 10;
	avg.in_24v = 			(avg.in_24v*9 + GetAdc_mV(IN_24V)) / 10;
	avg.in_led = 			(avg.in_led*9 + GetAdc_mV(IN_LED)) / 10;
	avg.in_button = 	(avg.in_button*9 + GetAdc_mV(IN_BUTTON)) / 10;
}

