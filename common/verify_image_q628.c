#define VERIFY_SIGN_MAGIC_DATA	(0x7369676E)  //SIGN

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
