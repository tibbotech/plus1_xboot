#ifndef NFDRIVER_H
#define NFDRIVER_H

#include <config.h>
#include <nand_boot/nandop.h>

#define BCH_ENCODE				0
#define BCH_DECODE				1

#define BCH_S338_CFG					(BCH_S338_BASE_ADDRESS + 0x00)
#define BCH_S338_DATA_PTR				(BCH_S338_BASE_ADDRESS + 0x04)
#define BCH_S338_PARITY_PTR				(BCH_S338_BASE_ADDRESS + 0x08)
#define BCH_S338_INT_STATUS				(BCH_S338_BASE_ADDRESS + 0x0C)
#define BCH_S338_SOFT_RESET				(BCH_S338_BASE_ADDRESS + 0x10)
#define BCH_S338_INT_MASK				(BCH_S338_BASE_ADDRESS + 0x14)
#define BCH_S338_REPORT_STATUS			(BCH_S338_BASE_ADDRESS + 0x18)
#define BCH_S338_SECTOR_ERR_REPORT		(BCH_S338_BASE_ADDRESS + 0x1C)
#define BCH_S338_SECTOR_FAIL_REPORT		(BCH_S338_BASE_ADDRESS + 0x20)
#define BCH_S338_CONTROL_REG			(BCH_S338_BASE_ADDRESS + 0x2C)

#define BCH_S338_ENCODE (0)
#define BCH_S338_DECODE (1)

typedef enum
{
	BCH_S338_1K60_BITS_MODE		=0x00,
	BCH_S338_1K40_BITS_MODE		=0x01,
	BCH_S338_1K24_BITS_MODE		=0x02,
	BCH_S338_1K16_BITS_MODE		=0x03,
	BCH_S338_512B8_BITS_MODE	=0x04,
	BCH_S338_512B4_BITS_MODE	=0x05,
	BCH_S338_TOTAL						=0x06,
	BCH_S338_OFF							=0xFF
}BCH_MODE_ENUM;

#define BCH_S338_RESET					1
#define BCH_S338_START					1
#define BCH_S338_ENC_DEC(x)				((x) << 4)
#define BCH_S338_CORRECT_MODE(x)		((x) << 8)
#define BCH_S338_DATA_MODE(x)				((x) << 11)
#define BCH_S338_SECTOR_NUMBER(x)		((x) << 16)
#define BCH_S338_SOFTWARE_PROTECTED_BYTES(x)	((x) << 28)

#define BCH_S338_INT  (1)
#define BCH_S338_BUSY (1 << 4)

#define BCH_S338_FINISH_MASK(x)			(x)
#define BCH_S338_DECODE_FAIL_MASK(x)	((x) << 1)

#define BCH_S338_FINISH (1)
#define BCH_S338_DECODE_FAIL (1 << 4)

enum bch_s338_ret_value {
	ret_BCH_S338_OK   = 0,
	ret_BCH_S338_FAIL = -1
};

