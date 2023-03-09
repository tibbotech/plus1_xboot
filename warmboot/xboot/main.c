#include <config.h>
#include <types.h>
#include <config_xboot.h>
#include <regmap.h>

#if defined(PLATFORM_Q645)
#include <SECGRP_q645.h>
#elif defined(PLATFORM_SP7350)
#include <SECGRP_sp7350.h>
#endif

#include <string.h>
#include "common.h"



int main()
{
	extern void *__a64rom, *__a64rom_end;
	void *beg = (void *)&__a64rom;
	void *end = (void *)&__a64rom_end;
	u32 start64_addr;

	prn_string("+++[wakeup xboot]" __DATE__ " " __TIME__ "\n");

	// restore SECGRP register
	memcpy((void *)WARMBOOT_A64_ADDR, beg, end - beg);
	start64_addr = (u32)WARMBOOT_A64_ADDR;

	volatile u32 *r = (void *)0xf810a000;  // enable system timer
	r[2] = 0xfffffff0; // set cntl
	r[3] = 0xf; // set cntu
	r[0] = 0x3; // en=1 & hdbg=1

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

extern uint32_t _estack, _sbss, _ebss;
__attribute__((naked)) void _start(void)
{
    // set stack pointer
    __asm volatile ("ldr r0, =_estack");
    __asm volatile ("mov sp, r0");

    // zero out .bss section
    for (uint32_t *dest = &_sbss; dest < &_ebss;)
    {
        *dest++ = 0;
    }
    // jump to board initialisation
    main();
}