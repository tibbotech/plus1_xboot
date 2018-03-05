#include <auto_config.h>

/**********************
 * Development Options
 *********************/

#if 0 // configure via "make config"
//////////////////////////////////////////////////
// Configurations for different iboot build
// * ASIC iBoot:  default
// * CSIM iBoot:  CSIM_NEW
// * Zebu iBoot:  CSIM_NEW + ZEBU_ZMEM_XBOOT_ADDR
// * NOR_iBoot:   PLATFORM_SPIBAREMETAL
// * 8388_iBoot:  PLATFORM_SPIBAREMETAL & PLATFORM_8388
//////////////////////////////////////////////////
#endif

/* SPI Baremetal boot */
#ifdef CONFIG_XIP_SPIBAREMETAL
#define PLATFORM_SPIBAREMETAL	        /* Build for EXT_BOOT */
#endif

/* Emulation */
#ifdef CONFIG_PLATFORM_8388
#define PLATFORM_8388                   /* Build for 8388 */
#elif defined(CONFIG_PLATFORM_I137)
#define PLATFORM_I137                   /* Build for I137 */
#endif

/* CSIM build: Enable Stamp. No UART. Less delay. */
#ifdef CONFIG_BOOT_ON_CSIM
#define CSIM_NEW
#endif

#ifdef CSIM_NEW
#define ZEBU_SPEED_UP
// Enable zmem support (skip loading xboot if it's preloaded on Zebu DRAM)
//#define ZEBU_ZMEM_XBOOT_ADDR    0x1000
#endif

// ZEBU ?
#ifdef ZEBU_SPEED_UP
#define SPEED_UP_UART_BAUDRATE
#define SPEED_UP_SPI_NOR_CLK    /* speed up SPI_NOR flash (eg. Zebu) */
#endif


/***********************
 * xBoot
 ***********************/
#define XBOOT_BUILD           /* define in xboot build */

/**********************
 * Register
 *********************/
#define REG_BASE           0x9c000000
#define RF_GRP(_grp, _reg) ((((_grp) * 32 + (_reg)) * 4) + REG_BASE)

#define AHB0_REG_BASE      0x9c100000
#define AHB_GRP(_ahb_grp, _grp, _reg) \
	((((_grp) * 32 + (_reg)) * 4) + ((_ahb_grp) * 0x1000) + AHB0_REG_BASE)

#define RF_MASK_V(_mask, _val)       (((_mask) << 16) | (_val))
#define RF_MASK_V_SET(_mask)         (((_mask) << 16) | (_mask))
#define RF_MASK_V_CLR(_mask)         (((_mask) << 16) | 0)

/**********************
 * Debug STAMP
 **********************/

#ifdef CSIM_NEW            // CSIM Stamp
#define STAMP(value)       REGS0(RF_GRP(0, 0), value);
#define CSTAMP(value)      { *(volatile unsigned int *)RF_GRP(0, 0) = (unsigned int)(value); }
#else                      // ASIC: No stamp
#define STAMP(value)       // empty
#define CSTAMP(value)      // empty
#endif

/**********************
 * Boot Mode
 *********************/

/* IV_MX[6:2] */
#if defined(PLATFORM_8388) || defined(PLATFORM_I137)
#define HW_CFG_REG              RF_GRP(0, 28)
#define HW_CFG_SHIFT            10
#else
#define HW_CFG_REG              RF_GRP(0, 30)
#define HW_CFG_SHIFT            7
#endif
#define HW_CFG_MASK             (0x1D << HW_CFG_SHIFT)

#define AUTO_SCAN               0x01
#define AUTO_SCAN_ACHIP         0x15
#define SPI_NOR_BOOT            0x11
#define SPINAND_BOOT            0x09
#define EMMC_BOOT               0x1D
#define SDCARD_ISP              0x05
#define UART_ISP                0x0D
#define USB_ISP                 0x19
#define NAND_LARGE_BOOT         0xff // Q628: no PARA_NAND

// New EXT_BOOT :
//  bit=0: CPU boots from iBoot
//  bit=1: CPU boots from NOR_iBoot (no romcode)
#define HW_CFG_EXT_BOOT        (1 << (HW_CFG_SHIFT + 1)) /* IV_MX3 */

/**********************
 * Clock
 *********************/
#define XTAL_CLK               (27 * 1000 * 1000)

#ifndef PLATFORM_8388
#define PLLSYS                 (202500 * 1000)      /* 202.5MHz */
#endif

/**********************
 * Timer
 *********************/
