/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: contains all hardware driver

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
 /******************************************************************************
  * @file    weight.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    17-April-2019
  * @brief   contains all hardware driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
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
#ifndef __WEIGHT_H
#define __WEIGHT_H

#include "common.h"

/* ---------------------------  WEIGHT HW definition -------------------------------*/
#define WEIGHT_SCK_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define WEIGHT_SCK_PORT       		 GPIOA	 
#define WEIGHT_SCK_PIN       			 GPIO_PIN_11 
#define WEIGHT_DOUT_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define WEIGHT_DOUT_PORT      		 GPIOB
#define WEIGHT_DOUT_PIN     			 GPIO_PIN_12  
#define HX711_SCK_0          			 HAL_GPIO_WritePin(WEIGHT_SCK_PORT,WEIGHT_SCK_PIN ,GPIO_PIN_RESET)
#define HX711_SCK_1          			 HAL_GPIO_WritePin(WEIGHT_SCK_PORT,WEIGHT_SCK_PIN ,GPIO_PIN_SET)
#define HX711_DOUT_0          		 HAL_GPIO_WritePin(WEIGHT_DOUT_PORT,WEIGHT_DOUT_PIN ,GPIO_PIN_RESET)
#define HX711_DOUT_1          		 HAL_GPIO_WritePin(WEIGHT_DOUT_PORT,WEIGHT_DOUT_PIN ,GPIO_PIN_SET)
/* ---------------------------  +5v PWR OUT definition -------------------------------*/
#define PWR_OUT_PORT              GPIOB
#define PWR_OUT_PIN               GPIO_PIN_5
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 
/**
 * @brief  
 *
 * @note
 * @retval None
 */

#define RCC_GPIO_CLK_ENABLE( __GPIO_PORT__ )              \
do {                                                    \
    switch( __GPIO_PORT__)                                \
    {                                                     \
      case GPIOA_BASE: __HAL_RCC_GPIOA_CLK_ENABLE(); break;    \
      case GPIOB_BASE: __HAL_RCC_GPIOB_CLK_ENABLE(); break;    \
      case GPIOC_BASE: __HAL_RCC_GPIOC_CLK_ENABLE(); break;    \
      case GPIOD_BASE: __HAL_RCC_GPIOD_CLK_ENABLE(); break;    \
      case GPIOH_BASE: default:  __HAL_RCC_GPIOH_CLK_ENABLE(); \
    }                                                    \
  } while(0)  

void WEIGHT_SCK_Init(void);
void WEIGHT_DOUT_Init(void);
uint32_t HX711_Read(void);
void Get_Maopi(void);
int32_t Get_Weight(void);
void HW_GPIO_Init( GPIO_TypeDef* port, uint16_t GPIO_Pin, GPIO_InitTypeDef* initStruct);
#ifdef __cplusplus
}
#endif

#endif

