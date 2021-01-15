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
#if defined(PLATFORM_I143)
void uphy_init(void)
{
	// 1. enable UPHY 0/1 & USBC 0/1 HW CLOCK */
	MOON0_REG->clken[2] = RF_MASK_V_SET(3 << 13);
	MOON0_REG->clken[2] = RF_MASK_V_SET(3 << 10);
	_delay_1ms(1);

	// 2. reset UPHY 0/1
	MOON0_REG->reset[2] = RF_MASK_V_SET(3 << 13);
	_delay_1ms(1);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(3 << 13);
	_delay_1ms(1);

	// 3. Default value modification
	UPHY0_RN_REG->gctrl[0] = 0x18888002;
	UPHY1_RN_REG->gctrl[0] = 0x18888002;
	_delay_1ms(1);

	// 4. PLL power off/on twice
	UPHY0_RN_REG->gctrl[2] = 0x88;
	UPHY1_RN_REG->gctrl[2] = 0x88;
	_delay_1ms(1);
	UPHY0_RN_REG->gctrl[2] = 0x80;
	UPHY1_RN_REG->gctrl[2] = 0x80;
	_delay_1ms(1);
	UPHY0_RN_REG->gctrl[2] = 0x88;
	UPHY1_RN_REG->gctrl[2] = 0x88;
	_delay_1ms(1);
	UPHY0_RN_REG->gctrl[2] = 0x80;
	UPHY1_RN_REG->gctrl[2] = 0x80;
	_delay_1ms(20);
	UPHY0_RN_REG->gctrl[2] = 0x0;
	UPHY1_RN_REG->gctrl[2] = 0x0;

	// 5. USBC 0/1 reset
	MOON0_REG->reset[2] = RF_MASK_V_SET(3 << 10);
	_delay_1ms(1);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(3 << 10);
	_delay_1ms(1);

	// 6. HW workaround
	UPHY0_RN_REG->cfg[19] |= 0x0f;
	UPHY1_RN_REG->cfg[19] |= 0x0f;

	// 7. USB DISC (disconnect voltage)
	UPHY0_RN_REG->cfg[7] = 0x8b;
	UPHY1_RN_REG->cfg[7] = 0x8b;

	// 8. RX SQUELCH LEVEL
	UPHY0_RN_REG->cfg[25] = 0x4;
	UPHY1_RN_REG->cfg[25] = 0x4;
}
#else
void uphy_init(void)
{
	unsigned int val, set;

	// 1. Default value modification
	/* Q628 uphy0_ctl uphy1_ctl */
	MOON4_REG->uphy0_ctl[0] = RF_MASK_V(0xffff, 0x4002);
	MOON4_REG->uphy0_ctl[1] = RF_MASK_V(0xffff, 0x8747);
	MOON4_REG->uphy1_ctl[0] = RF_MASK_V(0xffff, 0x4004);
	MOON4_REG->uphy1_ctl[1] = RF_MASK_V(0xffff, 0x8747);

	// 2. PLL power off/on twice
	/* Q628 uphy012_ctl */
	MOON4_REG->uphy0_ctl[3] = RF_MASK_V(0xffff, 0x88);
	MOON4_REG->uphy1_ctl[3] = RF_MASK_V(0xffff, 0x88);
	_delay_1ms(1);
	MOON4_REG->uphy0_ctl[3] = RF_MASK_V(0xffff, 0x80);
	MOON4_REG->uphy1_ctl[3] = RF_MASK_V(0xffff, 0x80);
	_delay_1ms(1);
	MOON4_REG->uphy0_ctl[3] = RF_MASK_V(0xffff, 0x88);
	MOON4_REG->uphy1_ctl[3] = RF_MASK_V(0xffff, 0x88);
	_delay_1ms(1);
	MOON4_REG->uphy0_ctl[3] = RF_MASK_V(0xffff, 0x80);
	MOON4_REG->uphy1_ctl[3] = RF_MASK_V(0xffff, 0x80);
	_delay_1ms(1);
	MOON4_REG->uphy0_ctl[3] = RF_MASK_V(0xffff, 0);
	MOON4_REG->uphy1_ctl[3] = RF_MASK_V(0xffff, 0);
	_delay_1ms(1);

	// 3. reset UPHY0/1
	/* Q628 UPHY0_RESET UPHY1_RESET : 1->0 */
	MOON0_REG->reset[2] = RF_MASK_V_SET(3 << 13);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(3 << 13);
	_delay_1ms(1);

	// 4. UPHY 0 internal register modification
	UPHY0_RN_REG->cfg[7] = 0x8b;
	UPHY1_RN_REG->cfg[7] = 0x8b;

	// 5. USBC 0 reset
	/* Q628 USBC0_RESET USBC1_RESET : 1->0 */
	MOON0_REG->reset[2] = RF_MASK_V_SET(3 << 10);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(3 << 10);

	CSTAMP(0xE5B0A000);

	// Backup solution to workaround real IC USB clock issue
	// (issue: hang on reading EHCI_USBSTS after EN_ASYNC_SCHEDULE)
	if (HB_GP_REG->hb_otp_data2 & 0x1) { // G350.2 bit[0]
		prn_string("uphy0 rx clk inv\n");
		MOON4_REG->uphy0_ctl[2] = RF_MASK_V_SET(1 << 6);
	}
	if (HB_GP_REG->hb_otp_data2 & 0x2) { // G350.2 bit[1]
		prn_string("uphy1 rx clk inv\n");
		MOON4_REG->uphy1_ctl[2] = RF_MASK_V_SET(1 << 6);
	}

	CSTAMP(0xE5B0A001);

     // OTP for USB DISC (disconnect voltage)
	/* Q628 OTP[UPHY0_DISC] OTP[UPHY1_DISC] */
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

	CSTAMP(0xE5B0A002);
}
#endif

