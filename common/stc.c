#include <common.h>
#include <types.h>
#include <regmap.h>
#include <config.h>

/*
 * STC_AV1 if for boot time measurement
 */
#define BOOT_TIME_STC  (STC_AV1_REG)

static void STC_hw_init(volatile struct stc_regs *regs)
{
#ifdef PLATFORM_SP7350
	regs->stc_config = 0x289;    /* Refer to the datasheet for the value */
	regs->stc_divisor = (1 << 15);   /* ext_refclk*/

	regs->stc_64 = 0;
	regs->stc_31_0 = 0;
	regs->stc_63_32 = 0;
#else
	regs->stc_config = 0;
	regs->stc_divisor = (1 << 15) |                      /* source = EXT_REFCLK / 2 */
		((XTAL_CLK / 2 / (TIMER_KHZ * 1000)) - 1);   /* divisor = (150 -1) */

	regs->stc_31_16 = 0;
	regs->stc_15_0 = 0;
#endif
}

/*
 * STC is for drivers' delay APIs
 */
void STC_init(void)
{
	STC_hw_init(STC_REG);
}

void STC_restart(void)
{
#ifdef PLATFORM_SP7350
	STC_REG->stc_63_32 = 0;
	STC_REG->stc_31_0 = 0;
#else
	STC_REG->stc_31_16 = 0;
	STC_REG->stc_15_0 = 0;
#endif
}

u32 STC_Get32(void)
{
#ifdef PLATFORM_SP7350
	STC_REG->stcl_32 = 0x1234;
	return  STC_REG->stcl_31_0;
#else
	STC_REG->stcl_2 = 0x1234;
	return (STC_REG->stcl_1 << 16) | STC_REG->stcl_0;
#endif
}

/* STC 90kHz : 1 tick = 11.11 us */
inline void STC_delay_ticks(u32 ticks)
{
#ifdef PLATFORM_SP7350
	STC_REG->stc_31_0 = 0;
	while (STC_REG->stc_31_0 < ticks);
#else
	STC_REG->stc_15_0 = 0;
	while (STC_REG->stc_15_0 < ticks);
#endif
}

/* STC 90kHz : max delay = 728 ms */
void STC_delay_1ms(u32 msec)
{
	STC_delay_ticks(msec * 90);
}

/* STC 90kHz : min = 11.11us, max = 728 ms */
void STC_delay_us(u32 usec)
{
	u32 ticks = usec / 11;
	STC_delay_ticks(ticks);
}

u32 AV1_GetStc32(void)
{
#ifdef PLATFORM_SP7350
	BOOT_TIME_STC->stcl_32 = 0x1234;
	return  BOOT_TIME_STC->stcl_31_0;
#else
	BOOT_TIME_STC->stcl_2 = 0x1234;
	return (BOOT_TIME_STC->stcl_1 << 16) | BOOT_TIME_STC->stcl_0;
#endif
}

void AV1_STC_init(void)
{
#if defined(PLATFORM_Q645)
	/* Clear STC_AV1_RESET */
	MOON0_REG->reset[4] = RF_MASK_V_CLR(1 << 3);
#elif defined(PLATFORM_SP7350)
	/* Clear STC_AV1_RESET */
	MOON0_REG_AO->reset[8] = RF_MASK_V_CLR(1 << 2);
#else
	/* Clear STC_AV1_RESET */
	MOON0_REG->reset[1] = RF_MASK_V_CLR(1 << 6);
#endif

	STC_hw_init(BOOT_TIME_STC);
}
