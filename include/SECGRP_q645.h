//==========================================================================
//    File      :  SECGRP1.h
//    Author    :  lumi.yang
//    Date      :  2020-07-16
//    Abstract  :  This is a base of SECGRP1_* and NORGRP1_*;
//==========================================================================
//==========================================================================
//    Modification History:
//	Version          Author          	Description
//    version 0.0	lumi.yang@2020.07.16	
//==========================================================================
#ifndef __INC_SECGRP1_Q645_H
#define __INC_SECGRP1_Q645_H

#include <common.h>

//=======================================================
//
//	Configure Register Macro Function
//
//=======================================================

struct SECGRP1_MAIN_REGS {
    UINT32  MAIN_CA55_S01;
    UINT32  MAIN_CA55_S02;
    UINT32  MAIN_CA55_S03;
    UINT32  MAIN_CA55_S04;
    UINT32  MAIN_reserved_1[4];
    UINT32  MAIN_CA55_S09;  //G083.8;
    UINT32  MAIN_reserved_2[4];
    UINT32  MAIN_SDPROT_S01;
    UINT32  MAIN_SDPROT_S02;
    UINT32  MAIN_SDPROT_S03;
    UINT32  MAIN_SDPROT_S04;
    UINT32  MAIN_SDPROT_S05;
    UINT32  MAIN_SDPROT_S06;
    UINT32  MAIN_reserved_3[4];
    UINT32  MAIN_NIC_S01;   //G083.23;
    UINT32  MAIN_NIC_S02;   //G083.24;
    UINT32  MAIN_reserved_4[7];
};

struct SECGRP1_PAI_REGS {
    UINT32  PAI_reserved_1[9];
    UINT32  PAI_CBDMA0_S01;
    UINT32  PAI_CBDMA0_S02;
    UINT32  PAI_reserved_2[13];
    UINT32  PAI_NIC_S02;  //G084.24;
    UINT32  PAI_NIC_S03;  //G084.25;
    UINT32  PAI_NIC_S04;  //G084.26;
    UINT32  PAI_NIC_S05;  //G084.27;
    UINT32  PAI_NIC_S06;  //G084.28;
    UINT32  PAI_reserved_3[3];
};

struct SECGRP1_PAII_REGS {
    UINT32  PAII_reserved_1[28];
    UINT32  PAII_NIC_S06;  //G085.28;
    UINT32  PAII_NIC_S07;  //G085.29;
    UINT32  PAII_NIC_S08;  //G085.30;
    UINT32  PAII_reserved_2[1];
};


#define SECGRP1_MAIN_REG    ((volatile struct SECGRP1_MAIN_REGS *)RF_GRP(SECGRP1_MAIN_RGST_NUM, 0))
#define SECGRP1_PAI_REG     ((volatile struct SECGRP1_PAI_REGS *)RF_GRP(SECGRP1_PAI_RGST_NUM, 0))
#define SECGRP1_PAII_REG    ((volatile struct SECGRP1_PAII_REGS *)RF_GRP(SECGRP1_PAII_RGST_NUM, 0))


//#define CMD_NO_CHECK
//#define NO_PRINT_MSG
#define QUI645_SECGRP1

#ifdef QUI645_SECGRP1

#ifndef SECGRP1_MAIN_RGST_NUM
#define SECGRP1_MAIN_RGST_NUM                (83) 	// Set RGST group number.
#endif

#ifndef SECGRP1_PAI_RGST_NUM
#define SECGRP1_PAI_RGST_NUM                 (84) 	// Set RGST group number.
#endif

#ifndef SECGRP1_PAII_RGST_NUM
#define SECGRP1_PAII_RGST_NUM                (85) 	// Set RGST group number.
#endif


#ifndef CA55_M0
#define CA55_M0                              (0x000)
#endif

#ifndef N77_MA
#define N77_MA                               (0x001)
#endif

#ifndef EVDN_MA0
#define EVDN_MA0                             (0x002)
#endif

#ifndef EVDN_MA2
#define EVDN_MA2                             (0x003)
#endif

#ifndef EVDN_MA3
#define EVDN_MA3                             (0x004)
#endif

#ifndef G31_MA
#define G31_MA                               (0x005)
#endif

#ifndef DUMMY0_MA
#define DUMMY0_MA                            (0x006)
#endif

