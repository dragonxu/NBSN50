#ifndef _common_h_
#define _common_h_

#include "stm32l0xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "print.h"
#include "flash.h"
#include "usart.h"
#include "rtc.h"
#include "at.h"
#include "adc.h"
#include "sht31.h"
#include "sht20.h"
#include "ds18b20.h"
#include "battery.h"
#include "iwdg.h"
#include "ultrasound.h"
#include "lidar.h"
#include "COAP_Statck.h"
#include "NBIOT_ComConfiguration.h"
#include "serverDataSend.h"
#include "serverDataProcessing.h"
#include "weight.h"


#define RXSIZE   1
#define model0 	'0'
#define model1 	'1'
#define model2 	'2'
#define model3 	'3'
#define model4 	'4'
#define model5 	'5'
#define model6 	'6'

typedef enum
{
	ALARMX = 0,
	ALARMA,
	ALARMB
}ALARM;

typedef struct
{
	uint8_t receiveFlag;
	uint8_t issued;
	char rxData[10];
	char Data[300];
}UART;

typedef struct
{
	uint8_t led;
	uint8_t lowpower;
	uint8_t watchdog;
	uint8_t power;
	uint8_t power_rxflag;
	uint8_t start;
	uint8_t InputPer;
	uint8_t pwordcount;
	char pword[10];
}SYSTEM;

typedef struct
{
	uint8_t socket;
	uint8_t nb_resart;
	uint8_t uplink;
	uint8_t model;
	uint8_t inmod;
	uint8_t time2count;
	uint8_t i2cDevice;
	uint16_t Tdc;
	uint16_t cycles;
	uint16_t power_time;
}WORKMODEL;

typedef struct
{
	uint8_t  flag;
	uint8_t  signal;
	uint16_t temDs18b20_1;
	uint16_t temDs18b20_2;
	uint16_t temDs18b20_3;
	uint16_t humSHT;
	uint16_t temSHT;
	uint16_t batteryLevel_mV;
	uint16_t adc0;
	uint16_t adc1;
	uint16_t adc4;
	uint16_t distance;
	uint16_t count;
	float GapValue;
	char data[32];
}SENSOR;

extern UART uart;
extern SYSTEM sys;
extern WORKMODEL workmodel;
extern SENSOR sensor;

void workModel(void);
void devInformation(void);
void lowPower(void);
void text(void);
void i2cDetection(void);
uint16_t ADCModel(uint32_t channel);
char* payLoadDeal(uint8_t model,char* payload);
ALARM My_AlarmInit(uint16_t timer,uint8_t alarmX);
void SystemClock_Config(void);
void RX_GPIO_Init(void);
void ledState(uint8_t state,uint16_t time);
void EX_GPIO_Init(uint8_t state);
void MyRtcInit(void);
void powerState(uint8_t state);
#endif
