#include "common.h"

UART uart ={.receiveFlag=0,.issued=0,.rxData={0},.Data={0}};
SYSTEM sys ={.InputPer=1,.pwordcount=0,.led=0,.watchdog = 0,.start=0,.power=0,.lowpower=0,.power_rxflag=0,.pword="12345678"};
WORKMODEL workmodel ={.socket='1',.uplink=1,.model=model0,.time2count=0,.i2cDevice=0,.Tdc=300,.cycles=0,.inmod=0,
											.nb_resart=0,.power_time = 0};
SENSOR sensor ={.flag=0,.signal=0,.temDs18b20_1=0,.temDs18b20_2=0,.temDs18b20_3=0,.humSHT=0,.temSHT=0,
								.batteryLevel_mV=0,.adc0=0,.adc1=0,.adc4=0,.distance=0,.count=0,.GapValue = 400.0,.data={0}};

static int num = 0;
								
void workModel(void)
{
	uint8_t result = fail;
	static uint8_t nrbNUM=0;
	user_main_printf("*****Upload start:%d*****\r\n",workmodel.cycles);
	sys.InputPer = 0;
	EX_GPIO_Init(workmodel.inmod-0x30);
	payLoadDeal(workmodel.model,sensor.data);
	if(sockCreat())
	{
		char payload[400]={0};
		dataSendGet(payload);
		HAL_Delay(100);
		memset(payload,0,sizeof(payload));
		if(dataSendPut(payload) == fail)
		{
			result = fail;
		}
		else
		{
			My_AlarmInit(workmodel.Tdc,ALARMA);
			result = success;		
		}
	}	
	if(!result)
	{
		user_main_printf("\nTry again in 3 minutes\n");
		dataTransmission(AT CSQ"\n");user_main_error("%s",uart.Data);
		HAL_Delay(100);
		dataTransmission(AT CSCON"?\n");user_main_error("%s",uart.Data);
		My_AlarmInit(180,ALARMA);
		nrbNUM++;
		if(nrbNUM >2)
		{
			nrbNUM = 0;
			dataTransmission(AT NRB"\n");
		}
	}
	HAL_Delay(100);
	workmodel.cycles++;
	sys.InputPer = 1;
	workmodel.uplink = 0;	
	sys.lowpower = 1;
	sockClosd();
	user_main_printf("*****End of upload*****\r\n");	
}

void devInformation(void)
{
	 user_main_printf("\r\nDRAGINO NBSN50-95 NB-IoT Sensor Node\r\n"
										"Image Version: V1.0.0\r\n"
										"NB-IoT Stack : D-BC95-001\r\n");										
	at_cfg_run(NULL);
	printf("\r\n");
}

ALARM My_AlarmInit(uint16_t timer,uint8_t alarmX)
{
	if(alarmX == ALARMB)
		HAL_RTC_DeactivateAlarm(&hrtc,RTC_ALARM_B);
	else if(alarmX == ALARMA)
		HAL_RTC_DeactivateAlarm(&hrtc,RTC_ALARM_A);
	
  RTC_AlarmTypeDef sAlarm = {0};
	uint8_t _hour = 0,_min = 0,_sec = 0;
	uint8_t hour = 0,min = 0,sec = 0;
	uint8_t minFlag=0,secFlag=0;
	
	RTC_TimeTypeDef stimestructureget;
	RTC_DateTypeDef sdatestructureget;
	HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
	
	uint16_t _median = 0;
	_hour 	= timer /3600;
	_median = timer %3600;
	_min  	= _median /60;
	_sec  	= _median %60;
	sec =	(stimestructureget.Seconds+_sec)%10+(stimestructureget.Seconds+_sec)/10*16;	
	if(sec > 0x59)
	{
		sAlarm.AlarmTime.Seconds = sec - 0x60;
		secFlag = 1;
	}
	else
		sAlarm.AlarmTime.Seconds = sec;
	
	if(secFlag)
	{
		min = (stimestructureget.Minutes+_min +1)%10+(stimestructureget.Minutes+_min +1)/10*16;
	}
	else
		min = (stimestructureget.Minutes+_min)%10+(stimestructureget.Minutes+_min)/10*16;
	if(min > 0x59)
	{
		sAlarm.AlarmTime.Minutes = min - 0x60;
		minFlag = 1;
	}
	else
		sAlarm.AlarmTime.Minutes = min;
	
	if(minFlag)
	{
		hour = (stimestructureget.Hours+_hour +1)%10+(stimestructureget.Hours+_hour +1)/10*16;
	}	
	else
		hour = (stimestructureget.Hours+_hour)%10+(stimestructureget.Hours+_hour)/10*16;
	if(hour > 0x23)
	{
		sAlarm.AlarmTime.Hours = hour - 0x24;
	}
	else
		sAlarm.AlarmTime.Hours = hour;

	sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
	
	if(alarmX == ALARMA)
		sAlarm.Alarm = RTC_ALARM_A;
	else 
		sAlarm.Alarm = RTC_ALARM_B;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
	
	return ALARMX;
}