#ifndef IOPI_CB_MA
#define IOPI_CB_MA                           (0x007)
#endif

#ifndef IOPD_CB_MA
#define IOPD_CB_MA                           (0x008)
#endif

#ifndef CM4S_MA
#define CM4S_MA                              (0x009)
#endif

#ifndef CM4ID_MA
#define CM4ID_MA                             (0x00a)
#endif

#ifndef CPIOL0_MA
#define CPIOL0_MA                            (0x00b)
#endif

#ifndef CPIOL1_MA
#define CPIOL1_MA                            (0x00c)
#endif

#ifndef DUMMY1_MA
#define DUMMY1_MA                            (0x00d)
#endif

#ifndef NBS_MA
#define NBS_MA                               (0x00e)
#endif

#ifndef SPI_NOR_MA
#define SPI_NOR_MA                           (0x00f)
#endif

#ifndef CARD0_MA
#define CARD0_MA                             (0x010)
#endif

#ifndef CARD1_M0
#define CARD1_M0                             (0x011)
#endif

#ifndef HWUA0_TX0_GDMA
#define HWUA0_TX0_GDMA                       (0x012)
#endif

#ifndef HWUA0_TX1_GDMA
#define HWUA0_TX1_GDMA                       (0x013)
#endif

#ifndef HWUA1_TX0_GDMA
#define HWUA1_TX0_GDMA                       (0x014)
#endif

#ifndef HWUA1_TX1_GDMA
#define HWUA1_TX1_GDMA                       (0x015)
#endif

#ifndef PERIO_MA
#define PERIO_MA                             (0x016)
#endif

#ifndef PERI1_MA
#define PERI1_MA                             (0x017)
#endif

#ifndef PERI2_MA
#define PERI2_MA                             (0x018)
#endif

#ifndef PERI3_MA
#define PERI3_MA                             (0x019)
#endif

#ifndef CBDMA0_MA
#define CBDMA0_MA                            (0x01a)
#endif

#ifndef SPI_CB0_MA
#define SPI_CB0_MA                           (0x01b)
#endif

#ifndef SPI_CB1_MA
#define SPI_CB1_MA                           (0x01c)
#endif

#ifndef SPI_CB2_MA
#define SPI_CB2_MA                           (0x01d)
#endif

#ifndef SPI_CB3_MA
#define SPI_CB3_MA                           (0x01e)
#endif

#ifndef SPI_CB4_MA
#define SPI_CB4_MA                           (0x01f)
#endif

#ifndef SPI_CB5_MA
#define SPI_CB5_MA                           (0x020)
#endif

#ifndef I2CM0_GDMA
#define I2CM0_GDMA                           (0x021)
#endif

#ifndef I2CM1_GDMA
#define I2CM1_GDMA                           (0x022)
#endif

#ifndef I2CM2_GDMA
#define I2CM2_GDMA                           (0x023)
#endif

#ifndef I2CM3_GDMA
#define I2CM3_GDMA                           (0x024)
#endif

#ifndef I2CM4_GDMA
#define I2CM4_GDMA                           (0x025)
#endif

#ifndef I2CM5_GDMA
#define I2CM5_GDMA                           (0x026)
#endif

#ifndef CSIIW0_MA
#define CSIIW0_MA                            (0x027)
#endif

#ifndef CSIIW1_MA
#define CSIIW1_MA                            (0x028)
#endif

#ifndef VCL_MA
#define VCL_MA                               (0x029)
#endif

#ifndef VCE_MA
#define VCE_MA                               (0x02a)
#endif

#ifndef VCD_MA
#define VCD_MA                               (0x02b)
#endif

#ifndef CPIOR0_MA
#define CPIOR0_MA                            (0x02c)
#endif

#ifndef CPIOR1_MA
#define CPIOR1_MA                            (0x02d)
#endif

#ifndef DUMMY2_MA
#define DUMMY2_MA                            (0x02e)
#endif

#ifndef HSM_MA
#define HSM_MA                               (0x02f)
#endif

#ifndef SEMC_I_MA
#define SEMC_I_MA                            (0x030)
#endif

#ifndef SEMC_D_MA
#define SEMC_D_MA                            (0x031)
#endif

#ifndef SPACC_MA
#define SPACC_MA                             (0x032)
#endif

#ifndef USB30C0_MA
#define USB30C0_MA                           (0x033)
#endif