/* NAND register */
//Test Channe register only for test
#define FM_CSR          (NAND_S330_BASE + 0x00)
#define FM_DESC_BA      (NAND_S330_BASE + 0x04)
#define FM_AC_TIMING0    			(NAND_S330_BASE + 0x08)
#define FM_RDYBSY_EN    (NAND_S330_BASE + 0x0c)
#define FM_PIO_CTRL1    (NAND_S330_BASE + 0x10)
#define FM_PIO_CTRL2    (NAND_S330_BASE + 0x14)
#define FM_PIO_CTRL3    (NAND_S330_BASE + 0x18)
#define FM_PIO_CTRL4    (NAND_S330_BASE + 0x1c)
#define FM_PIO_CTRL5    (NAND_S330_BASE + 0x20)
#define FM_PIO_CTRL6    (NAND_S330_BASE + 0x24)
#define FM_PIO_CTRL7    (NAND_S330_BASE + 0x28)
#define FM_PIO_CTRL8    (NAND_S330_BASE + 0x2c)
#define FM_TGL_CTRL     			(NAND_S330_BASE + 0x30)
#define FM_SWITCH_CTRL  			(NAND_S330_BASE + 0x34)
#define FM_BIT0_TOLERANCE_CNT	(NAND_S330_BASE + 0x38)
#define FM_INTRMSK      (NAND_S330_BASE + 0x40)
#define FM_INTR_STS     (NAND_S330_BASE + 0x44)
#define FM_RDYBSY_DLY_INT	(NAND_S330_BASE + 0x48)
#define FM_RANDOM_EN		(NAND_S330_BASE + 0x50)
#define FM_PRBS_COE		(NAND_S330_BASE + 0x54)
#define FM_SEEDGEN_CTRL		(NAND_S330_BASE + 0x58)
#define FM_PRE_NUM0		(NAND_S330_BASE + 0x5C)
#define FM_PRE_NUM1		(NAND_S330_BASE + 0x60)
#define FM_PRE_NUM2		(NAND_S330_BASE + 0x64)
#define FM_PRE_NUM3		(NAND_S330_BASE + 0x68)
#define FM_PRE_NUM4		(NAND_S330_BASE + 0x6C)
#define FM_PRE_NUM5		(NAND_S330_BASE + 0x70)
#define FM_PRE_NUM6		(NAND_S330_BASE + 0x74)
#define FM_PRE_NUM7		(NAND_S330_BASE + 0x78)
#define FM_PIO1_PAGE_ADDR			(NAND_S330_BASE + 0x7C)
#define FM_PIO1_LOAD_SEED			(NAND_S330_BASE + 0x80)
#define FM_SUCMD_WAIT_CTRL		(NAND_S330_BASE + 0x84)
#define FM_STS_RD_STS					(NAND_S330_BASE + 0x88)
#define FM_AC_TIMING1					(NAND_S330_BASE + 0x8C)

#define rFM_CSR         (*(volatile unsigned *)FM_CSR)
#define rFM_DESC_BA     (*(volatile unsigned *)FM_DESC_BA)
#define rFM_AC_TIMING0					(*(volatile unsigned *)FM_AC_TIMING0)
#define rFM_RDYBSY_EN   (*(volatile unsigned *)FM_RDYBSY_EN)
#define rFM_PIO_CTRL1   (*(volatile unsigned *)FM_PIO_CTRL1)
#define rFM_PIO_CTRL2   (*(volatile unsigned *)FM_PIO_CTRL2)
#define rFM_PIO_CTRL3   (*(volatile unsigned *)FM_PIO_CTRL3)
#define rFM_PIO_CTRL4   (*(volatile unsigned *)FM_PIO_CTRL4)
#define rFM_PIO_CTRL5   (*(volatile unsigned *)FM_PIO_CTRL5)
#define rFM_PIO_CTRL6   (*(volatile unsigned *)FM_PIO_CTRL6)
#define rFM_PIO_CTRL7   (*(volatile unsigned *)FM_PIO_CTRL7)
#define rFM_PIO_CTRL8   (*(volatile unsigned *)FM_PIO_CTRL8)
#define rFM_TGL_CTRL     				(*(volatile unsigned *)FM_TGL_CTRL)
#define rFM_SWITCH_CTRL  				(*(volatile unsigned *)FM_SWITCH_CTRL)
#define rFM_BIT0_TOLERANCE_CNT	(*(volatile unsigned *)FM_BIT0_TOLERANCE_CNT)
#define rFM_INTRMSK     (*(volatile unsigned *)FM_INTRMSK)
#define rFM_INTR_STS    (*(volatile unsigned *)FM_INTR_STS)
#define rFM_RDYBSY_DLY_INT    (*(volatile unsigned *)FM_RDYBSY_DLY_INT)
#define rFM_RANDOM_EN    (*(volatile unsigned *)FM_RANDOM_EN)
#define rFM_PRBS_COE    (*(volatile unsigned *)FM_PRBS_COE)
#define rFM_SEEDGEN_CTRL    (*(volatile unsigned *)FM_SEEDGEN_CTRL)
#define rFM_PRE_NUM0    (*(volatile unsigned *)FM_PRE_NUM0)
#define rFM_PRE_NUM1    (*(volatile unsigned *)FM_PRE_NUM1)
#define rFM_PRE_NUM2    (*(volatile unsigned *)FM_PRE_NUM2)
#define rFM_PRE_NUM3    (*(volatile unsigned *)FM_PRE_NUM3)
#define rFM_PRE_NUM4    (*(volatile unsigned *)FM_PRE_NUM4)
#define rFM_PRE_NUM5    (*(volatile unsigned *)FM_PRE_NUM5)
#define rFM_PRE_NUM6    (*(volatile unsigned *)FM_PRE_NUM6)
#define rFM_PRE_NUM7    (*(volatile unsigned *)FM_PRE_NUM7)
#define rFM_PIO1_PAGE_ADDR			(*(volatile unsigned *)FM_PIO1_PAGE_ADDR)
#define rFM_PIO1_LOAD_SEED			(*(volatile unsigned *)FM_PIO1_LOAD_SEED)
#define rFM_SUCMD_WAIT_CTRL			(*(volatile unsigned *)FM_SUCMD_WAIT_CTRL)
#define rFM_STS_RD_STS					(*(volatile unsigned *)FM_STS_RD_STS)
#define rFM_AC_TIMING1					(*(volatile unsigned *)FM_AC_TIMING1)


