#include "at.h"

static uint8_t keep = 0;

ATEerror_t ATInsPro(char* atdata)
{
	uint8_t i = 0;
	rtrim(atdata);
	if(strcmp(atdata,AT) == 0)
	{
		user_main_printf("%s",ATError_description[AT_OK]);
		return AT_OK;
	}
	else if(strcmp(atdata,AT"?") == 0)
	{
		at_que(atdata);
		return AT_OK;
	}
	
	while(strstr(atdata,ATCommand[i].string) == NULL)
	{
		i++;
		if(i>(sizeof(ATCommand) / sizeof(struct ATCommand_s))-1)
		{
			if(workmodel.model != model6)
				user_main_printf("%s",ATError_description[AT_ERROR]);
			return AT_ERROR;
		}
	}

	if(strstr(atdata,"=?"))
	{	
		printf("%s\r\n",ATError_description[ATCommand[i].get(atdata)]);
	}
	else if(strstr(atdata,"="))
	{
		printf("%s\r\n",ATError_description[ATCommand[i].set(atdata)]);
	}
	else if(strstr(atdata,"?"))
	{
		user_main_printf("%s",ATCommand[i].help_string);
		printf("%s\r\n",ATError_description[AT_OK]);
	}
	else 
	{
		ATCommand[i].run(atdata);
	}
	return AT_OK;
}

/************** 			AT			 **************/
ATEerror_t at_return_error(const char *param)
{
  return AT_ERROR;
}

