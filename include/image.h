#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <types.h>
#include <swab.h>

#define __be32   u32
#define ulong     unsigned long

/* mkimage format */

#define IH_MAGIC        0x27051956      /* Image Magic Number           */
#define IH_NMLEN                32      /* Image Name Length            */

typedef struct image_header {
        __be32          ih_magic;       /* Image Header Magic Number    */
        __be32          ih_hcrc;        /* Image Header CRC Checksum    */
        __be32          ih_time;        /* Image Creation Timestamp     */
        __be32          ih_size;        /* Image Data Size              */
        __be32          ih_load;        /* Data  Load  Address          */
        __be32          ih_ep;          /* Entry Point Address          */
        __be32          ih_dcrc;        /* Image Data CRC Checksum      */
        uint8_t         ih_os;          /* Operating System             */
        uint8_t         ih_arch;        /* CPU architecture             */
        uint8_t         ih_type;        /* Image Type                   */
        uint8_t         ih_comp;        /* Compression Type             */
        uint8_t         ih_name[IH_NMLEN];      /* Image Name           */
} image_header_t;

#define uimage_to_cpu(x)                be32_to_cpu(x)
#define cpu_to_uimage(x)                cpu_to_be32(x)

static inline uint32_t image_get_header_size(void)
{
        return (sizeof(image_header_t));
}

static inline char *image_get_name(const image_header_t *hdr)
{
        return (char *)hdr->ih_name;
}

static inline uint8_t image_get_arch(const image_header_t *hdr)
{
        return ((uint8_t)hdr->ih_arch);
}


static inline ulong image_get_data(const image_header_t *hdr)
{
        return ((ulong)hdr + image_get_header_size());
}


#define image_get_hdr_l(f) \
        static inline uint32_t image_get_##f(const image_header_t *hdr) \
        { \
                return uimage_to_cpu(hdr->ih_##f); \
        }
image_get_hdr_l(magic)          /* image_get_magic */
image_get_hdr_l(hcrc)           /* image_get_hcrc */
image_get_hdr_l(time)           /* image_get_time */
image_get_hdr_l(size)           /* image_get_size */
image_get_hdr_l(load)           /* image_get_load */
image_get_hdr_l(ep)             /* image_get_ep */
image_get_hdr_l(dcrc)           /* image_get_dcrc */


static inline int image_check_magic(const image_header_t *hdr)
{
        return (image_get_magic(hdr) == IH_MAGIC);
}


/* declaration */

int image_check_hcrc(const image_header_t *hdr);
int image_check_dcrc(const image_header_t *hdr);

#endif
