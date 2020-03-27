/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "usart.h"
#include "rtc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "common.h"
#include "NBIOT_ComConfiguration.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define  UART_ENABLE_RE(USARTx)       USARTx.Instance->CR1|= (uint32_t)0x0004            
#define  UART_DISABLE_RE(USARTx)      USARTx.Instance->CR1&= (~(uint32_t)0x0004)  
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void My_UART_RxCpltCallback(void);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	sys.power = 1;
	sys.start = 1;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
	MX_DMA_Init();
  MX_LPUART1_UART_Init();
  MX_USART2_UART_Init(); 
  MX_ADC_Init();
  MX_IWDG_Init();
  MX_TIM3_Init();
	MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	HAL_Delay(100);
	HAL_UART_Receive_IT(&huart2,(uint8_t*)uart.rxData,RXSIZE);
	HAL_Delay(100);
	HAL_UART_Receive_DMA(&hlpuart1,(uint8_t*)uart.rxData,RXSIZE);
	HAL_Delay(100);
	memset(uart.Data,0,sizeof(uart.Data));
	My_AlarmInit(10,ALARMB);	
	
	ledState(HAL_OK,1000);
	HAL_Delay(100);
	devInformation();
	
	nbiotInit();
	MyRtcInit();
		
	user_main_printf("Please ENTER Password to active AT Command Line\r\n");
	memset(uart.Data,0,sizeof(uart.Data));
	__HAL_TIM_CLEAR_FLAG(&htim3,TIM_FLAG_UPDATE);
	HAL_TIM_Base_Start_IT(&htim3);
	
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(workmodel.model != model0 && workmodel.model != model6 && workmodel.uplink == 1)
		{
			sys.start = 0;			
			sys.power = 1;
			workModel();
		}
		if(sys.power && sys.led == 0 && sys.start == 0)
		{	
			lowPower();
		}
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim7)
	{
		if(sys.led == 1)
		{
			ledState(!HAL_OK,NULL);
			HAL_TIM_Base_Stop_IT(&htim7);
		}
	}
	else if(htim == &htim3)
	{	
		if(sys.pwordcount == 1)
		{
			user_main_printf("+PWORD: TIMEOUT\r\n");
			sys.pwordcount = 0;
			sys.power = 1;
			sys.power_rxflag = 0;
			sys.start = 0;
		}
		else if(sys.start == 1)
		{
			sys.start = 0;
			user_main_printf("No action\r\n");
		}
		HAL_TIM_Base_Stop_IT(&htim3);
		sys.lowpower = 1;
	}
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{	
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	workmodel.uplink = 1;
	workmodel.nb_resart = 0;
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
	sys.watchdog = 0;
	sys.lowpower = 0;
	HAL_IWDG_Refresh(&hiwdg);
	My_AlarmInit(18,ALARMB);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_3)
	{
		sys.power_rxflag = 1;
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);		
	}
	else if(GPIO_Pin == GPIO_PIN_14)
	{
		sensor.count++;
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
		workmodel.uplink = 1;	
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &hlpuart1)
	{
		strcat(uart.Data,uart.rxData);
		if(strstr(uart.Data,"+NSOSTR") != NULL && uart.Data[strlen(uart.Data)-1] == '\n')	
		{
			if(workmodel.model == model6)
				user_main_printf("%s",uart.Data);
			
			char *pch = strrchr(uart.Data,',');
			char sock[2]={0};
			sprintf(sock+strlen(sock), "%c", workmodel.socket);
			if(uart.Data[pch - uart.Data +1] == sock[0])
				uart.issued=1;
		}
		else if((strstr(uart.Data,ATError_description[AT_OK]) || strstr(uart.Data,ATError_description[AT_ERROR])) && uart.Data[strlen(uart.Data)-1] == '\n')
		{
			if(workmodel.model == model6)
			{
				user_main_printf("%s",uart.Data);
//				memset(uart.Data,0,sizeof(uart.Data));
			}
			
			uart.receiveFlag=1;
		}
		while(HAL_UART_Receive_DMA(&hlpuart1,(uint8_t*)uart.rxData,RXSIZE) == HAL_OK);
	}
	else if(huart == &huart2)
	{
		if(!sys.InputPer)
			goto end;
		
		strcat(uart.Data,uart.rxData);

		if(uart.rxData[0] == '\r')
		{
//			UART_DISABLE_RE(huart2);			
			My_UART_RxCpltCallback();
//			UART_ENABLE_RE(huart2);
			
			__HAL_TIM_CLEAR_FLAG(&htim3,TIM_FLAG_UPDATE);
			HAL_TIM_Base_Start_IT(&htim3);
		}
			
		end:
		{}
			
		while(HAL_UART_Receive_IT(&huart2,(uint8_t*)uart.rxData,RXSIZE) == HAL_OK);			
	}
}


