#ifndef HAL_SD_MMC_H
#define HAL_SD_MMC_H

#define ERR_SUCCESS		0
#define ERR_FAIL			-1
#define ERR_NEGATIVE_VALUE (-1)

//----------------------------------------------------------------------------
/**
 *	Response Type
 */
#define HW_RES_TYPE_0  0x00000000 // No response
#define HW_RES_TYPE_1  0x00000001 // 001b
#define HW_RES_TYPE_2  0x00000002 // 010b
#define HW_RES_TYPE_3  0x00000003 // 011b
#define HW_RES_TYPE_6  0x00000006 // 110b
#define HW_RES_TYPE_1B 0x00000007 // 111b


//----------------------------------------------------------------------------
/**
 *	SD/MMC Command ID
 */
#define CMD0 			0x00000000
#define CMD1 			0x00000001
#define CMD2 			0x00000002
#define CMD3 			0x00000003
#define CMD5 			0x00000005
#define CMD6 			0x00000006
#define CMD7 			0x00000007
#define CMD8 			0x00000008
#define CMD9 			0x00000009
#define CMD11 		0x0000000B
#define CMD12 		0x0000000C
#define CMD13 		0x0000000D
#define CMD16 		0x00000010
#define CMD17 		0x00000011
#define CMD18 		0x00000012
#define CMD24 		0x00000018
#define CMD25 		0x00000019
#define CMD55			0x00000037



/**
 *	Application CMD ID
 */
#define ACMD6			0xF0000006
#define ACMD13          0xF000000d
#define ACMD41		0xF0000029
#define ACMD51		0xF0000033


#define MMCCMD6		0x00000006
#define MMCCMD8		0x00000008

/**
 *	CMD Args
 */
#define ARGS_STUFF_BITS	0x00000000


/**
 *	CMD8: Voltage Supplied  Value Definition
 */
#define ARGS_VS_0B					0x00 // Not Defined
#define ARGS_VS_1B					0x01 // 2.7-3.6V
#define ARGS_VS_10B					0x02 // Reserved for Low Voltage Range
#define ARGS_VS_100B				0x04 // Reserved
#define ARGS_VS_1000B				0x08 // Reserved
#define ARGS_CHECK_PATTERN	0x000000AA

/**
 *	ACMD41: VDD Voltage Window
 */
#define ARGS_OCR_VDD_WIN_V27_28 	(0x01<<15) //Bit15: 2.7-2.8
#define ARGS_OCR_VDD_WIN_V28_29 	(0x01<<16) //Bit15: 2.8-2.9
#define ARGS_OCR_VDD_WIN_V29_30 	(0x01<<17) //Bit15: 2.9-3.0
#define ARGS_OCR_VDD_WIN_V30_31 	(0x01<<18) //Bit15: 3.0-3.1
#define ARGS_OCR_VDD_WIN_V31_32 	(0x01<<19) //Bit15: 3.1-3.2
#define ARGS_OCR_VDD_WIN_V32_33 	(0x01<<20) //Bit15: 3.2-3.3
#define ARGS_OCR_VDD_WIN_V33_34 	(0x01<<21) //Bit15: 3.3-3.4
#define ARGS_OCR_VDD_WIN_V34_35 	(0x01<<22) //Bit15: 3.4-3.5
#define ARGS_OCR_VDD_WIN_V35_36 	(0x01<<23) //Bit15: 3.5-3.6

#define ARGS_S18R(x)								(x<<24) // S18R: Switching to 1.8V Request 0: use current voltage 1: switch to 1.8V
#define ARGS_XPC(x)									(x<<28) // XPC: SDXC Power Control 0: power saving 1: maximum performance
#define ARGS_CARD_CAPACITY_STATUS		(30) // CCS
#define ARGS_CARD_POWER_UP_STATUS		(31) // Busy Bit
#define ARGS_HOST_CAPACITY_SUPPORT	(0x01<<30)

