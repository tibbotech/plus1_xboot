#include <types.h>
#include <common.h>
#include <regmap.h>
#include "cpio.h"

//#define CPIO_ANA_DBG

#ifdef CPIO_TEST
void cpio_taxi(int tcnt, int ecnt)
{
	unsigned int data_tmp;
	int i;

	prn_string("CPIO Test with FACK AXI begin\n");
	prn_string("Test count: "); prn_dword(tcnt);
	cpiol_reg->IO_TCTL &= ~(0xF<<2);
	cpiol_reg->IO_TCTL |= (tcnt<<2);
	cpiol_reg->IO_TCTL |= 0x1;

	data_tmp = cpiol_reg->IO_TCTL;
	while ((data_tmp & 0x1) == 0x0) {
		STC_delay_us(100);
		prn_string("Test run == 0x0\n");
		data_tmp = cpiol_reg->IO_TCTL;
	}

	data_tmp = cpiol_reg->IO_TCTL;
	if (tcnt == 0x0) {
		STC_delay_1ms (0x100);
		cpiol_reg->IO_TCTL &= ~(0x1);
	}

	data_tmp = cpiol_reg->IO_TCTL;
	i = 0x10;
	while (((data_tmp & 0x1) != 0x0) && (i != 0x0)) {
		STC_delay_us (100);
		prn_string("Test run == 0x1!!!\n");
		data_tmp = cpiol_reg->IO_TCTL;
		i--;
	}
	if (i == 0x0) {
		prn_string("Run bit not cleared after timeout!!!\n");
		prn_dword(cpiol_reg->IOP_STS);
		prn_dword(cpiol_reg->IO_TCTL);
		while (1);
	}

	data_tmp = cpiol_reg->IO_TCTL;
	if ((data_tmp & (0x3F << 11)) != (ecnt << 11)) {
		prn_string("Test error count check fail:\n");
		prn_string("Read Error count: "); prn_dword((data_tmp >> 11) & 0x3F);
		prn_string("Expected Error count: "); prn_dword(ecnt);prn_string("\n");
		prn_dword(cpiol_reg->IOP_STS);
		prn_dword(cpiol_reg->IO_TCTL);
		while (1);
	} else {
		prn_string("Test passed\n\n");
	}
}

void cpio_taxi_0(void)
{
	int i;

	prn_string("IO test control register="); prn_dword(cpiol_reg->IO_TCTL);
	prn_string("Fake AXI test with error insertion\n\n");
	cpiol_reg->IO_TCTL |= (0x1<<9);		// Enable error
	cpio_taxi(0xF, 0x1E);
	cpio_taxi(0x0, 0x20);
	prn_string("Fake AXI test without error insertion\n\n");
	cpiol_reg->IO_TCTL &= ~(0x1<<9);	// Disable error insert
	cpiol_reg->IO_TCTL |= (0x5<<6);		// Set size to random;
	cpio_taxi(0xF, 0x0);
	for (i = 0x0; i < 0x10; i++) {
		cpio_taxi(0x0, 0x0);
	}
}

void cpio_taxi_1(void)
{
	unsigned int data_tmp;
	int i, k;

	prn_string("CPIO Test1 with FACK AXI begin\n");
	cpiol_reg->IO_TCTL &= ~(0xF<<2);
	cpiol_reg->IO_TCTL |= 0x1;
	data_tmp = cpiol_reg->IO_TCTL;
	while ((data_tmp & 0x1) == 0x0) {
		STC_delay_us(100);
		prn_string("Test run == 0x0\n");
		data_tmp = cpiol_reg->IO_TCTL;
		while (1);
	}
	data_tmp = cpiol_reg->IO_TCTL;
	k = 0x0;
	while ((data_tmp & 0x1) != 0x0) {
		prn_string("\nTimes "); prn_dword0 (k); prn_string(" Test running... ...\n");
		prn_string("Test ctrl: "); prn_dword(cpiol_reg->IO_TCTL);
		prn_string("PHY status: "); prn_dword(cpiol_reg->IOP_STS);
		data_tmp = cpiol_reg->IO_TCTL;
		prn_string("Error Count: "); prn_dword((data_tmp >> 11) & 0x3F);
		if (((data_tmp >> 11) & 0x3F) == 0x20) {
			prn_string("Test hang-up!!!\n");
			while (1);
		}
		for (i = 0; i < 0x6; i++) {
			STC_delay_1ms (0x100);
		}
		k++;
	}
	prn_string("Test stoped by error\n");
	while (1);
}

void cpio_cfg_pll(void)
{
	prn_string("Config CPIO PLL and analog\n");
#if 0
	prn_string("VCO: 9.6G, PLL: 2.4G\n");
	cpiol_reg->AFE_CTL[0] = 0x248083FE;
	cpiol_reg->AFE_CTL[1] = 0x4527BDF9;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x12C18C33;
	cpiol_reg->AFE_CTL[7] = 0x00003009;
#endif
#if 0
	prn_string("VCO: 9.6G, PLL: 4.8G\n");
	cpiol_reg->AFE_CTL[0] = 0x2C8083FA;
	cpiol_reg->AFE_CTL[1] = 0x252001FB;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x12C19C33;
	cpiol_reg->AFE_CTL[7] = 0x00003001;
#endif
#if 1
	prn_string("VCO: 9.6G, PLL: 9.6G\n");
	cpiol_reg->AFE_CTL[0] = 0x2C8083FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BDFB;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x12C18C33;
	cpiol_reg->AFE_CTL[7] = 0x00003001;
#endif
#if 0
	prn_string("VCO: 6.4G, PLL: 6.4G\n");
	cpiol_reg->AFE_CTL[1] = 0x2520013B;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x12C18833;
	cpiol_reg->AFE_CTL[7] = 0x00003001;
	cpiol_reg->AFE_CTL[0] = 0x2C8083FA;
#endif
#if 0
	prn_string("VCO: 4.1G, PLL: 4.1G\n");
	cpiol_reg->AFE_CTL[0] = 0x248083FA;
	cpiol_reg->AFE_CTL[1] = 0x4527BDF9;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x1F330A63;
	cpiol_reg->AFE_CTL[7] = 0x00003001;
#endif
#if 0
	prn_string("VCO: 14.8G, PLL: 7.4G\n");
	cpiol_reg->AFE_CTL[0] = 0x2C8083FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BDFB;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x12C18973;
	cpiol_reg->AFE_CTL[7] = 0x00003001;
#endif
#if 0
	prn_string("VCO: 12.6G, PLL: 12.6G\n");
	cpiol_reg->AFE_CTL[0] = 0x2C8083FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BDFB;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x1F331FF3;
	cpiol_reg->AFE_CTL[7] = 0x00000001;
#endif
#if 0
	prn_string("VCO: 10G, PLL: 10G\n");
	cpiol_reg->AFE_CTL[0] = 0x2C8083FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BDFB;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x1F330CB3;
	cpiol_reg->AFE_CTL[7] = 0x00003001;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 0.4G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90000000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F331823;
	cpiol_reg->AFE_CTL[7] = 0x00001009;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 0.8G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90000000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F330823;
	cpiol_reg->AFE_CTL[7] = 0x00001009;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 1.6G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90000000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F331823;
	cpiol_reg->AFE_CTL[7] = 0x00001001;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 2.5G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90081000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F331CA3;
	cpiol_reg->AFE_CTL[7] = 0x00001001;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 1.25G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90081000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F330A23;
	cpiol_reg->AFE_CTL[7] = 0x00001009;
#endif

	prn_dword(cpiol_reg->AFE_CTL[6]);
}

