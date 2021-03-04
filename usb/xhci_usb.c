#include <types.h>
#include <usb/xhci_usb.h>
#include <fat/fat.h>
#include <common.h>

#ifdef FPGA
#define XHCI_DEBUG
#endif
//#define XHCI_DEBUG
#ifdef XBOOT_BUILD
#define USB_DBG()
#else
#define USB_DBG()   dbg()
#define USB_PRN
#endif

void USB_vendorCmd(u8 bReq, u8 bCmd, u16 wValue, u16 wIndex, u16 wLen);
int stor_BBB_transport(u32 datalen, u32 cmdlen, u8 dir_in, u8 *buf);
extern void _delay_1ms(UINT32 period); // force delay even in CSIM
extern void boot_reset(void);

// UPHY 2 & 3 init (dh_feng)
#if defined(PLATFORM_I143)
void uphy_init(void)
{
	// 1. enable UPHY 2/3 & USBC 0/1 HW CLOCK */
	MOON0_REG->clken[2] = RF_MASK_V_SET(1 << 15);
	MOON0_REG->clken[2] = RF_MASK_V_SET(1 << 9);
	_delay_1ms(1);

	// 2. reset UPHY 2/3
	MOON0_REG->reset[2] = RF_MASK_V_SET(1 << 15);
	MOON0_REG->reset[2] = RF_MASK_V_SET(1 << 9);
	_delay_1ms(1);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(1 << 15);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(1 << 9);
	_delay_1ms(1);

	// 3. Default value modification
	UPHY2_RN_REG->gctrl[0] = 0x18888002;
	_delay_1ms(1);

	// 4. PLL power off/on twice
	UPHY2_RN_REG->gctrl[2] = 0x88;
	_delay_1ms(1);
	UPHY2_RN_REG->gctrl[2] = 0x80;
	_delay_1ms(1);
	UPHY2_RN_REG->gctrl[2] = 0x88;
	_delay_1ms(1);
	UPHY2_RN_REG->gctrl[2] = 0x80;
	_delay_1ms(20);
	UPHY2_RN_REG->gctrl[2] = 0x0;

	// 5. USBC 0/1 reset
	//MOON0_REG->reset[2] = RF_MASK_V_SET(3 << 10);
	//_delay_1ms(1);
	//MOON0_REG->reset[2] = RF_MASK_V_CLR(3 << 10);
	//_delay_1ms(1);

	// 6. HW workaround
	UPHY2_RN_REG->cfg[19] |= 0x0f;

	// 7. USB DISC (disconnect voltage)
	UPHY2_RN_REG->cfg[7] = 0x8b;

	// 8. RX SQUELCH LEVEL
	UPHY2_RN_REG->cfg[25] = 0x4;
	
	//U3 phy settings
	UPHY3_U3_REG->cfg[0]   = 0x43;
	UPHY3_U3_REG->cfg[11]  = 0x21;
	UPHY3_U3_REG->cfg[13]  = 0x5;
	UPHY3_U3_REG->cfg[17]  = 0x1f;
	UPHY3_U3_REG->cfg[18]  = 0x0;
	UPHY3_U3_REG->cfg[95]  = 0x33;
	UPHY3_U3_REG->cfg[112] = 0x11;
	UPHY3_U3_REG->cfg[113] = 0x0;
	UPHY3_U3_REG->cfg[114] = 0x1;
	UPHY3_U3_REG->cfg[115] = 0x0;
	UPHY3_U3_REG->cfg[128] = 0x9;	
	//U3 phy settings
	UPHY3_U3_REG->cfg[0]   = 0x43;
	UPHY3_U3_REG->cfg[11]  = 0x21;
	UPHY3_U3_REG->cfg[13]  = 0x5;
	UPHY3_U3_REG->cfg[17]  = 0x1f;
	UPHY3_U3_REG->cfg[18]  = 0x0;
	UPHY3_U3_REG->cfg[95]  = 0x33;
	UPHY3_U3_REG->cfg[112] = 0x11;
	UPHY3_U3_REG->cfg[113] = 0x0;
	UPHY3_U3_REG->cfg[114] = 0x1;
	UPHY3_U3_REG->cfg[115] = 0x0;
	UPHY3_U3_REG->cfg[128] = 0x9;
	//eq settings
	UPHY3_U3_REG->cfg[75]  = 0x0;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x1f;
	UPHY3_U3_REG->cfg[79]  = 0x0e;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x1;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x1f;
	UPHY3_U3_REG->cfg[79]  = 0x0e;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x2;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x4f;
	UPHY3_U3_REG->cfg[79]  = 0x0e;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x3;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x4f;
	UPHY3_U3_REG->cfg[79]  = 0x0e;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x4;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x7f;
	UPHY3_U3_REG->cfg[79]  = 0x0e;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x5;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x7f;
	UPHY3_U3_REG->cfg[79]  = 0x0e;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x6;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xaf;
	UPHY3_U3_REG->cfg[79]  = 0x0e;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x7;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xaf;
	UPHY3_U3_REG->cfg[79]  = 0x0e;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x8;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xdf;
	UPHY3_U3_REG->cfg[79]  = 0x0e;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x9;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xdf;
	UPHY3_U3_REG->cfg[79]  = 0x0e;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0xa;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x0f;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0xb;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x0f;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0xc;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x3f;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0xd;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x3f;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0xe;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x6f;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0xf;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x6f;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x10;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x9f;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x11;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0x9f;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x12;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xcf;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x13;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xcf;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x14;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xef;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x15;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xef;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x16;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xef;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x17;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xef;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x18;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x70;
	UPHY3_U3_REG->cfg[78]  = 0xee;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x19;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x70;
	UPHY3_U3_REG->cfg[78]  = 0xee;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x1a;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xb0;
	UPHY3_U3_REG->cfg[78]  = 0xed;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x1b;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xb0;
	UPHY3_U3_REG->cfg[78]  = 0xed;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x1c;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xec;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x1d;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xec;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x1e;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xec;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x1f;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xec;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x20;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x70;
	UPHY3_U3_REG->cfg[78]  = 0xeb;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x21;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x70;
	UPHY3_U3_REG->cfg[78]  = 0xeb;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x22;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xb0;
	UPHY3_U3_REG->cfg[78]  = 0xea;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x23;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xb0;
	UPHY3_U3_REG->cfg[78]  = 0xea;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x24;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xe9;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x25;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xe9;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x26;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xe9;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x27;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xe9;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x28;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x70;
	UPHY3_U3_REG->cfg[78]  = 0xe8;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x29;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x70;
	UPHY3_U3_REG->cfg[78]  = 0xe8;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x2a;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xb0;
	UPHY3_U3_REG->cfg[78]  = 0xe7;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x2b;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xb0;
	UPHY3_U3_REG->cfg[78]  = 0xe7;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x2c;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xe6;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x2d;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xe6;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x2e;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xe6;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x2f;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xe6;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x30;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x70;
	UPHY3_U3_REG->cfg[78]  = 0xe5;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x31;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x70;
	UPHY3_U3_REG->cfg[78]  = 0xe5;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x32;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xb0;
	UPHY3_U3_REG->cfg[78]  = 0xe4;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x33;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xb0;
	UPHY3_U3_REG->cfg[78]  = 0xe4;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x34;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xe3;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x35;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xe3;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x36;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xe3;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x37;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xe3;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x38;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x70;
	UPHY3_U3_REG->cfg[78]  = 0xe2;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x39;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x70;
	UPHY3_U3_REG->cfg[78]  = 0xe2;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x3a;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xb0;
	UPHY3_U3_REG->cfg[78]  = 0xe1;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x3b;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xb0;
	UPHY3_U3_REG->cfg[78]  = 0xe1;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x3c;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xe0;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x3d;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0xf0;
	UPHY3_U3_REG->cfg[78]  = 0xe0;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x3e;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xe0;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;
	
	UPHY3_U3_REG->cfg[80]  = 0x00;
	UPHY3_U3_REG->cfg[75]  = 0x3f;
	UPHY3_U3_REG->cfg[76]  = 0xbe;
	UPHY3_U3_REG->cfg[77]  = 0x30;
	UPHY3_U3_REG->cfg[78]  = 0xe0;
	UPHY3_U3_REG->cfg[79]  = 0x0f;
	UPHY3_U3_REG->cfg[80]  = 0x01;	
}
#endif

void usb_power_init(void)
{
	// a. enable pin mux control
	//    Host: enable
	//    Device: disable
#ifdef PLATFORM_I143
	/* I143 USBC0_OTG_EN_SEL USBC1_OTG_EN_SEL */
	MOON0_REG->clken[2] = RF_MASK_V_SET(1 << 12);
	
	// b. USB control register:
	/* I143 USBC0_TYPE, USBC0_SEL, USBC1_TYPE, USBC1_SEL */
	MOON0_REG->reset[2] = RF_MASK_V_SET(1 << 12);
	_delay_1ms(1);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(1 << 12);
#endif
}

void handshake(volatile u32 *ptr, u32 mask, u32 done, int usec)
{
	u32 result;
	
	do {
		result = *ptr;
		if (result == ~(u32)0) {
			prn_string("\n		ERR 0 ");
			break;
		}
		result &= mask;
		if (result == done)
			break;
		usec--;
		_delay_1ms(1);
	} while (usec > 0);
	if (usec <= 0)
		prn_string("\n  	ERR 1 ");
}

u32 xhci_v1_0_td_remainder(int running_total, int trb_buff_len, unsigned int total_packet_count, int maxpacketsize, unsigned int num_trbs_left)
{
	int packets_transferred;

	/* One TRB with a zero-length data packet. */
	if (num_trbs_left == 0 || (running_total == 0 && trb_buff_len == 0))
		return 0;

	/*
	 * All the TRB queueing functions don't count the current TRB in
	 * running_total.
	 */
	packets_transferred = (running_total + trb_buff_len) / maxpacketsize;

	if ((total_packet_count - packets_transferred) > 31)
		return 31 << 17;
	return (total_packet_count - packets_transferred) << 17;
}

u32 xhci_td_remainder(unsigned int remainder)
{
	u32 max = (1 << (21 - 17 + 1)) - 1;

	if ((remainder >> 10) >= max)
		return max << 17;
	else
		return (remainder >> 10) << 17;
}

struct xhci_ring *xhci_ring_alloc(int link_trbs, int no)
{
	struct xhci_ring *tmpring;	
#ifdef XHCI_DEBUG
	prn_string("\n  	*ring setting no  "); prn_dword(no);
#endif
	if (no == 0x10) {
		//memset32(g_io_buf.usb.xhci.pcmdtrb, 0, sizeof(g_io_buf.usb.xhci.pcmdtrb));
		tmpring = &g_io_buf.usb.xhci.pcmd_ring;
		tmpring->first_seg = &g_io_buf.usb.xhci.pcmd_ring_first_seg;
		tmpring->first_seg->trbs = g_io_buf.usb.xhci.pcmdtrb;
		tmpring->first_seg->next = NULL;
		tmpring->first_seg->next = tmpring->first_seg;
	} else if (no == 0x11) {
		u8 temp = 0;
		struct xhci_segment *prev;
		
		tmpring = &g_io_buf.usb.xhci.pevent_ring;
		tmpring->first_seg = g_io_buf.usb.xhci.pevent_ring_first_seg;
		tmpring->first_seg->trbs = &g_io_buf.usb.xhci.peventtrb[temp][0];
		tmpring->first_seg->next = NULL;
		//temp++;
		
		prev = tmpring->first_seg;
		while (++temp < ERST_NUM_SEGS)
		{
			struct xhci_segment *next;

#ifdef XHCI_DEBUG			
			prn_string("  	ERST_NUM_SEGS "); prn_dword((intptr_t)prev->trbs);
#endif
			next = &g_io_buf.usb.xhci.pevent_ring_first_seg[temp];
			next->trbs = &g_io_buf.usb.xhci.peventtrb[temp][0];
			next->next = NULL;
			//xhci_link_segments
			prev->next = next;
			
			prev = next;
		}
		prev->next = tmpring->first_seg;
	} else {
		if (no == 0x12) {
			no = ep_num - 1;
			memset((u8 *)&g_io_buf.usb.xhci.pep_ring[no], 0, sizeof(g_io_buf.usb.xhci.pep_ring)/ep_num);
			memset((u8 *)&g_io_buf.usb.xhci.pep_ring_first_seg[no], 0, sizeof(g_io_buf.usb.xhci.pep_ring_first_seg)/ep_num);
			memset((u8 *)&g_io_buf.usb.xhci.peptrb[no][0], 0, sizeof(g_io_buf.usb.xhci.peptrb)/ep_num);
			tmpring = &g_io_buf.usb.xhci.pep_ring[no];
			tmpring->first_seg = &g_io_buf.usb.xhci.pep_ring_first_seg[no];
			tmpring->first_seg->trbs = &g_io_buf.usb.xhci.peptrb[no][0];
			tmpring->first_seg->next = NULL;
			tmpring->first_seg->next = tmpring->first_seg;
		
		} else if (no < (ep_num * 2 - 1)) {
			no = (no + 1) / 2;
			//prn_string("\n  	*ring setting no  "); prn_dword(no);
			memset((u8 *)&g_io_buf.usb.xhci.pep_ring[no], 0, sizeof(g_io_buf.usb.xhci.pep_ring)/ep_num);
			memset((u8 *)&g_io_buf.usb.xhci.pep_ring_first_seg[no], 0, sizeof(g_io_buf.usb.xhci.pep_ring_first_seg)/ep_num);
			memset((u8 *)&g_io_buf.usb.xhci.peptrb[no][0], 0, sizeof(g_io_buf.usb.xhci.peptrb)/ep_num);
			tmpring = &g_io_buf.usb.xhci.pep_ring[no];
			tmpring->first_seg = &g_io_buf.usb.xhci.pep_ring_first_seg[no];
			tmpring->first_seg->trbs = &g_io_buf.usb.xhci.peptrb[no][0];
			tmpring->first_seg->next = NULL;
			tmpring->first_seg->next = tmpring->first_seg;
		} else {
#ifdef XHCI_DEBUG
			prn_string("\n!!!!!ring no overflow!!!!! ");
#endif
			return NULL;
		}
	} 

	// link = true
	if (link_trbs) {
		u32 ringtmp;
		u64 ringtmp_64;
	
		ringtmp_64 = (intptr_t) tmpring->first_seg->trbs;
		tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.segment_ptr = ringtmp_64;
		//prn_string("\n  *ringtmp_64 "); prn_dword(ringtmp_64);
	
		/*
	 	 * Set the last TRB in the segment to
	 	 * have a TRB type ID of Link TRB
		*/
		ringtmp = tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control;
		ringtmp &= ~TRB_TYPE_BITMASK;
		ringtmp |= (TRB_LINK << TRB_TYPE_SHIFT);
		tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control = ringtmp;	
		tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control |= LINK_TOGGLE;
	}
	//xhci_initialize_ring_info
	tmpring->enqueue = tmpring->first_seg->trbs;
	tmpring->enq_seg = tmpring->first_seg;
	tmpring->dequeue = tmpring->enqueue;
	tmpring->deq_seg = tmpring->first_seg;
	tmpring->cycle_state = 1;
#ifdef XHCI_DEBUG	
	prn_string("  	*tmpring->first_seg->trbs[0].link.control "); prn_dword(tmpring->first_seg->trbs[0].link.control);
	prn_string("  	*tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control "); prn_dword(tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control);
	prn_string("  	*tmpring.enqueue "); prn_dword((intptr_t)tmpring->enqueue);
	prn_string("  	*tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1] "); prn_dword((intptr_t)&tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1]);
	prn_string("  	*tmpring->first_seg "); prn_dword((intptr_t)&tmpring->first_seg);
#endif
	return tmpring;
}

