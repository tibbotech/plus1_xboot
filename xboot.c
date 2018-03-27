#include <types.h>
#include <common.h>
#include <banner.h>
#include <bootmain.h>
#include <regmap.h>
#include <fat/fat.h>
#include <cpu/cpu.h>
#include <cpu/arm.h>
#include <image.h>
#include <misc.h>
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
__attribute__ ((section("xboot_header_sect")))   u8                  g_xboot_buf[32];

static void init_hw(void)
{
	int i;
	dbg();

#ifdef CONFIG_PLATFORM_Q628
	/* clken[all]  = enable */
	for (i = 0; i < sizeof(MOON0_REG->clken) / 4; i++)
		MOON0_REG->clken[i] = RF_MASK_V_SET(0xffff);
	/* gclken[all] = no */
	for (i = 0; i < sizeof(MOON0_REG->gclken) / 4; i++)
		MOON0_REG->gclken[i] = RF_MASK_V_CLR(0xffff);
	/* reset[all] = clear */
	for (i = 0; i < sizeof(MOON0_REG->reset) / 4; i++)
		MOON0_REG->reset[i] = RF_MASK_V_CLR(0xffff);
#else
	/* clken[all] enable */
	for (i = 0; i < sizeof(MOON0_REG->clken) / 4; i++)
		MOON0_REG->clken[i] = 0xffffffff;
	/* gclken[all] = no */
	for (i = 0; i < sizeof(MOON0_REG->clken) / 4; i++)
		MOON0_REG->gclken[i] = 0;
	/* reset[all] = clear */
	for (i = 0; i < sizeof(MOON0_REG->reset) / 4; i++)
		MOON0_REG->reset[i] = 0;
#endif

	dbg();
}

static int run_draminit(void)
{
	/* skip dram init on csim/zebu */
#ifdef CONFIG_BOOT_ON_CSIM
	prn_string("skip draminit\n");
#else
	int (*dram_init)(void);
#ifdef CONFIG_STANDALONE_DRAMINIT
	dram_init = (void *)DRAMINIT_RUN_ADDR;
	prn_string("standalone draiminit\n");
	dram_init = (void *)DRAMINIT_RUN_ADDR;
#else
	extern int dram_init_main(void);
	dram_init = (void *)dram_init_main;
#endif

	prn_string("Run draiminit@"); prn_dword((u32)dram_init);
	dram_init();
	prn_string("Done draiminit\n");
#endif

	// put a brieft dram test
	if (dram_test()) {
		mon_shell();
		return -1;
	}
	return 0;
}

static inline void release_spi_ctrl(void)
{
	// SPIFL & SPI_COMBO no reset
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
	MOON0_REG->reset[0] &= ~(0x3 << 9);
#else
	//FIXME: q628
	//MOON0_REG->reset[0] = RF_MASK_V_CLR(3 << 9);
#endif
}

/* Return 1 = SPI_X1, 2 = SPI_X2  */
inline int get_current_spi_pinmux(void)
{
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
	return (MOON1_REG->sft_cfg[1] & 0x3);
#else
	//FIXME: q628
	return 1; //((MOON1_REG->sft_cfg[1] >> 5) & 0x3);
#endif
}

#ifdef CONFIG_HAVE_SPI_NOR

// return image data size (exclude header)
#ifdef CONFIG_USE_ZMEM
__attribute__((unused))
#endif
static int nor_load_uhdr_image(const char *img_name, void *dst, void *src, int verify)
{
	struct image_header *hdr;
	int i, len, step;

	prn_string("load "); prn_string(img_name);
	prn_string("@"); prn_dword((u32)dst);
	prn_string("\n");

	dbg();
	memcpy32(dst, src, sizeof(*hdr)/4); // 64/4

	dbg();
	hdr = (struct image_header *)dst;

	dbg();
	// magic check
	if (!image_check_magic(hdr)) {
		prn_string("bad mgaic\n");
		return -1;
	}
	// check name
	if (memcmp((const u8 *)image_get_name(hdr), (const u8 *)img_name, strlen(img_name)) != 0) {
		prn_string("bad name\n");
		return -1;
	}
	// header crc
	if (!image_check_hcrc(hdr)) {
		prn_string("bad hcrc\n");
		return -1;
	}

	// load image data
	len = image_get_size(hdr);
	prn_string("load data size="); prn_decimal(len); prn_string("\n");

	/* copy chunk size */
#ifdef CSIM_NEW
	step = 2048;
#else
	step = 256 * 1024;
#endif

        for (i = 0; i < len; i += step) {
                prn_string(".");
                memcpy32(dst + sizeof(*hdr) + i, src + sizeof(*hdr) + i,
                        (len - i < step) ? (len - i + 3) / 4 : step / 4);
        }
        prn_string("\n");

	// verify image data
	if (verify && !image_check_dcrc(hdr)) {
		prn_string("corrupted\n");
		return -1;
	}

	return len;
}

