#ifndef _serverdatasend_h_
#define _serverdatasend_h_

typedef enum
{
	fail =0,
	success
}STATUS;

#include "common.h"

STATUS dataSendPut(char* payload);
STATUS dataSendGet(char* payload);
STATUS sockCreat(void);
STATUS sockClosd(void);

#endif
