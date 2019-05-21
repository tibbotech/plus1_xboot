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
#include <otp/sp_otp.h>
#ifdef CONFIG_FT_ROM_TEST
#include <gpio_drv.h>
#endif
#ifdef CONFIG_HAVE_EMMC
#include <sdmmc_boot/drv_sd_mmc.h>    /* initDriver_SD */
#include <part_efi.h>
#endif

#ifdef CONFIG_SECURE_BOOT_SIGN
#define VERIFY_SIGN_MAGIC_DATA	(0x7369676E)
#define SIGN_DATA_SIZE	(64+8)// 64:sign data  8:flag data
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
#ifdef CONFIG_HAVE_OTP
extern void mon_rw_otp(void);
#endif

extern void *__etext, *__data, *__edata;
extern void *__except_stack_top;

__attribute__ ((section("storage_buf_sect")))    union storage_buf   g_io_buf;
__attribute__ ((section("bootinfo_sect")))       struct bootinfo     g_bootinfo;
__attribute__ ((section("boothead_sect")))       u8                  g_boothead[GLOBAL_HEADER_SIZE];
__attribute__ ((section("xboot_header_sect")))   u8                  g_xboot_buf[32];

#ifdef CONFIG_FT_ROM_TEST
static void FT_Rom_Test_End(char gpioData)
{
	u32 gpio = 34;
	int i = 0;
	GPIO_F_SET(gpio,1);
	GPIO_M_SET(gpio,1);
	GPIO_E_SET(gpio,1);
	for(i = 7;i >= 0;i--)
	{
		GPIO_O_SET(gpio,(gpioData>>i)&0x01);
		_delay_1ms(2);
	}
}
#endif

#ifdef CONFIG_SECURE_BOOT_SIGN
u8 *data=NULL, *sig=NULL;
u8 in_pub[32] = {0};
unsigned int data_size=0;

static void load_otp_pub_key(void)
{
	int i;
	for (i = 0; i < 32; i++) {
		sunplus_otprx_read(i+64,(char *)&in_pub[i]);
	}
	prn_string("OTP pub-key:\n");
	prn_dump_buffer(in_pub, 32);
}

int verify_uboot_signature(const struct image_header  *hdr)
{
	int sig_size = 64;
	int sig_flag_size = 8;
	int ret = -1;
	int mmu = 1;
	int imgsize = 0;
	u8 sig_flag[8] = {0};
	
	/* Load public key */
	if (g_bootinfo.sb_flag & SB_FLAG_ENABLE) {
		prn_string("* Secure *\n");
	}

	imgsize = image_get_size(hdr);

	/* load signature from image end */
	if (imgsize < sig_size) {
		prn_string("too small img\n");
		goto out;
	}
	prn_string("Verify signature...(xboot-->uboot)\n");

	if (!g_bootinfo.bootcpu) {
		/* If B boots, need A to enable mmu */
		if ((HB_GP_REG->hb_otp_data0 >> 11) & 0x1) {
			prn_string("(B only)");
			mmu = 0;
		} else {
			prn_string("(AB)");
		}
	}

	data = ((u8 *)hdr);
	data_size = imgsize  + sizeof(struct image_header);//- sig_size-sig_flag_size;
	sig = data + data_size+sig_flag_size;
	
	sig_flag[0]=*(u8 *)(data+data_size); // get sign flag data
	sig_flag[1]=*(u8 *)((data+data_size)+1);
	sig_flag[2]=*(u8 *)((data+data_size)+2);
	sig_flag[3]=*(u8 *)((data+data_size)+3);
	u32 sig_magic_data = (sig_flag[0]<<24)|(sig_flag[1]<<16)|(sig_flag[2]<<8)|(sig_flag[3]);
	prn_string("sig_magic_data=");prn_dword0(sig_magic_data);
	if(sig_magic_data != VERIFY_SIGN_MAGIC_DATA)
	{
		prn_string("\n imgdata no secure flag \n");
		goto out;
	}
	
	load_otp_pub_key();

	/* verify signature */
	int  (*fptr)(const unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key);;
	fptr = (int (*)(const unsigned char *, const unsigned char *, size_t , const unsigned char *))SECURE_VERIFY_FUN_ADDR;

	if (mmu) {
		/* enable mmu and dcache */
		fill_mmu_page_table();
		enable_mmu();
	}
	ret = !fptr(sig, data, data_size, in_pub);
	if (ret) {
		prn_string("\nverify FAIL !!!!!!\t signature:\n");
		prn_dump_buffer(sig, sig_size);
	} else {
		prn_string("\nverify OK  !!!!!!\n");
	}
	/* disable dcache */
	if (mmu) {
		disable_mmu();
	}
out:
	/* Not Secure Chip => still allow booting */
	if ((ret != 0) && (!(g_bootinfo.sb_flag & SB_FLAG_ENABLE))) {
		prn_string("\n ******OTP Secure Boot is OFF ******\n");
		return 0;
	}
	return ret;
}
#endif