#ifdef CONFIG_STANDALONE_DRAMINIT
static int nor_load_draminit(void)
{
	struct xboot_hdr *xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET);
	int len;

	if (xhdr->magic != XBOOT_HDR_MAGIC) {
		prn_string("no xboot hdr\n");
		return -1;
	}

	// locate to where xboot.img.orig ends
	len = sizeof(struct xboot_hdr)  + xhdr->length;

	return nor_load_uhdr_image("draminit", (void *)DRAMINIT_LOAD_ADDR,
			(void *)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + len), 1);
}
#endif

static int nor_draminit(void)
{
#ifdef CONFIG_STANDALONE_DRAMINIT
	if (nor_load_draminit() <= 0) {
		prn_string("No draminit\n");
		return -1;
	}

	cpu_invalidate_icache_all();
#endif
	return run_draminit();
}

static void boot_next_set_addr(unsigned int addr)
{
	volatile unsigned int *next = (volatile unsigned int *)BOOT_ANOTHER_POS;
	*next = addr;
	prn_string("boot next @"); prn_dword(*next);
}

static void boot_next_in_another(void)
{
	prn_string("wake up another to run\n");

#ifdef PLATFORM_I137 /* B_SRAM address is 9e00_0000 from A view */
	*(volatile unsigned int *)A_START_POS_B_VIEW = ((u32)&boot_next_no_stack) - 0x800000;
#else
	*(volatile unsigned int *)A_START_POS_B_VIEW = (u32)&boot_next_no_stack;
#endif
	while (1);
}

#ifdef CONFIG_LOAD_LINUX
static void spi_nor_linux(void)
{
	struct image_header *hdr;

#ifdef CONFIG_USE_ZMEM
	prn_string("[zmem] check dtb\n");
	hdr = (struct image_header *)DTB_LOAD_ADDR;
	if (!image_check_magic(hdr)) {
		prn_string("[zmem] no uhdr magic: "); prn_dword(image_get_magic(hdr));
		mon_shell();
	} else if (!image_check_dcrc(hdr)) {
		prn_string("corrupted\n");
		mon_shell();
	}
	prn_string("[zmem] check linux\n");
	hdr = (struct image_header *)LINUX_LOAD_ADDR;
	if (!image_check_magic(hdr)) {
		prn_string("[zmem] no uhdr magic: "); prn_dword(image_get_magic(hdr));
		mon_shell();
	} else if (!image_check_hcrc(hdr)) {
		prn_string("bad hcrc\n");
		mon_shell();
	}
#else
	int res;
	int verify = 1;

	res = nor_load_uhdr_image("dtb", (void *)DTB_LOAD_ADDR,
			(void *)(SPI_FLASH_BASE + SPI_DTB_OFFSET), verify);
	if (res <= 0) {
		prn_string("No dtb\n");
		return;
	}

#ifdef CONFIG_BOOT_ON_CSIM
	verify = 0; /* big image */
#endif
	res = nor_load_uhdr_image("linux", (void *)LINUX_LOAD_ADDR,
			(void *)(SPI_FLASH_BASE + SPI_LINUX_OFFSET), verify);
	if (res <= 0) {
		prn_string("No linux\n");
		return;
	}
	hdr = LINUX_LOAD_ADDR;
#endif

	prn_string((const char *)image_get_name(hdr)); prn_string("\n");

	boot_next_set_addr(LINUX_RUN_ADDR);

	// if B (and vmlinux is not B's), wake up A
	if (g_bootinfo.bootcpu == 0 && *(u32 *)LINUX_RUN_ADDR != 0xe321f0d3) { /* arm9 vmlinux.bin first word */
		boot_next_in_another();
	// directly run Linux
	} else {
		prn_string("run linux@"); prn_dword(LINUX_RUN_ADDR);
		boot_next_no_stack();
	}
}
#endif

