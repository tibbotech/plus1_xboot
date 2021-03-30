#include <image.h>
#include <common.h>

#ifdef USE_QKBOOT_IMG
#define CHKSUM_FN   sum32
static uint32_t sum32(uint32_t sum, uint8_t *data, uint32_t len)
{
        uint32_t val = 0, pos =0;

#ifdef CONFIG_BOOT_ON_ZEBU
	prn_string("sum len=");
	prn_decimal_ln(len);
#endif

	for (; pos + 4 <= len; pos += 4) {
#ifdef CONFIG_BOOT_ON_ZEBU
		/* print '.' per 64K */
		if ((pos & 0xffff) == 0) {
			prn_string(".");
		}
#endif
		sum += *(uint32_t *)(data + pos);
	}
#ifdef CONFIG_BOOT_ON_ZEBU
	prn_string("\n");
#endif

        // word0: 3 2 1 0
        // word1: _ 6 5 4
        for (; len - pos; len--)
                val = (val << 8) | data[len - 1];

        sum += val;

        return sum;
}
#else
#include <crc.h>
#define CHKSUM_FN   crc32
#endif

int image_check_hcrc(const image_header_t *hdr)
{
        u32 hcrc;
        u32 len = image_get_header_size();
        image_header_t header;

        /* Copy header so we can blank CRC field for re-calculation */
        memcpy((u8 *)&header, (u8 *)hdr, image_get_header_size());
        header.ih_hcrc = 0;

        hcrc = CHKSUM_FN(0, (unsigned char *)&header, len);

        return (hcrc == image_get_hcrc(hdr));
}

int image_check_dcrc(const image_header_t *hdr)
{
        ulong len = image_get_size(hdr);
        ulong dcrc = CHKSUM_FN(0, (unsigned char *)image_get_data(hdr), len);

        return (dcrc == image_get_dcrc(hdr));
}

