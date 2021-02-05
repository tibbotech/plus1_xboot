#ifndef __INC_ARM_H__
#define __INC_ARM_H__

#define ARM_MODE_MASK       0x1F  // CPSR[4:0] = Mode
#define ARM_MODE_ABORT      0x17
#define ARM_MODE_FIQ        0x11
#define ARM_MODE_IRQ        0x12
#define ARM_MODE_SVC        0x13
#define ARM_MODE_SYSTEM     0x1F
#define ARM_MODE_UNDEFINED  0x1B
#define ARM_MODE_USER       0x10
#define ARM_MODE_MONITOR    0x16

#endif