void cpio_reset(int re_cfg)
{
	prn_string("Reset CPIO controller\n");

	*(volatile u32 *)(0xf8000058) = ((0x6<<16) | 0x6);
	STC_delay_us (1);
	*(volatile u32 *)(0xf8000058) = (0x6<<16);
	cpiol_reg->AFE_CTL[7] |= 0x1;		// enable test clock output
	prn_dword(cpiol_reg->PHY_CTRL);
	cpiol_reg->PHY_CTRL = (0x1<<19);	// Disable auto initial;
	//cpiol_reg->PHY_CTRL = (0x1<<18);	// 2 lane mode
	cpiol_reg->PHY_CTRL |= ((0x1<<5) | (0x1<<(5+16)));	// internal pad

	if (re_cfg) {
		cpio_cfg_pll();
	}

	//cpiol_reg->AFE_CTL[7] |= (0x1<<8);	// GPO DS;
	//cpiol_reg->AFE_CTL[7] |= (0x1<<9);	// GPIO inv

	//cpiol_reg->TEST_C[1] |= (0x1<<9);	// force control
	//cpiol_reg->AFE_CTL[0] |= (0x1F<<10);	// XOVER all
	//cpiol_reg->AFE_CTL[0] |= (0x1F<<17);	// POL-inv all

	cpiol_reg->PHY_CTRL |= (0x1<<7);	// Release ref reset
	prn_dword(cpiol_reg->PHY_CTRL);
	STC_delay_us (100);
}

void cpio_reset1(int dly0, int dly1)
{
	prn_string("Reset CPIO controller"); prn_dword0 (dly0); prn_dword(dly1);

	*(volatile u32 *)(0xf8000058) = ((0x6<<16) | 0x6);
	STC_delay_us (1);
	*(volatile u32 *)(0xf8000058) = (0x6<<16);
	cpiol_reg->AFE_CTL[7] |= 0x1;		// enable test clock output
	prn_dword(cpiol_reg->PHY_CTRL);
	cpiol_reg->PHY_CTRL = (0x1<<19);	// Disable auto initial;
	//cpiol_reg->PHY_CTRL = (0x1<<18);	// 2 lane mode
	//cpiol_reg->PHY_CTRL |= ((0x1<<5) | (0x1<<(5+16)));	// internal pad

	cpio_cfg_pll();
	//cpiol_reg->AFE_CTL[0] |= (0x1<<17);	// POL_INV
	//cpiol_reg->AFE_CTL[0] |= (0x1<<30);	// RCK dly en1
	//cpiol_reg->AFE_CTL[0] |= (dly0<<30);	// RCK dly en2
	//cpiol_reg->AFE_CTL[0] |= (dly1<<12);	// RCK dly value

	if (dly0 == 0x0) {
		cpiol_reg->AFE_CTL[3] &= ~(0x7<<26);
		cpiol_reg->AFE_CTL[3] |= ((dly1&0x7)<<26);
		cpiol_reg->AFE_CTL[4] &= ~(0x7);
		cpiol_reg->AFE_CTL[4] |= (dly1&0x7);
	} else {
		cpiol_reg->AFE_CTL[3] &= ~(0x7<<29);
		cpiol_reg->AFE_CTL[3] |= ((dly1&0x7)<<29);
		cpiol_reg->AFE_CTL[4] &= ~(0x7<<3);
		cpiol_reg->AFE_CTL[4] |= ((dly1&0x7)<<3);
	}

	//cpiol_reg->AFE_CTL[7] |= (0x1<<8);	// GPO DS;
	//cpiol_reg->AFE_CTL[7] |= (0x1<<9);	// GPIO inv

	//cpiol_reg->TEST_C[1] |= (0x1<<9);	// force control
	//cpiol_reg->AFE_CTL[0] |= (0x1F<<10);	// XOVER all
	//cpiol_reg->AFE_CTL[0] |= (0x1F<<17);	// POL-inv all

	cpiol_reg->PHY_CTRL |= (0x1<<7);	// Release ref reset
	prn_dword(cpiol_reg->PHY_CTRL);
	STC_delay_us (100);
}

