#include <auto_config.h>

/**********************
 * Development Options
 *********************/

#if 0 // configure via "make config"
//////////////////////////////////////////////////
// Configurations for different iboot build
// * ASIC iBoot:  default
// * CSIM iBoot:  CSIM_NEW
// * Zebu iBoot:  CSIM_NEW + CONFIG_BOOT_ON_ZEBU
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
#elif defined(CONFIG_PLATFORM_3502)
#define PLATFORM_3502                   /* Build for 3502 */
#endif

/* CSIM build: Enable Stamp. No UART. Less delay. */
#ifdef CONFIG_BOOT_ON_CSIM
#define CSIM_NEW
#endif

/* Zebu build: speed up options */
#ifdef CONFIG_BOOT_ON_ZEBU
#define SPEED_UP_UART_BAUDRATE
#define SPEED_UP_SPI_NOR_CLK    /* speed up SPI_NOR flash (eg. Zebu) */
#endif

/* zmem support */
#ifdef CONFIG_USE_ZMEM
#define ZMEM_XBOOT_ADDR    0x1000
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

#define A_REG_BASE              0x9ec00000
#define A_RF_GRP(_grp, _reg)    ((((_grp) * 32 + (_reg)) * 4) + A_REG_BASE)

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
#define HW_CFG_MASK             (0x1D << HW_CFG_SHIFT)
#elif defined(PLATFORM_3502)
#define HW_CFG_REG              RF_GRP(0, 28)
#define HW_CFG_SHIFT            14
#define HW_CFG_MASK             (0x1D << HW_CFG_SHIFT)
#else
#define HW_CFG_REG              RF_GRP(0, 31)
#define HW_CFG_SHIFT            12
#define HW_CFG_MASK             (0x1F << HW_CFG_SHIFT)
#endif

#define AUTO_SCAN               0x01
#define AUTO_SCAN_ACHIP         0x15
#define SPI_NOR_BOOT            0x11
#define SPINAND_BOOT            0x09
#define EMMC_BOOT               0x1F
#define EXT_BOOT                0x19
#define SDCARD_ISP              0x07
#define UART_ISP                0x0F
#define USB_ISP                 0x17
#define NAND_LARGE_BOOT         0xff // Q628: no PARA_NAND

/**********************
 * Clock
 *********************/
#define XTAL_CLK               (27 * 1000 * 1000)

#ifndef PLATFORM_8388
#define PLLSYS                 (202500 * 1000)      /* 202.5MHz */
#endif

#if defined(PLATFORM_I137) || defined(CONFIG_PLATFORM_Q628)
#define CLK_A_PLLCLK            2000000000      /* 2GHz */
#define CLK_B_PLLSYS            202500000       /* 202.5MHz */
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
#ifdef PLATFORM_I137
#define SRAM0_SIZE          (32 * 1024)
#else
#define SRAM0_SIZE          (40 * 1024)
#endif

#define SRAM0_BASE          0x9e800000
#define SRAM0_END           (SRAM0_BASE + SRAM0_SIZE)

#ifdef PLATFORM_I137
#define B_SRAM_BASE_A_VIEW  0x9e000000
#define A_WORK_MEM_BASE     0x9e800000
#else
#define B_SRAM_BASE_A_VIEW  0x9e800000
#define A_WORK_MEM_BASE     0x9ea00000
#endif

#define A_WORK_MEM_SIZE     (512 * 1024)

/* RAM region : must match with boot.ldi */
#ifdef PLATFORM_I137
#define XBOOT_BUF_SIZE      (20 * 1024)
#else
#define XBOOT_BUF_SIZE      (28 * 1024)
#endif
#define STORAGE_BUF_SIZE    (9 * 1024)
#define BOOTINFO_SIZE       (512)
#define GLOBAL_HEADER_SIZE  (512)
#define CDATA_SIZE          (512)
#define STACK_SIZE          (1472) /* 1.5K - 64 */

/**********************
 * CPU boot address
 *********************/
#define CPU_WAIT_INIT_VAL        0xffffffff
#define B_START_POS              (SRAM0_END - 0x8)       // 9e809ff8
#define BOOT_ANOTHER_POS         (SRAM0_END - 0x4)       // 9e809ffc

#ifdef PLATFORM_I137
#define A_START_POS_B_VIEW       (SRAM0_END - 0xc)       // 9e809ff4
#define SRAM0_BASE_A_VIEW        0x9e000000
#define A_START_POS_A_VIEW       (SRAM0_BASE_A_VIEW + SRAM0_SIZE - 0xc) // 9e007ff4
#define BOOT_ANOTHER_POS_A_VIEW  (SRAM0_BASE_A_VIEW + SRAM0_SIZE - 0x4) // 9e007ffc
#else
#define A_START_POS_B_VIEW        (SRAM0_END - 0xc)       // 9e809ff4
#define A_START_POS_A_VIEW        A_START_POS_B_VIEW
#define BOOT_ANOTHER_POS_A_VIEW   BOOT_ANOTHER_POS
#endif


/**********************
 * UART
 *********************/
#ifdef PLATFORM_8388
#define UART_SRC_CLK        (270 * 1000 * 1000)  /* 8388 SYSSLOW */
#elif defined(PLATFORM_3502)
#define UART_SRC_CLK        (250 * 1000 * 1000)  /* 3502 SYSCLK */
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

// If undefined, always use first uboot (uboot1)
// If defined, use 2nd uboot (uboot2) if ok ; fallback to uboot1 otherwise
#define HAVE_UBOOT2_IN_NAND

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
/* Q628 SD0 SD1 source clock */
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
/* FIXME: Q628 OTP[WHO_BOOT] = G350.0 bit11 */
//#define OTP_WHO_BOOT_REG	RF_GRP(350, 0)
//#define OTP_WHO_BOOT_BIT	11
#define OTP_WHO_BOOT_REG	RF_GRP(4, 31) // temp OTP
#define OTP_WHO_BOOT_BIT	0
#endif
