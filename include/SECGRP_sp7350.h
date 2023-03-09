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
#ifndef __INC_SECGRP1_SP7350_H
#define __INC_SECGRP1_SP7350_H

//#include <common.h>

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
	UINT32  MAIN_CA55_S05;
    UINT32  MAIN_CBDMA0_S01;
    UINT32  MAIN_CBDMA0_S02;
    UINT32  MAIN_NIC_S01;
    UINT32  MAIN_NIC_S02;
    UINT32  MAIN_reserved_1[23];
};

struct SECGRP1_PAI_REGS {
    UINT32  PAI_NIC_S03;
    UINT32  PAI_reserved_1[31];
};

struct SECGRP1_PAII_REGS {
    UINT32  PAII_NIC_S04;  //G085.28;
    UINT32  PAII_NIC_S05;  //G085.29;
    UINT32  PAII_reserved_2[30];
};

#define SECGRP1_MAIN_REG    ((volatile struct SECGRP1_MAIN_REGS *)RF_GRP(SECGRP1_MAIN_RGST_NUM, 0))
#define SECGRP1_PAI_REG     ((volatile struct SECGRP1_PAI_REGS *)RF_GRP(SECGRP1_PAI_RGST_NUM, 0))
#define SECGRP1_PAII_REG    ((volatile struct SECGRP1_PAII_REGS *)RF_GRP(SECGRP1_PAII_RGST_NUM, 0))


#ifndef SECGRP1_MAIN_RGST_NUM
#define SECGRP1_MAIN_RGST_NUM                (113) 	// Set RGST group number.
#endif
	
#ifndef SECGRP1_PAI_RGST_NUM
#define SECGRP1_PAI_RGST_NUM                 (114) 	// Set RGST group number.
#endif
	
#ifndef SECGRP1_PAII_RGST_NUM
#define SECGRP1_PAII_RGST_NUM                (115) 	// Set RGST group number.
#endif
	
//G113.7	
#ifndef NPU_MA
#define NPU_MA                               (0x000)
#endif
	
#ifndef AXI_DMA_M0
#define AXI_DMA_M0                           (0x001)
#endif
	
#ifndef AXI_DMA_M1
#define AXI_DMA_M1                           (0x002)
#endif
	
#ifndef CBDMA_M0
#define CBDMA_M0                             (0x003)
#endif
	
#ifndef CPIOR0_M0
#define CPIOR0_M0                            (0x004)
#endif
	
#ifndef CPIOR1_M0
#define CPIOR1_M0                            (0x005)
#endif
	
#ifndef SEC_AES_MA
#define SEC_AES_MA                           (0x006)
#endif
	
#ifndef SEC_HASH_MA
#define SEC_HASH_MA                          (0x007)
#endif

//G113.8
#ifndef SEC_RSA_MA
#define SEC_RSA_MA                           (0x008)
#endif
	
#ifndef UART2AXI_MA
#define UART2AXI_MA                          (0x009)
#endif
	
#ifndef VI23_CSIIW0_MA
#define VI23_CSIIW0_MA                       (0x00a)
#endif
	
#ifndef VI23_CSIIW1_MA
#define VI23_CSIIW1_MA                       (0x00b)
#endif
	
#ifndef VI23_CSIIW2_MA
#define VI23_CSIIW2_MA                       (0x00c)
#endif
	
#ifndef VI23_CSIIW3_MA
#define VI23_CSIIW3_MA                       (0x00d)
#endif

//G114.0
#ifndef VCL_MA
#define VCL_MA                               (0x010)
#endif

//G115.0
#ifndef IMAGEAD0_MA
#define IMAGEAD0_MA                          (0x018)
#endif

#ifndef OSD0_MA
#define OSD0_MA                              (0x019)
#endif

#ifndef OSD1_MA
#define OSD1_MA                              (0x01a)
#endif

#ifndef OSD2_MA
#define OSD2_MA                              (0x01b)
#endif

#ifndef OSD3_MA
#define OSD3_MA                              (0x01c)
#endif

#ifndef VI0_CSIIW0_MA
#define VI0_CSIIW0_MA                        (0x01d)
#endif

#ifndef VI0_CSIIW1_MA
#define VI0_CSIIW1_MA                        (0x01e)
#endif

//G115.1
#ifndef VI1_CSIIW0_MA
#define VI1_CSIIW0_MA                        (0x020)
#endif

#ifndef VI1_CSIIW1_MA
#define VI1_CSIIW1_MA                        (0x021)
#endif

#ifndef VI4_CSIIW0_MA
#define VI4_CSIIW0_MA                        (0x022)
#endif

#ifndef VI4_CSIIW1_MA
#define VI4_CSIIW1_MA                        (0x023)
#endif

#ifndef VI5_CSIIW0_MA
#define VI5_CSIIW0_MA                        (0x024)
#endif

#ifndef VI5_CSIIW1_MA
#define VI5_CSIIW1_MA                        (0x025)
#endif