void lowPower(void)
{
	if(sys.lowpower)
		user_main_printf("Enter low power mode\r\n");
	
	HAL_TIM_Base_Stop_IT(&htim3);
	HAL_Delay(100);
	sys.watchdog = 1;
	RX_GPIO_Init();
	
	/* ------------------------------ */
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Enable Ultra low power mode */
  HAL_PWREx_EnableUltraLowPower();
  
  /* Enable the fast wake up from Ultra low power mode */
  HAL_PWREx_EnableFastWakeUp();
	
  /* Select MSI as system clock source after Wake Up from Stop mode */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);
  
	__HAL_RCC_PWR_CLK_ENABLE();
	
  HAL_SuspendTick();

  /* Enter Stop Mode */
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	
	HAL_ResumeTick();
	
	/* ------------------------------ */
	HAL_UART_MspInit(&huart2);
	HAL_UART_Receive_IT(&huart2,(uint8_t*)uart.rxData,RXSIZE);
	
	SystemClock_Config();
	HAL_Delay(10);
	if(sys.watchdog)
	{
		user_main_printf("Exit low power mode\r\n");
		sys.power = 0;
		sys.pwordcount = 0;
	}		
	if(sys.power_rxflag)
	{
		user_main_printf("please enter password\r\n");
		sys.power_rxflag = 0;
	}
//	if(sys.lowpower)
//	{
//		user_main_printf("Exit low power mode\r\n");
//		sys.power = 0;
//		sys.pwordcount = 0;
//		if(sys.power_rxflag)
//		{
//			user_main_printf("please enter password\r\n");
//			sys.power_rxflag = 0;
//		}
//	}		
	memset(uart.Data,0,sizeof(uart.Data));
}

void i2cDetection(void)
{
	uint8_t sum=0;
	uint8_t txdata1[1]={0xE7},txdata2[2]={0xF3,0x2D};
	workmodel.i2cDevice = 0;
	sht20Init();
	while(HAL_I2C_Master_Transmit(&hi2c1,0x80,txdata1,1,1000) != HAL_OK)
	{
		 sum++;
		 if(sum>20)
		 {
			 workmodel.i2cDevice=0;
			 break;
		 }
	 }
	 if(HAL_I2C_Master_Transmit(&hi2c1,0x80,txdata1,1,1000) == HAL_OK)
	 {
		 workmodel.i2cDevice=1;
	   user_main_printf("Use Sensor is SHT20");
	 }
	 
	 if(workmodel.i2cDevice==0)
	 {	 
		sum=0;		 			 
		sht31Init();	 
		while(HAL_I2C_Master_Transmit(&hi2c1,0x88,txdata2,2,1000) != HAL_OK) 
		{
		 sum++;
		 if(sum>20)
		 {
			 workmodel.i2cDevice=0;
			 break;
		 }
		}
	  if(HAL_I2C_Master_Transmit(&hi2c1,0x88,txdata2,2,1000) == HAL_OK)
	  {
			 workmodel.i2cDevice=2;
			 user_main_printf("Use Sensor is SHT31");
	  }
   }
	 if(workmodel.i2cDevice==0)
	 {
//		 HAL_I2C_MspDeInit(&hi2c1);
		 
		 user_main_printf("No I2C device detected");
	 }	
}

