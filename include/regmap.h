#ifndef __INC_REGMAP_H
#define __INC_REGMAP_H

#include <config.h>

#if defined(PLATFORM_Q628)
#include <regmap_q628.h>
#elif defined(PLATFORM_Q645)
#include <regmap_q645.h>
#elif defined(PLATFORM_Q654)
#include <regmap_q654.h>
#endif

#endif /* __INC_REGMAP_H */
