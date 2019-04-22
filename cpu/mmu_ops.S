#include <config.h>
#include <asm/link.h>
#include <asm/common.h>
#include <cpu/arm.h>

#if 0 /* This is ARM assembly */
#define ARM(x...)
#define THUMB(x...)	x
#else
#define ARM(x...)	x
#define THUMB(x...)
#endif


/* MMU section attribute */
#define _UND	0x00000		/* S=b0 TEX=b000 AP=b00, Domain=b0000, XN=b0, C=b0, B=b0 */
#define _DEV	0x00c12		/* S=b0 TEX=b000 AP=b11, Domain=b0000, XN=b1, C=b0, B=b0 */
#define _MEM	0x00c1e		/* S=b0 TEX=b000 AP=b11, Domain=b0000, XN=b1, C=b1, B=b1 */

/* Fill MMU sections */
.macro fill_mmu_sect start, end, attr
	ldr	r0, =(\start)
	ldr	r1, =(\end)
	ldr	r2, =(\attr)
	bl	_fill_mmu_sect
.endm

/* Read CPU ID */
.macro set_Z_if_B rd
	mrc	p15, 0, \rd, c0, c0, 0	@ read ID Code: 9260 vs C070
	tst	\rd, #(1 << 14)		@ 9 vs C = Z vs z
.endm

/*
 *	v7_flush_dcache_all()
 *
 *	Flush the whole D-cache.
 *
 *	Corrupted registers: r0-r7, r9-r11 (r6 only in Thumb mode)
 *
 *	Note: copied from arch/arm/mm/cache-v7.S of Linux 4.4
 */
FUNC(__v7_flush_dcache_all)
	WORD_DMB				@ ensure ordering with previous memory accesses
	mrc	p15, 1, r0, c0, c0, 1		@ read clidr
	mov	r3, r0, lsr #23			@ move LoC into position
	ands	r3, r3, #7 << 1			@ extract LoC*2 from clidr
	beq	finished			@ if loc is 0, then no need to clean
start_flush_levels:
	mov	r10, #0				@ start clean at cache level 0
flush_levels:
	add	r2, r10, r10, lsr #1		@ work out 3x current cache level
	mov	r1, r0, lsr r2			@ extract cache type bits from clidr
	and	r1, r1, #7			@ mask of the bits for current cache only
	cmp	r1, #2				@ see what cache we have at this level
	blt	skip				@ skip if no cache, or just i-cache
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	WORD_ISB				@ isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0		@ read the new csidr
	and	r2, r1, #7			@ extract the length of the cache lines
	add	r2, r2, #4			@ add 4 (line length offset)
	mov	r4, #0x300
	orr	r4, r4, #0xff
	ands	r4, r4, r1, lsr #3		@ find maximum number on the way size
	clz	r5, r4				@ find bit position of way size increment
	mov	r7, #0x7f00
	orr	r7, r7, #0xff
	ands	r7, r7, r1, lsr #13		@ extract max number of the index size
loop1:
	mov	r9, r7				@ create working copy of max index
loop2:
 ARM(	orr	r11, r10, r4, lsl r5	)	@ factor way and cache number into r11
 THUMB(	lsl	r6, r4, r5		)
 THUMB(	orr	r11, r10, r6		)	@ factor way and cache number into r11
 ARM(	orr	r11, r11, r9, lsl r2	)	@ factor index number into r11
 THUMB(	lsl	r6, r9, r2		)
 THUMB(	orr	r11, r11, r6		)	@ factor index number into r11

	mcr	p15, 0, r11, c7, c14, 2	@ clean & invalidate by set/way
	subs	r9, r9, #1			@ decrement the index
	bge	loop2
	subs	r4, r4, #1			@ decrement the way
	bge	loop1
skip:
	add	r10, r10, #2			@ increment cache number
	cmp	r3, r10
	bgt	flush_levels
finished:
	mov	r10, #0				@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	WORD_DSBT
	WORD_ISB
	bx	lr
ENDFUNC(__v7_flush_dcache_all)

FUNC(_fill_mmu_sect)
	mov	r3, r0, asl #2
	add	r3, r4
loop:
	cmp	r0, r1
	orrcc r5, r2, r0, asl #20
	strcc r5, [r3], #4
	addcc	r0, r0, #1
	bcc	loop
	bx	lr
ENDFUNC(_fill_mmu_sect)

FUNC(fill_mmu_page_table)
	push	{ r4, r5, lr } /* corrupts */

	/* Initial MMU table @ A_WORK_MEM_BASE */
	ldr	r4, =(A_WORK_MEM_BASE)
	
	fill_mmu_sect 0x000, 0x200, _MEM
	fill_mmu_sect 0x200, 0x980, _UND
	fill_mmu_sect 0x980, 0x1000, _MEM
	fill_mmu_sect 0x9c0, 0x9e0, _DEV

	orr	r4, r4, #0x43		@ Inner Shared WBWA page table walks
	mcr	p15, 0, r4, c2, c0, 0	@ Write Translation Table Base Register 0 (TTBR0)
	mvn	r0, #0			@ Set all Domains to Manager
	mcr	p15, 0, r0, c3, c0, 0	@ Write Domain Access Control Register

	pop	{ r4, r5, lr }
	b	safe_out
ENDFUNC(fill_mmu_page_table)

FUNC(enable_mmu)
	mrc	p15, 0, r0, c1, c0, 0	@ read control register
	orr	r0, r0, #0x00000005	@ enable DCache, MMU
	b	write_ctl_out
ENDFUNC(enable_mmu)

FUNC(disable_mmu)
	/* flush dcache */
	set_Z_if_B r3
	bne	a_flush
	mov	r3, #0
b_flush:
	mrc	p15, 0, r15, c7, c14, 3
	bne	b_flush
	mcr	p15, 0, r3, c7, c10, 4
	b	dis_mmu
a_flush:
 ARM(	stmfd	sp!, {r4-r5, r7, r9-r11, lr}	)
 THUMB(	stmfd	sp!, {r4-r7, r9-r11, lr}	)
	bl	__v7_flush_dcache_all
 ARM(	ldmfd	sp!, {r4-r5, r7, r9-r11, lr}	)
 THUMB(	ldmfd	sp!, {r4-r7, r9-r11, lr}	)

dis_mmu:
	/* disable MMU */
	mrc	p15, 0, r0, c1, c0, 0	@ read control register
	bic	r0, r0, #0x00000005	@ disable DCache, MMU
	b	write_ctl_out
ENDFUNC(disable_mmu)

write_ctl_out:
	mcr	p15, 0, r0, c1, c0, 0	@ write control register
	b	safe_out

safe_out:
	set_Z_if_B r0
	beq	skip_dsb_isb		@ ARM926 skips DSB ISB
	WORD_DSB
	WORD_ISB
skip_dsb_isb:
	bx	lr