void ana_test(int xover)
{
	int ecnt, ecnt0, ecnt1, ecnt2, ecnt3;
	int i;

	prn_string("Analog function TEST\n");
	cpiol_reg->TEST_C[1] |= (0x1<<8);
	//cpiol_reg->TEST_C[0] |= (0x1<<4);

	//cpiol_reg->TEST_D[0] = 0xe1e1e1e1;
	//cpiol_reg->TEST_D[1] = 0xeeeeeeee;
	//cpiol_reg->TEST_D[1] = 0x815FA815;
	//cpiol_reg->TEST_D[1] = 0xF3CF3CF0;
	//cpiol_reg->TEST_D[2] = 0x11111111;
	//cpiol_reg->TEST_D[2] = ~(0x815FA815);
	//cpiol_reg->TEST_D[2] = ~(0xF3CF3CF0);
	//cpiol_reg->TEST_D[3] = 0xF3CF3CF0;
	cpiol_reg->TEST_D[0] = 0x55555555;
	cpiol_reg->TEST_D[1] = 0x55555555;
	cpiol_reg->TEST_D[2] = 0x55555555;
	cpiol_reg->TEST_D[3] = 0x55555555;
	//cpiol_reg->TEST_D[0] = 0x815FA815;
	//cpiol_reg->TEST_D[1] = 0x815FA815;
	//cpiol_reg->TEST_D[2] = 0x815FA815;
	//cpiol_reg->TEST_D[3] = 0x815FA815;
#if 0
	prn_string("VCO: 4.1G, PLL: 4.1G\n");
	cpiol_reg->AFE_CTL[0] = 0x248083FA;
	cpiol_reg->AFE_CTL[1] = 0x4527BDF9;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x1F330A63;
	cpiol_reg->AFE_CTL[7] = 0x00003001;
#endif
#if 1
	prn_string("VCO: 9.6G, PLL: 9.6G\n");
	cpiol_reg->AFE_CTL[0] = 0x2C8083FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BDFB;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x12C18C33;
	cpiol_reg->AFE_CTL[7] = 0x00003001;
#endif
#if 0
	prn_string("VCO: 9.6G, PLL: 2.4G\n");
	cpiol_reg->AFE_CTL[0] = 0x248183FA;
	//cpiol_reg->AFE_CTL[0] = 0x248103FA;
	cpiol_reg->AFE_CTL[1] = 0x4527BDF9;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x12C18C33;
	cpiol_reg->AFE_CTL[7] = 0x00003009;
#endif
#if 0
	prn_string("VCO: 14.8G, PLL: 7.4G\n");
	cpiol_reg->AFE_CTL[0] = 0x2C8083FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BDFB;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x12C18973;
	cpiol_reg->AFE_CTL[7] = 0x00003001;
#endif
#if 0
	prn_string("VCO: 12.6G, PLL: 12.6G\n");
	cpiol_reg->AFE_CTL[0] = 0x2C8083FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BDFB;
	cpiol_reg->AFE_CTL[2] = 0x00000C00;
	cpiol_reg->AFE_CTL[3] = 0x48081000;
	cpiol_reg->AFE_CTL[4] = 0x000000D2;
	cpiol_reg->AFE_CTL[5] = 0x00006801;
	cpiol_reg->AFE_CTL[6] = 0x1F330CB3;
	cpiol_reg->AFE_CTL[7] = 0x00001001;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 0.4G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90000000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F331823;
	cpiol_reg->AFE_CTL[7] = 0x00001009;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 2.5G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90081000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F331CA3;
	cpiol_reg->AFE_CTL[7] = 0x00001001;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 0.8G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FF;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90000000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F330823;
	cpiol_reg->AFE_CTL[7] = 0x00001009;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 0.4G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90000000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F331823;
	cpiol_reg->AFE_CTL[7] = 0x00001009;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 1.25G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FF;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90081000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F330B23;
	cpiol_reg->AFE_CTL[7] = 0x00001009;
#endif

	if (xover) {
		cpiol_reg->AFE_CTL[0] |= (0x1F<<10);
	}

	prn_string("Print All AFE_CTL register\n");
	for (i = 0; i < 8; i++) {
		prn_dword(cpiol_reg->AFE_CTL[i]);
	}
	prn_string("Test RX Datas\n");
	ecnt = 0;

	prn_string("\nTest Channel0\n");
	cpiol_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpiol_reg->AFE_CTL[0] |= (0x11<<5);	// Enable channel0
	ecnt0 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpiol_reg->TEST_D[4]);
		prn_dword(cpiol_reg->TEST_D[5]);
#endif
		if ((cpiol_reg->TEST_D[4] != 0xaaaaaaaa) && (cpiol_reg->TEST_D[4] != 0x55555555))
			ecnt0++;
		if (cpiol_reg->TEST_D[5] != 0x0)
			ecnt1++;
	}
	if (ecnt0 != 0x0) {
		prn_string("Channel0 Test Fail with error count: "); prn_dword(ecnt0);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	prn_string("\nTest Channel1\n");
	cpiol_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpiol_reg->AFE_CTL[0] |= (0x12<<5);	// Enable channel1
	ecnt1 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpiol_reg->TEST_D[4]);
		prn_dword(cpiol_reg->TEST_D[5]);
#endif
		if (cpiol_reg->TEST_D[4] != 0x0)
			ecnt1++;
		if ((cpiol_reg->TEST_D[5] != 0xaaaaaaaa) && (cpiol_reg->TEST_D[5] != 0x55555555))
			ecnt1++;
	}
	if (ecnt1 != 0x0) {
		prn_string("Channel1 Test Fail with error count: "); prn_dword(ecnt1);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	cpiol_reg->TEST_C[0] |= (0x1<<4);

	prn_string("\nTest Channel2\n");
	cpiol_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpiol_reg->AFE_CTL[0] |= (0x14<<5);	// Enable channel2
	ecnt2 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpiol_reg->TEST_D[4]);
		prn_dword(cpiol_reg->TEST_D[5]);
#endif
		if ((cpiol_reg->TEST_D[4] != 0xaaaaaaaa) && (cpiol_reg->TEST_D[4] != 0x55555555))
			ecnt2++;
		if (cpiol_reg->TEST_D[5] != 0x0)
			ecnt2++;
	}
	if (ecnt2 != 0x0) {
		prn_string("Channel2 Test Fail with error count: "); prn_dword(ecnt2);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	prn_string("\nTest Channel3\n");
	cpiol_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpiol_reg->AFE_CTL[0] |= (0x18<<5);	// Enable channel3
	ecnt3 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpiol_reg->TEST_D[4]);
		prn_dword(cpiol_reg->TEST_D[5]);
#endif
		if (cpiol_reg->TEST_D[4] != 0x0)
			ecnt3++;
		if ((cpiol_reg->TEST_D[5] != 0xaaaaaaaa) && (cpiol_reg->TEST_D[5] != 0x55555555))
			ecnt3++;
	}
	if (ecnt3 != 0x0) {
		prn_string("Channel1 Test Fail with error count: "); prn_dword(ecnt3);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	prn_string("\nTest 2lane mode transfers(lane1, lane2)\n");
	cpiol_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpiol_reg->AFE_CTL[0] |= (0x16<<5);	// Enable channel1, channel2
	ecnt1 = 0;
	ecnt2 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpiol_reg->TEST_D[4]);
		prn_dword(cpiol_reg->TEST_D[5]);