#define NF_ERASE			1
#define NF_READ				2
#define NF_WRITE			3
#define NF_READID			4
#define NF_READSTATUS	5

#define NAND_CS0	0
#define NAND_CS1	1
#define NAND_CS2	2
#define NAND_CS3	3

#define	ND_INTR_DESC_DONE		(1<<0)
#define	ND_INTR_DESC_END		(1<<1)
#define	ND_INTR_DESC_ERROR		(1<<2)
#define	ND_INTR_DESC_INVALID	(1<<3)
#define ND_INTR_DESC_RB1 		(1<<12)
#define ND_INTR_DESC_RB2 		(1<<13)
#define ND_INTR_DESC_RB3 		(1<<14)
#define ND_INTR_DESC_RB4 		(1<<15)
#define ND_INTR_DESC_RB 		(ND_INTR_DESC_RB1|ND_INTR_DESC_RB2)
#define ND_INTR_DESC_NFC_BUSY 	(1<<7)

#define NAND_UNKNOWN			0
#define NAND_TOSHIBA			0x98
#define NAND_SAMSUNG      0xEC
#define NAND_THOMSON			0x20
#define NAND_HYNIX				0xAD
#define NAND_INFINEON			0xC1
#define NAND_MICRON				0x2C
#define NAND_INTEL				0x89
#define NAND_MXIC					0xC2
#define NAND_SANDISK			0x45
#define NAND_POWERFLASH   0x92

#define MAUNCMD_RESET	0xFF00
#define MAUNCMD_READID	0x9000
#define MAUNCMD_BLOCK_ERASE	0x6000
#define MAUNCMD_CONF_BLOCK_ERASE	0xD000
#define MAUNCMD_WRITE	0x8000
#define MAUNCMD_CONF_WRITE	0x1000
#define MAUNCMD_WRITE_TWO_PLAN	0x8100
#define MAUNCMD_CONF_WRITE_TWO_PLAN	0x1100


#define AUTOCMD_BLOCK_ERASE					0x60D0
//#define AUTOCMD_BLOCK_ERASE_TWOPLAN_BEGIN	0x6060
//#define AUTOCMD_BLOCK_ERASE_TWOPLAN_END		0xD000

#define AUTOCMD_READ 						0x0030
#define AUTOCMD_WRITE 						0x8010
#define AUTOCMD_WRITE_TWOPLAN_BEGIN 		0x8011
#define AUTOCMD_WRITE_TWOPLAN_END 			0x8110

#define NF_BEGIN 0
#define NF_END 1


