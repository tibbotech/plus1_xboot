/******************************************************************************
*                          Include File
*******************************************************************************/
#include <common.h>
#include <bootmain.h>
#include <otp/sp_otp.h>

#ifdef CONFIG_SECURE_BOOT_SIGN

// u-boot.img =|header+data+sbinf0|
#define SB_INFO_SIZE	200

#define __ALIGN4       __attribute__((aligned(4)))
#define __ALIGN8       __attribute__((aligned(8)))

void prn_dump(const char *title, const unsigned char *buf, int len)
{
	int i;

	if (title) {
		prn_string(title);
		prn_string(" "); prn_dword((u32)buf);
	}

	for (i = 0; i < len; i++) {
		if (i && !(i & 0xf)) {
			prn_string("\n");
		}
		prn_byte(buf[i]);
	}
	prn_string("\n");
}

#if defined(PLATFORM_SP7350)
//#include "verify_image_q654.c"
#include "verify_image_q654_hw.c"
#elif defined(PLATFORM_Q645)
#include "verify_image_q645.c"
#else
#include "verify_image_q628.c"
#endif

int xboot_verify_next_image(const struct image_header  *hdr)
{
	int ret;

#if defined(PLATFORM_SP7350)
	ret = q654_image_verify_decrypt(hdr);
#elif defined(PLATFORM_Q645)
	ret = q645_image_verify_decrypt(hdr);
#else
	ret = q628_verify_uboot_signature(hdr);
#endif
	if (ret) {
		prn_string(image_get_name(hdr));
		prn_string(" verify fail !!\nhalt!");
	}
	return ret;
}
#endif
