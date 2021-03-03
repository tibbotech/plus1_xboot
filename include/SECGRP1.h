//==========================================================================
//    File      :  SECGRP1.h
//    Author    :  lumi.yang
//    Date      :  2020-07-16
//    Abstract  :  This is a base of SECGRP1_* and NORGRP1_*;
//==========================================================================
//==========================================================================
//    Modification History:
//	Version 		Author 			Description
//    version 0.0	lumi.yang@2020.07.16	
//==========================================================================
#ifndef __INC_SECGRP1_H
#define __INC_SECGRP1_H

#include <common.h>



//=======================================================
//
//	Configure Register Macro Function
//
//=======================================================


struct SECGRP1_MAIN_REGS {
    UINT32  G083_CA55_S01                         ; 
    UINT32  G083_CA55_S02                         ; 
    UINT32  G083_CA55_S03                         ; 
    UINT32  G083_CA55_S04                         ; 
    UINT32  G083_reserved_04                      ; 
    UINT32  G083_reserved_05                      ; 
    UINT32  G083_reserved_06                      ; 
    UINT32  G083_reserved_07                      ; 
    UINT32  G083_CA55_S09                         ; 
    UINT32  G083_reserved_09                      ; 
    UINT32  G083_reserved_10                      ; 
    UINT32  G083_reserved_11                      ; 
    UINT32  G083_reserved_12                      ; 
    UINT32  G083_SDPROT_S01                       ; 
    UINT32  G083_SDPROT_S02                       ; 
    UINT32  G083_SDPROT_S03                       ; 
    UINT32  G083_SDPROT_S04                       ; 
    UINT32  G083_reserved_17                      ; 
    UINT32  G083_reserved_18                      ; 
    UINT32  G083_reserved_19                      ; 
    UINT32  G083_reserved_20                      ; 
    UINT32  G083_reserved_21                      ; 
    UINT32  G083_reserved_22                      ; 
    UINT32  G083_reserved_23                      ; 
    UINT32  G083_reserved_24                      ; 
    UINT32  G083_reserved_25                      ; 
    UINT32  G083_NIC_S01                          ; 
    UINT32  G083_NIC_S02                          ; 
    UINT32  G083_NIC_S03                          ; 
    UINT32  G083_NIC_S04                          ; 
    UINT32  G083_NIC_S05                          ; 
    UINT32  G083_reserved_31                      ; 
};

#define SECGRP1_MAIN_REG    ((volatile struct SECGRP1_MAIN_REGS *)RF_GRP(SECGRP1_MAIN_RGST_NUM, 0))

struct SECGRP1_QII_REGS {
    UINT32  G084_reserved_00                      ; 
    UINT32  G084_reserved_01                      ; 
    UINT32  G084_reserved_02                      ; 
    UINT32  G084_reserved_03                      ; 
    UINT32  G084_reserved_04                      ; 
    UINT32  G084_reserved_05                      ; 
    UINT32  G084_reserved_06                      ; 
    UINT32  G084_reserved_07                      ; 
    UINT32  G084_reserved_08                      ; 
    UINT32  G084_reserved_09                      ; 
    UINT32  G084_reserved_10                      ; 
    UINT32  G084_reserved_11                      ; 
    UINT32  G084_reserved_12                      ; 
    UINT32  G084_reserved_13                      ; 
    UINT32  G084_reserved_14                      ; 
    UINT32  G084_reserved_15                      ; 
    UINT32  G084_reserved_16                      ; 
    UINT32  G084_reserved_17                      ; 
    UINT32  G084_reserved_18                      ; 
    UINT32  G084_reserved_19                      ; 
    UINT32  G084_reserved_20                      ; 
    UINT32  G084_reserved_21                      ; 
    UINT32  G084_reserved_22                      ; 
    UINT32  G084_reserved_23                      ; 
    UINT32  G084_reserved_24                      ; 
    UINT32  G084_reserved_25                      ; 
    UINT32  G084_NIC_S01                          ; 
    UINT32  G084_NIC_S02                          ; 
    UINT32  G084_NIC_S03                          ; 
    UINT32  G084_NIC_S04                          ; 
    UINT32  G084_NIC_S05                          ; 
    UINT32  G084_reserved_31                      ; 
};

#define SECGRP1_QII_REG    ((volatile struct SECGRP1_QII_REGS *)RF_GRP(SECGRP1_QII_RGST_NUM, 0))

struct SECGRP1_QIII_REGS {
    UINT32  G085_reserved_00                      ; 
    UINT32  G085_reserved_01                      ; 
    UINT32  G085_reserved_02                      ; 
    UINT32  G085_reserved_03                      ; 
    UINT32  G085_reserved_04                      ; 
    UINT32  G085_reserved_05                      ; 
    UINT32  G085_reserved_06                      ; 
    UINT32  G085_reserved_07                      ; 
    UINT32  G085_reserved_08                      ; 
    UINT32  G085_CBDMA0_S01                       ; 
    UINT32  G085_CBDMA0_S02                       ; 
    UINT32  G085_reserved_11                      ; 
    UINT32  G085_reserved_12                      ; 
    UINT32  G085_reserved_13                      ; 
    UINT32  G085_reserved_14                      ; 
    UINT32  G085_reserved_15                      ; 
    UINT32  G085_reserved_16                      ; 
    UINT32  G085_reserved_17                      ; 
    UINT32  G085_reserved_18                      ; 
    UINT32  G085_reserved_19                      ; 
    UINT32  G085_reserved_20                      ; 
    UINT32  G085_reserved_21                      ; 
    UINT32  G085_reserved_22                      ; 
    UINT32  G085_reserved_23                      ; 
    UINT32  G085_reserved_24                      ; 
    UINT32  G085_reserved_25                      ; 
    UINT32  G085_NIC_S01                          ; 
    UINT32  G085_NIC_S02                          ; 
    UINT32  G085_NIC_S03                          ; 
    UINT32  G085_NIC_S04                          ; 
    UINT32  G085_NIC_S05                          ; 
    UINT32  G085_reserved_31                      ; 
};

