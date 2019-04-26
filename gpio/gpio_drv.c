#include <common.h>
#include <types.h>

#include <gpio_drv.h>

#define REG_BASE           0x9c000000
#define RF_GRP(_grp, _reg) ((((_grp) * 32 + (_reg)) * 4) + REG_BASE)

#define GPIO_FIRST(X)   (RF_GRP(101, (25+X)))
#define GPIO_MASTER(X)  (RF_GRP(6, (0+X)))
#define GPIO_OE(X)      (RF_GRP(6, (8+X)))
#define GPIO_OUT(X)     (RF_GRP(6, (16+X)))
#define GPIO_IN(X)      (RF_GRP(6, (24+X)))
#define GPIO_I_INV(X)   (RF_GRP(7, (0+X)))
#define GPIO_O_INV(X)   (RF_GRP(7, (8+X)))
#define GPIO_OD(X)      (RF_GRP(7, (16+X)))


int gpio_first_1(u32 bit)
{
	u32 idx, value, reg_val;

	idx = bit >> 5;
	if (idx > 4) {
		return -1;
	}
	
	value = 1 << (bit & 0x1f);

	reg_val = *((volatile unsigned int *)(GPIO_FIRST(idx)));
	reg_val |= value;
	*((volatile unsigned int *) (GPIO_FIRST(idx))) = reg_val;	

	return 0;
}

int gpio_first_0(u32 bit)
{
	u32 idx, value, reg_val;

	idx = bit >> 5;
	if (idx > 4) {
		return -1;
	}

	value = 1 << (bit & 0x1f);

	reg_val = *((volatile unsigned int *)(GPIO_FIRST(idx)));
	reg_val &= (~value);
	*((volatile unsigned int *) (GPIO_FIRST(idx))) = reg_val;

	return 0;
}


int gpio_first_value_get(u32 bit, u32 *gpio_first_value)
{
	u32 idx, value, reg_val;
	
	idx = bit >> 5;
	if (idx > 5) {
		return -1;
	}
	
	value = 1 << (bit & 0x1f);
	
	reg_val = *((volatile unsigned int *)(GPIO_FIRST(idx)));
	*gpio_first_value =  (reg_val & value) ? 1 : 0;
		
	return 0;
}


u32 gpio_first_val_get(u32 bit)
{
	u32 value = 0;

	gpio_first_value_get(bit, &value);

	return value;
}


int gpio_master_1(u32 bit)
{
	u32 idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	value = (1 << (bit & 0x0f) | 1 << ((bit & 0x0f) + 0x10));

	reg_val = *((volatile unsigned int *)(GPIO_MASTER(idx)));
	reg_val |= value;
	*((volatile unsigned int *) (GPIO_MASTER(idx))) = reg_val;	

	return 0;
}


int gpio_master_0(u32 bit)
{
	u32 idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	reg_val = *((volatile unsigned int *)(GPIO_MASTER(idx)));
	value = ((reg_val | (1 << ((bit & 0x0f) + 0x10)) ) & ~( 1 << (bit & 0x0f)));
	*((volatile unsigned int *) (GPIO_MASTER(idx))) = value;

	return 0;
}

int gpio_master_value_get(u32 bit, u32 *gpio_master_value)
{
	u32 idx, value, reg_val;
	
	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}
	
	value = 1 << (bit & 0x0f);
	
	reg_val = *((volatile unsigned int *)(GPIO_MASTER(idx)));
	*gpio_master_value =  (reg_val & value) ? 1 : 0;
		
	return 0;
}

u32 gpio_master_val_get(u32 bit)
{
	u32 value = 0;

	gpio_master_value_get(bit, &value);

	return value;
}

int gpio_set_oe(u32 bit)
{
	u32 idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	value = (1 << (bit & 0x0f) |  1 << ((bit & 0x0f) + 0x10));

	reg_val = *((volatile unsigned int *)(GPIO_OE(idx)));
	reg_val |= value;
	*((volatile unsigned int *) (GPIO_OE(idx))) = reg_val;

	return 0;
}

int gpio_clr_oe(u32 bit)
{
	u32 idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}
	
	reg_val = *((volatile unsigned int *)(GPIO_OE(idx)));
	value = ((reg_val | (1 << ((bit & 0x0f) + 0x10)) ) & ~( 1 << (bit & 0x0f)));
	*((volatile unsigned int *) (GPIO_OE(idx))) = value;

	return 0;
}

int gpio_oe_value_get(u32 bit, u32 *gpio_out_enable_value)
{
	u32 idx, value, reg_val;
	
	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}
	
	value = 1 << (bit & 0x0f);
	
	reg_val = *((volatile unsigned int *)(GPIO_OE(idx)));
	*gpio_out_enable_value =  (reg_val & value) ? 1 : 0;
		
	return 0;
}

u32 gpio_oe_val_get(u32 bit)
{
	u32 value = 0;

	gpio_oe_value_get(bit, &value);

	return value;
}

int gpio_out_1(u32 bit)
{
	u32 idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	value = (1 << (bit & 0x0f) | 1 << ((bit & 0x0f) + 0x10));

	reg_val = *((volatile unsigned int *)(GPIO_OUT(idx)));
	reg_val |= value;
	*((volatile unsigned int *) (GPIO_OUT(idx))) = reg_val;

	return 0;
}

int gpio_out_0(u32 bit)
{
	u32 idx, value, reg_val;

	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}

	reg_val = *((volatile unsigned int *)(GPIO_OUT(idx)));
	value = (( reg_val | (1 << ((bit & 0x0f) + 0x10)) ) & ~( 1 << (bit & 0x0f)));
	*((volatile unsigned int *) (GPIO_OUT(idx))) = value;

	return 0;
}

int gpio_out_value_get(u32 bit, u32 *gpio_out_value)
{
	u32 idx, value, reg_val;
	
	idx = bit >> 4;
	if (idx > 8) {
		return -1;
	}
	
	value = 1 << (bit & 0x0f);
	
	reg_val = *((volatile unsigned int *)(GPIO_OUT(idx)));
	*gpio_out_value =  (reg_val & value) ? 1 : 0;
		
	return 0;
}

u32 gpio_out_val_get(u32 bit)
{
	u32 value = 0;

	gpio_out_value_get(bit, &value);

	return value;
}

