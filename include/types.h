#ifndef __TYPE_H__
#define __TYPE_H__

//#include <stddef.h>
#include <stdint.h>

typedef unsigned long       size_t;

typedef unsigned long long  u64;
typedef unsigned int        u32;
typedef unsigned short      u16;
typedef unsigned char       u8;
typedef unsigned char       u08;

typedef long long           s64;
typedef int                 s32;
typedef short               s16;
typedef signed char         s8;

typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned long		DWORD;

#define UINT64			u64
#define UINT32			u32
#define UINT16			u16
#define UINT8			u8

#define INT64			s64
#define INT32			s32
#define INT16			s16
#define INT8			s8

#define SINT32			s32
#define SINT16			s16
#define SINT8			s8

#define  uint32_t		unsigned int
#define  uint8_t		unsigned char

#define PASS 			0
#define FAIL			2

#define ROM_SUCCESS		PASS
#define ROM_FAIL		FAIL

#define DEV_READ		2
#define DEV_WRITE		3

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL    ((void *)0)
#endif

#if __riscv_xlen == 64
#define ADDRESS_CONVERT(addr)	((u64)addr)
#else
#define ADDRESS_CONVERT(addr)	(addr)
#endif
#endif // __TYPES_H__
