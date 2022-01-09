#include <config.h>
#include <config_xboot.h>
#include <asm/link.h>
#include <asm/common.h>

.global _start
_start:
FUNC(boot_next_no_stack)
	STAMP(0x5BA00000);
	mov	r0, #0
	ldr	r1, =LINUX_MACH_ID
	ldr	r2, =LINUX_ATAG_DTB_ADDR
	ldr	r12, =LINUX_RUN_ADDR
	dsb
	isb
	bx	r12
ENDFUNC(boot_next_no_stack)