static void spi_nor_uboot(void)
{
	int len = 0;
	struct image_header *hdr;

	hdr = (struct image_header *)UBOOT_LOAD_ADDR;

#ifdef CONFIG_USE_ZMEM

	prn_string("[zmem] check uboot\n");
	if (!image_check_magic(hdr)) {
		prn_string("[zmem] no uhdr magic: "); prn_dword(image_get_magic(hdr));
		mon_shell();
	} else {
		len = image_get_size(hdr);
		prn_string("[zmem] uboot len="); prn_dword(len);
	}
#else
	len = nor_load_uhdr_image("uboot", (void *)UBOOT_LOAD_ADDR,
			(void *)(SPI_FLASH_BASE + SPI_UBOOT_OFFSET), 1);
	if (len <= 0) {
		mon_shell();
		return;
	}
#endif

	prn_string((const char *)image_get_name(hdr)); prn_string("\n");

	boot_next_set_addr(UBOOT_RUN_ADDR);

	/* if B and u-boot is A's, wake up A */
	if (g_bootinfo.bootcpu == 0 && memcmp((const u8 *)image_get_name(hdr), (const u8 *)"uboot_B", 7)) {
		boot_next_in_another();
	} else {
		prn_string("Run u-boot @"); prn_dword(UBOOT_RUN_ADDR);
		exit_bootROM(UBOOT_RUN_ADDR);
	}
}

static void spi_nor_boot(int pin_x)
{
#ifdef SPEED_UP_SPI_NOR_CLK
	dbg();
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
	SPI_CTRL_REG->spi_ctrl = (SPI_CTRL_REG->spi_ctrl & ~0x7) | 0x5; // CLK_SPI/16
#else
	SPI_CTRL_REG->spi_ctrl = (SPI_CTRL_REG->spi_ctrl & ~(7 << 16)) | (3 << 16); // 3: CLK_SPI/6
	SPI_CTRL_REG->spi_cfg[2] = 0x00150095; // restore default after seeting spi_ctrl
#endif
#endif

	if (nor_draminit()) {
		dbg();
		return;
	}

	// spi linux
#ifdef CONFIG_LOAD_LINUX
	spi_nor_linux();
#endif

	spi_nor_uboot();
}
#endif /* CONFIG_HAVE_SPI_NOR */

#ifdef CONFIG_HAVE_FS_FAT

/* return image data size (exclude header) */
static int fat_load_uhdr_image(fat_info *finfo, const char *img_name, void *dst,
	u32 img_offs, int max_img_sz)
{
	struct image_header *hdr = dst;
	int len;
	int ret;
	u8 *buf = g_io_buf.usb.sect_buf;

	prn_string("fat load ");
	prn_string(img_name);
	prn_string("\n");

	/* usb dma need aligned address */
	if ((u32)dst & 0x7ff) {
		prn_string("WARN: unaligned dst "); prn_dword((u32)dst);
	}

	/* read header first */
	len = 64;
	ret = fat_read_file(0, finfo, buf, img_offs, len, dst);
	if (ret == FAIL) {
		prn_string("load hdr failed\n");
		return -1;
	}

	/* uhdr_dump(hdr); */

	/* magic check */
	if (!image_check_magic(hdr)) {
		prn_string("bad mgaic\n");
		return -1;
	}

	/* header crc */
	if (!image_check_hcrc(hdr)) {
		prn_string("bad hcrc\n");
		return -1;
	}

	/* load image data */
	len = image_get_size(hdr);
	prn_string("load data size=");
	prn_decimal(len);
	prn_string("\n");

	if (len + 64 > max_img_sz) {
		prn_string("image is too big, size=");
		prn_decimal(len + 64);
		return -1;
	}

	ret = fat_read_file(0, finfo, buf, img_offs + 64, len, dst + 64);
	if (ret == FAIL) {
		prn_string("load body failed\n");
		return -1;
	}

	/* verify image data */
	if (!image_check_dcrc(hdr)) {
		prn_string("corrupted image\n");
		/* prn_crc(dst, len + 64, 4096); */
		return -1;
	}

	return len;
}