void My_UART_RxCpltCallback(void)
{	
	rtrim(uart.Data);	
	if(strcmp(uart.Data,sys.pword) == 0 && sys.pwordcount == 0)
	{
		user_main_printf("\r\nPassword Correct\r\n");
		sys.pwordcount = 1;
		goto end;
	}
	else if(sys.pwordcount == 0)
	{
		user_main_printf("\r\nPassword Incorrect\r\n");
		goto end;
	}
			
	if(sys.pwordcount != 0 && strlen(uart.Data) >0)
	{
		ATInsPro(uart.Data);
		if(workmodel.model != model6)
			goto end;
	}
	if(workmodel.model == model6 && strlen(uart.Data) >0)
	{
		char databuff[strlen(uart.Data)+2];
		memset(databuff,0,sizeof(databuff));
		strcat(databuff,&uart.Data[1]);
		memset(uart.Data,0,sizeof(uart.Data));
		strcat(databuff,"\n");
		HAL_UART_Transmit_DMA(&hlpuart1,(uint8_t*)databuff,strlen(databuff));
		goto end;
	}
		
	end:
	{memset(uart.Data,0,sizeof(uart.Data));}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart == (&hlpuart1))
	{
		uint32_t isrflags = READ_REG(huart->Instance->ISR);
		switch(huart->ErrorCode)
    {
				case HAL_UART_ERROR_NONE:
						user_main_error("HAL_UART_ERROR_NONE\r\n");
						break;
				case HAL_UART_ERROR_PE:
						user_main_error("HAL_UART_ERROR_PE\r\n");
						READ_REG(huart->Instance->RDR);//PE清标志，第二步读DR
						READ_REG(huart->Instance->TDR);
						__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_PE);//清标志
						break;
				case HAL_UART_ERROR_NE:
						user_main_error("HAL_UART_ERROR_NE\r\n");
						READ_REG(huart->Instance->RDR);//NE清标志，第二步读DR
						READ_REG(huart->Instance->TDR);
						__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_NE);//清标志
						break;
				case HAL_UART_ERROR_FE:
						user_main_error("HAL_UART_ERROR_FE\r\n");
						READ_REG(huart->Instance->RDR);//FE清标志，第二步读DR
						READ_REG(huart->Instance->TDR);
						__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE);//清标志
						break;
				case HAL_UART_ERROR_ORE:
						user_main_error("HAL_UART_ERROR_ORE\r\n");
						READ_REG(huart->Instance->RDR);//ORE清标志，第二步读DR
						READ_REG(huart->Instance->TDR);
						__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);//清标志
						break;
				case HAL_UART_ERROR_DMA:
						user_main_error("HAL_UART_ERROR_DMA\r\n");
						break;
				default:
						user_main_error("other\r\n");
						break;
    }
		MX_LPUART1_UART_Init();
		HAL_UART_Receive_DMA(&hlpuart1,(uint8_t*)uart.rxData,RXSIZE);
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Configure LSE Drive Capability 
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_LPUART1
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	user_main_printf("Wrong");
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
