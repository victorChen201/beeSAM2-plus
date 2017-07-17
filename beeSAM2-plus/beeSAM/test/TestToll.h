/*------------------------------------------------------------------------------
 Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
 All rights reserved.

 Header file of test toll module for bSAM2 project.
 File name:TestToll.h
 Description：
 -------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
 Author    : liao si liang
 Version   : 1.0
 Date      : 2014/01/22
 Time      : 14:40
 创建文件.
 -------------------------------------------------------------------------------*/
#ifndef __TEST_TOLL_H__
#define __TEST_TOLL_H__
#include "ioport.h"
#include "pio.h"
#include <FreeRTOS.h>
#include <task.h>
#include "timer_mgt_sam.h"
#include "spi_lowlevel.h"
#include <string.h>
#include "config_uart.h"
#include "sys_time.h"
#include "net_cortex_m4.h"
     
     
#define  TEST_PRI      3
#define  TEST_STACK    2048  
#define  TEST_LEN      100     
#define  TEST_FIN      98
     
#define  T_BUTTON_ID   ID_PIOA
#define  T_PORT        PIOA
#define  T_BUTTON      PIO_PA19
#define  T_ATTR       (PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_FALL_EDGE)
#define  T_PIN         PIO_PA20

     
     
#define  TEST_UART   "at\r\n"
#define  TEST_SPI    "intercel test spi flash\r\n"
#define  TEST_BUF    4096
#define  TEST_TIME   3000
     

#define  ASK         "START_TEST\r\n"
#define  ANSER       "YES\r\n"
     
#define  TESTING     "TESTING......"  
#define  CONTINUE    "CONTINUE......"

#define  NET_TEST    "NET_TEST\r\n"
#define  NETPASS     "1NET_PASS\r\n"
#define  NETFAIL     "1NET_FAIL\r\n"

#define  UART_TEST   "UART_TEST\r\n"
#define  UARTPASS    "2UART_PASS\r\n"
#define  UARTFAIL    "2UART_FAIL\r\n"

#define  FLASH_TEST  "FLASH_TEST\r\n"
#define  FLASHPASS   "3FLASH_PASS\r\n"
#define  FLASHFAIL   "3FALSH_FAIL\r\n"

#define  BTN_TEST    "BTN_TEST\r\n"
#define  BTNPASS     "4BUTTON_PASS\r\n"
#define  BTNFAIL     "4BUTTON_FAIL\r\n"

#define  RF_TEST     "RF_TEST\r\n"
#define  RFPASS      "5RF_PASS\r\n"
#define  RFFAIL      "5RF_FAIL\r\n"     
     
#define  LED_TEST    "LED_TEST\r\n"

#define  OVER        "OVER\r\n"

#define  PORT        8888

void test_toll_event(void);
void test_toll_config(void);
int test_int_uart(void);
int test_spi(void);
int test_btn(void);
void init_test_env(void);

#endif