#endif
		if ((cpiol_reg->TEST_D[4] != 0xaaaaaaaa) && (cpiol_reg->TEST_D[4] != 0x55555555))
			ecnt2++;
		if (cpiol_reg->TEST_D[5] != 0x0)
			ecnt2++;
	}
	cpiol_reg->TEST_C[0] &= ~(0x3<<4);
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpiol_reg->TEST_D[4]);
		prn_dword(cpiol_reg->TEST_D[5]);
#endif
		if (cpiol_reg->TEST_D[4] != 0x0)
			ecnt1++;
		if ((cpiol_reg->TEST_D[5] != 0xaaaaaaaa) && (cpiol_reg->TEST_D[5] != 0x55555555))
			ecnt1++;
	}
	if ((ecnt1 != 0x0) || (ecnt2 != 0x0)) {
		prn_string("2lane mode Test Fail with error count (ch1, ch2): "); prn_dword0 (ecnt1); prn_string(" , "); prn_dword(ecnt2);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	prn_string("\nTest 4lane mode transfers(lane0, lane1, lane2, lane3)\n");
	cpiol_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpiol_reg->AFE_CTL[0] |= (0x1F<<5);	// Enable channel0, channel1, channel2, channel3
	ecnt0 = 0;
	ecnt1 = 0;
	ecnt2 = 0;
	ecnt3 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpiol_reg->TEST_D[4]);
		prn_dword(cpiol_reg->TEST_D[5]);
#endif
		if ((cpiol_reg->TEST_D[4] != 0xaaaaaaaa) && (cpiol_reg->TEST_D[4] != 0x55555555))
			ecnt0++;
		if ((cpiol_reg->TEST_D[5] != 0xaaaaaaaa) && (cpiol_reg->TEST_D[5] != 0x55555555))
			ecnt1++;
	}
	cpiol_reg->TEST_C[0] |= (0x1<<4);
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpiol_reg->TEST_D[4]);
		prn_dword(cpiol_reg->TEST_D[5]);
#endif
		if ((cpiol_reg->TEST_D[4] != 0xaaaaaaaa) && (cpiol_reg->TEST_D[4] != 0x55555555))
			ecnt2++;
		if ((cpiol_reg->TEST_D[5] != 0xaaaaaaaa) && (cpiol_reg->TEST_D[5] != 0x55555555))
			ecnt3++;
	}
	if ((ecnt0 != 0x0) || (ecnt1 != 0x0) || (ecnt2 != 0x0) || (ecnt3 != 0x0)) {
		prn_string("4lane mode Test Fail with error count (ch0, ch1, ch2, ch3): "); prn_dword0 (ecnt0);
		prn_string(" , "); prn_dword0 (ecnt1); prn_string(" , "); prn_dword0 (ecnt2); prn_string(" , "); prn_dword(ecnt3);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	if (ecnt != 0x0) {
		prn_string("\nAnalog Test with "); prn_dword0(ecnt); prn_string(" test Fail!!\n");
#ifdef CPIO_ANA_DBG
		while (1);
#endif
	} else
		prn_string("\nAll Analog Test Pass\n");
	//while (1);
}

