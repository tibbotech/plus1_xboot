#include <types.h>
#include <common.h>
#include "config.h"

////////////////
//
// dram_test - write -> read -> compare
//

//#define DRAM_TEST_VERBOSE


#define DRAM_CHECK_CYCLING //if write/read addr is large than dram size,will write/read cyclically,so need to check it


#ifdef DRAM_CHECK_CYCLING
#define COMPARE_LEN	0x10

int do_dram_check_cycling(int dramsize_type)
{
	unsigned int check_addr[]={0x100000,0x4100000,0x8100000,0x10100000};//offset 1M at start of(512M 1G 2G 4G)
	int i=0,j=0;
	volatile unsigned int *addr;
	int len = sizeof(check_addr)/sizeof(check_addr[0]);
	//write date in special address
	for(j=0;j<=dramsize_type && j<len;j++)
	{
		unsigned int *beg = (unsigned int *)check_addr[j];//avoid conflict with dram_test
		unsigned int *end = beg+COMPARE_LEN;
		prn_dword((unsigned int)beg);
		for (i=0,addr = (end - 1); addr >= beg && addr < end; addr--,i++)
		{
			*addr = (unsigned int)addr;
		}
	}
	//read and compare with the write data
	for(j=0;j<=dramsize_type && j<len;j++)
	{
		unsigned int *beg = (unsigned int *)check_addr[j];
		unsigned int *end = beg+COMPARE_LEN;
		for (i=0,addr = (end - 1); addr >= beg && addr < end; addr--,i++)
		{
			if (*addr != (unsigned int)addr)
			{
				prn_string("dram size is not match,OTP set is ");prn_decimal(dramsize_type);
				prn_string("\nval_write=");prn_dword((unsigned int)*addr);
				prn_string("val_read=");prn_dword((unsigned int)addr);
				return -1;
			}
		}
	}
	return 0;
}

#endif

int do_dram_test(unsigned int *beg,unsigned int *end)
{
	volatile unsigned int *addr;
	//unsigned int *beg  = (unsigned int *)DRAM_TEST_BEGIN;
	//unsigned int *end  = (unsigned int *)DRAM_TEST_END;
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
	prn_dword0((unsigned int)beg);
        prn_string(" - ");
	prn_dword((unsigned int)end);

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
				~(unsigned int)addr :
				(unsigned int)addr;

			if (((unsigned int)addr & 0xfffff) == 0) {
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
				~(unsigned int)addr :
				(unsigned int)addr;

			if (((unsigned int)addr & 0xfffff) == 0) {
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
				prn_dword((unsigned int)addr);
				prn_string(" expect=");
				prn_dword((unsigned int)expect);
				prn_string(" read=");
				prn_dword((unsigned int)val);
				bad = 1;
				goto out;
			} else if (((unsigned int)addr & 0x3fffff) == 0) {
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
		prn_string("!! DRAM TEST FAILED from!!\n");
		prn_dword0((unsigned int)beg);
		prn_string(" - ");
		prn_dword((unsigned int)end);
	}

	CSTAMP(0x8388FFFF);
	return bad;
}

int dram_test(void)
{
	int ret = 0;
	unsigned int test_addr[]={DRAM_SECOND_TEST_512M_BEGIN,DRAM_SECOND_TEST_1G_BEGIN,DRAM_SECOND_TEST_2G_BEGIN,DRAM_SECOND_TEST_4G_BEGIN};
	volatile unsigned int *ptr;
	ptr = (volatile unsigned int *)(PENTAGRAM_OTP_ADDR + (7 << 2));//G[350.7]
	int dramsize_type = ((*ptr)>>16)&0x03; // 000:512Mb 001:1Gb 010:2Gb 011:4Gb

	/******* test dram start ****************/
	unsigned int *beg  = (unsigned int *)DRAM_FIRST_TEST_BEGIN;
	unsigned int *end  = (unsigned int *)(DRAM_FIRST_TEST_BEGIN+DRAM_TEST_LEN);
	if(do_dram_test(beg,end))
	{
		return 1;
	}
#ifdef DRAM_CHECK_CYCLING
	/******* check write/read is cycling or not in specific address  ****************/
	if(do_dram_check_cycling(dramsize_type))
	{
		return -1;
	}
#endif

	/******* test dram end ****************/
	beg  = (unsigned int *)(test_addr[dramsize_type]);
	end  = (unsigned int *)(test_addr[dramsize_type]+DRAM_TEST_LEN);
	if(do_dram_test(beg,end))
	{
		return -1;
	}
	return ret;
}

