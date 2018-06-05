#ifndef _BOOTMAIN_INC_
#define _BOOTMAIN_INC_

#define XBOOT_HDR_MAGIC   0x54554258      // XBUT (X=58h)

struct xboot_hdr {
        u32 magic;
        u32 version;
        u32 length;       // exclude header
        u32 checksum;     // exclude header
        u32 reserved[4];
};

void SetBootDev(unsigned int bootdev, unsigned int pinx, unsigned int dev_port);

int get_xboot_size(u8 *img);
int verify_xboot_img(u8 *img);

/* pinmux ops */
int get_spi_nor_pinmux(void);

#endif
