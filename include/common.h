#ifndef _COMMON_INC_
#define _COMMON_INC_

#include <config.h>
#ifdef XBOOT_BUILD
#include <config_xboot.h>
#endif
#include <types.h>
#include <romvsr.h>
#include <stc.h>
#include <fat/fat.h>
#ifdef CONFIG_HAVE_USB_DISK
#include <usb/ehci_usb.h>
#endif
#ifdef CONFIG_HAVE_SNPS_USB3_DISK
#include <usb/xhci_usb.h>
#endif
#include <nand_boot/nfdriver.h>
#include <nand_boot/nfdriver.h>
#ifdef CONFIG_PLATFORM_Q645
#include <spinand_boot/sp_spinand_q645.h>
#else
#include <spinand_boot/sp_spinand.h>
#endif
#include <sdmmc_boot/hal_sd_mmc.h>
#include <romvsr.h>

/*
 * timer
 */
void delay_1ms(UINT32 period);

/*
 * uart print
 */
#define dbg_info()	_dbg_info(__FILE__, __LINE__)
#define dbg()		_dbg_info(__FILE__, __LINE__)
void _dbg_info(char *, u32);
void prn_string(const char *);
void prn_byte(unsigned char);
void prn_dword(unsigned int);
void prn_dword0(unsigned int);
void prn_decimal(unsigned int);
void prn_decimal_ln(unsigned int);
void prn_dump_buffer(unsigned char *buf, int len);
void uart0_putc(unsigned char c);

void *memcpy16(u16 *s1, const u16 *s2, int n);
void *memcpy32(u32 *s1, const u32 *s2, int n);
void *memcpy128(u32 *s1, const u32 *s2, int n);
int   memcmp(const u8 *s1, const u8 *s2, int n);
void *memset32(u32 *s1, u32 val, int n);

void exit_bootROM(u32 addr);
void do_boot_flow(u32 mode);
void boot_reset(void);

// Due to SRAM layout changed in IC revisions,
// old xboot can't use romshare functions which use SRAM.

#if !defined(XBOOT_BUILD) || defined(CONFIG_DEBUG_WITH_2ND_UART)
void mon_shell(void);
void diag_printf(const char *fmt, ...);
#else
#define mon_shell      rom_shell
#define diag_printf    rom_printf
#endif

#if !defined(XBOOT_BUILD) || !defined(CONFIG_HAVE_ARCH_FASTMEM)
void *memcpy(u8 *s1, const u8 *s2, int n);
void *memset(u8 *s1, int c, int n);
#else
#define memcpy         rom_memcpy
#define memset         rom_memset
#endif

#ifdef CONFIG_SECURE_BOOT_SIGN
void fill_mmu_page_table(void);
void enable_mmu(void);
void disable_mmu(void);
#endif
/* rom share : APIs for external callers */
#define rom_printf     (*(rom_printf_t)(p_romvsr->printf))
#define rom_shell      (*(rom_shell_t)(p_romvsr->shell))
#define rom_memcpy     (*(rom_memcpy_t)(p_romvsr->memcpy))
#define rom_memset     (*(rom_memset_t)(p_romvsr->memset))

/*
 * build-time checker
 */
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

enum Device_table {
	DEVICE_PARA_NAND = 0,
	DEVICE_USB_ISP,
	DEVICE_EMMC,
	DEVICE_SDCARD,
	DEVICE_UART_ISP,
	DEVICE_SPI_NOR,
	DEVICE_SPI_NAND,
	DEVICE_MAX
};

/*
 * storage_buf usage depends on storage driver
 */
union storage_buf {
	/* nand */
	struct nand_io {
		u8 data[8 * 1024];
		u8 spare[512];
#define NAND_MAX_DESC_SIZE 4
		desc_command_t descInfo[NAND_MAX_DESC_SIZE];    /* 32*4 bytes */
	} nand;

	/* usb */
	struct usb_io {
		u8           cmd_buf[1024];         /* for USB_vendorCmd(), usb descriptors */	
#ifdef CONFIG_HAVE_SNPS_USB3_DISK
		xhci_usb     xhci;
#endif
		//u8           draminit_tmp[7 * 1024];
		u8           sect_buf[512];         /* for FAT data read,  sector-size buffer */
		
#ifdef CONFIG_HAVE_USB_DISK
		ehci_usb     ehci;                  /* for all usb transfer, 320 bytes */
#endif

	} usb;

