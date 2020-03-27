#include "ultrasound.h"

void GPIO_ULT_INPUT_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = ULT_Echo_PIN;
	GPIO_InitStruct.Mode =GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  HAL_GPIO_Init( ULT_Echo_PORT, &GPIO_InitStruct);
}

void GPIO_ULT_OUTPUT_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	ULT_TRIG_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = ULT_TRIG_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  HAL_GPIO_Init( ULT_TRIG_PORT,&GPIO_InitStruct);
}

uint16_t ULT_distance(void)
{
	GPIO_ULT_INPUT_Init();
	GPIO_ULT_OUTPUT_Init();
	MX_TIM2_Init();
	
	uint32_t time;
	uint16_t distance = 0;
	uint8_t  ult_flags=0;
	
	if(HAL_GPIO_ReadPin(ULT_Echo_PORT, ULT_Echo_PIN)==0)
	{
		 ult_flags=0;
	}
	else
	{
		 ult_flags=1;
	}
	
	if(ult_flags==0)
	{
		uint8_t error_num=0;
		while(error_num<10)
		{
			HAL_GPIO_WritePin(ULT_TRIG_PORT,ULT_TRIG_PIN,GPIO_PIN_SET);
			HAL_Delay(1);	
			HAL_GPIO_WritePin(ULT_TRIG_PORT,ULT_TRIG_PIN,GPIO_PIN_RESET);
			
			while(!HAL_GPIO_ReadPin(ULT_Echo_PORT, ULT_Echo_PIN));
			__HAL_TIM_SET_COUNTER(&htim2,0);
			__HAL_TIM_ENABLE(&htim2);
			while(HAL_GPIO_ReadPin(ULT_Echo_PORT, ULT_Echo_PIN));
			time=__HAL_TIM_GetCounter(&htim2)+(int)((__HAL_TIM_GetCounter(&htim2))/33);

			__HAL_TIM_DISABLE(&htim2);

			distance=(170*(time*5/(float)1000)+5);

			if((distance<240)||(distance>6000))
			{
				user_main_printf("Distance is out of range");
				error_num++;
				if(error_num==9)				
					break;
			}
			else
			{
				user_main_printf("Distance=%d mm",distance);
				return distance;
			}
		}
	}
	
	else
	{
		user_main_printf("ULT is not connect");
		return distance;
	}	
	
	return distance=0;
}
