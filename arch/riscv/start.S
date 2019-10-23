#include <config.h>
#include <asm/link.h>
#include <asm/smp.h>
#include <config_xboot.h>

#define REGBYTES 8

	.section .text,"ax",@progbits
	.globl start

/* Startup code */
start:

/* initialize global pointer */
    la	sp, __stack_top
	smp_pause(s1, s2)/* pause hart 1...4,only hart 0 do the work */
	
init_bss:
    /* init bss section */
    la	a0, __bss_start
    la	a1, __bss_end__-REGBYTES /* section end is actually the start of the next section */
    li	a2, 0x0
    jal	fill_block

C_code_start:

	call xboot_main 
	ret

/* Fills memory blocks */
fill_block:
    sw		a2, 0(a0)
    bgeu	a0, a1, fb_end
    addi	a0, a0, REGBYTES
    j		fill_block
fb_end:
    ret


.global exit_bootROM
exit_bootROM:
	smp_resume(s1, s2)
	call xboot_slave_main
	ret
	
.global boot_other_hart
boot_other_hart:
	slli	t1,a0,0x20
	srli	t1,t1,0x20
	csrr a0, mhartid
	slli a0, a0, 10
	li a1, DTB_RUN_ADDR
	jr	t1
	ret


//do nothing
FUNC(fill_mmu_page_table)
	ret
ENDFUNC(fill_mmu_page_table)

FUNC(enable_mmu)
	ret
ENDFUNC(enable_mmu)

FUNC(disable_mmu)
	ret
ENDFUNC(disable_mmu)

