#include <types.h>
#include <common.h>
#include <banner.h>
#include <bootmain.h>
#include <regmap.h>
#include <cpu/arm.h>
#include <cpu/cpu.h>

extern void *__except_stack_top;

/* Exception Handler */
void do_exec(u32 type, u32 lr)
{
	char *str = "?";
	int i;
	u32 *bptr = ((u32 *)&__except_stack_top) - 13;

	CSTAMP(0xBEBE0001);
	CSTAMP(type);
	CSTAMP(lr);
	CSTAMP(0xBEBE0002);

	switch (type & ARM_MODE_MASK) {
	case ARM_MODE_ABORT: str = "ABORT"; break;
	case ARM_MODE_FIQ: str = "FIQ"; break;
	case ARM_MODE_IRQ: str = "IRQ"; break;
	case ARM_MODE_SVC: str = "SVC"; break;
	case ARM_MODE_SYSTEM: str = "System"; break;
	case ARM_MODE_UNDEFINED: str = "Undef"; break;
	case ARM_MODE_USER: str = "User"; break;
	}
	prn_string("["); prn_string(str);
	prn_string("] CoreID="); prn_dword(cpu_main_id());
	prn_string("PC="); prn_dword(lr);
	prn_string("CPSR="); prn_dword0(type);

	prn_string("\n");
	for (i = 0; i < 13; i++) {
		prn_string("r"); prn_decimal(i);
		prn_string(" = "); prn_dword(bptr[i]);
	}

	CSTAMP(0xBEBE0003);
#ifdef CONFIG_HAVE_MON_SHELL
	mon_shell();
#else
	while (1);
#endif
}
