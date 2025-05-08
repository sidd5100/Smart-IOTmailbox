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
#define R1_pin_Pin GPIO_PIN_0
#define R1_pin_GPIO_Port GPIOB
#define R2_pin_Pin GPIO_PIN_1
#define R2_pin_GPIO_Port GPIOB
#define R3_pin_Pin GPIO_PIN_2
#define R3_pin_GPIO_Port GPIOB
#define R4_pin_Pin GPIO_PIN_3
#define R4_pin_GPIO_Port GPIOB
#define C1_pin_Pin GPIO_PIN_4
#define C1_pin_GPIO_Port GPIOB
#define C2_pin_Pin GPIO_PIN_5
#define C2_pin_GPIO_Port GPIOB
#define C4_pin_Pin GPIO_PIN_7
#define C4_pin_GPIO_Port GPIOB
#define C3_pin_Pin GPIO_PIN_8
#define C3_pin_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
