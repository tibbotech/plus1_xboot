#include <config.h>
#include <types.h>
#include <config_xboot.h>
#include <regmap.h>

void prn_string(const char *);
void prn_byte(unsigned char b);
void prn_dword(unsigned int);
void prn_dword0(unsigned int);

static unsigned int current_el(void)
{
	unsigned int el;
	asm volatile("mrs %0, CurrentEL" : "=r" (el) : : "cc");
	return el >> 2;
}

// return core number: 0, 1, 2, 3
static unsigned int cpu_core_id(void)
{
	unsigned long val;

	asm volatile ("mrs %0, mpidr_el1" : "=r" (val) : : "cc");

	return (int)((val >> 8) & 0x3); // q642: MPIDR : 8100_0000 vs 8100_0100
}

void boot_cpuN(unsigned long scr_init)
{
	char str[4] = { 0, ' ', 0 };
	int core = cpu_core_id();
	void (*fn)(void);

	fn = (void (*)())BL31_ADDR;

	// u-boot or linux

	// core0 NS-EL1 @ address
	if (core == 0) {
		prn_string("core0 ");
		prn_string(scr_init & 1 ? "NS" : "S"); // bit0=SCR_NS in SCR_EL3
		prn_string("-EL");
		str[0] = ('0' + current_el());
		prn_string(str);
		prn_string("@");
		prn_dword((unsigned int)(unsigned long)fn);
	}

	CSTAMP(0xCA0100AA);
	CSTAMP(core);

	DSB();

	fn();
}

// core 0 wakes core1~3
void wake_cpuN(void)
{
	volatile unsigned int *cpuN_pos; // only 32-bit per core
	extern void cpuN_start(void);
	int i;

	CSTAMP(0xCA000100);

	// let smp cores enter _start_a64 (see go_a32_to_a64)
	for (i = 1; i < SMP_CORES; i++) {
		cpuN_pos = (unsigned int *)(unsigned long)CORE_CPU_START_POS(i);
		*cpuN_pos = CPU_WAIT_A64_VAL;
	}

	asm volatile ("dsb sy");
	asm volatile ("sev");

	CSTAMP(0xCA000109);
}
