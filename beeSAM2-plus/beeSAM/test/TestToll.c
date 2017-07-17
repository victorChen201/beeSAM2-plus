/*------------------------------------------------------------------------------
 Copyright (c), 2014-2024,Intercel(Shenzhen)Limited.
 All rights reserved.

 Source file of TestToll module for bSAM2 project.
 File name:TestToll.c
 Description：

 -------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------
 Author    : liao si liang
 Version   : 1.0
 Date      : 2014/01/22
 Time      : 14:43

 -------------------------------------------------------------------------------*/

/***************************包含头文件******************************/

#include "TestToll.h"


/*(Description)
 获取PIO口状态
 (Parameter)
 Pio  IO口组

 (Return)
 uint32_t: IO口数值
 */

uint32_t pio_get_input_status(const Pio *p_pio)
{
	return p_pio->PIO_PDSR;
}

/*(Description)
 测试串口
 (Parameter)
 无
 (Return)
 int： 0 FAIL
 1 PASS
 */

int test_int_uart(void)
{

	char get[10] =
	{ 0 };
    at_cmd_query ( "at\r\n", strlen("at\r\n"), get, sizeof(get), 15000);
	if (strlen(get) != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



int test_rf(void)
{
   char get[100] =
	{ 0 };
    at_cmd_query ( "AT$TESTMODE=01\r\n", strlen("AT$TESTMODE=01\r\n"), get, sizeof(get), 15000);
	if (strstr(get, "OK") != NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}
/*(Description)
 测试SPI Flash
 (Parameter)
 无
 (Return)
 int： 0 FAIL
 1 PASS
 */

int test_spi( void )
{
	erease_block(0, 4);
	//spi_flash_write(0, TEST_SPI, strlen(TEST_SPI));
	char test_spi[64] =
	{ 0 };
	//spi_flash_read(0, test_spi, strlen(TEST_SPI));
	erease_block(0, 4);
	if (strcmp(test_spi, TEST_SPI) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}




int test_button( void )
{
    int btnf = 0;
    unsigned int t1,t2;
    t1 = 0;
    t2 = 0;

    while(1)
    {

        int flag = 0;

        flag = pio_get_input_status( T_PORT);
        if (flag & T_BUTTON)
        {
            continue;
        }
        else
        {
            msleep(100);
            btnf++;
        }
        if (btnf > 2)
        {
            return 1;

           /* snd_to_uart(1, BTNPASS);
            msleep(100);
            rcv_from_uart(1, rcv_from_ui, 10);
            */

        }
        t2 = sys_get_ms();

        if ((btnf < 2) && (t2 - t1 > 10000))
        {
            return 0;
            /*snd_to_uart(1, BTNFAIL);
            msleep(100);
            rcv_from_uart(1, rcv_from_ui, 10);*/
        }
    }
}

void test_led( void )
{
    int flick = 0xff;
    while(1)
    {
      if (flick & 1)
       {
            ioport_toggle_pin_level(LED_ZGB_GPIO);
        }

        if (flick & 2)
        {
            ioport_toggle_pin_level(LED_PWR_GPIO);
        }

        if (flick & 4)
        {
            ioport_toggle_pin_level(LED_NET_GPIO);
        }
      msleep(1000);
    }

}


static unsigned char G_Test_Buf[TEST_LEN] = {0};

static unsigned char test_state = 1;




void bSam_Test_Task( void* prm )
{
	UNUSED(prm);

    while( test_state )
    {
       msleep(100);
    }
    if( test_int_uart() == 1 )
    {
        memcpy( G_Test_Buf, UARTPASS, strlen(UARTPASS));
        test_state =1;
     }
    else
    {
        memcpy( G_Test_Buf, UARTFAIL, strlen(UARTFAIL));
        test_state = 1;
     }

    while( test_state )
    {
       msleep(100);
    }

    if( test_spi() == 1 )
    {
        memcpy( G_Test_Buf, FLASHPASS, strlen(FLASHPASS));
        test_state =1;
     }
     else
    {
         memcpy( G_Test_Buf, FLASHFAIL, strlen(FLASHFAIL));
        test_state =1;
     }
    while( test_state )
    {
       msleep(100);
    }

    if( test_button() == 1)
    {
        memcpy( G_Test_Buf, BTNPASS, strlen(BTNPASS));
        test_state =1;
     }
    else
    {
        memcpy( G_Test_Buf, BTNPASS, strlen(BTNPASS));
        test_state =1;
     }

    while( test_state )
    {
       msleep(100);
    }
    if( test_rf() == 1 )
    {
        memcpy( G_Test_Buf, RFPASS, strlen(RFPASS));
        test_state =1;

    }
    else
    {
        memcpy( G_Test_Buf, RFFAIL, strlen(RFFAIL));
        test_state =1;

    }
    test_led();


}

void bSamNet_tcp(void);

void test_toll_config(void)
{
    bSamNet_tcp();
    xTaskCreate( bSam_Test_Task, (const char *const) "bSam_Test_Task",TEST_STACK, NULL, TEST_PRI, NULL);
}




static err_t bSamNet_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p,
		err_t err)
{
	UNUSED(arg);

     if(err == ERR_OK &&  p!= NULL)
     {

        tcp_recved(pcb,p->tot_len);
        unsigned char Test_buf[100] = {0};
        memcpy( Test_buf, p->payload, p->len);
        if( strcmp( Test_buf, ASK) == 0 )
        {
            tcp_write( pcb, NETPASS, strlen(NETPASS), 0);
            test_state = 0;
        }
        else if( strcmp( Test_buf, ANSER) == 0 )
        {
              test_state = 0;
             // tcp_write( pcb, G_Test_Buf, 100, 0);
              memset( G_Test_Buf, 0, sizeof(G_Test_Buf));
        }
        else
        {
            if( test_state == 1)
            {
              tcp_write( pcb, G_Test_Buf, 100, 0);
            }
            else
            {
               tcp_write( pcb, G_Test_Buf, strlen(G_Test_Buf), 0);
            }
        }

        pbuf_free(p);
        p = NULL;
        tcp_close(pcb);

      }
     else
      {
          tcp_arg(pcb, NULL);
          tcp_sent(pcb, NULL);
          tcp_recv(pcb, NULL);
          tcp_close(pcb);
      }
        return ERR_OK;



}


static void bSamNet_conn_err(void* arg,err_t err)
{
	UNUSED(arg);
	UNUSED(err);
}


static err_t bSamNet_pol(void* arg,struct tcp_pcb* pcb)
{
	UNUSED(arg);
	UNUSED(pcb);

	return ERR_OK;
}

static  err_t bSamNet_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
      UNUSED(arg);
      UNUSED(err);


      tcp_arg(pcb,arg);
      tcp_recv(pcb, bSamNet_recv);
      tcp_err(pcb, bSamNet_conn_err);

      tcp_poll(pcb, bSamNet_pol, 4);
      return ERR_OK;
}

void bSamNet_tcp()
{
   struct tcp_pcb *pcb;
   pcb = tcp_new();
   tcp_bind(pcb, IP_ADDR_ANY, PORT);
   pcb = tcp_listen(pcb);
   tcp_accept(pcb, bSamNet_accept);

}






void bSam_Test_Init(void* prm)
{
	UNUSED(prm);

#ifndef M_DBG_ONLY_FOR_NETWORK
	config_info_chk();
#endif
	wdt_restart();
	init_ethernet();
#ifndef M_DBG_ONLY_FOR_NETWORK
	//为保险起见，在这里喂一次狗
	wdt_restart();
	//zigbee_manager_init();
	wdt_restart();
	pwm_led_zgb_on();
	ssdp_init();
	ntp_init();
	//init_rule_task();
	//init_schdu_task();
#endif
	net_info_scan();
    msleep(3000);
	pwm_led_net_on();

	pwm_led_pwr_dis();

	test_toll_config();
    while(1)
    {
        msleep(1000);
    }



}


void init_test_env(void)
{
   xTaskCreate( bSam_Test_Init, (const char *const) "bSam_Test_Init",1024, NULL, 3, NULL);
}

