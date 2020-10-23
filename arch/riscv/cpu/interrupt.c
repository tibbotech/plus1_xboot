#include <types.h>
#include <common.h>

#define MCAUSE_INT	0x8000000000000000

unsigned long handle_trap(unsigned long cause, unsigned long epc, void *regs)
{
	if (cause & MCAUSE_INT) {
		prn_string("is_irq");
	} 
	prn_string("\nexception code:");prn_decimal_ln(epc);
	prn_string("\n");
	prn_dword0(epc);
	while(1);
	return epc;
}
