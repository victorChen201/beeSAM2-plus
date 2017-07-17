//#define	__M_FILE_NAME_misc_H__	//屏蔽本头文件
#ifndef	__M_FILE_NAME_misc_H__
#define	__M_FILE_NAME_misc_H__

#include "common.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#define MUI_GPIO_PIN_ZIGBEE_RST		23
#define MUI_GPIO_PIN_SYS_RST		11
#define MUI_GPIO_PIN_FUNC_KEY		22
#define MUI_GPIO_PIN_PWR_LED 		26
#define MUI_GPIO_PIN_ZIGBEE_LED		14
#define MUI_GPIO_PIN_NET_LED		17

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
extern int config_factory_new_manual_chk (void);
extern int get_linbutton_key (void);
extern int config_init (void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#ifdef M_SYS_IO_CTRL
extern int gpio_init (void);
extern int get_led_light_mode (void);
extern int set_led_light_mode(uint8_t ui8Mode);
extern int led_light_ctrl (uint8_t ui8Pin, uint8_t ui8Val);
extern int led_light_test (uint8_t ui8Pin, uint8_t ui8Val);
extern int zigbee_rst_ctrl (uint8_t ui8Val);
extern int zigbee_module_rst (void);
extern int button_state_get (uint8_t ui8Pin);
#else
#define gpio_init()					(0)
#define get_led_light_mode()		(0)
#define set_led_light_mode(Mode)	(0)
#define led_light_ctrl(Pin, Val)	(0)
#define led_light_test(Pin, Val)	(0)
#define zigbee_rst_ctrl(Val)		(0)
#define zigbee_module_rst()			(0)
#define button_state_get(Pin)		(0)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
#endif	//end of "#ifndef __M_FILE_NAME_misc_H__".
