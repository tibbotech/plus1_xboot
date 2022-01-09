#include <config.h>

/* ARM */
#ifdef __arm__
#define REGS0(addr, value) \
  ldr r0 , =value    ;     \
  ldr r1 , =addr     ;     \
  str r0 , [r1, #0]  ;

#define LDREG(addr, reg) \
  ldr r1 , =addr    ;    \
  ldr reg, [r1, #0] ;

#define STREG(addr, reg) \
  ldr r1 , =addr    ;    \
  str reg, [r1, #0] ;

#else
#error "Not support"
#endif