//xhci_mem_init
void xhci_mem_init()
{
	u64 tmp1_64, tmp2_64, deq;
	struct xhci_segment *seg = NULL;
	//u64 deq;

#ifdef XHCI_DEBUG
	prn_string("\nxhci_mem_init ");
#endif
	g_io_buf.usb.xhci.dcbaa = (struct xhci_device_context_array *) g_io_buf.usb.xhci.dev_context_ptrs;
	g_io_buf.usb.xhci.hcor->or_dcbaap = (intptr_t)g_io_buf.usb.xhci.dev_context_ptrs;

	g_io_buf.usb.xhci.cmd_ring = xhci_ring_alloc(1, 0x10);	
		
	tmp1_64 = (u64)g_io_buf.usb.xhci.hcor->or_crcr; //need to check
	tmp2_64 = (intptr_t)g_io_buf.usb.xhci.cmd_ring->first_seg->trbs;

	tmp1_64 = (tmp1_64 & (u64) CMD_RING_RSVD_BITS) | (tmp2_64 & (u64) ~CMD_RING_RSVD_BITS) |
		  g_io_buf.usb.xhci.cmd_ring->cycle_state;

	g_io_buf.usb.xhci.hcor->or_crcr = tmp1_64;
	
	// write the address of db register
	tmp1_64 = g_io_buf.usb.xhci.hccr->cr_dboff;
	tmp1_64 &= DBOFF_MASK;
	g_io_buf.usb.xhci.dba = (struct xhci_doorbell_array *)((char *)g_io_buf.usb.xhci.hccr + tmp1_64);
	
	// write the address of runtime register 
	tmp1_64 = g_io_buf.usb.xhci.hccr->cr_rtsoff;
	tmp1_64 &= RTSOFF_MASK;
	g_io_buf.usb.xhci.run_regs = (struct xhci_run_regs *)((char *)g_io_buf.usb.xhci.hccr + tmp1_64);

	// writting the address of ir_set structure 
	g_io_buf.usb.xhci.ir_set = &g_io_buf.usb.xhci.run_regs->ir_set[0];
#ifdef XHCI_DEBUG
	prn_string("\n  	#g_io_buf.usb.xhci.dba "); prn_dword((intptr_t)g_io_buf.usb.xhci.dba);
	prn_string("  	#g_io_buf.usb.xhci.run_regs "); prn_dword((intptr_t)g_io_buf.usb.xhci.run_regs);
	prn_string("  	#g_io_buf.usb.xhci.ir_set "); prn_dword((intptr_t)g_io_buf.usb.xhci.ir_set);
#endif

	// Event ring does not maintain link TRB 
	g_io_buf.usb.xhci.event_ring = xhci_ring_alloc(0, 0x11);
 
	seg = g_io_buf.usb.xhci.event_ring->first_seg;
	deq = (intptr_t) g_io_buf.usb.xhci.event_ring->dequeue;
		
	memset32((u32 *)g_io_buf.usb.xhci.pentries, 0, sizeof(struct xhci_erst_entry) * ERST_NUM_SEGS / 4);

	g_io_buf.usb.xhci.erst.entries = g_io_buf.usb.xhci.pentries;

	g_io_buf.usb.xhci.erst.num_entries = ERST_NUM_SEGS;
	//seg = g_io_buf.usb.xhci.event_ring->first_seg;
	for (tmp1_64 = 0; tmp1_64 < ERST_NUM_SEGS; tmp1_64++) {
		tmp2_64 = (intptr_t)seg->trbs;
		g_io_buf.usb.xhci.erst.entries[tmp1_64].seg_addr = tmp2_64;
		g_io_buf.usb.xhci.erst.entries[tmp1_64].seg_size = TRBS_PER_SEGMENT;
		g_io_buf.usb.xhci.erst.entries[tmp1_64].rsvd = 0;
		seg = seg->next;
		//prn_string("\ntmp1_64 "); prn_dword(tmp1_64);
		//prn_string("tmp2_64 "); prn_dword(tmp2_64);
		//prn_string("addr "); prn_dword((intptr_t)&g_io_buf.usb.xhci.erst.entries[tmp1_64]);
		//prn_string("result "); prn_dword(g_io_buf.usb.xhci.erst.entries[tmp1_64].seg_addr);
	}

	// Event ring does not maintain link TRB	
	//tmp1_64 = (intptr_t) g_io_buf.usb.xhci.event_ring->dequeue;

	// Update HC event ring dequeue pointer 
	g_io_buf.usb.xhci.ir_set->erst_dequeue = (u64)deq & (u64)~ERST_PTR_MASK;
	// set ERST count with the number of entries in the segment table 

	tmp1_64 = g_io_buf.usb.xhci.ir_set->erst_size;
	tmp1_64 &= ERST_SIZE_MASK;
	tmp1_64 |= ERST_NUM_SEGS;
	g_io_buf.usb.xhci.ir_set->erst_size = tmp1_64;

	// this is the event ring segment table pointer 
	tmp1_64 = g_io_buf.usb.xhci.ir_set->erst_base;//need to check

	tmp1_64 &= ERST_PTR_MASK;
	tmp1_64 |= ((intptr_t)(g_io_buf.usb.xhci.erst.entries) & ~ERST_PTR_MASK);

	g_io_buf.usb.xhci.ir_set->erst_base = tmp1_64;//need to check

	// xhci_scratchpad_alloc
	//tmp1_64 = HCS_MAX_SCRATCHPAD(g_io_buf.usb.xhci.hccr->cr_hcsparams2);	
#ifdef XHCI_DEBUG
	prn_string("\n  	#num_sp "); prn_dword(HCS_MAX_SCRATCHPAD(g_io_buf.usb.xhci.hccr->cr_hcsparams2));
	prn_string("  	#hcor->or_pagesize "); prn_dword(g_io_buf.usb.xhci.hcor->or_pagesize);
#endif
	tmp1_64 = g_io_buf.usb.xhci.hcor->or_pagesize & 0xffff;
	for (tmp2_64 = 0; tmp2_64 < 16; tmp2_64++) {
		if ((0x1 & tmp1_64) != 0)
			break;
		tmp1_64 = tmp1_64 >> 1;
	}
	tmp1_64 = 1 << (tmp2_64 + 12);
	
	for (tmp2_64 = 0; tmp2_64 < HCS_MAX_SCRATCHPAD(g_io_buf.usb.xhci.hccr->cr_hcsparams2); tmp2_64++) {
		g_io_buf.usb.xhci.sparray[tmp2_64] = (intptr_t)(g_io_buf.usb.xhci.sparraybuf + tmp2_64 * tmp1_64);
	}
	g_io_buf.usb.xhci.pscratchpad.sp_array = g_io_buf.usb.xhci.sparray;
	
	g_io_buf.usb.xhci.scratchpad = &g_io_buf.usb.xhci.pscratchpad;
	
	g_io_buf.usb.xhci.dcbaa->dev_context_ptrs[0] = (intptr_t)g_io_buf.usb.xhci.pscratchpad.sp_array;

	//prn_string("  g_io_buf.usb.xhci.scratchpad->sp_array "); prn_dword((intptr_t)g_io_buf.usb.xhci.scratchpad->sp_array);
	//prn_string("  *g_io_buf.usb.xhci.scratchpad->sp_array "); prn_dword(*g_io_buf.usb.xhci.scratchpad->sp_array);
	//prn_string("  dev_context_ptrs[0] "); prn_dword((intptr_t)g_io_buf.usb.xhci.scratchpad->sp_array);	
	//initializing the virtual devices to NULL
	for (tmp2_64 = 0; tmp2_64 < MAX_HC_SLOTS; ++tmp2_64)
		g_io_buf.usb.xhci.devs[tmp2_64] = NULL;

	/*
	 * Just Zero'ing this register completely,
	 * or some spurious Device Notification Events
	 * might screw things here.
	 */
	g_io_buf.usb.xhci.hcor->or_dnctrl = 0x0;
	//end of xhci_mem_init
}

void giveback_first_trb(int ep_index, int start_cycle, struct xhci_generic_trb *start_trb)
{
	/*
	 * Pass all the TRBs to the hardware at once and make sure this write
	 * isn't reordered.
	 */
	if (start_cycle)
		start_trb->field[3] |= start_cycle;
	else
		start_trb->field[3] &= ~TRB_CYCLE;

	/* Ringing EP doorbell here */
	g_io_buf.usb.xhci.dba->doorbell[g_io_buf.usb.xhci.udev.slot_id] = DB_VALUE(ep_index, 0);
}

int last_trb_on_last_seg(struct xhci_ring *ring, struct xhci_segment *seg, union xhci_trb *trb)
{
	//prn_string("\n  		***last_trb_on_last_seg ring seg trb\n"); 
	//prn_string("  		"); prn_dword(ring); 
	//prn_string("  		"); prn_dword(seg); 
	//prn_string("  		"); prn_dword(trb);
	if (ring == g_io_buf.usb.xhci.event_ring)
		return ((trb == &seg->trbs[TRBS_PER_SEGMENT]) &&
			(seg->next == ring->first_seg));
	else
		return trb->link.control & LINK_TOGGLE;
}

int last_trb(struct xhci_ring *ring, struct xhci_segment *seg, union xhci_trb *trb)
{
	//prn_string("\n		**last_trb ring, seg, trb\n");
	//prn_string("		"); prn_dword(ring); 
	//prn_string("		"); prn_dword(seg); 
	//prn_string("		"); prn_dword(trb);

	if (ring == g_io_buf.usb.xhci.event_ring)
		return trb == &seg->trbs[TRBS_PER_SEGMENT];
	else
		return TRB_TYPE_LINK_LE32(trb->link.control);
}

void prepare_ring(struct xhci_ring *ep_ring)
{
	union xhci_trb *next = ep_ring->enqueue;
	
	//prn_string("\n  	*>>>prepare_ring");
	while (last_trb(ep_ring, ep_ring->enq_seg, next)) {
		next->link.control &= ~TRB_CHAIN;

		next->link.control ^= TRB_CYCLE;
		
		if (last_trb_on_last_seg(ep_ring, ep_ring->enq_seg, next))
			ep_ring->cycle_state = (ep_ring->cycle_state ? 0 : 1);
		ep_ring->enq_seg = ep_ring->enq_seg->next;
		ep_ring->enqueue = ep_ring->enq_seg->trbs;
		next = ep_ring->enqueue;
	}
	//prn_string("\n  	prepare_ring end<<<*");
}

void inc_enq(struct xhci_ring *ring, int more_trbs_coming)
{
	u32 chain;
	union xhci_trb *next;
        
        //if (ring == g_io_buf.usb.xhci.cmd_ring)
        //{
        	//prn_string("\n  generic.field[0] "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue->generic.field[0]);
        	//prn_string("\n  generic.field[1] "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue->generic.field[1]);
        	//prn_string("\n  generic.field[2] "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue->generic.field[2]);
        	//prn_string("\n  generic.field[3] "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue->generic.field[3]);
        //}
        //else if (ring == g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[0].ring)
        //{
        	//prn_string("\n  *epgeneric.field[0][1][2][3]\n"); 
        	//prn_dword(g_io_buf.usb.xhci.devs[1]->eps[0].ring->enqueue->generic.field[0]); 
        	//prn_dword(g_io_buf.usb.xhci.devs[1]->eps[0].ring->enqueue->generic.field[1]); 
        	//prn_dword(g_io_buf.usb.xhci.devs[1]->eps[0].ring->enqueue->generic.field[2]); 
        	//prn_dword(g_io_buf.usb.xhci.devs[1]->eps[0].ring->enqueue->generic.field[3]);
        //}
  	//prn_string("\n  last_trb ring "); prn_dword(ring);
	chain = ring->enqueue->generic.field[3] & TRB_CHAIN;	
	next = ++(ring->enqueue);
	/*
	 * Update the dequeue pointer further if that was a link TRB or we're at
	 * the end of an event ring segment (which doesn't have link TRBS)
	 */
	while (last_trb(ring, ring->enq_seg, next)) {
		if (ring != g_io_buf.usb.xhci.event_ring) {
			//prn_string("\n  @@@cmd ring full ");
			/*
			 * If the caller doesn't plan on enqueueing more
			 * TDs before ringing the doorbell, then we
			 * don't want to give the link TRB to the
			 * hardware just yet.  We'll give the link TRB
			 * back in prepare_ring() just before we enqueue
			 * the TD at the top of the ring.
			 */
			if (!chain && !more_trbs_coming)
				break;

			/*
			 * If we're not dealing with 0.95 hardware or
			 * isoc rings on AMD 0.96 host,
			 * carry over the chain bit of the previous TRB
			 * (which may mean the chain bit is cleared).
			 */
			next->link.control &= ~TRB_CHAIN;
			next->link.control |= chain;

			next->link.control ^= TRB_CYCLE;			
		}
		/* Toggle the cycle bit after the last ring segment. */
		if (last_trb_on_last_seg(ring, ring->enq_seg, next))
		{
			ring->cycle_state = (ring->cycle_state ? 0 : 1);
		}

		ring->enq_seg = ring->enq_seg->next;
		ring->enqueue = ring->enq_seg->trbs;
		next = ring->enqueue;
	}
}

void inc_deq(struct xhci_ring *ring)
{
	do {
		/*
		 * Update the dequeue pointer further if that was a link TRB or
		 * we're at the end of an event ring segment (which doesn't have
		 * link TRBS)
		 */
		if (last_trb(ring, ring->deq_seg, ring->dequeue)) {
			//prn_string("\n  @@@event ring full ");
			if (ring == g_io_buf.usb.xhci.event_ring && last_trb_on_last_seg(ring, ring->deq_seg, ring->dequeue)) {
				ring->cycle_state = (ring->cycle_state ? 0 : 1);
			}
			ring->deq_seg = ring->deq_seg->next;
			ring->dequeue = ring->deq_seg->trbs;
		} else {
			ring->dequeue++;
		}
	} while (last_trb(ring, ring->deq_seg, ring->dequeue));
}

void queue_trb(struct xhci_ring *ring, int more_trbs_coming, unsigned int *trb_fields)
{
	struct xhci_generic_trb *trb;	
        int i;
        
	trb = &ring->enqueue->generic;
	
	for (i = 0; i < 4; i++) {
		trb->field[i] = trb_fields[i];
		//prn_string("\n  	***trb->field[i] "); prn_dword(trb->field[i]);
	}
#if 0
	if (ring == g_io_buf.usb.xhci.cmd_ring)
		prn_string("\n  	***queue CMD trb "); 
	else if (ring == g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[0].ring)
		prn_string("\n  	***queue EP0 trb ");
	else if (ring == g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[1].ring)
		prn_string("\n  	***queue EP1 trb ");
	else if (ring == g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[4].ring)
		prn_string("\n  	***queue EP4 trb ");
	else if (ring == g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[6].ring)
		prn_string("\n  	***queue EP6 trb ");
	//prn_dword(ring->enqueue);
#endif
	inc_enq(ring, more_trbs_coming);
}

void queue_command(u8 *ptr, unsigned int slot_id, int ep_index, trb_type cmd)
{
	//queue command
	u32 trb_fields[4];
	//u32 tmpqueue;
	u64 tmpqueue_64;
	
	prepare_ring(g_io_buf.usb.xhci.cmd_ring);
	tmpqueue_64 = (intptr_t) ptr;
	trb_fields[0] = tmpqueue_64;
	trb_fields[1] = tmpqueue_64 >> 32;
	trb_fields[2] = 0;
	trb_fields[3] = TRB_TYPE(cmd) | SLOT_ID_FOR_TRB(slot_id) | g_io_buf.usb.xhci.cmd_ring->cycle_state;
	
	if (cmd >= TRB_RESET_EP && cmd <= TRB_SET_DEQ)
		trb_fields[3] |= EP_ID_FOR_TRB(ep_index);	
	//prn_string("\n  	*trb_fields[0][1][2][3]\n"); 
	//prn_string("  	"); prn_dword(trb_fields[0]); prn_string("  	"); prn_dword(trb_fields[1]);
	//prn_string("  	"); prn_dword(trb_fields[2]); prn_string("  	"); prn_dword(trb_fields[3]);
	//prn_string("\n  	***queue command "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue);
	queue_trb(g_io_buf.usb.xhci.cmd_ring, 0, trb_fields);
			
	//prn_string("\n  	***queue command inc "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue);
	//
	g_io_buf.usb.xhci.dba->doorbell[0] = DB_VALUE_HOST;
	
}
//xhci_acknowledge_event
void acknowledge_event(void)
{
	u64 tmpack;
	
	inc_deq(g_io_buf.usb.xhci.event_ring);	
	//
	tmpack = ((intptr_t) (g_io_buf.usb.xhci.event_ring->dequeue) | ERST_EHB);
	g_io_buf.usb.xhci.ir_set->erst_dequeue = tmpack;
}
//xhci_wait_for_event
union xhci_trb *wait_for_event(trb_type expected)
{
	trb_type type;
	u32 timeout = 0;
	union xhci_trb *event;
	
	do {
		//union xhci_trb *event = g_io_buf.usb.xhci.event_ring->dequeue;
		event = g_io_buf.usb.xhci.event_ring->dequeue;
				
                //prn_string("\n  	*xhci_wait_for_event "); prn_dword((u64)g_io_buf.usb.xhci.event_ring->dequeue);
		if ((event->event_cmd.flags & TRB_CYCLE) != g_io_buf.usb.xhci.event_ring->cycle_state)
			continue;

		type = TRB_FIELD_TO_TYPE(event->event_cmd.flags);
		if (type == expected) {	
			return event;
		}

		//xhci_acknowledge_event(ctrl);
		acknowledge_event();
		timeout++;
	} while (timeout < 15);
		
