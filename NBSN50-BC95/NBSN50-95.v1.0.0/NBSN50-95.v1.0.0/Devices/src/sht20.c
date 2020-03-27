#include "sht20.h"

void sht20Init(void)
{
	MX_I2C1_Init();
}

void sht20Data(void)
{
	float hum=0.0;
	float tem=0.0;
	uint8_t rxdata[6]={0};
	uint8_t errorHum = 0,errorTem = 0;
	uint8_t SHT2X_Hum_Buffer[1]={0xf5};
	uint8_t SHT2X_Tem_Buffer[1]={0xf3};
	uint16_t AD_code = 0,sum1 = 0,sum2=0;
	sht20Init();
	
	while(HAL_I2C_Master_Transmit(&hi2c1,0x80,SHT2X_Hum_Buffer,strlen((char*)SHT2X_Hum_Buffer),1000) != HAL_OK)
	{
		sum1++;
		if(sum1>=500)
		{
			break;
		}
		if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
		{}
	}
	
	while(HAL_I2C_Master_Receive(&hi2c1,0x81,rxdata,2,1000) != HAL_OK)
	{
		sum2++;
		if(sum2>=3000)       // The minimum time required for temperature conversion, the timeout jumps out of the loop
		{
			break;
		}
		if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
		{}
	}
		
	AD_code=(rxdata[0]<<8)+rxdata[1];
	AD_code &=~0x0003;   //14bit
	hum=AD_code*125.0/65536-6;
	if(hum>100.0)
	{
		hum=100.0;
	}
  if(hum<0.0)
	{
		errorHum++;
		if(errorHum==2)
		{
			sht20Init();
		}
		if(errorHum==3)
		{
			NVIC_SystemReset();
		}
	}
	
	memset(rxdata,0,sizeof(rxdata));
	while(HAL_I2C_Master_Transmit(&hi2c1,0x80,SHT2X_Tem_Buffer,strlen((char*)SHT2X_Tem_Buffer),1000) != HAL_OK)
	{
		sum1++;
		if(sum1>=500)
		{
			break;
		}
		if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
		{}
	}
	
	while(HAL_I2C_Master_Receive(&hi2c1,0x81,rxdata,2,1000) != HAL_OK)
	{
		sum2++;
		if(sum2>=3000)       // The minimum time required for temperature conversion, the timeout jumps out of the loop
		{
			break;
		}
		if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
		{}
	}
	
	AD_code=(rxdata[0]<<8)+rxdata[1];
	AD_code &=~0x0003;   //14bit
	tem=AD_code*175.72/65536-46.85;
	if((tem<-40.0)||(tem>125.0))
	{
		errorTem++;
		if(errorTem==2)
		{
			sht20Init();
		}
		if(errorTem==3)
		{
			NVIC_SystemReset();
		}
	}

  user_main_printf("Humidity =%.2f %%rh",hum);
	user_main_printf("tem =%.2f °„C",tem);	
	
	sensor.temSHT = tem*10;
	sensor.humSHT = hum*10;
}