void ana_test1(int re_cfg, int cfg_v, int cfg_v0, int cfg_v1, int cfg_v2, int cfg_v3)
{
	int ecnt, ecnt0, ecnt1, ecnt2, ecnt3;
	unsigned int tmp_data0[10];
	unsigned int tmp_data1[10];
	unsigned int tmp_data2[10];
	unsigned int tmp_data3[10];
	unsigned int tmp_data;
	int i;

	prn_string("Analog function TEST\n");
	cpiol_reg->TEST_C[1] |= (0x1<<8);
	//cpiol_reg->TEST_C[0] |= (0x1<<4);

	//cpiol_reg->TEST_D[0] = 0xe1e1e1e1;
	cpiol_reg->TEST_D[0] = 0x815FA815;
	cpiol_reg->TEST_D[1] = 0x815FA815;
	cpiol_reg->TEST_D[2] = 0x815FA815;
	cpiol_reg->TEST_D[3] = 0x815FA815;
	//cpiol_reg->TEST_D[2] = ~(0xF3CF3CF0);
	//cpiol_reg->TEST_D[3] = 0xF3CF3CF0;

	//cpiol_reg->TEST_D[0] = 0xAAAAAAAA;
	//cpiol_reg->TEST_D[1] = 0xAAAAAAAA;
	//cpiol_reg->TEST_D[2] = 0xAAAAAAAA;
	//cpiol_reg->TEST_D[3] = 0xAAAAAAAA;

#if 0
	prn_string("VCO: 3.2G, PLL: 0.4G\n");
	cpiol_reg->AFE_CTL[0] = 0x240103FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90000000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F331823;
	cpiol_reg->AFE_CTL[7] = 0x00001009;
#endif
#if 1
	prn_string("VCO: 3.2G, PLL: 0.8G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FF;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90000000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F330823;
	cpiol_reg->AFE_CTL[7] = 0x00001009;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 1.6G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90000000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F331823;
	cpiol_reg->AFE_CTL[7] = 0x00001001;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 2.5G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90081000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F331CA3;
	cpiol_reg->AFE_CTL[7] = 0x00001001;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 1.25G\n");
	cpiol_reg->AFE_CTL[0] = 0x240183FA;
	cpiol_reg->AFE_CTL[1] = 0x2527BEF9;
	cpiol_reg->AFE_CTL[2] = 0x00000000;
	cpiol_reg->AFE_CTL[3] = 0x90081000;
	cpiol_reg->AFE_CTL[4] = 0x000000E4;
	cpiol_reg->AFE_CTL[5] = 0x00004801;
	cpiol_reg->AFE_CTL[6] = 0x1F330A23;
	cpiol_reg->AFE_CTL[7] = 0x00001009;
#endif

#if 0
	cpiol_reg->AFE_CTL[0] |= (0x1F<<10);	// XOVER mode all
#endif
#if 1
	cpiol_reg->AFE_CTL[0] &= ~(0x1<<15);	// Disable analog loopback
#endif

	if (re_cfg == 0x1) {
		cpiol_reg->AFE_CTL[5] &= ~(0x7<<9);
		cpiol_reg->AFE_CTL[5] |= ((cfg_v&0x7)<<9);

		cpiol_reg->AFE_CTL[5] &= ~(0x7<<13);
		cpiol_reg->AFE_CTL[5] |= ((cfg_v0&0x7)<<13);

		cpiol_reg->AFE_CTL[3] &= ~(0x7<<26);
		cpiol_reg->AFE_CTL[3] |= ((cfg_v1&0x7)<<26);
		cpiol_reg->AFE_CTL[4] &= ~(0x7);
		cpiol_reg->AFE_CTL[4] |= (cfg_v1&0x7);

		cpiol_reg->AFE_CTL[3] &= ~(0x7<<29);
		cpiol_reg->AFE_CTL[3] |= ((cfg_v2&0x7)<<29);
		cpiol_reg->AFE_CTL[4] &= ~(0x7<<3);
		cpiol_reg->AFE_CTL[4] |= ((cfg_v2&0x7)<<3);

		cpiol_reg->AFE_CTL[0] |= ((cfg_v3&0x20)<<12);	// POL_INV
		cpiol_reg->AFE_CTL[0] |= (0x1<<30);		// RCK dly en1
		cpiol_reg->AFE_CTL[1] |= ((cfg_v3&0x10)<<26);	// RCK dly en2
		cpiol_reg->AFE_CTL[2] |= (cfg_v3<<12);		// RCK dly value
	}

	prn_string("Print All AFE_CTL register\n");
	for (i = 0; i < 8; i++) {
		prn_dword(cpiol_reg->AFE_CTL[i]);
	}

	for (i = 0; i < 10; i++) {
		tmp_data0[i] = cpiol_reg->TEST_D[4];
	}
	for (i = 0; i < 10; i++) {
		tmp_data1[i] = cpiol_reg->TEST_D[5];
	}

	cpiol_reg->TEST_C[0] |= (0x1<<4);

	for (i = 0; i < 10; i++) {
		tmp_data2[i] = cpiol_reg->TEST_D[4];
	}
	for (i = 0; i < 10; i++) {
		tmp_data3[i] = cpiol_reg->TEST_D[5];
	}

	prn_string("\nPrint TX datas\n");
	prn_string("CH0 data: "); prn_dword(cpiol_reg->TEST_D[0]);
	prn_string("CH1 data: "); prn_dword(cpiol_reg->TEST_D[1]);
	prn_string("CH2 data: "); prn_dword(cpiol_reg->TEST_D[2]);
	prn_string("CH3 data: "); prn_dword(cpiol_reg->TEST_D[3]);

	prn_string("\nPrint RX datas\n");
	prn_string("CH0 data:\n");
	for (i = 0; i < 10; i++) {
		prn_dword(tmp_data0[i]);
	}
	prn_string("CH1 data:\n");
	for (i = 0; i < 10; i++) {
		prn_dword(tmp_data1[i]);
	}
	prn_string("CH2 data:\n");
	for (i = 0; i < 10; i++) {
		prn_dword(tmp_data2[i]);
	}
	prn_string("CH3 data:\n");
	for (i = 0; i < 10; i++) {
		prn_dword(tmp_data3[i]);
	}

	ecnt0 = 0x0;
	ecnt1 = 0x0;
	ecnt2 = 0x0;
	ecnt3 = 0x0;
	for (i = 0; i < 10; i++) {
		if (tmp_data0[i] != tmp_data0[0])
			ecnt0++;
		if (tmp_data1[i] != tmp_data1[0])
			ecnt1++;
		if (tmp_data2[i] != tmp_data2[0])
		ecnt2++;
	if (tmp_data3[i] != tmp_data3[0])
		ecnt3++;
	}

	if (ecnt0 != 0x0)
		prn_string("CH0 all data not equal\n");
	if (ecnt1 != 0x0)
		prn_string("CH1 all data not equal\n");
	if (ecnt2 != 0x0)
		prn_string("CH2 all data not equal\n");
	if (ecnt3 != 0x0)
		prn_string("CH3 all data not equal\n");

	ecnt = 0x1;
	for (i = 0; i < 32; i++) {
		tmp_data = (tmp_data0[0]>>i) | (tmp_data0[0]<<(32-i));
		if (tmp_data == cpiol_reg->TEST_D[0]) {
			ecnt = 0x0;
			break;
		}
		//prn_dword(tmp_data);
	}
	if (ecnt == 0x1) {
		prn_string("CH0 RX data check fail\n");
	}
	ecnt0 += ecnt;
	if (ecnt0 != 0x0) {
		prn_string("CH0 test with error count: "); prn_dword(ecnt0);
	} else {
		prn_string("CH0 test pass\n");
	}

	ecnt = 0x1;
	for (i = 0; i < 32; i++) {
		tmp_data = (tmp_data1[0]>>i) | (tmp_data1[0]<<(32-i));
		if (tmp_data == cpiol_reg->TEST_D[1]) {
			ecnt = 0x0;
			break;
		}
		//prn_dword(tmp_data);
	}
	if (ecnt == 0x1) {
		prn_string("CH1 RX data check fail\n");
	}
	ecnt1 += ecnt;
	if (ecnt1 != 0x0) {
		prn_string("CH1 test with error count: "); prn_dword(ecnt1);
	} else {
		prn_string("CH1 test pass\n");
	}

	ecnt = 0x1;
	for (i = 0; i < 32; i++) {
		tmp_data = (tmp_data2[0]>>i) | (tmp_data2[0]<<(32-i));
		if (tmp_data == cpiol_reg->TEST_D[2]) {
			ecnt = 0x0;
			break;
		}
		//prn_dword(tmp_data);
	}
	if (ecnt == 0x1) {
		prn_string("CH2 RX data check fail\n");
	}
	ecnt2 += ecnt;
	if (ecnt2 != 0x0) {
		prn_string("CH2 test with error count: "); prn_dword(ecnt2);
	} else {
		prn_string("CH2 test pass\n");
	}

	ecnt = 0x1;
	for (i = 0; i < 32; i++) {
		tmp_data = (tmp_data3[0]>>i) | (tmp_data3[0]<<(32-i));
		if (tmp_data == cpiol_reg->TEST_D[3]) {
			ecnt = 0x0;
			break;
		}
		//prn_dword(tmp_data);
	}
	if (ecnt == 0x1) {
		prn_string("CH3 RX data check fail\n");
	}
	ecnt3 += ecnt;
	if (ecnt3 != 0x0) {
		prn_string("CH3 test with error count: "); prn_dword(ecnt3);
	} else {
		prn_string("CH3 test pass\n");
	}
}

void gpo_test(int tnum)
{
	prn_string("GPO test set\n");
	cpiol_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | (tnum<<4) | 0x5);
	while ((cpiol_reg->TEST_C[1]) & (0x1 << 31));

	prn_string("GPO test run\n");
	cpiol_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | 0x5);
	while ((cpiol_reg->TEST_C[1]) & (0x1 << 31));

	if ((cpiol_reg->IOP_STS) & (0xF << 20)) {
		prn_string("Test Fail!!!\n");
		prn_dword(cpiol_reg->IOP_STS);
	} else {
		prn_string("Test Pass!!\n");
	}
}

