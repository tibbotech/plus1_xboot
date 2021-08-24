
/******************************************************************************
*                          Include File
*******************************************************************************/
#include <common.h>
#include <bootmain.h>
#include <tcpsum.h>

/******************************************************************************
*                          Global Variabl
*******************************************************************************/

/******************************************************************************
*                          External Definition
*******************************************************************************/

/******************************************************************************
*                         Function Prototype
*******************************************************************************/

unsigned int getBootDevID(void)
{
	return g_bootinfo.bootdev;
}

void set_spi_nor_pinmux(int pin_x)
{
#ifdef PLATFORM_I143
	MOON1_REG->sft_cfg[1] = RF_MASK_V(0x3 << 0, pin_x << 0);
#elif defined (PLATFORM_Q645)
	MOON1_REG->sft_cfg[1] = RF_MASK_V(0x1 << 0, pin_x << 0);
#else
	MOON1_REG->sft_cfg[1] = RF_MASK_V(0xf, (pin_x << 2) | pin_x);
#endif

}

/* Return 1 = X1,SPI_NOR, 2 = X2,SPI_NOR */
int get_spi_nor_pinmux(void)
{
#ifdef PLATFORM_Q645
	return ((MOON1_REG->sft_cfg[1] >> 0) & 0x1);
#else
	return (MOON1_REG->sft_cfg[1] & 0x3);
#endif
}

static inline void set_spi_nand_pinmux(int pin_x)
{
#ifdef PLATFORM_Q645
	MOON1_REG->sft_cfg[1] = RF_MASK_V(0x3 << 4, pin_x << 4);
#else
	MOON1_REG->sft_cfg[1] = RF_MASK_V(1 << 4, pin_x << 4);
#endif
}

static inline void set_emmc_pinmux(int pin_x)
{
#ifdef PLATFORM_I143
	MOON1_REG->sft_cfg[1] = RF_MASK_V(1 << 2, pin_x << 2);
#elif defined (PLATFORM_Q645)
	MOON1_REG->sft_cfg[1] = RF_MASK_V(1 << 3, pin_x << 3);
#else
	// Q628 eMMC : X1,CARD0_SD
	MOON1_REG->sft_cfg[1] = RF_MASK_V(1 << 5, pin_x << 5);
#endif
}

static inline void set_sdcard1_pinmux(int pin_x)
{
#ifdef PLATFORM_I143
	MOON1_REG->sft_cfg[4] = RF_MASK_V(1 << 0, pin_x << 0);
#elif defined (PLATFORM_Q645)
	MOON1_REG->sft_cfg[1] = RF_MASK_V(1 << 6, pin_x << 6);
#else
	// Q628 SD_CARD : X1,CARD1_SD
	MOON1_REG->sft_cfg[1] = RF_MASK_V(1 << 6, pin_x << 6);
#endif
}

/**
 * SetBootDev - set bootmode/bootdev/pinmux
 */
