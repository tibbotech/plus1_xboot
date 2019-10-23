unsigned int cpu_main_id(void)
{
        unsigned int val;
        asm("mrc  p15, 0, %0, c0, c0, 0" : "=r" (val) : : "cc");
        return val;
}

void cpu_invalidate_icache_all(void)
{
	asm volatile ("mcr p15, 0, %0, c7, c5, 0" : : "r" (0));
}

void cpu_wfi(void)
{
#ifdef __ARM_ARCH_7A__
	asm volatile ("wfi");
#else
	/* Dynamic power management (wait for interrupt mode) */
	asm volatile ("mcr p15, 0, %0, c7, c0, 4" : : "r" (0));
#endif
}
