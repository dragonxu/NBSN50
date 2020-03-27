#include "NBIOT_PortSend.h"

char* dataTransmission(char* buff)
{
	uint16_t errnum = 0;
	uart.receiveFlag = 0;
	memset(uart.Data,0,sizeof(uart.Data));
	HAL_UART_Transmit_DMA(&hlpuart1,(uint8_t*)buff,strlen(buff));
	while(uart.receiveFlag == 0)
	{
		HAL_Delay(1);
		errnum++;
		if(errnum>2000)
			break;
	}
	user_main_info("%s",uart.Data);
	return uart.Data;
}

void nbiotInit(void)
{
	uint8_t errorNum = 0;
	while(strstr(dataTransmission(AT"\n"), ATError_description[AT_OK]) == NULL)
	{
		HAL_Delay(100);
		errorNum++;
		if(errorNum > 6)
		{
			user_main_printf("\r\nNBIOT is not responding, trying to restart\r\n");
			dataTransmission(AT NRB"\n");
			while(strstr(uart.Data,"REBOOTING")!=NULL && uart.Data[strlen(uart.Data)-2]=='O' && uart.Data[strlen(uart.Data)-1]=='K');
			errorNum = 0;
		}
	}
	
	errorNum = 0;HAL_Delay(100);	
	while(strstr(dataTransmission(AT CFUN"?\n"),"+CFUN:1") == NULL)
	{
		dataTransmission(AT CFUN"=1\n");
		user_main_printf("\r\nSet the UE function...\r\n");
		HAL_Delay(100);
		errorNum++;
		if(errorNum > 6)
		{
			user_main_printf("\r\nFailed to set UE function, trying to restart\r\n");
			dataTransmission(AT NRB"\n");
			while(strstr(uart.Data,"REBOOTING")!=NULL && uart.Data[strlen(uart.Data)-2]=='O' && uart.Data[strlen(uart.Data)-1]=='K');
			errorNum = 0;
		}
	}user_main_printf("Set the UE function successfully.");
		
	errorNum = 0;HAL_Delay(100);
	while(strstr(dataTransmission(AT CGATT"?\n"),"+CGATT:1") == NULL)
	{
		if(errorNum == 0)
			dataTransmission(AT CGATT"=1\n");
		user_main_printf("\r\nTrying to register to the network...\r\n");
		HAL_Delay(3000);
		errorNum++;
		if(errorNum > 100)
		{
			user_main_printf("\r\nFailed to register to the network, trying to restart and reconnect to the network after 1 hour\r\n");
			dataTransmission(AT NRB"\n");
			while(strstr(uart.Data,"REBOOTING")!=NULL && uart.Data[strlen(uart.Data)-2]=='O' && uart.Data[strlen(uart.Data)-1]=='K');
			errorNum = 0;
			workmodel.nb_resart = 1;
			My_AlarmInit(3600,ALARMA);
			//Go online again in 1 hour
			while(workmodel.nb_resart && sys.power_rxflag)
			{
				lowPower();
				workmodel.nb_resart = 0;
			}
    }
	}user_main_printf("Register to the network.");
	
	dataTransmission(AT CPSMS "=1,,,01000001,00000000\n");	//Closed T3324
	
	user_main_printf("NBIOT initialized successfully.");
	
	ledState(HAL_OK,4000);
}
