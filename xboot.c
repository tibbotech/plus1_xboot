#include <types.h>
#include <common.h>
#include <banner.h>
#include <bootmain.h>
#include <regmap.h>
#include <fat/fat.h>
#include <cpu/cpu.h>
#include <cpu/arm.h>
#ifdef CONFIG_HAVE_EMMC
#include <sdmmc_boot/drv_sd_mmc.h>    /* initDriver_SD */
#include <part_efi.h>
#endif

/*
 * TOC
 * ---------------------
 *  spi_nor_boot
 *  spi_nand_boot
 *  para_nand_boot
 *  emmc_boot
 *  usb_isp
 *  sdcard_isp
 */

extern void *__etext, *__data, *__edata;
extern void *__except_stack_top;

__attribute__ ((section("storage_buf_sect")))    union storage_buf   g_io_buf;
__attribute__ ((section("bootinfo_sect")))       struct bootinfo     g_bootinfo;
__attribute__ ((section("boothead_sect")))       u8                  g_boothead[GLOBAL_HEADER_SIZE];
//__attribute__ ((section("xboot_buf_sect")))      u8                  g_xboot_buf[XBOOT_BUF_SIZE];

static void init_hw(void)
{
	dbg();

	/* clken0 ~ 3 enable */
	MOON0_REG->clken[0] = 0xFFFFFFFF;
	MOON0_REG->clken[1] = 0xFFFFFFFF;
	MOON0_REG->clken[2] = 0xFFFFFFFF;
	MOON0_REG->clken[3] = 0xFFFFFFFF;
	MOON0_REG->clken[4] = 0xFFFFFFFF;

	/* gclken0 ~ 3 */
	MOON0_REG->gclken[0] = 0;
	MOON0_REG->gclken[1] = 0;
	MOON0_REG->gclken[2] = 0;
	MOON0_REG->gclken[3] = 0;
	MOON0_REG->gclken[4] = 0;

	/* release module reset */
	MOON0_REG->reset[0] = 0;
	MOON0_REG->reset[1] = 0;
	MOON0_REG->reset[2] = 0;
	MOON0_REG->reset[3] = 0;
	MOON0_REG->reset[4] = 0;

	dbg();
}

static inline void release_spi_ctrl(void)
{
	// SPIFL & SPI_COMBO no reset
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
	MOON0_REG->reset[0] &= ~(0x3 << 9);
#else
	MOON0_REG->reset[0] = RF_MASK_V_CLR(3 << 9);
#endif
}

/* Return 1 = SPI_X1, 2 = SPI_X2  */
inline int get_current_spi_pinmux(void)
{
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
	return (MOON1_REG->sft_cfg[1] & 0x3);
#else
	return ((MOON1_REG->sft_cfg[1] >> 5) & 0x3);
#endif
}

#ifdef CONFIG_HAVE_SPI_NOR
static void spi_nor_boot(int pin_x)
{
	//FIXME: spi uboot
	mon_shell();
}
#endif

#ifdef CONFIG_HAVE_FS_FAT

#define ISP_IMG_OFF_XBOOT    0
static void do_fat_boot(u32 type, u32 port)
{
	fat_info        g_finfo;
	u32 ret;
	u8 *buf = (u8 *) g_io_buf.usb.draminit_tmp;

	dbg();
	prn_string("finding file\n");

	ret = fat_boot(type, port, &g_finfo, buf);
	if (ret == FAIL) {
		dbg();
		return;
	}

	dbg();
	prn_string("loading file\n");
	//FIXME: usb  read
	mon_shell();
}
#endif /* CONFIG_HAVE_FS_FAT */

#ifdef CONFIG_HAVE_USB_DISK
static void usb_isp(void)
{
	dbg();
	prn_string("\n{{usb_isp}}\n");

	do_fat_boot(USB_ISP, g_bootinfo.bootdev_port);
}
#endif

#ifdef CONFIG_HAVE_SDCARD
static void sdcard_isp(void)
{
	prn_string("\n{{sdcard_isp}}\n");
	do_fat_boot(SDCARD_ISP, 1);
}
#endif

#ifdef CONFIG_HAVE_UART_BOOTSTRAP
static void uart_isp(u32 forever)
{
	prn_string("\n{{uart_isp}}\n");
	mon_shell();
}
#endif

#ifdef CONFIG_HAVE_EMMC
static void emmc_boot(void)
{
	prn_string("\n{{emmc_boot}}\n");
	//FIXME: emmc boot
	mon_shell();
}
#endif /* CONFIG_HAVE_EMMC */

#ifdef CONFIG_HAVE_NAND_COMMON
static void nand_uboot(u32 type)
{
	prn_string("\n{{nand_boot}}\n");
	//FIXME
	mon_shell();
}
#endif

#ifdef CONFIG_HAVE_PARA_NAND 
static void release_para_nand(void)
{
#ifdef PLATFORM_8388
	MOON0_REG->reset[2] &= ~(0x3 << 3);  // NAND & BCH no reset
#else
	// Q628 has no reset BCH
#endif
}

