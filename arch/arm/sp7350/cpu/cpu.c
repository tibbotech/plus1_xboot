
inline void dsb(void)
{
	asm volatile("dsb");
}

inline void isb(void)
{
	asm volatile("isb");
}

unsigned int cpu_core_id(void)
{
	unsigned int val;
	asm volatile("mrc  p15, 0, %0, c0, c0, 0" : "=r"(val) : : "cc");
	return (unsigned int)((val >> 8) & 0x3); // q642: MPIDR : 8100_0000 vs 8100_0100
}

void cpu_invalidate_icache_all(void)
{
	asm volatile("mcr p15, 0, %0, c7, c5, 0" : : "r"(0));
}

void cpu_wfi(void)
{
	/* coverity[no_escape] */
	while (1)
	/* loop forever */;
}

