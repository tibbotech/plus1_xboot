#include <otp/sp_otp.h>

static volatile struct otprx2_sunplus *regs = (volatile struct otprx2_sunplus *)(0x9C002800);
static volatile struct hbgpio2_sunplus *otp_data = (volatile struct hbgpio2_sunplus *)(0x9C0027A0);

static int sunplus_otprx_read(int addr, char *value)
{
    unsigned int data;
	
	regs->otp_cmd_status = 0x0;
	regs->otp_addr = addr;
	regs->otp_cmd = 0x1E04;

	do
    {
        data = regs->otp_cmd_status;
    } while((data & OTP_READ_DONE) != OTP_READ_DONE);

	*value = otp_data->hb_gpio_rgst_bus32_9 & 0xFF;

	return 0;
}

static int sunplus_otprx_write(int addr, char value)
{
	unsigned int data;
	
    regs->otp_ctrl = 0xFD01;
	regs->otp_prog_addr = addr;
	regs->otp_prog_ctl = 0x03;

	data = (value << 8) + 0x12;
	regs->otp_prog_reg25= data;

	regs->otp_prog_wr = 0x01;
	regs->otp_prog_pgenb = 0x00;

	do
    {
        data = regs->otp_prog_state;
	} while((data & 0x1F) != 0x13);

	regs->otp_prog_pgenb =0x01;
	regs->otp_prog_wr = 0x00;
	regs->otp_rsv = 0x00;

	return 0;
}