#ifdef CONFIG_PLATFORM_Q628
static int b_pll_get_rate(void)
{
	unsigned int reg = MOON4_REG->pllsys;    /* G4.26 */
	unsigned int reg2 = MOON4_REG->clk_sel0; /* G4.27 */

	if ((reg >> 9) & 1) /* bypass? */
		return 27000000;
	return (((reg & 0xf) + 1) * 13500000) >> ((reg2 >> 4) & 0xf);
}
#endif

#if defined(PLATFORM_I137) || defined(CONFIG_PLATFORM_Q628)
static void prn_clk_info(int is_A)
{
	unsigned int b_sysclk, io_ctrl;
	unsigned int a_pllclk, coreclk, ioclk, sysclk, clk_cfg, a_pllioclk;

	prn_string("B: b_sysclk=");
#if defined(PLATFORM_I137)
	b_sysclk = CLK_B_PLLSYS >> ((MOON0_REG->clk_sel[1] >> 4) & 7);
#else
	b_sysclk = b_pll_get_rate();
#endif
	prn_decimal(b_sysclk / 1000000);
	prn_string("M abio_ctrl=(");
	io_ctrl = BIO_CTL_REG->io_ctrl;
	prn_decimal((io_ctrl & 2) ? 16 : 8);
	prn_string("bit,"); prn_string((io_ctrl & 1) ? "DDR)\n" : "SDR)\n");

	if (is_A) {
		clk_cfg = A_MOON0_REG->clk_cfg;
		a_pllclk = (((A_MOON0_REG->pll_ctl[0] >> 16) & 0xff) + 1) * (27 * 1000 * 1000);
		coreclk = a_pllclk / (1 + ((clk_cfg >> 10) & 1));
		sysclk = coreclk / (1 + ((clk_cfg >> 3) & 1));
		a_pllioclk = (((A_MOON0_REG->pllio_ctl[0] >> 16) & 0xff) + 1) * (27 * 1000 * 1000);
		ioclk = a_pllioclk / (20 + 5 * ((clk_cfg >> 4) & 7)) / ((clk_cfg >> 16) & 0xff) * 10;
		prn_string("A: a_pllc="); prn_decimal(a_pllclk / 1000000);
		prn_string("M core="); prn_decimal(coreclk / 1000000);
		prn_string("M a_sysclk="); prn_decimal(sysclk / 1000000);
		prn_string("M a_pllio="); prn_decimal(a_pllioclk / 1000000);
		prn_string("M abio_bus="); prn_decimal(ioclk / 1000000);
		prn_string("M\n");
	}
}
#endif

static void prn_A_setup(void)
{
#ifdef CONFIG_PLATFORM_Q628
	prn_string("A_G0.11(pll): "); prn_dword(A_PLL_CTL0_CFG);
	prn_string("A_G0.3(abio): "); prn_dword(ABIO_CFG);
	prn_string("A_G0.18(ioctrl): "); prn_dword(ABIO_IOCTRL_CFG);
#endif
}

static void init_hw(void)
{
	int i;
	__attribute__((unused)) int is_A = 0;

	dbg();

#if 0 /* experiment : slower b_sysclk  */
	//MOON4_REG->pllsys = RF_MASK_V(0xf, 0xe); /* 202.5 (default) */
	//MOON4_REG->pllsys = RF_MASK_V(0xf, 0xd); /* 189 */
	MOON4_REG->pllsys = RF_MASK_V(0xf, 0x7); /* 108 */
#endif

	if ((cpu_main_id() & 0xfff0) == 0x9260)
		prn_string("-- B --\n");
	else {
		is_A = 1;
		prn_string("-- A --\n");
		prn_A_setup();
#ifdef CONFIG_PLATFORM_Q628
		/* raise ca7 clock */
		extern void A_raise_pll(void);
		A_raise_pll();
#endif
#if defined(PLATFORM_I137) || defined(CONFIG_PLATFORM_Q628)
		extern void A_setup_abio(void);
		A_setup_abio();
#endif
#ifdef CONFIG_PLATFORM_Q628
		extern void A_bus_fixup(void);
		A_bus_fixup();
#endif
	}

#ifdef CONFIG_PLATFORM_Q628
#ifdef CONFIG_PARTIAL_CLKEN
	prn_string("partial clken\n");
	/* power saving, provided by yuwen + CARD_CTL4 */
	const int ps_clken[] = {
		0x67ef, 0xffff, 0xff03, 0xfff0, 0x0004, /* G0.1~5  */
		0x0000, 0x8000, 0xffff, 0x0040, 0x0004, /* G0.6~10 */
	};
	for (i = 0; i < sizeof(MOON0_REG->clken) / 4; i++)
		MOON0_REG->clken[i] = RF_MASK_V(0xffff, ps_clken[i]);
#else
	/* clken[all]  = enable */
	for (i = 0; i < sizeof(MOON0_REG->clken) / 4; i++)
		MOON0_REG->clken[i] = RF_MASK_V_SET(0xffff);
#endif
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

#if defined(PLATFORM_I137) || defined(CONFIG_PLATFORM_Q628)
	prn_clk_info(is_A);
#endif

#if defined(CONFIG_PLATFORM_Q628) && !defined(CONFIG_DISABLE_CORE2_3)
	if (is_A) {
		prn_string("release cores\n");
		extern void A_release_cores(void);
		A_release_cores();
	}
#endif

	dbg();
}

