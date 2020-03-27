#ifndef _at_h_
#define _at_h_

#include "common.h"

/* AT Command strings. Commands start with AT */
#define AT      	 "AT"
#define RESET      "Z"
#define CFGMOD     "+CFGMOD"

#define SERVADDR   "+SERVADDR"
#define URI   		 "+URI"

#define FDR        "+FDR"
#define DADDR      "+DADDR"
#define TDC        "+TDC"
#define PWORD      "+PWORD"
#define CFG        "+CFG"
#define INTMOD		 "+INTMOD"

#define CLRCOUNT   "+CLRCOUNT"
#define COUTIME    "+COUTIME"
#define _5VT    	 "+5VT"
#define WEIGRE     "+WEIGRE"
#define WEIGAP     "+WEIGAP"
/**********************************************/

typedef enum
{
	AT_OK = 0,
  AT_ERROR,
	AT_PARAM_ERROR,
	AT_BUSY_ERROR,
	AT_TEST_PARAM_OVERFLOW,
	AT_RX_ERROR
}ATEerror_t;

static const char *const ATError_description[] =
{
  "\r\nOK\r\n",
	"\r\nERROR\r\n",
	"\r\nAT_PARAM_ERROR\r\n",
	"\r\nAT_BUSY_ERROR\r\n",
	"\r\nAT_TEST_PARAM_OVERFLOW\r\n",
	"\r\nAT_RX_ERROR\r\n"
};

ATEerror_t at(const char *param);
ATEerror_t at_que(const char *param);
ATEerror_t at_reset_run(const char *param);
ATEerror_t at_mod_set(const char *param);
ATEerror_t at_mod_get(const char *param);
ATEerror_t at_pword_get(const char *param);
ATEerror_t at_pword_set(const char *param);
ATEerror_t at_fdr_run(const char *param);
ATEerror_t at_cfg_run(const char *param);

ATEerror_t at_servaddr_get(const char *param);
ATEerror_t at_servaddr_set(const char *param);
ATEerror_t at_uri_get(const char *param);
ATEerror_t at_uri_set(const char *param);
ATEerror_t at_tdc_get(const char *param);
ATEerror_t at_tdc_set(const char *param);
ATEerror_t at_inmod_set(const char *param);
ATEerror_t at_inmod_get(const char *param);
ATEerror_t at_5vt_set(const char *param);
ATEerror_t at_5vt_get(const char *param);
ATEerror_t at_weight_reset(const char *param);
ATEerror_t at_weight_get(const char *param);
ATEerror_t at_weight_GapValue_set(const char *param);
ATEerror_t at_weight_GapValue_get(const char *param);

ATEerror_t at_return_error(const char *param);

struct ATCommand_s 
{
  const char *string;                       /*< command string, after the "AT" */
  const int size_string;                    /*< size of the command string, not including the final \0 */
  ATEerror_t (*get)(const char *param);     /*< =? after the string to get the current value*/
  ATEerror_t (*set)(const char *param);     /*< = (but not =?\0) after the string to set a value */
  ATEerror_t (*run)(const char *param);     /*< \0 after the string - run the command */
#if !defined(NO_HELP)
  const char *help_string;                  /*< to be printed when ? after the string */
#endif
};

static const struct ATCommand_s ATCommand[] =
{	
	/** ATZ **/	
  {		
    .string = AT RESET,
		.size_string = sizeof(RESET) - 1,
#ifndef NO_HELP
    .help_string = AT RESET "        : Trig a reset of the MCU",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_reset_run,
  },
	/** AT+CFGMOD **/	
  {
    .string = AT CFGMOD,
		.size_string = sizeof(CFGMOD) - 1,
#ifndef NO_HELP
    .help_string = AT CFGMOD "  : Working mode selection",
#endif
    .get = at_mod_get,
    .set = at_mod_set,
    .run = at_return_error,
  },
	/** AT+PWORD **/	
	{
    .string = AT PWORD,
		.size_string = sizeof(PWORD) - 1,
#ifndef NO_HELP
    .help_string = AT PWORD "   : Get or set the System password",
#endif
    .get = at_pword_get,
    .set = at_pword_set,
    .run = at_return_error,
  },	
		/** AT+FDR **/	
	{
    .string = AT FDR,
		.size_string = sizeof(FDR) - 1,
#ifndef NO_HELP
    .help_string = AT FDR "     : Reset Parameters to Factory Default",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_fdr_run,
  },
		/** AT+CFG **/	
	{
    .string = AT CFG,
		.size_string = sizeof(CFG) - 1,
#ifndef NO_HELP
    .help_string = AT CFG "     : Print all settings",
#endif
    .get = at_return_error,
    .set = at_return_error,
    .run = at_cfg_run,
  },
		/** AT+SERVADDR **/	
	{
    .string = AT SERVADDR,
		.size_string = sizeof(SERVADDR) - 1,
#ifndef NO_HELP
    .help_string = AT SERVADDR ": Get or Set the Server address",
#endif
    .get = at_servaddr_get,
    .set = at_servaddr_set,
    .run = at_return_error,
  },
		/** AT+URI **/	
	{
    .string = AT URI,
		.size_string = sizeof(URI) - 1,
#ifndef NO_HELP
    .help_string = AT URI "     : Get or Set the Resource parameters",
#endif
    .get = at_uri_get,
    .set = at_uri_set,
    .run = at_return_error,
  },
		/** AT+TDC **/	
	{
    .string = AT TDC,
		.size_string = sizeof(TDC) - 1,
#ifndef NO_HELP
    .help_string = AT TDC "     : Get or set the application data transmission interval in s",
#endif
    .get = at_tdc_get,
    .set = at_tdc_set,
    .run = at_return_error,
  },
		/** AT+INMOD **/	
	{
    .string = AT INTMOD,
		.size_string = sizeof(INTMOD) - 1,
#ifndef NO_HELP
    .help_string = AT INTMOD "  : Get or Set the trigger interrupt mode\r\n(0:Disable,1:falling or rising,2:falling,3:rising)",
#endif
    .get = at_inmod_get,
    .set = at_inmod_set,
    .run = at_return_error,
  },
			/** AT+5VT **/	
	{
    .string = AT _5VT,
		.size_string = sizeof(_5VT) - 1,
#ifndef NO_HELP
    .help_string = AT _5VT "     : Get or Set extend the time of 5V power",
#endif
    .get = at_5vt_get,
    .set = at_5vt_set,
    .run = at_return_error,
  },
			/** AT+WEIGRE **/	
	{
    .string = AT WEIGRE,
		.size_string = sizeof(WEIGRE) - 1,
#ifndef NO_HELP
    .help_string = AT WEIGRE "  : Set the weight to 0g\r\n",
#endif
    .get = at_weight_get,
    .set = at_return_error,
    .run = at_weight_reset,
  },
			/** AT+WEIGAP **/	
	{
    .string = AT WEIGAP,
		.size_string = sizeof(WEIGAP) - 1,
#ifndef NO_HELP
    .help_string = AT WEIGAP "  : Get or Set the GapValue of weight",
#endif
    .get = at_weight_GapValue_get,
    .set = at_weight_GapValue_set,
    .run = at_return_error,
  },
};

ATEerror_t ATInsPro( char* at);

#endif
