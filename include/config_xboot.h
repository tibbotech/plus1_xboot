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
#define SPI_UBOOT_OFFSET      (256 * 1024)

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
#define UBOOT_RUN_ADDR       0x200000   // @2MB
#define UBOOT_LOAD_ADDR      (UBOOT_RUN_ADDR - 0x40)

// fat
//#define FAT_DRAM_BUF_ADDR    0x800000   // @8MB


/////////////////////////
// mkimage Type
// mkimage -T standalone --> uhdr with CRC32
// mkimage -T quickboot  --> uhdr with SUM32
#define USE_QKBOOT_IMG  // consistent with draminit and uboot image

#endif
