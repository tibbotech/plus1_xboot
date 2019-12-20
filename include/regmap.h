#ifndef __INC_REGMAP_H
#define __INC_REGMAP_H

#include <config.h>

#ifdef PLATFORM_8388  // use 8388 regmap
#include <regmap_gemini.h>
#elif defined(PLATFORM_I137)
#include <regmap_i137.h>
#elif defined(PLATFORM_3502)
#include <regmap_3502.h>
#else
#include <regmap_i143.h>
#endif

#endif /* __INC_REGMAP_H */