	prn_string("\n!!!!!wait_for_event timeout ");
	//if (expected == TRB_TRANSFER)
	return NULL;
}

//xhci_get_slot_ctx
struct xhci_slot_ctx *get_slot_ctx(xhci_usb *ctrl, struct xhci_container_ctx *ctx) 
{
	if (ctx->type == XHCI_CTX_TYPE_DEVICE)
		return (struct xhci_slot_ctx *)ctx->bytes;

	return (struct xhci_slot_ctx *) (ctx->bytes + CTX_SIZE(ctrl->hccr->cr_hccparams));
}
//xhci_get_ep_ctx
struct xhci_ep_ctx *get_ep_ctx(xhci_usb *ctrl, struct xhci_container_ctx *ctx, int ep_index)
{
	/* increment ep index by offset of start of ep ctx array */
	ep_index++;
	if (ctx->type == XHCI_CTX_TYPE_INPUT)
		ep_index++;
	//prn_string("\n  *ep&ctrl->hccr->cr_hccparams "); prn_dword(&ctrl->hccr->cr_hccparams);
        //prn_string("\n  *epCTX_SIZE "); prn_dword(ctrl->hccr->cr_hccparams);
	return (struct xhci_ep_ctx *) (ctx->bytes + (ep_index * CTX_SIZE(ctrl->hccr->cr_hccparams)));
}
//xhci_submit_root
void submit_root(struct devrequest *req, void *buf)
{
	u16 typeReq;
	u32 reg, tmpreg = 0;

	typeReq = req->request | req->requesttype << 8;
	switch (typeReq) {	
		case USB_REQ_GET_STATUS | ((0x23 | USB_DIR_IN) << 8):
			reg = g_io_buf.usb.xhci.hcor->portregs[req->index - 1].or_portsc;
			if (reg & 0x1) {
				tmpreg |= USB_PORT_STAT_CONNECTION;
				switch (reg & DEV_SPEED_MASK) {
					case XDEV_FS:					 
						//prn_string("  	SPEED = FULLSPEED\n");
						break;
					case XDEV_LS:
						tmpreg |= USB_PORT_STAT_LOW_SPEED;
						//prn_string("  	SPEED = LOWSPEED\n");
						break;
					case XDEV_HS:
						tmpreg |= USB_PORT_STAT_HIGH_SPEED;
						//prn_string("  	SPEED = HIGHSPEED\n");
						break;
					case XDEV_SS:
						tmpreg |= USB_PORT_STAT_SUPER_SPEED;
						//prn_string("  	SPEED = SUPERSPEED\n");
						break;
				}
			}
			if (reg & PORT_PE)
				tmpreg |= USB_PORT_STAT_ENABLE;
			if ((reg & PORT_PLS_MASK) == XDEV_U3)
				tmpreg |= USB_PORT_STAT_SUSPEND;
			if (reg & PORT_OC)
				tmpreg |= USB_PORT_STAT_OVERCURRENT;
			if (reg & PORT_RESET)
				tmpreg |= USB_PORT_STAT_RESET;
			if (reg & PORT_POWER)
				/*
			 	* XXX: This Port power bit (for USB 3.0 hub)
			 	* we are faking in USB 2.0 hub port status;
			 	* since there's a change in bit positions in
			 	* two:
			 	* USB 2.0 port status PP is at position[8]
			 	* USB 3.0 port status PP is at position[9]
			 	* So, we are still keeping it at position [8]
			 	*/
				tmpreg |= USB_PORT_STAT_POWER;
				
			if (reg & PORT_CSC)
				tmpreg |= PORT_CSC>> 1;//(USB_PORT_STAT_C_CONNECTION << 16);
			if (reg & PORT_PEC)
				tmpreg |= PORT_PEC >> 1;//(USB_PORT_STAT_C_ENABLE << 16);
			if (reg & PORT_OCC)
				tmpreg |= PORT_OCC >> 1;//(USB_PORT_STAT_C_OVERCURRENT << 16);
			if (reg & PORT_RC)
				tmpreg |= PORT_RC >> 1;//(USB_PORT_STAT_C_RESET << 16);			
			break;
		case USB_REQ_SET_FEATURE | ((USB_DIR_OUT | 0x23) << 8):
			reg = g_io_buf.usb.xhci.hcor->portregs[req->index - 1].or_portsc;
			reg = (reg & XHCI_PORT_RO) | (reg & XHCI_PORT_RWS);
			switch (req->value) {
				case 0x1://USB_PORT_FEAT_ENABLE:
					reg |= PORT_PE;
					break;
				case 0x8://USB_PORT_FEAT_POWER:
					reg |= PORT_POWER;
					break;
				case 0x4://USB_PORT_FEAT_RESET:
					reg |= PORT_RESET;
					break;
				default:
					prn_string("\n!!!!!unknown feature");
					return;
			}
			g_io_buf.usb.xhci.hcor->portregs[req->index - 1].or_portsc = reg;
			tmpreg = g_io_buf.usb.xhci.hcor->portregs[req->index - 1].or_portsc;
			break;
		case USB_REQ_CLEAR_FEATURE | ((USB_DIR_OUT | 0x23) << 8):
			reg = g_io_buf.usb.xhci.hcor->portregs[req->index - 1].or_portsc;			
			reg = (reg & XHCI_PORT_RO) | (reg & XHCI_PORT_RWS);
			tmpreg = reg;
			switch (req->value) {
				case 0x1://USB_PORT_FEAT_ENABLE:
					tmpreg &= ~PORT_PE;
					break;
				case 0x8://USB_PORT_FEAT_POWER:
					tmpreg &= ~PORT_POWER;
					break;
				case 0x14://USB_PORT_FEAT_C_RESET:
				case 0x10://USB_PORT_FEAT_C_CONNECTION:
				//case 0x13://USB_PORT_FEAT_C_OVER_CURRENT:
				//case 0x11://USB_PORT_FEAT_C_ENABLE:
					switch (req->value) {
						case 0x14://USB_PORT_FEAT_C_RESET:
							tmpreg |= PORT_RC;
							break;
						case 0x10://USB_PORT_FEAT_C_CONNECTION:
							tmpreg |= PORT_CSC;
							break;
						//case 0x13://USB_PORT_FEAT_C_OVER_CURRENT:
						//	tmpreg |= PORT_OCC;
						//	break;
						//case 0x11://USB_PORT_FEAT_C_ENABLE:
						//	tmpreg |= PORT_PEC;
						//	break;
						//case 0x12://USB_PORT_FEAT_C_SUSPEND:
						//	tmpreg |= PORT_PLC;
						//	break;
						default:
							prn_string("\n!!!!!unknown feature");
							return;
					}
					g_io_buf.usb.xhci.hcor->portregs[req->index - 1].or_portsc = tmpreg;
					break;
				default:
					prn_string("\n!!!!!unknown feature");
					break;
			}
			g_io_buf.usb.xhci.hcor->portregs[req->index - 1].or_portsc = reg;
			tmpreg = g_io_buf.usb.xhci.hcor->portregs[req->index - 1].or_portsc;
			break;
		default:
			prn_string("Unknown request\n");
			return;
	}
	memcpy(buf, (u8 *)&tmpreg, 4);
}
//usb_set_address
void set_address(xhci_usb *xusb)
{	
	struct xhci_input_control_ctx *ctrl_ctx;
        struct xhci_virt_device *virt_dev;
	// xhci_setup_addressable_virt_dev
	struct xhci_ep_ctx *ep0_ctx;
	struct xhci_slot_ctx *slot_ctx;
	union xhci_trb *pevent;
	u64 tmp_64;
	int port_num, route = 0;
	
	virt_dev = xusb->devs[xusb->udev.slot_id];
//xhci_setup_addressable_virt_dev
	//xhci_get_ep_ctx
	ep0_ctx = get_ep_ctx(xusb, virt_dev->in_ctx, 0);
	// xhci_get_slot_ctx
	slot_ctx = get_slot_ctx(xusb, virt_dev->in_ctx);
	slot_ctx->dev_info |= LAST_CTX(1);
	
	tmp_64 = xusb->udev.devnum;
	port_num = xusb->udev.portnr[tmp_64];
	
	while(tmp_64 > 2) {		
		if (port_num > 15)
			port_num = 15;
		route |= port_num << (xusb->udev.depth * 4);
		tmp_64--;
		port_num = xusb->udev.portnr[tmp_64];
	}

	slot_ctx->dev_info |= route;
	slot_ctx->dev_info2 |= (xusb->udev.portnr[2] & ROOT_HUB_PORT_MASK) << ROOT_HUB_PORT_SHIFT;//(port_num & ROOT_HUB_PORT_MASK) << ROOT_HUB_PORT_SHIFT;
	ep0_ctx->ep_info2 = CTRL_EP << EP_TYPE_SHIFT;
	if (xusb->udev.speed == USB_SPEED_SUPER) {
		slot_ctx->dev_info |= SLOT_SPEED_SS;	
		ep0_ctx->ep_info2 |= ((512 & MAX_PACKET_MASK) << MAX_PACKET_SHIFT);
	} else if (xusb->udev.speed == USB_SPEED_HIGH) {
		slot_ctx->dev_info |= SLOT_SPEED_HS;
		ep0_ctx->ep_info2 |= ((64 & MAX_PACKET_MASK) << MAX_PACKET_SHIFT);
	} else {
#ifdef XHCI_DEBUG
		prn_string("\n  	unknown speed "); prn_dword(xusb->udev.speed);
#endif
	}
#ifdef XHCI_DEBUG
	prn_string("  	g_io_buf.usb.xhci.udev.depth "); prn_dword(xusb->udev.depth);
	prn_string("  	route "); prn_dword(route);
	prn_string(" 	slot_ctx->dev_info "); prn_dword(slot_ctx->dev_info);
	prn_string(" 	slot_ctx->dev_info2 "); prn_dword(slot_ctx->dev_info2);
	prn_string(" 	ep0_ctx->dev_info2 "); prn_dword(ep0_ctx->ep_info2);	
#endif			
	ep0_ctx->ep_info2 |= ((0 & MAX_BURST_MASK) << MAX_BURST_SHIFT) | ((3 & ERROR_COUNT_MASK) << ERROR_COUNT_SHIFT);
	
	tmp_64 = (intptr_t)virt_dev->eps[0].ring->first_seg->trbs;
	ep0_ctx->deq = tmp_64 | virt_dev->eps[0].ring->cycle_state;
#ifdef XHCI_DEBUG
	prn_string("  	ep0_ctx->deq "); prn_dword(ep0_ctx->deq);
#endif
	
	ep0_ctx->tx_info = EP_AVG_TRB_LENGTH(8);
//xhci_get_input_control_ctx
	ctrl_ctx = (struct xhci_input_control_ctx *) virt_dev->in_ctx->bytes;
	ctrl_ctx->add_flags = SLOT_FLAG | EP0_FLAG;
	ctrl_ctx->drop_flags = 0;

	queue_command((void *)ctrl_ctx, xusb->udev.slot_id, 0, TRB_ADDR_DEV);	
	//even ring
	pevent = wait_for_event(TRB_COMPLETION);

	if (GET_COMP_CODE(pevent->event_cmd.status) != COMP_SUCCESS) {
		prn_string("\n!!!!!set_address GET_COMP_CODE!!!!! "); prn_dword(GET_COMP_CODE(pevent->event_cmd.status));
	}

	acknowledge_event();
//xhci_get_slot_ctx
	_delay_1ms(20);
	slot_ctx = get_slot_ctx(xusb, virt_dev->out_ctx);
#ifdef XHCI_DEBUG
	prn_string("  	xHCI internal address is: "); prn_dword(slot_ctx->dev_state & DEV_ADDR_MASK);
#endif
}

void slot_copy(struct xhci_container_ctx *in_ctx, struct xhci_container_ctx *out_ctx)
{
	struct xhci_slot_ctx *in_slot_ctx;
	struct xhci_slot_ctx *out_slot_ctx;

	in_slot_ctx = get_slot_ctx(&g_io_buf.usb.xhci, in_ctx);
	out_slot_ctx = get_slot_ctx(&g_io_buf.usb.xhci, out_ctx);

	in_slot_ctx->dev_info = out_slot_ctx->dev_info;
	in_slot_ctx->dev_info2 = out_slot_ctx->dev_info2;
	in_slot_ctx->tt_info = out_slot_ctx->tt_info;
	in_slot_ctx->dev_state = out_slot_ctx->dev_state;
}
//xhci_endpoint_copy
void endpoint_copy(struct xhci_container_ctx *in_ctx, struct xhci_container_ctx *out_ctx, int ep_index)
{
	struct xhci_ep_ctx *out_ep_ctx;
	struct xhci_ep_ctx *in_ep_ctx;

	out_ep_ctx = get_ep_ctx(&g_io_buf.usb.xhci, out_ctx, ep_index);
	in_ep_ctx = get_ep_ctx(&g_io_buf.usb.xhci, in_ctx, ep_index);

	in_ep_ctx->ep_info = out_ep_ctx->ep_info;
	in_ep_ctx->ep_info2 = out_ep_ctx->ep_info2;
	in_ep_ctx->deq = out_ep_ctx->deq;
	in_ep_ctx->tx_info = out_ep_ctx->tx_info;
}

//usb_parse_config
int usb_parse_config(unsigned char *buffer, int cfgno)
{
	struct usb_descriptor_header *head;
	int index, ifno, epno, curr_if_num;
	//u16 ep_wMaxPacketSize;
	struct usb_interface *if_desc = NULL;

	ifno = -1;
	epno = -1;
	curr_if_num = -1;

	//dev->configno = cfgno;
	head = (struct usb_descriptor_header *) &buffer[0];
	
	memcpy((u8 *)&g_io_buf.usb.xhci.udev.config, (u8 *)head, USB_DT_CONFIG_SIZE);
	g_io_buf.usb.xhci.udev.config.no_of_if = 0;

	index = g_io_buf.usb.xhci.udev.config.desc.bLength;
#ifdef XHCI_DEBUG
	prn_string("\n  	config.desc.bLength "); prn_dword(g_io_buf.usb.xhci.udev.config.desc.bLength);
#endif
	/* Ok the first entry must be a configuration entry,
	 * now process the others */
	head = (struct usb_descriptor_header *) &buffer[index];
	while (index + 1 < g_io_buf.usb.xhci.udev.config.desc.wTotalLength && head->bLength) {
#ifdef XHCI_DEBUG
		prn_string("  	config.desc.bLength "); prn_dword(head->bDescriptorType);
		prn_string("  	index "); prn_dword(index);
#endif
		
		switch (head->bDescriptorType) {
		case USB_DT_INTERFACE:
			if (head->bLength != USB_DT_INTERFACE_SIZE) {
#ifdef XHCI_DEBUG
				prn_string("\n!!!!!ERROR: Invalid USB IF length!!!!!");
#endif
				break;
			}
			if (index + USB_DT_INTERFACE_SIZE > g_io_buf.usb.xhci.udev.config.desc.wTotalLength) {
#ifdef XHCI_DEBUG
				prn_string("\n!!!!!USB IF descriptor overflowed buffer!!!!!");
#endif
				break;
			}
			if (((struct usb_interface_descriptor *) head)->bInterfaceNumber != curr_if_num) {
				/* this is a new interface, copy new desc */
				ifno = g_io_buf.usb.xhci.udev.config.no_of_if;
				if (ifno >= USB_MAXINTERFACES) {
#ifdef XHCI_DEBUG
					prn_string("\n!!!!!Too many USB interfaces!!!!!");
#endif
					break;
				}
				if_desc = &g_io_buf.usb.xhci.udev.config.if_desc[ifno];
				g_io_buf.usb.xhci.udev.config.no_of_if++;
				memcpy((u8 *)if_desc, (u8 *)head, USB_DT_INTERFACE_SIZE);
				if_desc->no_of_ep = 0;
				if_desc->num_altsetting = 1;
				curr_if_num = if_desc->desc.bInterfaceNumber;
			} else {
				/* found alternate setting for the interface */
				if (ifno >= 0) {
					if_desc = &g_io_buf.usb.xhci.udev.config.if_desc[ifno];
					if_desc->num_altsetting++;
				}
			}
			break;
		case USB_DT_ENDPOINT:
			if (head->bLength != USB_DT_ENDPOINT_SIZE &&
			    head->bLength != USB_DT_ENDPOINT_AUDIO_SIZE) {
#ifdef XHCI_DEBUG
			    	prn_string("\n!!!!!ERROR: Invalid USB EP length!!!!!");
#endif
				break;
			}
			if (index + head->bLength > g_io_buf.usb.xhci.udev.config.desc.wTotalLength) {
#ifdef XHCI_DEBUG
				prn_string("\n!!!!!USB EP descriptor overflowed buffer!!!!!");
#endif
				break;
			}
			if (ifno < 0) {
#ifdef XHCI_DEBUG
				prn_string("\n!!!!!Endpoint descriptor out of order!!!!!");
#endif
				break;
			}
			epno = g_io_buf.usb.xhci.udev.config.if_desc[ifno].no_of_ep;
			if_desc = &g_io_buf.usb.xhci.udev.config.if_desc[ifno];
			if (epno >= USB_MAXENDPOINTS) {
#ifdef XHCI_DEBUG
				prn_string("\n!!!!!Interface has too many endpoints!!!!!");
#endif
				break;
			}
			/* found an endpoint */
			if_desc->no_of_ep++;
			memcpy((u8 *)&if_desc->ep_desc[epno], (u8 *)head, USB_DT_ENDPOINT_SIZE);
			
#ifdef XHCI_DEBUG
			prn_string("  	ifno "); prn_dword(ifno);
			prn_string("  	epno "); prn_dword(epno);
#endif
			break;
		case USB_DT_SS_ENDPOINT_COMP:
			if (head->bLength != USB_DT_SS_EP_COMP_SIZE) {
#ifdef XHCI_DEBUG
				prn_string("\n!!!!!ERROR: Invalid USB EPC length!!!!!");
#endif
				break;
			}
			if (index + USB_DT_SS_EP_COMP_SIZE > g_io_buf.usb.xhci.udev.config.desc.wTotalLength) {
#ifdef XHCI_DEBUG
				prn_string("\n!!!!!USB EPC descriptor overflowed buffer!!!!!");
#endif
				break;
			}
			if (ifno < 0 || epno < 0) {
#ifdef XHCI_DEBUG
				prn_string("\n!!!!!EPC descriptor out of order!!!!!");
#endif
				break;
			}
			if_desc = &g_io_buf.usb.xhci.udev.config.if_desc[ifno];
			memcpy((u8 *)&if_desc->ss_ep_comp_desc[epno], (u8 *)head, USB_DT_SS_EP_COMP_SIZE);
			break;
		default:
			if (head->bLength == 0) {
#ifdef XHCI_DEBUG
				prn_string("\n!!!!!head->bLength = 0!!!!!");
#endif
				break;
			}
#ifdef XHCI_DEBUG
                        prn_string("\n!!!!!unknown Description Type!!!!!");
#endif

			break;
		}
		index += head->bLength;
		head = (struct usb_descriptor_header *)&buffer[index];
	}
	return 0;
}

