/******************************************************************************
*                          Include File
*******************************************************************************/
#include <common.h>
#include <bootmain.h>
#include <otp/sp_otp.h>

#ifdef CONFIG_SECURE_BOOT_SIGN

// u-boot.img =|header+data+sbinf0|
#define SB_INFO_SIZE	200 

#define VERIFY_SIGN_MAGIC_DATA	(0x7369676E)  //SIGN

#define __ALIGN4       __attribute__((aligned(4)))
#define __ALIGN8       __attribute__((aligned(8)))

void prn_dump(const char *title, const unsigned char *buf, int len)
{
	int i;

	if (title) {
		prn_string(title);
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
// ECIES decrypt
//
// Input : (receiver_Kpriv), (ephemeral_Kpub), (iv, ivlen), ciphertext(in), auth_tag
// Output: plaintext(out)
// Return 0 if decrypted auth_tag is matched
//
// 1. shared_secret ss = ECDH(ephemeral_Kpub, receiver_Kpriv)
// 2. secrete key sk = HKDF(ss)
// 3. (plaintext out, tag) = AES_GCM(sk, in)
// 4. verify (tag == auth_tag)
int q654_ecies_curve25519_decrypt(const u8 *receiver_Kpriv, const u8 *ephemeral_Kpub,
		const u8 *iv, u32 ivlen, u8 *in, u32 inlen, u8 *out, const u8 auth_tag[16])
{
	__ALIGN4
	unsigned char ss[32], sk[32];
	const unsigned char additional[] = {};
	int ret = -1;

	SC_x25519(ss, ephemeral_Kpub, receiver_Kpriv);

	// S1=salt=NULL, info=NULL
	SC_hkdf_shaX_512(NULL, 0, ss, 32, NULL, 0, sk, 32);

	ret = SC_aes_gcm_ad(sk, 32, iv, (size_t)ivlen, in, inlen, additional, 0, auth_tag, out);

	if (ret) {
		prn_string("bad decrypt in ECIES aes gcm\n");
	}

	memset(ss, 0, sizeof(ss));
	memset(sk, 0, sizeof(sk));

	return ret;
}


static int is_nonzero(const u8 *buf, int len)
{
	int i = 0;

	for (i = 0; i < len; i++)
		if (buf[i])
			return 1;
	return 0;
}

const uint8_t ed_pub_0[32] = { 0xA0, 0xD0, 0x0C, 0xB4, 0xA3, 0x48, 0x2C, 0x53, 0xC1, 0x32, 0xC9, 0x07, 0x9F, 0x92, 0xA4, 0x3E, 0x4F, 0x8B, 0xDA, 0x04, 0x62, 0xCA, 0x96, 0x1B, 0x9F, 0xBE, 0x06, 0xC8, 0xEC, 0x5D, 0x4F, 0x89 };
static int q654_load_otp_Sb_pub_key(u8 in_pub[32])
{
	int ret = 0;

	prn_string("load OTP Sb_Kpub\n");
#if 0 //test code for use test-keys
	//#include "../secure/test-keys/ed_pub_0.inc"
	//#include "../../../build/tools/secure_hsm/secure/otp_Sb_keys/ed_pub_0.inc"
	memcpy(in_pub, ed_pub_0, 32);
	prn_string("Test pub-key:\n");
#else
	CSTAMP(0xbbbbbbbb);

	ret = SC_key_otp_load(in_pub, 16*4, 32); // OTP16~23 from G73

#endif

#ifdef CONFIG_BOOT_ON_ZEBU
	prn_dump("Kpub:\n", in_pub, 32);
#endif

	CSTAMP(0x00001258);
	return ret;
}

const uint8_t x_priv_0[32] = { 0x88, 0xD2, 0xFB, 0x65, 0xBF, 0xF7, 0xB9, 0x2D, 0xCF, 0x7A, 0x9B, 0x29, 0xBF, 0x49, 0xBC, 0xF7, 0xE9, 0x80, 0x56, 0x70, 0x28, 0xF7, 0x10, 0xD2, 0x0C, 0x0E, 0x61, 0x5A, 0xE3, 0x53, 0xA9, 0x7E };
static int q654_load_otp_Device_priv_key(u8 in_priv[32])
{
	int ret = 0;

	prn_string("load OTP Dev_Kpriv\n");
#if 0 //test code for use test-keys
	//#include "../secure/test-keys/x_priv_0.inc"
	//#include "../../../build/tools/secure_hsm/secure/otp_Device_keys/ed_pub_0.inc"
	memcpy(in_priv, x_priv_0, 32);
	prn_string("Test priv-key:\n");
#else

	ret = SC_key_otp_load(in_priv, 24*4, 32); // OTP24~32 from G73

#endif

#ifdef CONFIG_BOOT_ON_ZEBU
	prn_dump("Dev_Kpriv:\n", in_priv, 32);
#endif

	return ret;
}

static struct sb_info *q654_get_sb_info(const struct image_header  *hdr)
{
	struct sb_info *xsb = NULL;

	int imgsize=image_get_size(hdr);
	
	// offs_sb is offset to sb_info after uboot_hdr
	xsb =(struct sb_info *)(((u8 *)hdr) + imgsize  + sizeof(struct image_header) - SB_INFO_SIZE);
	if (((u32)xsb & 0x3) || (xsb->magic != SB_MAGIC)) {
		prn_string("bad sb magic @");
		prn_dword((u32)xsb);
		if (0 == ((u32)xsb & 3)) {
			prn_dword0(xsb->magic);
			prn_dump("sb_info\n", (const unsigned char *)xsb, sizeof(struct sb_info));
		}
		return NULL;
	}
	return xsb;
}

static int q654_verify_image_signature(const struct image_header *hdr, struct sb_info *xsb)
{
	__ALIGN4
	u8 h_val[64], sig[64];
	__ALIGN4
	u8 in_pub[32];
	u8 *data;
	int sig_size = 64;
	int data_size;
	u32 t1, t2;
	int ret = -1;

	CSTAMP(0x235b0001);

	prn_string("Verify signature\n");

	/* Load public key */
	if (q654_load_otp_Sb_pub_key(in_pub)) {
		prn_string("load otp Sb_Kpub fail\n");
		return ret;
	}

	if (!is_nonzero(in_pub, 32)) {
		prn_string("Sb_Kpub : all zero\n");
		return -1;
	}

	if (SC_shaX_512(in_pub, 32, (u8 *)h_val)) {
		dbg();
		return -1;
	}

	if (*(u32 *)h_val != xsb->hash_Sb_Kpub) {
		prn_string("Detected wrong key, Sb_Kpub hash ");
		prn_dword0(*(u32 *)h_val);
		prn_string(" != ");
		prn_dword(xsb->hash_Sb_Kpub);
		return ret;
	}

	/* load signature from sb_info */
	memcpy(sig, xsb->sb_signature, 64);
	memset(xsb->sb_signature, 0, 64); /* compute signature with sb_signature = 0 */


	/* data = bin + sb_info */
	data = ((u8 *)hdr) + sizeof(struct image_header);
	data_size = image_get_size(hdr);

	/* ed25519 hash sequence */
	//t1 = AV1_GetStc32();
	SC_ed25519_hash(sig, data, data_size, in_pub, h_val);
	//t2 = (AV1_GetStc32() - t1) / TIMER_KHZ;
	//prn_string("tH=");
	//prn_decimal_ln(t2); // 0

	//prn_dump("hash:\n", h_val, 64);

	/* verify signature : EdDSA */
	t1 = AV1_GetStc32();
	ret = SC_ed25519_verify_hash(sig, in_pub, h_val);
	t2 = (AV1_GetStc32() - t1) / TIMER_KHZ;

	if (ret) {
		prn_string(" FAIL\n");
		prn_dump("signature in image is bad:\n", sig, sig_size);
		prn_dump("calc hash:\n", h_val, sizeof(h_val));
	} else {
		prn_string(" OK\n");
	}

	prn_string("tV=");
	prn_decimal_ln(t2);

	return ret;
}

static int q654_decrypt_image(const struct image_header *hdr, struct sb_info *xsb, u8 *cipher_bin)
{
	__ALIGN4
	u8 dev_Kpriv[32];
	__ALIGN4
	u8 sess_Kaes[32];
	__ALIGN4
	u8 h_val[64];
	const unsigned char additional[] = {};
	int ret = -1;
	u32 t1, t2;

	CSTAMP(0x236b0001);

	if (q654_load_otp_Device_priv_key(dev_Kpriv)) {
		prn_string("load otp Dev_Kpriv fail\n");
		return -1;
	}

	if (!is_nonzero(dev_Kpriv, 32)) {
		prn_string("Dev_Kpriv : all zero\n");
		return -1;
	}

	if (SC_shaX_512(dev_Kpriv, 32, (u8 *)h_val)) {
		dbg();
		return -1;
	}

	if (*(u32 *)h_val != xsb->hash_Dev_Kpriv) {
		prn_string("Detected wrong key, Dev_Kpriv hash ");
		prn_dword0(*(u32 *)h_val);
		prn_string(" != ");
		prn_dword(xsb->hash_Dev_Kpriv);
		return ret;
	}

	prn_string("Decrypting KAES\n");


	t1 = AV1_GetStc32();
#if 0
	// Method 1: Ed25519_verify + AES_decrypt key   + AES_decrypt uboot
	ret = SC_aes_gcm_ad(dev_Kpriv, 32, xsb->eph_IV, 12, xsb->KAES_encrypted,
			32, additional, 0, xsb->KAES_auth_tag, 0);
#else
	// Method 2: Ed25519_verify + ECIES_decrypt key + AES_decrypt uboot
	ret = q654_ecies_curve25519_decrypt(dev_Kpriv, (const u8 *)xsb->eph_Kpub,
		(const u8 *)xsb->eph_IV, 12, (u8 *)xsb->KAES_encrypted, 32,
		sess_Kaes, (const u8 *)xsb->KAES_auth_tag);
#endif
	t2 = (AV1_GetStc32() - t1) / TIMER_KHZ;

	prn_string("tECIES=");
	prn_decimal_ln(t2);

	CSTAMP(0xa1520001);
	CSTAMP(t2);

	prn_string("Decrypt KAES: ");
	if (ret) {
		prn_string("fail\n");
#ifdef PLATFORM_SPIBAREMETAL
		prn_string("WARN: clean-build iboot to update uboot test key\n");
		while (1);
#endif
		goto clr_out;
	} else {
		prn_string("ok\n");

		prn_string("Decrypting uboot, len=");
		prn_dword(xsb->body_cipher_len);

		t1 = AV1_GetStc32();
		ret = SC_aes_gcm_ad(sess_Kaes, 32, (u8 *)xsb->eph_IV, 12, cipher_bin,
				xsb->body_cipher_len, additional, 0, (u8 *)xsb->body_auth_tag, cipher_bin); /* in place */
		t2 = (AV1_GetStc32() - t1) / TIMER_KHZ;
		prn_string("tAES=");
		prn_decimal_ln(t2);

		CSTAMP(0xa1520002);
		CSTAMP(t2);

		if (ret) {
			prn_string("decrypt fail\n");
		}
	}

clr_out:
	memset(dev_Kpriv, 0, 32);
	memset(sess_Kaes, 0, 32);
	return ret;
}

/* Return ROM_SUCCESS(=0) if ok */
int q654_image_verify_decrypt(const struct image_header  *hdr)
{
	int ret = 0;
	int mmu = 0;
	struct sb_info *xsb;
	
	CSTAMP(0x23490001);

	if(CONFIG_COMPILE_WITH_SECURE == 0){
		prn_string("not Secure image\n");
		return ROM_SUCCESS;
	}
	
	if (!IS_IC_SECURE_ENABLE()) {
		prn_string("Error: non-secure IC can't boot Secure image\n");
		return ROM_FAIL;
	}

	/* Secure boot flow requirement:
	 * 1. OTP[RMA] != 0
	 * 2. OTP[SECURE] = 1
	 * 3. OTP[KEY] != 0
	 * 4. SB image (sb_info appended)
	 */

	CSTAMP(0x23490004);
	
	/* Is SB info appended */
	prn_string("read SB info\n");
	xsb = q654_get_sb_info(hdr);
	if (NULL == xsb){
		prn_string("SB img: bad SB info\n");
		return ROM_FAIL;
	}

	/* Is signature appended ? */
	if ((xsb->sb_flags & SB_FLAG_SIGNED) == 0) {
		prn_string("SB img: missed signed flag\n");
		return ROM_FAIL;
	}

	/* Enable MMU and DCache */
	mmu = 1;
	CSTAMP(0x23490007);
	hal_dcache_enable();
	CSTAMP(0x23490008);

	/* Verify signature */

	if (q654_verify_image_signature(hdr, xsb)) {
		CSTAMP(0x23490009);

		// Bad signature
		prn_string("Bad signature\n");

		// Let boot_flow() fallback to ISP soon
		ret = ROM_FAIL;
		goto sb_out;
	}

	prn_string("Verified signature\n");

	CSTAMP(0x2349000b);

	
	/* Is encrypted ? */
	if ((xsb->sb_flags & SB_FLAG_ENCRYPTED) == 0) {
		prn_string("SB img: no encrypted flag\n");
		ret = ROM_SUCCESS;
		goto sb_out;
	}

	/* Decrypt uboot */
	if (q654_decrypt_image(hdr, xsb, (u8 *)hdr + sizeof(struct image_header))) {
		CSTAMP(0x2349000c);

		prn_string("fail to decrypt uboot\n");
		ret = ROM_FAIL;
		goto sb_out;
	}

	prn_string("Decrypted valid uboot\n");

	CSTAMP(0x2349000d);

sb_out:
	CSTAMP(0x23490100);

	/* disable mmu and dcache */
	if (mmu) {
		CSTAMP(0x23490101);
		hal_dcache_disable();
	}

	CSTAMP(0x23490102);
	return ret;
}

#elif defined(PLATFORM_Q645)
// ECIES decrypt
//
// Input : (receiver_Kpriv), (ephemeral_Kpub), (iv, ivlen), ciphertext(in), auth_tag
// Output: plaintext(out)
// Return 0 if decrypted auth_tag is matched
//
// 1. shared_secret ss = ECDH(ephemeral_Kpub, receiver_Kpriv)
// 2. secrete key sk = HKDF(ss)
// 3. (plaintext out, tag) = AES_GCM(sk, in)
// 4. verify (tag == auth_tag)
int q645_ecies_curve25519_decrypt(const u8 *receiver_Kpriv, const u8 *ephemeral_Kpub,
		const u8 *iv, u32 ivlen, u8 *in, u32 inlen, u8 *out, const u8 auth_tag[16])
{
	__ALIGN4
	unsigned char ss[32], sk[32];
	const unsigned char additional[] = {};
	int ret = -1;

	SC_x25519(ss, ephemeral_Kpub, receiver_Kpriv);

	// S1=salt=NULL, info=NULL
	SC_hkdf_shaX_512(NULL, 0, ss, 32, NULL, 0, sk, 32);

	ret = SC_aes_gcm_ad(sk, 32, iv, (size_t)ivlen, in, inlen, additional, 0, auth_tag, out);

	if (ret) {
		prn_string("bad decrypt in ECIES aes gcm\n");
	}

	memset(ss, 0, sizeof(ss));
	memset(sk, 0, sizeof(sk));

	return ret;
}


static int is_nonzero(const u8 *buf, int len)
{
	int i = 0;

	for (i = 0; i < len; i++)
		if (buf[i])
			return 1;
	return 0;
}

const uint8_t ed_pub_0[32] = { 0xA0, 0xD0, 0x0C, 0xB4, 0xA3, 0x48, 0x2C, 0x53, 0xC1, 0x32, 0xC9, 0x07, 0x9F, 0x92, 0xA4, 0x3E, 0x4F, 0x8B, 0xDA, 0x04, 0x62, 0xCA, 0x96, 0x1B, 0x9F, 0xBE, 0x06, 0xC8, 0xEC, 0x5D, 0x4F, 0x89 };
static int q645_load_otp_Sb_pub_key(u8 in_pub[32])
{
	int ret = 0;

	prn_string("load OTP Sb_Kpub\n");
#if 0 //test code for use test-keys
	//#include "../secure/test-keys/ed_pub_0.inc"
	//#include "../../../build/tools/secure_hsm/secure/otp_Sb_keys/ed_pub_0.inc"
	memcpy(in_pub, ed_pub_0, 32);
	prn_string("Test pub-key:\n");
#else
	CSTAMP(0xbbbbbbbb);

	ret = SC_key_otp_load(in_pub, 0, 32); // G779.0~7
#endif

#ifdef CONFIG_BOOT_ON_ZEBU
	prn_dump("Kpub:\n", in_pub, 32);
#endif

	CSTAMP(0x00001258);
	return ret;
}

const uint8_t x_priv_0[32] = { 0x88, 0xD2, 0xFB, 0x65, 0xBF, 0xF7, 0xB9, 0x2D, 0xCF, 0x7A, 0x9B, 0x29, 0xBF, 0x49, 0xBC, 0xF7, 0xE9, 0x80, 0x56, 0x70, 0x28, 0xF7, 0x10, 0xD2, 0x0C, 0x0E, 0x61, 0x5A, 0xE3, 0x53, 0xA9, 0x7E };
static int q645_load_otp_Device_priv_key(u8 in_priv[32])
{
	int ret = 0;

	prn_string("load OTP Dev_Kpriv\n");
#if 0 //test code for use test-keys
	//#include "../secure/test-keys/x_priv_0.inc"
	//#include "../../../build/tools/secure_hsm/secure/otp_Device_keys/ed_pub_0.inc"
	memcpy(in_priv, x_priv_0, 32);
	prn_string("Test priv-key:\n");
#else

	ret = SC_key_otp_load(in_priv, 32, 32); // G779.8~15
#endif

#ifdef CONFIG_BOOT_ON_ZEBU
	prn_dump("Dev_Kpriv:\n", in_priv, 32);
#endif

	return ret;
}

static struct sb_info *q645_get_sb_info(const struct image_header  *hdr)
{
	struct sb_info *xsb = NULL;

	int imgsize=image_get_size(hdr);
	
	// offs_sb is offset to sb_info after uboot_hdr
	xsb =(struct sb_info *)(((u8 *)hdr) + imgsize  + sizeof(struct image_header) - SB_INFO_SIZE);
	if (((u32)xsb & 0x3) || (xsb->magic != SB_MAGIC)) {
		prn_string("bad sb magic @");
		prn_dword((u32)xsb);
		if (0 == ((u32)xsb & 3)) {
			prn_dword0(xsb->magic);
			prn_dump("sb_info\n", (const unsigned char *)xsb, sizeof(struct sb_info));
		}
		return NULL;
	}
	return xsb;
}

static int q645_verify_image_signature(const struct image_header *hdr, struct sb_info *xsb)
{
	__ALIGN4
	u8 h_val[64], sig[64];
	__ALIGN4
	u8 in_pub[32];
	u8 *data;
	int sig_size = 64;
	int data_size;
	u32 t1, t2;
	int ret = -1;

	CSTAMP(0x235b0001);

	prn_string("Verify signature\n");

	/* Load public key */
	if (q645_load_otp_Sb_pub_key(in_pub)) {
		prn_string("load otp Sb_Kpub fail\n");
		return ret;
	}

	if (!is_nonzero(in_pub, 32)) {
		prn_string("Sb_Kpub : all zero\n");
		return -1;
	}

	if (SC_shaX_512(in_pub, 32, (u8 *)h_val)) {
		dbg();
		return -1;
	}

	if (*(u32 *)h_val != xsb->hash_Sb_Kpub) {
		prn_string("Detected wrong key, Sb_Kpub hash ");
		prn_dword0(*(u32 *)h_val);
		prn_string(" != ");
		prn_dword(xsb->hash_Sb_Kpub);
		return ret;
	}

	/* load signature from sb_info */
	memcpy(sig, xsb->sb_signature, 64);
	memset(xsb->sb_signature, 0, 64); /* compute signature with sb_signature = 0 */


	/* data = bin + sb_info */
	data = ((u8 *)hdr) + sizeof(struct image_header);
	data_size = image_get_size(hdr);

	/* ed25519 hash sequence */
	//t1 = AV1_GetStc32();
	SC_ed25519_hash(sig, data, data_size, in_pub, h_val);
	//t2 = (AV1_GetStc32() - t1) / TIMER_KHZ;
	//prn_string("tH=");
	//prn_decimal_ln(t2); // 0

	//prn_dump("hash:\n", h_val, 64);

	/* verify signature : EdDSA */
	t1 = AV1_GetStc32();
	ret = SC_ed25519_verify_hash(sig, in_pub, h_val);
	t2 = (AV1_GetStc32() - t1) / TIMER_KHZ;

	if (ret) {
		prn_string(" FAIL\n");
		prn_dump("signature in image is bad:\n", sig, sig_size);
		prn_dump("calc hash:\n", h_val, sizeof(h_val));
	} else {
		prn_string(" OK\n");
	}

	prn_string("tV=");
	prn_decimal_ln(t2);

	return ret;
}

static int q645_decrypt_image(const struct image_header *hdr, struct sb_info *xsb, u8 *cipher_bin)
{
	__ALIGN4
	u8 dev_Kpriv[32];
	__ALIGN4
	u8 sess_Kaes[32];
	__ALIGN4
	u8 h_val[64];
	const unsigned char additional[] = {};
	int ret = -1;
	u32 t1, t2;

	CSTAMP(0x236b0001);

	if (q645_load_otp_Device_priv_key(dev_Kpriv)) {
		prn_string("load otp Dev_Kpriv fail\n");
		return -1;
	}

	if (!is_nonzero(dev_Kpriv, 32)) {
		prn_string("Dev_Kpriv : all zero\n");
		return -1;
	}

	if (SC_shaX_512(dev_Kpriv, 32, (u8 *)h_val)) {
		dbg();
		return -1;
	}

	if (*(u32 *)h_val != xsb->hash_Dev_Kpriv) {
		prn_string("Detected wrong key, Dev_Kpriv hash ");
		prn_dword0(*(u32 *)h_val);
		prn_string(" != ");
		prn_dword(xsb->hash_Dev_Kpriv);
		return ret;
	}

	prn_string("Decrypting KAES\n");


	t1 = AV1_GetStc32();
#if 0
	// Method 1: Ed25519_verify + AES_decrypt key   + AES_decrypt uboot
	ret = SC_aes_gcm_ad(dev_Kpriv, 32, xsb->eph_IV, 12, xsb->KAES_encrypted,
			32, additional, 0, xsb->KAES_auth_tag, 0);
#else
	// Method 2: Ed25519_verify + ECIES_decrypt key + AES_decrypt uboot
	ret = q645_ecies_curve25519_decrypt(dev_Kpriv, (const u8 *)xsb->eph_Kpub,
		(const u8 *)xsb->eph_IV, 12, (u8 *)xsb->KAES_encrypted, 32,
		sess_Kaes, (const u8 *)xsb->KAES_auth_tag);
#endif
	t2 = (AV1_GetStc32() - t1) / TIMER_KHZ;

	prn_string("tECIES=");
	prn_decimal_ln(t2);

	CSTAMP(0xa1520001);
	CSTAMP(t2);

	prn_string("Decrypt KAES: ");
	if (ret) {
		prn_string("fail\n");
#ifdef PLATFORM_SPIBAREMETAL
		prn_string("WARN: clean-build iboot to update uboot test key\n");
		while (1);
#endif
		goto clr_out;
	} else {
		prn_string("ok\n");

		prn_string("Decrypting uboot, len=");
		prn_dword(xsb->body_cipher_len);

		t1 = AV1_GetStc32();
		ret = SC_aes_gcm_ad(sess_Kaes, 32, (u8 *)xsb->eph_IV, 12, cipher_bin,
				xsb->body_cipher_len, additional, 0, (u8 *)xsb->body_auth_tag, cipher_bin); /* in place */
		t2 = (AV1_GetStc32() - t1) / TIMER_KHZ;
		prn_string("tAES=");
		prn_decimal_ln(t2);

		CSTAMP(0xa1520002);
		CSTAMP(t2);

		if (ret) {
			prn_string("decrypt fail\n");
		}
	}

clr_out:
	memset(dev_Kpriv, 0, 32);
	memset(sess_Kaes, 0, 32);
	return ret;
}

/* Return ROM_SUCCESS(=0) if ok */
int q645_image_verify_decrypt(const struct image_header  *hdr)
{
	int ret = 0;
	int mmu = 0;
	struct sb_info *xsb;
	
	CSTAMP(0x23490001);

	if(CONFIG_COMPILE_WITH_SECURE == 0){
		prn_string("not Secure image\n");
		return ROM_SUCCESS;
	}
	
	if (!IS_IC_SECURE_ENABLE()) {
		prn_string("Error: non-secure IC can't boot Secure image\n");
		return ROM_FAIL;
	}

	/* Secure boot flow requirement:
	 * 1. OTP[RMA] != 0
	 * 2. OTP[SECURE] = 1
	 * 3. OTP[KEY] != 0
	 * 4. SB image (sb_info appended)
	 */

	CSTAMP(0x23490004);
	
	/* Is SB info appended */
	prn_string("read SB info\n");
	xsb = q645_get_sb_info(hdr);
	if (NULL == xsb){
		prn_string("SB img: bad SB info\n");
		return ROM_FAIL;
	}

	/* Is signature appended ? */
	if ((xsb->sb_flags & SB_FLAG_SIGNED) == 0) {
		prn_string("SB img: missed signed flag\n");
		return ROM_FAIL;
	}

	/* Enable MMU and DCache */
	mmu = 1;
	CSTAMP(0x23490007);
	hal_dcache_enable();
	CSTAMP(0x23490008);

	/* Verify signature */

	if (q645_verify_image_signature(hdr, xsb)) {
		CSTAMP(0x23490009);

		// Bad signature
		prn_string("Bad signature\n");

		// Let boot_flow() fallback to ISP soon
		ret = ROM_FAIL;
		goto sb_out;
	}

	prn_string("Verified signature\n");

	CSTAMP(0x2349000b);

	
	/* Is encrypted ? */
	if ((xsb->sb_flags & SB_FLAG_ENCRYPTED) == 0) {
		prn_string("SB img: no encrypted flag\n");
		ret = ROM_SUCCESS;
		goto sb_out;
	}

	/* Decrypt uboot */
	if (q645_decrypt_image(hdr, xsb, (u8 *)hdr + sizeof(struct image_header))) {
		CSTAMP(0x2349000c);

		prn_string("fail to decrypt uboot\n");
		ret = ROM_FAIL;
		goto sb_out;
	}

	prn_string("Decrypted valid uboot\n");

	CSTAMP(0x2349000d);

sb_out:
	CSTAMP(0x23490100);

	/* disable mmu and dcache */
	if (mmu) {
		CSTAMP(0x23490101);
		hal_dcache_disable();
	}

	CSTAMP(0x23490102);
	return ret;
}

#else
static void q628_load_otp_pub_key(unsigned char *in_pub)
{
	int i;
	for (i = 0; i < 32; i++) {
#ifdef CONFIG_HAVE_OTP
		otprx_read(HB_GP_REG,SP_OTPRX_REG,i+64,(char *)&in_pub[i]);
#else
		in_pub[i] = 0;
#endif
	}
	prn_string("OTP pub-key:\n");
	prn_dump_buffer(in_pub, 32);
}

int q628_verify_uboot_signature(const struct image_header  *hdr)
{
	int sig_size = 64;
	int sig_flag_size = 8;
	int ret = -1,mmu = 1;
	int imgsize = 0,data_size;
	u8 	in_pub[32];
	u8 *data=NULL, *sig=NULL;

	/* Not Secure Chip => return ok */
	if ((!(g_bootinfo.sb_flag & SB_FLAG_ENABLE))) {
		prn_string("\n ******OTP Secure Boot is OFF, return success******\n");
		return 0;
	}

	imgsize = image_get_size(hdr);

	/* load signature from image end */
	if (imgsize < sig_size) {
		prn_string("too small img\n");
		goto out;
	}
	prn_string("Verify signature...(xboot-->uboot)\n");

	if (!g_bootinfo.bootcpu) {
		/* If B boots, need A to enable mmu */
		if ((HB_GP_REG->hb_otp_data0 >> 11) & 0x1) {
			prn_string("(B only)");
			mmu = 0;
		} else {
			prn_string("(AB)");
		}
	}

	data = ((u8 *)hdr);
	data_size = imgsize  + sizeof(struct image_header);//- sig_size-sig_flag_size;
	sig = data + data_size+sig_flag_size;
	prn_string("sig_magic_data=");
	u32 sig_magic_data = (*(u8 *)(data+data_size))<<24|(*(u8 *)(data+data_size+1))<<16|(*(u8 *)(data+data_size+2))<<8|(*(u8 *)(data+data_size+3));
	prn_dword(sig_magic_data);
	if(sig_magic_data != VERIFY_SIGN_MAGIC_DATA)
	{
		prn_string("\n imgdata no secure flag \n");
		goto out;
	}

	q628_load_otp_pub_key(in_pub);

	/* verify signature */
	int  (*fptr)(const unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key);;
	fptr = (int (*)(const unsigned char *, const unsigned char *, size_t , const unsigned char *))SECURE_VERIFY_FUN_ADDR;

	if (mmu) {
		/* enable mmu and dcache */
		fill_mmu_page_table();
		enable_mmu();
	}
	ret = !fptr(sig, data, data_size, in_pub);
	if (ret) {
		prn_string("\nverify FAIL !!!!!!\t signature:\n");
		prn_dump_buffer(sig, sig_size);
	} else {
		prn_string("\nverify OK  !!!!!!\n");
	}
	/* disable dcache */
	if (mmu) {
		disable_mmu();
	}
out:
	return ret;
}
#endif

int xboot_verify_next_image(const struct image_header  *hdr)
{
	int ret = -1;
#if defined(PLATFORM_SP7350)
	ret = q654_image_verify_decrypt(hdr);
#elif defined(PLATFORM_Q645)
	ret = q645_image_verify_decrypt(hdr);
#else
	ret = q628_verify_uboot_signature(hdr);
#endif	
	if(ret)
	{
		prn_string("\n xboot verify uboot fail !! \nhalt!");
	}
	return ret;
}
#endif