static void fixup_boot_compatible(void)
{
	prn_string("put bootinfo\n");

	/* bootinfo and bhdr SRAM addresses are changed in new iBoot ROM v1.02.
	 * Have a copy in old addresses so that u-boot can use it.
	 * Though these addresses are in new 3K-64 stack. 2K-64 stack is sufficient near
	 * exit_xboot.
	 */
#define ROM_V100_BOOTINFO_ADDR	0x9e809400
#define ROM_V100_BHDR_ADDR	0x9e809600
	memcpy((u8 *)ROM_V100_BOOTINFO_ADDR, &g_bootinfo, sizeof(struct bootinfo));

	if ((g_bootinfo.gbootRom_boot_mode == SPINAND_BOOT) ||
	    (g_bootinfo.gbootRom_boot_mode == NAND_LARGE_BOOT)) {
		memcpy((u8 *)ROM_V100_BHDR_ADDR, &g_boothead, GLOBAL_HEADER_SIZE);
	}
}

static void exit_xboot(const char *msg, u32 addr)
{
	fixup_boot_compatible();

	prn_decimal_ln(AV1_GetStc32());
	if (msg) {
		prn_string(msg); prn_dword(addr);
	}

	exit_bootROM(addr);
}

static int run_draminit(void)
{
	/* skip dram init on csim/zebu */
#ifdef CONFIG_BOOT_ON_CSIM
	prn_string("skip draminit\n");
#else
	int save_val;
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
	save_val = g_bootinfo.mp_flag;
#ifdef PLATFORM_3502
	g_bootinfo.mp_flag = 1;		/* mask prints */
#endif
	int size = dram_init();
	if(size == 1)
		diag_printf("\n  >>>>>> dram size is 1G  <<<<<< \n\n",size);
	else if(size == 3)
		diag_printf("\n  >>>>>>  dram size is 4G  <<<<<<\n\n",size);
	else
		diag_printf("\n  >>>>>> dram size is other  <<<<<<\n\n",size);
	g_bootinfo.mp_flag = save_val;	/* restore prints */
	prn_string("Done draiminit\n");
#endif


#ifdef CONFIG_USE_ZMEM
	/* don't corrupt zmem */
	return 0;
#endif

	// put a brieft dram test
	if (dram_test()) {
		mon_shell();
		return -1;
	}
#ifdef CONFIG_FT_ROM_TEST
	FT_Rom_Test_End(0xFF);
#endif	
	return 0;
}

static inline void release_spi_ctrl(void)
{
	// SPIFL & SPI_COMBO no reset
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
	MOON0_REG->reset[0] &= ~(0x3 << 9);
#else
	/* Q628 SPI NOR */
	MOON0_REG->reset[0] = RF_MASK_V_CLR(3 << 9); /* SPI_COMBO_RESET=0, SPIFL_RESET=0 */
#endif
}

