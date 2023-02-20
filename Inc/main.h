/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define LED_R_Pin GPIO_PIN_2
#define LED_R_GPIO_Port GPIOE
#define LED_G_Pin GPIO_PIN_3
#define LED_G_GPIO_Port GPIOE
#define MH1_Pin GPIO_PIN_0
#define MH1_GPIO_Port GPIOC
#define MH2_Pin GPIO_PIN_1
#define MH2_GPIO_Port GPIOC
#define MH3_Pin GPIO_PIN_2
#define MH3_GPIO_Port GPIOC
#define UV_ADC_Pin GPIO_PIN_3
#define UV_ADC_GPIO_Port GPIOC
#define AS_2_ADC_Pin GPIO_PIN_0
#define AS_2_ADC_GPIO_Port GPIOA
//#define AS_1_ADC_Pin GPIO_PIN_1
//#define AS_1_ADC_GPIO_Port GPIOA
#define AF_ADC_Pin GPIO_PIN_1
#define AF_ADC_GPIO_Port GPIOA
#define MH0_Pin GPIO_PIN_2
#define MH0_GPIO_Port GPIOA
#define MH4_Pin GPIO_PIN_3
#define MH4_GPIO_Port GPIOA
#define MH5_Pin GPIO_PIN_4
#define MH5_GPIO_Port GPIOA
#define SCLK_1_Pin GPIO_PIN_5
#define SCLK_1_GPIO_Port GPIOA
#define PH_1_ADC_Pin GPIO_PIN_6
#define PH_1_ADC_GPIO_Port GPIOA
#define MOSI_2_Pin GPIO_PIN_7
#define MOSI_2_GPIO_Port GPIOA
#define PH_2_ADC_Pin GPIO_PIN_4
#define PH_2_ADC_GPIO_Port GPIOC
#define DISCHARGE_1_Pin GPIO_PIN_5
#define DISCHARGE_1_GPIO_Port GPIOC
#define IR_BOT_Pin GPIO_PIN_5 //
#define IR_BOT_GPIO_Port GPIOB
#define RED_BOT_Pin GPIO_PIN_4 //
#define RED_BOT_GPIO_Port GPIOB
#define BLUE_BOT_Pin GPIO_PIN_2
#define BLUE_BOT_GPIO_Port GPIOB
#define GREEN_BOT_Pin GPIO_PIN_7
#define GREEN_BOT_GPIO_Port GPIOE
#define NMR_1_Pin GPIO_PIN_8
#define NMR_1_GPIO_Port GPIOE
#define INSERT_LED_1_Pin GPIO_PIN_9
#define INSERT_LED_1_GPIO_Port GPIOE
#define ENC_Pin GPIO_PIN_10
#define ENC_GPIO_Port GPIOE
#define ENC_LED_Pin GPIO_PIN_11
#define ENC_LED_GPIO_Port GPIOE
#define UV_LED_Pin GPIO_PIN_12
#define UV_LED_GPIO_Port GPIOE
#define AS_LED_Pin GPIO_PIN_11   //было PE13
#define AS_LED_GPIO_Port GPIOD
#define MOTOR_R_Pin GPIO_PIN_15
#define MOTOR_R_GPIO_Port GPIOE
#define GREEN_TOP_Pin GPIO_PIN_10
#define GREEN_TOP_GPIO_Port GPIOB
#define IR_TOP_Pin GPIO_PIN_11
#define IR_TOP_GPIO_Port GPIOB
#define NMR_2_Pin GPIO_PIN_12
#define NMR_2_GPIO_Port GPIOB
#define SCLK_2_Pin GPIO_PIN_13
#define SCLK_2_GPIO_Port GPIOB
#define TEST_Pin GPIO_PIN_14
#define TEST_GPIO_Port GPIOB
#define MOSI_2B15_Pin GPIO_PIN_15
#define MOSI_2B15_GPIO_Port GPIOB
#define BLUE_TOP_Pin GPIO_PIN_8
#define BLUE_TOP_GPIO_Port GPIOD
#define DISCHARGE_2_Pin GPIO_PIN_9
#define DISCHARGE_2_GPIO_Port GPIOD
#define RED_TOP_Pin GPIO_PIN_10
#define RED_TOP_GPIO_Port GPIOD
#define INSERT_2_Pin GPIO_PIN_1  //
#define INSERT_2_GPIO_Port GPIOB
#define INSERT_1_Pin GPIO_PIN_0  //
#define INSERT_1_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_13
#define LCD_CS_GPIO_Port GPIOD
#define LCD_RESET_Pin GPIO_PIN_14
#define LCD_RESET_GPIO_Port GPIOD
#define LCD_A0_Pin GPIO_PIN_15
#define LCD_A0_GPIO_Port GPIOD
#define LCD_SDA_Pin GPIO_PIN_6
#define LCD_SDA_GPIO_Port GPIOC
#define LCD_SCK_Pin GPIO_PIN_7
#define LCD_SCK_GPIO_Port GPIOC
#define BUT_2_Pin GPIO_PIN_8
#define BUT_2_GPIO_Port GPIOA
#define BUT_1_Pin GPIO_PIN_15
#define BUT_1_GPIO_Port GPIOA
#define VCC_SD_Pin GPIO_PIN_0
#define VCC_SD_GPIO_Port GPIOD
#define BUZZER_Pin GPIO_PIN_0
#define BUZZER_GPIO_Port GPIOE
#define VCC_USB_Pin GPIO_PIN_1
#define VCC_USB_GPIO_Port GPIOE

#define SD_POWER_Pin GPIO_PIN_0
#define SD_POWER_GPIO_Port GPIOD

#define SD_CD_Pin GPIO_PIN_3
#define SD_CD_GPIO_Port GPIOB

#define UV_Collector_Pin GPIO_PIN_7

#define Clutz_R_Pin GPIO_PIN_1
#define Clutz_B_Pin GPIO_PIN_3
#define Clutz_G_Pin GPIO_PIN_4

#define EXIT_JAM_SENSOR_Pin GPIO_PIN_9
#define PATH_OPEN_Pin GPIO_PIN_14



#define ADC_IN_LED_Pin GPIO_PIN_0				// ADC PortC
#define ADC_IN_BUTTON_Pin GPIO_PIN_1
#define ADC_IN_3_3V_Pin GPIO_PIN_0			// ADC PortA
#define ADC_IN_DRV_MAIN_Pin GPIO_PIN_1
#define ADC_IN_DRV_BAT_Pin GPIO_PIN_2
#define ADC_IN_INVERTOR_Pin GPIO_PIN_3
#define ADC_IN_VBAT_Pin GPIO_PIN_4
#define ADC_IN_PRINTER_Pin GPIO_PIN_5
#define ADC_IN_12V_Pin GPIO_PIN_6
#define ADC_IN_5V_Pin GPIO_PIN_7
#define ADC_OUT_24V_Pin GPIO_PIN_0			// ADC PortB
#define ADC_IN_24V_Pin GPIO_PIN_1






/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
