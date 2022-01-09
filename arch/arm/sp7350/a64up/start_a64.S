#include <config.h>

#define REGS0(addr, value)      \
        ldr w16, =value    ;    \
        ldr x17, =addr     ;    \
        str w16, [x17]     ;

# rd = core # = 0, 1, 2, 3, ...
.macro get_core_N_ID rn
        mrs     \rn, mpidr_el1
        lsr     \rn, \rn, #8
        ands    \rn, \rn, #3
.endm

.macro get_core_N_sp rd, rn
        ldr     \rd , =__init_stack_N /* boot_a64.ldi */
        mrs     \rn, mpidr_el1
        lsr     \rn, \rn, #8
        ands    \rn, \rn, #3
1:
        b.eq    2f
        sub     \rd, \rd, #512 /* per-cpu stack size */
        subs    \rn, \rn, #1
        b       1b
2:
        bic     \rd, \rd, #0xf /* for 16-byte alignment */
.endm

/* xboot aarch64 start code */
.global _start_a64
_start_a64:
	STAMP(0xa6401000);

        /* default SPSel = 1 (use SP_ELx) */

	/* setup stack */
        get_core_N_sp x0, x1
        mov     sp, x0		// sp_el3
        msr     sp_el2, x0
        msr     sp_el1, x0
        msr     sp_el0, x0

	get_core_N_ID x7
        ands    x7, x7, #3
        b.ne    boot_a64

	STAMP(0xa6401001);

	// core 0 path : wake_cpuN
	bl	wake_cpuN

boot_a64:
	// core0~3 path

	STAMP(0xa6401002);


#ifdef CONFIG_A64_EL1
//
// ref: raspberry-pi-os-master/src/lesson02/src/boot.S
//

// ***************************************
// SCTLR_EL1, System Control Register (EL1), Page 2654 of AArch64-Reference-Manual.
// ***************************************

#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN         (0 << 24)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)

#define SCTLR_VALUE_MMU_DISABLED        (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

// ***************************************
// HCR_EL2, Hypervisor Configuration Register (EL2), Page 2487 of AArch64-Reference-Manual.
// ***************************************

#define HCR_RW                          (1 << 31)   // 1: EL1 is aarch64
#define HCR_VALUE                       HCR_RW

// ***************************************
// SCR_EL3, Secure Configuration Register (EL3), Page 2648 of AArch64-Reference-Manual.
// ***************************************

#define SCR_RESERVED                    (3 << 4)
#define SCR_RW                          (1 << 10)
#define SCR_NS                          (1 << 0)    // 1: EL1 is non-secure
#define SCR_VALUE                       (SCR_RESERVED | SCR_RW | SCR_NS)    // drop to NS-EL1
//#define SCR_VALUE                       (SCR_RESERVED | SCR_RW)           // drop to  S-EL1

// ***************************************
// SPSR_EL3, Saved Program Status Register (EL3) Page 389 of AArch64-Reference-Manual.
// ***************************************

#define SPSR_MASK_ALL                   (7 << 6)
#define SPSR_EL1h                       (5 << 0)
#define SPSR_VALUE_EL1                  (SPSR_MASK_ALL | SPSR_EL1h)

el3_to_el1:
	ldr     x0, =SCTLR_VALUE_MMU_DISABLED
	msr     sctlr_el1, x0

	ldr     x0, =HCR_VALUE
	msr     hcr_el2, x0

	ldr     x0, =SCR_VALUE
	msr     scr_el3, x0

	ldr     x1, =SPSR_VALUE_EL1
	msr     spsr_el3, x1

	adr     x2, el1_entry
	msr     elr_el3, x2
	eret
el1_entry:
#else
	mov	x0, 0
#endif

	STAMP(0xa6401004);
	// arg0: x0 = SCR_VALUE
	bl	boot_cpuN
1:
	b	1b

