#include "main.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "rtc.h"
#include "pins.h"
#include "st7735.h"
#include "usbd_cdc_if.h"
#include "timer.h"
#include "my_adc.h"
#include "flash.h"
#include "usb_cdc.h"
#include "dma.h"
#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void SystemClock_Config(void);
int get_system_reset_cause(void);


int main(void)
{
	int cnt=0;
	
	HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();	
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_ADC1_Init();
	ADC1_init();
	SysTick->CTRL = 0;									
	SET_ALL_OFF;
	USART1_SendBuf("Start testing");				
	//MX_DMA_Init();
	USART1_EnableInt();
	EnablePeriodicalAdc();
	lcdInit();

	WaitInput24V();
	if(StartButton==0) {
		LOAD_R_24V(ON);
		delay_ms(500);
		LOAD_24V(ON);
		while(1) {
			LCD_ShowVoltages();
		}
	}
	LCD_TerminalAddString("*** Ready ***");
	LCD_TerminalAddString("Press START");
	
	while (1) {
		if(StartButton==0) {
			LED_RED(1);
			MainTestRoutine();
		}
		cnt++;
		if(cnt<500) {
			LED_RED(1);
		}
		else {
			LED_RED(0);
		}
		if (cnt>1000) {cnt=0;}
		delay_ms(1);
  }
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;//|RCC_OSCILLATORTYPE_LSE;		
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;																				
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}


void _Error_Handler(char *file, int line)
{

}



//0 - סבנמסא םו בûכמ
//1 - LOW POWER RESET
//2 - סבנמס ןמ WWWD
//3 - סבנמס ןמ IWDT
//4 - soft reset
//5 - POWER ON RESET
//6 - EXTERNAL PIN RESET
//7 - BROWNOUT RESET (BOR)
//8 - ץףיודמחםאוע
int get_system_reset_cause(void)
 {
		int reset_cause = 0;
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
		  {
        reset_cause = 1; //LOW POWER RESET
      }
		 else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
      {
        reset_cause = 2; //סבנמס ןמ WWWD
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
      {
        reset_cause = 3; //סבנמס ןמ IWDT 
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
      {
        reset_cause = 4; //soft reset ?? NVIC_SystemReset()
			}
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
      {
        reset_cause = 5; //POWER ON RESET
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
      {
        reset_cause = 6; //EXTERNAL PIN RESET
      }
    else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
      {
        reset_cause = 7; //BROWNOUT RESET (BOR)
      }
    else
      {
        reset_cause = 8; //ץףיודמחםאוע
      }

    // Clear all the reset flags or else they will remain set during future resets until system power is fully removed.
    __HAL_RCC_CLEAR_RESET_FLAGS();
    return reset_cause; 
 }