//MCD_TYPE
#define CMD_TYPE_READ 0x1
#define CMD_TYPE_WRITE 0x2
#define CMD_TYPE_ERASE 0x3
#define CMD_TYPE_READ_STATUS 0x4
#define CMD_TYPE_HALFAUTO_ERASE 0x5 //HalfAutoErase
#define CMD_TYPE_HALFAUTO_READ 0x5
#define CMD_TYPE_HALFAUTO_WRITE 0x6//HalfAutoWrite
#define CMD_TYPE_MANUAL_MODE_CMD 0x7
#define CMD_TYPE_MANUAL_MODE_ADDR 0x8
#define CMD_TYPE_MANUAL_MODE_PYLOAD_WRITE 0x9
#define CMD_TYPE_MANUAL_MODE_PYLOAD_READ 0xa
#define CMD_TYPE_MANUAL_MODE_REDUNT_WRITE 0xb
#define CMD_TYPE_MANUAL_MODE_REDUNT_READ 0xc
#define CMD_TYPE_SUPER_MANUAL_MODE 0xd
#define CMD_TYPE_MANUAL_MODE_STS_RD 0xf


#define MANUAL_MODE_OWNER 0x80
#define MANUAL_MODE_END_DESC 0x40
#define MANUAL_MODE_LAST_SECTOR 0x20
#define MANUAL_MODE_REDUNT_ENABLE 0x10
#define MANUAL_MODE_ADDRNUMBER_MASK 0x7


#define DESC_OFFSET 8


#define DESC_CMD 0
#define DESC_LENGTH 1
#define DESC_INTERRUPT 2
#define DESC_ADDR1 3
#define DESC_ADDR2 4
#define DESC_PAYLOAD 5
#define DESC_REDUNT 6
#define DESC_REDUNT_INFO 7


#define MAX_NAND_WAIT_CNT (200*1000) //200ms

#define NAND_WAIT_CNT 100 //10 us
#define MAX_NAND_TIMEOUT 5

#define MAX_BCH_WAIT_CNT (200*1000) //200ms
#define BCH_WAIT_CNT 100 //10 us

typedef struct SmallBlkInfo
{
	UINT16 PhyPageLen;
	UINT16 PhyPageNoPerBlk;

	UINT16 PhyTotalBlkNo;
	UINT16 LogPageLen;

	UINT16 LogPageNoPerBlk;
	UINT16 LogTotalBlkNo;

	UINT16 PhyBlkPerLogBlk;
	UINT16 PhyPagePerLogBlk;

	UINT8 PhyPagePerLogPage;
	UINT8 PhyPageLenShift;
	UINT8 PhyPageNoPerBlkShift;
	UINT8 PhyPagePerLogPageShift;

	UINT8 PhyBlkPerLogBlkShift;
	UINT8 PhyPagePerLogBlkShift;
	UINT16 PhyReduntLenLog;

	UINT32 reserved[2];

}SmallBlkInfo_t;

typedef struct desc_u32_0_field_read_status_set_s {
	UINT32	cmd_1: 8;
	UINT32	cmd_0: 8;
	UINT32	have_addr: 1;
	UINT32	reserve: 7;
	UINT32	multi_func_sel: 4;
	UINT32	cmd_type: 4;
} desc_u32_0_field_read_status_set_t;

typedef struct desc_u32_0_field_sts_rd_s {
	UINT32	cmd_1: 8;
	UINT32	cmd_0: 8;
	UINT32	have_addr: 1;
	UINT32	enable_repeat_sts_rd: 1;
	UINT32	enable_repeat_till_time_out: 1;
	UINT32	enable_repeat_till_condition_match_or_time_out: 1;
	UINT32	reserve: 4;
	UINT32	multi_func_sel: 4;
	UINT32	cmd_type: 4;
} desc_u32_0_field_sts_rd_t;

typedef struct desc_u32_0_field_super_cmd_ctl_s {
	UINT32	cmd_1: 8;
	UINT32	cmd_0: 8;
	UINT32	is_two_command: 1;
	UINT32	have_addr: 1;
	UINT32	wait_ready: 1;
	UINT32	have_read: 1;
	UINT32	have_write: 1;
	UINT32	reserve:3;
	UINT32	multi_func_sel: 4;
	UINT32	cmd_type: 4;
} desc_u32_0_field_super_cmd_ctl_t;

typedef struct desc_u32_0_field_s {
	UINT32	cmd_1: 8;
	UINT32	cmd_0: 8;
	UINT32	read_status_data: 8;
	UINT32	multi_func_sel: 4;
	UINT32	cmd_type: 4;
} desc_u32_0_field_t;