//usb_set_maxpacket_ep
void usb_set_maxpacket_ep(usb_device *dev, int if_idx, int ep_idx)
{
	int b;
	struct usb_endpoint_descriptor *ep;
	u16 ep_wMaxPacketSize;

	ep = &dev->config.if_desc[if_idx].ep_desc[ep_idx];

	b = ep->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
	ep_wMaxPacketSize = ep->wMaxPacketSize;

	if ((ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_CONTROL) {
		/* Control => bidirectional */
		dev->epmaxpacketout[b] = ep_wMaxPacketSize;
		dev->epmaxpacketin[b] = ep_wMaxPacketSize;
#ifdef XHCI_DEBUG
		prn_string("\n  	no "); prn_dword(b);
		prn_string("  	##Control EP epmaxpacketout/in "); prn_dword(dev->epmaxpacketout[b]);
		prn_string("  	##Control EP epmaxpacketin "); prn_dword(dev->epmaxpacketin[b]);
#endif
	} else {
		if ((ep->bEndpointAddress & 0x80) == 0) {
			/* OUT Endpoint */
			if (dev->ep_dir[1] == 0)
				dev->ep_dir[1] = b;
			if (ep_wMaxPacketSize > dev->epmaxpacketout[b]) {
				dev->epmaxpacketout[b] = ep_wMaxPacketSize;
#ifdef XHCI_DEBUG
				prn_string("\n  	no "); prn_dword(b);
				prn_string("  	##Control EP epmaxpacketout "); prn_dword(dev->epmaxpacketout[b]);
#endif
			}
		} else {
			/* IN Endpoint */
			if (dev->ep_dir[0] == 0)
				dev->ep_dir[0] = b;
			if (ep_wMaxPacketSize > dev->epmaxpacketin[b]) {
				dev->epmaxpacketin[b] = ep_wMaxPacketSize;
#ifdef XHCI_DEBUG
				prn_string("\n  	no "); prn_dword(b);
				prn_string("  	##Control EP epmaxpacketin "); prn_dword(dev->epmaxpacketin[b]);
#endif
			}
		} /* if out */
	} /* if control */
}

//usb_set_maxpacket
int usb_set_maxpacket(usb_device *dev)
{
	int i, ii;
#ifdef XHCI_DEBUG	
	prn_string("\n  	config.desc.bNumInterfaces "); prn_dword(dev->config.desc.bNumInterfaces);
#endif	
	for (i = 0; i < g_io_buf.usb.xhci.udev.config.desc.bNumInterfaces; i++) {
#ifdef XHCI_DEBUG
		prn_string("  	config.if_desc[i].desc.bNumEndpoints "); prn_dword(dev->config.if_desc[i].desc.bNumEndpoints);
#endif
		dev->ep_dir[0] = 0;
		dev->ep_dir[1] = 0;
		for (ii = 0; ii < dev->config.if_desc[i].desc.bNumEndpoints; ii++)
			usb_set_maxpacket_ep(dev, i, ii);
	}

	return 0;
}

int configure_endpoints(unsigned int slot_id, int ctx_change)
{
	struct xhci_container_ctx *in_ctx;
	struct xhci_virt_device *virt_dev;
#ifdef XHCI_DEBUG	
	union xhci_trb *pevent;
#endif

	virt_dev = g_io_buf.usb.xhci.devs[slot_id];
	in_ctx = virt_dev->in_ctx;

	queue_command(in_ctx->bytes, slot_id, 0, ctx_change ? TRB_EVAL_CONTEXT : TRB_CONFIG_EP);
#ifdef XHCI_DEBUG
	pevent = wait_for_event(TRB_COMPLETION);

	prn_string("\n  	*TRB_TO_SLOT_ID "); prn_dword(TRB_TO_SLOT_ID(pevent->event_cmd.flags));
	if (GET_COMP_CODE(pevent->event_cmd.status) != COMP_SUCCESS) {
		prn_string("\n!!!!!GET_COMP_CODE!!!!! "); prn_dword(GET_COMP_CODE(pevent->event_cmd.status));
	}
#else
	wait_for_event(TRB_COMPLETION);
#endif
	//switch (GET_COMP_CODE(le32_to_cpu(event->event_cmd.status))) {
	//case COMP_SUCCESS:
	//	debug("Successful %s command\n",
	//		ctx_change ? "Evaluate Context" : "Configure Endpoint");
	//	break;
	//default:
	//	printf("ERROR: %s command returned completion code %d.\n",
	//		ctx_change ? "Evaluate Context" : "Configure Endpoint",
	//		GET_COMP_CODE(le32_to_cpu(event->event_cmd.status)));
	//	return -EINVAL;
	//}

	acknowledge_event();

	return 0;
}

int usb_endpoint_dir_in(struct usb_endpoint_descriptor *epd)
{
	return ((epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN);
}

int usb_endpoint_num(struct usb_endpoint_descriptor *epd)
{
	return epd->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
}

int usb_endpoint_xfer_control(struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_CONTROL);
}

int usb_endpoint_xfer_int(struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_INT);
}

int usb_endpoint_xfer_isoc(struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_ISOC);
}

unsigned int xhci_get_ep_index(struct usb_endpoint_descriptor *desc)
{
	unsigned int index;

	if (usb_endpoint_xfer_control(desc))
		index = (unsigned int)(usb_endpoint_num(desc) * 2);
	else
		index = (unsigned int)((usb_endpoint_num(desc) * 2) - (usb_endpoint_dir_in(desc) ? 0 : 1));

	return index;
}

int usb_endpoint_xfer_bulk(struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_BULK);
}

int usb_endpoint_maxp(struct usb_endpoint_descriptor *epd)
{
	return epd->wMaxPacketSize;
}

int usb_endpoint_maxp_mult(struct usb_endpoint_descriptor *epd)
{
	int maxp = epd->wMaxPacketSize;

	return USB_EP_MAXP_MULT(maxp) + 1;
}

u32 xhci_get_max_esit_payload(usb_device *udev,
	struct usb_endpoint_descriptor *endpt_desc,
	struct usb_ss_ep_comp_descriptor *ss_ep_comp_desc)
{
	int max_burst;
	int max_packet;

	/* Only applies for interrupt or isochronous endpoints */
	if (usb_endpoint_xfer_control(endpt_desc) ||
	    usb_endpoint_xfer_bulk(endpt_desc))
		return 0;

	/* SuperSpeed Isoc ep with less than 48k per esit */
	if (udev->speed >= USB_SPEED_SUPER)
		return ss_ep_comp_desc->wBytesPerInterval;

	max_packet = usb_endpoint_maxp(endpt_desc);
	max_burst = usb_endpoint_maxp_mult(endpt_desc);

	/* A 0 in max burst means 1 transfer per ESIT */
	return max_packet * max_burst;
}

u32 clamp_val(u32 interval, u32 min_v, u32 max_v)
{
	if (interval < min_v)
		return min_v;
	if (interval > max_v)
		return max_v;
	return interval;
}

unsigned int xhci_microframes_to_exponent(unsigned int desc_interval, unsigned int min_exponent, unsigned int max_exponent)
{
	unsigned int interval;
	u32 tmp_interval;
	
	tmp_interval = desc_interval;
	//interval = fls(desc_interval) - 1;
	int r = 32;

	if (!tmp_interval) {
		r = 0;
	} else {
		if (!(tmp_interval & 0xffff0000u)) {
			tmp_interval <<= 16;
			r -= 16;	
		}	
		if (!(tmp_interval & 0xff000000u)) {
			tmp_interval <<= 8;
			r -= 8;
		} 
		if (!(tmp_interval & 0xf0000000u)) {
			tmp_interval <<= 4;
			r -= 4;
		}
		if (!(tmp_interval & 0xc0000000u)) {
			tmp_interval <<= 2;
			r -= 2;
		}
		if (!(tmp_interval & 0x80000000u)) {
			tmp_interval <<= 1;
			r -= 1;
		}
	}
	interval = r - 1;

	interval = clamp_val(interval, min_exponent, max_exponent);
	if ((1 << interval) != desc_interval)
		prn_string("\n!!!!!rounding interval to microframes!!!!!");

	return interval;
}

unsigned int xhci_parse_microframe_interval(usb_device *udev, struct usb_endpoint_descriptor *endpt_desc)
{
	if (endpt_desc->bInterval == 0)
		return 0;

	return xhci_microframes_to_exponent(endpt_desc->bInterval, 0, 15);
}

unsigned int xhci_parse_exponent_interval(usb_device *udev, struct usb_endpoint_descriptor *endpt_desc)
{
	unsigned int interval;

	interval = clamp_val(endpt_desc->bInterval, 1, 16) - 1;
	if (interval != endpt_desc->bInterval - 1)
		prn_string("\n!!!!!ep - rounding interval to sframes!!!!!");

	if (udev->speed == USB_SPEED_FULL) {
		/*
		 * Full speed isoc endpoints specify interval in frames,
		 * not microframes. We are using microframes everywhere,
		 * so adjust accordingly.
		 */
		interval += 3;	/* 1 frame = 2^3 uframes */
	}

	return interval;
}

unsigned int xhci_parse_frame_interval(usb_device *udev, struct usb_endpoint_descriptor *endpt_desc)
{
	return xhci_microframes_to_exponent(endpt_desc->bInterval * 8, 3, 10);
}

unsigned int xhci_get_endpoint_interval(usb_device *udev, struct usb_endpoint_descriptor *endpt_desc)
{
	unsigned int interval = 0;

	switch (udev->speed) {
	case USB_SPEED_HIGH:
		/* Max NAK rate */
		if (usb_endpoint_xfer_control(endpt_desc) || usb_endpoint_xfer_bulk(endpt_desc)) {
			interval = xhci_parse_microframe_interval(udev, endpt_desc);
			break;
		}
		/* Fall through - SS and HS isoc/int have same decoding */

	case USB_SPEED_SUPER:
		if (usb_endpoint_xfer_int(endpt_desc) || usb_endpoint_xfer_isoc(endpt_desc)) {
			interval = xhci_parse_exponent_interval(udev, endpt_desc);
		}
		break;

	case USB_SPEED_FULL:
		if (usb_endpoint_xfer_isoc(endpt_desc)) {
			interval = xhci_parse_exponent_interval(udev, endpt_desc);
			break;
		}
		/*
		 * Fall through for interrupt endpoint interval decoding
		 * since it uses the same rules as low speed interrupt
		 * endpoints.
		 */

	case USB_SPEED_LOW:
		if (usb_endpoint_xfer_int(endpt_desc) || usb_endpoint_xfer_isoc(endpt_desc)) {
			interval = xhci_parse_frame_interval(udev, endpt_desc);
		}
		break;

	default:
		prn_string("\n!!!!!unknown speed!!!!!");
	}

	return interval;
}

u32 xhci_get_endpoint_mult(usb_device *udev,
	struct usb_endpoint_descriptor *endpt_desc,
	struct usb_ss_ep_comp_descriptor *ss_ep_comp_desc)
{
	if (udev->speed < USB_SPEED_SUPER || !usb_endpoint_xfer_isoc(endpt_desc))
		return 0;

	return ss_ep_comp_desc->bmAttributes;
}

u32 xhci_get_endpoint_max_burst(usb_device *udev,
	struct usb_endpoint_descriptor *endpt_desc,
	struct usb_ss_ep_comp_descriptor *ss_ep_comp_desc)
{
	/* Super speed and Plus have max burst in ep companion desc */
	if (udev->speed >= USB_SPEED_SUPER)
		return ss_ep_comp_desc->bMaxBurst;

	if (udev->speed == USB_SPEED_HIGH && (usb_endpoint_xfer_isoc(endpt_desc) || usb_endpoint_xfer_int(endpt_desc)))
		return usb_endpoint_maxp_mult(endpt_desc) - 1;