	/* uart bootstrap */
	struct bstrap_io {
		volatile u32  *UART_data;
		volatile u32  *UART_lsr;
		u32           timer_enable;
		u32           length;
		u32           flag;
		u8            checkSum;
		u8            count;
		u8            cmd;
	} bstrap;

	// generic
	//u8 g_storage_buf[STORAGE_BUF_SIZE];
};

#define SB_FLAG_ENABLE    1

#ifdef PLATFORM_Q645
#define FLAG_SECURE_ENABLE       (1 << 0)
#define FLAG_HSM_DISABLE         (1 << 8)
#define IS_IC_SECURE_ENABLE()    (g_bootinfo.hw_security & FLAG_SECURE_ENABLE)
#define IS_IC_HSM_DISABLE()      (g_bootinfo.hw_security & FLAG_HSM_DISABLE)

#define IBOOT_FLAG_SILENT        (1 << 0)
#define IS_IC_SILENT()           (g_bootinfo.iboot_flags & IBOOT_FLAG_SILENT)
#endif
struct bootinfo {
	u32     bootrom_ver;         // iboot version
	u32     hw_bootmode;         // hw boot mode (latched: auto, nand, usb_isp, sd_isp, etc)
	u32     gbootRom_boot_mode;  // sw boot mode (category: nand, sd, usb)
	u32     bootdev;             // boot device (exact device: sd0, sd1, ...)
	u32     bootdev_pinx;        // boot device pinmux
	u32     bootdev_port;        // usb0~1, sd0~1
	u32     app_blk_start;       // the block after xboot block(s)
	u32     mp_flag;             // mp machine flag
	u32     bootcpu;             // 0: B, 1: A
	u32     in_xboot;            // 0=in iboot, 1=in xboot
#ifdef PLATFORM_Q645	
	u32     hw_security;         // hw security
#endif	
	u32     sb_flag;             // secure boot flag, bit0=1(secure boot)

	/*
	 * ROM code puts all C code global variables here so that :
	 * 1. xboot can continue driver operations (eg. skip nand init)
	 * 2. ROM functions may be reused in xboot
	 */

	/* common/bootmain.c */
	u32     sd_cur_base;

	/* nand/nandop.c */
	struct SysInfo sys_nand;
	UINT8 gInitNand_flag;
	UINT8 gheader_already_read_out_flag;
	UINT8 gRandomizeFlag;
	UINT8 reserved;
	SDev_t gsdev;

	/* nand/nfdriver.c */
	UINT32 g_int_wake_up_flag;
	UINT8 software_protect_byte[6];
	UINT8 gNANDIDLength;
	desc_command_t *nand_desc_comm;

	/* nand/spi_nand.c */
	struct sp_spinand_info our_spinfc;

	/* MMC */
	int gSD_HIGHSPEED_EN_SET_val[4];
	int gSD_RD_CLK_DELAY_TIME_val[4];
	struct STORAGE_DEVICE sd_Storage_dev;
	int mmc_active_part;

	/* fat */
	unsigned char fat_fileName[1][12];
};

/*
 * global variables
 */
extern union storage_buf   g_io_buf;
extern u8                  g_xboot_buf[];
extern struct bootinfo     g_bootinfo;
extern  u8                 g_boothead[GLOBAL_HEADER_SIZE];

#define is_in_bootROM()           (!g_bootinfo.in_xboot)

/* To ease driver code comparing, keep these names. */
#define g_pyldData                (g_io_buf.nand.data)
#define g_spareData               (g_io_buf.nand.spare)
#define g_pSysInfo                (&(g_bootinfo.sys_nand))
#define gDEV_SDCTRL_BASE_ADRS     (g_bootinfo.sd_cur_base)
#define gStorage_dev              (g_bootinfo.sd_Storage_dev)
#define p_desc_comm               (g_bootinfo.nand_desc_comm)

void boot_next_no_stack(void);

#endif /* _COMMON_INC_ */