#define SECGRP1_QIII_REG    ((volatile struct SECGRP1_QIII_REGS *)RF_GRP(SECGRP1_QIII_RGST_NUM, 0))

struct SECGRP1_QIV_REGS {
    UINT32  G085_reserved_00                      ; 
    UINT32  G085_reserved_01                      ; 
    UINT32  G085_reserved_02                      ; 
    UINT32  G085_reserved_03                      ; 
    UINT32  G085_reserved_04                      ; 
    UINT32  G085_reserved_05                      ; 
    UINT32  G085_reserved_06                      ; 
    UINT32  G085_reserved_07                      ; 
    UINT32  G085_reserved_08                      ; 
    UINT32  G085_reserved_09                      ; 
    UINT32  G085_reserved_10                      ; 
    UINT32  G085_reserved_11                      ; 
    UINT32  G085_reserved_12                      ; 
    UINT32  G085_reserved_13                      ; 
    UINT32  G085_reserved_14                      ; 
    UINT32  G085_reserved_15                      ; 
    UINT32  G085_reserved_16                      ; 
    UINT32  G085_reserved_17                      ; 
    UINT32  G085_reserved_18                      ; 
    UINT32  G085_reserved_19                      ; 
    UINT32  G085_reserved_20                      ; 
    UINT32  G085_reserved_21                      ; 
    UINT32  G085_reserved_22                      ; 
    UINT32  G085_reserved_23                      ; 
    UINT32  G085_reserved_24                      ; 
    UINT32  G085_reserved_25                      ;
    UINT32  G086_NIC_S01                          ; 
    UINT32  G086_NIC_S02                          ; 
    UINT32  G086_NIC_S03                          ; 
    UINT32  G086_NIC_S04                          ; 
    UINT32  G086_NIC_S05                          ; 
    UINT32  G086_reserved_31                      ; 
};

#define SECGRP1_QIV_REG    ((volatile struct SECGRP1_QIV_REGS *)RF_GRP(SECGRP1_QIV_RGST_NUM, 0))

struct SECGRP1_VIDEOIN_REGS {
    UINT32  G114_reserved_00                      ; 
    UINT32  G114_reserved_01                      ; 
    UINT32  G114_reserved_02                      ; 
    UINT32  G114_reserved_03                      ; 
    UINT32  G114_reserved_04                      ; 
    UINT32  G114_reserved_05                      ; 
    UINT32  G114_reserved_06                      ; 
    UINT32  G114_reserved_07                      ; 
    UINT32  G114_reserved_08                      ; 
    UINT32  G114_reserved_09                      ; 
    UINT32  G114_reserved_10                      ; 
    UINT32  G114_reserved_11                      ; 
    UINT32  G114_reserved_12                      ; 
    UINT32  G114_reserved_13                      ; 
    UINT32  G114_reserved_14                      ; 
    UINT32  G114_reserved_15                      ; 
    UINT32  G114_reserved_16                      ; 
    UINT32  G114_reserved_17                      ; 
    UINT32  G114_reserved_18                      ; 
    UINT32  G114_reserved_19                      ; 
    UINT32  G114_reserved_20                      ; 
    UINT32  G114_reserved_21                      ; 
    UINT32  G114_reserved_22                      ; 
    UINT32  G114_reserved_23                      ; 
    UINT32  G114_reserved_24                      ; 
    UINT32  G114_reserved_25                      ; 
    UINT32  G114_NIC_S01                          ; 
    UINT32  G114_NIC_S02                          ; 
    UINT32  G114_NIC_S03                          ; 
    UINT32  G114_reserved_29                      ; 
    UINT32  G114_reserved_30                      ; 
    UINT32  G114_reserved_31                      ; 
};

#define SECGRP1_VIDEOIN_REG    ((volatile struct SECGRP1_VIDEOIN_REGS *)RF_GRP(SECGRP1_VIDEOIN_RGST_NUM, 0))

struct SECGRP1_DISP_REGS {
    UINT32  G113_reserved_00                      ; 
    UINT32  G113_reserved_01                      ; 
    UINT32  G113_reserved_02                      ; 
    UINT32  G113_reserved_03                      ; 
    UINT32  G113_reserved_04                      ; 
    UINT32  G113_reserved_05                      ; 
    UINT32  G113_reserved_06                      ; 
    UINT32  G113_reserved_07                      ; 
    UINT32  G113_reserved_08                      ; 
    UINT32  G113_reserved_09                      ; 
    UINT32  G113_reserved_10                      ; 
    UINT32  G113_reserved_11                      ; 
    UINT32  G113_reserved_12                      ; 
    UINT32  G113_reserved_13                      ; 
    UINT32  G113_reserved_14                      ; 
    UINT32  G113_reserved_15                      ; 
    UINT32  G113_reserved_16                      ; 
    UINT32  G113_reserved_17                      ; 
    UINT32  G113_reserved_18                      ; 
    UINT32  G113_reserved_19                      ; 
    UINT32  G113_reserved_20                      ; 
    UINT32  G113_reserved_21                      ; 
    UINT32  G113_reserved_22                      ; 
    UINT32  G113_reserved_23                      ; 
    UINT32  G113_reserved_24                      ; 
    UINT32  G113_reserved_25                      ; 
    UINT32  G113_NIC_S01                          ; 
    UINT32  G113_NIC_S02                          ; 
    UINT32  G113_reserved_28                      ; 
    UINT32  G113_reserved_29                      ; 
    UINT32  G113_reserved_30                      ; 
    UINT32  G113_reserved_31                      ; 
};

#define SECGRP1_DISP_REG    ((volatile struct SECGRP1_DISP_REGS *)RF_GRP(SECGRP1_DISP_RGST_NUM, 0))