	return 0;
}
//xhci_set_configuration
int set_configuration(xhci_usb *xdev)
{
	struct xhci_virt_device *virt_dev;
	struct xhci_container_ctx *in_ctx;
	struct xhci_container_ctx *out_ctx;
	struct xhci_input_control_ctx *ctrl_ctx;
	struct xhci_slot_ctx *slot_ctx;
	struct xhci_ep_ctx *ep_ctx[MAX_EP_CTX_NUM];
	struct usb_interface *ifdesc;
	int ep_index;
	int cur_ep;
	int num_of_ep;
	unsigned int interval;
	unsigned int mult;
	unsigned int max_burst;
	unsigned int avg_trb_len;
	unsigned int err_count = 0;
	unsigned int dir;
	unsigned int ep_type;
	int ep_flag = 0;
	int max_ep_flag = 0;
	u32 max_esit_payload;
	u64 ptrb_64; 
	
	virt_dev = xdev->devs[xdev->udev.slot_id];
	out_ctx = virt_dev->out_ctx;
	in_ctx = virt_dev->in_ctx;
	
	num_of_ep = xdev->udev.config.if_desc[0].no_of_ep;
	ifdesc = &xdev->udev.config.if_desc[0];
	
	ctrl_ctx = (struct xhci_input_control_ctx *) in_ctx->bytes;//xhci_get_input_control_ctx(in_ctx);
	/* Initialize the input context control */
	ctrl_ctx->add_flags = SLOT_FLAG;
	ctrl_ctx->drop_flags = 0;
	
	/* EP_FLAG gives values 1 & 4 for EP1OUT and EP2IN */
	for (cur_ep = 0; cur_ep < num_of_ep; cur_ep++) { 
		ep_flag = xhci_get_ep_index(&ifdesc->ep_desc[cur_ep]);
		ctrl_ctx->add_flags |= (1 << (ep_flag + 1));
#ifdef XHCI_DEBUG
		prn_string("\n  	cur_ep "); prn_dword(cur_ep);
		prn_string("  	ep_flag "); prn_dword(ep_flag);
#endif
		if (max_ep_flag < ep_flag)
			max_ep_flag = ep_flag;
	}
	
	slot_copy(in_ctx, out_ctx);
	slot_ctx = get_slot_ctx(xdev, in_ctx);
	
	slot_ctx->dev_info &= ~(LAST_CTX_MASK);
	slot_ctx->dev_info |= LAST_CTX(max_ep_flag + 1) | 0;
        
	endpoint_copy(in_ctx, out_ctx, 0);
	
	/* filling up ep contexts */
	for (cur_ep = 0; cur_ep < num_of_ep; cur_ep++) {
		struct usb_endpoint_descriptor *endpt_desc = NULL;
		struct usb_ss_ep_comp_descriptor *ss_ep_comp_desc = NULL;

		endpt_desc = &ifdesc->ep_desc[cur_ep];
		ss_ep_comp_desc = &ifdesc->ss_ep_comp_desc[cur_ep];
		ptrb_64 = 0;

		/*
		 * Get values to fill the endpoint context, mostly from ep
		 * descriptor. The average TRB buffer lengt for bulk endpoints
		 * is unclear as we have no clue on scatter gather list entry
		 * size. For Isoc and Int, set it to max available.
		 * See xHCI 1.1 spec 4.14.1.1 for details.
		 */
		max_esit_payload = xhci_get_max_esit_payload(&xdev->udev, endpt_desc, ss_ep_comp_desc);
		interval = xhci_get_endpoint_interval(&xdev->udev, endpt_desc);
		//interval = 0;
		//if (cur_ep == 2)
		//	interval = 7;
		mult = xhci_get_endpoint_mult(&xdev->udev, endpt_desc, ss_ep_comp_desc);
		max_burst = xhci_get_endpoint_max_burst(&xdev->udev, endpt_desc, ss_ep_comp_desc);
		avg_trb_len = max_esit_payload;

		ep_index = xhci_get_ep_index(endpt_desc);
		ep_ctx[ep_index] = get_ep_ctx(xdev, in_ctx, ep_index);

		/* Allocate the ep rings */
		//virt_dev->eps[ep_index].ring = xhci_ring_alloc(1, true);
		virt_dev->eps[ep_index].ring = xhci_ring_alloc(1, ep_index);	

		/*NOTE: ep_desc[0] actually represents EP1 and so on */
		dir = (((endpt_desc->bEndpointAddress) & (0x80)) >> 7);
		ep_type = (((endpt_desc->bmAttributes) & (0x3)) | (dir << 2));

		ep_ctx[ep_index]->ep_info = EP_MAX_ESIT_PAYLOAD_HI(max_esit_payload) | EP_INTERVAL(interval) | EP_MULT(mult);

		ep_ctx[ep_index]->ep_info2 = ep_type << EP_TYPE_SHIFT;
		ep_ctx[ep_index]->ep_info2 |= MAX_PACKET(endpt_desc->wMaxPacketSize);

		/* Allow 3 retries for everything but isoc, set CErr = 3 */
		if (!usb_endpoint_xfer_isoc(endpt_desc))
			err_count = 3;
		ep_ctx[ep_index]->ep_info2 |= MAX_BURST(max_burst) | ERROR_COUNT(err_count);

		ptrb_64 = (intptr_t) virt_dev->eps[ep_index].ring->enqueue;
		ep_ctx[ep_index]->deq = ptrb_64 | virt_dev->eps[ep_index].ring->cycle_state;

		/*
		 * xHCI spec 6.2.3:
		 * 'Average TRB Length' should be 8 for control endpoints.
		 */
		if (usb_endpoint_xfer_control(endpt_desc))
			avg_trb_len = 8;
		ep_ctx[ep_index]->tx_info = EP_MAX_ESIT_PAYLOAD_LO(max_esit_payload) | EP_AVG_TRB_LENGTH(avg_trb_len);
#ifdef XHCI_DEBUG
		prn_string("\n  	ep_index "); prn_dword(ep_index);
		prn_string("  	ep_info "); prn_dword(ep_ctx[ep_index]->ep_info);
		prn_string("  	ep_info2 "); prn_dword(ep_ctx[ep_index]->ep_info2);
		prn_string("  	tx_info "); prn_dword(ep_ctx[ep_index]->tx_info);
#endif
		
	}

	return configure_endpoints(xdev->udev.slot_id, 0);
}
#if defined(CONFIG_HAVE_USB3_HUB)
//xhci_update_hub_device
void update_hub_device(xhci_usb *xdev, pUSB_DevDesc pDev, struct usb_hub_descriptor *pHub)
{
	//struct usb_hub_device *hub = dev_get_uclass_priv(udev->dev);
	struct xhci_virt_device *virt_dev;
	struct xhci_input_control_ctx *ctrl_ctx;
	struct xhci_container_ctx *out_ctx;
	struct xhci_container_ctx *in_ctx;
	struct xhci_slot_ctx *slot_ctx;
	unsigned think_time;
	u32 temp, tmp_portnr;

	if (pDev->bDeviceProtocol == 3) //super speed
		temp = 0x2a;//USB_DT_SS_HUB
	else 
		temp = 0x29;//USB_DT_HUB
				
	//usb_get_hub_descriptor
	USB_vendorCmd(0xa0, USB_REQ_GET_DESCRIPTOR, temp << 8, 0, 4);//0xa0 = USB_DIR_IN | USB_RT_HUB
	if (pHub->bLength > sizeof(struct usb_hub_descriptor))
		pHub->bLength = sizeof(struct usb_hub_descriptor);
	//usb_get_hub_descriptor
	USB_vendorCmd(0xa0, USB_REQ_GET_DESCRIPTOR, temp << 8, 0, pHub->bLength);//0xa0 = USB_DIR_IN | USB_RT_HUB
	tmp_portnr = pHub->bNbrPorts;
	if (pDev->bDeviceProtocol == 2)
		USB_vendorCmd(0x1, USB_REQ_SET_INTERFACE, 1, 0, 0);
	//usb_get_hub_status
	USB_vendorCmd(0xa0, USB_REQ_GET_STATUS, 0, 0, 4);//0xa0 = USB_DIR_IN | USB_RT_HUB
#ifdef XHCI_DEBUG
	prn_string("  	!!!!!sts "); prn_dword(USB_dataBuf[0] | (USB_dataBuf[1]<<8) | (USB_dataBuf[2]<<16) | (USB_dataBuf[3]<<24));
#endif
	//memcpy((u8 *)&portst, USB_dataBuf, 4);
/////				
	virt_dev = xdev->devs[xdev->udev.slot_id];

	out_ctx = virt_dev->out_ctx;
	in_ctx = virt_dev->in_ctx;

	ctrl_ctx = (struct xhci_input_control_ctx *) in_ctx->bytes;//xhci_get_input_control_ctx(in_ctx);
	/* Initialize the input context control */
	ctrl_ctx->add_flags = SLOT_FLAG;
	ctrl_ctx->drop_flags = 0;

	/* slot context */
	slot_copy(in_ctx, out_ctx);
	slot_ctx = get_slot_ctx(xdev, in_ctx);
	
	/* Update hub related fields */
	slot_ctx->dev_info |= DEV_HUB;
	/*
	 * refer to section 6.2.2: MTT should be 0 for full speed hub,
	 * but it may be already set to 1 when setup an xHCI virtual
	 * device, so clear it anyway.
	 */
	//if (hub->tt.multi)????
	//	slot_ctx->dev_info |= DEV_MTT;
	//else if (udev->speed == USB_SPEED_FULL)
	//	slot_ctx->dev_info &= cpu_to_le32(~DEV_MTT);
	slot_ctx->dev_info2 |= XHCI_MAX_PORTS(tmp_portnr);//udev->maxchild);
	/*
	 * Set TT think time - convert from ns to FS bit times.
	 * Note 8 FS bit times == (8 bits / 12000000 bps) ~= 666ns
	 *
	 * 0 =  8 FS bit times, 1 = 16 FS bit times,
	 * 2 = 24 FS bit times, 3 = 32 FS bit times.
	 *
	 * This field shall be 0 if the device is not a high-spped hub.
	 */
	think_time = 666;//hub->tt.think_time;
	if (think_time != 0)
		think_time = (think_time / 666) - 1;
	if (g_io_buf.usb.xhci.udev.speed == USB_SPEED_HIGH)
		slot_ctx->tt_info |= TT_THINK_TIME(think_time);
	slot_ctx->dev_state = 0;
#ifdef XHCI_DEBUG
	prn_string("\n  	update_hub_device dev_info "); prn_dword(slot_ctx->dev_info);
	prn_string("  	dev_info2 "); prn_dword(slot_ctx->dev_info2);
	prn_string("  	slot_ctx->tt_info "); prn_dword(slot_ctx->tt_info);
#endif
	configure_endpoints(xdev->udev.slot_id, 0);
/////
	temp = g_io_buf.usb.xhci.udev.devnum;
	if (temp < 2)
		g_io_buf.usb.xhci.udev.depth = -1; 
	else {			
		g_io_buf.usb.xhci.udev.depth = 0;
		while ((temp--) > 2) {
			g_io_buf.usb.xhci.udev.depth++;
		}
		if (pDev->bDeviceProtocol == 3) {
			//debug("set hub (%p) depth to %d\n", dev, depth);
			/*
			 * This request sets the value that the hub uses to
			 * determine the index into the 'route string index'
			 * for this hub.
			 */
			USB_vendorCmd(0x20, 0x0c, g_io_buf.usb.xhci.udev.depth, 0, 0);//usb_set_hub_depth(dev, depth);
		}
	}
	xdev->udev.Hub_NbrPorts = tmp_portnr;
}
#endif
#if 0
//abort_td
void abort_td(unsigned int slot_id, int ep_index)
{
	//struct xhci_ctrl *ctrl = xhci_get_ctrl(udev);
	//struct xhci_ring *ring =  ctrl->devs[udev->slot_id]->eps[ep_index].ring;
	struct xhci_ring *ring = g_io_buf.usb.xhci.devs[slot_id]->eps[ep_index].ring;
	union xhci_trb *pevent;
	//u32 field;
	
	prn_string("\nabort_td\n");
	queue_command(NULL, slot_id, ep_index, TRB_STOP_RING);

	pevent = wait_for_event(TRB_TRANSFER);
	//field = pevent->trans_event.flags;
	//BUG_ON(TRB_TO_SLOT_ID(field) != udev->slot_id);
	//BUG_ON(TRB_TO_EP_INDEX(field) != ep_index);
	//BUG_ON(GET_COMP_CODE(le32_to_cpu(event->trans_event.transfer_len
	//	!= COMP_STOP)));
	if (GET_COMP_CODE(pevent->trans_event.transfer_len) != COMP_STOP) {
		prn_string("!!!!!GET_COMP_CODE step1 "); prn_dword(GET_COMP_CODE(pevent->trans_event.transfer_len));
	}
	acknowledge_event();

	pevent = wait_for_event(TRB_COMPLETION);
	//BUG_ON(TRB_TO_SLOT_ID(le32_to_cpu(event->event_cmd.flags))
	//	!= udev->slot_id || GET_COMP_CODE(le32_to_cpu(
	//	event->event_cmd.status)) != COMP_SUCCESS);
	if (GET_COMP_CODE(pevent->event_cmd.status) != COMP_SUCCESS) {
		prn_string("!!!!!GET_COMP_CODE step2 "); prn_dword(GET_COMP_CODE(pevent->trans_event.transfer_len));
	}
	acknowledge_event();

	queue_command((void *)((uintptr_t)ring->enqueue | ring->cycle_state), slot_id, ep_index, TRB_SET_DEQ);
	pevent = wait_for_event(TRB_COMPLETION);
	//BUG_ON(TRB_TO_SLOT_ID(le32_to_cpu(event->event_cmd.flags))
	//	!= udev->slot_id || GET_COMP_CODE(le32_to_cpu(
	//	event->event_cmd.status)) != COMP_SUCCESS);
	if (GET_COMP_CODE(pevent->event_cmd.status) != COMP_SUCCESS) {
		prn_string("!!!!!GET_COMP_CODE step3 "); prn_dword(GET_COMP_CODE(pevent->trans_event.transfer_len));
	}
	acknowledge_event();
}
#endif
void ctrl_tx(struct devrequest *req, int length, void *buffer, u32 pipe)
{
	struct xhci_ring *ep_ring;
	int ep_index;
	//struct xhci_ep_ctx *ep_ctx = NULL;
	struct xhci_generic_trb *start_trb;
	int start_cycle;
	int num_trbs = 2;
	u32 field;
	u32 trb_fields[4];
	u64 buf_64 = 0;
	union xhci_trb *pevent;
	u32 length_field;
	
	ep_index = usb_pipe_ep_index(pipe);
	//prn_string("\n  g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[ep_index].ring "); prn_dword(g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[ep_index].ring);
#ifdef XHCI_DEBUG
	prn_string("\n  	*pipe "); prn_dword(pipe);
	//prn_string("  	*ep_index "); prn_dword(ep_index);
#endif
	ep_ring = g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[ep_index].ring;
	//ep_ctx = g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->out_ctx->bytes + CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams);
	if (length > 0) 
		num_trbs++;
	
	prepare_ring(ep_ring);
	start_trb = &ep_ring->enqueue->generic;
	start_cycle = ep_ring->cycle_state;
	//prn_string("\n  	start_trb "); prn_dword(start_trb);
	//prn_string("\n  	start_cycle "); prn_dword(start_cycle);
	
	field = 0;	
	field |= TRB_IDT | (TRB_SETUP << TRB_TYPE_SHIFT);
	if (start_cycle == 0)
		field |= 0x1;
	trb_fields[0] = req->requesttype | req->request << 8 | req->value << 16;
	trb_fields[1] = req->index | req->length << 16;
	/* TRB_LEN | (TRB_INTR_TARGET) */
	trb_fields[2] = (8 | ((0 & TRB_INTR_TARGET_MASK) << TRB_INTR_TARGET_SHIFT));
	/* Immediate data in pointer */
	trb_fields[3] = field;
	queue_trb(ep_ring, 1, trb_fields);
	
	/* Re-initializing field to zero */
	field = 0;
	/* If there's data, queue data TRBs */
	/* Only set interrupt on short packet for IN endpoints */
	if (usb_pipein(pipe))
		field = TRB_ISP | (TRB_DATA << TRB_TYPE_SHIFT);
	else
		field = (TRB_DATA << TRB_TYPE_SHIFT);

	length_field = (length & TRB_LEN_MASK) | xhci_td_remainder(length) |
			((0 & TRB_INTR_TARGET_MASK) << TRB_INTR_TARGET_SHIFT);
			
	if (length > 0) {
		if (req->requesttype & USB_DIR_IN)
			field |= TRB_DIR_IN;
		buf_64 = (intptr_t)buffer;

		trb_fields[0] = buf_64;
		trb_fields[1] = buf_64 >> 32;
		trb_fields[2] = length_field;
		trb_fields[3] = field | ep_ring->cycle_state;

		queue_trb(ep_ring, 1, trb_fields);
	}
	
	/*
	 * Queue status TRB -
	 * see Table 7 and sections 4.11.2.2 and 6.4.1.2.3
	 */

	/* If the device sent data, the status stage is an OUT transfer */
	field = 0;
	if (length > 0 && (req->requesttype & USB_DIR_IN))
		field = 0;
	else
		field = TRB_DIR_IN;

	trb_fields[0] = 0;	
	trb_fields[1] = 0;
	trb_fields[2] = ((0 & TRB_INTR_TARGET_MASK) << TRB_INTR_TARGET_SHIFT);
		/* Event on completion */
	trb_fields[3] = field | TRB_IOC | (TRB_STATUS << TRB_TYPE_SHIFT) | ep_ring->cycle_state;

	queue_trb(ep_ring, 0, trb_fields);
	
	giveback_first_trb(ep_index, start_cycle, start_trb);

	pevent = wait_for_event(TRB_TRANSFER);
#ifdef XHCI_DEBUG
	if (!pevent)
		prn_string("\n  XXXwait_for_event err ");
#endif		
	field = EVENT_TRB_LEN(pevent->trans_event.transfer_len);
	if (field)
		g_io_buf.usb.xhci.udev.act_len = length - field;
	else		
		g_io_buf.usb.xhci.udev.act_len = length;

	//prn_string("\n  	EVENT_TRB_LEN "); prn_dword(EVENT_TRB_LEN(pevent->trans_event.transfer_len));
	//prn_string("  	g_io_buf.usb.xhci.udev.act_len "); prn_dword(g_io_buf.usb.xhci.udev.act_len);
	//prn_string("\n  buffer L "); prn_dword(pevent->trans_event.buffer);
	//prn_string("\n  buffer H "); prn_dword(pevent->trans_event.buffer >> 32);
	acknowledge_event();
	
	if (GET_COMP_CODE(pevent->trans_event.transfer_len) == COMP_SHORT_TX) {
		/* Short data stage, clear up additional status stage event */
#ifdef XHCI_DEBUG
		prn_string("  	GET_COMP_CODE "); prn_dword(GET_COMP_CODE(pevent->trans_event.transfer_len));
#endif
		pevent = wait_for_event(TRB_TRANSFER);
#ifdef XHCI_DEBUG
		if (!pevent)
			prn_string("\n  XXXwait_for_event err ");
#endif
			//goto abort;
		//BUG_ON(TRB_TO_SLOT_ID(field) != slot_id);
		//BUG_ON(TRB_TO_EP_INDEX(field) != ep_index);
		acknowledge_event();
	}
	//return (udev->status != USB_ST_NOT_PROC) ? 0 : -1;
}
//xhci_bulk_tx
int bulk_tx(u32 pipe, int length, void *buffer)
{
	int num_trbs = 0;
	struct xhci_generic_trb *start_trb;
	int first_trb = 0;
	int start_cycle;
	u32 field = 0;
	u32 length_field = 0;
	int ep_index;
	struct xhci_virt_device *virt_dev;
	struct xhci_ring *ring;
	union xhci_trb *pevent;	
	int running_total, trb_buff_len, maxpacketsize;
	unsigned int total_packet_count;
	u64 addr;
	u32 trb_fields[4];
	u64 pval_64 = (intptr_t)buffer;
	
	
	ep_index = usb_pipe_ep_index(pipe);
	virt_dev = g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id];
	//prn_string("\n		*pipe "); prn_dword(pipe);
	//prn_string("		*ep_index "); prn_dword(ep_index);
	//prn_string("\n		*g_io_buf.usb.xhci.udev.slot_id "); prn_dword(g_io_buf.usb.xhci.udev.slot_id);
  
	//ep_ctx = xhci_get_ep_ctx(ctrl, virt_dev->out_ctx, ep_index);
	//ep_ctx = virt_dev->out_ctx->bytes + CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams);
	
	ring = virt_dev->eps[ep_index].ring;
	/*
	 * How much data is (potentially) left before the 64KB boundary?
	 * XHCI Spec puts restriction( TABLE 49 and 6.4.1 section of XHCI Spec)
	 * that the buffer should not span 64KB boundary. if so
	 * we send request in more than 1 TRB by chaining them.
	 */
	running_total = TRB_MAX_BUFF_SIZE - (((u32)pval_64) & (TRB_MAX_BUFF_SIZE - 1));
	trb_buff_len = running_total;
	running_total &= TRB_MAX_BUFF_SIZE - 1;
	//prn_string("\n		*running_total "); prn_dword(running_total);
	//prn_string("\n		*trb_buff_len "); prn_dword(trb_buff_len);
	//prn_string("\n		*pval_64 H "); prn_dword(pval_64>>32);
	//prn_string("		*pval_64 L "); prn_dword(pval_64);
	//prn_string("		length "); prn_dword(length);
	/*
	 * If there's some data on this 64KB chunk, or we have to send a
	 * zero-length transfer, we need at least one TRB
	 */
	if (running_total != 0 || length == 0)
		num_trbs++;
		
	/* How many more 64KB chunks to transfer, how many more TRBs? */
	while (running_total < length) {
		num_trbs++;
		running_total += TRB_MAX_BUFF_SIZE;
	}
	
	/*
	 * XXX: Calling routine prepare_ring() called in place of
	 * prepare_trasfer() as there in 'Linux' since we are not
	 * maintaining multiple TDs/transfer at the same time.
	 */
	prepare_ring(ring);
	
	start_trb = &ring->enqueue->generic;
	start_cycle = ring->cycle_state;
	//prn_string("\n		start_trb "); prn_dword((intptr_t)start_trb);
	//prn_string("\n		start_cycle "); prn_dword(start_cycle);
	
	running_total = 0;
	//maxpacketsize = usb_maxpacket(udev, pipe);
	if ((pipe & USB_DIR_IN) == 0)
		maxpacketsize = g_io_buf.usb.xhci.udev.epmaxpacketout[((pipe>>15) & 0xf)];
	else
		maxpacketsize = g_io_buf.usb.xhci.udev.epmaxpacketin[((pipe>>15) & 0xf)];
	//total_packet_count = DIV_ROUND_UP(length, maxpacketsize);
	total_packet_count = (length + maxpacketsize - 1)/maxpacketsize;
	//prn_string("\n		maxpacketsize "); prn_dword(maxpacketsize);
	//prn_string("		total_packet_count "); prn_dword(total_packet_count);
	/* How much data is in the first TRB? */
	/*
	 * How much data is (potentially) left before the 64KB boundary?
	 * XHCI Spec puts restriction( TABLE 49 and 6.4.1 section of XHCI Spec)
	 * that the buffer should not span 64KB boundary. if so
	 * we send request in more than 1 TRB by chaining them.
	 */
	addr = pval_64;

	if (trb_buff_len > length)
		trb_buff_len = length;

	first_trb = 1;
	/* Queue the first TRB, even if it's zero-length */
	do {
		u32 remainder = 0;
		
		field = 0;
		/* Don't change the cycle bit of the first TRB until later */
		if (first_trb) {
			first_trb = 0;
			if (start_cycle == 0)
				field |= TRB_CYCLE;
		} else {
			field |= ring->cycle_state;
		}
		/*
		 * Chain all the TRBs together; clear the chain bit in the last
		 * TRB to indicate it's the last TRB in the chain.
		 */
		if (num_trbs > 1)
			field |= TRB_CHAIN;
		else
			field |= TRB_IOC;

		/* Only set interrupt on short packet for IN endpoints */
		if (usb_pipein(pipe))
			field |= TRB_ISP;

		/* Set the TRB length, TD size, and interrupter fields. */
		if (HC_VERSION(g_io_buf.usb.xhci.hccr->cr_capbase) < 0x100)
			remainder = xhci_td_remainder(length - running_total);
		else
			remainder = xhci_v1_0_td_remainder(running_total, trb_buff_len, total_packet_count, maxpacketsize, num_trbs - 1);
		//prn_string("		remainder "); prn_dword(remainder);
		length_field = ((trb_buff_len & TRB_LEN_MASK) | remainder | ((0 & TRB_INTR_TARGET_MASK) << TRB_INTR_TARGET_SHIFT));
		
		trb_fields[0] = addr;
		trb_fields[1] = addr >> 32;
		trb_fields[2] = length_field;
		trb_fields[3] = field | (TRB_NORMAL << TRB_TYPE_SHIFT);

		queue_trb(ring, (num_trbs > 1), trb_fields);

		--num_trbs;

		running_total += trb_buff_len;

		/* Calculate length for next transfer */
		addr += trb_buff_len;
		
		//trb_buff_len = min((length - running_total), TRB_MAX_BUFF_SIZE);
		if (TRB_MAX_BUFF_SIZE >= (length - running_total))
			trb_buff_len = length - running_total;
		else
			trb_buff_len = TRB_MAX_BUFF_SIZE;
	} while (running_total < length);

	giveback_first_trb(ep_index, start_cycle, start_trb);

	pevent = wait_for_event(TRB_TRANSFER);
	if (!pevent) {
		prn_string("\n!!!!!XHCI bulk transfer timed out, aborting...\n");
#if 0
		abort_td(g_io_buf.usb.xhci.udev.slot_id, ep_index);
#endif
		return -1;//ETIMEDOUT;
	}
	field = pevent->trans_event.flags;	

	//record_transfer_result(udev, event, length);
	field = EVENT_TRB_LEN(pevent->trans_event.transfer_len);
	if (field)
		g_io_buf.usb.xhci.udev.act_len = length - field;
	else		
		g_io_buf.usb.xhci.udev.act_len = length;

	if ((GET_COMP_CODE(pevent->trans_event.transfer_len) != 0x1) &&
	    (GET_COMP_CODE(pevent->trans_event.transfer_len) != 0xd)) {
		prn_string("\n  !!!!!GET_COMP_CODE!!!!! "); prn_dword(GET_COMP_CODE(pevent->trans_event.transfer_len));
		acknowledge_event();
		return -GET_COMP_CODE(pevent->trans_event.transfer_len);
	}
	if (g_io_buf.usb.xhci.udev.act_len != length)
		prn_string("\n  !!!!!length != act_len!!!!! ");
		
	//xhci_acknowledge_event(ctrl);
	acknowledge_event();
	
	return GET_COMP_CODE(pevent->trans_event.transfer_len);////field;
}
//usb_string
void usb_string(int j, char *buf, int size)
{
	unsigned int u, idx, err;
	
	if (g_io_buf.usb.xhci.udev.string_langid == 0) {
#ifdef XHCI_DEBUG
		prn_string("\n**<usb_get_string langid>**");
#endif
		USB_vendorCmd(USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, 0x300, 0, 0xff);
		g_io_buf.usb.xhci.udev.string_langid = (g_io_buf.usb.cmd_buf[3]<<8)|g_io_buf.usb.cmd_buf[2];
#ifdef XHCI_DEBUG
		prn_string("\n  	g_io_buf.usb.cmd_buf[0] "); prn_dword(g_io_buf.usb.cmd_buf[0]);
		prn_string("  	language ID "); prn_dword(g_io_buf.usb.xhci.udev.string_langid);
#endif
	}
	
	USB_vendorCmd(USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, 0x300 + j, g_io_buf.usb.xhci.udev.string_langid, 0xff);

	err = 0xff - g_io_buf.usb.xhci.udev.act_len;
	size--;		/* leave room for trailing NULL char in output buffer */
	for (idx = 0, u = 2; u < err; u += 2) {
		if (idx >= size)
			break;
		if (g_io_buf.usb.cmd_buf[u+1])			/* high byte */
			buf[idx++] = '?';  /* non-ASCII character */
		else
			buf[idx++] = g_io_buf.usb.cmd_buf[u];
	}
	buf[idx] = 0;
	err = idx;
}

