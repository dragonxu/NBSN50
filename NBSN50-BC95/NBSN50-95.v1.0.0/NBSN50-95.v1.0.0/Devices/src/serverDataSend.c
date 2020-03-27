#include "serverDataSend.h"

STATUS dataSendPut(char* payload)
{
	uint8_t errorNum=0;
	uint8_t unread=0;
	uart.issued =0;
	char address[25]={0};
	char uri[65]={0};    
	uint32_t add  = FLASH_USER_COUNT_SERVADDR_ADDR;
  uint32_t temp = flashRead(add);	
	while(temp != 0x0)
	{
		if(temp == 0x3A)
			temp = 0x2C;
		address[strlen(address)]=(char)temp;
		add = add + 4;
		temp = flashRead(add);
	}

	add = FLASH_USER_COUNT_URI_ADDR;
  temp = flashRead(add);	
	while(temp != 0x0 && add != FLASH_USER_COUNT_URI_ADDR + FLASH_PAGE_SIZE*3)
	{	
		uri[strlen(uri)]=(char)temp;
		add = add + 4;
		temp = flashRead(add);
	}
	
	putMsg(payload,uri,sensor.data);																							
	uint16_t payloadLen = strlen(payload)/2;
	memset(payload,0,strlen(payload));																
																																		
	strcat(payload,AT NSOST "=");																			
	sprintf(payload+strlen(payload), "%c", workmodel.socket);					
	strcat(payload, ",");
	strcat(payload,address);																					
	strcat(payload, ",");																							
	sprintf(payload+strlen(payload), "%d", payloadLen);								
	strcat(payload, ",");																							
	putMsg(payload,uri,sensor.data);	
	strcat(payload, ",1\n");
	user_main_debug("put-payload:%s",payload);
	
	
	do{
     user_main_printf("Waiting to send put-data.");
     dataTransmission(payload);
		 HAL_Delay(2000);
		 errorNum++;
		 if(errorNum>10)
		 {
			 user_main_printf("Put-Data uploaded failed.");
			 return fail;
		 }
    }while(!uart.issued);
	
	do{
			memset(payload,0,strlen(payload));
			strcat(payload,AT NSORF"=");
			payload[strlen(payload)]=workmodel.socket;
			strcat(payload,",512\n");
			dataTransmission(payload);
			
			memset(payload,0,strlen(payload));
			unread = receiveNBIOT(payload,uart.Data);
//			if(ackMsg((payload[0] - 0x30)<<4) == ackMessage)
//			{
//				user_main_printf("Put-Data uploaded successfully.");
//				ledState(HAL_OK,500);
//			}
//			else if(ackMsg((payload[0] - 0x30)<<4) == conMessage)
//			{
//				feedBackSub(payload);
//			}
//			else
//				return fail;
		
			if(ackMsg((payload[0] - 0x30)<<4) == ackMessage)
			{
				user_main_printf("Put-Data uploaded successfully.");
				ledState(HAL_OK,500);
			}
			else if(ackMsg((payload[0] - 0x30)<<4) == conMessage)
			{
				feedBackSub(payload);
			}
			else
			{
				user_main_printf("Put-Data uploaded successfully.");
			}
		}	while(unread);
		
	return success;
}

