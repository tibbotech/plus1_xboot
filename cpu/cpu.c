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