//usb_inquiry
void usb_inquiry(void)
{
	memset(g_io_buf.usb.xhci.reserved, 0, sizeof(g_io_buf.usb.xhci.reserved));
	g_io_buf.usb.xhci.reserved[0] = SCSI_INQUIRY;
	g_io_buf.usb.xhci.reserved[1] = g_io_buf.usb.xhci.udev.lun << 5;
	g_io_buf.usb.xhci.reserved[4] = 36;
	if (stor_BBB_transport(36, 12, 1, NULL) == 0) {
#ifdef XHCI_DEBUG
		prn_string("\n  	ISO Vers "); prn_dword(g_io_buf.usb.cmd_buf[2]);
		prn_string("  	Response Data "); prn_dword(g_io_buf.usb.cmd_buf[3]);
#endif
	} else 
		prn_string("=>usb_inquiry\n");
}
//usb_test_unit_ready
void usb_test_unit_ready(void)
{
	int retry = 10;
	
	memset(g_io_buf.usb.xhci.reserved, 0, sizeof(g_io_buf.usb.xhci.reserved));
	do 
	{
		g_io_buf.usb.xhci.reserved[0] = SCSICMD_TEST_UNIT_READY;
		g_io_buf.usb.xhci.reserved[1] = g_io_buf.usb.xhci.udev.lun << 5;
		if (stor_BBB_transport(0, 12, 0, NULL) == 0)
			return;
		else
			prn_string(" =>usb_test_unit_ready\n");
		memset(g_io_buf.usb.xhci.reserved, 0, sizeof(g_io_buf.usb.xhci.reserved));
		//usb_request_sense
		g_io_buf.usb.xhci.reserved[0] = SCSICMD_REQUEST_SENSE;
		g_io_buf.usb.xhci.reserved[1] = g_io_buf.usb.xhci.udev.lun << 5;
		g_io_buf.usb.xhci.reserved[4] = 18;
		if (stor_BBB_transport(18, 12, 0, NULL) == 0) {
			prn_string("\nRequest sense return\n"); 
			prn_dword(g_io_buf.usb.cmd_buf[2]);
			prn_dword(g_io_buf.usb.cmd_buf[12]);
			prn_dword(g_io_buf.usb.cmd_buf[13]);
			memset(g_io_buf.usb.xhci.reserved, 0, sizeof(g_io_buf.usb.xhci.reserved));
			_delay_1ms(100);	
		} else	
			prn_string(" =>usb_request_sense\n");
	} while (retry--);
	prn_string("unit still not ready...\n");
	boot_reset();		
}
//usb_read_capacity
void usb_read_capacity(void)
{
	memset(g_io_buf.usb.xhci.reserved, 0, sizeof(g_io_buf.usb.xhci.reserved));
	g_io_buf.usb.xhci.reserved[0] = SCSI_RD_CAPAC;
	g_io_buf.usb.xhci.reserved[1] = g_io_buf.usb.xhci.udev.lun << 5;
	stor_BBB_transport(8, 12, 1, NULL);
	//prn_string("\n    g_io_buf.usb.cmd_buf[0] ");prn_dword((g_io_buf.usb.cmd_buf[1]<<8)|g_io_buf.usb.cmd_buf[0]);
	//prn_string("\n    g_io_buf.usb.cmd_buf[2] ");prn_dword((g_io_buf.usb.cmd_buf[3]<<8)|g_io_buf.usb.cmd_buf[2]);
}
//usb_stor_BBB_clear_endpt_stall
void stor_BBB_clear_endpt_stall(u8 endpt)
{
	USB_vendorCmd(USB_RECIP_ENDPOINT, USB_REQ_CLEAR_FEATURE, 0, endpt, 0);
}
//usb_stor_BBB_transport
int stor_BBB_transport(u32 datalen, u32 cmdlen, u8 dir_in, u8 *buf)
{
	int result;
	sCBW *pcbw;
	sCSW *pcsw;
	int ep_in, ep_out, data_actlent = 0;//, e_o, e_i;
	
	ep_out = usb_sndbulkpipe(&g_io_buf.usb.xhci.udev, g_io_buf.usb.xhci.udev.ep_dir[1]);
        ep_in = usb_rcvbulkpipe(&g_io_buf.usb.xhci.udev, g_io_buf.usb.xhci.udev.ep_dir[0]);
	//prn_string("\n ep in "); prn_dword(ep_in);
	//prn_dword(g_io_buf.usb.xhci.udev.ep_dir[0]);
	//prn_string(" ep out "); prn_dword(ep_out);
	//prn_dword(g_io_buf.usb.xhci.udev.ep_dir[1]);
// 1	
#ifdef XHCI_DEBUG
	prn_string("\n  	COMMAND phase==>");
#endif
	pcbw = (sCBW*)(&g_io_buf.usb.xhci.reserved[32]);
        pcsw = (sCSW*)(&g_io_buf.usb.xhci.reserved[32]);
        memset(g_io_buf.usb.cmd_buf, 0, sizeof(g_io_buf.usb.cmd_buf));

	pcbw->dCBWSignature = CBWSignature;
	pcbw->dCBWTag = g_io_buf.usb.xhci.udev.CBWTag++;
	pcbw->dCBWDataTransferLength = datalen;
	pcbw->bmCBWFlags = dir_in ? CBWFLAGS_IN : CBWFLAGS_OUT;
	pcbw->bCBWLUN = g_io_buf.usb.xhci.udev.lun;
	pcbw->bCBWCBLength = cmdlen;
	/* copy the command data into the CBW command data buffer */
	/* DST SRC LEN!!! */  
	//prn_string("\n  	pcbw->dCBWTag "); prn_dword(pcbw->dCBWTag);             	
	memset(pcbw->CBWCB, 0, sizeof(pcbw->CBWCB));
	memcpy(pcbw->CBWCB, g_io_buf.usb.xhci.reserved, cmdlen);
	//prn_string("\n  	pcbw->CBWCB \n");
	//for (i = 0; i < pcbw->bCBWCBLength; i++) {
	//	prn_string("  	"); prn_dword(pcbw->CBWCB[i]);
	//}

	if (bulk_tx(ep_out, UMASS_BBB_CBW_SIZE, pcbw) == -6) {
		stor_BBB_clear_endpt_stall(g_io_buf.usb.xhci.udev.ep_dir[1]);
	}
	//if (result < 0)
	//	debug("usb_stor_BBB_comdat:usb_bulk_msg error\n");
	//_delay_1ms(20);
//2
	if (datalen > 0) {
#ifdef XHCI_DEBUG
		prn_string("\n  	DATA phase==>");
#endif
		//prn_string("\n  	pcbw->dCBWDataTransferLength "); prn_dword(pcbw->dCBWDataTransferLength);
		if (buf == NULL) //{
			buf = g_io_buf.usb.cmd_buf;
			//prn_string("\n  	buf = NULL ");
			//result = bulk_tx(ep_in, pcbw->dCBWDataTransferLength, g_io_buf.usb.cmd_buf);

		//} else {
			//prn_string("\n  	buf != NULL ");
		result = bulk_tx(ep_in, pcbw->dCBWDataTransferLength, buf);			
		//}
		if (result == -6) {
			stor_BBB_clear_endpt_stall(g_io_buf.usb.xhci.udev.ep_dir[0]);
		}
		data_actlent = g_io_buf.usb.xhci.udev.act_len;

	} 
//3
#ifdef XHCI_DEBUG
	prn_string("\n  	STATUS phase==>");
#endif
	memset((u8 *)pcsw, 0, UMASS_BBB_CSW_SIZE);
	result = bulk_tx(ep_in, UMASS_BBB_CSW_SIZE, pcsw);
	if (result == -6) {
		stor_BBB_clear_endpt_stall(g_io_buf.usb.xhci.udev.ep_dir[0]);
	}
	//_delay_1ms(20);
  	
  	//ep_in = pcsw->dCSWDataResidue;
  	//prn_string("\n  	pcsw->dCSWDataResidue "); prn_dword(ep_in);
	//if (ep_in == 0 && datalen != 0 && datalen - g_io_buf.usb.xhci.udev.act_len != 0)
	//	ep_in = datalen - g_io_buf.usb.xhci.udev.act_len;
	//prn_string("  	pipe "); prn_dword(ep_in);
	if (CSWSignature != pcsw->dCBWSignature) {//CSWSIGNATURE
		prn_string("\n!!!!! pcsw->dCBWSignature "); prn_dword(pcsw->dCBWSignature);
		//usb_stor_BBB_reset(us);
		return -1;//USB_STOR_TRANSPORT_FAILED;
	} else if ((g_io_buf.usb.xhci.udev.CBWTag - 1) != pcsw->dCBWTag) {
		prn_string("\n!!!!! pcsw->dCBWTag "); prn_dword(pcsw->dCBWTag);
		//usb_stor_BBB_reset(us);
		return -1;//USB_STOR_TRANSPORT_FAILED;
	} else if (pcsw->bCSWStatus > 0x2) {//CSWSTATUS_PHASE
		prn_string("\n!!!!! >PHASE ");
		//usb_stor_BBB_reset(us);
		return -1;//USB_STOR_TRANSPORT_FAILED;
	} else if (pcsw->bCSWStatus == 0x2) {//CSWSTATUS_PHASE
		prn_string("\n!!!!! =PHASE ");
		//usb_stor_BBB_reset(us);
		return -1;//USB_STOR_TRANSPORT_FAILED;
	} else if (data_actlent > datalen) {
		prn_string("\n!!!!! transferred %dB instead of %ldB ");
		return -1;//USB_STOR_TRANSPORT_FAILED;
	} else if (pcsw->bCSWStatus == 0x1) {//CSWSTATUS_FAILED
		prn_string("\n!!!!! FAILED ");
		return -1;//USB_STOR_TRANSPORT_FAILED;
	}
	
	return 0;
}
//xhci_alloc_device
void xhci_alloc_device(void)
{
	union xhci_trb *event; 
	u8 tmp_index = 0;
	
	queue_command(NULL, 0, 0, TRB_ENABLE_SLOT);	
	event = wait_for_event(TRB_COMPLETION);
	if (GET_COMP_CODE(event->event_cmd.status) != COMP_SUCCESS) {
		prn_string("\n!!!!!GET_COMP_CODE!!!!! "); prn_dword(GET_COMP_CODE(event->event_cmd.status));
	}	
	g_io_buf.usb.xhci.udev.slot_id = TRB_TO_SLOT_ID(event->event_cmd.flags);
			
	acknowledge_event();
        
        if (g_io_buf.usb.xhci.udev.devnum == 2) {
        	g_io_buf.usb.xhci.udev.r_slot_id = g_io_buf.usb.xhci.udev.slot_id;
        	tmp_index = 0;
        } else 
        	tmp_index = 1;
//xhci_alloc_virt_device
	memset((u8 *)&g_io_buf.usb.xhci.pdevs[tmp_index], 0, sizeof(g_io_buf.usb.xhci.pdevs)/2);
        memset((u8 *)&g_io_buf.usb.xhci.pout_ctx[tmp_index], 0, sizeof(g_io_buf.usb.xhci.pout_ctx)/2);
        memset((u8 *)&g_io_buf.usb.xhci.poutbyte[tmp_index][0], 0, sizeof(g_io_buf.usb.xhci.poutbyte)/2);
        memset((u8 *)&g_io_buf.usb.xhci.pin_ctx[tmp_index], 0, sizeof(g_io_buf.usb.xhci.pin_ctx)/2);
        memset((u8 *)&g_io_buf.usb.xhci.pinbyte[tmp_index][0], 0, sizeof(g_io_buf.usb.xhci.pinbyte)/2);
#ifdef XHCI_DEBUG
        prn_string("\n  	slot id "); prn_dword(g_io_buf.usb.xhci.udev.slot_id);
       	prn_string("  	size of xhci_virt_device "); prn_dword(sizeof(struct xhci_virt_device));
	prn_string("  	size of xhci_virt_device "); prn_dword(sizeof(g_io_buf.usb.xhci.pdevs));
        prn_string("  	size of cr_hccparams "); prn_dword(CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams));
        prn_string("  	size of pinbyte "); prn_dword(sizeof(g_io_buf.usb.xhci.pinbyte));
