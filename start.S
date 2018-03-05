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
	beq     skip_dsb_isb            @ ARM926 skips DSB ISB
	WORD_DSB
	WORD_ISB
skip_dsb_isb:
	bx      r12
ENDFUNC(exit_bootROM)

FUNC(run_linux_no_stack)
	mov	r0, #0			@ 0
	mov	r1, #0			@ mach id
	ldr	r2, =DTB_RUN_ADDR	@ dtb
	ldr	r12, =LINUX_RUN_ADDR
	bx	r12
ENDFUNC(run_linux_no_stack)
