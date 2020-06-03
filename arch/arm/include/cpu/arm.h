#ifndef __INC_ARM_H__
#define __INC_ARM_H__

#define ARM_MODE_MASK       0x1F  // CPSR[4:0] = Mode
#define ARM_MODE_ABORT      0x17
#define ARM_MODE_FIQ        0x11
#define ARM_MODE_IRQ        0x12
#define ARM_MODE_SVC        0x13
#define ARM_MODE_SYSTEM     0x1F
#define ARM_MODE_UNDEFINED  0x1B
#define ARM_MODE_USER       0x10

#ifdef __ARM_ARCH_7A__
#warning "******************************************"
#warning "*** You build with ARMv7 toolchain!    ***"
#warning "******************************************"
#define WORD_DSB        dsb
#define WORD_DSBT       dsb st
#define WORD_DMB        dmb
#define WORD_ISB        isb
#define WORD_WFENE      wfene
#define WORD_SEV        sev
#else /* ARMv5 */
#define WORD_DSB        .word   0xf57ff04f
#define WORD_DSBT       .word   0xf57ff04e
#define WORD_DMB        .word   0xf57ff05f
#define WORD_ISB        .word   0xf57ff06f
#define WORD_WFENE      .word   0x1320f002
#define WORD_SEV        .word   0xe320f004
#endif
#endif