static void do_fat_boot(u32 type, u32 port)
{
	u32 ret;
	fat_info g_finfo;
#ifdef CONFIG_STANDALONE_DRAMINIT
	u8 *buf = (u8 *) g_io_buf.usb.draminit_tmp;
	struct xboot_hdr *xhdr = (struct xboot_hdr *)buf;
	int len;
#endif

	prn_string("finding file\n");

	ret = fat_boot(type, port, &g_finfo, g_io_buf.usb.sect_buf);
	if (ret == FAIL) {
		prn_string("no file\n");
		return;
	}

#ifdef CONFIG_STANDALONE_DRAMINIT
	/* nor_load_draminit(); */
	dbg();

	/* check xboot0 header to know draminit offset */
	len = 32;
	ret = fat_read_file(0, &g_finfo, g_io_buf.usb.sect_buf, ISP_IMG_OFF_XBOOT, len, buf);
	if (ret == FAIL) {
		prn_string("load xboot hdr failed\n");
		return;
	}

	if (xhdr->magic != XBOOT_HDR_MAGIC) {
		prn_string("xboot0 magic is wrong\n");
		return;
	}

	prn_string("xboot len=");
	prn_dword(32 + xhdr->length);

	/* draminit.img offset = size of xboot.img */
	len = fat_load_uhdr_image(&g_finfo, "draminit", buf, 32 + xhdr->length, sizeof(g_io_buf.usb.draminit_tmp));
	if (len <= 0) {
		prn_string("load draminit failed\n");
		return;
	}

	if ((u32)buf != (u32)DRAMINIT_LOAD_ADDR)
		memcpy32((u32 *) DRAMINIT_LOAD_ADDR, (u32 *) buf, (64 + len + 3) / 4);
#endif

	run_draminit();

	/* load u-boot from usb */
	if (fat_load_uhdr_image(&g_finfo, "uboot", (void *)UBOOT_LOAD_ADDR, ISP_IMG_OFF_UBOOT, 0x00200000) <= 0) {
		prn_string("failed to load uboot\n");
		return;
	}

	prn_string("Run u-boot @");
	prn_dword(UBOOT_RUN_ADDR);
	exit_bootROM(UBOOT_RUN_ADDR);
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

#define EMMC_BLOCK_SZ 512     // bytes in a eMMC sector

static int emmc_read(u8 *buf, u32 blk_off, u32 count)
{
#ifdef EMMC_USE_DMA_READ
        /* dma mode supports multi-sector read */
        return ReadSDSector(blk_off, count, (unsigned int *)buf);
#else
        /* polling mode supports single-sector read */
        int res;
        for (; count > 0; count--, buf += EMMC_BLOCK_SZ, blk_off++) {
                res = ReadSDSector(blk_off, 1, (unsigned int *)buf);
                if (res < 0) {
                        return res;
                }
        }
        return 0;
#endif
}

/**
 * emmc_load_uhdr_image
 * img_name       - image name in uhdr
 * dst            - destination address
 * loaded         - some data has been loaded
 * blk_off        - emmc block offset number
 * only_load_hdr  - only load header
 * size_limit     - image size in header > this limit, return error
 * mmc_part       - current active mmc part
 *
 * Return image data size (> 0) if ok (exclude header)
 */
static int emmc_load_uhdr_image(const char *img_name, u8 *dst, u32 loaded,
	u32 blk_off, int only_load_hdr, int size_limit, int mmc_part)
{
	struct image_header *hdr = (struct image_header *) dst;
	int len, res, blks;

	prn_string("emmc load "); prn_string(img_name);
	if (loaded) {
		prn_string("\nloaded=");
		prn_decimal_ln(loaded);
	}

	// load uhdr
	if (loaded < sizeof(*hdr)) {
		prn_string("@blk="); prn_dword(blk_off);
		res = emmc_read(dst + loaded, blk_off, 1);
		if (res) {
			prn_string("fail to read hdr\n");
			return -1;
		}
		loaded += EMMC_BLOCK_SZ;
		blk_off++;
	}

	// verify header
	if (!image_check_magic(hdr)) {
		prn_string("bad mg\n");
		return -1;
	}
	if (memcmp((const u8 *)image_get_name(hdr), (const u8 *)img_name, strlen(img_name)) != 0) {
		prn_string("bad name\n");
		return -1;
	}
	if (!image_check_hcrc(hdr)) {
		prn_string("bad hcrc\n");
		return -1;
	}

	len = image_get_size(hdr);
	if (len <= 0 || (sizeof(*hdr) + len) > size_limit)
		return -1;

	if (only_load_hdr)
		return len;

	// load image data
	prn_string("data size="); prn_decimal_ln(len);
	if ((len <= 0) || (len + sizeof(*hdr)) >= size_limit) {
		prn_string("size > limit="); prn_decimal_ln(size_limit);
		return -1;
	}

	// load remaining
	res = sizeof(*hdr) + len - loaded;
	if (res > 0) {
		blks = (res + EMMC_BLOCK_SZ - 1) / EMMC_BLOCK_SZ;
		res = emmc_read(dst + loaded, blk_off, blks);
		if (res) {
			prn_string("failed to load data\n");
			return -1;
		}
	}

	// verify image data
	prn_string("verify img...\n");
	if (!image_check_dcrc(hdr)) {
		prn_string("corrupted\n");
		return -1;
	}

	return len;
}

#ifdef CONFIG_STANDALONE_DRAMINIT
int emmc_load_draminit(void *buf, int mmc_part)
{
	u32 sz_sect = EMMC_BLOCK_SZ;
	u32 xbsize, loaded;

	/* Because draminit.img is catenated to xboot.img,
	 * initial part of draminit.img may have been loaded by xboot's last sector */
	xbsize = get_xboot_size(g_xboot_buf);
	prn_string("xbsize="); prn_dword(xbsize);
	loaded = xbsize;
	while (loaded >= sz_sect) {
		loaded -= sz_sect;
	}
	if (loaded) {
		loaded = sz_sect - loaded;
	}
	prn_string("loaded="); prn_dword(loaded);
	if (loaded) {
		memcpy32((u32 *)buf, (u32 *)(g_xboot_buf + xbsize), loaded / 4);
	}

	/* Load remaining draminit.img from xboot's following sectors */

	if (emmc_load_uhdr_image("draminit", buf, loaded, g_bootinfo.app_blk_start, 0,
				 0x10000, mmc_part) <= 0) {
		dbg();
		return -1;
	}
	return 0;
}
#endif

static void emmc_boot(void)
{
	gpt_header *gpt_hdr;
	gpt_entry *gpt_part;
	u32 blk_start1;
	int res, len = 0;
	int i;

	prn_string("\n{{emmc_boot}}\n");

	SetBootDev(DEVICE_EMMC, 1, 0);

#ifdef CONFIG_STANDALONE_DRAMINIT
	/* continue to load draminit after iboot loading xboot */
	if (emmc_load_draminit(DRAMINIT_LOAD_ADDR, g_bootinfo.mmc_active_part)) {
		dbg();
		return;
	}
#endif

	if (run_draminit()) {
		return;
	}

	if (initDriver_SD(EMMC_SLOT_NUM, MMC_USER_AREA)) {
		prn_string("init fail\n");
		return;
	} else {
		/* load uboot from GPT disk */
		prn_string("Read GPT\n");
		res = emmc_read(g_boothead, 1, 1); /* LBA 1 */
		if (res < 0) {
			prn_string("can't read LBA 1\n");
			return;
		}

		gpt_hdr = (gpt_header *)g_boothead;
		if (gpt_hdr->signature != GPT_HEADER_SIGNATURE) {
			prn_string("bad hdr sig\n");
			return;
		}

		res = emmc_read(g_boothead, 2, 1); /* LBA 2 */
		if (res < 0) {
			dbg();
			return;
		}
		gpt_part = (gpt_entry *)g_boothead;

		/* look for uboot at GPT part 1 or 2 */
		for (i = 0; i < 2; i++) {
			blk_start1 = (u32) gpt_part[i].starting_lba;
			prn_string("part"); prn_decimal(1 + i);
			prn_string(" LBA="); prn_dword(blk_start1);

			len = emmc_load_uhdr_image("uboot", (void *)UBOOT_LOAD_ADDR, 0,
				blk_start1, 0, 0x200000, MMC_USER_AREA);
			if (len > 0)
				break;
		}

		if (len <= 0) {
			prn_string("bad uboot\n");
			return;
		}
	}

	prn_string("Run u-boot @");
	prn_dword(UBOOT_RUN_ADDR);
	exit_bootROM(UBOOT_RUN_ADDR);
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
	 * g_bootinfo.gbootRom_boot_mode = USB_ISP; g_bootinfo.bootdev = DEVICE_USB_ISP; g_bootinfo.bootdev_port = 1;
	 * g_bootinfo.gbootRom_boot_mode = EMMC_BOOT;
	 * g_bootinfo.gbootRom_boot_mode = UART_ISP;
	 * g_bootinfo.gbootRom_boot_mode = NAND_LARGE_BOOT; g_bootinfo.app_blk_start = 2;
	 * g_bootinfo.gbootRom_boot_mode = SPI_NAND_BOOT;
	 * g_bootinfo.gbootRom_boot_mode = SDCARD_ISP; g_bootinfo.bootdev = DEVICE_SD0; g_bootinfo.bootdev_pinx = 1;
	 * prn_string("force boot mode="); prn_dword(g_bootinfo.gbootRom_boot_mode);
	 */

	prn_string("mode=");
	prn_dword(g_bootinfo.gbootRom_boot_mode);

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
