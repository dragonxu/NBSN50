#include "flash.h"

static FLASH_EraseInitTypeDef EraseInitStruct;	
static FLASH_ProcessTypeDef pFlash;
__IO uint32_t data32 = 0;
uint32_t PAGEError = 0;

FLAHPAGES flashErase(uint32_t page_address,uint8_t page)
{
	HAL_FLASH_Unlock();
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = page_address;
	EraseInitStruct.NbPages     = page;//total pages:(FLASH_USER_COUNT_END_ADDR - FLASH_USER_COUNT_START_ADDR) / FLASH_PAGE_SIZE;

//  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
//  {	
////    while (1)
//    user_main_printf("error in Erase operation\n\r");
//		NVIC_SystemReset();
//  }
	while(HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK){};
  HAL_FLASH_Lock();
	return PAGE0;
}

FLAHPAGES flashWrite(uint32_t add, uint8_t *data, uint16_t count)
{
	uint32_t Address=0;
	uint8_t i=0;
	
	HAL_FLASH_Unlock();

  Address = add;

  while (i<count)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, data[i]) == HAL_OK)
    {
      Address = Address + 0x04;
			i++;
		}
    else
    {
			user_main_printf("error in Write operation\n\r");
//			NVIC_SystemReset();
    }
  }
  HAL_FLASH_Lock();
	return PAGE0;
}

uint32_t flashRead(uint32_t Address)
{
	  data32 = *(__IO uint32_t *)Address;
		return data32;
}
