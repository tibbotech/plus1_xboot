#include <types.h>
#include <usb/ehci_usb.h>
#include <fat/fat.h>
#include <common.h>

#ifdef FPGA
#define EHCI_DEBUG
#endif

#ifdef XBOOT_BUILD
#define USB_DBG()
#else
#define USB_DBG()   dbg()
#define USB_PRN
#endif

void USB_requestSense(void);
UINT32 USB_transfer(UINT32 wLen, BYTE *buffer);
UINT32 doTransfer(BYTE *buf);
void USB_vendorCmd(UINT32 bReq,UINT32 bCmd,UINT32 wValue,UINT32 wIndex,UINT32 wLen);
int USB_testUnitReady(void);

extern void _delay_1ms(UINT32 period); // force delay even in CSIM
extern void boot_reset(void);

#define MAX_TEST_UNIT_READY_TRY   5

// usb spec 2.0 Table 7-3  VHSDSC (min, max) = (525, 625)
// default = 586.5 mV (405 + 11 * 16.5)
// update  = 619.5 mV (405 + 13 * 16.5)
#define DEFAULT_UPHY_DISC   0xd   // 13 (=619.5mV)
#define ORIG_UPHY_DISC      0xb   // 11 (=586.5mV)

// UPHY 0 & 1 init (dh_feng)
void uphy_init(void)
{
	unsigned int val, set;

	// 1. Default value modification
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
	MOON1_REG->sft_cfg[14] = 0x87474002;
        MOON1_REG->sft_cfg[15] = 0x87474004;
#else
	/* Q628 uphy0_ctl uphy1_ctl */
	MOON4_REG->uphy0_ctl[0] = RF_MASK_V(0xffff, 0x4002);
	MOON4_REG->uphy0_ctl[1] = RF_MASK_V(0xffff, 0x8747);
	MOON4_REG->uphy1_ctl[0] = RF_MASK_V(0xffff, 0x4004);
	MOON4_REG->uphy1_ctl[1] = RF_MASK_V(0xffff, 0x8747);
#endif

	// 2. PLL power off/on twice
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
	MOON1_REG->sft_cfg[21] = 0x8888;
	_delay_1ms(1);
	MOON1_REG->sft_cfg[21] = 0x8080;
	_delay_1ms(1);
	MOON1_REG->sft_cfg[21] = 0x8888;
	_delay_1ms(1);
	MOON1_REG->sft_cfg[21] = 0x8080;
	_delay_1ms(1);
	MOON1_REG->sft_cfg[21] = 0;
#else
	/* Q628 uphy012_ctl */
	MOON4_REG->uphy0_ctl[3] = RF_MASK_V(0xffff, 0x8888);
	_delay_1ms(1);
	MOON4_REG->uphy0_ctl[3] = RF_MASK_V(0xffff, 0x8080);
	_delay_1ms(1);
	MOON4_REG->uphy0_ctl[3] = RF_MASK_V(0xffff, 0x8888);
	_delay_1ms(1);
	MOON4_REG->uphy0_ctl[3] = RF_MASK_V(0xffff, 0x8080);
	_delay_1ms(1);
	MOON4_REG->uphy0_ctl[3] = RF_MASK_V(0xffff, 0);
#endif

	// 3. reset UPHY0
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
	MOON0_REG->reset[1] |= (3 << 13);
	MOON0_REG->reset[1] &= ~(3 << 13);
#else
	/* Q628 UPHY0_RESET UPHY1_RESET : 1->0 */
	MOON0_REG->reset[2] = RF_MASK_V_SET(3 << 13);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(3 << 13);
#endif
	_delay_1ms(1);

	// 4. UPHY 0 internal register modification
	UPHY0_RN_REG->cfg[7] = 0x8b;
#ifndef PLATFORM_I137
	UPHY1_RN_REG->cfg[7] = 0x8b;
#endif

	// 5. USBC 0 reset
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
	MOON0_REG->reset[1] |= (3 << 10);
	MOON0_REG->reset[1] &= ~(3 << 10);
#else
	/* Q628 USBC0_RESET USBC1_RESET : 1->0 */
	MOON0_REG->reset[2] = RF_MASK_V_SET(3 << 10);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(3 << 10);
#endif

	CSTAMP(0xE5B0A000);

	// uphy rx clk: invert (avoid 8388 uphy clock bug)
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
	MOON1_REG->sft_cfg[19] |= (1 << 6);
#else
	/* Q628 uphy0_ctl_2 uphy1_ctl_2 */
	MOON4_REG->uphy0_ctl[2] = RF_MASK_V_SET(1 << 6);
	MOON4_REG->uphy1_ctl[2] = RF_MASK_V_SET(1 << 6);
#endif

	CSTAMP(0xE5B0A001);

        // OTP for USB DISC (disconnect voltage)
#if defined(PLATFORM_8388)
        val = HB_GP_REG->hb_otp_data6;
        set = val & 0x1F; // UPHY0 DISC
        if (!set) {
                set = DEFAULT_UPHY_DISC;
        } else if (set <= ORIG_UPHY_DISC) {
                set += 2;
        }
        UPHY0_RN_REG->cfg[7] = (UPHY0_RN_REG->cfg[7] & ~0x1F) | set;
        set = (val >> 5) & 0x1F; // UPHY1 DISC
        if (!set) {
                set = DEFAULT_UPHY_DISC;
        } else if (set <= ORIG_UPHY_DISC) {
                set += 2;
        }
        UPHY1_RN_REG->cfg[7] = (UPHY1_RN_REG->cfg[7] & ~0x1F) | set;
#else
	/* Q628 OTP[UPHY0_DISC] OTP[UPHY1_DISC] */
	val = HB_GP_REG->hb_otp_data6;
        set = val & 0x1F; // UPHY0 DISC
        if (!set) {
                set = DEFAULT_UPHY_DISC;
        } else if (set <= ORIG_UPHY_DISC) {
                set += 2;
        }
        UPHY0_RN_REG->cfg[7] = (UPHY0_RN_REG->cfg[7] & ~0x1F) | set;
#ifndef PLATFORM_I137
        set = (val >> 5) & 0x1F; // UPHY1 DISC
        if (!set) {
                set = DEFAULT_UPHY_DISC;
        } else if (set <= ORIG_UPHY_DISC) {
                set += 2;
        }
        UPHY1_RN_REG->cfg[7] = (UPHY1_RN_REG->cfg[7] & ~0x1F) | set;
#endif
#endif

	CSTAMP(0xE5B0A002);
}