uint16_t ADCModel(uint32_t channel)
{
	uint16_t measuredLevel = HW_AdcReadChannel( channel );
	float adc_mV= measuredLevel*sensor.batteryLevel_mV/4096;
	HAL_Delay(100);
	if(channel == ADC_CHANNEL_0)
		user_main_printf("adc_mV(1):%.2f",adc_mV);
	else if(channel == ADC_CHANNEL_1)
		user_main_printf("adc_mV(2):%.2f",adc_mV);
	else if(channel == ADC_CHANNEL_4)
		user_main_printf("adc_mV(3):%.2f",adc_mV);
	return adc_mV*100;
//	return adc_mV;
}

char* payLoadDeal(uint8_t model,char* payload)
{	
	memset(payload,0,strlen(payload));
	powerState(HAL_OK);
	if(model == model1)
	{
		sensor.batteryLevel_mV = getVoltage();
		user_main_printf("remaining battery = %d mv",sensor.batteryLevel_mV);
		sensor.temDs18b20_1 = DS18B20_GetTemp_SkipRom(1)*10;
		sensor.adc0 = ADCModel(ADC_CHANNEL_0)*10;
		
		i2cDetection();
		if(workmodel.i2cDevice == 1)
			sht20Data();
		else if(workmodel.i2cDevice == 2)
			sht31Data();
		
		sprintf(payload+strlen(payload), "%.4x", sensor.batteryLevel_mV);
		sprintf(payload+strlen(payload), "%.4x", sensor.temDs18b20_1);
		sprintf(payload+strlen(payload), "%.4x", sensor.adc0);
		sprintf(payload+strlen(payload), "%.2x", sensor.count);
		sprintf(payload+strlen(payload), "%.4x", sensor.temSHT);
		sprintf(payload+strlen(payload), "%.4x", sensor.humSHT);		
	}
	else if(model == model2)
	{
		sensor.batteryLevel_mV = getVoltage();
		user_main_printf("remaining battery =%d mv",sensor.batteryLevel_mV);
		sensor.temDs18b20_1 = DS18B20_GetTemp_SkipRom(1)*10;
		sensor.adc0 = ADCModel(ADC_CHANNEL_0);
		sensor.distance = LidarLite();
		if(sensor.distance == 4095)
		{
			sensor.distance = 0;
			sensor.distance = ULT_distance();
		}
		
		sprintf(payload+strlen(payload), "%.4x", sensor.batteryLevel_mV);
		sprintf(payload+strlen(payload), "%.4x", sensor.temDs18b20_1);
		sprintf(payload+strlen(payload), "%.4x", sensor.adc0);
		sprintf(payload+strlen(payload), "%.2x", sensor.count);
		sprintf(payload+strlen(payload), "%.4x", sensor.distance);
		strcat(payload,"FFFF");
	}
	else if(model == model3)
	{
		sensor.batteryLevel_mV = getVoltage();
		sensor.adc0 = ADCModel(ADC_CHANNEL_0);
		sensor.adc1 = ADCModel(ADC_CHANNEL_1);
		sensor.adc4 = ADCModel(ADC_CHANNEL_4);
		
		user_main_printf("remaining battery =%d mv",sensor.batteryLevel_mV);
		
		i2cDetection();
		if(workmodel.i2cDevice == 1)
			sht20Data();
		else if(workmodel.i2cDevice == 2)
			sht31Data();
		
		sprintf(payload+strlen(payload), "%.4x", sensor.adc0);
		sprintf(payload+strlen(payload), "%.4x", sensor.adc1);
		sprintf(payload+strlen(payload), "%.4x", sensor.adc4);
		sprintf(payload+strlen(payload), "%.2x", sensor.count);
		sprintf(payload+strlen(payload), "%.4x", sensor.temSHT);
		sprintf(payload+strlen(payload), "%.4x", sensor.humSHT);
		sprintf(payload+strlen(payload), "%.2x", sensor.batteryLevel_mV/100);
	}
	else if(model == model4)
	{
		sensor.batteryLevel_mV = getVoltage();
		user_main_printf("remaining battery =%d mv",sensor.batteryLevel_mV);
		sensor.temDs18b20_1 = DS18B20_GetTemp_SkipRom(1)*10;
		sensor.temDs18b20_2 = DS18B20_GetTemp_SkipRom(2)*10;
		sensor.temDs18b20_3 = DS18B20_GetTemp_SkipRom(3)*10;
		sensor.adc0 = ADCModel(ADC_CHANNEL_0);
		
		sprintf(payload+strlen(payload), "%.4x", sensor.batteryLevel_mV);
		sprintf(payload+strlen(payload), "%.4x", sensor.temDs18b20_1);
		sprintf(payload+strlen(payload), "%.4x", sensor.adc0);
		sprintf(payload+strlen(payload), "%.2x", sensor.count);
		sprintf(payload+strlen(payload), "%.4x", sensor.temDs18b20_2);
		sprintf(payload+strlen(payload), "%.4x", sensor.temDs18b20_3);
	}
	else if(model == model5)
	{
		if(num == 0)
		{
			at_weight_reset((char*)HAL_OK);
			num =1;
		}
		
		sensor.batteryLevel_mV = getVoltage();
		user_main_printf("remaining battery =%d mv",sensor.batteryLevel_mV);
		sensor.temDs18b20_1 = DS18B20_GetTemp_SkipRom(1)*10;
		sensor.adc0 = ADCModel(ADC_CHANNEL_0)*10;
		int32_t Weight = Get_Weight();
		user_main_printf("Weight is %d g\r\n",Weight);
		sprintf(payload+strlen(payload), "%.4x", sensor.batteryLevel_mV);
		sprintf(payload+strlen(payload), "%.4x", sensor.temDs18b20_1);
		sprintf(payload+strlen(payload), "%.4x", sensor.adc0);
		sprintf(payload+strlen(payload), "%.2x", sensor.count);
		sprintf(payload+strlen(payload), "%.4x", Weight);
		strcat(payload,"FFFF");		
	}
	
	user_main_debug("payload:%s\r\n",(payload));
	HAL_Delay(100+workmodel.power_time);
	powerState(!HAL_OK);
	
	return payload;
}
void ledState(uint8_t state,uint16_t time)
{
	 HAL_TIM_Base_Stop_IT(&htim7);
	 GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
	 
	/*Configure GPIO pin : PA8 */
	if(state == HAL_OK)	 
	{
		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);				
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
		
		sys.led = 1;
		MX_TIM7_Init(time);
		__HAL_TIM_CLEAR_FLAG(&htim7,TIM_FLAG_UPDATE);
		HAL_TIM_Base_Start_IT(&htim7);
	}
	else if(state != HAL_OK)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		sys.led = 0;
	}
}

