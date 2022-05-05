#include <regmap.h>
#include <common.h>

#define R_GRP16(x) ((x)>>3)
#define R_GRP32(x) ((x)>>4)

#define G_POS16(x) ((x) & 0x07)
#define G_POS32(x) ((x) & 0x0F)
#define G_SHF16(x) (1<<G_POS16(x))
#define G_SHF32(x) (1<<G_POS32(x))

// set PIN _p to GPIO
// IN(_iso=0) or out(_iso=1)
// _out is output value for OUT pin
void gpio_set_IO( uint8_t _p, uint8_t _iso, uint8_t _out) {
 // gpio_first(1)
 PAD_CTL_REG->gpio_first[ R_GRP32( _p)] |= ( G_SHF32( _p));
 // gpio(1) or IOP(0)
 REG_GPIOXT->ctl_sel[ R_GRP16( _p)] |= RF_MASK_V_SET( G_SHF16( _p));
 // in or out
 REG_GPIOXT->oe[ R_GRP16( _p)] |= RF_MASK_V( G_SHF16( _p), _iso << G_POS16( _p));
 if ( !_iso) return;
 // set output value
 REG_GPIOXT->o[ R_GRP16( _p)] |= RF_MASK_V( G_SHF16( _p), _out << G_POS16( _p));
 return;  }

// get gpio value
uint8_t gpio_getV( uint8_t _pin) {
 if ( ( REG_GPIOXT->in[ R_GRP32( _pin)] & G_SHF32( _pin))) return( 0);
 return( 1);  }
