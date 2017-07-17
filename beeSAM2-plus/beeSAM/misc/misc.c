#include "misc.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
//#define MISC_DBG

#ifndef MISC_PUTC
#ifdef MISC_DBG
#define MISC_PUTC(Char)				PUTC(Char)
#else
#define MISC_PUTC(Char)
#endif
#endif

#ifndef MISC_PUTS
#ifdef MISC_DBG
#define MISC_PUTS(Str, Len)			PUTS(Str, Len)
#else
#define MISC_PUTS(Str, Len)
#endif
#endif

#ifndef MISC_PUTASCC
#ifdef MISC_DBG
#define MISC_PUTASCC(Char)			PUTASCC(Char)
#else
#define MISC_PUTASCC(Char)
#endif
#endif

#ifndef MISC_PUTASCS
#ifdef MISC_DBG
#define MISC_PUTASCS(Str, Len)		PUTASCS(Str, Len)
#else
#define MISC_PUTASCS(Str, Len)
#endif
#endif

#ifndef MISC_PRINT
#ifdef MISC_DBG
#define MISC_PRINT(Fmt, Arg...)		PRINT(Fmt, ##Arg)
#else
#define MISC_PRINT(Fmt, Arg...)
#endif
#endif

#ifndef MISC_DYN_PUTS
#ifdef MISC_DBG
#define MISC_DYN_PUTS(Dbg, Str, Len)		DYN_PUTS((M_DBG_MODULE_MISC|Dbg), Str, Len)
#else
#define MISC_DYN_PUTS(Dbg, Str, Len)
#endif
#endif

#ifndef MISC_DYN_PUTASCC
#ifdef MISC_DBG
#define MISC_DYN_PUTASCC(Dbg, Char)			DYN_PUTASCC((M_DBG_MODULE_MISC|Dbg), Char)
#else
#define MISC_DYN_PUTASCC(Dbg, Char)
#endif
#endif

#ifndef MISC_DYN_PUTASCS
#ifdef MISC_DBG
#define MISC_DYN_PUTASCS(Dbg, Str, Len)		DYN_PUTASCS((M_DBG_MODULE_MISC|Dbg), Str, Len)
#else
#define MISC_DYN_PUTASCS(Dbg, Str, Len)
#endif
#endif

#ifndef MISC_DYN_PRINT
#ifdef MISC_DBG
#define MISC_DYN_PRINT(Dbg, Fmt, Arg...)	DYN_PRINT((M_DBG_MODULE_MISC|Dbg), Fmt, ##Arg)
#else
#define MISC_DYN_PRINT(Dbg, Fmt, Arg...)
#endif
#endif

//定义GPIO name

