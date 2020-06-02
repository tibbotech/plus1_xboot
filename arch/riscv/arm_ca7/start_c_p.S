#include <config.h>
#include <asm/link.h>
#include <asm/common.h>
#include <cpu/arm.h>
#include <config_xboot.h>

.global _start
_start:
	STAMP(0xCA700001);
	ldr r0, =RF_GRP(18, 0)
	ldr	r1, ='C'
	str	r1, [r0]
	ldr r0, =RF_GRP(18, 0)
	ldr	r1, ='A'
	str	r1, [r0]
	ldr r0, =RF_GRP(18, 0)
	ldr	r1, ='7'
	str	r1, [r0]	

	/* abio up */
#if defined(PLATFORM_I137) || defined(CONFIG_PLATFORM_I143)
#ifdef CONFIG_PLATFORM_I143
	bl	A_raise_pll		@ A: speed up core clock
#endif
	bl	A_setup_abio		@ A: speed up ABIO bus clock
#if defined(CONFIG_PLATFORM_I143) && !defined(CONFIG_DISABLE_CORE2_3)
	bl	A_release_cores
#endif
	bl	A_bus_fixup
	bl	A_BIO_access_set
skip_abio_up:
#endif
	mov	r0, #0			@ 0
	mov	r1, #0			@ mach id
	ldr	r2, = DTB_RUN_ADDR_A_VIEW	@ dtb
	ldr	r12, = A_BOOT_POS_A_VIEW
	ldr	r12, [r12]
	WORD_DSB
	WORD_ISB
	ldr r4, = 0x9c000000
	str r12, [r4]
	bx	r12
	b .


/*
 * r0 : A RGST base
 * r1 : A G0.3 clk_cfg
 * r2 : A STC counter address
 * r3 : A G0.18 ioctrl_cfg
 * lr : return address
 */
A_setup_iobus_start:
	/* ASCI io delay */
#if defined(CONFIG_PLATFORM_I143) && !defined(CONFIG_BOOT_ON_ZEBU)
	str	r3, [r0, #0x48]
#endif
	str	r1, [r0, #0xc]
	/* delay 1ms before using ABIO */
	ldr	r5, [r2]	@ time0
	mov	r6, #90		@ 90 ticks = 1ms
	add	r6, r6, r5	@ r6 = time0 + 1ms
1:
	ldr	r5, [r2]	@ r5 = now
	cmp	r5, r6
	blt	1b
	bx	lr
A_setup_iobus_end:

/* A : set BIO to access CBDMA  */
FUNC(A_BIO_access_set)
	ldr	r0, = 0x9c105038	@ A BIO 0.14 src addr
	ldr	r1, = 0x9e80000f
	str	r1, [r0]

	ldr	r0, = 0x9c10503c	@ A BIO 0.15 dst addr
	ldr	r1, = 0xfe8000ff
	str	r1, [r0]
	
	ldr r0, = 0x9c105030	@ A BIO 0.12 enable remap
	ldr r2, = 0x400040
	ldr r1, [r0]
	orr r1,r1,r2
	str r1, [r0]
	
	bx	lr
ENDFUNC(A_BIO_access_set)


FUNC(A_raise_pll)
	ldr	r0, =A_RF_GRP(0, 11)	@ A G0.11 pll_ctl0
	ldr	r1, =A_PLL_CTL0_CFG
	str	r1, [r0]
	bx	lr
ENDFUNC(A_raise_pll)

/* A : setup abio on A_SRAM */
FUNC(A_setup_abio)
	push	{ r4, r5, r6, r11 } /* corrupt */
	mov	r11, lr
	ldr	r1, =(B_SRAM_BASE_A_VIEW - SRAM0_BASE) + A_setup_iobus_start
	ldr	r2, =(B_SRAM_BASE_A_VIEW - SRAM0_BASE) + A_setup_iobus_end
	ldr	r3, =A_WORK_MEM_BASE
	ldr r4, =0x9c000000
	str r1, [r4]
	str r2, [r4]
	
iobus_cpy:
	cmp	r1, r2
	bge	iobus_start
	ldr	r0, [r1], #4
	str	r0, [r3], #4

	ldr r4, =0x9c000000
	str r0, [r4]
	b	iobus_cpy
	
iobus_start:
	WORD_DSB
	WORD_ISB
	ldr	r0, =A_REG_BASE
	ldr	r1, =ABIO_CFG
	/* setup A_STC for delay purpose */
	ldr	r2, =A_RF_GRP(19, 0)
	ldr	r3, =A_RF_GRP(19, 3)	@ A_STC divisor
	ldr	r4, =(1 << 15 | 0x95)	@ A_STC 90K with extdiv=1
	str	r4, [r3]
#if defined(CONFIG_PLATFORM_I143) && !defined(CONFIG_BOOT_ON_ZEBU)
	ldr	r3, =ABIO_IOCTRL_CFG
#endif
	/* Jump to A_SRAM */
	mov	lr, pc
	ldr	pc, =A_WORK_MEM_BASE
	mov	lr, r11
	pop	{ r4, r5, r6, r11 }
	bx	lr
ENDFUNC(A_setup_abio)

#ifdef CONFIG_PLATFORM_I143
/* A release cores 2, 3 */
FUNC(A_release_cores)
	ldr	r2, =A_RF_GRP(0, 2)
#ifndef CONFIG_BOOT_ON_ZEBU
	/* reset     : ca7_ctl_cfg=1    , ca7_sw_rst=0x0667f
	 * power on  :             3->2 ,            0x1e67f -> 0x1ffff
	 * power off :             3->1 ,            0x0667f
	 */
	ldr	r0, =A_RF_GRP(0, 20)
	mov	r1, #0x3
	str	r1, [r0]
	WORD_DSB
	mov	r1, #0x2
	str	r1, [r0]
	WORD_DSB
	ldr	r1, =0x1e67f	@ CORE_2~3 : poweron reset=0
	str	r1, [r2]
	WORD_DSB
#endif
	ldr	r1, =0x1ffff	@ CORE_2~3 : poweron reset=1, primary reset=1, debug reset=1
	str	r1, [r2]
	WORD_DSB
	bx	lr
ENDFUNC(A_release_cores)

FUNC(A_bus_fixup)
	/* NOC fixup (provided by Twofish) */
	ldr	r0, =0x9c1a3f60
	mov	r1, #0x20
	str	r1, [r0]
	str	r1, [r0, #8]
	WORD_DSB
	bx	lr
ENDFUNC(A_bus_fixup)
#endif