//----------------------------------------------------------------------------
/**
 *	SD's "State" value
 */
#define CURRENT_STATE_IDLE		0
#define CURRENT_STATE_READY		1
#define CURRENT_STATE_IDENT		2
#define CURRENT_STATE_STBY		3
#define CURRENT_STATE_TRAN		4
#define CURRENT_STATE_DATA		5
#define CURRENT_STATE_RCV			6
#define CURRENT_STATE_PRG			7
#define CURRENT_STATE_DIS			8
#define CARD_STATE_ERR_CODE		0xFDF98008 // Card status definition (CMD13)
//----------------------------------------------------------------------------
/**
 *	SD's "SCR" value
 */
#define SD_SPEC_V1_0_V1_01	0
#define SD_SPEC_V1_10				1
#define SD_SPEC_V2_00				2

/**
 *	MMC's "CSD" value
 */
#define MMC_SPEC_V1_0_V1_2  	0 // Implements system specification 1.0 - 1.2
#define MMC_SPEC_V1_4			    1 // Implements system specification 1.4
#define MMC_SPEC_V2_0_V2_2		2 // Implements system specification 2.0 - 2.2
#define MMC_SPEC_V3_1_V3_31		3 // Implements system specification 3.1 - 3.2 - 3.31
#define MMC_SPEC_V4_0_V4_1		4 // Implements system specification 4.0 - 4.1

/*
 * MMC_SWITCH access modes
 */
#define MMC_SWITCH_MODE_CMD_SET			0x00	/* Change the command set */
#define MMC_SWITCH_MODE_SET_BITS		0x01	/* Set bits which are 1 in value */
#define MMC_SWITCH_MODE_CLEAR_BITS	0x02	/* Clear bits which are 1 in value */
#define MMC_SWITCH_MODE_WRITE_BYTE	0x03	/* Set target to value */

/*
 * EXT_CSD fields
 */
#define EXT_CSD_ERASE_GROUP	175	/* R/W */
#define EXT_CSD_BOOT_CONFIG	179	/* R/W */
#define EXT_CSD_BUS_WIDTH		183	/* R/W */
#define EXT_CSD_HS_TIMING		185	/* R/W */
#define EXT_CSD_CARD_TYPE		196	/* RO */
#define EXT_CSD_REV					192	/* RO */
#define EXT_CSD_SEC_CNT			212	/* RO, 4 bytes */

/*
 * EXT_CSD field definitions
 */
#define EXT_CSD_CMD_SET_NORMAL		(1<<0)
#define EXT_CSD_CMD_SET_SECURE		(1<<1)
#define EXT_CSD_CMD_SET_CPSECURE	(1<<2)

#define EXT_CSD_CARD_TYPE_26	(1<<0)	/* Card can run at 26MHz */
#define EXT_CSD_CARD_TYPE_52	(1<<1)	/* Card can run at 52MHz */

#define EXT_CSD_BUS_WIDTH_1	0	/* Card is in 1 bit mode */
#define EXT_CSD_BUS_WIDTH_4	1	/* Card is in 4 bit mode */
#define EXT_CSD_BUS_WIDTH_8	2	/* Card is in 8 bit mode */

//----------------------------------------------------------------------------
/**
 *	R1 Code
 */
