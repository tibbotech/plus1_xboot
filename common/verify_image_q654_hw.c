#include "crypto_drv.h"

/***********************************
|---------------------------|
|        uImage header      |
|---------------------------|
|            data           |
|---------------------------|
|       sign (256byte)      |
|---------------------------|
|     rsakey_N (256byte)    |
|---------------------------|
***********************************/
//#define TRACE   prn_string(__FUNCTION__);prn_string(":");prn_decimal_ln(__LINE__);
#ifndef TRACE
#define TRACE
#endif

#ifndef ALGIN
#define __ALIGN_MASK(x,mask)	(((x)+(mask))&~(mask))
#define ALIGN(x,a)		__ALIGN_MASK((x),(typeof(x))(a)-1)
#endif

//#define QCTRL_TEST
#ifdef QCTRL_TEST

#define qctl_regs	0xF8000F00
#define MASK_SET(shift, width, value) \
({ \
	u32 m = ((1 << (width)) - 1) << (shift); \
	(m << 16) | (((value) << (shift)) & m); \
})

static void q_write(u32 n, u32 v)
{
	u32 shift = (n & 1) ? 0 : 8;
	u32 r = qctl_regs + (n >> 1) * 4;
	*(volatile u32 *)r = v << shift;
}

static u32 q_read(u32 n)
{
	u32 shift = (n & 1) ? 0 : 8;
	u32 r = qctl_regs + (n >> 1) * 4;
	u32 v = *(volatile u32 *)r;
	return (v >> shift) & 7;
}

#define QREQ(n, s)	q_write(n, MASK_SET(3, 1, s))
#define QCTL(n)		q_read(n)
#define QACCEPT(s)	(s & 1)
#define QACTIVE(s)	(s & 2)
#define QDENY(s)	(s & 4)

#define mdelay(n)

#define DUMP_START	0xabcd1234

static void sp_qctrl_accept_test(int led)
{
	u32 s;

	for (u32 n = 0; n < 44; n++) {
		int retry = 10;
		int pass = 0;

		if (n == 0 || n == 39) continue; // SKIP CA55/CA55SCUL3
		if (n == 3  || n == 4  ||
		    n == 6  || n == 18 ||
		    n == 20 || n == 34 ||
		    n == 38 || n == 42) continue; // SKIP Reserved

		//if (n == 15 && led == 1) continue;

		prn_string("QCTL_"); prn_decimal(n); prn_string("_"); prn_decimal(led);
TRACE;
		q_write(n, MASK_SET(6, 1, led)); // DEVICE_LED_EN

		// power down loop
		while (!pass && retry--) {
TRACE;
			QREQ(n, 0);
			while (1) {
TRACE;
				s = QCTL(n);
TRACE;
				if (!QACCEPT(s) && !QDENY(s)) { // Q_STOPPED
					pass = 1;
					break;
				}
				else if (QACCEPT(s) && QDENY(s)) { // Q_DENIED
TRACE;
					break;
				}
			}

			// re-enter Q_RUN
			QREQ(n, 1);
			while (1) {
				s = QCTL(n);
				if (QACCEPT(s) && !QDENY(s)) // Q_RUN
					break;
			}
		}

		prn_string(pass ? " PASS\n" : " FAIL!!!\n");
		if (!pass) RID_FAIL();
	}
}

static void sp_qctrl_deny_test()
{
	u32 n = 8; // SEC
	u32 s;
	u32 retry = 100;
	int pass = 0;

	q_write(n, MASK_SET(6, 1, 1)); // DEVICE_LED

	while (!QACTIVE(QCTL(n))) { // wait QACTIVE high
		if (!--retry) goto test_end; // FAILED
	}

	retry = 10;
	// power down loop
	while (!pass) {
		QREQ(n, 0);
		while (1) {
			s = QCTL(n);
			if (!QACCEPT(s) && !QDENY(s)) { // Q_STOPPED
				if (!--retry) goto test_end; // FAILED
				break; // retry for test only
			}
			else if (QACCEPT(s) && QDENY(s)) { // Q_DENIED
				pass = 1;
				break;
			}
		}

		// re-enter Q_RUN
		QREQ(n, 1);
		while (1) {
			s = QCTL(n);
			if (QACCEPT(s) && !QDENY(s)) // Q_RUN
				break;
		}
	}

test_end:
	prn_string("sp_qctrl_deny_test ");
	prn_string(pass ? "PASS\n" : "FAIL!!!\n");
	if (pass) RID_PASS(); else RID_FAIL();
}
#endif

