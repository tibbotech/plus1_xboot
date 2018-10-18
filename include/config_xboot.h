#ifndef _INC_CONFIG_XBOOT_
#define _INC_CONFIG_XBOOT_

#include <auto_config.h>

////////////////////////////////////////////////////////////////////////////
//
// Config (xboot)
//

//
// Image Address (source)
//

// SPI NOR
#define SPI_DTB_OFFSET        0x020000   // 128K
#define SPI_UBOOT_OFFSET      0x040000   // 256K
#define SPI_INITRAMFS_OFFSET  0x400000   // 4M
#define SPI_LINUX_OFFSET      0x600000   // 6M

//
// Load & Run Address
//

// draminit
#define DRAMINIT_LOAD_ADDR   (g_io_buf.usb.draminit_tmp)  /* 9e804d00 */
#define DRAMINIT_RUN_ADDR    (DRAMINIT_LOAD_ADDR + 0x40)  /* skip header */

// dram_test
#define DRAM_TEST_BEGIN      0x800000
#define DRAM_TEST_LEN        1024
#define DRAM_TEST_END        (DRAM_TEST_BEGIN + DRAM_TEST_LEN)

// u-boot
#define UBOOT_LOAD_ADDR      0x200000
#define UBOOT_RUN_ADDR       0x200040

// DTB
#define DTB_LOAD_ADDR        (0x300000 - 0x40)          /* dtb */
#define DTB_RUN_ADDR         0x300000                   /* skip header */

// Linux
#define LINUX_RUN_ADDR       0x308000                   /* vmlinux */
#define LINUX_LOAD_ADDR      (LINUX_RUN_ADDR - 0x40)    /* - header */

// initramfs
#define INITRAMFS_RUN_ADDR   0x2100000                   /* cpio */
#define INITRAMFS_LOAD_ADDR  (INITRAMFS_RUN_ADDR - 0x40)

// need to load initramfs if it's split from uImage
//#define LOAD_SPLIT_INITRAMFS

/////////////////////////
// mkimage Type
// mkimage -T standalone --> uhdr with CRC32
// mkimage -T quickboot  --> uhdr with SUM32
#define USE_QKBOOT_IMG  // consistent with draminit and uboot image

/* ISP image offset */
#define ISP_IMG_OFF_XBOOT    (0)
#define ISP_IMG_OFF_UBOOT    (64 * 1024)

//
// ABIO config
//
#define ABIO_32M   0x200408
#define ABIO_100M  0x080718
#define ABIO_200M  0x040718
#define ABIO_400M  0x020718

#define ABIO_IOCTRL_CFG     0x00f1e005  /* asic A_G0.18 io delay (xhdu, POSTSIM_ON) */

#ifdef CONFIG_PLATFORM_I137
#define ABIO_CFG ABIO_200M
#else
#define ABIO_CFG ABIO_400M
//#define A_PLL_CTL0_CFG      0x2c5109  /* default: pllclk=1215M, corelck= 607.5M */
#define A_PLL_CTL0_CFG      0x445149    /* pllclk=1863M, corelck= 931.5M */
#endif

#endif
