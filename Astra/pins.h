#include "gpio.h"

#define ON 1
#define OFF 0

#define TFT_C_D(x) ((x)? (HAL_GPIO_WritePin(GPIOD,LCD_A0_Pin,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(GPIOD,LCD_A0_Pin,GPIO_PIN_RESET)));           //PD15
#define TFT_RESET(x) ((x)? (HAL_GPIO_WritePin(GPIOD,LCD_RESET_Pin,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(GPIOD,LCD_RESET_Pin,GPIO_PIN_RESET)));   //PD14
#define TFT_CS(x) ((x)? (HAL_GPIO_WritePin(GPIOD,LCD_CS_Pin,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(GPIOD,LCD_CS_Pin,GPIO_PIN_RESET)));            //PD13
#define TFT_SCK(x) ((x)? (HAL_GPIO_WritePin(GPIOC,LCD_SCK_Pin,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(GPIOC,LCD_SCK_Pin,GPIO_PIN_RESET)));         //PC7
#define TFT_SDA(x) ((x)? (HAL_GPIO_WritePin(GPIOC,LCD_SDA_Pin,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(GPIOC,LCD_SDA_Pin,GPIO_PIN_RESET)));         //PC6

#define StartButton HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4) 																																							// start button
#define LED_RED(x) ((x)? (HAL_GPIO_WritePin(GPIOE,LED_R_Pin,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(GPIOE,LED_R_Pin,GPIO_PIN_RESET)));	// LED
#define LOAD_R_24V(x) ((x)? (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,GPIO_PIN_RESET)));	
#define LOAD_24V(x) ((x)? (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14,GPIO_PIN_RESET)));	
#define LOAD_BATTERY(x) ((x)? (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,GPIO_PIN_SET)) : (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8,GPIO_PIN_RESET)));	

#define LOAD_5V(x) ((x)? (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,GPIO_PIN_RESET)) : (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,GPIO_PIN_SET)));
#define LOAD_12V(x) ((x)? (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_RESET)) : (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_10,GPIO_PIN_SET)));	
#define LOAD_PRINTER(x) ((x)? (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11,GPIO_PIN_RESET)) : (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11,GPIO_PIN_SET)));	
#define LOAD_CHARGE(x) ((x)? (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_12,GPIO_PIN_RESET)) : (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_12,GPIO_PIN_SET)));	
#define LOAD_INVERTOR(x) ((x)? (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_RESET)) : (HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_SET)));	

#define SET_ALL_OFF LOAD_24V(OFF); LOAD_R_24V(OFF); LOAD_BATTERY(OFF); LOAD_5V(OFF); LOAD_12V(OFF); LOAD_PRINTER(OFF); LOAD_CHARGE(OFF); LOAD_INVERTOR(OFF);

#define SET_BUTTON(x) ((x)? (HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET)) : (HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET)));	