typedef union desc_u32_0_s {
	UINT32 							u32;
	desc_u32_0_field_t  field;
	desc_u32_0_field_read_status_set_t field_read_status_set;
	desc_u32_0_field_sts_rd_t field_sts_rd;
	desc_u32_0_field_super_cmd_ctl_t field_super_cmd_ctl;
} desc_u32_0_t;


typedef struct desc_u32_1_field_sts_rd_s {
	UINT32	stop_condition_mask: 8;
	UINT32	stop_condition_value: 8;
	UINT32	stop_condition_match: 1;
	UINT32	time_out_occur: 1;
	UINT32	reserve:14;
} desc_u32_1_field_sts_rd_t;

typedef struct desc_u32_1_field_s {
	UINT32	redunt_length: 16;
	UINT32	payload_length: 16;
} desc_u32_1_field_t;

typedef union desc_u32_1_s {
	UINT32 							u32;
	desc_u32_1_field_t  field;
	desc_u32_1_field_sts_rd_t field_sts_rd;
} desc_u32_1_t;

typedef struct desc_u32_2_field_s {
	UINT32	interrupt_status: 16;
	UINT32	interrupt_mask: 16;
} desc_u32_2_field_t;

typedef union desc_u32_2_s {
	UINT32 							u32;
	desc_u32_2_field_t  field;
} desc_u32_2_t;

typedef struct desc_u32_3_field_s {
	UINT32	addr_4: 8;
	UINT32	addr_5: 8;
	UINT32	data_sector_number: 7;
	UINT32	data_structure_type: 1;
	UINT32	addr_number: 3;
	UINT32	check_ff: 1;
	UINT32	redunt_enable:1;
	UINT32	cs_change_ctrl: 1;
	UINT32	end_desc: 1;
	UINT32	owner: 1;
} desc_u32_3_field_t;

typedef union desc_u32_3_s {
	UINT32 							u32;
	desc_u32_3_field_t  field;
} desc_u32_3_t;

typedef struct desc_u32_4_field_s {
	UINT32	addr_0: 8;
	UINT32	addr_1: 8;
	UINT32	addr_2: 8;
	UINT32	addr_3: 8;
} desc_u32_4_field_t;


typedef union desc_u32_4_s {
	UINT32 							u32;
	desc_u32_4_field_t  field;
} desc_u32_4_t;

typedef struct desc_u32_5_field_sts_rd_s {
	UINT32	interval: 8;
	UINT32	time_out_number: 24;
} desc_u32_5_field_sts_rd_t;

typedef struct desc_u32_5_field_s {
	UINT32  						payload_bp;
} desc_u32_5_field_t;

typedef union desc_u32_5_s {
	UINT32 							u32;
	desc_u32_5_field_t  field;
	desc_u32_5_field_sts_rd_t field_sts_rd;
} desc_u32_5_t;

typedef struct desc_u32_6_field_s {
	UINT32  						redunt_bp;
} desc_u32_6_field_t;

typedef union desc_u32_6_s {
	UINT32 							u32;
	desc_u32_6_field_t  field;
} desc_u32_6_t;

typedef struct desc_u32_7_field_s {
	UINT32	next_desc_offset: 14;
	UINT32	randomizer_en: 1;
	UINT32	ac_timing_sel: 1;
	UINT32	redunt_sector_address_offset: 16;
} desc_u32_7_field_t;

typedef union desc_u32_7_s {
	UINT32 							u32;
	desc_u32_7_field_t  field;
} desc_u32_7_t;

typedef struct desc_command_s {
	desc_u32_0_t	u32_0;
	desc_u32_1_t	u32_1;
	desc_u32_2_t	u32_2;
	desc_u32_3_t	u32_3;
	desc_u32_4_t	u32_4;
	desc_u32_5_t	u32_5;
	desc_u32_6_t	u32_6;
	desc_u32_7_t	u32_7;
} desc_command_t;

#define HAL_NAND_ADDR_COL_1_BYTE	(1<<0)
#define HAL_NAND_ADDR_COL_2_BYTE	(1<<1)
#define HAL_NAND_ADDR_COL_ONLY		(1<<2)
#define HAL_NAND_ADDR_ROW_ONLY		(1<<3)

#endif