void SetBootDev(unsigned int bootdev, unsigned int pin_x, unsigned int dev_port)
{
	// stamp : I'm going to pinmux
	CSTAMP(0xC0DE5555);

	dbg();
	prn_string("dev="); prn_decimal_ln(bootdev);
	prn_string("pin="); prn_decimal_ln(pin_x); 

	g_bootinfo.bootdev = bootdev;
	g_bootinfo.bootdev_pinx = pin_x;
	g_bootinfo.bootdev_port = dev_port;

	// This switch :
	// * set gbootRom_boot_mode
	// * configure pinmux
	switch(bootdev) {
	default:
		prn_string("unknown type!\n");
		break;
	case DEVICE_USB_ISP:
		g_bootinfo.gbootRom_boot_mode = USB_ISP;
		break;
	case DEVICE_UART_ISP:
		g_bootinfo.gbootRom_boot_mode = UART_ISP;
		break;
	case DEVICE_SPI_NOR:
		g_bootinfo.gbootRom_boot_mode = SPI_NOR_BOOT;
		set_spi_nor_pinmux(pin_x);
		break;
	case DEVICE_SPI_NAND:
		g_bootinfo.gbootRom_boot_mode = SPINAND_BOOT;
		if (pin_x == 1 && get_spi_nor_pinmux() == 2) {
			set_spi_nor_pinmux(0);   /* conflict: X2,SPI_NOR */
		}
		set_spi_nand_pinmux(pin_x);
		break;
	case DEVICE_PARA_NAND:
		g_bootinfo.gbootRom_boot_mode = NAND_LARGE_BOOT;
		break;
#ifdef CONFIG_HAVE_EMMC
	case DEVICE_EMMC:
		g_bootinfo.gbootRom_boot_mode = EMMC_BOOT;
		gDEV_SDCTRL_BASE_ADRS = (unsigned int)ADDRESS_CONVERT(CARD0_CTL_REG); /* eMMC is on SD0 */
		if (pin_x == 1) {
			set_spi_nand_pinmux(0);  /* conflict: X1,SPI_NAND */
		}
		if (pin_x == 1 && get_spi_nor_pinmux() == 2) {
			set_spi_nor_pinmux(0);   /* conflict: X2,SPI_NOR */
		}
		set_emmc_pinmux(pin_x);
		break;
#endif
#ifdef CONFIG_HAVE_SDCARD
	case DEVICE_SDCARD:
		g_bootinfo.gbootRom_boot_mode = SDCARD_ISP;
		gDEV_SDCTRL_BASE_ADRS = (unsigned int)ADDRESS_CONVERT(CARD1_CTL_REG);
		set_sdcard1_pinmux(pin_x);
		break;
#endif
	}

	CSTAMP(0xC0DE5556);
}

/* return xboot size in header */
int get_xboot_size(u8 *img)
{
        struct xboot_hdr *hdr = (struct xboot_hdr *)img;

        // magic check
        if (hdr->magic != XBOOT_HDR_MAGIC) {
		dbg();
                return 0;
        }

	if (sizeof(struct xboot_hdr) + hdr->length > XBOOT_BUF_SIZE) {
		prn_string("xboot too big: "); prn_dword(hdr->length);
		return 0;
	}

	return (sizeof(struct xboot_hdr) + hdr->length);
}

#ifndef XBOOT_BUILD
/* Return ROM_SUCCESS(=0) if ok */
int verify_xboot_img(u8 *img)
{
        struct xboot_hdr *hdr = (struct xboot_hdr *)img;
	u32 checksum;

        /*
         * Verify xboot 32-byte header
         */
        prn_string("mg=");
        prn_dword(hdr->magic);
        prn_string("len=");
        prn_dword(hdr->length);
        prn_string("chk=");
        prn_dword(hdr->checksum);

        // magic check
        if (hdr->magic != XBOOT_HDR_MAGIC) {
                dbg();
                return ROM_FAIL;
        }

        // length check
        if (sizeof(*hdr) + hdr->length > XBOOT_BUF_SIZE) {
                prn_string("bad len\n");
                return ROM_FAIL;
        }

        // checksum verify
        dbg();
		checksum = tcpsum(g_xboot_buf + sizeof(*hdr), hdr->length);
        if (checksum != hdr->checksum) {
                prn_string("bad checksum, calc=");
                prn_dword(checksum);
                return ROM_FAIL;
        }

	return ROM_SUCCESS;
}
#endif

void initfunptr(UINT32 devId)
{
	switch(devId) {
#ifdef CONFIG_HAVE_PARA_NAND
	case DEVICE_PARA_NAND:
		initNandFunptr();
		break;
#endif
#ifdef CONFIG_HAVE_SPI_NAND
	case DEVICE_SPI_NAND:
		initSPINandFunptr();
		break;
#endif
	default:
		prn_string("Bad devId\n");
		break;
	}
}

#ifdef CONFIG_HAVE_NAND_COMMON
static long InitBootDev(void)
{
        g_bootinfo.gInitNand_flag = 0;

        initfunptr(getBootDevID());

        return InitNand();
}

int InitDevice(int type)
{
        int ret;

        MainInitBufferAddr_nand();

        ret = InitBootDev();
        if (ret != ROM_SUCCESS) {
                printk("InitBootDev Fail.\n");
                return ret;
        }

        // header should have been loaded
        ret = Load_Header_Profile(type);
        if (ret != ROM_SUCCESS) {
                printk("Load_Header_Profile Fail.\n");
                return ret;
        }

        return ROM_SUCCESS;
}
#endif
