#include <common.h>
#include <sp_adc.h>

#define SP_ADC_CLK_DIV_MASK		0x700
#define SP_ADC_DATA_MASK		0xFFF
#define SP_ADC_CHEN_MASK		0x38	//bit3 ~ bit5

#define SP_ADC_EN			0x02
#define SP_ADC_SRFS			0x04
#define SP_ADC_BYPASS			0x20
#define SP_ADC_DATA_READY		0x8000  
#define SP_ADC_CHAN_SET			0x688  //  011010001000 = 0x688
#define SP_ADC_CHAN0_SET		0x00
#define SP_ADC_CHAN1_SET		0x08
#define SP_ADC_CHAN2_SET		0x10
#define SP_ADC_CHAN3_SET		0x18

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

	temp_reg = adc_regs->adc_cfg0b;
	temp_reg &= ~SP_ADC_CHEN_MASK;

	//adc_regs->adc_cfg0e = SP_ADC_CHAN_SET; 	// set adc chan
	switch (adc_chan) {
			case 0:
				temp_reg |= SP_ADC_CHAN0_SET;
			break;
			case 1:
				temp_reg |= SP_ADC_CHAN1_SET;
			break;
			case 2:
				temp_reg |= SP_ADC_CHAN2_SET;
			break;
			case 3:
				temp_reg |= SP_ADC_CHAN3_SET;
			break;
	}

	temp_reg &= ~SP_ADC_SRFS;
	adc_regs->adc_cfg0b = temp_reg; 	// adc start
	delay_1ms(1);
	temp_reg |= SP_ADC_SRFS;
	adc_regs->adc_cfg0b = temp_reg;		// adc start
	delay_1ms(1);

	temp_reg = adc_regs->adc_cfg02;
	temp_reg |=  SP_ADC_BYPASS;
	adc_regs->adc_cfg02 = temp_reg;

	stat =  adc_regs->adc_cfg0d;
	while((stat & SP_ADC_DATA_READY) != SP_ADC_DATA_READY)
	{
		stat =  adc_regs->adc_cfg0d;
	}

	temp_buf = adc_regs->adc_cfg0d;
	temp_buf &= SP_ADC_DATA_MASK;
	*adc_buf = temp_buf;

	//temp_reg = adc_regs->adc_cfg0b;
	//temp_reg &= ~SP_ADC_EN;
	//adc_regs->adc_cfg0b = temp_reg;		// sdc disable
}