void usb_power_init(int is_host)
{
	// a. enable pin mux control
	//    Host: enable
	//    Device: disable
#ifdef PLATFORM_8388
	if (is_host) {
		MOON1_REG->sft_cfg[8] |= (3 << 2);
	} else {
		MOON1_REG->sft_cfg[8] &= ~(3 << 2);
	}
#elif defined(PLATFORM_I137)
	if (is_host) {
		MOON1_REG->sft_cfg[5] |= (1 << 2);
	} else {
		MOON1_REG->sft_cfg[5] &= ~(1 << 2);
	}
#else
	/* Q628 USBC0_OTG_EN_SEL USBC1_OTG_EN_SEL */
	if (is_host) {
		MOON1_REG->sft_cfg[3] = RF_MASK_V_SET(3 << 2);
	} else {
		MOON1_REG->sft_cfg[3] = RF_MASK_V_CLR(3 << 2);
	}
#endif

	// b. USB control register:
#ifdef PLATFORM_8388
	//    Host:   ctrl=1, host sel=1, type=1
	//    Device  ctrl=1, host sel=0, type=0
	if (is_host) {
		MOON2_REG->sft_cfg[3] |= ((7 << 12) | (7 << 4));
	} else {
		MOON2_REG->sft_cfg[3] |= ((1 << 12) | (1 << 4));
		MOON2_REG->sft_cfg[3] &= ~((3 << 13) | (3 << 5));
	}
#elif defined(PLATFORM_I137)
	//    Host:   host sel=1, type=1
	//    Device  host sel=0, type=0
	if (is_host) {
		MOON2_REG->sft_cfg[3] |= (3 << 5);
	} else {
		MOON2_REG->sft_cfg[3] &= ~(3 << 5);
	}
#else
	/* Q628 USBC0_TYPE, USBC0_SEL, USBC1_TYPE, USBC1_SEL */
	if (is_host) {
		MOON4_REG->usbc_ctl = RF_MASK_V_SET((3 << 13) | (3 << 5));
	} else {
		MOON4_REG->usbc_ctl = RF_MASK_V_CLR((3 << 13) | (3 << 5));
	}
#endif
}