ATEerror_t at_que(const char *param)
{
	user_main_printf("\r\nAT+<CMD>?        : Help on <CMD>\r\n"
              "AT+<CMD>         : Run <CMD>\r\n"
              "AT+<CMD>=<value> : Set the value\r\n"
              "AT+<CMD>=?       : Get the value\r\n");
	for (int i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
	{
		printf("%s\r\n",ATCommand[i].help_string);
	}	
	printf("%s\r\n",ATError_description[AT_OK]);
  return AT_OK;
}

/************** 			ATZ			 **************/
ATEerror_t at_reset_run(const char *param)
{
	NVIC_SystemReset();
  return AT_OK;
}

/************** 			AT+CFGMOD			 **************/
ATEerror_t at_mod_set(const char *param)
{
	char* pos = strchr(param,'=');
	workmodel.model = param[(pos-param)+1];
	flashErase(FLASH_USER_COUNT_WORKMODEl_ADDR,PAGE1);
	flashWrite(FLASH_USER_COUNT_WORKMODEl_ADDR, &workmodel.model, 1);
	if(workmodel.model == model1)
	{
		HAL_RTC_DeactivateAlarm(&hrtc,RTC_ALARM_B);
		user_main_printf("\r\nDefault mode\r\n");
	}
	else if(workmodel.model == model2)
	{
		user_main_printf("\r\nDistance mode\r\n");
	}
	else if(workmodel.model == model3)
	{
		user_main_printf("\r\n3xADC + I2C mode\r\n");
	}
	else if(workmodel.model == model4)
	{
		user_main_printf("\r\n3xDS18B20 mode\r\n");
	}
	else if(workmodel.model == model5)
	{
		user_main_printf("\r\nweiget mode\r\n");
	}
	else if(workmodel.model == model6)
	{
		user_main_printf("\r\nNB configuration mode\r\n");
	}
	else if(workmodel.model == model0)
	{
		user_main_printf("\r\nConfiguration mode\r\n");
	}
	else
	{
		user_main_printf("%s",ATError_description[AT_PARAM_ERROR]);
		return AT_PARAM_ERROR;
	}
	
	if(workmodel.model != model0 && workmodel.model != model6)
		workmodel.uplink = 1;
	else
		workmodel.uplink = 0;
	
  return AT_OK;
}

ATEerror_t at_mod_get(const char *param)
{
	uint32_t add = FLASH_USER_COUNT_WORKMODEl_ADDR;
  uint32_t temp = flashRead(add);
	if(temp == model0|| temp == model1 || temp == model2|| temp == model3|| temp == model4||temp == model5||temp == model6)
		workmodel.model = temp;
	else
		workmodel.model = model0;
	
	if(keep)
		printf("AT+CFGMOD=");
	printf("%c\r\n",workmodel.model);
	
  return AT_OK;
}

/************** 			AT+PWORD			 **************/
ATEerror_t at_pword_set(const char *param)
{
	char* pos = strchr(param,'=');
	if(strlen(param) - (pos-param)-1 >9)
	{		
		user_main_printf("%s",ATError_description[AT_PARAM_ERROR]);
		return AT_PARAM_ERROR;
	}
	
	memset(sys.pword,0,sizeof(sys.pword));
	strncpy(sys.pword, param+(pos-param)+1, strlen(param) - (pos-param)-1);
	flashErase(FLASH_USER_COUNT_PWORD_ADDR,PAGE1);
	flashWrite(FLASH_USER_COUNT_PWORD_ADDR,(uint8_t*)sys.pword, strlen(sys.pword));
  return AT_OK;
}

ATEerror_t at_pword_get(const char *param)
{	
	uint32_t add = FLASH_USER_COUNT_PWORD_ADDR;
  uint32_t temp = flashRead(add);
	if(keep)
		printf("AT+PWORD=");
	if(temp!=0x0)
	{
		int i =0;
		memset(sys.pword,0,sizeof(sys.pword));
		while(temp!=0x0)
		{
			sys.pword[i++] = (char)temp;
			add = add + 4;
			temp = flashRead(add);
		}
	}
	printf("%s\r\n",sys.pword);
	return AT_OK;
}

/************** 			AT+FDR		 **************/
ATEerror_t at_fdr_run(const char *param)
{
	flashErase(FLASH_USER_COUNT_START_ADDR,(FLASH_USER_COUNT_END_ADDR - FLASH_USER_COUNT_START_ADDR) / FLASH_PAGE_SIZE);
	NVIC_SystemReset();
  return AT_OK;
}

/************** 			AT+CFG		 **************/
ATEerror_t at_cfg_run(const char *param)
{
	keep = 1;
	user_main_printf();
	for (int i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
	{
		ATCommand[i].get(param);
	}	
	keep = 0;
	if(!sys.start)
		printf("\r\n%s",ATError_description[AT_OK]);
  return AT_OK;
}

/************** 			AT+SERVADDR		 **************/
ATEerror_t at_servaddr_get(const char *param)
{
	uint32_t add = FLASH_USER_COUNT_SERVADDR_ADDR;
  uint32_t temp = flashRead(add);
	if(keep)
		printf("AT+SERVADDR=");
	while(temp!=0x0)
	{
		printf("%c",(char)temp);
		add = add + 4;
		temp = flashRead(add);
	}
	printf("\r\n");
  return AT_OK;
}

ATEerror_t at_servaddr_set(const char *param)
{
	char* pos = strchr(param,'=');
	if(strlen(param) - (pos-param)-1 >23)
	{
		user_main_printf("%s",ATError_description[AT_PARAM_ERROR]);
		return AT_PARAM_ERROR;
	}
	
	uint32_t add = FLASH_USER_COUNT_SERVADDR_ADDR;
	flashErase(add,PAGE1);
	flashWrite(add,(uint8_t*)(param+(pos-param)+1), strlen(param) - (pos-param)-1);
  return AT_OK;
}

/************** 			AT+URI		 **************/
ATEerror_t at_uri_get(const char *param)
{
	uint32_t add = FLASH_USER_COUNT_URI_ADDR;
  uint32_t temp = flashRead(add);
	if(keep)
		printf("AT+URI=");
	while(temp!=0x0)
	{
		printf("%c",(char)temp);
		add = add + 4;
		temp = flashRead(add);
	}
	printf("\r\n");
  return AT_OK;
}

ATEerror_t at_uri_set(const char *param)
{
	char* pos = strchr(param,'=');
	if(strlen(param) - (pos-param)-1 >64)
	{
		user_main_printf("%s",ATError_description[AT_PARAM_ERROR]);
		return AT_PARAM_ERROR;
	}
	
	uint32_t add = FLASH_USER_COUNT_URI_ADDR;
	flashErase(add,PAGE2);
	flashWrite(add,(uint8_t*)(param+(pos-param)+1), strlen(param) - (pos-param)-1);
  return AT_OK;
}

/************** 			AT+TDC		 **************/
ATEerror_t at_tdc_get(const char *param)
{
	uint32_t add = FLASH_USER_COUNT_TDC_ADDR;
  uint32_t temp = flashRead(add);
	uint8_t dataTdc[10] = {0};
	uint8_t i = 0;
	while(temp!=0x0)
	{
		dataTdc[i] = (char)temp;
		add = add + 4;
		i++;
		temp = flashRead(add);
	}
	uint16_t time = atoi((char*)dataTdc);
	if(time != 0)
		workmodel.Tdc = time;
	if(keep)
		printf("AT+TDC=");
	printf("%d\r\n",workmodel.Tdc);
  return AT_OK;
}

ATEerror_t at_tdc_set(const char *param)
{
	char* pos = strchr(param,'=');
	workmodel.Tdc = atoi((param+(pos-param)+1));
	
	uint32_t add = FLASH_USER_COUNT_TDC_ADDR;
	flashErase(add,PAGE1);
	flashWrite(add,(uint8_t*)(param+(pos-param)+1), strlen(param) - (pos-param)-1);
  return AT_OK;
}

/************** 			AT+INMOD		 **************/
ATEerror_t at_inmod_get(const char *param)
{
	uint32_t add = FLASH_USER_COUNT_INMOD_ADDR;
  uint32_t temp = flashRead(add);

	workmodel.inmod = (char)temp;
	if(workmodel.inmod != '0' &&workmodel.inmod != '1' &&workmodel.inmod != '2' &&workmodel.inmod != '3')
	{
		workmodel.inmod = '0';
	}
	
	if(keep)
		printf("AT+INMOD=");
	printf("%c\r\n",workmodel.inmod);
	
  return AT_OK;
}

ATEerror_t at_inmod_set(const char *param)
{
	char* pos = strchr(param,'=');
	workmodel.inmod = param[(pos-param)+1];
	if(workmodel.inmod != '0' &&workmodel.inmod != '1' &&workmodel.inmod != '2' &&workmodel.inmod != '3')
	{
		user_main_printf("%s",ATError_description[AT_PARAM_ERROR]);
		return AT_PARAM_ERROR;
	}
	
	uint32_t add = FLASH_USER_COUNT_INMOD_ADDR;
	flashErase(add,PAGE1);
	flashWrite(add, &workmodel.inmod, 1);
  return AT_OK;
}

/************** 			AT+5VT		 **************/
ATEerror_t at_5vt_get(const char *param)
{
	uint32_t add = FLASH_USER_COUNT_5VT_ADDR;
  uint32_t temp = flashRead(add);

	workmodel.power_time = temp;
	if(workmodel.power_time == 0)
	{
		workmodel.power_time = 0;
	}
	
	if(keep)
		printf("AT+5VT=");
	printf("%d\r\n",workmodel.power_time);
  return AT_OK;
}

ATEerror_t at_5vt_set(const char *param)
{
	char* pos = strchr(param,'=');
	workmodel.power_time = atoi((param+(pos-param)+1));
	
	uint32_t add = FLASH_USER_COUNT_5VT_ADDR;
	flashErase(add,PAGE1);
	
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, add, workmodel.power_time);
	HAL_FLASH_Lock();
	
	user_main_printf("%d",workmodel.power_time);
  return AT_OK;
}
/************** 			AT+WEIGHT		 **************/

ATEerror_t at_weight_reset(const char *param)
{
//	powerState(HAL_OK);
	WEIGHT_SCK_Init();
	WEIGHT_DOUT_Init();
	Get_Maopi();
  Get_Maopi();
//	powerState(!HAL_OK);
	
	return AT_OK;	
}

ATEerror_t at_weight_get(const char *param)
{
	uint32_t add = FLASH_USER_COUNT_GapValue_ADDR;
  uint32_t temp = flashRead(add);

	sensor.GapValue = ((float)temp/10);
	if(sensor.GapValue == 0)
	{
		sensor.GapValue = 400.0;
	}
	if(keep)
		printf("AT+WEIGRE=");
	printf("%0.1f",sensor.GapValue);
	
	return AT_OK;
}

ATEerror_t at_weight_GapValue_set(const char *param)
{
	uint16_t gapvalue_a = 0;
	uint8_t  gapvalue_b = 0;
	
	char* pos = strchr(param,'=');
	char* pos1 = strchr(param,'.');
	if(pos1 == NULL)
	{
		gapvalue_a = atoi((param+(pos-param)+1));
		gapvalue_b = 0;
	}
	else
	{
		char inter[10] = {0};
		for(int i=0;i<pos1-pos-1;i++)
			inter[i] = param[pos-param+i+1];		
		
		gapvalue_a = atoi(inter);
		gapvalue_b = atoi((param+(pos1-param)+1));
	}
	if(gapvalue_b>=10)
  {
    return AT_PARAM_ERROR;
  }	
	sensor.GapValue = gapvalue_a+((float)gapvalue_b/10.0);
	Get_Weight();
	
	uint32_t add = FLASH_USER_COUNT_GapValue_ADDR;
	flashErase(add,PAGE1);
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, add, sensor.GapValue*10);
	HAL_FLASH_Lock();
	
  return AT_OK;	
}

ATEerror_t at_weight_GapValue_get(const char *param)
{
	if(!keep)
		user_main_printf("%0.1f g\r\n",(float)Get_Weight());
	
  return AT_OK;		
}