void gpo_test1(void)
{
	int i;

	prn_dword(cpiol_reg->PHY_CTRL);
	prn_string("GPO test for clock\n");
	gpo_test(1);

	prn_string("GPO test for valid\n");
	gpo_test(3);

	prn_string("GPO test for ready\n");
	gpo_test(4);

	for (i = 0; i < 0x8; i++) {
		prn_string("GPO test for data "); prn_dword(i);
		gpo_test(i | 0x8);
	}

	prn_string("GPO test for all pads\n");
	gpo_test(2);
	while (1) {
		cpiol_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | 0x5);
		while ((cpiol_reg->TEST_C[1]) & (0x1 << 31));
	}
}

void cpio_test(void)
{
#if 0
	prn_dword(cpiol_reg->PHY_CTRL);
	while (1);
#endif

	unsigned int data_tmp;
	int i;

	prn_string("\n\n--- CPIO-L test Begin ----\n\n");
	*(volatile u32 *)(0xf80001E4) = 0xC0000000;	// Disable MIPI enable
	*(volatile u32 *)(0xf8000084) = 0x02000000;	// Close UA1 pinmux
	*(volatile u32 *)(0xf8000180) = 0x003F0001;	// Select CPIOL probeout
	*(volatile u32 *)(0xf8000098) = 0x08000800;  	// Enable probe pinmux;
	cpiol_reg->AFE_CTL[7] |= 0x1;			// enable test clock output

	data_tmp = *(volatile u32 *)(0xf8000058);
	if ((data_tmp & 0x6) != 0x0) {
		prn_string("CPIO system reset not released, releasing!!\n");
		*(volatile u32 *)(0xf8000058) = (0x6<<16);
		STC_delay_us (100);
	}

	//cpiol_reg->PHY_CTRL = (0x1<<19);			// Disable auto initial;
	//cpiol_reg->PHY_CTRL = (0x1<<6);
	//cpiol_reg->PHY_CTRL |= ((0x1<<5) | (0x1<<(5+16)));	// internal pad

	//cpiol_reg->AFE_CTL[1] |= (0x3<<8);
	cpiol_reg->AFE_CTL[7] |= (0x1<<8);	// GPO DS;
	cpiol_reg->AFE_CTL[7] |= (0x1<<9);

	//cpiol_reg->TEST_C[1] |= (0x1<<9);	// force control
	//cpiol_reg->AFE_CTL[0] |= (0x1F<<10);	// XOVER all

	cpiol_reg->PHY_CTRL |= (0x1<<7); 	// Release ref reset

	STC_delay_us(100);

	//gpo_test1();

#if 0
	cpio_reset(1);
	prn_string("ATPG Test\n\n");
	cpiol_reg->IO_TCTL |= (0x3<<18);
	STC_delay_us (1);
	cpio_taxi_0();
	STC_delay_us (100);

	#if 0
	prn_dword(cpiol_reg->PHY_CTRL);
	prn_string("GPO test set\n");
	cpiol_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | (0x2<<4) | 0x5);
	while ((cpiol_reg->TEST_C[1]) & (0x1 << 31));
	prn_string("GPIO test run\n");
	cpiol_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | 0x5);
	while (1) {
		while ((cpiol_reg->TEST_C[1]) & (0x1 << 31));
		cpiol_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | 0x5);
		//prn_string("GPO test\n");
	}
	#endif
	cpiol_reg->TEST_C[0] |= ((0x1<<6) | (0xF));
	cpiol_reg->TEST_C[0] |= (0x1<<31);
	prn_dword(cpiol_reg->TEST_C[0]);
	prn_dword(cpiol_reg->IOP_STS);
	prn_dword(cpiol_reg->TEST_D[4]);
	prn_dword(cpiol_reg->TEST_D[5]);
	prn_string("TEST result\n");
	prn_dword(cpiol_reg->TEST_C[0]);
	prn_dword(cpiol_reg->IOP_STS);
	prn_dword(cpiol_reg->TEST_D[4]);
	prn_dword(cpiol_reg->TEST_D[5]);
	STC_delay_1ms (10);
	cpiol_reg->TEST_C[0] &= ~(0x1<<31);
	prn_string("TEST result\n");
	cpiol_reg->TEST_C[0] &= ~(0x3<<4);
	prn_dword(cpiol_reg->TEST_C[0]);
	prn_dword(cpiol_reg->IOP_STS);
	prn_dword(cpiol_reg->TEST_D[4]);
	prn_dword(cpiol_reg->TEST_D[5]);
	prn_string("TEST result\n");
	cpiol_reg->TEST_C[0] |= (0x1<<4);
	prn_dword(cpiol_reg->TEST_C[0]);
	prn_dword(cpiol_reg->IOP_STS);
	prn_dword(cpiol_reg->TEST_D[4]);
	prn_dword(cpiol_reg->TEST_D[5]);
	prn_string("TEST result\n");
	cpiol_reg->TEST_C[0] &= ~(0x3<<4);
	cpiol_reg->TEST_C[0] |= (0x2<<4);
	prn_dword(cpiol_reg->TEST_C[0]);
	prn_dword(cpiol_reg->IOP_STS);
	prn_dword(cpiol_reg->TEST_D[4]);
	prn_dword(cpiol_reg->TEST_D[5]);
	prn_string("TEST result\n");
	cpiol_reg->TEST_C[0] |= (0x3<<4);
	prn_dword(cpiol_reg->TEST_C[0]);
	prn_dword(cpiol_reg->IOP_STS);
	prn_dword(cpiol_reg->TEST_D[4]);
	prn_dword(cpiol_reg->TEST_D[5]);
	while (1);