struct NORGRP1_QIII_REGS {
    UINT32  G082_reserved_00                      ; 
    UINT32  G082_reserved_01                      ; 
    UINT32  G082_reserved_02                      ; 
    UINT32  G082_reserved_03                      ; 
    UINT32  G082_reserved_04                      ; 
    UINT32  G082_reserved_05                      ; 
    UINT32  G082_reserved_06                      ; 
    UINT32  G082_reserved_07                      ; 
    UINT32  G082_reserved_08                      ; 
    UINT32  G082_reserved_09                      ; 
    UINT32  G082_reserved_10                      ; 
    UINT32  G082_reserved_11                      ; 
    UINT32  G082_reserved_12                      ; 
    UINT32  G082_reserved_13                      ; 
    UINT32  G082_reserved_14                      ; 
    UINT32  G082_reserved_15                      ; 
    UINT32  G082_reserved_16                      ; 
    UINT32  G082_reserved_17                      ; 
    UINT32  G082_reserved_18                      ; 
    UINT32  G082_reserved_19                      ; 
    UINT32  G082_reserved_20                      ; 
    UINT32  G082_reserved_21                      ; 
    UINT32  G082_reserved_22                      ; 
    UINT32  G082_reserved_23                      ; 
    UINT32  G082_reserved_24                      ; 
    UINT32  G082_reserved_25                      ; 
    UINT32  G082_reserved_26                      ; 
    UINT32  G082_reserved_27                      ; 
    UINT32  G082_reserved_28                      ; 
    UINT32  G082_reserved_29                      ; 
    UINT32  G082_reserved_30                      ; 
    UINT32  G082_reserved_31                      ; 
};

#define NORGRP1_QIII_REG    ((volatile struct NORGRP1_QIII_REGS *)RF_GRP(NORGRP1_QIII_RGST_NUM, 0))



//#define CMD_NO_CHECK	
//#define NO_PRINT_MSG
#define QUI642_SECGRP1 

#ifdef QUI642_SECGRP1

	#ifndef NORGRP1_QIII_RGST_NUM
	#define NORGRP1_QIII_RGST_NUM 		(82) 	// Set RGST group number.
	#endif

	#ifndef SECGRP1_MAIN_RGST_NUM
	#define SECGRP1_MAIN_RGST_NUM 		(83) 	// Set RGST group number.
	#endif

	#ifndef SECGRP1_QII_RGST_NUM
	#define SECGRP1_QII_RGST_NUM 		(84) 	// Set RGST group number.
	#endif

	#ifndef SECGRP1_QIII_RGST_NUM
	#define SECGRP1_QIII_RGST_NUM 		(85) 	// Set RGST group number.
	#endif

	#ifndef SECGRP1_QIV_RGST_NUM
	#define SECGRP1_QIV_RGST_NUM 		(86) 	// Set RGST group number.
	#endif

	#ifndef SECGRP1_DISP_RGST_NUM
	#define SECGRP1_DISP_RGST_NUM 		(113) 	// Set RGST group number.
	#endif

	#ifndef SECGRP1_VIDEOIN_RGST_NUM
	#define SECGRP1_VIDEOIN_RGST_NUM 	(114) 	// Set RGST group number.
	#endif
	
	#ifndef CA55_M0
	#define CA55_M0			(0x000)
	#endif

	#ifndef DSPC_MA
	#define DSPC_MA			(0x001)
	#endif

	#ifndef DSPD_MA
	#define DSPD_MA			(0x002)
	#endif

	#ifndef GC8000L_MA0
	#define GC8000L_MA0		(0x003)
	#endif

	#ifndef GC8000L_MA1
	#define GC8000L_MA1		(0x004)
	#endif

	#ifndef DEC400DE_MA
	#define DEC400DE_MA		(0x005)
	#endif

	#ifndef DEC400D_MA
	#define DEC400D_MA		(0x006)
	#endif

	#ifndef VCD_MA
	#define VCD_MA			(0x007)
	#endif

	#ifndef VCE_MA
	#define VCE_MA			(0x008)
	#endif

	#ifndef USBC0_MA
	#define USBC0_MA		(0x009)
	#endif

	#ifndef USBC1_MA
	#define USBC1_MA		(0x00a)
	#endif

	#ifndef USBH_MA
	#define USBH_MA			(0x00b)
	#endif

	#ifndef PERI0_MB
	#define PERI0_MB		(0x00c)
	#endif

	#ifndef I2CM0_GDMA
	#define I2CM0_GDMA		(0x00d)
	#endif

	#ifndef I2CM1_GDMA
	#define I2CM1_GDMA		(0x00e)
	#endif

	#ifndef HWUA0_TX0_GDMA
	#define HWUA0_TX0_GDMA	(0x00f)
	#endif

	#ifndef HWUA0_TX1_GDMA
	#define HWUA0_TX1_GDMA	(0x010)
	#endif

	#ifndef HWUA1_TX0_GDMA
	#define HWUA1_TX0_GDMA	(0x011)
	#endif

	#ifndef MIPZ0_CB
	#define MIPZ0_CB		(0x012)
	#endif

	#ifndef MIPZ1_MB
	#define MIPZ1_MB		(0x013)
	#endif

	#ifndef IOPI_CB
	#define IOPI_CB			(0x014)
	#endif

	#ifndef IOPD_CB
	#define IOPD_CB			(0x015)
	#endif

	#ifndef CBDMA0_MA
	#define CBDMA0_MA		(0x016)
	#endif

	#ifndef CARD0_MA
	#define CARD0_MA		(0x017)
	#endif

	#ifndef CARD1_MA
	#define CARD1_MA		(0x018)
	#endif

	#ifndef CARD2_MA
	#define CARD2_MA		(0x019)
	#endif

	#ifndef SPI_NOR_MA
	#define SPI_NOR_MA		(0x01a)
	#endif

	#ifndef NBS_MA
	#define NBS_MA			(0x01b)
	#endif

	#ifndef IMG_DEC0_MB
	#define IMG_DEC0_MB		(0x01c)
	#endif

	#ifndef PERI1_MB
	#define PERI1_MB		(0x01d)
	#endif

	#ifndef PERI2_MB
	#define PERI2_MB		(0x01e)
	#endif

	#ifndef PERI3_MB
	#define PERI3_MB		(0x01f)
	#endif

	#ifndef DUMMY_MA
	#define DUMMY_MA		(0x020)
	#endif

	#ifndef UART2AXI_MA
	#define UART2AXI_MA		(0x021)
	#endif

	#ifndef SPI_CB_0_MA
	#define SPI_CB_0_MA		(0x022)
	#endif

	#ifndef SPI_CB_1_MA
	#define SPI_CB_1_MA		(0x023)
	#endif

	#ifndef HSM_MA
	#define HSM_MA			(0x024)
	#endif

	#ifndef SEMC_I_MA
	#define SEMC_I_MA		(0x025)
	#endif

	#ifndef SEMC_D_MA
	#define SEMC_D_MA		(0x026)
	#endif

	#ifndef SPACC_MA
	#define SPACC_MA		(0x027)
	#endif