#define R1_OUT_OF_RANGE				(1 << 31)	/* er, c */
#define R1_ADDRESS_ERROR			(1 << 30)	/* erx, c */
#define R1_BLOCK_LEN_ERROR		(1 << 29)	/* er, c */
#define R1_ERASE_SEQ_ERROR		(1 << 28)	/* er, c */
#define R1_ERASE_PARAM				(1 << 27)	/* ex, c */
#define R1_WP_VIOLATION				(1 << 26)	/* erx, c */
#define R1_CARD_IS_LOCKED			(1 << 25)	/* sx, a */
#define R1_LOCK_UNLOCK_FAILED	(1 << 24)	/* erx, c */
#define R1_COM_CRC_ERROR			(1 << 23)	/* er, b */
#define R1_ILLEGAL_COMMAND		(1 << 22)	/* er, b */
#define R1_CARD_ECC_FAILED		(1 << 21)	/* ex, c */
#define R1_CC_ERROR						(1 << 20)	/* erx, c */
#define R1_ERROR							(1 << 19)	/* erx, c */
#define R1_UNDERRUN						(1 << 18)	/* ex, c */
#define R1_OVERRUN						(1 << 17)	/* ex, c */
#define R1_CID_CSD_OVERWRITE	(1 << 16)	/* erx, c, CID/CSD overwrite */
#define R1_WP_ERASE_SKIP			(1 << 15)	/* sx, c */
#define R1_CARD_ECC_DISABLED	(1 << 14)	/* sx, a */
#define R1_ERASE_RESET				(1 << 13)	/* sr, c */
#define R1_STATUS(x)					(x & 0xFFFFE000)
#define R1_CURRENT_STATE(x)		((x & 0x00001E00) >> 9)	/* sx, b (4 bits) */
#define R1_READY_FOR_DATA			(1 << 8)	/* sx, a */
#define R1_SWITCH_ERROR				(1 << 7)	/* sx, c */
#define R1_APP_CMD						(1 << 5)	/* sr, c */

//----------------------------------------------------------------------------
/**
 *	Kinds of cards
 */
#define WHAT_STORAGE_STANDARD_CAPACITY_SD		1
#define WHAT_STORAGE_HIGH_CAPACITY_SD				2
#define WHAT_STORAGE_STANDARD_CAPACITY_MMC	3
#define WHAT_STORAGE_HIGH_CAPACITY_MMC			4

//----------------------------------------------------------------------------
/**
 *	SD' Bus Width
 */
 #define BUS_WIDTH_1BIT	0
 #define BUS_WIDTH_4BIT 2

//----------------------------------------------------------------------------
/**
 *	Others
 */
#define TIME_OUT_LOOP_COUNT_SHORT	500
#define TIME_OUT_LOOP_COUNT				0x0fffff

#define BLOCK_LEN_BYTES_8		8
#define BLOCK_LEN_BYTES_64	64
#define BLOCK_LEN_BYTES_512	512

#define EMMC_ACCESS_USER_AREA					0x0
#define EMMC_ACCESS_BOOT_PARTITION_1	0x1
#define EMMC_ACCESS_BOOT_PARTITION_2	0x2
#define EMMC_ACCESS_RPMB							0x3
#define EMMC_ACCESS_GP_PARTITION_1		0x4
#define EMMC_ACCESS_GP_PARTITION_2		0x5
#define EMMC_ACCESS_GP_PARTITION_3		0x6
#define EMMC_ACCESS_GP_PARTITION_4		0x7

//#define MMC_MAX_READ_TIME    5000  // 5s
#define MMC_MAX_READ_TIME    1000  // 1s

//----------------------------------------------------------------------------
/**
 *
 */