#endif
	g_io_buf.usb.xhci.pout_ctx[tmp_index].type = (int)XHCI_CTX_TYPE_DEVICE;
        g_io_buf.usb.xhci.pout_ctx[tmp_index].size = (MAX_EP_CTX_NUM + 1) * CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams);        
        g_io_buf.usb.xhci.pout_ctx[tmp_index].bytes = &g_io_buf.usb.xhci.poutbyte[tmp_index][0];
        g_io_buf.usb.xhci.pdevs[tmp_index].out_ctx = &g_io_buf.usb.xhci.pout_ctx[tmp_index];
        //in ctx
        g_io_buf.usb.xhci.pin_ctx[tmp_index].type = (int)XHCI_CTX_TYPE_INPUT;
        g_io_buf.usb.xhci.pin_ctx[tmp_index].size = (MAX_EP_CTX_NUM + 1) * CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams);
        g_io_buf.usb.xhci.pin_ctx[tmp_index].size += CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams);
	g_io_buf.usb.xhci.pin_ctx[tmp_index].bytes = &g_io_buf.usb.xhci.pinbyte[tmp_index][0];
        g_io_buf.usb.xhci.pdevs[tmp_index].in_ctx = &g_io_buf.usb.xhci.pin_ctx[tmp_index];
        	
        g_io_buf.usb.xhci.pdevs[tmp_index].eps[0].ring = xhci_ring_alloc(1, tmp_index == 0 ? 0x12:0);  
        g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id] = &g_io_buf.usb.xhci.pdevs[tmp_index];

	g_io_buf.usb.xhci.dcbaa->dev_context_ptrs[g_io_buf.usb.xhci.udev.slot_id] = (intptr_t)g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->out_ctx->bytes;
}

void USB_vendorCmd(u8 bReq, u8 bCmd, u16 wValue, u16 wIndex, u16 wLen)
{
	struct devrequest psetup_packet \
			  __attribute__ ((aligned(XHCI_ALIGNMENT)));
	u32 pipe;
	
	psetup_packet.requesttype = bReq;
	psetup_packet.request = bCmd;
	psetup_packet.value = wValue;
	psetup_packet.index = wIndex;
	psetup_packet.length = wLen;
	
	if (bReq & USB_DIR_IN)
		pipe = usb_rcvctrlpipe(&g_io_buf.usb.xhci.udev, 0);
	else 
		pipe = usb_sndctrlpipe(&g_io_buf.usb.xhci.udev, 0);
		
	if (g_io_buf.usb.xhci.udev.devnum < 2) {
		submit_root(&psetup_packet, g_io_buf.usb.cmd_buf);
		return;	
	}
		
	if ((bCmd == USB_REQ_SET_ADDRESS) && (bReq == 0)){
		set_address(&g_io_buf.usb.xhci);
		return;
	}	
	if (bCmd == USB_REQ_SET_CONFIGURATION)
		set_configuration(&g_io_buf.usb.xhci);
	memset(g_io_buf.usb.cmd_buf, 0, 1024);
	ctrl_tx(&psetup_packet, psetup_packet.length, g_io_buf.usb.cmd_buf, pipe);
#ifdef XHCI_DEBUG
	if (g_io_buf.usb.xhci.udev.act_len != psetup_packet.length) {
		prn_string("\n!!!!!usb_get_configuration_len != length!!!!!");
	}
#endif
}
//usb_get_port_status
void usb_get_port_status(u32 port, void *result)
{
	USB_vendorCmd(0xa3, USB_REQ_GET_STATUS, 0, port, 4);
#ifdef XHCI_DEBUG
	prn_string("  	!!!!!sts "); prn_dword(USB_dataBuf[0] | (USB_dataBuf[1]<<8) | (USB_dataBuf[2]<<16) | (USB_dataBuf[3]<<24));
#endif
	memcpy((u8 *)result, USB_dataBuf, 4);
}
//usb_select_config
void usb_select_config(pUSB_DevDesc pDev)
{
	pUSB_CfgDesc pCfg = (pUSB_CfgDesc)(USB_dataBuf);
//get_descriptor_len
#ifdef XHCI_DEBUG
	prn_string("\n**<get_descriptor_len>**");
#endif
	USB_vendorCmd(USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, DESC_DEVICE, 0, 0x12);
	memcpy(g_io_buf.usb.xhci.reserved, USB_dataBuf, 0x12);	
	_delay_1ms(1);
//usb_get_configuration_len
#ifdef XHCI_DEBUG
	prn_string("\n**<usb_get_configuration_len>**");
#endif		
	USB_vendorCmd(USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, DESC_CONFIGURATION, 0, 0x9);	
	
//usb_get_configuration_no
#ifdef XHCI_DEBUG
	prn_string("\n**<usb_get_configuration_no>**");	
#endif
	USB_vendorCmd(USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, DESC_CONFIGURATION, 0, pCfg->wLength);
	
	usb_parse_config(g_io_buf.usb.cmd_buf, 0);
	usb_set_maxpacket(&g_io_buf.usb.xhci.udev);
	//prn_string("\n    g_io_buf.usb.cmd_buf[0] "); prn_dword((g_io_buf.usb.cmd_buf[1]<<8)|g_io_buf.usb.cmd_buf[0]);
	//prn_string("\n    g_io_buf.usb.cmd_buf[2] "); prn_dword((g_io_buf.usb.cmd_buf[3]<<8)|g_io_buf.usb.cmd_buf[2]); 
	//prn_string("\n    pCfg->wLength "); prn_dword(pCfg->wLength);
	//prn_string("\n    pCfg->bLength "); prn_dword(pCfg->bLength);
	//prn_string("\n    pCfg->bNumIntf "); prn_dword(pCfg->bNumIntf);
#ifdef XHCI_DEBUG
	prn_string("\n  	pCfg->bConfigurationValue "); prn_dword(pCfg->bCV);
#endif
	
//xhci_set_configuration
#ifdef XHCI_DEBUG
	prn_string("\n**<set_configuration>**");
#endif
	USB_vendorCmd(0, USB_REQ_SET_CONFIGURATION, pCfg->bCV, 0, 0);	
	_delay_1ms(10);
//usb_string
#ifdef XHCI_DEBUG	
	prn_string("\n  	pDev->bDeviceClass "); prn_dword(pDev->bDeviceClass);
	prn_string("  	pDev->bDeviceProtocol "); prn_dword(pDev->bDeviceProtocol);
	prn_string("  	pDev->iManufacturer "); prn_dword(pDev->iManufacturer);
	prn_string("  	pDev->iProduct "); prn_dword(pDev->iProduct);
	prn_string("  	pDev->iSerialNumber "); prn_dword(pDev->iSerialNumber);
#endif
	g_io_buf.usb.xhci.udev.string_langid = 0;
	usb_string(pDev->iManufacturer, g_io_buf.usb.xhci.udev.mf, sizeof(g_io_buf.usb.xhci.udev.mf));
	usb_string(pDev->iProduct, g_io_buf.usb.xhci.udev.prod, sizeof(g_io_buf.usb.xhci.udev.prod));
	usb_string(pDev->iSerialNumber, g_io_buf.usb.xhci.udev.serial, sizeof(g_io_buf.usb.xhci.udev.serial));
	prn_string("\n  	mf     "); prn_string(g_io_buf.usb.xhci.udev.mf);
	prn_string("\n  	prod   "); prn_string(g_io_buf.usb.xhci.udev.prod);
	prn_string("\n  	serial "); prn_string(g_io_buf.usb.xhci.udev.serial);
}

void usb_storage()
{
	//memset(g_io_buf.usb.xhci.reserved, 0, sizeof(g_io_buf.usb.xhci.reserved));
//usb_get_max_lun
#ifdef XHCI_DEBUG
	prn_string("\n\n**<usb_get_max_lun>**");
#endif
	USB_vendorCmd(0xa1, 0xfe, 0, 0, 1);
	g_io_buf.usb.xhci.udev.lun = g_io_buf.usb.cmd_buf[0];	
//usb_inquiry
#ifdef XHCI_DEBUG
	prn_string("\n  	g_io_buf.usb.xhci.udev.lun "); prn_dword(g_io_buf.usb.xhci.udev.lun);
	prn_string("\n**<usb_inquiry>**");
#endif
	usb_inquiry();
//usb_test_unit_ready
#ifdef XHCI_DEBUG
	prn_string("\n**<usb_test_unit_ready>**");
#endif
	usb_test_unit_ready();	
//usb_read_capacity
#ifdef XHCI_DEBUG
	prn_string("\n**<usb_read_capacity>**");
#endif
	usb_read_capacity();
}

u32 port_test(u32 i)
{
	u32 val_64, tmp;
	struct usb_port_status portst;
	
	prn_string("port "); prn_dword(i);
	
	usb_get_port_status(i, &portst);
	if (!(portst.wPortStatus & USB_PORT_STAT_CONNECTION) && !(portst.wPortChange & USB_PORT_STAT_CONNECTION)) {
#ifdef XHCI_DEBUG
		prn_string("  	no connection port no "); prn_dword(i);
#endif					
		return 0;
	} 
							
	if (portst.wPortChange & USB_PORT_STAT_C_RESET) {
		USB_vendorCmd(0x23, USB_REQ_CLEAR_FEATURE, 20, i, 0);//USB_PORT_FEAT_C_RESET = 20
	}

	usb_get_port_status(i, &portst);
	USB_vendorCmd(0x23, USB_REQ_CLEAR_FEATURE, 16, i, 0);//USB_PORT_FEAT_C_CONNECTION = 16			
// Reset the port
	val_64 = 0;
	tmp = 20;	
	do
	{
		USB_vendorCmd(0x23, USB_REQ_SET_FEATURE, 4, i, 0);//USB_PORT_FEAT_RESET = 4
		_delay_1ms(tmp); //HUB_SHORT_RESET_TIME = 20
		usb_get_port_status(i, &portst);
		//prn_string("\n hcor->portregs[trb_64].or_portsc "); prn_dword(tmp1);
		tmp = 200;
		if (portst.wPortStatus & PORT_PE)
			break;
	} while ((val_64++) < 5);
	
	if (val_64 < 5) {
		if ((portst.wPortStatus & (USB_PORT_STAT_LOW_SPEED | USB_PORT_STAT_HIGH_SPEED)) >= USB_PORT_STAT_HIGH_SPEED) {
			USB_vendorCmd(0x23, USB_REQ_CLEAR_FEATURE, 20, i, 0);
			//prn_string("\n port 1 Reset the port end "); prn_dword(g_io_buf.usb.xhci.hcor->portregs[0].or_portsc);
#ifdef XHCI_DEBUG
			prn_string("\n**<port 1/2 Reset the port end**> "); 
#endif
			g_io_buf.usb.xhci.udev.devnum++; //new device start, devnum 1 is root hub.
			g_io_buf.usb.xhci.udev.portnr[g_io_buf.usb.xhci.udev.devnum] = i;
			switch (portst.wPortStatus & (USB_PORT_STAT_LOW_SPEED | USB_PORT_STAT_HIGH_SPEED)) {
				case USB_PORT_STAT_HIGH_SPEED:
					g_io_buf.usb.xhci.udev.speed = USB_SPEED_HIGH;
#ifdef XHCI_DEBUG
					prn_string("\n#####HIGH speed ");
#endif 
					break;
				case USB_PORT_STAT_SUPER_SPEED:
					g_io_buf.usb.xhci.udev.speed = USB_SPEED_SUPER;
#ifdef XHCI_DEBUG
					prn_string("\n#####SUPER speed ");
#endif
					break;
				default:
					prn_string("\n#####no support speed ");
					break;
			}
#ifdef XHCI_DEBUG
			prn_dword(g_io_buf.usb.xhci.udev.speed);
#endif
			return 1;
		} else {
#ifdef XHCI_DEBUG
			prn_string("==> un support speed");
#endif
			return 0;
		}
	} else {
#ifdef XHCI_DEBUG
		prn_string("==> reset failed");
#endif
		return 0;
	}	
}

