#include "config_uart.h"

#define ioport_set_port_peripheral_mode(port, masks, mode) \
	do {\
		ioport_set_port_mode(port, masks, mode);\
		ioport_disable_port(port, masks);\
	} while (0)

volatile void * volatile mstdio_base[4];

int (*mptr_put[4])(void volatile*, char);
void (*mptr_get[4])(void volatile*, char*);

int config_uart(int com, int baudrate)
{
	const usart_serial_options_t usart =
	{ .baudrate = baudrate, //波特率
			.paritytype = CONF_UART_PARITY, //校验
			.charlength = 3 << 6, .stopbits = 1 << 12

	};

	volatile int uartid = 0;
	volatile int uart = 0;
	switch (com)
	{
	case 1:
		uartid = ID_UART0;
		uart = (int) UART0;
		ioport_set_port_peripheral_mode(PINS_UART0_PORT, PINS_UART0,
				PINS_UART0_MASK);

		break;
	case 2:
		uartid = ID_UART1;
		uart = (int) UART1;
		break;
	case 3:
		uartid = ID_USART0;
		uart = (int) USART0;
		break;
	case 4:
		uartid = ID_USART1;
		uart = (int) USART1;
		ioport_set_port_peripheral_mode(PINS_USART1_PORT, PINS_USART1,
				PINS_USART1_MASK);
		ioport_set_pin_dir(PIO_PA23_IDX, IOPORT_DIR_OUTPUT);
		ioport_set_pin_level(PIO_PA23_IDX, 0);
		break;
	default:
		return -1;
	}

	sysclk_enable_peripheral_clock(uartid);
	usart_serial_init((Usart *) uart, (usart_serial_options_t *) (&usart));

	mstdio_base[com - 1] = (void*) (uart);
	mptr_put[com - 1] = (int (*)(void volatile*, char)) &usart_serial_putchar;
	mptr_get[com - 1] = (void (*)(void volatile*, char*)) &usart_serial_getchar;
	return 1;
}

int snd_to_uart(int com, const unsigned char* data)
{

	while (*data != '\0')
		mptr_put[com - 1](mstdio_base[com - 1], *data++);

}

int rcv_from_uart(int com, unsigned char* buf, int len)
{
	while ((*(buf - 1) != '\n') && (*(buf - 2) != '\r'))
	{
		mptr_get[com - 1](mstdio_base[com - 1], buf++);
	}
}
void uart_printf(char *str);
char *rcv_from_uart1(int com, char ret[])
{
	char buf[100] = "";
	int i = 0;
	while (1)
	{
		mptr_get[com - 1](mstdio_base[com - 1], &ret[i++]);
		if (strstr(ret, "OK") != NULL || strstr(ret, "ERROR") != NULL)
		{
			mptr_get[com - 1](mstdio_base[com - 1], &ret[i++]);
			mptr_get[com - 1](mstdio_base[com - 1], &ret[i++]);
			return ret;
		}
	}
}

void send_atcmd(char *atcmd)
{
	//config_uart(4, 38400);
	//snd_to_uart(4, atcmd);
	put_string(atcmd, strlen(atcmd));
}

char *get_atcmd_ret(char ret[], int len)
{
	//rcv_from_uart1(4, ret);
	get_str(ret, len, 1000 * 30);
}

void uart_rw(char *atcmd, char ret[])
{
	send_atcmd(atcmd);
	get_atcmd_ret(ret, 1024);
}

void uart_printf(char *str)
{
	config_uart(1, 9600);
	snd_to_uart(1, str);
}

