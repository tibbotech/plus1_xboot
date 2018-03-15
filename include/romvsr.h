#ifndef __ROMVSR_INC_H__
#define __ROMVSR_INC_H__

#include <config.h>

typedef void (*rom_printf_t)(const char *fmt, ...);
typedef void (*rom_shell_t)(void);
typedef void *(*rom_memcpy_t)(void *_dst, const void *_src, size_t _count);
typedef void *(*rom_memset_t)(void *_dst, int _c, size_t count);

struct romvsr {
	void *printf;
	void *shell;
	void *memcpy;
	void *memset;
};
#ifndef XBOOT_BUILD
extern const struct romvsr * const p_romvsr;
#else
extern void *__rom_vsr_base;
#define p_romvsr ((const struct romvsr * const)(&__rom_vsr_base))
#endif

#endif
