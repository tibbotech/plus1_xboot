#include <config.h>
#include <config_xboot.h>
#include <asm/link.h>
#include <asm/common.h>

/*
 * Please build with ARMv5 toolchain!
 */
#ifdef __ARM_ARCH_7A__
#warning "******************************************"
#warning "*** You build with ARMv7 toolchain!    ***"
#warning "******************************************"
#define WORD_DSB	dsb
#define WORD_ISB	isb
#else /* ARMv5 */
#define WORD_DSB	.word	0xf57ff04f
#define WORD_ISB	.word	0xf57ff06f
#endif

.global _start
_start:
	/* setup stack */
	ldr     sp, =__stack_top
	bic     sp, sp, #7      /* for 8-byte alignment */

	/* zero BSS */
	ldr     r0, =__bss_start
	ldr     r1, =__bss_end__
	mov     r2, #0
clbss_l:
	cmp     r0, r1
	bhs     clbss_e
	str     r2, [r0]
	add     r0, r0, #4
	b       clbss_l
clbss_e:
	/* c function */
	blx	xboot_main

	/* never turn */
boot_fail:
	b       boot_fail


/* Read CPU ID */
.macro set_Z_if_B rd
        mrc     p15, 0, \rd, c0, c0, 0  @ read ID Code: 9260 vs C070
        tst     \rd, #(1 << 14)         @ 9 vs C = Z vs z
.endm

FUNC(exit_bootROM)
	mov	r12, r0
	mov     r1, #0
	mov     r2, #0
	mov     r3, #0
	mov     r4, #0
	mov     r5, #0
	mov     r6, #0
	mov     r7, #0
	mov     r8, #0
	mov     r9, #0
	mov     r10, #0
	mov     r11, #0

	set_Z_if_B r0
	beq	1f			@ ARM926 skips DSB ISB
	WORD_DSB
	WORD_ISB
1:
	bx	r12
ENDFUNC(exit_bootROM)

#ifdef PLATFORM_I137
/*
 * r0 : A RGST base
 * r1 : A G0.3 clk_cfg
 * lr : return address
 */
A_setup_iobus_start:
	str	r1, [r0, #0xc]
	/* delay before using ABIO */
	mov	r1, #0x10000
1:
	subs	r1, r1, #1
	bne	1b
	bx	lr
A_setup_iobus_end:

/* A : setup abio on A_SRAM */
.macro A_setup_abio
	ldr	r1, =(B_SRAM_BASE_A_VIEW - SRAM0_BASE) + A_setup_iobus_start
	ldr	r2, =(B_SRAM_BASE_A_VIEW - SRAM0_BASE) + A_setup_iobus_end
	ldr	r3, =A_WORK_MEM_BASE
iobus_cpy:
	cmp	r1, r2
	bge	iobus_start
	ldr	r0, [r1], #4
	str	r0, [r3], #4
	b	iobus_cpy
iobus_start:
	ldr	r0, =A_REG_BASE
	//ldr	r1, =0x80718		@ 100M  (BogoMIPS: SDR=1.53 DDR=1.71)
	ldr	r1, =0x40718		@ 200M  (BogoMIPS: SDR=2.06 DDR=2.18)
	//ldr	r1, =0x20718		@ 400M  (BogoMIPS: SDR=2.45 DDR=NA)
	mov	lr, pc
	ldr	pc, =A_WORK_MEM_BASE
.endm
#endif

FUNC(boot_next_no_stack)
#ifdef PLATFORM_I137
	set_Z_if_B r5
	beq	skip_abio_up
	ldr	r0, =RF_GRP(18, 0)
	ldr	r1, ='O'		@ print 'O'
	str	r1, [r0]
	A_setup_abio			@ A: speed up ABIO bus clock
	ldr	r0, =RF_GRP(18, 0)
	ldr	r1, ='K'		@ print 'K'
	str	r1, [r0]
skip_abio_up:
#endif
	mov	r0, #0			@ 0
	mov	r1, #0			@ mach id
	ldr	r2, =DTB_RUN_ADDR	@ dtb
	set_Z_if_B r5
	ldrne	r12, =BOOT_ANOTHER_POS_A_VIEW
	ldreq	r12, =BOOT_ANOTHER_POS
	ldr	r12, [r12]
	beq	1f			@ ARM926 skips DSB ISB
	WORD_DSB
	WORD_ISB
1:
	bx	r12
ENDFUNC(boot_next_no_stack)
