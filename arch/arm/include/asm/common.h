#include <config.h>

//#define RF_GRP(group, reg)  ((4 * (group * 32 + reg)) + REG_BASE)

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
