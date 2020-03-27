#ifndef _Ultrasound_h_
#define _Ultrasound_h_

#include "common.h"
#include "tim.h"

/* ---------------------------  ULT definition -------------------------------*/
#define ULT_TRIG_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define ULT_TRIG_PORT             GPIOB
#define ULT_TRIG_PIN              GPIO_PIN_12
#define ULT_Echo_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define ULT_Echo_PORT             GPIOA
#define ULT_Echo_PIN              GPIO_PIN_11

void GPIO_ULT_INPUT_Init(void);
void GPIO_ULT_OUTPUT_Init(void);
uint16_t ULT_distance(void);

#endif
