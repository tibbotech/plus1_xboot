//#include <config.h> 
//#include "mmu.h"

// unused ROM is set DCACHE_FAULT
const static struct section_setup cache_sect[] = {
	/*  va,    pa,    mb,     type                } */
	{ 0x000, 0x000, 0x100, DCACHE_WRITEBACK       }, /* DRAM                  */	
	{ 0xe00, 0xe00, 0x100, DCACHE_OFF             }, /* HSM, SPAcc            */
	{ 0xf00, 0xf00, 0x040, DCACHE_WRITETHROUGH    }, /* nor flash             */
	{ 0xfa2, 0xfa2,     1, DCACHE_WRITEBACK       }, /* cbdma sram            */
	{ 0xf80, 0xf80, 0x020, DCACHE_OFF             }, /* RGST                  */
	{ 0xfff, 0xfff,     1, DCACHE_WRITETHROUGH    }, /* ROM                   */
	{     0,     0,     0, DCACHE_FAULT           }  /* mark end              */
};

