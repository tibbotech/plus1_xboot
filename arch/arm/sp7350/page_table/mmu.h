#ifndef _INC_ARM_MMU_H
#define _INC_ARM_MMU_H

/* options available for data cache on each page */
enum dcache_option {
	DCACHE_FAULT        = 0x00, /* invalid */
	DCACHE_OFF          = 0x02,
	DCACHE_WRITETHROUGH = 0x0a,
	DCACHE_WRITEBACK    = 0x0e,
};

/* Size of an MMU section */
enum {
	MMU_SECTION_SHIFT       = 20,
	MMU_SECTION_SIZE        = 1 << MMU_SECTION_SHIFT,
};

struct section_setup {
	unsigned int va_start; /* va start MB */
	unsigned int pa_start; /* pa start MB */
	unsigned int size;     /* size in MB  */
	unsigned int type;     /* dcache_option */
};

#endif // _INC_ARM_MMU_H
