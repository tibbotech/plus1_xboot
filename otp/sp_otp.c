#include <common.h>
#include <otp/sp_otp.h>

static volatile struct otprx_sunplus *regs = (volatile struct otprx_sunplus *)(OTPRX_BASE_ADR);
static volatile struct hbgpio_sunplus *otp_data = (volatile struct hbgpio_sunplus *)(HB_GPIO);

int sunplus_otprx_read(int addr, char *value)
{
  unsigned int addr_data;
  unsigned int byte_shift;	
  unsigned int data;
  u32 timeout = OTP_WAIT_MICRO_SECONDS;   
    
  addr_data = (addr % 16) / 4;
  byte_shift = ((addr % 16) % 4);
	
	regs->otp_cmd_status = 0x0;
	
  addr = (addr / 16) * 4 * 32;
	regs->otp_addr = addr;
	
	regs->otp_cmd = 0x1E04;

	do
    {
        delay_1ms(1);
		if (timeout-- == 0)
		    return -1;
		                	
        data = regs->otp_cmd_status;
    } while((data & OTP_READ_DONE) != OTP_READ_DONE);

	*value = (otp_data->hb_gpio_rgst_bus32[8+addr_data] >> (8 * byte_shift)) & 0xFF;

	return 0;
}

int sunplus_otprx_write(int addr, char value)
{
	unsigned int data;
	u32 timeout = OTP_WAIT_MICRO_SECONDS;
		
    regs->otp_ctrl = 0xFD01;
	regs->otp_prog_addr = addr;
	regs->otp_prog_ctl = 0x03;

	data = value;
	data = (data << 8) + 0x12;
	regs->otp_prog_reg25= data;

	regs->otp_prog_wr = 0x01;
	regs->otp_prog_pgenb = 0x00;

	do
    {
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