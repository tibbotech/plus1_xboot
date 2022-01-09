#ifndef __ROMVSR_INC_H__
#define __ROMVSR_INC_H__

#include <config.h>

#if defined(PLATFORM_Q645)
#define SC_shaX_512        (*(rom_shaX_512_t)(p_romvsr->_rom_shaX_512))
#define SC_ed25519_hash    (*(rom_ed25519_hash_t)(p_romvsr->_rom_ed25519_hash))
#define SC_ed25519_verify_hash (*(rom_ed25519_v_h_t)(p_romvsr->_rom_ed25519_verify_hash))
#define SC_x25519          (*(rom_x25519_t)(p_romvsr->_rom_x25519))
#define SC_key_otp_load		(*(rom_otp_load)(p_romvsr->_rom_otp_load))
#define SC_hkdf_shaX_512	(*(rom_hkdf_shaX_512)(p_romvsr->_rom_hkdf_shaX_512))
#define SC_aes_gcm_ad		(*(rom_aes_gcm_ad)(p_romvsr->_rom_aes_gcm_ad))

#define hal_dcache_sync           (*(rom_dcache_sync_t)(p_romvsr->_rom_dcache_sync))
#define hal_dcache_invalidate_all (*(rom_dcache_invalidate_all_t)(p_romvsr->_rom_dcache_invalidate_all))
#define hal_dcache_flush          (*(rom_dcache_flush_t)(p_romvsr->_rom_dcache_flush))
#define hal_dcache_store          (*(rom_dcache_store_t)(p_romvsr->_rom_dcache_store))
#define hal_dcache_invalidate     (*(rom_dcache_invalidate_t)(p_romvsr->_rom_dcache_invalidate))

typedef void *(*rom_memcpy_t)(void *dst, const void *s2, int n);
typedef void *(*rom_memset_t)(void *dst, int c, int n);
typedef void (*rom_printf_t)(const char *fmt, ...);
typedef void (*rom_shell_t)(void);

typedef int (*rom_hkdf_shaX_512)(const unsigned char *salt, int salt_len, const unsigned char *ikm, int ikm_len,const unsigned char *info, int info_len, uint8_t okm[], int okm_len);
typedef int (*rom_aes_gcm_ad)(const u8 *key, size_t key_len, const u8 *iv, size_t iv_len,const u8 *crypt, size_t crypt_len,const u8 *aad, size_t aad_len, const u8 *tag, u8 *plain);
typedef int (*rom_otp_load)(unsigned char *buf, int otp_byte_off, int bytes);
typedef int (*rom_shaX_512_t)(const void *message, size_t message_len, unsigned char h_val[64]);
typedef void (*rom_ed25519_hash_t)(const unsigned char *signature, const void *message, size_t message_len, const unsigned char *public_key, unsigned char h_val[64]);
typedef int (*rom_ed25519_v_h_t)(const unsigned char *signature, const unsigned char *public_key, const unsigned char h_val[64]);
typedef void (*rom_x25519_t)(unsigned char *shared_secret, const unsigned char *public_key, const unsigned char *private_key);

typedef void (*rom_dcache_enable_t)(void);
typedef void (*rom_dcache_disable_t)(void);
typedef int (*rom_dcache_is_enabled_t)(void);
typedef void (*rom_dcache_sync_t)(void);
typedef void (*rom_dcache_invalidate_all_t)(void);
typedef void (*rom_dcache_flush_t)(unsigned long addr, size_t size);
typedef void (*rom_dcache_store_t)(unsigned long addr, size_t size);
typedef void (*rom_dcache_invalidate_t)(unsigned long addr, size_t size);

struct romvsr {
	void *memcpy;
	void *memset;
	void *printf;
	void *shell;

	void *_rom_shaX_512;
	void *_rom_ed25519_hash;
	void *_rom_ed25519_verify_hash;
	void *_rom_x25519;
	void *_rom_otp_load;
	void *_rom_hkdf_shaX_512;
	void *_rom_aes_gcm_ad;

	void *_rom_dcache_enable;
	void *_rom_dcache_disable;
	void *_rom_dcache_is_enabled;
	void *_rom_dcache_sync;
	void *_rom_dcache_invalidate_all;
	void *_rom_dcache_flush;
	void *_rom_dcache_store;
	void *_rom_dcache_invalidate;
};