//###############################################################################

	#ifndef DDFCH0_MA
	#define DDFCH0_MA		(0x100)
	#endif

	#ifndef DDFCH1_MA
	#define DDFCH1_MA		(0x101)
	#endif

	#ifndef DDFCH2_MA
	#define DDFCH2_MA		(0x102)
	#endif 

	#ifndef PITLANE0_MA
	#define PITLANE0_MA		(0x103)
	#endif

	#ifndef PITLANE1_MA
	#define PITLANE1_MA		(0x104)
	#endif

	#ifndef OSD0_MA
	#define OSD0_MA			(0x105)
	#endif

	#ifndef OSD1_MA
	#define OSD1_MA			(0x106)
	#endif

	#ifndef OSD2_MA
	#define OSD2_MA			(0x107)
	#endif

	#ifndef OSD3_MA
	#define OSD3_MA			(0x108)
	#endif

	#ifndef RTEIR_MA
	#define RTEIR_MA		(0x109)
	#endif

	#ifndef RTEIW_MA
	#define RTEIW_MA		(0x10a)
	#endif

	#ifndef R180_MA
	#define R180_MA			(0x10b)
	#endif

//###############################################################################

	#ifndef VI0_VIIW0_MA
	#define VI0_VIIW0_MA	(0x200)
	#endif

	#ifndef VI0_VIIW1_MA
	#define VI0_VIIW1_MA	(0x201)
	#endif

	#ifndef VI0_VIIW2_MA
	#define VI0_VIIW2_MA	(0x202)
	#endif

	#ifndef VI0_VIIR_MA
	#define VI0_VIIR_MA		(0x203)
	#endif

	#ifndef VI1_VIIW0_MA
	#define VI1_VIIW0_MA	(0x204)
	#endif

	#ifndef VI1_VIIW1_MA
	#define VI1_VIIW1_MA	(0x205)
	#endif

	#ifndef VI1_VIIW2_MA
	#define VI1_VIIW2_MA	(0x206)
	#endif

	#ifndef VI1_VIIR_MA
	#define VI1_VIIR_MA		(0x207)
	#endif

	#ifndef VI2_VIIW0_MA
	#define VI2_VIIW0_MA	(0x208)
	#endif

	#ifndef VI2_VIIW1_MA
	#define VI2_VIIW1_MA	(0x209)
	#endif

	#ifndef VI2_VIIW2_MA
	#define VI2_VIIW2_MA	(0x20a)
	#endif

	#ifndef VI2_VIIR_MA
	#define VI2_VIIR_MA		(0x20b)
	#endif

	#ifndef VI3_VIIW0_MA
	#define VI3_VIIW0_MA	(0x20c)
	#endif

	#ifndef VI3_VIIW1_MA
	#define VI3_VIIW1_MA	(0x20d)
	#endif

	#ifndef VI3_VIIW2_MA
	#define VI3_VIIW2_MA	(0x20e)
	#endif

	#ifndef VI3_VIIR_MA
	#define VI3_VIIR_MA		(0x20f)
	#endif

	#ifndef VI4_VIIW0_MA
	#define VI4_VIIW0_MA	(0x210)
	#endif

	#ifndef VI4_VIIR_MA
	#define VI4_VIIR_MA		(0x211)
	#endif

	#ifndef TVDEC_MB
	#define TVDEC_MB		(0x212)
	#endif

//###############################################################################

	#ifndef SECGRP1_BASE_ZERO 
	#define SECGRP1_BASE_ZERO 	(0x00000000)
	#endif

	#ifndef SECGRP1_BASE_ONE 
	#define SECGRP1_BASE_ONE 	(0x00000001)
	#endif

#endif

