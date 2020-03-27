#ifndef _nbiot_data_intercepth_
#define _nbiot_data_intercepth_

#include "NBIOT_ComConfiguration.h"


uint8_t receiveNBIOT(char* data,const char* param);
void parseNBIOT(char*data,char *pubackid,const char* topic,const char* param);
void strTohex(char *pbDest, const char *pbSrc);
void HexToStr(char *pszDest, const char *pbSrc);
#endif
