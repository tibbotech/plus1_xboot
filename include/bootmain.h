#ifndef _BOOTMAIN_INC_
#define _BOOTMAIN_INC_

#define XBOOT_HDR_MAGIC   0x54554258      // XBUT (X=58h)

#define XB_SECURE_BOOT    0x00000001

struct xboot_hdr {
	u32 magic;
	u32 version;
	u32 length;       // exclude header
	u32 checksum;     // exclude header
	u32 img_flag;
	u32 reserved[3];
};

void set_spi_nor_pinmux(int pin_x);
void SetBootDev(unsigned int bootdev, unsigned int pinx, unsigned int dev_port);

int get_xboot_size(u8 *img);
int verify_xboot_img(u8 *img);

/* pinmux ops */
int get_spi_nor_pinmux(void);

#endif
