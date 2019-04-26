#ifndef _SP_GPIO_H_
#define _SP_GPIO_H_

#include <common.h>

extern int gpio_first_1(u32 bit);
extern int gpio_first_0(u32 bit);
extern u32 gpio_first_val_get(u32 bit);
extern int gpio_master_1(u32 bit);
extern int gpio_master_0(u32 bit);
extern int gpio_set_oe(u32 bit);
extern int gpio_clr_oe(u32 bit);
extern int gpio_out_1(u32 bit);
extern int gpio_out_0(u32 bit);

#define GPIO_F_SET(a,d) do { \
                            if(d) { \
                                gpio_first_1(a); \
                            } else { \
                                gpio_first_0(a); \
                            } \
                        } while(0)
#define GPIO_F_GET(a)	gpio_first_val_get(a)

#define GPIO_M_SET(a,d) do { \
                            if(d) { \
                                gpio_master_1(a); \
                            } else { \
                                gpio_master_0(a); \
                            } \
                        } while(0)
#define GPIO_M_GET(a)	gpio_master_val_get(a)					

#define GPIO_E_SET(a,d) do { \
                            if(d) { \
                                gpio_set_oe(a); \
                            } else { \
                                gpio_clr_oe(a); \
                            } \
                        } while(0)
#define GPIO_E_GET(a)	gpio_oe_val_get(a)

#define GPIO_O_SET(a,d) do { \
                            if(d) { \
                                gpio_out_1(a); \
                            } else { \
                                gpio_out_0(a); \
                            } \
                        } while(0)
#define GPIO_O_GET(a)	gpio_out_val_get(a)


#endif /* _SP_GPIO_H_ */

