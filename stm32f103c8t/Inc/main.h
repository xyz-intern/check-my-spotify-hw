/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

#define START_STOP_BTN_Pin GPIO_PIN_3
#define START_STOP_BTN_GPIO_Port GPIOA
#define START_STOP_BTN_EXTI_IRQn EXTI3_IRQn
#define NEXT_PREVIOUS_BTN_Pin GPIO_PIN_4
#define NEXT_PREVIOUS_BTN_GPIO_Port GPIOA
#define NEXT_PREVIOUS_BTN_EXTI_IRQn EXTI4_IRQn
#define VOLUMN_UP_DOWN_Pin GPIO_PIN_5
#define VOLUMN_UP_DOWN_GPIO_Port GPIOA
#define VOLUMN_UP_DOWN_EXTI_IRQn EXTI9_5_IRQn
#define TRANSFORM_DISPLAY_Pin GPIO_PIN_6
#define TRANSFORM_DISPLAY_GPIO_Port GPIOA
#define TRANSFORM_DISPLAY_EXTI_IRQn EXTI9_5_IRQn
#define CLK1_Pin GPIO_PIN_0
#define CLK1_GPIO_Port GPIOB
#define DIO1_Pin GPIO_PIN_1
#define DIO1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
