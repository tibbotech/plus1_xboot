#ifndef _BOOTMAIN_INC_
#define _BOOTMAIN_INC_

/* xboot.img = xboot_hdr + xboot_encrypted(Q645) + sb_info    */

#define XBOOT_HDR_MAGIC   0x54554258      // XBUT (X=58h)
#define XB_SB_FLAG        (1 << 0)       // secure boot

struct xboot_hdr {
	u32 magic;
	u32 version;
	u32 length;       // exclude header
	u32 checksum;     // exclude header
	u32 img_flag;
	u32 reserved[3];
};


/* hsm_keys data will be part of encrypted xboot body. See boot.ldi . */
#define HSM_KEY_MAGIC     0x4b4d5348   // HSMK (H=48h)
struct hsm_keys {
	u32 magic;

	// ROM_CODE will apply those keys and clear these key fileds.
	u32 key_duk[4];     // Device Unique Key (HSM KGK is generated from DUK and KPF)
	u32 key_bbr[4];     // semc BBR key
	u32 key_app0[4];    // semc APP0 key
	u32 key_app1[4];    // semc APP1 key
	u32 key_adc[4];     // semc ADC key

	// Reserved for user_keys[]
	u32 len_user_keys;  // byte length of user_keys[]
	u8  user_keys[4];   // user key (length can vary)

} __attribute__((packed));


/*
 * security info (appended after xboot.bin)
 */
#define SB_MAGIC          0x55434553     // SECU (S=55h)
#define SB_FLAG_SIGNED    (1 << 0)       // image signature is provided
#define SB_FLAG_ENCRYPTED (1 << 1)       // image is encrypted

struct sb_info {
	u32 magic;
	u32 sb_flags;

	// Detect wrong OTP keys
	u32 hash_Sb_Kpub;      // Sb_Kpub hash   : sha3-512/32
	u32 hash_Dev_Kpriv;    // Dev_Kpriv hash : sha3-512/32

	// Secure boot signature
	u32 sb_signature[16];  // ed25519 signature, with this field = 0, excluding xboot_hdr

	// offset = 80B

	// ECIES encryption of KAES :
	u32 eph_Kpub[8];       // ECIES: ephemeral publick key (32B)
	u32 eph_IV[3];         // ephemeral IV (12B)
	u32 reserved_84;       // pad to 8-byte

	// offset = 128B

	// encrypted KAES
	u32 KAES_auth_tag[4];  // GCM auth tag (16B) of KAES_encrypted
	u32 KAES_encrypted[8]; // KAES (32B) encrypted by ECIES

	// offset = 176B

	// encrypted xboot
	u32 body_auth_tag[4];  // GCM auth tag (16B) of xboot_encrypted
	u32 body_cipher_len;   // xboot encrypted length (if not fully encrypted)
	u32 reserved_188;      // pad to 8-byte

	// above total 200 bytes (4 * 50)
} __attribute__((packed));

/*
	OTP :
	Sb_Kpub      : OTP key for EdDSA_verify
	Dev_Kpriv : OTP key for ECIES_decrypt (KAES_encrypted)

	PC encryption :
	0. Gen for each build : eph_IV, KAES
	1. xBoot_encrypted = AES_encrypt   (KAES, data=xboot, eph_IV)
	2. (KAES_encrypted, eph_IV, eph_Kpub, eph_MAC)
                           = ECIES_encrypt (Device_Kpub, KAES)
                           = Gen ephemeral ASYM key : { eph_Kpub, eph_Kpriv } ,
			     shared_secret = ECDH(eph_Kpriv, Device_Kpub),
                             secret_key = HKDF(shared_secret),
                             (KAES_encrypted, eph_MAC) = AES_encrypt(secret_key, data=KAES, eph_IV)
	3. Sb_signature    = EdDSA_sign    (Sb_Kpriv, output of step 1 and 2)

	Image = output of step 1, 2, 3

	Device decryption :
	3. true            = EdDSA_verify  (Sb_Kpub, Image)
	2. KAES            = ECIES_decrypt (Dev_Kpriv, KAES_encrypted, eph_IV, eph_Kpub, eph_MAC)
	1. xboot           = AES_decrypt   (KAES, data=xBoot_encrypted, eph_IV)
 */
void set_spi_nor_pinmux(int pin_x);
void SetBootDev(unsigned int bootdev, unsigned int pinx, unsigned int dev_port);

int get_xboot_size(u8 *img);
int verify_xboot_img(u8 *img);

/* pinmux ops */
int get_spi_nor_pinmux(void);

#endif
