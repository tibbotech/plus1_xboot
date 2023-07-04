#include <config.h>
#include <types.h>
#include <config_xboot.h>
#include <regmap.h>

#include <SECGRP_sp7350.h>

#include <string.h>
#include "common.h"

#define PM_DATA_SAVE_ADDRESS   0xFA29E000   /* Save the maindomain register and User data */
#define CPU_START_ADDRESS       0xfa23fc00
#define CPU_START_POS(core_id)	((CPU_START_ADDRESS - 0x10) - ((core_id) * 8))

void restore_save_data()
{
	uint8_t *save_data = (uint8_t *)PM_DATA_SAVE_ADDRESS;

	memcpy((void *)RGST_SECURE_REG,(void *)save_data, sizeof(uint32_t) * 32);
	save_data += sizeof(uint32_t) * 32;
	memcpy((void *)SECGRP1_MAIN_REG, (void *)save_data,  sizeof(uint32_t) * 3 * 32);
	save_data += sizeof(uint32_t) * 3 * 32;
	memcpy((void *)CPU_START_POS(3),(void *)save_data, (CPU_START_POS(0)-CPU_START_POS(3)));
	save_data += (CPU_START_POS(0)-CPU_START_POS(3));
	#define TZC_REGION_ID							(1)
	*(volatile u32 *)(0xf8c40100 + 0x20 * TZC_REGION_ID) = OPTEE_RUN_ADDR;					// BASE_LOW
	*(volatile u32 *)(0xf8c40104 + 0x20 * TZC_REGION_ID) = 0x00000000;						// BASE_HIGH
	*(volatile u32 *)(0xf8c40108 + 0x20 * TZC_REGION_ID) = OPTEE_RUN_ADDR + (0x200000 - 1); // TOP_LOW
	*(volatile u32 *)(0xf8c4010c + 0x20 * TZC_REGION_ID) = 0x00000000;						// TOP_HIGH
	*(volatile u32 *)(0xf8c40110 + 0x20 * TZC_REGION_ID) = 0xc000000f;						// ATTR: secure access enable
	*(volatile u32 *)(0xf8c40114 + 0x20 * TZC_REGION_ID) = 0x00000000;						// ID_ACCESS disable
	UADBG_REG->div_l = UART_BAUD_DIV_L(BAUDRATE, UART_SRC_CLK);
	UADBG_REG->div_h = UART_BAUD_DIV_H(BAUDRATE, UART_SRC_CLK);
	UA2AXI_REG->axi_en = 0; // Disable UA2AXI and enable UADBG.
	volatile u32 *r = (void *)0xf810a000;
	r[2] = 0xfffffff0; // set cntl
	r[3] = 0xf; // set cntu
	r[0] = 0x3; // en=1 & hdbg=1
}
int main()
{
	extern void *__a64rom, *__a64rom_end;
	void *beg = (void *)&__a64rom;
	void *end = (void *)&__a64rom_end;
	u32 start64_addr;

	prn_string("+++[wakeup xboot]" __DATE__ " " __TIME__ "\n");

	restore_save_data();
	// restore SECGRP register
	memcpy((void *)WARMBOOT_A64_ADDR, beg, end - beg);
	start64_addr = (u32)WARMBOOT_A64_ADDR;


	// xboot -> a64up -> BL31
	prn_string("a64up@"); prn_dword(start64_addr);

	// set aa64 boot address for all SMP cores
	SECGRP1_MAIN_REG->MAIN_CA55_S01 = start64_addr;
	SECGRP1_MAIN_REG->MAIN_CA55_S02 = start64_addr;
	SECGRP1_MAIN_REG->MAIN_CA55_S03 = start64_addr;
	SECGRP1_MAIN_REG->MAIN_CA55_S04 = start64_addr;

	DSB();

	// core 0 switches to AA64
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