#endif

#if 0
	ana_test1(0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
	while (1);

	int j, k, m, n;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			for (k = 0; k < 8; k++) {
				for (m = 0; m < 8; m++) {
					for (n = 0; n < 0x40; n++) {
						prn_string("\nTX_ADJ: "); prn_dword0(i);prn_string(" RX_ADJ: "); prn_dword(j);
						prn_string("EQ0_TC: "); prn_dword0(k);prn_string(" EQ1_TC: "); prn_dword(m);
						prn_string("RX_DLY: "); prn_dword(n);
						ana_test1(0x1, i, j, k, m, n);
					}
				}
			}
		}
	}

	while (1);
	//ana_test(1);
	//STC_delay_1ms (1);
	//prn_string("Enable XOVER mode\n");
	//ana_test(1);
	//while (1);
#endif
	//ana_test(0);
	//while (1);
#if 0
	cpio_reset(0x0);

	prn_string("Enable Controller loopback in low speed\n\n");
	cpiol_reg->IO_TCTL |= (0x1<<18);
	STC_delay_us (1);
	cpio_taxi_0();

	prn_string("Enable Controller loopback in high speed\n\n");
	cpiol_reg->IO_TCTL |= (0x7<<18);
	STC_delay_us (1);
	cpio_taxi_0();
#endif

#if 0
	cpio_reset(0x1);

	prn_string("Enable DPHY loopback in low speed\n\n");
	cpiol_reg->IO_TCTL |= (0x2<<18);
	STC_delay_us (1);
	cpio_taxi_0();

	prn_string("Switch to high speed in DPHY loopback mode\n");
	cpiol_reg->PHY_CTRL |= 0x1;
	STC_delay_us (100);

	data_tmp = cpiol_reg->PHY_CTRL;
	if ((data_tmp & 0x1) == 0x0) {
		prn_string("Mode Not switched to high speed mode\n");
		prn_dword(cpiol_reg->PHY_CTRL);
		prn_dword(cpiol_reg->IOP_STS);
		while (1);
	}

	data_tmp = cpiol_reg->IOP_STS;
	if (((data_tmp & 0x1) == 0x0) || ((data_tmp & (0x1<<5)) != 0x0) || ((data_tmp & (0x1<<2)) != 0x0)) {
		prn_string("PHY Status check fail\n");
		prn_dword(cpiol_reg->IOP_STS);
		while (1);
	}
	prn_string("Mode switch pass\n");
	STC_delay_us (1);

	cpio_taxi_0();

	prn_string("All Digital Loopback Test Finished!!!\n\n");
#endif

	cpio_reset(0x1);
//	for (k = 0; k < 0x20; k++) {
//	cpio_reset1(((k>>4) & 0x1), (k&0xF));
//	prn_string("\n");
//	prn_dword(k);
	prn_string("Enable APHY loopback in low speed\n\n");
	cpiol_reg->IO_TCTL |= (0x3<<18);
	//cpiol_reg->IO_TCTL |= (0x4<<18);
	STC_delay_us (1);
	cpio_taxi_0();
	STC_delay_us (5);
	prn_dword(cpiol_reg->AFE_CTL[5]);

	prn_string("Switch to high speed in APHY loopback mode\n");
	cpiol_reg->PHY_CTRL |= 0x1;
	//cpiol_reg->PHY_CTRL |= ((0x1<<5) | (0x1<<(5+16)));
	STC_delay_us (100);

	data_tmp = cpiol_reg->PHY_CTRL;
	if ((data_tmp & 0x1) == 0x0) {
		prn_string("Mode Not switched to high speed mode\n");
		prn_dword(cpiol_reg->PHY_CTRL);
		prn_dword(cpiol_reg->IOP_STS);
		cpiol_reg->PHY_CTRL |= 0x1;
		STC_delay_us (100);
		prn_dword(cpiol_reg->PHY_CTRL);
		prn_dword(cpiol_reg->IOP_STS);
		prn_string("Debug Registers\n");
		for (i = 0; i < 8; i++) {
			prn_dword(cpiol_reg->IO_DBG[i]);
		}
		//continue;
		while (1);
	}

	data_tmp = cpiol_reg->IOP_STS;
	if (((data_tmp & 0x1) == 0x0) || ((data_tmp & (0x1<<5)) != 0x0) || ((data_tmp & (0x1<<2)) != 0x0)) {
		prn_string("PHY Status check fail\n");
		prn_dword(cpiol_reg->PHY_CTRL);
		prn_dword(cpiol_reg->IOP_STS);
		prn_string("Debug Registers\n");
		for (i = 0; i < 8; i++) {
			prn_dword(cpiol_reg->IO_DBG[i]);
		}
		//continue;
		while (1);
	}
	prn_string("Mode switch pass\n");
	prn_dword(cpiol_reg->PHY_CTRL);
	STC_delay_us (1);
//	}
//	while (1);

	//while (1) {
	//	cpio_taxi_0();
	//}
	cpio_taxi_0();

	//cpio_taxi_1();

	prn_string("Loopback Test Finished!!!\n\n");
	while (1);
}
#endif // CPIO_TEST

