#include <config.h>

.global _start
_start:

	/* setup stack */
	ldr     sp, =_estack
	bic     sp, sp, #7      /* for 8-byte alignment */

	/* zero BSS */
	ldr     r0, =_sbss
	ldr     r1, =_ebss
	mov     r2, #0
clbss_l:
	cmp     r0, r1
	bhs     clbss_e
	str     r2, [r0]
	add     r0, r0, #4
	b       clbss_l
clbss_e:
	/* c function */
	blx	main

	/* never turn */
boot_fail:
	b       boot_fail


