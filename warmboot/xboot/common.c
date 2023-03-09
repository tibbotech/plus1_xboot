#include <config.h>
#include <types.h>
#include <regmap.h>

#define UART_put_byte(x) uart0_putc(x)

static void uart0_wait(void)
{
	unsigned int lsr = 0;

	while (!lsr) {
		lsr = UART0_REG->lsr;
		lsr &= 1;
	}
}

void uart0_putc(unsigned char c)
{
	uart0_wait();
	UART0_REG->dr = c;
}

void prn_string(const char *str)
{
	while (*str) {
		if (*str == '\n')
			UART_put_byte('\r');
		UART_put_byte(*str);
		str++;
	}
}

void prn_byte(unsigned char b)
{
	char c;

	c = (b >> 4);
	if (c < 0xA)
		UART_put_byte(c + 0x30);
	else
		UART_put_byte(c + 0x37);
	c = (b & 0xF);
	if (c < 0xA)
		UART_put_byte(c + 0x30);
	else
		UART_put_byte(c + 0x37);
	UART_put_byte(' ');
}

void prn_dword0(unsigned int w)
{
	char c, i;

	UART_put_byte('0');
	UART_put_byte('x');
	for (i = 1; i <= 8; i++) {
		c = (w >> (32 - (i << 2))) & 0xF;
		if (c < 0xA)
			UART_put_byte(c + 0x30);
		else
			UART_put_byte(c + 0x37);
	}
}

void prn_dword(unsigned int w)
{
	prn_dword0(w);
	prn_string("\n");
}


void *memcpy(UINT8 *dest, UINT8 *src, int n)
{
	UINT8 *src_end = (UINT8 *)((unsigned long)(src) + n);
	UINT8 *to = (UINT8 *)(dest);

	while (src != src_end) {
		*to = *src;
		to++;
		src++;
	}

	return (void *)(dest);
}