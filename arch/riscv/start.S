#include <config.h>
#include <asm/link.h>
#include <asm/smp.h>
#include <config_xboot.h>
#include <asm/asm.h>


	.section .text,"ax",@progbits
	.globl start

/* Startup code */
start:
    la t0, trap_entry
    csrw mtvec, t0
    
/* initialize global pointer */
    la	sp, __stack_top
	//smp_pause(s1, s2)/* pause hart 1...4,only hart 0 do the work */
	
init_bss:
    /* init bss section */
    la	a0, __bss_start
    la	a1, __bss_end__-SZREG /* section end is actually the start of the next section */
    li	a2, 0x0
    jal	fill_block

C_code_start:
	la	t0, __boot_info_tmp_value
	fence	rw, w
	amoswap.w zero, zero, 0(t0)
	csrr a0, mhartid
	bnez a0, boot_other_hart
	call xboot_main 
	ret

/* Fills memory blocks */
fill_block:
    sw		a2, 0(a0)
    bgeu	a0, a1, fb_end
    addi	a0, a0, SZREG
    j		fill_block
fb_end:
    ret

.global exit_bootROM
exit_bootROM:
	la	t0, __boot_info_tmp_value
	amoswap.w zero, a0, 0(t0)

	//smp_resume(s1, s2) //resume hart 1...4
	j boot_other_hart
	ret
	
.global boot_other_hart
boot_other_hart:
	la	t0, __boot_info_tmp_value  //used bootinfo_size+384 for temp value to store next addr
	li t1,0
1:	amoswap.w t1, t1, 0(t0)
	fence	r, rw
	beqz	t1, 1b
	
	slli	t1,t1,0x20
	srli	t1,t1,0x20
	amoswap.w zero, t1, 0(t0)
	
	csrr a0, mhartid
	slli a0, a0, 10
	li a1, DTB_RUN_ADDR
	jr	t1
	ret

/* When trap is an interrupt, this function is called */
.align 2
trap_entry:

	/* System call and other traps */
	addi sp, sp, -SZREG*32
	REG_S x1, 1*SZREG(sp)
	REG_S x2, 2*SZREG(sp)
	REG_S x3, 3*SZREG(sp)
	REG_S x4, 4*SZREG(sp)
	REG_S x5, 5*SZREG(sp)
	REG_S x6, 6*SZREG(sp)
	REG_S x7, 7*SZREG(sp)
	REG_S x8, 8*SZREG(sp)
	REG_S x9, 9*SZREG(sp)
	REG_S x10, 10*SZREG(sp)
	REG_S x11, 11*SZREG(sp)
	REG_S x12, 12*SZREG(sp)
	REG_S x13, 13*SZREG(sp)
	REG_S x14, 14*SZREG(sp)
	REG_S x15, 15*SZREG(sp)
	REG_S x16, 16*SZREG(sp)
	REG_S x17, 17*SZREG(sp)
	REG_S x18, 18*SZREG(sp)
	REG_S x19, 19*SZREG(sp)
	REG_S x20, 20*SZREG(sp)
	REG_S x21, 21*SZREG(sp)
	REG_S x22, 22*SZREG(sp)
	REG_S x23, 23*SZREG(sp)
	REG_S x24, 24*SZREG(sp)
	REG_S x25, 25*SZREG(sp)
	REG_S x26, 26*SZREG(sp)
	REG_S x27, 27*SZREG(sp)
	REG_S x28, 28*SZREG(sp)
	REG_S x29, 29*SZREG(sp)
	REG_S x30, 30*SZREG(sp)
	REG_S x31, 31*SZREG(sp)

	csrr a0, mcause
	csrr a1, mepc

	mv a2, sp
	jal	handle_trap

	csrw mepc, a0

	REG_L x1, 1*SZREG(sp)
	REG_L x2, 2*SZREG(sp)
	REG_L x3, 3*SZREG(sp)
	REG_L x4, 4*SZREG(sp)
	REG_L x5, 5*SZREG(sp)
	REG_L x6, 6*SZREG(sp)
	REG_L x7, 7*SZREG(sp)
	REG_L x8, 8*SZREG(sp)
	REG_L x9, 9*SZREG(sp)
	REG_L x10, 10*SZREG(sp)
	REG_L x11, 11*SZREG(sp)
	REG_L x12, 12*SZREG(sp)
	REG_L x13, 13*SZREG(sp)
	REG_L x14, 14*SZREG(sp)
	REG_L x15, 15*SZREG(sp)
	REG_L x16, 16*SZREG(sp)
	REG_L x17, 17*SZREG(sp)
	REG_L x18, 18*SZREG(sp)
	REG_L x19, 19*SZREG(sp)
	REG_L x20, 20*SZREG(sp)
	REG_L x21, 21*SZREG(sp)
	REG_L x22, 22*SZREG(sp)
	REG_L x23, 23*SZREG(sp)
	REG_L x24, 24*SZREG(sp)
	REG_L x25, 25*SZREG(sp)
	REG_L x26, 26*SZREG(sp)
	REG_L x27, 27*SZREG(sp)
	REG_L x28, 28*SZREG(sp)
	REG_L x29, 29*SZREG(sp)
	REG_L x30, 30*SZREG(sp)
	REG_L x31, 31*SZREG(sp)

	addi sp, sp, SZREG*32
	mret	

//do nothing
.global fill_mmu_page_table
fill_mmu_page_table:
	ret

.global enable_mmu
enable_mmu:
	ret

.global disable_mmu
disable_mmu:
	ret		