#define TIMER_KHZ           90

/**********************
 * ROM
 *********************/
#define BOOT_ROM_BASE       0xffff0000
#define PROTECT_STA_ADDR    0xd000 /* ROM private section */
#define PROTECT_END_ADDR    0xffff /* ROM end */

/**********************
 * SPI
 *********************/
#define SPI_FLASH_BASE      0x98000000
#define SPI_IBOOT_OFFSET    ( 0 * 1024)
#define SPI_XBOOT_OFFSET    (64 * 1024) 

#define MAGIC_NUM_SPI_BAREMETAL 0x6D622B52

/**********************
 * SRAM 
 *********************/
#define SRAM0_BASE          0x9e800000
#define SRAM0_SIZE          (32 * 1024)
#define SRAM0_END           (SRAM0_BASE + SRAM0_SIZE)

/* Physically AB_SRAM0_BASE == SRAM0_BASE */
#define AB_SRAM0_BASE       0x9e800000
#define AB_SRAM0_END        (AB_SRAM0_BASE + SRAM0_SIZE)

/* RAM region : must match with boot.ldi */
#define XBOOT_BUF_SIZE      (20 * 1024)
#define STORAGE_BUF_SIZE    (9 * 1024)
#define BOOTINFO_SIZE       (512)
#define GLOBAL_HEADER_SIZE  (512)
#define CDATA_SIZE          (256)
#define STACK_SIZE          (2 * 1024 - 320)

/**********************
 * CPU boot address
 *********************/
#define CPU_WAIT_INIT_VAL   0xffffffff
#define CPU_B_START_POS     (SRAM0_END - 0x8)       // 0x9e807ff8
#define CPU_A_START_POS     (SRAM0_END - 0xc)       // 0x9e807ff4

/**********************
 * UART
 *********************/
#ifdef PLATFORM_8388
#define UART_SRC_CLK        (270 * 1000 * 1000)  /* 8388 SYSSLOW */
#else
#define UART_SRC_CLK        (XTAL_CLK)
#endif

/*
 * X = ((sclk + baud/2) / baud)
 * DIV_H = X  >> 12
 * DIV_L = (X & 0xf) << 12 | ((X >> 4) & 0xff - 1)
 */
#define UART_BAUD_DIV_H(baud, sclk)     ((((sclk) + ((baud) / 2)) / (baud)) >> 12)
#define UART_BAUD_DIV_L(baud, sclk)     ((((((sclk) + ((baud) / 2)) / (baud)) & 0xf) << 12) | \
					 ((((((sclk) + ((baud) / 2)) / (baud)) >> 4) & 0xff) - 1))
#ifdef SPEED_UP_UART_BAUDRATE
#define BAUDRATE            921600
#else
#define BAUDRATE            115200
#endif

#ifdef CSIM_NEW
#define UART_BT_TIMEOUT    (TIMER_KHZ)      /* 1ms */
#else
#define UART_BT_TIMEOUT    (TIMER_KHZ * 10) /* 10ms */
#endif

#define HAVE_PRINTF

/***********************
 * NAND
***********************/

// Xboot skips nand init:
// Partial nand init only reads nand id
// (to makesure nand io ok after clock change)
#ifdef XBOOT_BUILD
#define PARTIAL_NAND_INIT
#endif

/***********************
* USB
***********************/
#define FAT_USB_4K_READ

/***********************
* SD CARD
***********************/
/* enable in iboot, disable in xboot */
//#define SD_VERBOSE

/* Card controller source clock */
#ifdef PLATFORM_8388
#define CARD_CLK           (135*1000*1000)  /* SD0 */
#define CARD012_CLK        (135*1000*1000)
#else
//FIXME: q628 SD0 SD1 source clock
#define CARD_CLK           (PLLSYS)
#define CARD012_CLK        (PLLSYS)
#endif

/***********************
* eMMC
***********************/
#define EMMC_SLOT_NUM   0
//#define EMMC_USE_DMA_READ	/* can't DMA to SRAM */

/***********************
* OTP
***********************/
#ifdef PLATFORM_8388
#define OTP_WHO_BOOT_REG	0x9e80fffc	/* Fake &OTP[WHO_BOOT] */
#define OTP_WHO_BOOT_BIT	2
#else
//FIXME: q628 OTP[WHO_BOOT]
#define OTP_WHO_BOOT_REG	RF_GRP(350, 0)	/* &OTP[WHO_BOOT] */
#define OTP_WHO_BOOT_BIT	2
#endif