#elif defined(PLATFORM_SP7350)
#define SC_shaX_512        (*(rom_shaX_512_t)(p_romvsr->_rom_shaX_512))
#define SC_ed25519_hash    (*(rom_ed25519_hash_t)(p_romvsr->_rom_ed25519_hash))
#define SC_ed25519_verify_hash (*(rom_ed25519_v_h_t)(p_romvsr->_rom_ed25519_verify_hash))
#define SC_x25519          (*(rom_x25519_t)(p_romvsr->_rom_x25519))
#define SC_key_otp_load		(*(rom_otp_load)(p_romvsr->_rom_otp_load))
#define SC_hkdf_shaX_512	(*(rom_hkdf_shaX_512)(p_romvsr->_rom_hkdf_shaX_512))
#define SC_aes_gcm_ad		(*(rom_aes_gcm_ad)(p_romvsr->_rom_aes_gcm_ad))

#define hal_dcache_sync           (*(rom_dcache_sync_t)(p_romvsr->_rom_dcache_sync))
#define hal_dcache_invalidate_all (*(rom_dcache_invalidate_all_t)(p_romvsr->_rom_dcache_invalidate_all))
#define hal_dcache_flush          (*(rom_dcache_flush_t)(p_romvsr->_rom_dcache_flush))
#define hal_dcache_store          (*(rom_dcache_store_t)(p_romvsr->_rom_dcache_store))
#define hal_dcache_invalidate     (*(rom_dcache_invalidate_t)(p_romvsr->_rom_dcache_invalidate))

typedef void *(*rom_memcpy_t)(void *dst, const void *s2, int n);
typedef void *(*rom_memset_t)(void *dst, int c, int n);
typedef void (*rom_printf_t)(const char *fmt, ...);
typedef void (*rom_shell_t)(void);

typedef int (*rom_hkdf_shaX_512)(const unsigned char *salt, int salt_len, const unsigned char *ikm, int ikm_len,const unsigned char *info, int info_len, uint8_t okm[], int okm_len);
typedef int (*rom_aes_gcm_ad)(const u8 *key, size_t key_len, const u8 *iv, size_t iv_len,const u8 *crypt, size_t crypt_len,const u8 *aad, size_t aad_len, const u8 *tag, u8 *plain);
typedef int (*rom_otp_load)(unsigned char *buf, int otp_byte_off, int bytes);
typedef int (*rom_shaX_512_t)(const void *message, size_t message_len, unsigned char h_val[64]);
typedef void (*rom_ed25519_hash_t)(const unsigned char *signature, const void *message, size_t message_len, const unsigned char *public_key, unsigned char h_val[64]);
typedef int (*rom_ed25519_v_h_t)(const unsigned char *signature, const unsigned char *public_key, const unsigned char h_val[64]);
typedef void (*rom_x25519_t)(unsigned char *shared_secret, const unsigned char *public_key, const unsigned char *private_key);

typedef void (*rom_dcache_enable_t)(void);
typedef void (*rom_dcache_disable_t)(void);
typedef int (*rom_dcache_is_enabled_t)(void);
typedef void (*rom_dcache_sync_t)(void);
typedef void (*rom_dcache_invalidate_all_t)(void);
typedef void (*rom_dcache_flush_t)(unsigned long addr, size_t size);
typedef void (*rom_dcache_store_t)(unsigned long addr, size_t size);
typedef void (*rom_dcache_invalidate_t)(unsigned long addr, size_t size);

struct romvsr {
	void *memcpy;
	void *memset;
	void *printf;
	void *shell;

	void *_rom_shaX_512;
	void *_rom_ed25519_hash;
	void *_rom_ed25519_verify_hash;
	void *_rom_x25519;
	void *_rom_otp_load;
	void *_rom_hkdf_shaX_512;
	void *_rom_aes_gcm_ad;

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
#endif

#ifndef XBOOT_BUILD
extern const struct romvsr * const p_romvsr;
#else
extern void *__rom_vsr_base;
#define p_romvsr ((const struct romvsr * const)(&__rom_vsr_base))
#endif

#endif
