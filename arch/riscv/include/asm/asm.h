/*
 * Copyright (C) 2015 Regents of the University of California
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 */

#ifndef _ASM_RISCV_ASM_H
#define _ASM_RISCV_ASM_H

#ifdef __ASSEMBLY__
#define __ASM_STR(x)	x
#else
#define __ASM_STR(x)	#x
#endif

#if __riscv_xlen == 64
#define CONFIG_64BIT
#define REG_L		ld
#define REG_S		sd
#define SZREG		8
#define LGREG		3
#elif __riscv_xlen == 32
#define CONFIG_32BIT
#define REG_L		lw
#define REG_S		sw
#define SZREG		4
#define LGREG		2
#else
#error "Unexpected __riscv_xlen"
#endif


#endif /* _ASM_RISCV_ASM_H */