STATUS dataSendGet(char* payload)
{
	uint8_t errorNum=0;
	uint8_t unread =0;
	uart.issued =0;
	char address[25]={0};
	char uri[65]={0};    
	uint32_t add  = FLASH_USER_COUNT_SERVADDR_ADDR;
  uint32_t temp = flashRead(add);	
	while(temp != 0x0)
	{
		if(temp == 0x3A)
			temp = 0x2C;
		address[strlen(address)]=(char)temp;
		add = add + 4;
		temp = flashRead(add);
	}

	add = FLASH_USER_COUNT_URI_ADDR;
  temp = flashRead(add);	
	while(temp != 0x0 && add != FLASH_USER_COUNT_URI_ADDR + FLASH_PAGE_SIZE*3)
	{	
		uri[strlen(uri)]=(char)temp;
		add = add + 4;
		temp = flashRead(add);
	}

	GetMsg(payload,uri);																							
	uint16_t payloadLen = strlen(payload)/2;
	memset(payload,0,strlen(payload));																
																																		
	strcat(payload,AT NSOST "=");																			
	sprintf(payload+strlen(payload), "%c", workmodel.socket);					
	strcat(payload, ",");
	strcat(payload,address);																					
	strcat(payload, ",");																							
	sprintf(payload+strlen(payload), "%d", payloadLen);								
	strcat(payload, ",");																							
	GetMsg(payload,uri);	
	strcat(payload, ",1\n");
	user_main_debug("get-payload:%s",payload);
		
	do{
     user_main_printf("Waiting to send get-data.");
     dataTransmission(payload);
		 HAL_Delay(2000);
		 errorNum++;
		 if(errorNum>10)
		 {
			user_main_printf("Get-Data downlink failed.");
			return fail;
		 }
    }while(!uart.issued);
	
	do{
			memset(payload,0,strlen(payload));
			strcat(payload,AT NSORF"=");
			payload[strlen(payload)]=workmodel.socket;
			strcat(payload,",512\n");
			dataTransmission(payload);
			
			memset(payload,0,strlen(payload));
			unread = receiveNBIOT(payload,uart.Data);
			if(ackMsg((payload[0] - 0x30)<<4) == ackMessage)
			{
				user_main_printf("Get-Data downlink successfully.");
			}
			else if(ackMsg((payload[0] - 0x30)<<4) == conMessage)
			{
				feedBackSub(payload);
			}
			else
			{
				user_main_printf("Get-Data downlink successfully.");
			}
		}	while(unread);	
	return success;
}

STATUS sockCreat(void)
{
	uint8_t errorNum=0;
	while(strstr(dataTransmission(AT NSOCR UDP),"ERROR") != NULL)
	{
			HAL_Delay(1000);
			errorNum ++;
			if(errorNum > 5)
			{
				user_main_printf("Failed to create socket.");
				return fail;
			}
  }
	
	char *pch = strchr(uart.Data,'O');
	workmodel.socket = uart.Data[(pch-uart.Data)-5];
	
	errorNum = 0;
	while(strstr(dataTransmission(AT CSQ"\n"),"+CSQ:99,99") != NULL)
	{
      HAL_Delay(1000);
      errorNum ++;
      if(errorNum > 30)
			{
				user_main_printf("\r\nNo signal.\r\n");
        return fail; 
			}
  }
	user_main_printf("NB-IoT Signal Strength ");
	user_main_printf("Protocol in Used: Coap");
	
	char singalBuff[5]={0}; 	
	char *pch1 = strchr(uart.Data,':');
	char *pch2 = strchr(uart.Data,',');
	for(int i =0;i<(pch2 - pch1)-1;i++)
		sprintf(singalBuff+strlen(singalBuff), "%c", uart.Data[7+i]);
	
	int singal = atoi(singalBuff);
	sprintf(sensor.data+strlen(sensor.data), "%.2x", singal);
	
	user_main_printf("Signal Strength:%s\r\n",singalBuff);
	return success;
}

STATUS sockClosd(void)
{
	char payload[60]={0};
	char address[25]={0};
	uint32_t add  = FLASH_USER_COUNT_SERVADDR_ADDR;
  uint32_t temp = flashRead(add);	
	while(temp != 0x0)
	{
		if(temp == 0x3A)
			temp = 0x2C;
		address[strlen(address)]=(char)temp;
		add = add + 4;
		temp = flashRead(add);
	}	
	
	strcat(payload,AT NSOSTF "=");																			
	sprintf(payload+strlen(payload), "%c", workmodel.socket);					
	strcat(payload, ",");
	strcat(payload,address);
	strcat(payload, ",0x200,1,FF");																															
	strcat(payload, "\n");																							
	dataTransmission(payload);				
	
	memset(payload,0,sizeof (payload));
	strcat(payload,"AT+NSOCL=1");
	payload[strlen(payload)-1] = workmodel.socket;
	strcat(payload,"\n");
	dataTransmission(payload);
	
	return success;
}