int usb_init(int port, int next_port_in_hub)
{	      
	u64 trb_64, val_64;
	
	CSTAMP(0xE5B00000);
	dbg();
	
	if (next_port_in_hub == 0)
	{
		u32 tmp1, tmp2, i;
		u8 Hub_NbrPorts_root;
		struct dwc3 *dwc3_reg;	
		
        	memset32((u32 *)&g_io_buf.usb.xhci, 0, sizeof(g_io_buf.usb.xhci)/4);
        	dbg();

		prn_string("\nxboot usb_init \n");
		//check merory address align 
		if ((intptr_t) g_io_buf.usb.xhci.sparraybuf & 0xfff) {
			prn_string("!!!!!g_io_buf.usb.xhci.sparraybuf addr not align "); prn_dword((intptr_t) g_io_buf.usb.xhci.sparraybuf);
		}
		for (i = 0; i < ERST_NUM_SEGS; i++) {
			if ((intptr_t) &g_io_buf.usb.xhci.peventtrb[i][0] & 0x3f) {
				prn_string("!!!!!g_io_buf.usb.xhci.peventtrb addr not align "); prn_dword((intptr_t) &g_io_buf.usb.xhci.peventtrb[i][0]);
			}
		}
		for (i = 0; i < ep_num; i++) {
			if ((intptr_t) &g_io_buf.usb.xhci.peptrb[i][0] & 0x3f) {
				prn_string("!!!!!g_io_buf.usb.xhci.peptrb addr not align "); prn_dword((intptr_t) &g_io_buf.usb.xhci.peptrb[i][0]);
			}
		}
		for (i = 0; i < 2; i++) {
			if ((intptr_t) &g_io_buf.usb.xhci.poutbyte[i][0] & 0x3f) {
				prn_string("!!!!!g_io_buf.usb.xhci.poutbyte addr not align "); prn_dword((intptr_t) &g_io_buf.usb.xhci.poutbyte[i][0]);
			}
			if ((intptr_t) &g_io_buf.usb.xhci.pinbyte[i][0] & 0x3f) {
				prn_string("!!!!!g_io_buf.usb.xhci.pinbyte addr not align "); prn_dword((intptr_t) &g_io_buf.usb.xhci.pinbyte[i][0]);
			}
		}
		//end
#if defined(PLATFORM_I143)
		CSTAMP(0xE5B00001);
		uphy_init();	
#endif
		CSTAMP(0xE5B00002);
		usb_power_init();

/* usb-uclass.c/usb_init.c -> xhci-spdwc3.c/xhci_dwc3_probe*/               
// xhci register base

		g_io_buf.usb.xhci.hccr = (struct xhci_hccr *) XHCI_REG;
		g_io_buf.usb.xhci.hcor = (struct xhci_hcor *)((char *)g_io_buf.usb.xhci.hccr +
				  	HC_LENGTH(g_io_buf.usb.xhci.hccr->cr_capbase));
        	dwc3_reg = (struct dwc3 *)((char *)(g_io_buf.usb.xhci.hccr) + DWC3_REG_OFFSET);
#ifdef XHCI_DEBUG
        	prn_string("\n#hccr "); prn_dword((intptr_t)g_io_buf.usb.xhci.hccr);
       		prn_string("#hcor "); prn_dword((intptr_t)g_io_buf.usb.xhci.hcor);
        	prn_string("#dwc3_reg "); prn_dword((intptr_t)dwc3_reg);
        	prn_string("g_io_buf.usb.xhci.hccr->cr_capbase "); prn_dword((u64)g_io_buf.usb.xhci.hccr->cr_capbase);
#endif
		CSTAMP(0xE5B00003);
//init DWC3
//sunplus_dwc3_phy_setup
		tmp1 = dwc3_reg->g_usb2phycfg[0];
		tmp1 &= ~DWC3_GUSB2PHYCFG_SUSPHY;
		tmp1 |= DWC3_GUSB2PHYCFG_PHYIF;
		tmp1 &= ~DWC3_GUSB2PHYCFG_USBTRDTIM_MASK;
		tmp1 |= DWC3_GUSB2PHYCFG_USBTRDTIM_16BIT;
		dwc3_reg->g_usb2phycfg[0] = tmp1;
#ifdef XHCI_DEBUG
		prn_string("dwc3_reg->g_usb2phycfg[0] "); prn_dword(dwc3_reg->g_usb2phycfg[0]);
//dwc3_core_init
		//prn_string("\ndwc3_reg->g_snpsid "); prn_dword(dwc3_reg->g_snpsid);
		prn_string("\ndwc3/phy reset");
#endif
//dwc3_core_soft_reset	
		dwc3_reg->g_ctl |= DWC3_GCTL_CORESOFTRESET; 
		/*dwc3_phy_reset*/
		dwc3_reg->g_usb3pipectl[0] |= DWC3_GUSB3PIPECTL_PHYSOFTRST;
		dwc3_reg->g_usb2phycfg[0] |= DWC3_GUSB2PHYCFG_PHYSOFTRST;
		_delay_1ms(100);
		dwc3_reg->g_usb3pipectl[0] &= ~DWC3_GUSB3PIPECTL_PHYSOFTRST;
		dwc3_reg->g_usb2phycfg[0] &= ~DWC3_GUSB2PHYCFG_PHYSOFTRST;
		_delay_1ms(100);
		dwc3_reg->g_ctl &= ~DWC3_GCTL_CORESOFTRESET;
	
		tmp1 = dwc3_reg->g_hwparams1;
		tmp2 = dwc3_reg->g_ctl;
		tmp2 &= ~DWC3_GCTL_SCALEDOWN_MASK;
		tmp2 &= ~DWC3_GCTL_DISSCRAMBLE;
		switch (DWC3_GHWPARAMS1_EN_PWROPT(tmp1)) {
		case DWC3_GHWPARAMS1_EN_PWROPT_CLK:
			tmp2 &= ~DWC3_GCTL_DSBLCLKGTNG;
			break;
		default:
			prn_string("\n!!!!!No power optimization available!!!!!\n");
		}
		tmp1 = dwc3_reg->g_snpsid;
		if ((tmp1 & 0xffff) < 0x190a)//DWC3_REVISION_MASK
			tmp2 |= DWC3_GCTL_U2RSTECN;
		dwc3_reg->g_ctl = tmp2;

//set dr mode
		dwc3_reg->g_ctl &= ~DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG);
		dwc3_reg->g_ctl |= DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_HOST);
#ifdef XHCI_DEBUG
		prn_string("\nset dr mode end "); prn_dword(dwc3_reg->g_ctl);
#endif
//xhci_register
	/*xhci reset*/
#ifdef XHCI_DEBUG
		prn_string("\nxhci reset");
#endif
		tmp1 = g_io_buf.usb.xhci.hcor->or_usbsts & STS_HALT;
		if (!tmp1) {
			tmp1 = g_io_buf.usb.xhci.hcor->or_usbcmd;
			tmp1 &= ~CMD_RUN;
#ifdef XHCI_DEBUG
			prn_string("\n		or_usbcmd "); prn_dword(tmp1);
#endif
			g_io_buf.usb.xhci.hcor->or_usbcmd = tmp1;
		}
		//handshake
		handshake(&g_io_buf.usb.xhci.hcor->or_usbsts, STS_HALT, STS_HALT, XHCI_MAX_HALT_USEC/1000);

		tmp1 = g_io_buf.usb.xhci.hcor->or_usbcmd;
		tmp1 |= CMD_RESET;
		g_io_buf.usb.xhci.hcor->or_usbcmd = tmp1;
		handshake(&g_io_buf.usb.xhci.hcor->or_usbcmd, CMD_RESET, 0, XHCI_MAX_HALT_USEC/1000);
		handshake(&g_io_buf.usb.xhci.hcor->or_usbsts, STS_CNR, 0, XHCI_MAX_HALT_USEC/1000);

		/*
	 	* Program the Number of Device Slots Enabled field in the CONFIG
	 	* register with the max value of slots the HC can handle.
	 	*/
		/*xhci_lowlevel_init*/
		tmp1 = g_io_buf.usb.xhci.hccr->cr_hcsparams1 & HCS_SLOTS_MASK;
		tmp2 = g_io_buf.usb.xhci.hcor->or_config;
		tmp1 |= (tmp2 & ~HCS_SLOTS_MASK);
		g_io_buf.usb.xhci.hcor->or_config = tmp1;
#ifdef XHCI_DEBUG
		prn_string("\n  	or_config "); prn_dword(g_io_buf.usb.xhci.hcor->or_config);
#endif
		//xhci_mem_init
		xhci_mem_init();	
	
		tmp1 = g_io_buf.usb.xhci.hccr->cr_hcsparams1;
		Hub_NbrPorts_root = ((tmp1 & HCS_MAX_PORTS_MASK) >> HCS_MAX_PORTS_SHIFT);
						
		// Port Indicators ????
		tmp1 = g_io_buf.usb.xhci.hccr->cr_hccparams;
#ifdef XHCI_DEBUG
		prn_string("\n  	#Hub_NbrPorts_root "); prn_dword(Hub_NbrPorts_root);
		prn_string("  	#hccr->cr_hccparams "); prn_dword(tmp1);
#endif
		//if (HCS_INDICATOR(tmp1))
		//	put_unaligned(get_unaligned(&descriptor.hub.wHubCharacteristics)
		//			| 0x80, &descriptor.hub.wHubCharacteristics);

		// Port Power Control ????
		//if (HCC_PPC(tmp1))
		//	put_unaligned(get_unaligned(&descriptor.hub.wHubCharacteristics)
		//			| 0x01, &descriptor.hub.wHubCharacteristics);
        
// xhci_start
#ifdef XHCI_DEBUG
		prn_string("\n**<xhci_start>**");
#endif
		tmp1 = g_io_buf.usb.xhci.hcor->or_usbcmd;
		tmp1 |= (CMD_RUN);
		g_io_buf.usb.xhci.hcor->or_usbcmd = tmp1;
        	handshake(&g_io_buf.usb.xhci.hcor->or_usbsts, STS_HALT, 0, XHCI_MAX_HALT_USEC/1000);
        
		/* Zero'ing IRQ control register and IRQ pending register */
		g_io_buf.usb.xhci.ir_set->irq_control = 0x0;
		g_io_buf.usb.xhci.ir_set->irq_pending = 0x0;

		//reg = HC_VERSION(g_io_buf.usb.xhci.hccr->cr_capbase);
		//prn_string("\n XHCI "); prn_dword(HC_VERSION(g_io_buf.usb.xhci.hccr->cr_capbase));
	
		pUSB_DevDesc pDev = (pUSB_DevDesc) g_io_buf.usb.xhci.reserved;
#if defined(CONFIG_HAVE_USB3_HUB)	
		struct usb_hub_descriptor *pHub = (struct usb_hub_descriptor *) USB_dataBuf;
#endif		
		g_io_buf.usb.xhci.udev.devnum = 1; //new device start, devnum 1 is root hub.
	
		for (trb_64 = 1; trb_64 <= Hub_NbrPorts_root; trb_64++) {
 			USB_vendorCmd(0x23, USB_REQ_SET_FEATURE, 8, trb_64, 0);//usb_hub_power_on, USB_PORT_FEAT_POWER = 8
		}
		_delay_1ms(200);

		for (i = 1; i <= Hub_NbrPorts_root; i++) {
			prn_string("\n#####root ");
			val_64 = port_test(i);
			if (val_64) {			
//_xhci_alloc_device
#ifdef XHCI_DEBUG
				prn_string("\n**<alloc_device>**");
#endif
				xhci_alloc_device();
//usb_setup_descriptor
				g_io_buf.usb.xhci.udev.maxpacketsize = 3;//PACKET_SIZE_64;
				g_io_buf.usb.xhci.udev.epmaxpacketin[0] = 64;
				g_io_buf.usb.xhci.udev.epmaxpacketout[0] = 64;	
//set address    set_address(void)
#ifdef XHCI_DEBUG
        			prn_string("\n**<set address>** "); prn_dword(g_io_buf.usb.xhci.udev.devnum);
#endif
				USB_vendorCmd(0, USB_REQ_SET_ADDRESS, g_io_buf.usb.xhci.udev.devnum, 0, 0);
//usb_select_config
				usb_select_config(pDev);
				
				if (pDev->bDeviceClass == 9) {
#if defined(CONFIG_HAVE_USB3_HUB)
					update_hub_device(&g_io_buf.usb.xhci, pDev, pHub);
#ifdef XHCI_DEBUG
					prn_string("  	pHub->bNbrPorts "); prn_dword(g_io_buf.usb.xhci.udev.Hub_NbrPorts);
#endif		
					for (trb_64 = 1; trb_64 <= g_io_buf.usb.xhci.udev.Hub_NbrPorts; trb_64++) {
 						USB_vendorCmd(0x23, USB_REQ_SET_FEATURE, 8, trb_64, 0);//usb_hub_power_on, USB_PORT_FEAT_POWER = 8
					}
					_delay_1ms(200);
					next_port_in_hub++;
					break;
#endif
				} else {
					usb_storage();//usb_storage
					//break;
					prn_string("\n#####end##### \n");	
					return 0;
				}
			}
		}
		if (i > Hub_NbrPorts_root) {
			prn_string("\n#####root hub scan end ==> no device##### \n");
			return -1;
		}	
	}
#if defined(CONFIG_HAVE_USB3_HUB) 
	pUSB_DevDesc pDev = (pUSB_DevDesc) g_io_buf.usb.xhci.reserved;
	
	for (trb_64 = next_port_in_hub; trb_64 <= g_io_buf.usb.xhci.udev.Hub_NbrPorts; trb_64++) {			
		prn_string("\n###hub ");

		g_io_buf.usb.xhci.udev.slot_id = g_io_buf.usb.xhci.udev.r_slot_id;
		val_64 = port_test(trb_64);
		if (val_64) {
//_xhci_alloc_device
#ifdef XHCI_DEBUG
			prn_string("\n**<alloc_device>**");
#endif
			xhci_alloc_device();
//usb_setup_descriptor
			g_io_buf.usb.xhci.udev.maxpacketsize = 3;//PACKET_SIZE_64;
			g_io_buf.usb.xhci.udev.epmaxpacketin[0] = 64;
			g_io_buf.usb.xhci.udev.epmaxpacketout[0] = 64;	
//set address    set_address(void)
#ifdef XHCI_DEBUG
        		prn_string("\n**<set address>** "); prn_dword(g_io_buf.usb.xhci.udev.devnum);
#endif
			USB_vendorCmd(0, USB_REQ_SET_ADDRESS, g_io_buf.usb.xhci.udev.devnum, 0, 0);
//usb_select_config
			usb_select_config(pDev);							
									
			if (pDev->bDeviceClass == 9) 
				continue;
			else 
				usb_storage();//usb_storage	
			return ++trb_64;						
		} 	
	}
	prn_string("\n#####hub scan end ==> no device##### \n");
#endif	
	return -1;
}

/*
 * usb_readSector
 * @lba		LBA to read
 * @count       not used
 * @buf		destination buffer
 */
int usb_readSector(u32 lba, u32 count, u32 *dest)
{
	CSTAMP(0xE5B00013);
	
	g_io_buf.usb.xhci.reserved[0] = SCSICMD_READ_10;
	g_io_buf.usb.xhci.reserved[1] = g_io_buf.usb.xhci.udev.lun << 5;
	g_io_buf.usb.xhci.reserved[2] = (u8)(lba >> 24) & 0xff;
	g_io_buf.usb.xhci.reserved[3] = (u8)(lba >> 16) & 0xff;
	g_io_buf.usb.xhci.reserved[4] = (u8)(lba >> 8) & 0xff;
	g_io_buf.usb.xhci.reserved[5] = (u8)(lba) & 0xff;
	g_io_buf.usb.xhci.reserved[7] = (u8)(count >> 8) & 0xff;
	g_io_buf.usb.xhci.reserved[8] = (u8)count & 0xff;

	if (stor_BBB_transport((count << 9), 12, 1, (u8 *)dest) != 0)
		prn_string(" =>usb_readSector\n");
				
	CSTAMP(0xE5B00014);
	return 0;
}

