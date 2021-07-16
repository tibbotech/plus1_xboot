#include <common.h>
#include <types.h>
#include <regmap.h>
#include <config.h>
#include <stc.h>

#ifndef CONFIG_HAVE_ARCH_FASTMEM
void *memcpy(UINT8 *s1, const UINT8 *s2, int n)
{
	UINT8 *s1_end = s1 + n;

	while(s1 != s1_end)
		*s1++ = *s2++;

	return s1;
}
#endif

void *memcpy32(UINT32 *s1, const UINT32 *s2, int n)
{
#ifdef CONFIG_HAVE_ARCH_FASTMEM
	return memcpy((UINT8 *)s1, (const UINT8 *)s2, n << 2);
#else
	UINT32 *s1_end = s1 + n;

	while(s1 != s1_end)
		*s1++ = *s2++;

	return s1;
#endif
}

void *memcpy128(UINT32 *s1, const UINT32 *s2, int n)
{
#ifdef CONFIG_HAVE_ARCH_FASTMEM
	return memcpy((UINT8 *)s1, (const UINT8 *)s2, n << 4);
#else
	UINT32 *s1_end = s1 + n;

	while(s1 != s1_end) {
		*s1++ = *s2++;
		*s1++ = *s2++;
		*s1++ = *s2++;
		*s1++ = *s2++;
	}

	return s1;
#endif
}

int memcmp(const void *s1_beg, const void *s2_beg, size_t n)
{
	int ret = 0;
	const UINT8 *s1 = (const UINT8 *)s1_beg;
	const UINT8 *s2 = (const UINT8 *)s2_beg;
	const UINT8 *s1_end = s1 + n;

	while(s1 != s1_end) {
		if (*s1++ != *s2++)
			ret = 1;
	}

	return ret;
}

#ifndef CONFIG_HAVE_ARCH_FASTMEM
void *memset(UINT8 *s1, int c, int n)
{
	UINT8 *s1_end = s1 + n;

	while (s1 != s1_end)
		*s1++ = c;

	return s1;
}
#endif

void *memset32(UINT32 *s1, UINT32 val, int n)
{
	UINT32 *s1_end = s1 + n;

	while(s1 != s1_end)
		*s1++ = val;

	return s1;
}

void _delay_1ms(UINT32 period)
{
	STC_delay_1ms(period);
}

void delay_1ms(UINT32 period)
{
	//dbg(); // too verbose
	prn_string(".");

#ifndef CSIM_NEW
	_delay_1ms(period);
#endif
}

// reset via watchdog
void boot_reset(void)
{
	prn_string("---------------------\n");
	prn_string("!!! RESET !!!\n");
	prn_string("---------------------\n");

	_delay_1ms(10);

	/* enable watchdog reset */
#if defined(PLATFORM_Q628)|| defined(PLATFORM_I143)
	MOON4_REG->misc_ctl_0 = RF_MASK_V_SET((1 << 4) | (1 << 1));
#endif

	/* STC: watchdog control */
#ifdef PLATFORM_Q645
	STC_REG->timerw_ctl = 0x3877;
	STC_REG->timerw_ctl = 0xAB00;
	STC_REG->timerw_cnt = 0x0001;
	STC_REG->timerw_ctl = 0x4A4B;
#else
	STC_REG->timerw_ctrl = 0x3877;
	STC_REG->timerw_ctrl = 0xAB00;
	STC_REG->timerw_cnt  = 0x0001;
	STC_REG->timerw_ctrl = 0x4A4B;
#endif
}