#ifndef get_timer
u32 get_timer(u32 base)
{
	u32 now = AV1_GetStc32() / TIMER_KHZ;
	return (now - base);
}
#endif

static void reverse_buf(u8 *buf, u32 len)
{
	int i;
	char temp;

	for (i = 0; i < len/2; ++i) {
		temp = buf[i];
		buf[i] = buf[len - 1 - i];
		buf[len - 1 - i] = temp;
	}
}

#define MIN(a, b)	((a) < (b) ? (a) : (b))

#define RSA_KEY_BITS	(2048)
#define RSA_KEY_SZ	(RSA_KEY_BITS / 8)

#define HASH_BUF_SZ	(200)

#if 0
#define HASH_MODE	M_SHA3_256
#define HASH_SZ		(32)
#else
#define HASH_MODE	M_SHA3_512
#define HASH_SZ		(64)
#endif
#define BLOCK_SZ	(HASH_BUF_SZ - (HASH_SZ << 1))
#define BATCH_SZ	(0xffff / (BLOCK_SZ * 4) * (BLOCK_SZ * 4))

static volatile struct sp_crypto_reg *reg = (void *)SP_CRYPTO_REG_BASE;
static u8 *rsakey_E, *rsakey_N;
static u8 *dst, *p2;

#ifdef CONFIG_ENCRYPTION
#if 0 // FOR TEST
#define sp_load_keys()
#include "../../../build/tools/secure_hsm/secure/otp_Device_keys/x_priv_0.inc"
#include "../../../build/tools/secure_hsm/secure/otp_Sb_keys/ed_pub_0.inc"
#else
uint8_t x_priv_0[32];
uint8_t ed_pub_0[32];
static void sp_load_keys()
{
	SC_key_otp_load(x_priv_0, 24*4, 32); // OTP24~32 from G73
	SC_key_otp_load(ed_pub_0, 16*4, 32); // OTP16~23 from G73
}
#endif

#define AES_MODE	(M_AES_CTR | M_KEYLEN(32)) // AES_256_CTR
#define AES_BLK_SZ	(16)
#define AES_BATCH_SZ	(0xffff / AES_BLK_SZ * AES_BLK_SZ)

static void sp_aes(u8 *src, u8 *key, int len, u32 mode)
{
	reg->AESPAR0 = AES_MODE | mode;
	reg->AESPAR1 = (uintptr_t)rsakey_E; // iv
	reg->AESPAR2 = (uintptr_t)key;

	//len = ALIGN(len, 16); // no need this, padding @ build time
	while (len) {
		u32 bs = MIN(len, AES_BATCH_SZ);

		reg->AESSPTR  = (uintptr_t)src;
		reg->AESDPTR  = (uintptr_t)src;
		reg->AESDMACS = SEC_DMA_SIZE(bs) | SEC_DMA_ENABLE;

		while (!(reg->SECIF & AES_DMA_IF));
		reg->SECIF = AES_DMA_IF; // clear aes dma finish flag

		src += bs;
		len -= bs;
	}
}
#endif

static u8 *sp_hash(u8 *data, int len)
{
	u8 *buf = (u8 *)ALIGN((uintptr_t)p2, 32);
	u32 padding;

	//printf("HASH_MODE: %08x  %d  %d  %03x\n", HASH_MODE, HASH_SZ, BLOCK_SZ, BATCH_SZ);
	memset(buf, 0, HASH_BUF_SZ);

	/* last block, padding */
	padding = BLOCK_SZ - (len % BLOCK_SZ);
	memset(data + len, 0, padding);
	data[len] = 0x06;
	len += padding;
	data[len - 1] |= 0x80;
	//prn_dump_buffer(data + len - (BLOCK_SZ << 1), BLOCK_SZ << 1);

	reg->HASHDPTR = (uintptr_t)buf;
	reg->HASHPAR0 = HASH_MODE;
	reg->HASHPAR1 = (uintptr_t)buf;

	while (len) {
		u32 bs = MIN(len, BATCH_SZ);

		reg->HASHSPTR = (uintptr_t)data;
		reg->HASHDMACS = SEC_DMA_SIZE(bs) | SEC_DMA_ENABLE;
#ifdef QCTRL_TEST
		sp_qctrl_deny_test();
#endif

		while (!(reg->SECIF & HASH_DMA_IF));
		reg->SECIF = HASH_DMA_IF; // clear hash dma finish flag

		//prn_dump_buffer(buf, HASH_SZ);
		data += bs;
		len -= bs;
	}

	return buf;
}

