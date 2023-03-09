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

	mov	x0, 0

	STAMP(0xa6401004);
	// arg0: x0 = SCR_VALUE
	bl	boot_cpuN
1:
	b	1b

