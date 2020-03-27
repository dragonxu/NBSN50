#ifndef _NBIOT_ComConfiguration_h_
#define _NBIOT_ComConfiguration_h_

#include "common.h"
#include "NBIOT_DataIntercept.h"
#include "NBIOT_PortSend.h"


#define AT  		"AT"                     		/* state */
#define CSQ   	"+CSQ"               				/* singel */
#define NRB			"+NRB"											/* restart */

#define CGATT		"+CGATT"										/* Connect to the network*/
#define CFUN		"+CFUN"											/* Set the UE function */
#define CEREG		"+CEREG"										/* Query whether the network has been activated. */
#define NSONMI	"+NSONMI"										/* Disable indication messages unsolicited result code. */
#define CSCON		"+CSCON"										/* Whether to connect to the network. */
#define CPSMS		"+CPSMS"										/* PWM Setting. */
#define CCLK		"+CCLK"										  /* GET time. */

#define NSOCL		"+NSOCL"										/* closed socked */
#define NSOCR		"+NSOCR"										/* Create SOCKET */
#define NSOST		"+NSOST"										/* Send UDP Message */
#define NSOSTF	"+NSOSTF"										/* Send UDP Message With Flag*/
#define NSORF		"+NSORF" 										/* Read message*/
#define UDP			"=DGRAM,17,56000,1\n"				/* Create UDP connection */


#endif
