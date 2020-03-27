#ifndef _flash_h_
#define _flash_h_

#include "stm32l0xx_hal.h"
#include "common.h"

#define FLASH_USER_COUNT_START_ADDR   		(FLASH_BASE + FLASH_PAGE_SIZE * 810)             				/* Start @ of user Flash area */
#define FLASH_USER_COUNT_END_ADDR     		(FLASH_USER_COUNT_START_ADDR + FLASH_PAGE_SIZE*100) 		/* End @ of user Flash area */

#define FLASH_USER_COUNT_PWORD_ADDR   		(FLASH_USER_COUNT_START_ADDR )       										/* Start @ of user PWORD area */
#define FLASH_USER_COUNT_SERVADDR_ADDR   	(FLASH_USER_COUNT_PWORD_ADDR + FLASH_PAGE_SIZE*1 )   		/* Start @ of user SERVADDR area */
#define FLASH_USER_COUNT_URI_ADDR   			(FLASH_USER_COUNT_SERVADDR_ADDR + FLASH_PAGE_SIZE*1 )   /* Start @ of user URI area */
#define FLASH_USER_COUNT_WORKMODEl_ADDR   (FLASH_USER_COUNT_URI_ADDR + FLASH_PAGE_SIZE*2 )   			/* Start @ of user workmdodel area */
#define FLASH_USER_COUNT_TDC_ADDR   			(FLASH_USER_COUNT_WORKMODEl_ADDR + FLASH_PAGE_SIZE*1 )  /* Start @ of user TDC area */
#define FLASH_USER_COUNT_INMOD_ADDR   		(FLASH_USER_COUNT_TDC_ADDR + FLASH_PAGE_SIZE*1 )   			/* Start @ of user INMOD area */
#define FLASH_USER_COUNT_5VT_ADDR   			(FLASH_USER_COUNT_INMOD_ADDR + FLASH_PAGE_SIZE*1 )   		/* Start @ of user 5VT area */
#define FLASH_USER_COUNT_GapValue_ADDR   	(FLASH_USER_COUNT_5VT_ADDR + FLASH_PAGE_SIZE*1 )   		  /* Start @ of user GapValue area */

typedef enum
{
	PAGE0 = 0,
	PAGE1,
	PAGE2,
	PAGE3,
	PAGE4,
}FLAHPAGES;


FLAHPAGES  flashErase(uint32_t page_address,uint8_t page);
FLAHPAGES  flashWrite(uint32_t add, uint8_t *data, uint16_t count);
uint32_t flashRead(uint32_t Address);
#endif
