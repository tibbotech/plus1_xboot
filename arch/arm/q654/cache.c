#include <types.h>
#include <aarch32/arch_helpers.h>
#include "page_table/mmu.h"
#include "page_table/mappings_arm.c"

#if 0
#include <common.h>
#else
#include <config.h>
#include <hal/hal_cache.h>
#include <config_xboot.h>
#include <romvsr.h>
void prn_string(const char *);
void prn_dword(unsigned int);
#endif

u32 *page_table = (u32 *)MMU_PGTBL_ADDR;

#define CR_M    (1 << 0)        /* MMU enable                           */
#define CR_A    (1 << 1)        /* Alignment abort enable               */
#define CR_C    (1 << 2)        /* Dcache enable                        */

static void set_section_dcache(int va_section, int pa_section, enum dcache_option option)
{
	u32 value;

	value = (pa_section << MMU_SECTION_SHIFT) | (3 << 10); // [11:10] = AP[1:0]
	value |= option;
	page_table[va_section] = value;
}

static inline void arm_init_before_mmu(void)
{
	hal_dcache_invalidate_all();
	tlbiall();
}

static inline void fill_mmu_page_table(void)
{
	arm_init_before_mmu();
	prn_string("pgt="); prn_dword((u32)page_table);

	int i;
	const struct section_setup *sect;

	/* default all to DCache off */
	for (i = 0; i < 4096; i++) {
		set_section_dcache(i, i, DCACHE_FAULT);
	}

	/* set cacheable regions */
	for (sect = &cache_sect[0]; sect->type != DCACHE_FAULT; sect++) {
		for (i = 0; i < sect->size; i++) {
			set_section_dcache(sect->va_start + i, sect->pa_start + i, sect->type);
		}
	}

	/* TTBR0 : page table address */
	asm volatile("mcr p15, 0, %0, c2, c0, 0" : : "r" (page_table) : "memory");

	/* DACR : Set the access control to Manager (0b11) */
	asm volatile("mcr p15, 0, %0, c3, c0, 0" : : "r" (~0));
}


/* enable C, M */
__attribute__ ((noinline))
void hal_dcache_enable(void)
{
	uint32_t reg;

	prn_string("dcache on\n");
	reg = read_sctlr();
	if (reg & CR_C) {
		prn_string("err: dcache has been ON\n");
		return;
	}

	fill_mmu_page_table();

	asm volatile ("dsb");

	write_sctlr(reg | (CR_C | CR_M)); /* enable MMU & dcache */
}

/* disable C, M */
__attribute__ ((noinline))
void hal_dcache_disable(void)
{
	uint32_t reg;

	prn_string("dcache off\n");
	reg = read_sctlr();
	if ((reg & (CR_C | CR_M)) == 0)
		return;

	asm volatile ("dsb");
	asm volatile ("isb");

	hal_dcache_sync();

	/*** forbid any write to dcache ***/
	write_sctlr(reg & ~(CR_C | CR_M)); /* disable MMU & dcache */
}

int hal_dcache_is_enabled(void)
{
	uint32_t reg;
	reg = read_sctlr();
	return !!(reg & CR_C);
}