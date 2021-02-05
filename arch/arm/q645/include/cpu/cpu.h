#ifndef __CPU_INC_H__
#define __CPU_INC_H__

unsigned int cpu_core_id(void);
void cpu_invalidate_icache_all(void);

void dsb(void);
void isb(void);

void cpu_wfi(void);

#endif
