#include "NBIOT_DataIntercept.h"

/**
  * @brief  Intercept the effective part of the issued data printed by NBIOT
  * @param  Processed data
	* @param  Raw data
  * @retval unread: 1->Read finished 0->Not read
  */
uint8_t receiveNBIOT(char* data,const char* param)
{
	uint8_t unread = 0;
	if(strlen(param)<20)
		return unread;
	
	uint8_t i = 0;
	char* start = strchr(param,',');
	char* end  = start;

	while(strchr(start,',') != NULL)
	{
		start = strchr(start,',');
		i++;
		end = start;
		start++;
	}

	start = strchr(param,',');
	for(int j=0;j<i-1;j++)
	{
		start = strchr(start,',');
		start++;
	}

	if(param[(end-param)+1] != '0')
		unread = 1;
	else
		unread = 0;
	
	memcpy(data,param +(start - param),end-start);
	return unread;
}

void strTohex(char *pbDest, const char *pbSrc)
{
	char h1,h2;
	char s1,s2;
	
	for (int i=0; i<strlen(pbSrc)/2; i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i+1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9) 
			s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9) 
			s2 -= 7;

		pbDest[i] = s1*16 + s2;
	}
}

void HexToStr(char *pszDest, const char *pbSrc)
{
	char	ddl, ddh;
	
	for (int i = 0; i < strlen(pbSrc); i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pszDest[i * 2] = ddh;
		pszDest[i * 2 + 1] = ddl;
	}
 
	pszDest[strlen(pbSrc) * 2] = '\0';
}
