#ifndef __HAL_GPIO_H__
#define __HAL_GPIO_H__

// gpio first : 0(disable), 1(enable)
static inline void HAL_GPIO_F_SET(unsigned int a, unsigned int d)
{
	if (d)
		GPIO_FIRST_REG->gpio_first[a / 32] |=  1 << (a % 32);
	else
		GPIO_FIRST_REG->gpio_first[a / 32] &= ~(1 << (a % 32));
}

// gpio control source selection : 0(IOP), 1(GPIO)
static inline void HAL_GPIO_M_SET(unsigned int a, unsigned int d)
{
	GPIO_MASTER_REG->gpio_master[a / 16] = (d ? 0x10001 : 0x10000) << (a % 16);
}

// gpio output enable : 0(input), 1(output)
static inline void HAL_GPIO_E_SET(unsigned int a, unsigned int d)
{
	GPIO_OE_REG->gpio_oe[a / 16] = (d ? 0x10001 : 0x10000) << (a % 16);
}

// gpio output data
static inline void HAL_GPIO_O_SET(unsigned int a, unsigned int d)
{
	GPIO_OUT_REG->gpio_out[a / 16] = (d ? 0x10001 : 0x10000) << (a % 16);
}

// gpio input data
static inline unsigned int HAL_GPIO_I_GET(unsigned int a)
{
	unsigned int d = (GPIO_IN_REG->gpio_in[a / 32] >> (a % 32)) & 0x1;
	return d;
}

// set gpio to GPO and output
static inline void HAL_GPIO_GPO(unsigned int a, unsigned int d)
{
	HAL_GPIO_M_SET(a, 1); // master=1
	HAL_GPIO_O_SET(a, d); // output data
	HAL_GPIO_E_SET(a, 1); // output enable=1
	HAL_GPIO_F_SET(a, 1); // gpio first=1
}

// set gpio to GPI
static inline void HAL_GPIO_GPI(unsigned int a)
{
	HAL_GPIO_M_SET(a, 1); // master=1
	HAL_GPIO_E_SET(a, 0); // input mode
	HAL_GPIO_F_SET(a, 1); // gpio first=1
}

// disable gpio mode,set pin to default value
static inline void HAL_GPIO_RESET(unsigned int a)
{
	HAL_GPIO_M_SET(a, 1); // master=1
	HAL_GPIO_E_SET(a, 0); // input mode
	HAL_GPIO_F_SET(a, 0); // gpio first=0
}
#endif