void cpio_slave(void)
{
	int dly_cnt0, dly_cnt1;
	unsigned int data_tmp;

	prn_string("\n\n---- CPIO-L slave mode Begin ----\n\n");
	*(volatile u32 *)(0xf80001E4) = 0xC0000000;	// Disable MIPI enable
	*(volatile u32 *)(0xf8000084) = 0x02000000;	// Close UA1 pinmux
	*(volatile u32 *)(0xf8000180) = 0x003F0001;	// Select CPIOL probeout
	*(volatile u32 *)(0xf8000098) = 0x08000800;	// Enable probe pinmux;

	data_tmp = *(volatile u32 *)(0xf8000058);
	if ((data_tmp & 0x6) != 0x0) {
		prn_string("CPIO system reset not released, releasing!!\n");
		*(volatile u32 *)(0xf8000058) = (0x6<<16);
		STC_delay_us (100);
	}

	// Change SN
	*(volatile u32 *)(0xf8105228) = 0xA5AA;
	*(volatile u32 *)(0xf810522C) = 0x5A5A;

	prn_string("VCO: 3.2G, PLL: 0.8G\n");
	*(volatile u32 *)(0xf8105230) = 0x2401FFFA;
	*(volatile u32 *)(0xf8105234) = 0x2527BEF9;
	*(volatile u32 *)(0xf8105238) = 0x00000000;
	*(volatile u32 *)(0xf810523C) = 0x90000000;
	*(volatile u32 *)(0xf8105240) = 0x000000E4;
	*(volatile u32 *)(0xf8105244) = 0x00004801;
	*(volatile u32 *)(0xf8105248) = 0x1F330B23;
	*(volatile u32 *)(0xf810524C) = 0x00001209;

	// Force XOVER
	*(volatile u32 *)(0xf810521C) |= (0x1<<9);

	STC_delay_1ms (100);

	AV1_STC_init();	// initial STC1

	// Release RESET
	dly_cnt0 = AV1_GetStc32();
	*(volatile u32 *)(0xf8105224) = 0x80;//(0x1<<7);
	while (((*(volatile u32 *)(0xf8105220) & 0x7F) == 0x0)) {
		//prn_dword(*(volatile u32 *)(0xf8105220));
	}
	dly_cnt1 = AV1_GetStc32();
	data_tmp = *(volatile u32 *)(0xf8105220);
	prn_string("PHY status change: "); prn_dword(data_tmp);
	if ((data_tmp & 0x7E) != 0x0) {
		prn_string("PHY status check fail!!!\n");
		if ((data_tmp & 0x2) != 0x0)
			prn_string("Low power status changed\n");
		if ((data_tmp & 0x4) != 0x0)
			prn_string("Decode Error!!!\n");
		if ((data_tmp & 0x8) != 0x0)
			prn_string("Connection detection fail!!!\n");
		if ((data_tmp & 0x10) != 0x0)
			prn_string("Lane detection fail!!!\n");
		if ((data_tmp & 0x20) != 0x0)
			prn_string("Mode switch fail!!!\n");
		if ((data_tmp & 0x40) != 0x0)
			prn_string("Location detection timeout!!!\n");
		prn_string("CPIO Initial Finished\n");
	} else {
		prn_string("PHY status check Passed\n");
		prn_string("CPIO Initial Finished\n");
	}
	data_tmp = *(volatile u32 *)(0xf8105224);
	prn_string("PHY Mode: "); prn_dword(data_tmp);
	prn_string("Timer start: "); prn_dword(dly_cnt0);
	prn_string("Timer End: "); prn_dword(dly_cnt1);

	while (1);
}

void cpio_master(void)
{
	int dly_cnt0, dly_cnt1;
	unsigned int data_tmp;

	prn_string("\n\n---- CPIO-L master mode Begin ----\n\n");
	*(volatile u32 *)(0xf80001E4) = 0xC0000000;	// Disable MIPI enable
	*(volatile u32 *)(0xf8000084) = 0x02000000;	// Close UA1 pinmux
	*(volatile u32 *)(0xf8000180) = 0x003F0001;	// Select CPIOL probeout
	*(volatile u32 *)(0xf8000098) = 0x08000800;	// Enable probe pinmux;

	data_tmp = *(volatile u32 *)(0xf8000058);
	if ((data_tmp & 0x6) != 0x0) {
		prn_string("CPIO system reset not released, releasing!!\n");
		*(volatile u32 *)(0xf8000058) = (0x6<<16);
		STC_delay_us (100);
	}

	// Change SN
	*(volatile u32 *)(0xf8105228) = 0xA5AA;
	*(volatile u32 *)(0xf810522C) = 0x80005A5A;

	prn_string("VCO: 3.2G, PLL: 0.8G\n");
	*(volatile u32 *)(0xf8105230) = 0x240183FA;
	*(volatile u32 *)(0xf8105234) = 0x2527BEF9;
	*(volatile u32 *)(0xf8105238) = 0x00000000;
	*(volatile u32 *)(0xf810523C) = 0x90000000;
	*(volatile u32 *)(0xf8105240) = 0x000000E4;
	*(volatile u32 *)(0xf8105244) = 0x00004801;
	*(volatile u32 *)(0xf8105248) = 0x1F330B23;
	*(volatile u32 *)(0xf810524C) = 0x00001209;

	// Force XOVER
	//*(volatile u32 *)(0xf810521C) |= (0x1<<9);

	STC_delay_1ms (100);

	AV1_STC_init();	// initial STC1

	// Release RESET
	dly_cnt0 = AV1_GetStc32();
	*(volatile u32 *)(0xf8105224) = 0x80;//(0x1<<7);
	while (((*(volatile u32 *)(0xf8105220) & 0x7F) == 0x0)) {
		//prn_dword(*(volatile u32 *)(0xf8105220));
	}
	dly_cnt1 = AV1_GetStc32();
	data_tmp = *(volatile u32 *)(0xf8105220);
	prn_string("PHY status change: "); prn_dword(data_tmp);
	if ((data_tmp & 0x7E) != 0x0) {
		prn_string("PHY status check fail!!!\n");
		if ((data_tmp & 0x2) != 0x0)
			prn_string("Low power status changed\n");
		if ((data_tmp & 0x4) != 0x0)
			prn_string("Decode Error!!!\n");
		if ((data_tmp & 0x8) != 0x0)
			prn_string("Connection detection fail!!!\n");
		if ((data_tmp & 0x10) != 0x0)
			prn_string("Lane detection fail!!!\n");
		if ((data_tmp & 0x20) != 0x0)
			prn_string("Mode switch fail!!!\n");
		if ((data_tmp & 0x40) != 0x0)
			prn_string("Location detection timeout!!!\n");
		prn_string("CPIO Initial Finished\n");
	} else {
		prn_string("PHY status check Passed\n");
		prn_string("CPIO Initial Finished\n");
	}
	data_tmp = *(volatile u32 *)(0xf8105224);
	prn_string("PHY Mode: "); prn_dword(data_tmp);
	prn_string("Timer start: "); prn_dword(dly_cnt0);
	prn_string("Timer End: "); prn_dword(dly_cnt1);

	while (1);
}