void usb_power_init(int is_host)
{
	// a. enable pin mux control
	//    Host: enable
	//    Device: disable
#ifdef PLATFORM_I143
	/* I143 USBC0_OTG_EN_SEL USBC1_OTG_EN_SEL */
	if (is_host) {
		MOON1_REG->sft_cfg[2] = RF_MASK_V_SET(3 << 12);
	} else {
		MOON1_REG->sft_cfg[2] = RF_MASK_V_CLR(3 << 12);
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
#ifdef PLATFORM_I143
	/* I143 USBC0_TYPE, USBC0_SEL, USBC1_TYPE, USBC1_SEL */
	if (is_host) {
		MOON5_REG->sft_cfg[17] = RF_MASK_V_SET((7 << 12) | (7 << 4));
	} else {
		MOON5_REG->sft_cfg[17] = RF_MASK_V_SET((1 << 12) | (1 << 4));
		MOON5_REG->sft_cfg[17] = RF_MASK_V_CLR((3 << 13) | (3 << 5));
	}
#else
	/* Q628 USBC0_TYPE, USBC0_SEL, USBC1_TYPE, USBC1_SEL */
	if (is_host) {
		MOON4_REG->usbc_ctl = RF_MASK_V_SET((7 << 12) | (7 << 4));
	} else {
		MOON4_REG->usbc_ctl = RF_MASK_V_SET((1 << 12) | (1 << 4));
		MOON4_REG->usbc_ctl = RF_MASK_V_CLR((3 << 13) | (3 << 5));
	}
#endif
}

int usb_init(int port, int next_port_in_hub)
{
	UINT32 tmp1, tmp2;
#ifdef CONFIG_HAVE_USB_HUB
	UINT8 NumberOfPorts;
	UINT8 port_num;
	unsigned int is_hub = 0;
#endif
	unsigned int dev_dct_cnt = 0;

#ifdef CONFIG_HAVE_USB_HUB
	if (next_port_in_hub)
		goto data_structure_init;
#endif

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
#ifdef CONFIG_HAVE_USB_HUB
	pUSB_HubDesc pHub = (pUSB_HubDesc)(USB_dataBuf);
	pUSB_PortStatus pPortsts = (pUSB_PortStatus)(USB_dataBuf);

data_structure_init:
#endif
	//EHCI_ASYNC_LISTADDR = CACHE_TO_BUS(&EHCI_QH);
	EHCI_ASYNC_LISTADDR = (u32)ADDRESS_CONVERT(&EHCI_QH);

	// Init QH
	//EHCI_QH.hw_next = (1<<1)|CACHE_TO_BUS(&EHCI_QH);
	EHCI_QH.hw_next = (1<<1)|((UINT32)ADDRESS_CONVERT(&EHCI_QH));
	//EHCI_QH.hw_info1 = QH_HEAD;
	EHCI_QH.hw_info2 = 1<<30;
	//EHCI_QH.hw_current = CACHE_TO_BUS(&EHCI_TD);
	EHCI_QH.hw_current = (UINT32)ADDRESS_CONVERT(&EHCI_TD);

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

#ifdef CONFIG_HAVE_USB_HUB
	if (next_port_in_hub) {
		is_hub = 1;
		port_num = next_port_in_hub;
		EHCI_addr = DEVICE_ADDRESS;
		goto scan_device_on_port;
	}

	prn_string("set addr\n");
	USB_vendorCmd(0, USB_REQ_SET_ADDRESS, DEVICE_ADDRESS, 0, 0);
	EHCI_addr = DEVICE_ADDRESS;

	prn_string("get dev desc (8)\n");
	USB_vendorCmd(0x80, USB_REQ_GET_DESCRIPTOR, DESC_DEVICE, 0, 0x08);

	prn_string("get dev desc ("); prn_decimal(pDev->bLength); prn_string(")\n");
	USB_vendorCmd(0x80, USB_REQ_GET_DESCRIPTOR, DESC_DEVICE, 0, (pDev->bLength));

	prn_string("get conf desc (18)\n");
	USB_vendorCmd(0x80, USB_REQ_GET_DESCRIPTOR, DESC_CONFIGURATION, 0, 0x12);

	if (USB_dataBuf[9+5] == USB_CLASS_MASS_STORAGE) {
		prn_string("usb mass storage device found\n");
		goto usb_storage_device;
	} else if (USB_dataBuf[9+5] == USB_CLASS_HUB) {
		is_hub = 1;
		prn_string("usb hub found\n");
	} else {
		return -1;
	}

	prn_string("set config "); prn_decimal(pCfg->bCV); prn_string("\n");
	USB_vendorCmd(0, USB_REQ_SET_CONFIG, (pCfg->bCV), 0, 0);

	prn_string("get hub desc (9)\n");
	USB_vendorCmd(0xA0, USB_REQ_GET_DESCRIPTOR, DESC_HUB, 0, 0x09);
	NumberOfPorts = pHub->bNumPorts;

	for (port_num = 1; port_num <= NumberOfPorts; port_num++) {
		prn_string("set feature (S_PORT"); prn_decimal(port_num); prn_string("_POWER) \n");
		USB_vendorCmd(0x23, USB_REQ_SET_FEATURE, S_PORT_POWER, port_num, 0);
	}

	for (port_num = 1; port_num <= NumberOfPorts; port_num++) {
		prn_string("get port "); prn_decimal(port_num); prn_string(" status \n");
		USB_vendorCmd(0xA3, USB_REQ_GET_STATUS, 0, port_num, 0x04);

	#if 0
		prn_string("port status :"); prn_dword(pPortsts->wPortStatus);
		prn_string("port change :"); prn_dword(pPortsts->wPortChange);
	#endif

		if (pPortsts->wPortStatus & 0x1)
			goto found_device_on_port;
	}

	prn_string("set feature (Device Remote Wakeup) \n");
	USB_vendorCmd(0, USB_REQ_SET_FEATURE, DEVICE_REMOTE_WAKEUP, 0, 0);

	#if 0
	while (1) {
		prn_string("get dev status (2)\n");
		USB_vendorCmd(0x80, USB_REQ_GET_STATUS, 0, 0, 0x02);

		if (pDevsts->wDevStatus & 0x02) // check remote-wakeup bit
			break;

		prn_string("set feature (Device Remote Wakeup) \n");
		USB_vendorCmd(0, USB_REQ_SET_FEATURE, DEVICE_REMOTE_WAKEUP, 0, 0);
	}
	#endif

	prn_string("clear feature (Device Remote Wakeup) \n");
	USB_vendorCmd(0, USB_REQ_CLEAR_FEATURE, DEVICE_REMOTE_WAKEUP, 0, 0);

	port_num = 1;

scan_device_on_port:
	if (next_port_in_hub) {
		prn_string("get hub desc (9)\n");
		USB_vendorCmd(0xA0, USB_REQ_GET_DESCRIPTOR, DESC_HUB, 0, 0x09);
		NumberOfPorts = pHub->bNumPorts;
	}

	while (port_num <= NumberOfPorts) {
		prn_string("get port "); prn_decimal(port_num); prn_string(" status \n");
		USB_vendorCmd(0xA3, USB_REQ_GET_STATUS, 0, port_num, 0x04);

	#if 0
		prn_string("port status :"); prn_dword(pPortsts->wPortStatus);
		prn_string("port change :"); prn_dword(pPortsts->wPortChange);
	#endif

		if (pPortsts->wPortStatus & 0x1)
			break;

		port_num++;
	}

	dev_dct_cnt = 0;
	if (port_num > NumberOfPorts) {
		prn_string("No usb mass storage devices on ports of the hub\n");

		while (dev_dct_cnt++ < 2500) {
			_delay_1ms(1);

			// print DOT every  100 ms
			if (!(dev_dct_cnt % 100))
				prn_string(".");
		}

		prn_string("\n");
		return -1;
	}

found_device_on_port:
	prn_string("clear feature (C_PORT"); prn_decimal(port_num); prn_string("_CONNECTION) \n");
	USB_vendorCmd(0x23, USB_REQ_CLEAR_FEATURE, C_PORT_CONNECTION, port_num, 0);

	prn_string("set feature (S_PORT"); prn_decimal(port_num); prn_string("_RESET) \n");
	USB_vendorCmd(0x23, USB_REQ_SET_FEATURE, S_PORT_RESET, port_num, 0);

	dev_dct_cnt = 0;
	while (1) {
		prn_string("get port "); prn_decimal(port_num); prn_string(" status \n");
		USB_vendorCmd(0xA3, USB_REQ_GET_STATUS, 0, port_num, 0x04);

		if (!(pPortsts->wPortStatus & 0x10))
			break;

		_delay_1ms(1);
		dev_dct_cnt++;
		if (dev_dct_cnt > 100) {	// 100ms
			prn_string("port reset timeout \n");
			break;
		}
	}

	prn_string("clear feature (C_PORT"); prn_decimal(port_num); prn_string("_RESET) \n");
	USB_vendorCmd(0x23, USB_REQ_CLEAR_FEATURE, C_PORT_RESET, port_num, 0);

	prn_string("get port "); prn_decimal(port_num); prn_string(" status \n");
	USB_vendorCmd(0xA3, USB_REQ_GET_STATUS, 0, port_num, 0x04);

	#if 0
	prn_string("port status :"); prn_dword(pPortsts->wPortStatus);
	prn_string("port change :"); prn_dword(pPortsts->wPortChange);
	#endif

	if (((pPortsts->wPortStatus & USB_SPEED_MASK) == USB_FULL_SPEED_DEVICE) ||
	    ((pPortsts->wPortStatus & USB_SPEED_MASK) == USB_LOW_SPEED_DEVICE)) {
		prn_string("skip usb low/full speed device found on port ");
		prn_decimal(port_num); prn_string(" of the hub\n");
		port_num++;
		goto scan_device_on_port;
	} else {
		prn_string("usb high speed device found on port ");
		prn_decimal(port_num); prn_string(" of the hub\n");
	}

	EHCI_addr = 0;

usb_storage_device:
#endif

	CSTAMP(0xE5B00008);
	prn_string("get dev desc (64)\n");
	USB_vendorCmd(0x80, USB_REQ_GET_DESCRIPTOR, DESC_DEVICE, 0, 0x40);

	prn_string("vid="); prn_byte((pDev->idVendor  >> 8) & 0xff); prn_byte(pDev->idVendor & 0xff);
	prn_string("pid="); prn_byte((pDev->idProduct >> 8) & 0xff); prn_byte(pDev->idProduct & 0xff);
	prn_string("rev="); prn_byte((pDev->bcdDevice >> 8) & 0xff); prn_byte(pDev->bcdDevice & 0xff);
	prn_string("\n");

#ifdef CONFIG_HAVE_USB_HUB
	if (is_hub) {
		CSTAMP(0xE5B00009);
		prn_string("set addr\n");
		USB_vendorCmd(0, USB_REQ_SET_ADDRESS, DEVICE_ADDRESS+port_num, 0, 0);
		EHCI_addr = DEVICE_ADDRESS + port_num;
	}
#else
	CSTAMP(0xE5B00009);
	prn_string("set addr\n");
	USB_vendorCmd(0, USB_REQ_SET_ADDRESS, DEVICE_ADDRESS, 0, 0);
	EHCI_addr = DEVICE_ADDRESS;
#endif

	CSTAMP(0xE5B0000A);
	prn_string("get dev desc ("); prn_decimal(pDev->bLength); prn_string(")\n");
	USB_vendorCmd(0x80, USB_REQ_GET_DESCRIPTOR, DESC_DEVICE, 0, (pDev->bLength));

	CSTAMP(0xE5B0000B);
	prn_string("get conf desc (18)\n");
	USB_vendorCmd(0x80, USB_REQ_GET_DESCRIPTOR, DESC_CONFIGURATION, 0, 18);

#ifdef CONFIG_HAVE_USB_HUB
	if (is_hub)
#endif
	{
		if (USB_dataBuf[9+5] != USB_CLASS_MASS_STORAGE) {
#ifdef CONFIG_HAVE_USB_HUB
			prn_string("not usb mass storage device\n");
			port_num++;
			EHCI_addr = DEVICE_ADDRESS;
			goto scan_device_on_port;
#else
			if (USB_dataBuf[9+5] == USB_CLASS_HUB)
				prn_string("usb hub not supported\n");
			else
				prn_string("not usb mass storage device\n");

			return -1;
#endif
		}
	}


	CSTAMP(0xE5B0000C);
	prn_string("get conf desc ("); prn_decimal(pCfg->wLength); prn_string(")\n");
	USB_vendorCmd(0x80, USB_REQ_GET_DESCRIPTOR, DESC_CONFIGURATION, 0, (pCfg->wLength));

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
	USB_vendorCmd(0, USB_REQ_SET_CONFIG, (pCfg->bCV), 0, 0);

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

#ifdef CONFIG_HAVE_USB_HUB
	if (is_hub)
		return ++port_num;
	else
		return 0;
#else
	return 0;
#endif
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
	u32 temp;
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
		addr = (UINT32)ADDRESS_CONVERT(buffer);
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

