#include <common.h>
#include <config.h>
#include <types.h>
#include <regmap.h>

#define SP_ADC_CLK_DIV_MASK		0x700
#define SP_ADC_DATA_MASK		0xFFF

#define SP_ADC_EN			0x02
#define SP_ADC_SRFS			0x04
#define SP_ADC_DATA_READY		0x8000  
#define SP_ADC_CHAN_SET			0x688  //  011010001000 = 0x688

void sp_adc_read(unsigned int adc_chan, unsigned int *adc_buf)
{
        unsigned int temp_reg;			
        unsigned int temp_buf;	
	u32 stat;
	volatile struct dw_adc_regs *adc_regs = ADC_REG_AO;

	temp_reg = adc_regs->adc_cfg0b;
	temp_reg |= SP_ADC_EN;
	adc_regs->adc_cfg0b = temp_reg;		// adc enable

	temp_reg = adc_regs->adc_cfg02;
	temp_reg &= ~SP_ADC_CLK_DIV_MASK;
	adc_regs->adc_cfg02 = temp_reg;		// set clk div = 0

	adc_regs->adc_cfg0e = SP_ADC_CHAN_SET; 	// set adc chan

	temp_reg = adc_regs->adc_cfg0b;
	temp_reg |= SP_ADC_SRFS;
	adc_regs->adc_cfg0b = temp_reg;		// adc start

	stat =  adc_regs->adc_cfg0d;

	while((stat & SP_ADC_DATA_READY) != SP_ADC_DATA_READY)
	{
		stat =  adc_regs->adc_cfg0d;
	}

	switch (adc_chan) {
			case 0:
				temp_buf = adc_regs->adc_cfg11;
				temp_buf &= SP_ADC_DATA_MASK;
				*adc_buf = temp_buf;
			break;
			case 1:
				temp_buf = adc_regs->adc_cfg12;
				temp_buf &= SP_ADC_DATA_MASK;
				*adc_buf = temp_buf;
			break;
			case 2:
				temp_buf = adc_regs->adc_cfg13;
				temp_buf &= SP_ADC_DATA_MASK;
				*adc_buf = temp_buf;
			break;
			case 3:
				temp_buf = adc_regs->adc_cfg14;
				temp_buf &= SP_ADC_DATA_MASK;
				*adc_buf = temp_buf;
			break;

	}
	temp_reg = adc_regs->adc_cfg0b;
	temp_reg &= ~SP_ADC_EN;
	adc_regs->adc_cfg0b = temp_reg;		// sdc enable
}


