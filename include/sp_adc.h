#ifndef DRV_ADC_H
#define DRV_ADC_H

#include <config.h>
#include <regmap.h>
#include <types.h>

void sp_adc_read(unsigned int adc_chan, unsigned int *adc_buf);

#endif
