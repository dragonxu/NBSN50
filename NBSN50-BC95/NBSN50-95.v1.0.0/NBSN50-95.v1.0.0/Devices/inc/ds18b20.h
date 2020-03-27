#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "common.h"

/* ---------------------------  DS18B20-1 HW definition -------------------------------*/
#define DOUT1_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define DOUT1_PORT          GPIOB	 
#define DOUT1_PIN           GPIO_PIN_3
#define DOUT1_READ()        HAL_GPIO_ReadPin(DOUT1_PORT,DOUT1_PIN)
#define DOUT1_0             HAL_GPIO_WritePin(DOUT1_PORT,DOUT1_PIN,GPIO_PIN_RESET)
#define DOUT1_1             HAL_GPIO_WritePin(DOUT1_PORT,DOUT1_PIN,GPIO_PIN_SET)

/* ---------------------------  DS18B20-2 HW definition -------------------------------*/
#define DOUT2_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define DOUT2_PORT          GPIOA	 
#define DOUT2_PIN           GPIO_PIN_9
#define DOUT2_READ()        HAL_GPIO_ReadPin(DOUT2_PORT,DOUT2_PIN)
#define DOUT2_0             HAL_GPIO_WritePin(DOUT2_PORT,DOUT2_PIN,GPIO_PIN_RESET)
#define DOUT2_1             HAL_GPIO_WritePin(DOUT2_PORT,DOUT2_PIN,GPIO_PIN_SET)

/* ---------------------------  DS18B20-3 HW definition -------------------------------*/
#define DOUT3_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define DOUT3_PORT          GPIOA	 
#define DOUT3_PIN           GPIO_PIN_10
#define DOUT3_READ()        HAL_GPIO_ReadPin(DOUT3_PORT,DOUT3_PIN)
#define DOUT3_0             HAL_GPIO_WritePin(DOUT3_PORT,DOUT3_PIN,GPIO_PIN_RESET)
#define DOUT3_1             HAL_GPIO_WritePin(DOUT3_PORT,DOUT3_PIN,GPIO_PIN_SET)

void DS18B20_delay(uint16_t time);
void DS18B20_Mode_IPU(uint8_t num);
void DS18B20_Mode_Out_PP(uint8_t num);
void DS18B20_IoDeInit(uint8_t num);
void DS18B20_Rst(uint8_t num);
uint8_t DS18B20_Presence(uint8_t num);
uint8_t DS18B20_ReadBit(uint8_t num);
uint8_t DS18B20_ReadByte(uint8_t num);
void DS18B20_WriteBit(uint8_t dat,uint8_t num);
void DS18B20_WriteByte(uint8_t dat,uint8_t num);
void DS18B20_SkipRom(uint8_t num);
float DS18B20_GetTemp_SkipRom (uint8_t num);


#endif 