static void para_nand_boot(int pin_x)
{
	u32 ret;

	prn_string("\n{{nand_boot}}\n");
	release_para_nand();
	dbg();
	SetBootDev(DEVICE_PARA_NAND, 1, 0);
	ret = InitDevice(0);
	if (ret == ROM_SUCCESS) {
		nand_uboot(NAND_LARGE_BOOT);
	}
	dbg();
}
#endif

#ifdef CONFIG_HAVE_SPI_NAND
static void spi_nand_boot(int pin_x)
{
	u32 ret;
	prn_string("\n{{spi_nand_boot}}\n");
	prn_decimal(pin_x); prn_string("\n");

#ifdef PLATFORM_SPIBAREMETAL
	if (get_current_spi_pinmux() == pin_x) {
		prn_string("skip in-use pins\n");
		return;
	}
#endif

	dbg();
	SetBootDev(DEVICE_SPI_NAND, pin_x, 0);
	ret = InitDevice(SPINAND_BOOT);
	if (ret == ROM_SUCCESS) {
		nand_uboot(SPINAND_BOOT);
	}
	dbg();
}
#endif

void boot_not_support(void)
{
	diag_printf("Not support boot mode 0x%x in this build\n",
		    g_bootinfo.gbootRom_boot_mode);
	mon_shell();
}

/*
 * boot_flow - Top boot flow logic
 */
static void boot_flow(void)
{
	/* Force romcode boot mode for xBoot testings :
	 * g_bootinfo.gbootRom_boot_mode = UART_ISP;
	 * g_bootinfo.gbootRom_boot_mode = NAND_LARGE_BOOT; g_bootinfo.app_blk_start = 2;
	 * g_bootinfo.gbootRom_boot_mode = SPI_NAND_BOOT;
	 * g_bootinfo.gbootRom_boot_mode = USB_ISP; g_bootinfo.bootdev = DEVICE_USB1_ISP;
	 * g_bootinfo.gbootRom_boot_mode = SDCARD_ISP; g_bootinfo.bootdev = DEVICE_SD0; g_bootinfo.bootdev_pinx = 1;
	 * prn_string("force boot mode="); prn_dword(g_bootinfo.gbootRom_boot_mode);
	 */

	/* coverity[no_escape] */
	while (1) {
		/* Read boot mode */
		switch (g_bootinfo.gbootRom_boot_mode) {
		case UART_ISP:
#ifdef CONFIG_HAVE_UART_BOOTSTRAP
			dbg();
			uart_isp(1);
#else
			boot_not_support();
#endif
			break;
		case USB_ISP:
#ifdef CONFIG_HAVE_USB_DISK
			dbg();
			usb_isp();
#else
			boot_not_support();
#endif
			break;
		case SDCARD_ISP:
#ifdef CONFIG_HAVE_SDCARD
			CSTAMP(0xC0DE000C);
			dbg();
			sdcard_isp();
#else
			boot_not_support();
#endif
			break;
		case SPI_NOR_BOOT:
#ifdef CONFIG_HAVE_SPI_NOR
			spi_nor_boot(g_bootinfo.bootdev_pinx);
#else
			boot_not_support();
#endif
			break;
		case SPINAND_BOOT:
#ifdef CONFIG_HAVE_SPI_NAND 
			spi_nand_boot(g_bootinfo.bootdev_pinx);
#else
			boot_not_support();
#endif
			break;
		case NAND_LARGE_BOOT:
#ifdef CONFIG_HAVE_PARA_NAND 
			para_nand_boot(g_bootinfo.bootdev_pinx);
#else
			boot_not_support();
#endif
			break;
		case EMMC_BOOT:
#ifdef CONFIG_HAVE_EMMC
			emmc_boot();
#else
			boot_not_support();
#endif
			break;
		default:
			dbg();
			boot_not_support();
		}
	}
}

static inline void init_cdata(void)
{
	char *src = (char *)&__etext;
	char *dst = (char *)&__data;
	while (dst < (char *)&__edata) {
	        *dst++ = *src++;
	}
}

void xboot_main(void)
{
	/* Initialize global data */
	init_cdata();

	g_bootinfo.in_xboot = 1;

	/* Is MP chip? Silent UART */
	//g_bootinfo.mp_flag = read_mp_bit();

	prn_decimal_ln(AV1_GetStc32());

	/* first msg */
	prn_string("+++xBoot " __DATE__ " " __TIME__ "\n");
	if ((cpu_main_id() & 0xfff0) == 0x9260)
		prn_string("-- B --\n");
	else
		prn_string("-- A --\n");

	/* init hw */
	init_hw();

	/* start boot flow */
	boot_flow();

	/*
	 * build-time bug checker
	 */
	BUILD_BUG_ON(sizeof(union storage_buf) > STORAGE_BUF_SIZE);
}