//###############################################################################
//
// 			Setting NIC Override Function
//
//###############################################################################
static inline void SET_NIC_OVERRIDE(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN_UP0(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QII_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QII_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP32(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP24(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP16(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIV_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIV_UP32(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_OVERRIDE(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN_UP0(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QII_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QII_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP32(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP24(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP16(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIV_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIV_UP32(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_VIDEOIN(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_VIDEOIN_UP16(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_VIDEOIN_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_VIDEOIN_UP0(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_OVERRIDE(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_MAIN_UP0(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QII_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QII_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP32(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP24(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP16(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIII_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIV_SET(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_QIV_UP32(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_VIDEOIN(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_VIDEOIN_UP16(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_VIDEOIN_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_VIDEOIN_UP0(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_DISP(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_DISP_UP8(unsigned int IP_NAME, unsigned int MODE) ;
static inline void SET_NIC_DISP_UP0(unsigned int IP_NAME, unsigned int MODE) ;



//###############################################################################
//
// 			Setting NIC Override Basic Define
//
//###############################################################################
#ifndef SET_OVERRIDE_EN
#define SET_OVERRIDE_EN(SHIFT)      		(SECGRP1_BASE_ONE << (8+SHIFT))
#endif

#ifndef SET_NOOVERRIDE_EN
#define SET_NOOVERRIDE_EN(SHIFT)      		(SECGRP1_BASE_ZERO << (8+SHIFT))
#endif

#ifndef SET_SEC_EN
#define SET_SEC_EN(SHIFT)      				(SECGRP1_BASE_ZERO << (SHIFT))
#endif

#ifndef SET_NOSEC_EN
#define SET_NOSEC_EN(SHIFT)      		    (SECGRP1_BASE_ONE << (SHIFT))
#endif

#ifndef SET_MASKING_OVERRIDE_EN
#define SET_MASKING_OVERRIDE_EN(SHIFT)      (SECGRP1_BASE_ONE << (24+SHIFT))
#endif

#ifndef SET_MASKING_SEC_EN
#define SET_MASKING_SEC_EN(SHIFT)      		(SECGRP1_BASE_ONE << (16+SHIFT))
#endif


#ifndef SET_NIC_NOR_MAIN
#define SET_NIC_NOR_MAIN(IP_NAME, MODE) 	({ CSTAMP(0x8200ffff); })
#endif

#ifndef SET_NIC_MAIN_UP8_M2
#define SET_NIC_MAIN_UP8_M2(IP_NAME)       	( SECGRP1_MAIN_REG->G083_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_MAIN_UP8_M1
#define SET_NIC_MAIN_UP8_M1(IP_NAME)       	( SECGRP1_MAIN_REG->G083_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_MAIN_UP8_M0
#define SET_NIC_MAIN_UP8_M0(IP_NAME)       	( SECGRP1_MAIN_REG->G083_NIC_S02 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif
#ifndef SET_NIC_MAIN_UP0_M2
#define SET_NIC_MAIN_UP0_M2(IP_NAME)       	( SECGRP1_MAIN_REG->G083_NIC_S01 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_MAIN_UP0_M1
#define SET_NIC_MAIN_UP0_M1(IP_NAME)       	( SECGRP1_MAIN_REG->G083_NIC_S01 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_MAIN_UP0_M0
#define SET_NIC_MAIN_UP0_M0(IP_NAME)       	( SECGRP1_MAIN_REG->G083_NIC_S01 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif
#ifndef SET_NIC_QII_UP8_M2
#define SET_NIC_QII_UP8_M2(IP_NAME)       	( SECGRP1_QII_REG->G084_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QII_UP8_M1
#define SET_NIC_QII_UP8_M1(IP_NAME)       	( SECGRP1_QII_REG->G084_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QII_UP8_M0
#define SET_NIC_QII_UP8_M0(IP_NAME)       	( SECGRP1_QII_REG->G084_NIC_S02 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif
#ifndef SET_NIC_QIII_UP32_M2
#define SET_NIC_QIII_UP32_M2(IP_NAME)       ( SECGRP1_QIII_REG->G085_NIC_S05 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QIII_UP32_M1
#define SET_NIC_QIII_UP32_M1(IP_NAME)       ( SECGRP1_QIII_REG->G085_NIC_S05 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QIII_UP32_M0
#define SET_NIC_QIII_UP32_M0(IP_NAME)       ( SECGRP1_QIII_REG->G085_NIC_S05 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif
#ifndef SET_NIC_QIII_UP24_M2
#define SET_NIC_QIII_UP24_M2(IP_NAME)       ( SECGRP1_QIII_REG->G085_NIC_S04 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QIII_UP24_M1
#define SET_NIC_QIII_UP24_M1(IP_NAME)       ( SECGRP1_QIII_REG->G085_NIC_S04 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QIII_UP24_M0
#define SET_NIC_QIII_UP24_M0(IP_NAME)       ( SECGRP1_QIII_REG->G085_NIC_S04 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif
#ifndef SET_NIC_QIII_UP16_M2
#define SET_NIC_QIII_UP16_M2(IP_NAME)       ( SECGRP1_QIII_REG->G085_NIC_S03 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QIII_UP16_M1
#define SET_NIC_QIII_UP16_M1(IP_NAME)       ( SECGRP1_QIII_REG->G085_NIC_S03 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QIII_UP16_M0
#define SET_NIC_QIII_UP16_M0(IP_NAME)       ( SECGRP1_QIII_REG->G085_NIC_S03 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif
#ifndef SET_NIC_QIII_UP8_M2
#define SET_NIC_QIII_UP8_M2(IP_NAME)       	( SECGRP1_QIII_REG->G085_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QIII_UP8_M1
#define SET_NIC_QIII_UP8_M1(IP_NAME)       	( SECGRP1_QIII_REG->G085_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QIII_UP8_M0
#define SET_NIC_QIII_UP8_M0(IP_NAME)       	( SECGRP1_QIII_REG->G085_NIC_S02 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif
#ifndef SET_NIC_QIV_UP32_M2
#define SET_NIC_QIV_UP32_M2(IP_NAME)       ( SECGRP1_QIV_REG->G086_NIC_S05 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QIV_UP32_M1
#define SET_NIC_QIV_UP32_M1(IP_NAME)       ( SECGRP1_QIV_REG->G086_NIC_S05 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_QIV_UP32_M0
#define SET_NIC_QIV_UP32_M0(IP_NAME)       ( SECGRP1_QIV_REG->G086_NIC_S05 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif
//=======================================================
//
//	G83.26~30
//
//=======================================================
//##############################################
// 	IP_NAME  : {Prefix_1st}_{Prefix_2nd} --> Please Reference Programming_Guide#Master_IP
//  Mode     : 0 is bypass IP AxPROT, 1 is overwrite normal mode, 2 is overwrite secure mode.
//  example  : SET_NIC_OVERRIDE(VCD_MA, 0x1) --> VCD Master overwrite to normal mode.
//##############################################
//R.26
#ifndef SET_NIC_OVERRIDE_DEF
#define SET_NIC_OVERRIDE_DEF
static inline void SET_NIC_OVERRIDE(unsigned int IP_NAME, unsigned int MODE) {
	if(IP_NAME >= 0x200) {
		SET_NIC_VIDEOIN((IP_NAME-0x200), MODE); 	// into VIDEOIN SECGRP1 Group
	} else if(IP_NAME >= 0x100) {
		SET_NIC_DISP((IP_NAME-0x100), MODE); 		// into DISP SECGRP1 Group
	} else {
		SET_NIC_MAIN(IP_NAME, MODE); 				// into Other SECGRP1 Group
	}
}
#endif

#ifndef SET_NIC_MAIN_DEF
#define SET_NIC_MAIN_DEF
static inline void SET_NIC_MAIN(unsigned int IP_NAME, unsigned int MODE) {
	//if((IP_NAME >= 0x000) && (IP_NAME <= 0x008)) {
	if (IP_NAME <= 0x008) {
		SET_NIC_MAIN_SET(IP_NAME, MODE); 					// into partition TOP  SECGRP1 Group
	} else if((IP_NAME >= 0x009) && (IP_NAME <= 0x00e)) {
		SET_NIC_QII_SET(IP_NAME, MODE); 					// into partition QII  SECGRP1 Group
	} else if((IP_NAME >= 0x00f) && (IP_NAME <= 0x023)) {
		SET_NIC_QIII_SET(IP_NAME, MODE); 					// into partition QIII SECGRP1 Group
	} else if((IP_NAME >= 0x024) && (IP_NAME <= 0x027)) {
		SET_NIC_QIV_SET(IP_NAME, MODE); 					// into partition QIV  SECGRP1 Group
	} else {
		RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_MAIN_SET_DEF
#define SET_NIC_MAIN_SET_DEF
static inline void SET_NIC_MAIN_SET(unsigned int IP_NAME, unsigned int MODE) {
	if(IP_NAME >= 0x008) {
		SET_NIC_MAIN_UP8((IP_NAME-0x008), MODE); 			// into partition TOP  SECGRP1 Group Addr "G083_NIC_S02"
	} else {
		SET_NIC_MAIN_UP0((IP_NAME), MODE); 					// into partition TOP  SECGRP1 Group Addr "G083_NIC_S01"
	}
}
#endif

#ifndef SET_NIC_MAIN_UP8_DEF
#define SET_NIC_MAIN_UP8_DEF
static inline void SET_NIC_MAIN_UP8(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_MAIN_UP8_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_MAIN_UP8_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_MAIN_UP8_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
} 
#endif



#ifndef SET_NIC_MAIN_UP0_DEF
#define SET_NIC_MAIN_UP0_DEF
static inline void SET_NIC_MAIN_UP0(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_MAIN_UP0_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_MAIN_UP0_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_MAIN_UP0_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif



#ifndef SET_NIC_QII_SET_DEF
#define SET_NIC_QII_SET_DEF
static inline void SET_NIC_QII_SET(unsigned int IP_NAME, unsigned int MODE) {
	SET_NIC_QII_UP8((IP_NAME-0x008), MODE); 				// into partition QII  SECGRP1 Group Addr "G084_NIC_S02"
}
#endif

#ifndef SET_NIC_QII_UP8_DEF
#define SET_NIC_QII_UP8_DEF
static inline void SET_NIC_QII_UP8(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_QII_UP8_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_QII_UP8_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_QII_UP8_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif



#ifndef SET_NIC_QIII_SET_DEF
#define SET_NIC_QIII_SET_DEF
static inline void SET_NIC_QIII_SET(unsigned int IP_NAME, unsigned int MODE) {
	if(IP_NAME >= 0x020) {
		SET_NIC_QIII_UP32((IP_NAME-0x020), MODE); 			// into partition QIII  SECGRP1 Group Addr "G085_NIC_S05"
	} else if(IP_NAME >= 0x018) {
		SET_NIC_QIII_UP24((IP_NAME-0x018), MODE); 			// into partition QIII  SECGRP1 Group Addr "G085_NIC_S04"
	} else if(IP_NAME >= 0x010) {
		SET_NIC_QIII_UP16((IP_NAME-0x010), MODE); 			// into partition QIII  SECGRP1 Group Addr "G085_NIC_S03"
	} else if(IP_NAME >= 0x008) {
		SET_NIC_QIII_UP8((IP_NAME-0x008), MODE); 			// into partition QIII  SECGRP1 Group Addr "G085_NIC_S02"
	} else {
		RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_QIII_UP32_DEF
#define SET_NIC_QIII_UP32_DEF
static inline void SET_NIC_QIII_UP32(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_QIII_UP32_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_QIII_UP32_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_QIII_UP32_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif



#ifndef SET_NIC_QIII_UP24_DEF
#define SET_NIC_QIII_UP24_DEF
static inline void SET_NIC_QIII_UP24(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_QIII_UP24_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_QIII_UP24_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_QIII_UP24_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif



#ifndef SET_NIC_QIII_UP16_DEF
#define SET_NIC_QIII_UP16_DEF
static inline void SET_NIC_QIII_UP16(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_QIII_UP16_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_QIII_UP16_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_QIII_UP16_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif



#ifndef SET_NIC_QIII_UP8_DEF
#define SET_NIC_QIII_UP8_DEF
static inline void SET_NIC_QIII_UP8(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_QIII_UP8_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_QIII_UP8_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_QIII_UP8_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif



#ifndef SET_NIC_QIV_SET_DEF
#define SET_NIC_QIV_SET_DEF
static inline void SET_NIC_QIV_SET(unsigned int IP_NAME, unsigned int MODE) {
	if(IP_NAME >= 0x020) {
		SET_NIC_QIV_UP32((IP_NAME-0x020), MODE); 			// into partition QIV  SECGRP1 Group Addr "G086_NIC_S05"
	} else {
		RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_QIV_UP32_DEF
#define SET_NIC_QIV_UP32_DEF
static inline void SET_NIC_QIV_UP32(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_QIV_UP32_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_QIV_UP32_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_QIV_UP32_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif



//=======================================================
//
//	G114.26~28
//
//=======================================================

#ifndef SET_NIC_VIDEOIN_UP16_M2
#define SET_NIC_VIDEOIN_UP16_M2(IP_NAME)		( SECGRP1_VIDEOIN_REG->G114_NIC_S03 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_VIDEOIN_UP16_M1
#define SET_NIC_VIDEOIN_UP16_M1(IP_NAME)		( SECGRP1_VIDEOIN_REG->G114_NIC_S03 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_VIDEOIN_UP16_M0
#define SET_NIC_VIDEOIN_UP16_M0(IP_NAME)		( SECGRP1_VIDEOIN_REG->G114_NIC_S03 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif
#ifndef SET_NIC_VIDEOIN_UP8_M2
#define SET_NIC_VIDEOIN_UP8_M2(IP_NAME)       	( SECGRP1_VIDEOIN_REG->G114_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_VIDEOIN_UP8_M1
#define SET_NIC_VIDEOIN_UP8_M1(IP_NAME)       	( SECGRP1_VIDEOIN_REG->G114_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_VIDEOIN_UP8_M0
#define SET_NIC_VIDEOIN_UP8_M0(IP_NAME)       	( SECGRP1_VIDEOIN_REG->G114_NIC_S02 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif
#ifndef SET_NIC_VIDEOIN_UP0_M2
#define SET_NIC_VIDEOIN_UP0_M2(IP_NAME)       	( SECGRP1_VIDEOIN_REG->G114_NIC_S01 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_VIDEOIN_UP0_M1
#define SET_NIC_VIDEOIN_UP0_M1(IP_NAME)       	( SECGRP1_VIDEOIN_REG->G114_NIC_S01 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_VIDEOIN_UP0_M0
#define SET_NIC_VIDEOIN_UP0_M0(IP_NAME)       	( SECGRP1_VIDEOIN_REG->G114_NIC_S01 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_VIDEOIN_DEF
static inline void SET_NIC_VIDEOIN(unsigned int IP_NAME, unsigned int MODE) {
	if(IP_NAME >= 0x010) {
		SET_NIC_VIDEOIN_UP16((IP_NAME-0x010), MODE); 			// into partition VIDEOIN  SECGRP1 Group Addr "G114_NIC_S03"
	} else if(IP_NAME >= 0x008) {
		SET_NIC_VIDEOIN_UP8((IP_NAME-0x008), MODE); 			// into partition VIDEOIN  SECGRP1 Group Addr "G114_NIC_S02"
	} else {
		SET_NIC_VIDEOIN_UP0(IP_NAME, MODE); 					// into partition VIDEOIN  SECGRP1 Group Addr "G114_NIC_S01"
	}
}
#endif

#ifndef SET_NIC_VIDEOIN_UP16_DEF
#define SET_NIC_VIDEOIN_UP16_DEF
static inline void SET_NIC_VIDEOIN_UP16(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_VIDEOIN_UP16_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_VIDEOIN_UP16_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_VIDEOIN_UP16_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_VIDEOIN_UP8_DEF
#define SET_NIC_VIDEOIN_UP8_DEF
static inline void SET_NIC_VIDEOIN_UP8(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_VIDEOIN_UP8_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_VIDEOIN_UP8_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_VIDEOIN_UP8_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_VIDEOIN_UP0_DEF
#define SET_NIC_VIDEOIN_UP0_DEF
static inline void SET_NIC_VIDEOIN_UP0(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_VIDEOIN_UP0_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_VIDEOIN_UP0_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_VIDEOIN_UP0_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif

//=======================================================
//
//	G114.26~28
//
//=======================================================

#ifndef SET_NIC_DISP_UP8_M2
#define SET_NIC_DISP_UP8_M2(IP_NAME)       	( SECGRP1_DISP_REG->G113_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_DISP_UP8_M1
#define SET_NIC_DISP_UP8_M1(IP_NAME)       	( SECGRP1_DISP_REG->G113_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_DISP_UP8_M0
#define SET_NIC_DISP_UP8_M0(IP_NAME)       	( SECGRP1_DISP_REG->G113_NIC_S02 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif
#ifndef SET_NIC_DISP_UP0_M2
#define SET_NIC_DISP_UP0_M2(IP_NAME)       	( SECGRP1_DISP_REG->G113_NIC_S01 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_DISP_UP0_M1
#define SET_NIC_DISP_UP0_M1(IP_NAME)       	( SECGRP1_DISP_REG->G113_NIC_S01 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_DISP_UP0_M0
#define SET_NIC_DISP_UP0_M0(IP_NAME)       	( SECGRP1_DISP_REG->G113_NIC_S01 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)) )
#endif

#ifndef SET_NIC_DISP_DEF
#define SET_NIC_DISP_DEF
static inline void SET_NIC_DISP(unsigned int IP_NAME, unsigned int MODE) {
	if(IP_NAME >= 0x008) {
		SET_NIC_DISP_UP8((IP_NAME-0x008), MODE); 			// into partition DISP  SECGRP1 Group Addr "G113_NIC_S02"
	} else {
		SET_NIC_DISP_UP0(IP_NAME, MODE); 					// into partition DISP  SECGRP1 Group Addr "G114_NIC_S01"
	}
}
#endif

#ifndef SET_NIC_DISP_UP8_DEF
#define SET_NIC_DISP_UP8_DEF
static inline void SET_NIC_DISP_UP8(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_DISP_UP8_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_DISP_UP8_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_DISP_UP8_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_DISP_UP0_DEF
#define SET_NIC_DISP_UP0_DEF
static inline void SET_NIC_DISP_UP0(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_DISP_UP0_M2(IP_NAME); 						// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_DISP_UP0_M1(IP_NAME); 						// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_DISP_UP0_M0(IP_NAME); 						// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif


//###############################################################################
//
// 			Setting / Get CA55
//
//###############################################################################
#ifndef SET_CA55_CORE0_RVBAR
#define SET_CA55_CORE0_RVBAR(regVal) 	(SECGRP1_MAIN_REG->G083_CA55_S01 = regVal)
#endif

#ifndef SET_CA55_CORE1_RVBAR
#define SET_CA55_CORE1_RVBAR(regVal) 	(SECGRP1_MAIN_REG->G083_CA55_S02 = regVal)
#endif

#ifndef SET_CA55_CORE2_RVBAR
#define SET_CA55_CORE2_RVBAR(regVal) 	(SECGRP1_MAIN_REG->G083_CA55_S03 = regVal)
#endif

#ifndef SET_CA55_CORE3_RVBAR
#define SET_CA55_CORE3_RVBAR(regVal) 	(SECGRP1_MAIN_REG->G083_CA55_S04 = regVal)
#endif

#ifndef GET_CA55_CORE0_RVBAR
#define GET_CA55_CORE0_RVBAR 			(SECGRP1_MAIN_REG->G083_CA55_S01)
#endif

#ifndef GET_CA55_CORE1_RVBAR
#define GET_CA55_CORE1_RVBAR 			(SECGRP1_MAIN_REG->G083_CA55_S02)
#endif

#ifndef GET_CA55_CORE2_RVBAR
#define GET_CA55_CORE2_RVBAR 			(SECGRP1_MAIN_REG->G083_CA55_S03)
#endif

#ifndef GET_CA55_CORE3_RVBAR
#define GET_CA55_CORE3_RVBAR 			(SECGRP1_MAIN_REG->G083_CA55_S04)
#endif

//###############################################################################
//
// 			Setting / Get CBDMA
//
//###############################################################################
#ifndef SET_CBDMA0_PROT_REGION_O
#define SET_CBDMA0_PROT_REGION_O(regVal) 	(SET_CBDMA0_S01(regVal))
#endif

#ifndef GET_CBDMA0_PROT_REGION_O
#define GET_CBDMA0_PROT_REGION_O 			(GET_CBDMA0_S01)
#endif

#ifndef SET_CBDMA0_PROT_REGION_1
#define SET_CBDMA0_PROT_REGION_1(regVal) 	(SET_CBDMA0_S02(regVal))
#endif

#ifndef GET_CBDMA0_PROT_REGION_1
#define GET_CBDMA0_PROT_REGION_1 			(GET_CBDMA0_S02)
#endif

#ifndef SET_CBDMA0_S01
#define SET_CBDMA0_S01(regVal) 	(SECGRP1_QIII_REG->G085_CBDMA0_S01 = regVal)
#endif

#ifndef GET_CBDMA0_S01
#define GET_CBDMA0_S01 			(SECGRP1_QIII_REG->G085_CBDMA0_S01)
#endif

#ifndef SET_CBDMA0_S02
#define SET_CBDMA0_S02(regVal) 	(SECGRP1_QIII_REG->G085_CBDMA0_S02 = regVal)
#endif

#ifndef GET_CBDMA0_S02
#define GET_CBDMA0_S02 			(SECGRP1_QIII_REG->G085_CBDMA0_S02)
#endif

//###############################################################################
//
// 			Setting / Get SDPROT
//
//###############################################################################
//##############################################
// 	BaseAddr : 0x1 (MB)
//  Mode     : 0 is All normal, 1 is All secure.
//  example  : SET_SDPROT(0x1, 1) --> 1 MB ~ 32 MB all protect.
//##############################################
#ifndef SET_SDPROT
#define SET_SDPROT(BaseAddr, Mode) 	({if(Mode == 1) {SET_SDPROT_S01(BaseAddr << 20); SET_SDPROT_S02(0x00000000);} else {SET_SDPROT_S01(BaseAddr << 20); SET_SDPROT_S02(0xffffffff);} })
#endif

#ifndef GET_SDPROT_ILL_ADDR
#define GET_SDPROT_ILL_ADDR 		(GET_SDPROT_S03)
#endif

#ifndef GET_SDPROT_ILL_ID
#define GET_SDPROT_ILL_ID 			(GET_SDPROT_S04 & (0xffffffff >> (32-20))) // 20 it's mean vaild bits.
#endif

#ifndef CLEAR_SDPROT
#define CLEAR_SDPROT 			(SECGRP1_MAIN_REG->G083_SDPROT_S04 = 0x80000000)
#endif

#ifndef SET_SDPROT_S01
#define SET_SDPROT_S01(regVal) 	(SECGRP1_MAIN_REG->G083_SDPROT_S01 = regVal)
#endif

#ifndef GET_SDPROT_S01
#define GET_SDPROT_S01 			(SECGRP1_MAIN_REG->G083_SDPROT_S01)
#endif

#ifndef SET_SDPROT_S02
#define SET_SDPROT_S02(regVal) 	(SECGRP1_MAIN_REG->G083_SDPROT_S02 = regVal)
#endif

#ifndef GET_SDPROT_S02
#define GET_SDPROT_S02 			(SECGRP1_MAIN_REG->G083_SDPROT_S02)
#endif

#ifndef GET_SDPROT_S03
#define GET_SDPROT_S03 			(SECGRP1_MAIN_REG->G083_SDPROT_S03)
#endif

#ifndef GET_SDPROT_S04
#define GET_SDPROT_S04 			(SECGRP1_MAIN_REG->G083_SDPROT_S04)
#endif

#endif /* __INC_SECGRP1_H */
