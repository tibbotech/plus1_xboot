#ifndef _INC_CONFIG_XBOOT_
#define _INC_CONFIG_XBOOT_

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
#define DTB_LOAD_ADDR        0x300000                   /* dtb */
#define DTB_RUN_ADDR         0x300040                   /* skip header */

// Linux
#define LINUX_RUN_ADDR       0x308000                   /* vmlinux */
#define LINUX_LOAD_ADDR      (LINUX_RUN_ADDR - 0x40)    /* - header */

/////////////////////////
// mkimage Type
// mkimage -T standalone --> uhdr with CRC32
// mkimage -T quickboot  --> uhdr with SUM32
#define USE_QKBOOT_IMG  // consistent with draminit and uboot image

/* ISP image offset */
#define ISP_IMG_OFF_XBOOT    (0)
#define ISP_IMG_OFF_UBOOT    (64 * 1024)

#endif
