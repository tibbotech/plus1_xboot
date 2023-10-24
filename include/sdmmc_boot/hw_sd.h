#include "config.h"
#include "common.h"

#ifdef PLATFORM_I143
#include "hw_sd_143.h"
#elif defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#include "hw_sd_645.h"
#else
#include "hw_sd_628.h"
#endif