void RX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
}

void EX_GPIO_Init(uint8_t state)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	  /*Configure GPIO pin : PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
	if(state == 0)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
	}
	else if(state == 1)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
	}
	else if(state == 2)
	{
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
	}
	else if(state == 3)
  {
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
	}
	
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	if(state != 0)
	{
		/* EXTI interrupt init*/
		HAL_NVIC_SetPriority(EXTI4_15_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	}
}

void MyRtcInit(void)
{
	user_main_printf("Wait for calibration time");
	
	dataTransmission(AT CCLK"?\n");
	
	char* colon = strchr(uart.Data,':');
	char* comma = strchr(uart.Data,',');
	char* carriage = strchr(uart.Data,'O');
	
	char datebuff[10] = {0};
	char timebuff[15] = {0};
	char snapbuff[3] = {0};
	
	int year = 0,mounth = 0,date = 0;
	int hour = 0,min = 0,		sec = 0 ;
	int jet_lag = 0;
	
	for(int i = 0;i<(comma - colon)-1;i++)
	{
		datebuff[i] = uart.Data[(comma - colon) +i-1];
	}
		
	for(int i=0;i<3;i++)
	{
		memset(snapbuff,0,sizeof(snapbuff));
		if(i == 0)
		{
			snapbuff[0]=datebuff[0];
			snapbuff[1]=datebuff[1];
			year = atoi(snapbuff);
		}
		else if(i == 1)
		{
			snapbuff[0]=datebuff[3];
			snapbuff[1]=datebuff[4];
			mounth = atoi(snapbuff);
		}
		else if(i == 2)
		{
			snapbuff[0]=datebuff[6];
			snapbuff[1]=datebuff[7];
			date = atoi(snapbuff);
		}
	}

	for(int i = 0;i<(carriage - comma)-5;i++)
	{
		timebuff[i] = uart.Data[(carriage - comma) +i+1];
	}
		
	for(int i=0;i<4;i++)
	{
		memset(snapbuff,0,sizeof(snapbuff));
		if(i == 0)
		{
			snapbuff[0]=timebuff[0];
			snapbuff[1]=timebuff[1];
			hour = atoi(snapbuff);
		}
		else if(i == 1)
		{
			snapbuff[0]=timebuff[3];
			snapbuff[1]=timebuff[4];
			min = atoi(snapbuff);
		}
		else if(i == 2)
		{
			snapbuff[0]=timebuff[6];
			snapbuff[1]=timebuff[7];
			sec = atoi(snapbuff);
		}
		else if(i == 3)
		{
			char* zheng = strchr(timebuff,'+');
			char* fu = strchr(timebuff,'-');
			if(zheng != NULL)
				sprintf(snapbuff+strlen(snapbuff), "%s", &timebuff[zheng-timebuff +1]);
			else if(fu != NULL)
				sprintf(snapbuff+strlen(snapbuff), "%s", &timebuff[fu-timebuff +1]);
			
			jet_lag = atoi(snapbuff);
			if(jet_lag > 0)
			{
				if(zheng != NULL)
				{
					hour = hour + jet_lag/4;
					if(hour >= 24)
						hour = hour - 24;
				}
				else if(fu != NULL)
				{
					hour = hour - jet_lag/4;
					if(hour < 0)
						hour =  24 + hour ;
				}
			}
		}		
	}
	
	RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
    
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = (hour )%10+(hour )/10*16;
  sTime.Minutes = (min )%10+(min )/10*16;
  sTime.Seconds = (sec )%10+(sec )/10*16;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = (mounth )%10+(mounth )/10*16;;
  sDate.Date = (date )%10+(date )/10*16;;
  sDate.Year = (year )%10+(year )/10*16;;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
	user_main_printf("Calibration time completed");
	HAL_IWDG_Refresh(&hiwdg);
	My_AlarmInit(18,ALARMB);
}

void powerState(uint8_t state)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
	
	GPIO_InitStruct.Pin = PWR_OUT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	/*Configure GPIO pin : PWR_OUT_PIN */
	if(state == HAL_OK)	 
	{					
		HAL_GPIO_WritePin(PWR_OUT_PORT,PWR_OUT_PIN,GPIO_PIN_RESET);		
	}
	else if(state != HAL_OK)	
	{
		HAL_GPIO_WritePin(PWR_OUT_PORT,PWR_OUT_PIN,GPIO_PIN_SET);
	}
	
	HAL_GPIO_Init(PWR_OUT_PORT, &GPIO_InitStruct);	
}
