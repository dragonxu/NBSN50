#ifndef _print_h
#define _print_h

#include "stdio.h"
#include "common.h"
#include "stm32l0xx_hal.h"


//#define USER_MAIN_DEBUG
#define user_main_printf(format, ...) do{																								\
																					RTC_DateTypeDef sdatestructureget;			\
																					RTC_TimeTypeDef stimestructureget;			\
																					HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);	\
																					HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);	\
																					printf("[%.2d-%.2d-%.2d ", sdatestructureget.Year,sdatestructureget.Month, sdatestructureget.Date);	\
																					printf(" %.2d:%.2d:%.2d]", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);		\
																					printf( format "\r\n", ##__VA_ARGS__);				\
																				}while(0)\

#ifdef USER_MAIN_DEBUG
	#define user_main_info(format, ...) printf("[main info]" format "\r\n", ##__VA_ARGS__)
	#define user_main_debug(format, ...) printf("[main debug]" format "\r\n", ##__VA_ARGS__)
	#define user_main_error(format, ...) printf("[main error]" format "\r\n",##__VA_ARGS__)
#else	
	#define user_main_info(format, ...)
	#define user_main_debug(format, ...)
	#define user_main_error(format, ...)
#endif

#endif
