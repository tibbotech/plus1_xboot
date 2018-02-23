#ifndef _INC_SWAB_H_
#define _INC_SWAB_H_


#define ___swab16(x) \
        ((u16)( \
                (((u16)(x) & (u16)0x00ffU) << 8) | \
                (((u16)(x) & (u16)0xff00U) >> 8) ))
#define ___swab32(x) \
        ((u32)( \
                (((u32)(x) & (u32)0x000000ffUL) << 24) | \
                (((u32)(x) & (u32)0x0000ff00UL) <<  8) | \
                (((u32)(x) & (u32)0x00ff0000UL) >>  8) | \
                (((u32)(x) & (u32)0xff000000UL) >> 24) ))

//
// Suppose CPU is little-endian
//

#define cpu_to_le32(_x) (_x) // do nothing
#define le32_to_cpu(_x) (_x) // do nothing

#define cpu_to_be32(_x) ___swab32(_x)
#define be32_to_cpu(_x) ___swab32(_x) 

#endif