int usb_init(int port)
{
	UINT32 tmp1, tmp2;
	unsigned int dev_dct_cnt = 0;

	CSTAMP(0xE5B00000);
	dbg();

	memset32((u32 *)&g_io_buf.usb.ehci, 0, sizeof(g_io_buf.usb.ehci)/4);

	// ehci register base
	g_io_buf.usb.ehci.ehci_hcd_regs = port ? EHCI1_REG : EHCI0_REG;

#ifndef EHCI_DEBUG
tryagain:
#endif
	dbg();

	CSTAMP(0xE5B00001);
	uphy_init();

	CSTAMP(0xE5B00002);
	usb_power_init(1);

	// init EHCI

	/*Enable ehci test*/
#if 0
	{
		u32 reg_tmp;
		reg_tmp = EHCI_PORTSC;
		reg_tmp &= (7<<16);
		reg_tmp |= (4<<16);
		EHCI_PORTSC = reg_tmp;

		prn_dword(EHCI_PORTSC);
		prn_dword(reg_tmp);
		prn_string("Test mode \n"); 
		while(1);
	}
#endif

	CSTAMP(0xE5B00003);
	prn_string("\nReset EHCI\n");
	EHCI_USBCMD |= EHCI_SFT_RESET;
	while (EHCI_USBCMD&EHCI_SFT_RESET) {
#ifdef EHCI_DEBUG
		prn_string("Wait Reset Done...\n");
		_delay_1ms(1);
#endif
	}

	/* set USB port to EHCI */
	EHCI_CONFIG = SET_EHCI;

	/* detect device in */
	//prn_string("Dect device\n");
	while((EHCI_PORTSC&PORT_CONNECT) == 0){

		_delay_1ms(1);

#if defined(CSIM_NEW) || defined(ASIC_TEST)
		if (dev_dct_cnt++ > 20) { // 20ms
#else
		if (dev_dct_cnt++ > 2500 ) { // 2.5s
#endif
			CSTAMP(0xE5B00004);
			prn_string("\nNo usb device on port ");
			prn_decimal(port);
			prn_string("\n");
			return -1;
		}

		// print DOT every  100 ms
		if (!(dev_dct_cnt % 100)) {
			CSTAMP(0xE5B0CCCC);
			prn_string(".");
		}
	}

	prn_string("\nusb"); prn_decimal(port);
	prn_string(" connected(");
	prn_decimal(dev_dct_cnt);
	prn_string("ms)\n");

	CSTAMP(0xE5B00005);

	dev_dct_cnt = 0;

	/* Reset device */
	//prn_string("Reset device\n");
	EHCI_PORTSC |= PORT_RESET;
#ifdef CSIM_NEW
	_delay_1ms(10);
#else
	_delay_1ms(50);
#endif
	EHCI_PORTSC &= (~PORT_RESET);

	// DON'T PRINT HERE to avoid delay
	// Implicit delay is not desired here

	// Put exact delay (500us ~ 1ms)
	STC_REG->stc_15_0 = 0;
	while (STC_REG->stc_15_0 < 50); // 555us

	// if this delsy is too short or too long,
	// USB_transfer() has error USBSTS=0x0000A007

	// usb controller will only set this bit to a one when the reset sequence
	// determines that the attached device is a high-speed device
	while ((EHCI_PORTSC & PORT_ENABLE) == 0) {

		if (dev_dct_cnt++ > 5) { // 50 ms
			prn_string("portsc="); prn_dword(EHCI_PORTSC);
			CSTAMP(0xE5B00006);
			prn_string("not HS, retry\n");
			goto tryagain;
		}

		CSTAMP(0xE5B0A000);
		_delay_1ms(10); // wait for port reset
	}

	CSTAMP(0xE5B00007);

	EHCI_USBCMD |= EN_SCHEDULE;
	_delay_1ms(3);

	//enum USB disk
	pUSB_CfgDesc pCfg = (pUSB_CfgDesc)(USB_dataBuf);
	pUSB_DevDesc pDev = (pUSB_DevDesc)(USB_dataBuf);

	//EHCI_ASYNC_LISTADDR = CACHE_TO_BUS(&EHCI_QH);
	EHCI_ASYNC_LISTADDR = (UINT32)(&EHCI_QH);

	// Init QH
	//EHCI_QH.hw_next = (1<<1)|CACHE_TO_BUS(&EHCI_QH);
	EHCI_QH.hw_next = (1<<1)|((UINT32)(&EHCI_QH));
	//EHCI_QH.hw_info1 = QH_HEAD;
	EHCI_QH.hw_info2 = 1<<30;
	//EHCI_QH.hw_current = CACHE_TO_BUS(&EHCI_TD);
	EHCI_QH.hw_current = (UINT32)(&EHCI_TD);

	EHCI_QH.hw_qtd_next = EHCI_LIST_END;
	EHCI_QH.hw_alt_next = EHCI_LIST_END;
	EHCI_QH.hw_token = TD_STS_HALT;
	//EHCI_QH.hw_buf[0] = 0;

	// Init td
	EHCI_TD.hw_qtd_next = EHCI_LIST_END;
	EHCI_TD.hw_alt_next = EHCI_LIST_END;
	EHCI_TD.hw_token = TD_STS_HALT;
	//EHCI_TD.hw_buf[0] = 0;

	//EHCI_TD_info0 = 0x00400000;
	EHCI_TD_infoOut = 0x02000000;
	EHCI_TD_infoIn = 0x02000000;

	prn_string("enum u-disk\n");

	EHCI_addr = 0;

	CSTAMP(0xE5B00008);
	prn_string("get dev desc (64)\n");
	USB_vendorCmd(0x80, GET_DESCRIPTOR, DESC_DEVICE, 0, 0x40);

	prn_string("vid="); prn_byte((pDev->idVendor  >> 8) & 0xff); prn_byte(pDev->idVendor & 0xff);
	prn_string("pid=");         prn_byte((pDev->idProduct >> 8) & 0xff); prn_byte(pDev->idProduct & 0xff);
	prn_string("rev=");    prn_byte((pDev->bcdDevice >> 8) & 0xff); prn_byte(pDev->bcdDevice & 0xff);
	prn_string("\n");

	CSTAMP(0xE5B00009);
	prn_string("set addr\n");
	USB_vendorCmd(0, SET_ADDRESS, 0x02, 0, 0);
	EHCI_addr = 0x02;

	CSTAMP(0xE5B0000A);
	prn_string("get dev desc ("); prn_decimal(pDev->bLength); prn_string(")\n");
	USB_vendorCmd(0x80, GET_DESCRIPTOR, DESC_DEVICE, 0, (pDev->bLength));

	CSTAMP(0xE5B0000B);
	prn_string("get conf desc (9)\n");
	USB_vendorCmd(0x80, GET_DESCRIPTOR, DESC_CONFIGURATION, 0, 9);

	CSTAMP(0xE5B0000C);
	prn_string("get conf desc ("); prn_decimal(pDev->bLength); prn_string(")\n");
	USB_vendorCmd(0x80, GET_DESCRIPTOR, DESC_CONFIGURATION, 0, (pCfg->wLength));

	tmp1 = USB_dataBuf[9+9+2];
	tmp2 = USB_dataBuf[9+9+7+2];

	if (tmp1 & 0x80) {
		CSTAMP(0xE5B0000D);
		EHCI_inEndpoint = tmp1 & 0x0F;
		EHCI_outEndpoint = tmp2 & 0x0F;
	} else {
		CSTAMP(0xE5B0000E);
		EHCI_outEndpoint = tmp1 & 0x0F;
		EHCI_inEndpoint = tmp2 & 0x0F;
	}

#ifdef EHCI_DEBUG
	prn_string("In  Endpoint "); prn_dword(EHCI_inEndpoint);
	prn_string("Out Endpoint "); prn_dword(EHCI_outEndpoint);
#endif

	CSTAMP(0xE5B0000F);
	prn_string("set config "); prn_decimal(pCfg->bCV); prn_string("\n");
	USB_vendorCmd(0, SET_CONFIG, (pCfg->bCV), 0, 0);

	// Test Unit Ready
	tmp2 = 0;
	for (tmp1 = 0; tmp1 < MAX_TEST_UNIT_READY_TRY; tmp1++) {
		prn_string("Test Unit Ready ("); prn_decimal(tmp1); prn_string(")\n");
		tmp2 = USB_testUnitReady();
#ifndef CSIM_NEW
		_delay_1ms(50);
#endif
	}
	if (tmp2) {
		prn_string("unit still not ready...\n");
		boot_reset();	
	}

	return 0;
}

int USB_testUnitReady(void)
{
	UINT32 i;
	UINT32 stallErr;
	sCBW *pcbw;
	pcbw=(sCBW*)(STRUCT_BUF_ADDR);

	CSTAMP(0xE5B00010);
	USB_DBG();

	for (i = 0; i < 31;i++)
		STRUCT_BUF_ADDR[i] = 0;

	stallErr = 0;

	pcbw->dCBWSignature = CBWSignature;
	pcbw->dCBWTag= SCSICMD_TEST_UNIT_READY_CBWTag;
	//pcbw->dCBWDataTransferLength = 0;
	//pcbw->bmCBWFlags = 0;
	//pcbw->bCBWLUN = 0;
	pcbw->bCBWCBLength = 0x06;

	//pcbw->CBWCB[0]= SCSICMD_TEST_UNIT_READY;

	stallErr = doTransfer(0);

	if (stallErr) {
		// note: some udisk always has stall error at 1st try
		prn_string("USB_testUnitReady STALL\n");
		USB_requestSense();
		return 1;
	}

	return 0;
}

void USB_requestSense(void)
{
	UINT32 i;
	sCBW *pcbw;
	pcbw=(sCBW*)(STRUCT_BUF_ADDR);

	CSTAMP(0xE5B00011);

	dbg();
	for (i = 0; i < 16;i++)
		STRUCT_BUF_ADDR[i] = 0;

	pcbw->dCBWSignature = CBWSignature;
	pcbw->dCBWTag = SCSICMD_REQUEST_SENSE_CBWTag;
	pcbw->dCBWDataTransferLength = 0x12;
	pcbw->bmCBWFlags = CBWIn;
	//pcbw->bCBWLUN = 0;
	pcbw->bCBWCBLength = 0x0C;

	pcbw->CBWCB[0] = SCSICMD_REQUEST_SENSE;
	//pcbw->CBWCB[2]=0;
	//pcbw->CBWCB[3]=0;
	pcbw->CBWCB[4]=0x12;
	//pcbw->CBWCB[5]=0;
	//pcbw->CBWCB[8]=0;

	dbg();
	doTransfer(STRUCT_BUF_ADDR);

	CSTAMP(0xE5B00012);
}

/*
 * usb_readSector
 * @lba		LBA to read
 * @count       not used
 * @buf		destination buffer
 */
int usb_readSector(u32 lba, u32 count, u32 *dest)
{
	UINT32 i;
	UINT32 stallErr;
	sCBW *pcbw;
	pcbw=(sCBW*)(STRUCT_BUF_ADDR);

	CSTAMP(0xE5B00013);

	USB_DBG();
	do {
		for (i = 0; i < 31;i++)
			STRUCT_BUF_ADDR[i] = 0;

		stallErr = 0;

		pcbw->dCBWSignature = CBWSignature;
		pcbw->dCBWTag = SCSICMD_READ10_CBWTag;
		pcbw->dCBWDataTransferLength = (count << 9); //default
		pcbw->bmCBWFlags = CBWIn;
		pcbw->bCBWLUN = 0;                  //the LUN of USB device
		pcbw->bCBWCBLength = 0x0a;

		pcbw->CBWCB[0]= SCSICMD_READ_10;
		//pcbw->CBWCB[1] = 0;
		pcbw->CBWCB[2] =(UINT8)((lba)>>24);
		pcbw->CBWCB[3] =(UINT8)((lba)>>16);
		pcbw->CBWCB[4] =(UINT8)((lba)>>8);
		pcbw->CBWCB[5] =(UINT8)((lba));
		//pcbw->CBWCB[7] = 0;     //number of sector: high byte
		pcbw->CBWCB[8] = count;     //number of sector: low  byte

		stallErr = doTransfer((u8*)dest);

		if (stallErr) {
			prn_string("usb_readSector STALL Error, send USB_requestSense\n");
			USB_requestSense();
		}
	} while (stallErr);

	CSTAMP(0xE5B00014);

	return 0;
}

UINT32 USB_transfer(UINT32 wLen, BYTE *buffer)
{
	UINT32 temp;
	UINT32 usb_err;
	UINT32 timeout;

	CSTAMP(0xE5B00015);

#ifdef USB_PRN
	if (((*EHCI_TD_info_ptr>>8)&3) == 1) {
		CSTAMP(0xE5B0D000);
		prn_string("In "); prn_decimal(wLen); prn_string("\n");
	} else if (((*EHCI_TD_info_ptr>>8)&3) == 0) {
		CSTAMP(0xE5B0D001);
		prn_string("Out "); prn_decimal(wLen); prn_string("\n");
	} else {
		CSTAMP(0xE5B0D002);
		prn_string("Setup "); prn_decimal(wLen); prn_string("\n");
	}
#endif

	// set TD
	//EHCI_TD.hw_qtd_next = EHCI_LIST_END;
	//EHCI_TD.hw_alt_next = EHCI_LIST_END;
	*EHCI_TD_info_ptr &= ~TD_TX_BYTE_MASK;  // clear TX len

	if(wLen > 512)
		wLen += 13;
	EHCI_TD.hw_token = (*EHCI_TD_info_ptr | (wLen << 16));

	//EHCI_TD.hw_buf[0] = CACHE_TO_BUS(buffer);

#if 1	/*add by yong.y*/
	{
		int cnt;
		int i;
		UINT32 addr;

		cnt = (int)wLen;
		addr = (UINT32)buffer;
		EHCI_TD.hw_buf[0] = addr;

		//cnt = cnt-(4*1024-addr&0xfff);
		cnt -= 4096;
		addr = (addr + 4096)&(~0xfff);

		for(i = 1;i < 5;i++){

			if(cnt <= 0)
				break;

			EHCI_TD.hw_buf[i] = addr;
			cnt  -= 4*1024;
			addr += 4*1024;
		}
	}
#else
	EHCI_TD.hw_buf[0] = (UINT32)buffer;
#endif

	// set QH
	//EHCI_QH.hw_next = (1<<1)|CACHE_TO_BUS(&EHCI_QH);
	EHCI_QH.hw_info1 = *EHCI_QH_info_ptr|(EHCI_endpoint << 8)|EHCI_addr;
	//EHCI_QH.hw_info2 |= 1<<30;
	//EHCI_QH.hw_current = CACHE_TO_BUS(&EHCI_TD);
	//EHCI_QH.hw_qtd_next = EHCI_LIST_END;
	//EHCI_QH.hw_alt_next = EHCI_LIST_END;

	EHCI_QH.hw_token = EHCI_TD.hw_token;

	EHCI_QH.hw_buf[0] = EHCI_TD.hw_buf[0];
	EHCI_QH.hw_buf[1] = EHCI_TD.hw_buf[1];
	EHCI_QH.hw_buf[2] = EHCI_TD.hw_buf[2];
	EHCI_QH.hw_buf[3] = EHCI_TD.hw_buf[3];
	EHCI_QH.hw_buf[4] = EHCI_TD.hw_buf[4];

	EHCI_USBCMD |= EN_ASYNC_SCHEDULE;

	//dbg();
	while((EHCI_USBSTS & STS_ASS) == 0) {
		//_delay_1ms(1);
	}

	USB_DBG();
	usb_err = 0;
	timeout = 2000; // 2s timeout
	while (--timeout) {
		temp = EHCI_USBSTS;
		if (STS_ERR & temp) {
			CSTAMP(0xE5B00016);
			prn_string("Found an error: usbsts="); prn_dword0(temp);
			prn_string(" portsc="); prn_dword(EHCI_PORTSC);

			// Need Clear stall
			temp = EHCI_endpoint;
			if (((*EHCI_TD_info_ptr>>8)&3) == 1) { //in
				temp |= 0x80;
			}

			//return temp; // Return endpoint

			// we have no good usb error handling -> use reset to avoid random hw error
			dbg();
			usb_err = 1;
			break;
		}

		if (STS_PASS & temp) {
			break;
		}

		_delay_1ms(1);
	};

	//
	// timeout or usb error --> reset to retry
	//
	if (timeout == 0) {
		prn_string("timeout!\n");
		usb_err = 1;
	}
	if (usb_err) {
		prn_string("reset to retry\n");
#ifndef CSIM_NEW
		_delay_1ms(500);
#endif
		boot_reset();
	}

	CSTAMP(0xE5B00017);

	USB_DBG();
	// Clear INT Status
	EHCI_USBSTS &= STS_INT_MASK;

	// Disbale AS,
	EHCI_USBCMD &= (~EN_ASYNC_SCHEDULE);

	if (EHCI_endpoint) {
		*EHCI_TD_info_ptr ^= TD_TOGGLE;
	}

	// make sure AS have disable
	//dbg();
	while(EHCI_USBSTS&STS_ASS) {
		_delay_1ms(1);
	}

	CSTAMP(0xE5B00018);

	return 0;
}

void USB_vendorCmd(UINT32 bReq,UINT32 bCmd,UINT32 wValue,UINT32 wIndex,UINT32 wLen)
{
	CSTAMP(0xE5B00019);

	USB_DBG();
	//prn_string("\nUSB_vendorCmd ");
	//prn_dword(bCmd);
	//prn_string("\n");

	// Set QH, TD
	// 64bytes packet, Data Toggle from TD, High Speed, QH_Head
	EHCI_QH_info0 = EHCI_QH_info0_Default;
	// IOC, CERR, Setup Token, Status ACTIVE
	EHCI_TD_info0 = EHCI_TD_info0_Default;

	EHCI_QH_info_ptr = &EHCI_QH_info0;
	EHCI_TD_info_ptr = &EHCI_TD_info0;

	// init setup packet
	USBsetup *USB_setup = (USBsetup*)(STRUCT_BUF_ADDR);

	USB_setup->bmRequest  = bReq;
	USB_setup->bRequest   = bCmd;
	USB_setup->wValue     = wValue;
	USB_setup->wIndex     = wIndex;
	USB_setup->wLength    = wLen;

	EHCI_endpoint = 0;
	USB_transfer(8, (UINT8*)USB_setup);

	EHCI_TD_info0 &= ~TD_PID_CODE_MASK; // reset PID Code
	EHCI_TD_info0 |= TD_TOGGLE;	        // force DATA1
	if (USB_setup->wLength) {
		EHCI_TD_info0 |= 0x0100;        // set PID to IN Token
		USB_transfer(USB_setup->wLength, (BYTE *)(USB_dataBuf));
	}

	EHCI_TD_info0 ^= 0x0100;            // IN <-> OUT
	USB_transfer(0, (UINT8*)USB_setup);
	USB_DBG();

	CSTAMP(0xE5B0001A);
}

UINT32 doTransfer(BYTE *buf)
{
	CSTAMP(0xE5B0001B);

	//prn_string("\ndoTransfer\n");

	UINT32 stallErr = 0;

	USB_DBG();
	sCBW *pcbw = (sCBW*)(STRUCT_BUF_ADDR);
	sCSW * psCSW = (sCSW*)(STRUCT_BUF_ADDR);

	// CBW Packet
	// 512bytes packet, Data Toggle from TD, High Speed, QH_Head
	EHCI_QH_infoOut = EHCI_QH_infoOut_Default;

	// IOC, CERR, OUT Token, Status ACTIVE
	EHCI_TD_infoOut |= EHCI_TD_infoOut_Default;

	EHCI_QH_info_ptr = &EHCI_QH_infoOut;
	EHCI_TD_info_ptr = &EHCI_TD_infoOut;

	EHCI_endpoint = EHCI_outEndpoint;
	USB_transfer(0x1F,(UINT8*)pcbw);

	// DATA Packet
	// 512bytes packet, Data Toggle from TD, High Speed, QH_Head
	EHCI_QH_infoIn = EHCI_QH_infoIn_Default;

	// IOC, CERR, OUT Token, Status ACTIVE
	EHCI_TD_infoIn |= EHCI_TD_infoIn_Default;

	EHCI_QH_info_ptr = (UINT32*)(&EHCI_QH_infoIn);
	EHCI_TD_info_ptr = &EHCI_TD_infoIn;

	EHCI_endpoint = EHCI_inEndpoint;

	//dbg();
	if (pcbw->dCBWDataTransferLength) {
		stallErr = USB_transfer(pcbw->dCBWDataTransferLength, buf);

		if (stallErr) {
			EHCI_endpoint = 0;
			USB_vendorCmd(0x2, USB_REQ_CLEAR_FEATURE, 0, stallErr , 0);

			EHCI_TD_infoIn &= ~TD_TOGGLE; // Reset toggle to data0
		}
	}

	// CSW Packet
#if 1	/*add by yong.y*/
	if(pcbw->dCBWDataTransferLength > 512){
		CSTAMP(0xE5B0001C);
		psCSW = (sCSW*)(buf+pcbw->dCBWDataTransferLength);

		if(psCSW->dCBWSignature == 0){
			USB_transfer(0x0D, (UINT8*)psCSW);
		}
		//prn_dump_buffer((unsigned char*)psCSW,13);
	} else {
		CSTAMP(0xE5B0001D);
		USB_transfer(0x0D, (UINT8*)psCSW);
	}
#else
	USB_transfer(0x0D, (UINT8*)psCSW);
#endif

	// Check Status
	if (psCSW->bCSWStatus) {
		CSTAMP(0xE5B0001E);
		prn_string("CSW Status Error\n");
		return 2;
	}

	CSTAMP(0xE5B0001F);
	USB_DBG();
	return stallErr;
}