#ifndef VI5_CSIIW2_MA
#define VI5_CSIIW2_MA                        (0x026)
#endif

#ifndef VI5_CSIIW3_MA
#define VI5_CSIIW3_MA                        (0x027)
#endif

//###############################################################################
	
#ifndef SECGRP1_BASE_ZERO
#define SECGRP1_BASE_ZERO                    (0x00000000)
#endif
	
#ifndef SECGRP1_BASE_ONE 
#define SECGRP1_BASE_ONE                     (0x00000001)
#endif


//###############################################################################
//
// 			Setting NIC Override Function
//
//###############################################################################

static inline void SET_NIC_OVERRIDE(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_MAIN(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_MAIN_SET(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_MAIN_UP8(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_MAIN_UP0(unsigned int IP_NAME, unsigned int MODE);

static inline void SET_NIC_PAI_SET(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAI_UP16(unsigned int IP_NAME, unsigned int MODE);

static inline void SET_NIC_PAII_SET(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAII_UP24(unsigned int IP_NAME, unsigned int MODE);
static inline void SET_NIC_PAII_UP32(unsigned int IP_NAME, unsigned int MODE);


//###############################################################################
//
// 			Setting NIC Override Basic Define
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
#define SET_NIC_NOR_MAIN(IP_NAME, MODE)		 ({ CSTAMP(0x8200ffff); })
#endif


#ifndef SET_NIC_MAIN_UP8_M2
#define SET_NIC_MAIN_UP8_M2(IP_NAME)		(SECGRP1_MAIN_REG->MAIN_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_MAIN_UP8_M1
#define SET_NIC_MAIN_UP8_M1(IP_NAME)		(SECGRP1_MAIN_REG->MAIN_NIC_S02 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_MAIN_UP8_M0
#define SET_NIC_MAIN_UP8_M0(IP_NAME)		(SECGRP1_MAIN_REG->MAIN_NIC_S02 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif

#ifndef SET_NIC_MAIN_UP0_M2
#define SET_NIC_MAIN_UP0_M2(IP_NAME)		(SECGRP1_MAIN_REG->MAIN_NIC_S01 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_MAIN_UP0_M1
#define SET_NIC_MAIN_UP0_M1(IP_NAME)		(SECGRP1_MAIN_REG->MAIN_NIC_S01 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_MAIN_UP0_M0
#define SET_NIC_MAIN_UP0_M0(IP_NAME)		(SECGRP1_MAIN_REG->MAIN_NIC_S01 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif


#ifndef SET_NIC_PAI_UP16_M2
#define SET_NIC_PAI_UP16_M2(IP_NAME)		(SECGRP1_PAI_REG->PAI_NIC_S03 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP16_M1
#define SET_NIC_PAI_UP16_M1(IP_NAME)		(SECGRP1_PAI_REG->PAI_NIC_S03 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAI_UP16_M0
#define SET_NIC_PAI_UP16_M0(IP_NAME)		(SECGRP1_PAI_REG->PAI_NIC_S03 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif


#ifndef SET_NIC_PAII_UP24_M2
#define SET_NIC_PAII_UP24_M2(IP_NAME)		(SECGRP1_PAII_REG->PAII_NIC_S04 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP24_M1
#define SET_NIC_PAII_UP24_M1(IP_NAME)		(SECGRP1_PAII_REG->PAII_NIC_S04 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP24_M0
#define SET_NIC_PAII_UP24_M0(IP_NAME)		(SECGRP1_PAII_REG->PAII_NIC_S04 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP32_M2
#define SET_NIC_PAII_UP32_M2(IP_NAME)		(SECGRP1_PAII_REG->PAII_NIC_S05 = (SET_OVERRIDE_EN(IP_NAME) | SET_SEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP32_M1
#define SET_NIC_PAII_UP32_M1(IP_NAME)		(SECGRP1_PAII_REG->PAII_NIC_S05 = (SET_OVERRIDE_EN(IP_NAME) | SET_NOSEC_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME) | SET_MASKING_SEC_EN(IP_NAME)))
#endif

#ifndef SET_NIC_PAII_UP32_M0
#define SET_NIC_PAII_UP32_M0(IP_NAME)		(SECGRP1_PAII_REG->PAII_NIC_S05 = (SET_NOOVERRIDE_EN(IP_NAME) | SET_MASKING_OVERRIDE_EN(IP_NAME)))
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
	SET_NIC_MAIN(IP_NAME, MODE);					// into SECGRP1 Group
}
#endif

#ifndef SET_NIC_MAIN_DEF
#define SET_NIC_MAIN_DEF
static inline void SET_NIC_MAIN(unsigned int IP_NAME, unsigned int MODE) {
	if (IP_NAME < 0x010) {
		SET_NIC_MAIN_SET(IP_NAME, MODE); 			// into partition TOP  SECGRP1 Group
	} else if((IP_NAME >= 0x010) && (IP_NAME < 0x018)) {
		SET_NIC_PAI_SET(IP_NAME, MODE); 			// into partition PAI  SECGRP1 Group
	} else if((IP_NAME >= 0x018) && (IP_NAME < 0x028)) {
		SET_NIC_PAII_SET(IP_NAME, MODE); 			// into partition PAII SECGRP1 Group
	} else {
		RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_MAIN_SET_DEF
#define SET_NIC_MAIN_SET_DEF
static inline void SET_NIC_MAIN_SET(unsigned int IP_NAME, unsigned int MODE) {
	if(IP_NAME >= 0x008) {
		SET_NIC_MAIN_UP8((IP_NAME-0x008), MODE); 		// into partition TOP  SECGRP1 Group Addr "MAIN_NIC_S02"
	} else {
		SET_NIC_MAIN_UP0((IP_NAME), MODE); 			// into partition TOP  SECGRP1 Group Addr "MAIN_NIC_S01"
	}
}
#endif

#ifndef SET_NIC_MAIN_UP8_DEF
#define SET_NIC_MAIN_UP8_DEF
static inline void SET_NIC_MAIN_UP8(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_MAIN_UP8_M2(IP_NAME); 				// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_MAIN_UP8_M1(IP_NAME); 				// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_MAIN_UP8_M0(IP_NAME); 				// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
} 
#endif

#ifndef SET_NIC_MAIN_UP0_DEF
#define SET_NIC_MAIN_UP0_DEF
static inline void SET_NIC_MAIN_UP0(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_MAIN_UP0_M2(IP_NAME); 				// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_MAIN_UP0_M1(IP_NAME); 				// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_MAIN_UP0_M0(IP_NAME); 				// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif


#ifndef SET_NIC_PAI_SET_DEF
#define SET_NIC_PAI_SET_DEF
static inline void SET_NIC_PAI_SET(unsigned int IP_NAME, unsigned int MODE) {
	if((IP_NAME >= 0x010) && (IP_NAME < 0x018)){
		SET_NIC_PAI_UP16((IP_NAME-0x010), MODE); 		// into partition PAI  SECGRP1 Group Addr "PAII_NIC_S03"
	} else {
		RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAI_UP16_DEF
#define SET_NIC_PAI_UP16_DEF
static inline void SET_NIC_PAI_UP16(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_PAI_UP16_M2(IP_NAME); 				// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_PAI_UP16_M1(IP_NAME); 				// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_PAI_UP16_M0(IP_NAME); 				// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAII_SET_DEF
#define SET_NIC_PAII_SET_DEF
static inline void SET_NIC_PAII_SET(unsigned int IP_NAME, unsigned int MODE) {
	if(IP_NAME >= 0x020) {
		SET_NIC_PAII_UP24((IP_NAME-0x020), MODE); 		// into partition PAII  SECGRP1 Group Addr "PAII_NIC_S08"
	} else if(IP_NAME >= 0x018) {
		SET_NIC_PAII_UP32((IP_NAME-0x018), MODE); 		// into partition PAII  SECGRP1 Group Addr "PAII_NIC_S07"
	} else {
		RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAII_UP24_DEF
#define SET_NIC_PAII_UP24_DEF
static inline void SET_NIC_PAII_UP24(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_PAII_UP24_M2(IP_NAME); 				// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_PAII_UP24_M1(IP_NAME); 				// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_PAII_UP24_M0(IP_NAME); 				// into Mode 0, override disable.
	} else {
		RID_FAIL();
	}
}
#endif

#ifndef SET_NIC_PAII_UP32_DEF
#define SET_NIC_PAII_UP32_DEF
static inline void SET_NIC_PAII_UP32(unsigned int IP_NAME, unsigned int MODE) {
	if(MODE == 0x2) {
		SET_NIC_PAII_UP32_M2(IP_NAME); 				// into Mode 2, override enable and setting to security access.
	} else if(MODE == 0x1) {
		SET_NIC_PAII_UP32_M1(IP_NAME); 				// into Mode 1, override enable and setting to normal access.
	} else if(MODE == 0x0) {
		SET_NIC_PAII_UP32_M0(IP_NAME); 				// into Mode 0, override disable.
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
// 			Setting / Get CBDMA
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
#define SET_CBDMA0_S01(regVal)                (SECGRP1_MAIN_REG->MAIN_CBDMA0_S01 = regVal)
#endif

#ifndef GET_CBDMA0_S01
#define GET_CBDMA0_S01                        (SECGRP1_MAIN_REG->MAIN_CBDMA0_S01)
#endif

#ifndef SET_CBDMA0_S02
#define SET_CBDMA0_S02(regVal)                (SECGRP1_MAIN_REG->MAIN_CBDMA0_S02 = regVal)
#endif

#ifndef GET_CBDMA0_S02
#define GET_CBDMA0_S02                        (SECGRP1_MAIN_REG->MAIN_CBDMA0_S02)
#endif


#endif /* __INC_SECGRP1_SP7350_H */
