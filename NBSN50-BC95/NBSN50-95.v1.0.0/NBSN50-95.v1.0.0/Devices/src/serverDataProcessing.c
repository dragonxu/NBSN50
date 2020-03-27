#include "serverDataProcessing.h"

//4445381D0A0000016101612AB102FF3031203030203030203145
void feedBackSub(char* param)
{	
	uint8_t errorNum=0;
	uart.issued =0;
	char dataCom[50]={0};
	for(int i=0;i<4;i++)
		sprintf(dataCom+strlen(dataCom), "%c", param[i+4]);
	
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
	
	strcat(payload,AT NSOST "=");																			
	sprintf(payload+strlen(payload), "%c", workmodel.socket);					
	strcat(payload, ",");
	strcat(payload,address);
	strcat(payload, ",");
	uint8_t dataCom_len = (strlen(dataCom)+4+8)/2;
	sprintf(payload+strlen(payload), "%d", dataCom_len);	
	strcat(payload, ",6400");
	strcat(payload,dataCom);
	strcat(payload, "0A000001,1\n");		
	user_main_debug("payload:%s",payload);
	
	do{
     dataTransmission(payload);
		 HAL_Delay(2000);
		 errorNum++;
		 if(errorNum>10)
			break;
    }while(!uart.issued);

	memset(payload,0,sizeof(payload));
	memset(dataCom,0,sizeof(dataCom));
	int i = 0;
	while(1)
	{
		if(param[i*2] == 'F' && param[i*2+1] == 'F')
			break;
		
		i++;
		if(i>200)
			return;
	}

	
	if((strlen(param)-i)/2>20)	//shu ju bu dui
		return;
	
	for(;i<strlen(param)-1;i++)
		sprintf(payload+strlen(payload), "%c", param[i+2]);	

	strTohex(dataCom,payload);
	user_main_printf("Rxtime out or Received:%s",dataCom);	
	
	if(dataCom[0] == '0' && dataCom[1] == '1' && strlen(dataCom) == 8)
	{
		char data[8]={0};
		memcpy(data,(dataCom)+2,6);
		workmodel.Tdc = hexToint(data);
	}
	else if(dataCom[0] == '0' && dataCom[1] == '2' && strlen(dataCom) == 4)
	{
		if(dataCom[2] == 'F' && dataCom[3] == 'F')
			NVIC_SystemReset();
	}
}

int hexToint(char* s)  
{  
    int i;  
    int n = 0;  
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))   
			i = 2;   
    else  
			i = 0;  
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)  
    {  
			if (tolower(s[i]) > '9')  
				n = 16 * n + (10 + tolower(s[i]) - 'a');  
			else  
				n = 16 * n + (tolower(s[i]) - '0');  
    }  
    return n;  
}

char *rtrim(char *str)
{
	if (str == NULL || *str == '\0' || *str == '\r' || *str == '\n')
	{
		return str;
	}
 
	int len = strlen(str);
	char *p = str + len - 1;
	while (p >= str  && isspace(*p))
	{
		*p = '\0';
		--p;
	}
 
	return str;
}