#define STR_GPIO_FILE_PATH			"/sys/class/gpio"
#define MUI_PRODUCT_TEST_MODE		1


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#ifdef M_SYS_IO_CTRL
static const uint8_t s_aui8FuctionGpioPins[] = {MUI_GPIO_PIN_ZIGBEE_RST, /*MUI_GPIO_PIN_FUNC_KEY, MUI_GPIO_PIN_SYS_RST,*/ MUI_GPIO_PIN_PWR_LED, MUI_GPIO_PIN_ZIGBEE_LED, MUI_GPIO_PIN_NET_LED};
static uint8_t s_ui8LightCtrlMode = 0;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#ifdef M_SYS_IO_CTRL
static int s_gpio_value_get (const char *pchFile);
static int s_gpio_value_set (const char *pchFile, uint8_t ui8Val);
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#ifdef M_SYS_IO_CTRL
static int s_gpio_value_get (const char *pchFile)
{
	uint8_t ui8Val;
	int iFd, iRet;

	iFd = open(pchFile, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (iFd < 0)
	{
		return iFd;
	}
	iRet = read(iFd, &ui8Val, sizeof(ui8Val));
	if (iRet > 0)
	{
		iRet = ('0' == ui8Val) ? 0 : 1;
	}
	close(iFd);

	return iRet;
}

static int s_gpio_value_set (const char *pchFile, uint8_t ui8Val)
{
	int iFd, iRet;

	iFd = open(pchFile, O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (iFd < 0)
	{
		return iFd;
	}
	ui8Val = (0 == ui8Val) ? '0' : '1';
	iRet = write(iFd, &ui8Val, sizeof(ui8Val));
	if (iRet > 0)
	{
		iRet = 1;
	}
	close(iFd);

	return iRet;
}

int gpio_init (void)
{
	char achBuf[128];
	int i;

	for(i=0; i<(int)(sizeof(s_aui8FuctionGpioPins)/sizeof(s_aui8FuctionGpioPins[0])); i++)
	{
		//memset(achBuf, 0, sizeof(achBuf));
		snprintf(achBuf, sizeof(achBuf), "echo %u > /sys/class/gpio/export", s_aui8FuctionGpioPins[i]);
		system(achBuf);
		//memset(achBuf, 0, sizeof(achBuf));
		snprintf(achBuf, sizeof(achBuf), "echo out > /sys/class/gpio/gpio%u/direction", s_aui8FuctionGpioPins[i]);
		system(achBuf);
	}

	return 0;
}

int get_led_light_mode (void)
{
	return (int)s_ui8LightCtrlMode;
}

int set_led_light_mode (uint8_t ui8Mode)
{
	if (0 == ui8Mode)
	{
		s_ui8LightCtrlMode = 0;
	}
	else
	{
		s_ui8LightCtrlMode = MUI_PRODUCT_TEST_MODE;
	}

	return s_ui8LightCtrlMode;
}

int led_light_ctrl (uint8_t ui8Pin, uint8_t ui8Val)
{
	char achBuf[128];

	if (MUI_PRODUCT_TEST_MODE != s_ui8LightCtrlMode)
	{
		snprintf(achBuf, sizeof(achBuf), STR_GPIO_FILE_PATH"/gpio%d/value", ui8Pin);
		switch (ui8Pin)
		{
			case MUI_GPIO_PIN_PWR_LED:
			case MUI_GPIO_PIN_NET_LED:
				ui8Val = !ui8Val;
				break;
			case MUI_GPIO_PIN_ZIGBEE_LED:
				//ui8Val = ui8Val;
				break;
			default:
				return -1;
		}
		return s_gpio_value_set(achBuf, ui8Val);
	}

	return 0;
}

int led_light_test (uint8_t ui8Pin, uint8_t ui8Val)
{
	char achBuf[128];

	if (MUI_PRODUCT_TEST_MODE == s_ui8LightCtrlMode)
	{
		snprintf(achBuf, sizeof(achBuf), STR_GPIO_FILE_PATH"/gpio%d/value", ui8Pin);
		return s_gpio_value_set(achBuf, ui8Val);
	}

	return 0;
}

int zigbee_rst_ctrl (uint8_t ui8Val)
{
	char achBuf[128];

	snprintf(achBuf, sizeof(achBuf), STR_GPIO_FILE_PATH"/gpio%d/value", MUI_GPIO_PIN_ZIGBEE_RST);
	return s_gpio_value_set(achBuf, ui8Val);
}

int zigbee_module_rst (void)
{
#if defined(M_FOR_beeSAM2_plus)
	#define MSTR_ATPortRstDev	"/dev/uzip_reset_pin"
	#define MSTR_ATPortRstCmd	"reset"

	int iFd = open(MSTR_ATPortRstDev, O_RDWR);
	if (iFd < 0)
	{
		return iFd;
	}
	write(iFd, MSTR_ATPortRstCmd, sizeof(MSTR_ATPortRstCmd)-1);
	close(iFd);
	return 0;
#elif defined(M_FOR_beeSAM3)
	zigbee_rst_ctrl(0);
	msleep(300);
	return zigbee_rst_ctrl(1);
#else
	return 0;
#endif
}

int button_state_get (uint8_t ui8Pin)
{
	char achBuf[128];

	snprintf(achBuf, sizeof(achBuf), STR_GPIO_FILE_PATH"/gpio%d/value", ui8Pin);
	return s_gpio_value_get(achBuf);
}

#endif

/**
 * Try factory new gateway.
 */
int	config_factory_new_manual_chk (void)
{
	return 0;
}

int	get_linbutton_key()
{
	uint8_t	uint;
	uint32_t nowtick;
	nowtick	= sys_get_ms();
	uint =1;
	return uint;
}

int read_button_key()
{
	int i;
	int iRet = 0;
	int fp = open("/dev/button", O_RDWR);
	if(fp < 0)
	{
		printf("open filed\n");
	}
	iRet = read(fp, &i, sizeof(int));
	if(iRet < 0)
	{
		printf("read failed !\n");
	}
	close(fp);
	return i;
}


//..............................................................................
/*	set_led_module设置灯的状态函数
 ;	(功能)设置LED灯的duty和period
 ;	(参数)
 ;		id_led:		LED灯的id (1~3)
 ;		module:		要设置的属性 1为开，2为period， 3为duty
 ;		duty:
 ;		period:
 ;	(返回)
 ;		int:		实际读出的数据的字节数,负数表示出错
 ;	(注意)
 ;
 */
int set_led_module(int id_led, int module, int duty, int period)
{
	ASSERT(id_led>0 && id_led<4);
	ASSERT(duty!=0 && period!=0);
	ASSERT(duty < period);
	char buf[10]="";
	int iRet = 0;
	int fp = open("/dev/led_beeSAM", O_RDWR);
	if(fp < 0)
	{
		printf("open filed\n");
		close(fp);
		return fp;
	}
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d%d%d", id_led, module, duty);
	iRet = write(fp, buf, sizeof(buf));
	if(iRet < 0)
	{
		close(fp);
		return iRet;
	}
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d%d%d", id_led, module, period);
	iRet = write(fp, buf, sizeof(buf));
	if(iRet < 0)
	{
		close(fp);
		return iRet;
	}
	close(fp);
	return iRet;
}

int set_led_off(int id_led)
{
	ASSERT(id_led>0 && id_led<4);
	char buf[10]="";
	int iRet = 0;
	int fp = open("/dev/led_beeSAM", O_RDWR);
	if(fp < 0)
	{
		printf("open filed\n");
		close(fp);
		return fp;
	}
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d0", id_led);
	iRet = write(fp, buf, sizeof(buf));
	if(iRet < 0)
	{
		close(fp);
		return iRet;
	}
	close(fp);
	return iRet;
}

