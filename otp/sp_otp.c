#include <common.h>
#include <otp/sp_otp.h>

static volatile struct otprx_sunplus *regs = (volatile struct otprx_sunplus *)(OTPRX_BASE_ADR);
static volatile struct hbgpio_sunplus *otp_data = (volatile struct hbgpio_sunplus *)(HB_GPIO);

int sunplus_otprx_read(int addr, char *value)
{
    unsigned int addr_data;
    unsigned int byte_shift;	
    unsigned int status;
    u32 timeout = OTP_READ_TIMEOUT;   
    
	addr_data = addr % (QAC628_OTP_WORD_SIZE * QAC628_OTP_WORDS_PER_BANK);
	addr_data = addr_data / QAC628_OTP_WORD_SIZE;
	
	byte_shift = addr % (QAC628_OTP_WORD_SIZE * QAC628_OTP_WORDS_PER_BANK);
	byte_shift = byte_shift % QAC628_OTP_WORD_SIZE;
	
	regs->otp_cmd_status = 0x0;
	
	addr = addr / (QAC628_OTP_WORD_SIZE * QAC628_OTP_WORDS_PER_BANK);
	addr = addr * QAC628_OTP_BIT_ADDR_OF_BANK;
	regs->otp_addr = addr;
	
	regs->otp_cmd = 0x1E04;

	do
    {
        delay_1ms(1);
		if (timeout-- == 0)
		    return -1;
		                	
        status = regs->otp_cmd_status;
    } while((status & OTP_READ_DONE) != OTP_READ_DONE);

	*value = (otp_data->hb_gpio_rgst_bus32[8+addr_data] >> (8 * byte_shift)) & 0xFF;

	return 0;
}

int sunplus_otprx_write(int addr, char value)
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