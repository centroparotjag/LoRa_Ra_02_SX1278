/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define led_button_Pin GPIO_PIN_13
#define led_button_GPIO_Port GPIOC
#define adc_ref25_Pin GPIO_PIN_0
#define adc_ref25_GPIO_Port GPIOA
#define adc_ntc_Pin GPIO_PIN_1
#define adc_ntc_GPIO_Port GPIOA
#define adc_pow_Pin GPIO_PIN_2
#define adc_pow_GPIO_Port GPIOA
#define adc_cir_0V_Pin GPIO_PIN_3
#define adc_cir_0V_GPIO_Port GPIOA
#define res_IPS_Pin GPIO_PIN_4
#define res_IPS_GPIO_Port GPIOA
#define dc_IPS_Pin GPIO_PIN_0
#define dc_IPS_GPIO_Port GPIOB
#define pow_hold_Pin GPIO_PIN_1
#define pow_hold_GPIO_Port GPIOB
#define DIO0_Pin GPIO_PIN_2
#define DIO0_GPIO_Port GPIOB
#define cs_Ra02_Pin GPIO_PIN_12
#define cs_Ra02_GPIO_Port GPIOB
#define cs_flash_Pin GPIO_PIN_13
#define cs_flash_GPIO_Port GPIOB
#define res_Ra02_Pin GPIO_PIN_8
#define res_Ra02_GPIO_Port GPIOA
#define pow_button_Pin GPIO_PIN_9
#define pow_button_GPIO_Port GPIOA
#define enc_up_Pin GPIO_PIN_3
#define enc_up_GPIO_Port GPIOB
#define enc_button_Pin GPIO_PIN_4
#define enc_button_GPIO_Port GPIOB
#define enc_down_Pin GPIO_PIN_5
#define enc_down_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