struct REG_STATUS
{
//Bit 0~2: 2: reserved for application specific commands 1-0: reserved for manufacturer test mode
    unsigned int reserved_2b_1b_0b:3;
//Bit 3: Error in the sequence of the authentication process 0:no error 1:error ,(MMC reserved)
    unsigned int ake_seq_error:1;
//Bit 4: reserved for SD I/O card
    unsigned int reserved_4b:1;
//Bit 5: The card will expect ACMD or an indication that the coammnd has been interpreted as ACMD 0:disabled 1:enabled
    unsigned int app_cmd:1;
//Bit 6: If set, device needs to perform background operations urgently ,(SD reserved)
    unsigned int mmc_switch_error:1;
//Bit 7: If set, the card did not switch to the expected mode as requested by the SWITCH command ,(SD reserved)
    unsigned int mmc_urgent_bkops:1;
//Bit 8: Corresponds to buffer empty signaling on the bus 0:not ready 1:ready
    unsigned int ready_for_data:1;
//Bit 12~9: The state of the card when receiving the comand If the command execution causes a state change ,it will be
// visible to the host in the response to the next command .The four bits are interpreted as a binary coded number between 0 and 15
// 0: idle 1: ready : 2: ident 3:stby 4:tran 5:data 6:rcv 7:prg 8:dis 9-14:reserved 15:reserved for I/O mode
    unsigned int current_state:4;
//Bit 13: An erase sequence was cleared before executing because an out of erase sequence command was received 0: cleared  1:set
    unsigned int erase_reset:1;
//Bit 14: Teh command has been executed without using the internal ECC 0: enabled 1:disabled ,(MMC: reserved, must be set to 0)
    unsigned int card_ecc_disabled:1;
//Bit 15: Set when only partial address space was erased due to existing write protected blocks or the
//temporary or permanent write protected card was erased 0:not protected 1:protected
    unsigned int wp_erase_skip:1;
//Bit 16: Can be either one of the following errors:  0:no error 1:error
// - The read only section of the CSD does not match the card content
// - An attempt to reverse the copy (set as original) or permanent WP(unprotected) bits was made
    unsigned int csd_overwrite:1;
//Bit 17: The card could not sustain data transfer in stream read mode , (SD: reserved)
    unsigned int mmc_underrun:1;
//Bit 18: The card cound not sustain data programming in strea write mode ,(SD: reserved)
    unsigned int mmc_overrun:1;
//Bit 19: A general or an unknown error occurred during the operation 0:no error 1:error
    unsigned int error:1;
//Bit 20: Internal carc controller error 0:no error 1:error
    unsigned int cc_error:1;
//Bit 21: Card internal ECC was applied but failed to correct the data 0:no error 1:error
    unsigned int card_ecc_failed:1;
//Bit 22: Command not leafl for the card state 0:no error 1:error
    unsigned int illegal_command:1;
//Bit 23: The CRC check of the previous command failed 0:no error 1:error
    unsigned int com_crc_error:1;
//Bit 24: Set when a sequence or password error has been detected in lock/unlock card command 0:no error 1:error
    unsigned int lock_unlock_failed:1;
//Bit 25: When set, signals that the card is locked by the host 0:card unlocked 1:card locked
    unsigned int card_is_locked:1;
//Bit 26: Set when the host attempts to write to a protected block or to the temporary or permanent wirte protected card 0:no protected 1:protected
    unsigned int wp_violation:1;
//Bit 27: An invalid selection of write-blocks for erase occurred 0:no error 1:error
    unsigned int erase_param:1;
//Bit 28: An error in the sequence of erase commands occurred 0:no error 1:error
    unsigned int erase_seq_error:1;
//Bit 29: The transferred block length is not allowed for this card or the number of trnsferred bytes does not match the block length 0:no error 1:error
    unsigned int block_len_error:1;
//Bit 30: A misaligned address which did not match the block length was used in the command 0:no error 1:error
    unsigned int address_error:1;
// Bit 31: The command's argument was out of the allowed range for this card 0:no error 1:error
    unsigned int out_of_range:1;
};

//----------------------------------------------------------------------------
/**
 *
 */
struct REG_CID
{
    unsigned int		manfid;       // manufacturer ID
    unsigned char		prod_name[8]; // product name
    unsigned int		serial;       // product serial number
    unsigned short	oemid;        // OEM / Application  ID
    unsigned short	year;         // manufacturing date: year
    unsigned char		hwrev;        // product revision
    unsigned char		fwrev;
    unsigned char		month;        // manufacturing date : month
};
//----------------------------------------------------------------------------
/**
 *
 */
struct REG_CSD
{