__attribute__((unused))
static void uhdr_dump(struct image_header *hdr)
{
	prn_string("magic=");
	prn_dword(image_get_magic(hdr));
	prn_string("hcrc =");
	prn_dword(image_get_hcrc(hdr));
	prn_string("time =");
	prn_dword(image_get_time(hdr));
	prn_string("size =");
	prn_dword(image_get_size(hdr));
	prn_string("load =");
	prn_dword(image_get_load(hdr));
	prn_string("entry=");
	prn_dword(image_get_ep(hdr));
	prn_string("dcrc =");
	prn_dword(image_get_dcrc(hdr));
	prn_string("name =");
	prn_string(image_get_name(hdr));
	prn_string("\n");
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
	
#ifdef CONFIG_SECURE_BOOT_SIGN
	len = image_get_size(hdr)+ SIGN_DATA_SIZE;
#else
	len = image_get_size(hdr);
#endif
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

//#define IPC_B2A_TEST
#ifdef IPC_B2A_TEST
#define IPC_A2B		(0x9c008100) // G258
#define IPC_B2A		(0x9c008180) // G259
#define CA7_READY	(0xca700001)

static void ipc_b2a_test(void)
{
	volatile unsigned int *a2b = (volatile unsigned int *)IPC_A2B;
	volatile unsigned int *b2a = (volatile unsigned int *)IPC_B2A;

	prn_string("IPC test:\nwait A ready...\n");
	while (a2b[31] != CA7_READY);

	prn_string("test B2A...\n");
	// direct (mbox)
	b2a[24] = 0x12345678;
	b2a[25] = 0x5a5a5a5a;
	b2a[26] = 0xa5a5a5a5;
	b2a[27] = 0xdeadc0de;
	b2a[28] = 0x01010101;
	b2a[29] = 0x19730611;
	b2a[30] = 0x87654321;
	b2a[31] = 0x00000000;
	// rpc
	b2a[0] = 1;
}
#endif

static void halt(void)
{
	while (1) {
		cpu_wfi();
	}
}

static void boot_next_in_A(void)
{
	fixup_boot_compatible();

	prn_string("wake up A\n");

	prn_A_setup();

	/* Wake up another to run from boot_next_no_stack() */
#ifdef PLATFORM_I137 /* B_SRAM address is 9e00_0000 from A view */
	*(volatile unsigned int *)A_START_POS_B_VIEW = ((u32)&boot_next_no_stack) - 0x800000;
#else
	*(volatile unsigned int *)A_START_POS_B_VIEW = (u32)&boot_next_no_stack;
#endif

	/* Drop to shell if having 2nd uart debug port */
#ifdef CONFIG_DEBUG_WITH_2ND_UART
	mon_shell();
#endif

#ifdef IPC_B2A_TEST
	ipc_b2a_test();
#endif

	/* B halt */
#ifdef CONFIG_PLATFORM_Q628
	prn_string("B wfi\n");
	halt();
#endif

	while (1);
}

/* Assume u-boot has been loaded */
static void boot_uboot(void)
{
	int is_for_A = 0;
	const struct image_header *hdr = (struct image_header *)UBOOT_LOAD_ADDR;
#ifdef CONFIG_SECURE_BOOT_SIGN
	prn_string(" start verify in xboot!!!!!\n");
	int ret = verify_uboot_signature(hdr);
	if(ret)
	{
		prn_string(" verify  fail !!!!!\\n");
		halt();
	}
#endif
	prn_string((const char *)image_get_name(hdr)); prn_string("\n");

	boot_next_set_addr(UBOOT_RUN_ADDR);

	is_for_A = memcmp((const u8 *)image_get_name(hdr), (const u8 *)"uboot_B", 7);

	/* if B but image is for A, wake up A */
	if (g_bootinfo.bootcpu == 0 && is_for_A) {
		boot_next_in_A();
	} else {
		/* if A but image is for B, pause */
		if (g_bootinfo.bootcpu == 1 && !is_for_A) {
			prn_string("WARN: A can't run B's u-boot\n");
			mon_shell();
		}

		exit_xboot("Run u-boot @", UBOOT_RUN_ADDR);
	}
}

#ifdef CONFIG_LOAD_LINUX

/* Assume dtb and uImage has been loaded */
static void boot_linux(void)
{
	int is_for_A = 0;
	const struct image_header *hdr = (struct image_header *)LINUX_LOAD_ADDR;

	prn_string((const char *)image_get_name(hdr)); prn_string("\n");

	boot_next_set_addr(LINUX_RUN_ADDR);

	is_for_A = (*(u32 *)LINUX_RUN_ADDR != 0xe321f0d3); /* arm9 vmlinux.bin first word */

	/* if B but image is for A, wake up A */
	if (g_bootinfo.bootcpu == 0 && is_for_A) {
		boot_next_in_A();
	} else {
		/* if A but image is for B, pause */
		if (g_bootinfo.bootcpu == 1 && !is_for_A) {
			prn_string("WARN: A can't run B's u-boot\n");
			mon_shell();
		}

		prn_string("run linux@"); prn_dword(LINUX_RUN_ADDR);
		boot_next_no_stack();
	}
}

static void spi_nor_linux(void)
{
	struct image_header *hdr;

#ifdef CONFIG_USE_ZMEM
#ifdef LOAD_SPLIT_INITRAMFS
	prn_string("[zmem] chk initramfs\n");
	hdr = (struct image_header *)INITRAMFS_LOAD_ADDR;
	if (!image_check_magic(hdr)) {
		prn_string("[zmem] no uhdr magic: "); prn_dword(image_get_magic(hdr));
		mon_shell();
	} else if (!image_check_hcrc(hdr)) {
		prn_string("bad hcrc\n");
		mon_shell();
	}
#endif
	prn_string("[zmem] chk dtb\n");
	hdr = (struct image_header *)DTB_LOAD_ADDR;
	if (!image_check_magic(hdr)) {
		prn_string("[zmem] no uhdr magic: "); prn_dword(image_get_magic(hdr));
		mon_shell();
	} else if (!image_check_dcrc(hdr)) {
		prn_string("corrupted\n");
		mon_shell();
	}
	prn_string("[zmem] chk linux\n");
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

#ifdef LOAD_SPLIT_INITRAMFS
#ifdef CONFIG_BOOT_ON_CSIM
	verify = 0; /* big */
#endif
	res = nor_load_uhdr_image("initramfs", (void *)INITRAMFS_LOAD_ADDR,
			(void *)(SPI_FLASH_BASE + SPI_INITRAMFS_OFFSET), verify);
	if (res <= 0) {
		prn_string("No initramfs!!!!!!!!!!!!!!!!!!!!!!!\n");
		//return;
	}
#endif

	verify = 1;
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
#endif

	boot_linux();
}
#endif

static void spi_nor_uboot(void)
{
	int len = 0;

#ifdef CONFIG_USE_ZMEM
	struct image_header *hdr;
	hdr = (struct image_header *)UBOOT_LOAD_ADDR;
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

	boot_uboot();
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
#ifdef CONFIG_SECURE_BOOT_SIGN
	ret = fat_read_file(0, finfo, buf, img_offs + 64, len + SIGN_DATA_SIZE, dst + 64);
#else
	ret = fat_read_file(0, finfo, buf, img_offs + 64, len, dst + 64);
#endif
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
	if (fat_load_uhdr_image(&g_finfo, "uboot", (void *)UBOOT_LOAD_ADDR, ISP_IMG_OFF_UBOOT, UBOOT_MAX_LEN) <= 0) {
		prn_string("failed to load uboot\n");
		return;
	}

	boot_uboot();
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
#ifdef PLATFORM_8388
	/* polling mode supports single-sector read */
	int res;
	for (; count > 0; count--, buf += EMMC_BLOCK_SZ, blk_off++) {
		res = ReadSDSector(blk_off, 1, (unsigned int *)buf);
		if (res < 0) {
			return res;
		}
	}
	return 0;
#else
	return ReadSDSector(blk_off, count, (unsigned int *)buf);
#endif
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

	if (only_load_hdr)
		return len;

	// load image data
	prn_string("data size="); prn_decimal_ln(len);
	if ((len <= 0) || (len + sizeof(*hdr)) >= size_limit) {
		prn_string("size > limit="); prn_decimal_ln(size_limit);
		return -1;
	}

	// load remaining
#ifdef CONFIG_SECURE_BOOT_SIGN
	res = sizeof(*hdr) + len + SIGN_DATA_SIZE - loaded;
#else
	res = sizeof(*hdr) + len - loaded;
#endif
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
	u32 blk_start1 = -1, blk_start2 = -1;
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

	if (initDriver_SD(EMMC_SLOT_NUM)) {
		prn_string("init fail\n");
		return;
	}

	/* mark active part */
	g_bootinfo.mmc_active_part = MMC_USER_AREA;
	if (SetMMCPartitionNum(0)) {
		prn_string("switch user area fail\n");
		return;
	}

	/* load uboot from GPT disk */
	prn_string("Read GPT\n");
	res = emmc_read(g_boothead, 1, 1); /* LBA 1 */
	if (res < 0) {
		prn_string("can't read LBA 1\n");
		return;
	}

	gpt_hdr = (gpt_header *)g_boothead;
	if (gpt_hdr->signature != GPT_HEADER_SIGNATURE) {
		prn_string("bad hdr sig: "); prn_dword((u32)gpt_hdr->signature);
		return;
	}

	res = emmc_read(g_boothead, 2, 1); /* LBA 2 */
	if (res < 0) {
		dbg();
		return;
	}
	gpt_part = (gpt_entry *)g_boothead;

	/* Look for uboot from GPT parts :
	 * part1 : xboot  / uboot1
	 * part2 : uboot1 / uboot2
	 * part3 : uboot2
	 * part4 : any
	 */
#ifndef HAVE_UBOOT2_IN_EMMC
	for (i = 0; i < 4; i++) {
		blk_start1 = (u32) gpt_part[i].starting_lba;
		prn_string("part"); prn_decimal(1 + i);
		prn_string(" LBA="); prn_dword(blk_start1);

		len = emmc_load_uhdr_image("uboot", (void *)UBOOT_LOAD_ADDR, 0,
				blk_start1, 0, UBOOT_MAX_LEN, MMC_USER_AREA);
		if (len > 0)
			break;
	}
#else
	/* uboot1 - facotry default uboot image
	 * uboot2 - updated uboot image
	 * Logic:
	 * 1) Load uboot1 (header only)
	 * 2) Load uboot2
	 * 3) If uboot2 is not good, load uboot1
	 */
	for (i = 0; i < 4; i++) {
		if (blk_start1 == -1) {
			/* look for uboot1 */
			blk_start1 = (u32) gpt_part[i].starting_lba;
			prn_string("part"); prn_decimal(1 + i);
			prn_string(" LBA="); prn_dword(blk_start1);
			len = emmc_load_uhdr_image("uboot", (void *)UBOOT_LOAD_ADDR, 0,
					blk_start1, 1, 0x200, MMC_USER_AREA);
			if (len > 0)
				prn_string("uboot1 hdr good\n");
			else
				blk_start1 = -1;
		} else {
			/* look for uboot2 */
			blk_start2 = (u32) gpt_part[i].starting_lba;
			prn_string("part"); prn_decimal(1 + i);
			prn_string(" LBA="); prn_dword(blk_start2);
			len = emmc_load_uhdr_image("uboot", (void *)UBOOT_LOAD_ADDR, 0,
					blk_start2, 0, UBOOT_MAX_LEN, MMC_USER_AREA);
			if (len > 0) {
				prn_string("uboot2 good\n");
				break; /* good uboot2 */
			}
			blk_start2 = -1;
		}
	}
	/* fallback to uboot1 if no uboot2 */
	if ((blk_start2 == -1) && (blk_start1 != -1)) {
		prn_string("use uboot1\n");
		len = emmc_load_uhdr_image("uboot", (void *)UBOOT_LOAD_ADDR, 0,
					   blk_start1, 0, UBOOT_MAX_LEN, MMC_USER_AREA);
	}
#endif
	if (len <= 0) {
		prn_string("bad uboot\n");
		return;
	}

	boot_uboot();
}
#endif /* CONFIG_HAVE_EMMC */

#ifdef CONFIG_HAVE_NAND_COMMON

/* bblk_read - Read boot block data
 * dst           - destination address
 * blk_off       - nand block offset
 * read_length   - length to read
 * max_blk_skip  - max bad blocks to skip
 * blk_last_read - last nand block read
 *
 * Return 0 if ok
 */
static int bblk_read(int type, u8 *dst, u32 blk_off, u32 read_length,
		     int max_blk_skip, u32 *blk_last_read)
{
	u32 pg_off, length, got = 0;
	int i, j, blks, blk_skip = 0;
	int res;
	u32 sect_sz, blk_use_sz;

	/* prn_string("bblk_read blk="); prn_dword0(blk_off);
	 * prn_string(" len="); prn_decimal(read_length); prn_string("\n");
	 */

	sect_sz = GetNANDPageCount_1K60(g_bootinfo.sys_nand.u16PyldLen) * 1024;
	blk_use_sz = g_bootinfo.sys_nand.u16PageNoPerBlk * sect_sz;
	blks = (read_length + blk_use_sz - 1) / blk_use_sz;

	/* for each good block */
	for (i = 0; i < blks; i++) {
		if (blk_last_read) {
			*blk_last_read = blk_skip + blk_off + i;
		}

		pg_off = (blk_skip + blk_off + i) * g_bootinfo.sys_nand.u16PageNoPerBlk;

		/* for each page */
		for (j = 0; j < g_bootinfo.sys_nand.u16PageNoPerBlk; j++) {
#ifdef SKIP_BLOCK_WITH_BAD_BLOCK_MARK
			if (j == 0 || j == 1) {
				g_spareData[0] = 0xFF;
			}
#endif

			/* read sect */
#ifdef CONFIG_HAVE_SPI_NAND
			if (type == SPINAND_BOOT) {
				res = SPINANDReadNANDPage_1K60(0, pg_off + j, (u32 *)(dst + got), &length);
			}
#endif
#ifdef CONFIG_HAVE_PARA_NAND
			if (type == NAND_LARGE_BOOT) {
				res = ReadNANDPage_1K60(NAND_CS0, pg_off + j, (u32 *)(dst + got), &length);
			}
#endif

#ifdef SKIP_BLOCK_WITH_BAD_BLOCK_MARK
			/*
			 * Check bad block mark
			 *
			 * TODO: find bad block mark pos by id. Refer to nand_decode_bbm_options().
			 */
			if (j == 0 || j == 1) {
				if (g_spareData[0] != 0xFF) {
					prn_string("!! Skip bad block ");
					prn_dword(blk_skip + blk_off + i);
					if (j == 1) { /* Cancel page 0 data */
						got -= sect_sz;
					}

					/* prn_string("pg_off="); prn_decimal(pg_off); prn_string("\n");
					 * prn_dump_buffer(g_spareData, g_bootinfo.sys_nand.u16ReduntLen);
					 */

					if (++blk_skip > max_blk_skip) {
						prn_string("too many bad blocks!\n");
						return -1;
					}
					i--;
					break;
				}
			}
#endif

			/* good block but read page failed? */
			if (res) {
				dbg();
				prn_string("failed to read page #");
				prn_dword(pg_off + j);
				return -1;
			}

			got += length;
			if (got >= read_length) {
				/* dbg(); */
				return 0; /* ok */
			}
		}
	}

	dbg();
	return -1;
}

/* Search for image header */
static int bblk_find_image(int type, const char *name, u8 *dst, u32 blk_off,
			   u32 blk_cnt, u32 *found_blk)
{
	u32 i;
	int res;
	struct image_header *hdr = (struct image_header *)dst;

	/* Block (blk_off + i) has image? */
	for (i = 0; i < blk_cnt; i++) {

		/* prn_string("bblk_find_image blk="); prn_decimal(blk_off + i); prn_string("\n"); */

		res = bblk_read(type, dst, blk_off + i, 64, 50, NULL);
		if (res < 0)
			continue;

		/* magic */
		if (!image_check_magic(hdr)) {
			/* prn_string("bad mgaic\n"); */
			continue;
		}

		/* check name */
		if (memcmp((const u8 *)image_get_name(hdr), (const u8 *)name, strlen(name)) != 0) {
			prn_string("bad name\n");
			continue;
		}

		/* header crc */
		if (!image_check_hcrc(hdr)) {
			prn_string("bad hcrc\n");
			continue;
		}

		*found_blk = blk_off + i;
		prn_string("found hdr at blk=");
		prn_dword(*found_blk);
		return 0;
	}

	return -1;
}

/**
 * nand_load_uhdr_image
 * img_name       - image name in uhdr
 * dst            - destination address
 * blk_off        - nand block offset number
 * search_blk_cnt - max block count to search
 * img_blk_end    - returned the last nand block number of the image
 * only_load_hdr  - only load header
 *
 * Return image data size (> 0) if ok (exclude header)
 */
static int nand_load_uhdr_image(int type, const char *img_name, void *dst,
		u32 blk_off, u32 search_blk_cnt, u32 *img_blk_end, int only_load_hdr)
{
	struct image_header *hdr = (struct image_header *)dst;
	u32 len;
	u32 real_blk_off = 0;
	int res;

	prn_string("nand load "); prn_string(img_name);
	prn_string("@blk="); prn_dword(blk_off);

	/* find image header */
	res = bblk_find_image(type, img_name, (u8 *)hdr, blk_off, search_blk_cnt, &real_blk_off);
	if (res) {
		prn_string("image hdr not found\n");
		return -1;
	}

	/* uhdr_dump(hdr); */

	len = image_get_size(hdr);

	if (only_load_hdr)
		return len;

	/* load image data */
	prn_string("load data size=");
	prn_decimal(len);
	prn_string("\n");
#ifdef CONFIG_SECURE_BOOT_SIGN
	res = bblk_read(type, (u8 *)hdr, real_blk_off, 64 + SIGN_DATA_SIZE + len, 100, img_blk_end);
#else
	res = bblk_read(type, (u8 *)hdr, real_blk_off, 64 + len, 100, img_blk_end);
#endif
	if (res) {
		prn_string("failed to load data\n");
		return -1;
	}

	/* verify image data */
	prn_string("verify img...");
	if (!image_check_dcrc(hdr)) {
		prn_string("corrupted img\n");
		return -1;
	}
	prn_string("ok\n");

	return len; /* ok */
}

static void nand_uboot(u32 type)
{
	u32 blk_start = g_bootinfo.app_blk_start;
	u32 blk_end = 0;
	struct image_header *hdr = (struct image_header *)UBOOT_LOAD_ADDR;
	struct image_header hdr1;
	int len;
	u32 sect_sz = GetNANDPageCount_1K60(g_bootinfo.sys_nand.u16PyldLen) * 1024;
	u32 blk_use_sz = g_bootinfo.sys_nand.u16PageNoPerBlk * sect_sz;

#ifdef CONFIG_STANDALONE_DRAMINIT
	if (ReadBootBlockDraminit((type == SPINAND_BOOT), (u8 *)DRAMINIT_LOAD_ADDR) < 0) {
		prn_string("Failed to load nand draminit\n");
		return;
	}
#endif

	if (run_draminit()) {
		return;
	}

#ifndef HAVE_UBOOT2_IN_NAND
	/* Load uboot1 from NAND */
	len = nand_load_uhdr_image(type, "uboot", (void *)UBOOT_LOAD_ADDR,
			blk_start, 10, &blk_end, 0);
#else
	/* uboot1 - facotry default uboot image
	 * uboot2 - updated uboot image
	 *
	 * Logic:
	 * 1) Load uboot1 header to guess uboot2 start block
	 * 2) Load uboot2
	 * 3) If uboot2 is not good, load uboot1
	 */

	memset((u8 *)&hdr1, 0, sizeof(hdr1));

	/* 1) uboot1 hdr */
	prn_string("Load uboot1 hdr\n");
	len = nand_load_uhdr_image(type, "uboot", (void *)hdr, blk_start, 10,
				&blk_end, 1);
	if (len <= 0) {
		prn_string("warn: not found uboot1\n");
		++blk_start; /* search for uboot2 since next block */
	} else {
		memcpy((u8 *)&hdr1, (const u8 *)hdr, sizeof(struct image_header));

		/* uboot2 follows uboot1
		 * uboot2 start blk = (uboot1 start blk) + (num of blocks of uboot1) */
		blk_start += (sizeof(struct image_header) + len + blk_use_sz - 1) / blk_use_sz;
	}
	/* 2) uboot2 (hdr + data) */
	prn_string("Load uboot2\n");
	len = nand_load_uhdr_image(type, "uboot", (void *)hdr, blk_start, 10, &blk_end, 0);
	if (len > 0) {
		prn_string("Use uboot2\n");
	} else if (image_get_size(&hdr1) > 0) {
		/* 3) uboot1 (fallback) hdr + data */
		prn_string("Fallback to uboot1\n");
		blk_start = g_bootinfo.app_blk_start;
		len = nand_load_uhdr_image(type, "uboot", (void *)hdr, blk_start,
				10, &blk_end, 0);
	}
#endif
	if (len <= 0) {
		prn_string("not found good uboot\n");
		return;
	}

	boot_uboot();
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
	int retry = 10;

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
	while (retry-- > 0) {
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

	prn_string("halt");
	halt();
}

static void init_uart(void)
{
#ifdef CONFIG_DEBUG_WITH_2ND_UART
#ifdef CONFIG_PLATFORM_Q628
	/* uart1 pinmux : x1,UA0_TX, X2,UA1_RX */
	MOON3_REG->sft_cfg[14] = RF_MASK_V((0x7f << 0), (1 << 0));
	MOON3_REG->sft_cfg[14] = RF_MASK_V((0x7f << 8), (2 << 8));
	MOON0_REG->reset[1] = RF_MASK_V_CLR(1 << 9); /* release UA1 */
	UART1_REG->div_l = UART_BAUD_DIV_L(BAUDRATE, UART_SRC_CLK);
	UART1_REG->div_h = UART_BAUD_DIV_H(BAUDRATE, UART_SRC_CLK);
#endif
#endif
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

	init_uart();

	prn_decimal_ln(AV1_GetStc32());

	/* first msg */
	prn_string("+++xBoot " __DATE__ " " __TIME__ "\n");
#if defined(CONFIG_PLATFORM_Q628) && (CONFIG_PLATFORM_IC_REV < 2)
	prn_string("NOTICE: this xboot works with ROM_CODE v1.0\n");
#endif

	/* init hw */
	init_hw();

#ifdef MON
	mon_shell();
#endif

#ifdef CONFIG_HAVE_OTP
	mon_rw_otp();
#endif

	/* start boot flow */
	boot_flow();

	/*
	 * build-time bug checker
	 */
	BUILD_BUG_ON(sizeof(union storage_buf) > STORAGE_BUF_SIZE);
}
