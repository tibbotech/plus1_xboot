#ifndef __INC_REGMAP_H
#define __INC_REGMAP_H

#include <config.h>

#ifdef PLATFORM_I143  
#include <regmap_i143.h>
#else
#include <regmap_q628.h>
#endif

#endif /* __INC_REGMAP_H */
