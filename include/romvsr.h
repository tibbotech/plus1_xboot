#ifndef __ROMVSR_INC_H__
#define __ROMVSR_INC_H__

#include <config.h>

typedef void (*rom_printf_t)(const char *fmt, ...);
typedef void (*rom_shell_t)(void);
typedef void *(*rom_memcpy_t)(void *_dst, const void *_src, size_t _count);
typedef void *(*rom_memset_t)(void *_dst, int _c, size_t count);

#ifdef PLATFORM_Q645
struct romvsr {
	void *memcpy;
	void *memset;
	void *printf;
	void *shell;

	void *_rom_shaX_512;
	void *_rom_ed25519_hash;
	void *_rom_ed25519_verify_hash;
	void *_rom_x25519;

	void *_rom_dcache_enable;
	void *_rom_dcache_disable;
	void *_rom_dcache_is_enabled;
	void *_rom_dcache_sync;
	void *_rom_dcache_invalidate_all;
	void *_rom_dcache_flush;
	void *_rom_dcache_store;
	void *_rom_dcache_invalidate;
};
#else
struct romvsr {
	void *printf;
	void *shell;
	void *memcpy;
	void *memset;
};
#endif

#ifndef XBOOT_BUILD
extern const struct romvsr * const p_romvsr;
#else
extern void *__rom_vsr_base;
#define p_romvsr ((const struct romvsr * const)(&__rom_vsr_base))
#endif

#endif
