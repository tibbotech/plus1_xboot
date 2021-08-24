#include <types.h>
#include <common.h>
#include "config.h"

////////////////
//
// dram_test - write -> read -> compare
//

//#define DRAM_TEST_VERBOSE

int dram_test(void)
{
	volatile unsigned int *addr;
	unsigned int *beg  = (unsigned int *)ADDRESS_CONVERT(DRAM_TEST_BEGIN);
	unsigned int *end  = (unsigned int *)ADDRESS_CONVERT(DRAM_TEST_END);
	unsigned int val, expect;
	int flip;
	int bad = 0;

	CSTAMP(0x83880000);
	CSTAMP(0x8388000F);
	CSTAMP(0x838800F0);
	CSTAMP(0x83880F00);
	CSTAMP(0x8388F000);
	CSTAMP(0x83880000);

#ifdef DRAM_TEST_VERBOSE
        prn_dword(STC_REG->stc_15_0);
#endif

        prn_string("dram test ");
		prn_dword0((unsigned int)ADDRESS_CONVERT(beg));
        prn_string(" - ");
		prn_dword((unsigned int)ADDRESS_CONVERT(end));

	for (flip = 1; flip <= 2; flip++) {
#ifdef DRAM_TEST_VERBOSE
		prn_string("flip=");
		prn_decimal(flip);
		if (flip % 2) {
			prn_string(" write ~addr\n");
		} else {
			prn_string(" write addr\n");
		}

		// Write (no read)
		prn_string("write the range\n");
#endif
		for (addr = (end - 1); addr >= beg && addr < end; addr--) {
			expect = (flip % 2) ?
				~(unsigned int)ADDRESS_CONVERT(addr) :
				(unsigned int)ADDRESS_CONVERT(addr);

			if (((unsigned int)ADDRESS_CONVERT(addr) & 0xfffff) == 0) {
				CSTAMP(addr);
			}

			//prn_string("write "); prn_dword(addr);

			// Write addr = addr
			*addr = (unsigned int) expect;
		}

#ifdef DRAM_TEST_VERBOSE
		prn_string("read to verify the range\n");
#endif

		// Read to verify
		for (addr = (end - 1); addr >= beg && addr < end; addr--) {
			expect = (flip % 2) ?
				~(unsigned int)ADDRESS_CONVERT(addr) :
				(unsigned int)ADDRESS_CONVERT(addr);

			if (((unsigned int)ADDRESS_CONVERT(addr) & 0xfffff) == 0) {
				CSTAMP(addr);
			}

			// Read addr
			val = *addr;

			// Comapre val == expect ?
			if (val != (unsigned int) expect) {
				// Mismatch
				CSTAMP(0x8388EEEE);
				CSTAMP(addr);

				prn_string("Mismatch @");
				prn_dword((unsigned int)ADDRESS_CONVERT(addr));
				prn_string(" expect=");
				prn_dword((unsigned int)expect);
				prn_string(" read=");
				prn_dword((unsigned int)val);
				bad = 1;
				goto out;
			} else if (((unsigned int)ADDRESS_CONVERT(addr) & 0x3fffff) == 0) {
				// Good
				CSTAMP(0x83886666);
				CSTAMP(addr);
#ifdef DRAM_TEST_VERBOSE
				prn_string("Good @");
				prn_dword((unsigned int)addr);
#endif
			}
		}
	}

out:
#ifdef DRAM_TEST_VERBOSE
        prn_string("dram test end\n");
        prn_dword(STC_REG->stc_15_0);
#endif

	if (bad) {
		prn_string("!! DRAM TEST FAILED !!\n");
	}

	CSTAMP(0x8388FFFF);
	return bad;
}