#ifndef USB30C1_MA
#define USB30C1_MA                           (0x034)
#endif

#ifndef USBC0_MA
#define USBC0_MA                             (0x035)
#endif

#ifndef CARD2_MA
#define CARD2_MA                             (0x036)
#endif

#ifndef UART2AXI_MA
#define UART2AXI_MA                          (0x037)
#endif

#ifndef CSIIW2_MA
#define CSIIW2_MA                            (0x038)
#endif

#ifndef CSIIW3_MA
#define CSIIW3_MA                            (0x039)
#endif

//###############################################################################

#ifndef SECGRP1_BASE_ZERO
#define SECGRP1_BASE_ZERO                    (0x00000000)
#endif

#ifndef SECGRP1_BASE_ONE 
#define SECGRP1_BASE_ONE                     (0x00000001)
#endif

#endif

//###############################################################################
//
//          	Setting NIC Override Function
//
//###############################################################################

static inline void SET_NIC_OVERRIDE(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_MAIN(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_MAIN_SET(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_MAIN_UP8(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_MAIN_UP0(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAI_SET(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAI_UP8(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAI_UP16(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAI_UP24(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAI_UP32(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAI_UP40(unsigned int IP_NAME, unsigned int MODE);

static inline void SET_NIC_PAII_SET(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAII_UP40(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAII_UP48(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAII_UP56(unsigned int IP_NAME, unsigned int MODE);


//###############################################################################
//
//          	Setting NIC Override Basic Define
//
//###############################################################################
#ifndef SET_OVERRIDE_EN
#define SET_OVERRIDE_EN(SHIFT)               (SECGRP1_BASE_ONE << (8+SHIFT))
#endif

#ifndef SET_NOOVERRIDE_EN
#define SET_NOOVERRIDE_EN(SHIFT)             (SECGRP1_BASE_ZERO << (8+SHIFT))
#endif

#ifndef SET_SEC_EN
#define SET_SEC_EN(SHIFT)                    (SECGRP1_BASE_ZERO << (SHIFT))
#endif

#ifndef SET_NOSEC_EN
#define SET_NOSEC_EN(SHIFT)                  (SECGRP1_BASE_ONE << (SHIFT))
#endif

#ifndef SET_MASKING_OVERRIDE_EN
#define SET_MASKING_OVERRIDE_EN(SHIFT)       (SECGRP1_BASE_ONE << (24+SHIFT))
#endif

#ifndef SET_MASKING_SEC_EN
#define SET_MASKING_SEC_EN(SHIFT)            (SECGRP1_BASE_ONE << (16+SHIFT))
#endif


#ifndef SET_NIC_NOR_MAIN
#define SET_NIC_NOR_MAIN(IP_NAME, MODE)      ({ CSTAMP(0x8200ffff); })
#endif

#ifndef SET_NIC_MAIN_UP8_M2
#define SET_NIC_MAIN_UP8_M2(IP_NAME)         (SECGRP1_MAIN_REG->MAIN_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_MAIN_UP8_M1
#define SET_NIC_MAIN_UP8_M1(IP_NAME)         (SECGRP1_MAIN_REG->MAIN_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_MAIN_UP8_M0
#define SET_NIC_MAIN_UP8_M0(IP_NAME)         (SECGRP1_MAIN_REG->MAIN_NIC_S02 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif

#ifndef SET_NIC_MAIN_UP0_M2
#define SET_NIC_MAIN_UP0_M2(IP_NAME)         (SECGRP1_MAIN_REG->MAIN_NIC_S01 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_MAIN_UP0_M1
#define SET_NIC_MAIN_UP0_M1(IP_NAME)         (SECGRP1_MAIN_REG->MAIN_NIC_S01 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_MAIN_UP0_M0
#define SET_NIC_MAIN_UP0_M0(IP_NAME)         (SECGRP1_MAIN_REG->MAIN_NIC_S01 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif


#ifndef SET_NIC_PAI_UP8_M2
#define SET_NIC_PAI_UP8_M2(IP_NAME)          (SECGRP1_PAI_REG->PAI_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP8_M1
#define SET_NIC_PAI_UP8_M1(IP_NAME)          (SECGRP1_PAI_REG->PAI_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP8_M0
#define SET_NIC_PAI_UP8_M0(IP_NAME)          (SECGRP1_PAI_REG->PAI_NIC_S02 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif


#ifndef SET_NIC_PAI_UP16_M2
#define SET_NIC_PAI_UP16_M2(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S03 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP16_M1
#define SET_NIC_PAI_UP16_M1(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S03 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP16_M0
#define SET_NIC_PAI_UP16_M0(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S03 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP24_M2
#define SET_NIC_PAI_UP24_M2(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S04 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP24_M1
#define SET_NIC_PAI_UP24_M1(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S04 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP24_M0
#define SET_NIC_PAI_UP24_M0(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S04 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP32_M2
#define SET_NIC_PAI_UP32_M2(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S05 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP32_M1
#define SET_NIC_PAI_UP32_M1(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S05 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP32_M0
#define SET_NIC_PAI_UP32_M0(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S05 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP40_M2
#define SET_NIC_PAI_UP40_M2(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S06 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP40_M1
#define SET_NIC_PAI_UP40_M1(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S06 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP40_M0
#define SET_NIC_PAI_UP40_M0(IP_NAME)         (SECGRP1_PAI_REG->PAI_NIC_S06 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP40_M2
#define SET_NIC_PAII_UP40_M2(IP_NAME)        (SECGRP1_PAII_REG->PAII_NIC_S06 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP40_M1
#define SET_NIC_PAII_UP40_M1(IP_NAME)        (SECGRP1_PAII_REG->PAII_NIC_S06 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP40_M0
#define SET_NIC_PAII_UP40_M0(IP_NAME)        (SECGRP1_PAII_REG->PAII_NIC_S06 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP48_M2
#define SET_NIC_PAII_UP48_M2(IP_NAME)        (SECGRP1_PAII_REG->PAII_NIC_S07 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP48_M1
#define SET_NIC_PAII_UP48_M1(IP_NAME)        (SECGRP1_PAII_REG->PAII_NIC_S07 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP48_M0
#define SET_NIC_PAII_UP48_M0(IP_NAME)        (SECGRP1_PAII_REG->PAII_NIC_S07 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP56_M2
#define SET_NIC_PAII_UP56_M2(IP_NAME)        (SECGRP1_PAII_REG->PAII_NIC_S08 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP56_M1
#define SET_NIC_PAII_UP56_M1(IP_NAME)        (SECGRP1_PAII_REG->PAII_NIC_S08 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP56_M0
#define SET_NIC_PAII_UP56_M0(IP_NAME)        (SECGRP1_PAII_REG->PAII_NIC_S08 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
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
	SET_NIC_MAIN(IP_NAME, MODE);                  	// into SECGRP1 Group
}
#endif

#ifndef SET_NIC_MAIN_DEF
#define SET_NIC_MAIN_DEF
static inline void SET_NIC_MAIN(unsigned int IP_NAME, unsigned int MODE) {
	//if((IP_NAME >= 0x000) && (IP_NAME <= 0x008)) {
	if (IP_NAME <= 0x008) {
         SET_NIC_MAIN_SET(IP_NAME, MODE);          	// into partition TOP  SECGRP1 Group
	} else if((IP_NAME >= 0x009) && (IP_NAME <= 0x029)) {
         SET_NIC_PAI_SET(IP_NAME, MODE);          	// into partition PAI  SECGRP1 Group
	} else if((IP_NAME >= 0x02a) && (IP_NAME <= 0x039)) {
         SET_NIC_PAII_SET(IP_NAME, MODE);          	// into partition PAII SECGRP1 Group
	} else {
         RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_MAIN_SET_DEF
#define SET_NIC_MAIN_SET_DEF
static inline void SET_NIC_MAIN_SET(unsigned int IP_NAME, unsigned int MODE) {
	if(IP_NAME >= 0x008) {
         SET_NIC_MAIN_UP8((IP_NAME-0x008), MODE);          // into partition TOP  SECGRP1 Group Addr "MAIN_NIC_S02"
	} else {
         SET_NIC_MAIN_UP0((IP_NAME), MODE);          	// into partition TOP  SECGRP1 Group Addr "MAIN_NIC_S01"
	}
}
#endif

#ifndef SET_NIC_MAIN_UP8_DEF
#define SET_NIC_MAIN_UP8_DEF
static inline void SET_NIC_MAIN_UP8(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
         SET_NIC_MAIN_UP8_M2(IP_NAME);                   // into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
         SET_NIC_MAIN_UP8_M1(IP_NAME);                   // into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
         SET_NIC_MAIN_UP8_M0(IP_NAME);                   // into Mode 0, override disable.
	} else {
         RID_FAIL();
	}
} 
#endif



#ifndef SET_NIC_MAIN_UP0_DEF
#define SET_NIC_MAIN_UP0_DEF
static inline void SET_NIC_MAIN_UP0(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
         SET_NIC_MAIN_UP0_M2(IP_NAME);                   // into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
         SET_NIC_MAIN_UP0_M1(IP_NAME);                   // into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
         SET_NIC_MAIN_UP0_M0(IP_NAME);                   // into Mode 0, override disable.
	} else {
         RID_FAIL();
	}
}
#endif



#ifndef SET_NIC_PAI_SET_DEF
#define SET_NIC_PAI_SET_DEF
static inline void SET_NIC_PAI_SET(unsigned int IP_NAME, unsigned int MODE) {
	if(IP_NAME >= 0x028) {
         SET_NIC_PAI_UP40((IP_NAME-0x028), MODE);          // into partition PAI  SECGRP1 Group Addr "PAII_NIC_S06"
	} else if(IP_NAME >= 0x020) {
         SET_NIC_PAI_UP32((IP_NAME-0x020), MODE);          // into partition PAI  SECGRP1 Group Addr "PAII_NIC_S05"
	} else if(IP_NAME >= 0x018) {
         SET_NIC_PAI_UP24((IP_NAME-0x018), MODE);          // into partition PAI  SECGRP1 Group Addr "PAII_NIC_S04"
	} else if(IP_NAME >= 0x010) {
         SET_NIC_PAI_UP16((IP_NAME-0x010), MODE);          // into partition PAI  SECGRP1 Group Addr "PAII_NIC_S03"
	} else if(IP_NAME >= 0x009) {
         SET_NIC_PAI_UP8((IP_NAME-0x008), MODE);         	// into partition PAI  SECGRP1 Group Addr "PAII_NIC_S02"
	} else {
         RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAI_UP8_DEF
#define SET_NIC_PAI_UP8_DEF
static inline void SET_NIC_PAI_UP8(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
         SET_NIC_PAI_UP8_M2(IP_NAME);                   // into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
         SET_NIC_PAI_UP8_M1(IP_NAME);                   // into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
         SET_NIC_PAI_UP8_M0(IP_NAME);                   // into Mode 0, override disable.
	} else {
         RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAI_UP16_DEF
#define SET_NIC_PAI_UP16_DEF
static inline void SET_NIC_PAI_UP16(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
         SET_NIC_PAI_UP16_M2(IP_NAME);                   // into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
         SET_NIC_PAI_UP16_M1(IP_NAME);                   // into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
         SET_NIC_PAI_UP16_M0(IP_NAME);                   // into Mode 0, override disable.
	} else {
         RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAI_UP24_DEF
#define SET_NIC_PAI_UP24_DEF
static inline void SET_NIC_PAI_UP24(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
         SET_NIC_PAI_UP24_M2(IP_NAME);                   // into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
         SET_NIC_PAI_UP24_M1(IP_NAME);                   // into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
         SET_NIC_PAI_UP24_M0(IP_NAME);                   // into Mode 0, override disable.
	} else {
         RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAI_UP32_DEF
#define SET_NIC_PAI_UP32_DEF
static inline void SET_NIC_PAI_UP32(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
         SET_NIC_PAI_UP32_M2(IP_NAME);                   // into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
         SET_NIC_PAI_UP32_M1(IP_NAME);                   // into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
         SET_NIC_PAI_UP32_M0(IP_NAME);                   // into Mode 0, override disable.
	} else {
         RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAI_UP40_DEF
#define SET_NIC_PAI_UP40_DEF
static inline void SET_NIC_PAI_UP40(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
         SET_NIC_PAI_UP40_M2(IP_NAME);                   // into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
         SET_NIC_PAI_UP40_M1(IP_NAME);                   // into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
         SET_NIC_PAI_UP40_M0(IP_NAME);                   // into Mode 0, override disable.
	} else {
         RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAII_SET_DEF
#define SET_NIC_PAII_SET_DEF
static inline void SET_NIC_PAII_SET(unsigned int IP_NAME, unsigned int MODE) {
	if(IP_NAME >= 0x038) {
         SET_NIC_PAII_UP56((IP_NAME-0x038), MODE);          // into partition PAII  SECGRP1 Group Addr "PAII_NIC_S08"
	} else if(IP_NAME >= 0x030) {
         SET_NIC_PAII_UP48((IP_NAME-0x030), MODE);          // into partition PAII  SECGRP1 Group Addr "PAII_NIC_S07"
	} else if(IP_NAME >= 0x02a) {
         SET_NIC_PAII_UP40((IP_NAME-0x028), MODE);         // into partition PAII  SECGRP1 Group Addr "PAII_NIC_S06"
	} else {
         RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAII_UP40_DEF
#define SET_NIC_PAII_UP40_DEF
static inline void SET_NIC_PAII_UP40(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
         SET_NIC_PAII_UP40_M2(IP_NAME);                   // into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
         SET_NIC_PAII_UP40_M1(IP_NAME);                   // into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
         SET_NIC_PAII_UP40_M0(IP_NAME);                   // into Mode 0, override disable.
	} else {
         RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAII_UP48_DEF
#define SET_NIC_PAII_UP48_DEF
static inline void SET_NIC_PAII_UP48(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
         SET_NIC_PAII_UP48_M2(IP_NAME);                   // into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
         SET_NIC_PAII_UP48_M1(IP_NAME);                   // into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
         SET_NIC_PAII_UP48_M0(IP_NAME);                   // into Mode 0, override disable.
	} else {
         RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAII_UP56_DEF
#define SET_NIC_PAII_UP56_DEF
static inline void SET_NIC_PAII_UP56(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
         SET_NIC_PAII_UP56_M2(IP_NAME);                   // into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
         SET_NIC_PAII_UP56_M1(IP_NAME);                   // into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
         SET_NIC_PAII_UP56_M0(IP_NAME);                   // into Mode 0, override disable.
	} else {
         RID_FAIL();
	}
}
#endif


//###############################################################################
//
//          	Setting / Get CA55
//
//###############################################################################
#ifndef SET_CA55_CORE0_RVBAR
#define SET_CA55_CORE0_RVBAR(regVal)         (SECGRP1_MAIN_REG->MAIN_CA55_S01 = regVal)
#endif

#ifndef SET_CA55_CORE1_RVBAR
#define SET_CA55_CORE1_RVBAR(regVal)         (SECGRP1_MAIN_REG->MAIN_CA55_S02 = regVal)
#endif

#ifndef SET_CA55_CORE2_RVBAR
#define SET_CA55_CORE2_RVBAR(regVal)         (SECGRP1_MAIN_REG->MAIN_CA55_S03 = regVal)
#endif

#ifndef SET_CA55_CORE3_RVBAR
#define SET_CA55_CORE3_RVBAR(regVal)         (SECGRP1_MAIN_REG->MAIN_CA55_S04 = regVal)
#endif

#ifndef GET_CA55_CORE0_RVBAR
#define GET_CA55_CORE0_RVBAR                 (SECGRP1_MAIN_REG->MAIN_CA55_S01)
#endif

#ifndef GET_CA55_CORE1_RVBAR
#define GET_CA55_CORE1_RVBAR                 (SECGRP1_MAIN_REG->MAIN_CA55_S02)
#endif

#ifndef GET_CA55_CORE2_RVBAR
#define GET_CA55_CORE2_RVBAR                 (SECGRP1_MAIN_REG->MAIN_CA55_S03)
#endif

#ifndef GET_CA55_CORE3_RVBAR
#define GET_CA55_CORE3_RVBAR                 (SECGRP1_MAIN_REG->MAIN_CA55_S04)
#endif

//###############################################################################
//
//          	Setting / Get CBDMA
//
//###############################################################################
#ifndef SET_CBDMA0_PROT_REGION_0
#define SET_CBDMA0_PROT_REGION_0(regVal)      (SET_CBDMA0_S01(regVal))
#endif

#ifndef GET_CBDMA0_PROT_REGION_0
#define GET_CBDMA0_PROT_REGION_0              (GET_CBDMA0_S01)
#endif

#ifndef SET_CBDMA0_PROT_REGION_1
#define SET_CBDMA0_PROT_REGION_1(regVal)      (SET_CBDMA0_S02(regVal))
#endif

#ifndef GET_CBDMA0_PROT_REGION_1
#define GET_CBDMA0_PROT_REGION_1              (GET_CBDMA0_S02)
#endif


#ifndef SET_CBDMA0_S01
#define SET_CBDMA0_S01(regVal)                (SECGRP1_PAI_REG->PAI_CBDMA0_S01 = regVal)
#endif

#ifndef GET_CBDMA0_S01
#define GET_CBDMA0_S01                        (SECGRP1_PAI_REG->PAI_CBDMA0_S01)
#endif

#ifndef SET_CBDMA0_S02
#define SET_CBDMA0_S02(regVal)                (SECGRP1_PAI_REG->PAI_CBDMA0_S02 = regVal)
#endif

#ifndef GET_CBDMA0_S02
#define GET_CBDMA0_S02                        (SECGRP1_PAI_REG->PAI_CBDMA0_S02)
#endif


//###############################################################################
//
//          	Setting / Get SDPROT
//
//###############################################################################
//##############################################
// 	BaseAddr : 0x1 (MB)
//  Mode     : 0 is All normal, 1 is All secure.
//  example  : SET_SDPROT(0x1, 1) --> 1 MB ~ 32 MB all protect.
//##############################################

#ifndef SET_SDPROT
#define SET_SDPROT(BaseAddr, Mode)            ({if (Mode == 1) {SET_SDPROT_BASE_ADDR(BaseAddr << 20); SET_SDPROT_S03(0x00000000);} else {SET_SDPROT_BASE_ADDR(BaseAddr << 20); SET_SDPROT_S03(0xffffffff);}})
#endif

#ifndef GET_SDPROT_ILL_ADDR
#define GET_SDPROT_ILL_ADDR                   (((GET_SDPROT_S05 & 0x1) << 32) || (GET_SDPROT_S04))
#endif

#ifndef GET_SDPROT_ILL_ID
#define GET_SDPROT_ILL_ID                     (GET_SDPROT_S06 & (0xffffffff >> (32-20))) // 20 it's mean vaild bits.
#endif

#ifndef CLEAR_SDPROT
#define CLEAR_SDPROT                          (SECGRP1_MAIN_REG->MAIN_SDPROT_S06 = 0x80000000)
#endif

#ifndef SET_SDPROT_BASE_ADDR
#define SET_SDPROT_BASE_ADDR(regVal)          {SET_SDPROT_S02((regVal & 0x100000000) >> 32); SET_SDPROT_S01(regVal & 0xffffffff)}
#endif

#ifndef GET_SDPROT_BASE_ADDR
#define GET_SDPROT_BASE_ADDR                 (((GET_SDPROT_S02 & 0x1) << 32) || (GET_SDPROT_S01))
#endif

#ifndef SET_SDPROT_S01
#define SET_SDPROT_S01(regVal)               (SECGRP1_MAIN_REG->MAIN_SDPROT_S01 = regVal)
#endif

#ifndef GET_SDPROT_S01
#define GET_SDPROT_S01                       (SECGRP1_MAIN_REG->MAIN_SDPROT_S01)
#endif

#ifndef SET_SDPROT_S02
#define SET_SDPROT_S02(regVal)               (SECGRP1_MAIN_REG->MAIN_SDPROT_S02 = regVal)
#endif

#ifndef GET_SDPROT_S02
#define GET_SDPROT_S02                       (SECGRP1_MAIN_REG->MAIN_SDPROT_S02)
#endif

#ifndef SET_SDPROT_S03
#define SET_SDPROT_S03(regVal)               (SECGRP1_MAIN_REG->MAIN_SDPROT_S03 = regVal)
#endif

#ifndef GET_SDPROT_S03
#define GET_SDPROT_S03                       (SECGRP1_MAIN_REG->MAIN_SDPROT_S03)
#endif

#ifndef GET_SDPROT_S04
#define GET_SDPROT_S04                      (SECGRP1_MAIN_REG->MAIN_SDPROT_S04)
#endif

#ifndef GET_SDPROT_S05
#define GET_SDPROT_S05                      (SECGRP1_MAIN_REG->MAIN_SDPROT_S05)
#endif

#ifndef GET_SDPROT_S06
#define GET_SDPROT_S06                     (SECGRP1_MAIN_REG->MAIN_SDPROT_S06)
#endif

#endif /* __INC_SECGRP1_Q645_H */
