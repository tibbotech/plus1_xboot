#include <config.h>
#include <types.h>
#include <config_xboot.h>
#include <regmap.h>
#include "hal_gpio.h"
#include "stc.h"

#include <SECGRP_sp7350.h>

#include <string.h>
#include "common.h"

/* Output peripheral-reset signal (low-active) */
#define OUTPUT_PER_RESETB

#define PM_DATA_SAVE_ADDRESS	0xFA29E000   /* Save the main domain register and User data */
#define CPU_START_ADDRESS	0xfa23fc00
#define CPU_START_POS(core_id)	((CPU_START_ADDRESS - 0x10) - ((core_id) * 8))

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

#define TZC_REGION_ID	(1)
void restore_save_data()
{
	uint8_t *save_data = (uint8_t *)PM_DATA_SAVE_ADDRESS;

	// Restore Secure registers (G502).
	memcpy((void *)RGST_SECURE_REG,(void *)save_data, sizeof(uint32_t) * 32);
	save_data += sizeof(uint32_t) * 32;

	// Restore SecureGroup registers (G113, G114, G115).
	memcpy((void *)SECGRP1_MAIN_REG, (void *)save_data,  sizeof(uint32_t) * 3 * 32);
	save_data += sizeof(uint32_t) * 3 * 32;

	// Restore CPU start address (core 3, 2, 1).
	memcpy((void *)CPU_START_POS(3),(void *)save_data, (CPU_START_POS(0)-CPU_START_POS(3)));
	save_data += (CPU_START_POS(0)-CPU_START_POS(3));

	// Restore secure zone for OP-TEE (2MB).
	*(volatile u32 *)(0xf8c40100 + 0x20 * TZC_REGION_ID) = OPTEE_RUN_ADDR;			// BASE_LOW
	*(volatile u32 *)(0xf8c40104 + 0x20 * TZC_REGION_ID) = 0x00000000;			// BASE_HIGH
	*(volatile u32 *)(0xf8c40108 + 0x20 * TZC_REGION_ID) = OPTEE_RUN_ADDR + (0x200000 - 1); // TOP_LOW
	*(volatile u32 *)(0xf8c4010c + 0x20 * TZC_REGION_ID) = 0x00000000;			// TOP_HIGH
	*(volatile u32 *)(0xf8c40110 + 0x20 * TZC_REGION_ID) = 0xc000000f;			// ATTR: secure access enable
	*(volatile u32 *)(0xf8c40114 + 0x20 * TZC_REGION_ID) = 0x00000000;			// ID_ACCESS disable

#if 0	// Enable UADBG for OP-TEE console.
	// OP-TEE uses UADBG as console by default. Enable this conditional
	// to enable pin-mux and settings of UADBG for resuming from
	// deep sleep.
	UADBG_REG->div_l = UART_BAUD_DIV_L(BAUDRATE, UART_SRC_CLK);
	UADBG_REG->div_h = UART_BAUD_DIV_H(BAUDRATE, UART_SRC_CLK);
	UA2AXI_REG->axi_en = 0; // Disable UA2AXI and enable UADBG.
#endif
#if 0	// Enable UADBG for Linux ttyS5
	MOON0_REG_AO->reset[5] = RF_MASK_V_CLR(1 << 10);	// UADBG_RESET=0
	MOON2_REG_AO->clken[5] = RF_MASK_V_SET(1 << 10);	// UADBG_CLKEN=1
	UA2AXI_REG->axi_en = 0; // Disable UA2AXI and enable UADBG.
#endif

	// Enable CA55_SYS_TIMER.
	volatile u32 *r = (void *)0xf810a000;
	r[2] = 0xfffffff0;	// set cntl
	r[3] = 0xf;		// set cntu
	r[0] = 0x3;		// en=1 & hdbg=1
}
int main()
{
	extern void *__a64rom, *__a64rom_end;
	void *beg = (void *)&__a64rom;
	void *end = (void *)&__a64rom_end;
	u32 start64_addr;

	prn_string("+++[wakeup xboot]" __DATE__ " " __TIME__ "\n");

#ifdef OUTPUT_PER_RESETB
	prn_string("RESETB" "\n");
	// GPIO settings for PER_RESETB (GPIO2)
	HAL_GPIO_GPO(2, 0);   // Output LOW.
	#if !defined(CONFIG_BOOT_ON_CSIM) && !defined(CONFIG_BOOT_ON_ZEBU)
	STC_delay_1ms(25);
	#endif
	HAL_GPIO_O_SET(2, 1); // Output HIGH.
	#if !defined(CONFIG_BOOT_ON_CSIM) && !defined(CONFIG_BOOT_ON_ZEBU)
	STC_delay_1ms(1);
	#endif
	HAL_GPIO_E_SET(2, 0); // Output Hi-Z.
	#if !defined(CONFIG_BOOT_ON_CSIM) && !defined(CONFIG_BOOT_ON_ZEBU)
	STC_delay_1ms(24);
	#endif
#endif

	restore_save_data();

	// Copy a64up code to new position.
	memcpy((void *)WARMBOOT_A64_ADDR, beg, end - beg);

	// Boot-flow: xboot -> a64up -> BL31
	// Set start-address of a64up for all SMP cores.
	start64_addr = (u32)WARMBOOT_A64_ADDR;
	prn_string("a64up@"); prn_dword(start64_addr);

	SECGRP1_MAIN_REG->MAIN_CA55_S01 = start64_addr;
	SECGRP1_MAIN_REG->MAIN_CA55_S02 = start64_addr;
	SECGRP1_MAIN_REG->MAIN_CA55_S03 = start64_addr;
	SECGRP1_MAIN_REG->MAIN_CA55_S04 = start64_addr;

	DSB();

	// Core 0 switches to 64-bit mode (AA64).
	asm volatile ("mcr p15, 0, %0, c12, c0, 2" : : "r"(0x03));	// RR=1 AA64=1

	ISB();
	while (1) {
		asm volatile ("wfi");
	}

	return 0;
}

    // set stack pointer

    // zero out .bss section
    // jump to board initialisation