static long long mont_w(unsigned char *mod)
{
	long long t = 1;
	long long mode;
	int i;

	memcpy(&mode, mod, sizeof(mode));
	for (i = 1; i < 63; i++) {
		t = (t * t * mode);
	}

	return (-t);
}

static void sp_expmod(u8 *dst, u8 *src, u8 *e, u8 *n, u32 len)
{
	reg->RSADPTR  = (uintptr_t)dst;
	reg->RSASPTR  = (uintptr_t)src;
	reg->RSAYPTR  = (uintptr_t)e;
	reg->RSAP2PTR = (uintptr_t)p2;

	if (reg->RSANPTR != (uintptr_t)n) {
		long long w = mont_w(n);
		reg->RSANPTR  = (uintptr_t)n;
		reg->RSAWPTRL = w;
		reg->RSAWPTRH = w >> 32;
		reg->RSAPAR0  = RSA_SET_PARA_D(RSA_KEY_BITS) | RSA_PARA_PRECAL_P2;
	} else {
		reg->RSAPAR0  = RSA_SET_PARA_D(RSA_KEY_BITS) | RSA_PARA_FETCH_P2;
	}

	reg->RSADMACS = SEC_DMA_SIZE(len) | SEC_DMA_ENABLE;

	while (!(reg->SECIF & RSA_DMA_IF));
	reg->SECIF = RSA_DMA_IF; // clear rsa dma finish flag
}

#define HEADER_SZ	(sizeof(struct image_header))

/* Return ROM_SUCCESS(=0) if ok */
int q654_image_verify_decrypt(const struct image_header  *hdr)
{
	u32 data_size = image_get_size(hdr) - (RSA_KEY_SZ * 2);
	u8 *data = (u8 *)image_get_data(hdr);
	u8 *src = data + data_size; // sign
	u32 t0, t1;  /* unit:ms */
	int ret;

#ifdef QCTRL_TEST
	CSTAMP(DUMP_START);
	*(volatile u32 *)0xF8800028 = 0x04000400; // PD_CA55_RESET assert
	*(volatile u32 *)0xF8800028 = 0x04000000; // PD_CA55_RESET deassert

	sp_qctrl_accept_test(0);
	sp_qctrl_accept_test(1);
#endif

	prn_string(image_get_name(hdr));
	prn_string(" ("); prn_decimal(data_size);
	prn_string(") verify signature ... ");

	t0 = get_timer(0);

	/* initial buffers */
	//prn_dump(image_get_name(hdr), (u8 *)hdr, 0x50);
	//prn_dump("sign (encrypted hash)", src, RSA_KEY_SZ);
	rsakey_N = src + (RSA_KEY_SZ * 1);
	//prn_dump("rsakey_N:", rsakey_N, RSA_KEY_SZ);
	rsakey_E = src + (RSA_KEY_SZ * 2);
	memset(rsakey_E, 0, RSA_KEY_SZ);
	rsakey_E[0] = rsakey_E[2] = 1;
	//prn_dump("rsakey_E:", rsakey_E, RSA_KEY_SZ);
	dst      = src + (RSA_KEY_SZ * 3);
	p2       = src + (RSA_KEY_SZ * 4);

	/* public key decrypt */
	sp_expmod(dst, src, rsakey_E, rsakey_N, RSA_KEY_SZ);
	reverse_buf(dst, HASH_SZ);

	/* hash data */
	src = sp_hash(data, data_size);

	/* verify sign */
	ret = memcmp(dst, src, HASH_SZ);

#ifdef CONFIG_ENCRYPTION
	/* aes decrypt data */
	sp_load_keys();
	//prn_dump("x_priv_0n", x_priv_0, 32);
	//prn_dump("ed_pub_0n", ed_pub_0, 32);
	memcpy(rsakey_N, x_priv_0, 32);
	memcpy(rsakey_E, ed_pub_0, 32);
	sp_aes(rsakey_N, rsakey_E, 32, M_ENC);		// key1 key:key2 iv:key2 -> key3
	//prn_dump("key3", rsakey_N, 32);
	memcpy(rsakey_E, x_priv_0, 32);
	sp_aes(data, rsakey_N, data_size, M_DEC);	// data key:key3 iv:key1
#endif

	t1 = get_timer(t0);

	//prn_dump("decrypted hash", dst, HASH_SZ);
	//prn_dump("hash", src, HASH_SZ);
	prn_decimal(t1); prn_string(" ms: "); prn_decimal_ln(ret);

	return ret;
}
