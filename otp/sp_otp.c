#include <common.h>
#include <otp/sp_otp.h>


int otprx_read(volatile struct hb_gp_regs *otp_data, volatile struct otprx_regs *regs, int addr, char *value)
{
	unsigned int addr_data;
	unsigned int byte_shift;
	unsigned int status;
	u32 timeout = OTP_READ_TIMEOUT;

	addr_data = addr % (OTP_WORD_SIZE * OTP_WORDS_PER_BANK);
	addr_data = addr_data / OTP_WORD_SIZE;

	byte_shift = addr % (OTP_WORD_SIZE * OTP_WORDS_PER_BANK);
	byte_shift = byte_shift % OTP_WORD_SIZE;

	regs->otp_cmd_status = 0x0;

	addr = addr / (OTP_WORD_SIZE * OTP_WORDS_PER_BANK);
	addr = addr * OTP_BIT_ADDR_OF_BANK;
	regs->otp_addr = addr;

	regs->otp_cmd = 0x1E04;

	do {
		delay_1ms(1);
		if (timeout-- == 0)
		    return -1;

		status = regs->otp_cmd_status;
	} while((status & OTP_READ_DONE) != OTP_READ_DONE);

	*value = (otp_data->reserved_8[addr_data] >> (8 * byte_shift)) & 0xFF;

	return 0;
}

int otprx_write(volatile struct hb_gp_regs *otp_data, volatile struct otprx_regs *regs, int addr, char value)
{
	unsigned int data;
	u32 timeout = OTP_WRITE_TIMEOUT;

	regs->otp_ctrl = 0xFD01;
	regs->otp_prog_addr = addr;
	regs->otp_prog_ctl = 0x03;

	data = value;
	data = (data << 8) + 0x12;
	regs->otp_prog_reg25= data;

	regs->otp_prog_wr = 0x01;
	regs->otp_prog_pgenb = 0x00;

	do {
		delay_1ms(1);
		if (timeout-- == 0)
			return -1;

		data = regs->otp_prog_state;
	} while((data & 0x1F) != 0x13);

	regs->otp_prog_pgenb =0x01;
	regs->otp_prog_wr = 0x00;
	regs->otp_prog_ctl = 0x00;

	return 0;
}