    unsigned char		mmca_vsn;
    unsigned short	cmdclass;
    unsigned short	tacc_clks;
    unsigned int		tacc_ns;
    unsigned int		r2w_factor;
    unsigned int		max_dtr;
    unsigned int		read_blkbits;
    unsigned int		write_blkbits;
    unsigned int		capacity;
    //----------
    unsigned int		read_partial:1;
    unsigned int		read_misalign:1;
    unsigned int		write_partial:1;
    unsigned int		write_misalign:1;

};
//----------------------------------------------------------------------------
/**
 *
 */
struct REG_SCR
{
	unsigned char sd_spec;       // SD Memory Card - Spec. Version
	unsigned char sd_bus_widths; // DAT Bus widths supported
};
//----------------------------------------------------------------------------
/**
 *  Extended Card Specific Data(EXT_CSD)
 */
typedef struct PARTITION_CONFIG_s
{
  unsigned char partition_access:3; 			///< bit2-bit0
  unsigned char boot_partition_enable:3;	///< bit5-bit3
  unsigned char boot_ack:1; 							///< bit6
  unsigned char reserved:1; 							///< bit7
}PARTITION_CONFIG_t;

struct MMC_EXT_CSD
{
  unsigned int				hs_max_clk;
  unsigned int				capacity;
  unsigned char				boot_size_mult;
  unsigned char				rev;
  PARTITION_CONFIG_t	partition_config;		///< partition configuration
};

struct DEV_CARD
{
	int    RCA_Address;
	struct REG_CID reg_CID;
	struct REG_CSD reg_CSD;
	struct REG_SCR reg_SCR;
	struct REG_STATUS reg_STATUS;
	struct MMC_EXT_CSD ext_csd; /* mmc v4 extended card specific */
	unsigned int raw_cid[4];
	unsigned int raw_csd[4];
	unsigned int bus_width;
};


//----------------------------------------------------------------------------
/**
 *
 */
struct STORAGE_DEVICE
{
	int what_dev;
	struct DEV_CARD dev_card;
	unsigned int total_sector;
};

//----------------------------------------------------------------------------
// Function APIs
void
SetPeriClk(
	unsigned int pClk
);

void
SetDevBaseAdrs(
	unsigned int BaseAdrs
);

int
IdentifyStorage(
    void
);

void
ResetChipSDCtrler(
    void
);

int
StopChipSDCtrler(
    void
);

void
InitChipCtrl(
    void
);

unsigned int
GetTotalSector(
	void
);

int
ReadSDSingleSector(
    unsigned int SectorNum,
    unsigned char* pRecBuff
);

int
WriteSDSingleSector(
    unsigned int SectorNum,
    unsigned char* pSendBuff
);

int
ReadSDMultipleSector(
    unsigned int SectorIdx,
    unsigned int SectorNum,
    unsigned char* pRecBuff
);

int
WriteSDMultipleSector(
    unsigned int SectorIdx,
    unsigned int SectorNum,
    unsigned char* pSendBuff
);

// For boot mode

void
InitChipCtrl_Boot(
    void
);

int
SetMMCPartitionNum(
    unsigned char PartitionNum
);

int
GetBootPartitionNum(
	void
);

int
ReadeMMCBootMultipleSector(
    unsigned int SectorIdx,
    unsigned int SectorNum,
    unsigned char* pRecBuff
);

int
ReadSDMultipleSectorDma(
	unsigned int SectorIdx,
	unsigned int SectorNum,
	unsigned char* pRecBuff
);


int
WriteeMMCBootMultipleSector(
    unsigned int SectorIdx,
    unsigned int SectorNum,
    unsigned char* pSendBuff
);

void
SetChipCtrlClk(
	unsigned int peri_clk,
    unsigned int bus_clk
);

void
ClearChipCtrlStatusReg(
    void
);

void
sdTryNextReadDelay(
		int card_num
);

#endif		//HAL_SD_MMC_H
