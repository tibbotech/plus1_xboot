#include <common.h>
#include <config.h>
#include <types.h>

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
	if (g_bootinfo.mp_flag) {
		return;
	}
        uart0_wait();
        UART0_REG->dr = c;
}

void _dbg_info(char *file, u32 line)
{
	if (g_bootinfo.mp_flag) {
		return;
	}
	prn_string("[d] ");
	prn_string(file);
	prn_string(" :");
	prn_decimal(line);
	prn_string("\n");
}

void prn_string(const char *str)
{
	if (g_bootinfo.mp_flag) {
		return;
	}
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

	if (g_bootinfo.mp_flag) {
		return;
	}

	c = (b >> 4);
	if(c < 0xA) UART_put_byte(c + 0x30);
	else		UART_put_byte(c + 0x37);
	c = (b & 0xF);
	if(c < 0xA) UART_put_byte(c + 0x30);
	else		UART_put_byte(c + 0x37);
	UART_put_byte(' ');
}

void prn_dword0(unsigned int w)
{
	char c, i;

	if (g_bootinfo.mp_flag) {
		return;
	}

	UART_put_byte('0');
	UART_put_byte('x');
	for(i=1; i<=8; i++) {
		c = (w >> (32 - (i<<2)) ) & 0xF;
		if(c < 0xA) UART_put_byte(c + 0x30);
		else		UART_put_byte(c + 0x37);
	}
}

void prn_dword(unsigned int w)
{
	if (g_bootinfo.mp_flag) {
		return;
	}
	prn_dword0(w);
	UART_put_byte('\n');
	UART_put_byte('\r');
}

void prn_decimal(unsigned int num)
{
	char a[16];
	int  i = 0;

	if (g_bootinfo.mp_flag) {
		return;
	}

	do {
		a[i] = num % 10;
		num /= 10;
		i++;
	} while (num);

	while(i > 0) {
		UART_put_byte(a[i-1] + 0x30);
		i--;
	}
}

void prn_decimal_ln(unsigned int num)
{
	if (g_bootinfo.mp_flag) {
		return;
	}
	prn_decimal(num);
	UART_put_byte('\n');
	UART_put_byte('\r');
}

void prn_dump_buffer(unsigned char *buf, int len)
{
	int i;

	if (g_bootinfo.mp_flag) {
		return;
	}

	for (i = 0; i < len; i++) {
		if (i && !(i & 0xf)) {
			prn_string("\n");
		}
		prn_byte(buf[i]);
	}
	prn_string("\n");
}

#if defined(HAVE_PRINTF) && !defined(XBOOT_BUILD)

#include <stdarg.h>

#define ADDCH(str, ch) do {	\
	if ((str) < end)	\
		*(str) = (ch);	\
	++str;			\
	} while (0)

static char *string(char *buf, char *end, char *s)
{
	if (s == NULL)
		s = "<NULL>";
	while(*s)
		ADDCH(buf, *s++);
	return buf;
}

static  char *put_dec(char *buf, u32 num)
{
	int numRem, numQuo;

	do{
		numQuo = num / 10;
		numRem = num % 10;
		num = numQuo;
		*buf++ = (char) numRem + '0';
	}while(numQuo);
	return buf;
}

static char *number(char *buf, char *end, u32 num, int base)
{
	/* we are called with base 8, 10 or 16, only, thus don't need "G..."  */
	static const char digits[16] = "0123456789ABCDEF";
	char tmp[66];
	int i;

	/* generate full string in tmp[], in reverse order */
	i = 0;
	if (num == 0)
		tmp[i++] = '0';
	else if (base != 10) { /* 8 or 16 */
		int mask = base - 1;
		int shift = 3;

		if (base == 16)
			shift = 4;

		do {
			tmp[i++] = (digits[((unsigned char)num) & mask]);
			num >>= shift;
		} while (num);
	} else { /* base 10 */
		i = put_dec(tmp, num) - tmp;
	}

	/* actual digits of result */
	while (--i >= 0)
		ADDCH(buf, tmp[i]);
	return buf;
}

static int diag_snprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	u64 num;
	int base = 10;
	char *str;
	char *end = buf + size;

	str = buf;

	for (; *fmt ; ++fmt) {
		if (*fmt != '%') {
			ADDCH(str, *fmt);
			continue;
		}
		++fmt;		/* this also skips first '%' */
		switch (*fmt) {
			case 'c':
				ADDCH(str, (unsigned char) va_arg(args, int));
				continue;
			case 's':
				str = string(str, end, va_arg(args, char *));
				continue;
			case '%':
				ADDCH(str, '%');
				continue;
				/* integer number formats - set up the flags and "break" */
			case 'o':
				base = 8;
				break;
			case 'x':
			case 'X':
			case 'p':
				base = 16;
				break;
			case 'd':
			case 'i':
			case 'u':
				base = 10;
				break;
			default:
				ADDCH(str, '%');
				if (*fmt)
					ADDCH(str, *fmt);
				else
					--fmt;
				continue;
		}

		num = va_arg(args, unsigned int);
		num = (signed int) num;
		if (*fmt == 'd' && (s32)num < 0) {
			ADDCH(str, '-');
			num = -num;
		}
		str = number(str, end, num, base);
	}

	*str = '\0';
	/* the trailing null byte doesn't count towards the total */
	return str - buf;
}

void diag_printf(const char *fmt, ...)
{
	char buf[256];
	va_list args;

	if (g_bootinfo.mp_flag) {
		return;
	}

	va_start(args, fmt);
	diag_snprintf(buf, 256, fmt, args);
	va_end(args);
	prn_string(buf);
}
#endif
