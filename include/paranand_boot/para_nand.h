#ifndef PARA_NAND_H
#define PARA_NAND_H

#include <types.h>

#define FTNANDC024_Data_Port_Fixed
//#undef FTNANDC024_Data_Port_Fixed

#define FTNANDC024_FW_VERSION "ver:2017.10.30"

//#define MTD_Cmd_Idx
#define CTD_Cmd_Idx

#define TRANSFER_TYPE "PIO"

#define min_2(a,b) 		( (INT32)a < (INT32)b ? a: b )
#define max_2(a,b) 		( (INT32)a > (INT32)b ? a: b )
#define min_3(a,b,c)	( min_2(a,b) < (INT32)c ? min_2(a,b): c)
#define max_3(a,b,c)	( max_2(a,b) > (INT32)c ? max_2(a,b): c)
#define min_4(a,b,c,d)	( min_3(a,b,c) < (INT32)d ? min_3(a,b,c): d)
#define max_4(a,b,c,d)	( max_3(a,b,c) > (INT32)d ? max_3(a,b,c): d)

#define	isblank(ch)	(((ch) == ' ') || ((ch) == '\t'))
#define	CMDLEN				50
#define	MAXARGS				20

#define CMD_SUCCESS			0
#define CMD_ARGU_INVAILD	1
#define CMD_FAIL			2

#define FTNANDC024_DMA_LLD_STRUCT_ADDR	0x500000
#define FTNANDC024_BUFF_START_ADDR	(FTNANDC024_DMA_LLD_STRUCT_ADDR + 0x20000)

#define FTNANDC024_CORE_FREQ			200000000
#define FTNANDC024_MAX_CE			8
#define FTNANDC024_MAX_CHANNEL 			1 // 2 //HW config
#define FTNANDC024_NUM_BMC_REGION		1 // 2 //HW config
#define FTNANDC024_BASE				0xF8120000 //0xF8120000 //RF_AMBA(288, 0)
#define FTNANDC024_DATA_PORT_BASE		0xFB000000
//#define	FTNANDC024_IRQ					13

#define Trans_Read						0
#define Trans_Write						1

#define Trans_Ok						0
#define Trans_Err_Timeout				1
#define Trans_Err_DMA					2
#define Trans_Err_Other					3

// Define the following four for the return value in FTNANDC024_compare_data
#define Compare_ok				        0
#define Compare_spare_fail			    1
#define Compare_data_fail		 	    2
#define Compare_data_and_spare_fail		3

// For eD3
#define eD3_TLC_Lower					0
#define eD3_TLC_Mid						1
#define eD3_TLC_Upper					2

#define eD3_SLC							0
#define eD3_TLC							1

#define Comp_Data						0
#define Comp_Spare						1

// Command status
#define Cmd_status_cmd_complete					(1 << 0)
//#define Cmd_status_crc_failed					(1 << 1)
#define Cmd_status_status_failed				(1 << 2)
#define Cmd_status_ecc_correct_failed			(1 << 3)
#define Cmd_status_ecc_correct_failed_in_spare	(1 << 4)
#define Cmd_status_hit_threshold				(1 << 5)
#define Cmd_status_hit_threshold_in_spare		(1 << 6)
#define Cmd_status_ecc_error					(Cmd_status_ecc_correct_failed|Cmd_status_ecc_correct_failed_in_spare)
#define Cmd_status_err_occur					(Cmd_status_status_failed| Cmd_status_ecc_error)
//#define Cmd_status_err_occur					(Cmd_status_status_failed)

// Clock mode
#define Asynchronous_Mode				0
#define Synchronous_Mode 				1	//NV-DDR
#define NV_DDR2_Mode 				2

#define DMA_To_Dev						0
#define DMA_From_Dev					1

#define FTNANDC024_32BIT(offset)		*((volatile UINT32 *)(FTNANDC024_BASE + offset))
#define FTNANDC024_16BIT(offset)		*((volatile UINT16 *)(FTNANDC024_BASE + offset))
#define FTNANDC024_8BIT(offset)			*((volatile UINT8 *)(FTNANDC024_BASE + offset))

/* Offset for the region */
#define FTNANDC024_DATA_PORT(offset)	*((volatile UINT32 *)(FTNANDC024_DATA_PORT_BASE + offset))

#define OFFSET_ECC_STATUS				0x0000	//Read only

#define OFFSET_ECC_CTRL				0x0008
#define ecc_existence_between_sector	0
#define ecc_nonexistence_between_sector	1
#define ecc_base_512byte				0
#define ecc_base_1kbyte					1
#define ecc_disable						0
#define	ecc_enable						1
#define ecc_unmask						0
#define ecc_mask						1

//Threshold Number of ECC error bits
#define OFFSET_ECC_THRES		0x0010
#define	threshold_ecc_error_1bit		0
#define	threshold_ecc_error_2bit		1
#define	threshold_ecc_error_3bit		2
#define	threshold_ecc_error_4bit		3
#define	threshold_ecc_error_5bit		4
#define	threshold_ecc_error_6bit		5
#define	threshold_ecc_error_7bit		6
#define	threshold_ecc_error_8bit		7
#define	threshold_ecc_error_9bit		8
#define	threshold_ecc_error_10bit		9
#define	threshold_ecc_error_11bit		10
#define	threshold_ecc_error_12bit		11
#define	threshold_ecc_error_13bit		12
#define	threshold_ecc_error_14bit		13
#define	threshold_ecc_error_15bit		14
#define	threshold_ecc_error_16bit		15
#define	threshold_ecc_error_17bit		16
#define	threshold_ecc_error_18bit		17
#define	threshold_ecc_error_19bit		18
#define	threshold_ecc_error_20bit		19
#define	threshold_ecc_error_21bit		20
#define	threshold_ecc_error_22bit		21
#define	threshold_ecc_error_23bit		22
#define	threshold_ecc_error_24bit		23
#define threshold_ecc_spare_error_1bit	0		//offset 0x34
#define threshold_ecc_spare_error_2bit	1
#define threshold_ecc_spare_error_3bit	2
#define threshold_ecc_spare_error_4bit	3
#define threshold_ecc_spare_error_5bit	4
#define threshold_ecc_spare_error_6bit	5
#define threshold_ecc_spare_error_7bit	6
#define threshold_ecc_spare_error_8bit	7

//Number of ECC Correction Capability bits
#define OFFSET_ECC_CORR		0x0018
#define	correct_ecc_error_1bit			0
#define	correct_ecc_error_2bit			1
#define	correct_ecc_error_3bit			2
#define	correct_ecc_error_4bit			3
#define	correct_ecc_error_5bit			4
#define	correct_ecc_error_6bit			5
#define	correct_ecc_error_7bit			6
#define	correct_ecc_error_8bit			7
#define	correct_ecc_error_9bit			8
#define	correct_ecc_error_10bit			9
#define	correct_ecc_error_11bit			10
#define	correct_ecc_error_12bit			11
#define	correct_ecc_error_13bit			12
#define	correct_ecc_error_14bit			13
#define	correct_ecc_error_15bit			14
#define	correct_ecc_error_16bit			15
#define	correct_ecc_error_17bit			16
#define	correct_ecc_error_18bit			17
#define	correct_ecc_error_19bit			18
#define	correct_ecc_error_20bit			19
#define	correct_ecc_error_21bit			20
#define	correct_ecc_error_22bit			21
#define	correct_ecc_error_23bit			22
#define	correct_ecc_error_24bit			23
#define	correct_ecc_error_25bit			24
#define	correct_ecc_error_26bit			25
#define	correct_ecc_error_27bit			26
#define	correct_ecc_error_28bit			27
#define	correct_ecc_error_29bit			28
#define	correct_ecc_error_30bit			29
#define	correct_ecc_error_31bit			30
#define	correct_ecc_error_32bit			31
#define	correct_ecc_error_33bit			32
#define	correct_ecc_error_34bit			33
#define	correct_ecc_error_35bit			34
#define	correct_ecc_error_36bit			35
#define	correct_ecc_error_37bit			36
#define	correct_ecc_error_38bit			37
#define	correct_ecc_error_39bit			38
#define	correct_ecc_error_40bit			39
#define	correct_ecc_error_41bit			40
#define	correct_ecc_error_42bit			41
#define	correct_ecc_error_43bit			42
#define	correct_ecc_error_44bit			43
#define	correct_ecc_error_45bit			44
#define	correct_ecc_error_46bit			45
#define	correct_ecc_error_47bit			46
#define	correct_ecc_error_48bit			47
#define	correct_ecc_error_49bit			48
#define	correct_ecc_error_50bit			49
#define	correct_ecc_error_51bit			50
#define	correct_ecc_error_52bit			51
#define	correct_ecc_error_53bit			52
#define	correct_ecc_error_54bit			53
#define	correct_ecc_error_55bit			54
#define	correct_ecc_error_56bit			55
#define	correct_ecc_error_57bit			56
#define	correct_ecc_error_58bit			57
#define	correct_ecc_error_59bit			58
#define	correct_ecc_error_60bit			59
#define	correct_ecc_error_61bit			60
#define	correct_ecc_error_62bit			61
#define	correct_ecc_error_63bit			62
#define	correct_ecc_error_64bit			63

#define	correct_ecc_spare_error_1bit			0		//offset 0x3c
#define	correct_ecc_spare_error_2bit			1
#define	correct_ecc_spare_error_3bit			2
#define	correct_ecc_spare_error_4bit			3
#define	correct_ecc_spare_error_5bit			4
#define	correct_ecc_spare_error_6bit			5
#define	correct_ecc_spare_error_7bit			6
#define	correct_ecc_spare_error_8bit			7
#define	correct_ecc_spare_error_9bit			8
#define	correct_ecc_spare_error_10bit			9
#define	correct_ecc_spare_error_11bit			10
#define	correct_ecc_spare_error_12bit			11
#define	correct_ecc_spare_error_13bit			12
#define	correct_ecc_spare_error_14bit			13
#define	correct_ecc_spare_error_15bit			14
#define	correct_ecc_spare_error_16bit			15
#define	correct_ecc_spare_error_17bit			16
#define	correct_ecc_spare_error_18bit			17
#define	correct_ecc_spare_error_19bit			18
#define	correct_ecc_spare_error_20bit			19
#define	correct_ecc_spare_error_21bit			20
#define	correct_ecc_spare_error_22bit			21
#define	correct_ecc_spare_error_23bit			22
#define	correct_ecc_spare_error_24bit			23
#define	correct_ecc_spare_error_25bit			24
#define	correct_ecc_spare_error_26bit			25
#define	correct_ecc_spare_error_27bit			26
#define	correct_ecc_spare_error_28bit			27
#define	correct_ecc_spare_error_29bit			28
#define	correct_ecc_spare_error_30bit			29
#define	correct_ecc_spare_error_31bit			30
#define	correct_ecc_spare_error_32bit			31
#define	correct_ecc_spare_error_33bit			32
#define	correct_ecc_spare_error_34bit			33
#define	correct_ecc_spare_error_35bit			34
#define	correct_ecc_spare_error_36bit			35
#define	correct_ecc_spare_error_37bit			36
#define	correct_ecc_spare_error_38bit			37
#define	correct_ecc_spare_error_39bit			38
#define	correct_ecc_spare_error_40bit			39
#define	correct_ecc_spare_error_41bit			40
#define	correct_ecc_spare_error_42bit			41
#define	correct_ecc_spare_error_43bit			42
#define	correct_ecc_spare_error_44bit			43
#define	correct_ecc_spare_error_45bit			44
#define	correct_ecc_spare_error_46bit			45
#define	correct_ecc_spare_error_47bit			46
#define	correct_ecc_spare_error_48bit			47
#define	correct_ecc_spare_error_49bit			48
#define	correct_ecc_spare_error_50bit			49
#define	correct_ecc_spare_error_51bit			50
#define	correct_ecc_spare_error_52bit			51
#define	correct_ecc_spare_error_53bit			52
#define	correct_ecc_spare_error_54bit			53
#define	correct_ecc_spare_error_55bit			54
#define	correct_ecc_spare_error_56bit			55
#define	correct_ecc_spare_error_57bit			56
#define	correct_ecc_spare_error_58bit			57
#define	correct_ecc_spare_error_59bit			58
#define	correct_ecc_spare_error_60bit			59
#define	correct_ecc_spare_error_61bit			60
#define	correct_ecc_spare_error_62bit			61
#define	correct_ecc_spare_error_63bit			62
#define	correct_ecc_spare_error_64bit			63


#define OFFSET_ECC_INTR_EN				0x0020
#define hit_spare_threshold_intr_disable	0
#define hit_spare_threshold_intr_enable		1
#define correct_spare_fail_intr_disable		0
#define correct_spare_fail_intr_enable		1
#define hit_threshold_intr_disable			0
#define hit_threshold_intr_enable			1
#define correct_fail_intr_disable			0
#define correct_fail_intr_enable			1

#define OFFSET_ECC_INTR_STS		0x0024
#define OFFSET_ECC_INTR_STS_SP		0x0026

#define OFFSET_ECC_STATUS_CLR		0x0028	//clear 0x0 and 0x2c

#define OFFSET_ECC_STATUS_SP		0x002C	//Read only
#define OFFSET_ECC_THRES_SP		0x0034
#define OFFSET_ECC_CORR_SP			0x003C

#define OFFSET_DEV_STS				0x0100
#define OFFSET_GENERAL_SET			0x0104
#define each_channel_have_1CE			0
#define	each_channel_have_2CE			1
#define	each_channel_have_4CE			2
#define	each_channel_have_8CE			3
#define check_crc_in_spare				0
#define noncheck_crc_in_spare			1
#define busy_ready_bit_location_bit0	0
#define busy_ready_bit_location_bit1	1
#define busy_ready_bit_location_bit2	2
#define busy_ready_bit_location_bit3	3
#define busy_ready_bit_location_bit4	4
#define busy_ready_bit_location_bit5	5
#define busy_ready_bit_location_bit6	6
#define busy_ready_bit_location_bit7	7
#define cmd_pass_fail_bit_location_bit0	0
#define cmd_pass_fail_bit_location_bit1	1
#define cmd_pass_fail_bit_location_bit2	2
#define cmd_pass_fail_bit_location_bit3	3
#define cmd_pass_fail_bit_location_bit4	4
#define cmd_pass_fail_bit_location_bit5	5
#define cmd_pass_fail_bit_location_bit6	6
#define cmd_pass_fail_bit_location_bit7	7
#define flash_write_protect_disable		0
#define flash_write_protect_enable		1
#define data_inverse_mode_disable		0
#define data_inverse_mode_enable		1
#define data_scrambler_disable			0
#define data_scrambler_enable			1

#define OFFSET_MEM_ATTR_SET1		0x0108
#define BI_byte_mask				(0x7 << 19)  //lichun@add, from FTNANDC024 v2.3
#define device_size_1page_per_ce		0
#define device_size_2page_per_ce		1
#define device_size_4page_per_ce		2
#define device_size_8page_per_ce		3
#define device_size_16page_per_ce		4
#define device_size_32page_per_ce		5
#define device_size_64page_per_ce		6
#define device_size_128page_per_ce		7
#define device_size_256page_per_ce		8
#define device_size_512page_per_ce		9
#define device_size_1024page_per_ce		10
#define device_size_2048page_per_ce		11
#define device_size_4096page_per_ce		12
#define device_size_8192page_per_ce		13
#define device_size_16384page_per_ce	14
#define device_size_32768page_per_ce	15
#define device_size_65536page_per_ce	16
#define device_size_131072page_per_ce	17
#define device_size_262144page_per_ce	18
#define device_size_524288page_per_ce	19
#define device_size_1048576page_per_ce	20

#define spare_area_8bytes_per_512byte	8
#define spare_area_16bytes_per_512byte	16
/*	#define spare_area_128byte				128//0x80
	#define spare_area_218byte				218//0xDA
	#define spare_area_224byte				224//0xE0*/
#define block_size_16kbyte				16
#define block_size_64kbyte				64	//0
#define block_size_128kbyte				128	// 1, 0 for Toshiba
#define block_size_256kbyte				256	// 2, 1 for Toshiba
#define block_size_512kbyte				512	// 3, 2 for Toshiba
#define block_size_1Mbyte               1024 // , 3 for Toshiba
#define block_size_1_5Mbyte				1536
#define block_size_2Mbyte				2048
#define block_size_3Mbyte				3072
#define block_size_4Mbyte				4096
#define block_size_8Mbyte				8192

#define page_size_512byte				1	//0
#define page_size_2kbyte				2	// 1, 0 for Toshiba
#define page_size_4kbyte				4	// 2, 1 for Toshiba
#define page_size_8kbyte				8	// 3, 2 for Toshiba
#define page_size_16kbyte				16

// The unit of define vaule is Kbyte
#define plane_size_128Mbit				16384
#define plane_size_256Mbit				32768
#define plane_size_2Gbit				262144


#define plane_num_1						1
#define plane_num_2						2
#define plane_num_4						4
#define plane_num_8						8

#define Row_address_1cycle				0
#define Row_address_2cycle				1
#define Row_address_3cycle				2
#define Row_address_4cycle				3
#define Column_address_1cycle			0
#define Column_address_2cycle			1

#define OFFSET_MEM_ATTR_SET2			0x010C

#define OFFSET_AC_TIMING0(ch)			(0x0110 + (ch << 3))
#define OFFSET_AC_TIMING1(ch)			(0x0114 + (ch << 3))
#define OFFSET_AC_TIMING2(ch)			(0x0190 + (ch << 3))
#define OFFSET_AC_TIMING3(ch)			(0x0194 + (ch << 3))

#define OFFSET_INTR_EN					0x0150
#define flash_status_fail_intr_disable	0
#define flash_status_fail_intr_enable	1
#define OFFSET_INTR_STATUS				0x0154
#define OFFSET_CURR_ROW_ADDR(ch)		(0x0158 + (ch << 2 ))
#define OFFSET_READ_STS				0x0178
#define OFFSET_ADDR_TOGGLE_BIT		0x0180
#define OFFSET_SW_RESET				0x0184
#define OFFSET_AUTO_CMP_PAT			0x018C
#define OFFSET_VAR_ADDR				0x01D0
#define OFFSET_EXT_CTRL					0x01E0	//lichun@add, from FTNANDC024 v2.4
#define Seed_sel(ch)			(1 << ch)
#define OFFSET_CMDQ_STS				0x0200
#define OFFSET_CMDQ_FLUSH				0x0204
#define OFFSET_CMDCMPLT_CNT			0x0208	//command complete counter
#define OFFSET_CMDCMPLT_CNT_RST		0x020C

/* Command Queue for all channel: 0x0280~0x0294 */

/* Command Queue for specified channel */
#define OFFSET_CMDQ1(ch)				(0x0300 + (ch << 5 ))
#define OFFSET_CMDQ2(ch)				(0x0304 + (ch << 5 ))
#define OFFSET_CMDQ3(ch)				(0x0308 + (ch << 5 ))
#define OFFSET_CMDQ4(ch)				(0x030C + (ch << 5 ))
	/* Support Flash Type */
#define Legacy_flash					0
#define ONFI2_flash					1
#define ONFI3_flash					2
#define Toggle1_flash					3
#define Toggle2_flash					4
    /* Command Starting CE */
#define Start_From_CE0					0

    /* Byte Mode */
#define Byte_Mode_Enable				1
#define Byte_Mode_Disable				0
    /* Ignore BMC region status of full/empty */
#define Ignore_BMC_Region_Status		1
#define No_Ignore_BMC_Region_Status		0
    /* BMC region selection */
#define BMC_Region0					0

    /* Spare area size ( for sector, byte, and highspeed mode) */
#define Spare_size_5Byte				4

	/* Spare area size (just for page mode) */
#define Spare_size_Byte(x)		(((x - 1) &0x1F) << 19)
#define Spare_size_ExByte(x)		((((x - 1) >> 5) &0x3) << 24)  //from FTNANDC024 v2.4, the extended spare_num is 0x304 b[25:24]
#define Scramble_seed_val1(x)	((x & 0xff) << 24)  //from FTNANDC024 v2.4, FW to program scramble seed
#define Scramble_seed_val2(x)	(((x & 0x3fff) >> 8) << 26)
    /* Command Index */
#define Command(index)					(UINT16)index
    /* Program Flow Control */
#define Programming_Control_Flow		1
#define Fixed_Control_Flow				0
    /* Spare SRAM access region */
#define Spare_Sram_Region0				0
#define Spare_Sram_Region1				1
#define Spare_Sram_Region2				2
#define Spare_Srma_Region3				3
    /* Command Handshake mode */
#define Command_Handshake_Enable		1
#define Command_Handshake_Disable		0
    /* Command Incrememtal Scale */
#define Command_Inc_By_Twoblock			2
#define Command_Inc_By_Block			1
#define Command_Inc_By_Page				0
	/* Command Complete Interrupt */
#define Command_Complete_Intrrupt_Enable	1
#define Command_Complete_Intrrupt_Disable	0
#define OFFSET_CMDQ5(ch)				(0x0310 + (ch << 5 ))
#define OFFSET_CMDQ6(ch)				(0x0314 + (ch << 5 ))
	/* eD3 operation Type */
#define Samsung_TLC_Rd					4
#define Toshiba_TLC_Rd_Upp				3
#define Toshiba_TLC_Rd_Mid				2
#define Toshiba_TLC_Rd_Low				1
#define Toshiba_TLC_Rd_Dis				0
#define Samsung_3SLC_To_TLC_En			(1 << 11)
#define Toshiba_3SLC_To_TLC_En			(1 << 10)
	/* Copyback Index */
#define	eD3_Copyback_Index(x)			(x & 0x3FF)

//BMC region 0~7
#define OFFSET_BMC_STS					0x0400
#define OFFSET_BMC_UMPTR(n)			(0x404 + (n << 2))	//Region n User mode r/w pointer
#define OFFSET_BMC_DMA				0x0424	//DMA mode r/w
#define OFFSET_BMC_RESET				0x0428
#define OFFSET_BMC_FORCE_RD			0x042C

#define OFFSET_REV_NUM					0x0500	//revision
#define OFFSET_FEATURE					0x0504
#define Max_Spare_Data_128Byte			(1 << 15)  //lichun@add, from FTNANDC024 v2.4
#define AHB_Slave_Port_Sync				0
#define	AHB_Slave_Port_Async			1

#define OFFSET_AHB_MEM				0x0508	//AHB slave memory space
	/* AHB slave memory space range */
#define AHB_Memory_32KByte			(1 << 6)
	/* AHB enable prefech (used for DMA mode)*/
#define AHB_Prefetch_Slave_0			(1 << 12)
#define AHB_Prefetch_Slave_1			(1 << 13)
#define AHB_Prefetch_Slave_2			(1 << 14)
#define AHB_Prefetch_Slave_3			(1 << 15)

#define OFFSET_SW_RST				0x050C
#define OFFSET_AHB_RST				0x0510	//AHB data slave port (0~3) reset
#define OFFSET_DQS_DELAY			0x0520
#define OFFSET_OPCODE				0x0700
#define OFFSET_SPARE_SRAM			0x1000	//Spare data
#define OFFSET_DATA_SRAM			0x20000
#define OFFSET_PROG_SRAM			0x2000	//for Programmable flow

#define READ_ID					0x5F
#define RESET					0x65
#define PAGE_READ				0x1C
#define ERASE					0x68
#define PAGE_WRITE				0x54
#define PAGE_WRITE_WITH_SPARE			0x26

// ===================================== Variable and Struct =====================================
struct collect_data_buf {
	UINT8 *u8_data_buf;
	UINT8 *u8_spare_data_buf;
	UINT32 u32_data_length_in_bytes;
	UINT16 u16_data_length_in_sector;

	/* Information for reading or writing data */
	UINT16 u16_spare_length;
	UINT8 u8_spare_size_for_sector;
	UINT8 u16_col_addr;
	UINT8 u8_ce_num;
	UINT8 u8_data_region_num;
	UINT8 u8_spare_region_num;
	UINT8 u8_interleave_level;		// 1: 1I operation
									// 2: 2I operation
									// 4: 4I operation
	UINT8 u8_ce_inc_mode;			// Incremental poilcy for interleave
	UINT8 u8_ce_dec_mode;			// Decremental poilcy for interleave
	UINT8 u8_user_mode;				// 0: BMC isn't in User Mode
									// 1: BMC is in User Mode
	UINT8 u8_user_mode_pointer;		// 0~15 for sector 512Bytes
									// 0~7  for sector 1KBytes
	UINT8 u8_handshake_mode;
	UINT8 u8_bytemode;				// 0: Not bytemode, 1: Bytemode
	UINT8 u8_dsa_mode;				// 0: Normal write, 1: DSA mode
									// (DSA: Dynamic Spare Assignment)

};

struct flash_timing_setting {
        /* AC Timing 0 Register */
        INT16 tRES;
        INT16 tREH;
        INT16 tWP;
        INT16 tWH;
        /* AC Timing 1 Register */
        INT16 t1;
        INT16 tRLAT;
        INT16 tBSY;
        /* AC Timing 2 Register */
        INT16 tBUF4;
        INT16 tBUF3;
        INT16 tBUF2;
        INT16 tBUF1;
        /* AC Timing 3 Register */
        INT16 tPRE;
        INT16 tPST;
        INT16 tPSTH;
        INT16 tWRCK;
};

struct flash_timing_setting_new {
	/* AC Timing 0 Register */
	INT32 AC_Timing0;
	/* AC Timing 1 Register */
	INT32 AC_Timing1;
	/* AC Timing 2 Register */
	INT32 AC_Timing2;
	/* AC Timing 3 Register */
	INT32 AC_Timing3;
};

struct flash_timing_info {
	UINT8 tWH;
	UINT8 tCH;
	UINT8 tCLH;
	UINT8 tALH;
	UINT8 tCALH;
	UINT8 tWP;
	UINT8 tREH;
	UINT8 tCR;
	UINT8 tRSTO;
	UINT8 tREAID;
	UINT8 tREA;
	UINT8 tRP;
	UINT8 tWB;
	UINT8 tRB;
	UINT8 tWHR;
	UINT32 tWHR2;
	UINT8 tRHW;
	UINT8 tRR;
	UINT8 tAR;
	UINT8 tRC;
	UINT32 tADL;
	UINT8 tRHZ;
	UINT32 tCCS;
	UINT8 tCS;
	UINT8 tCS2;
	UINT8 tCLS;
	UINT8 tCLR;
	UINT8 tALS;
	UINT8 tCALS;
	UINT8 tCAL2;
	UINT8 tCRES;
	UINT8 tCDQSS;
	UINT8 tDBS;
	UINT32 tCWAW;
	UINT8 tWPRE;
	UINT8 tRPRE;
	UINT8 tWPST;
	UINT8 tRPST;
	UINT8 tWPSTH;
	UINT8 tRPSTH;
	UINT8 tDQSHZ;
	UINT8 tDQSCK;
	UINT8 tCAD;
	UINT8 tDSL;
	UINT8 tDSH;
	UINT8 tDQSL;
	UINT8 tDQSH;
	UINT8 tDQSD;
	UINT8 tCKWR;
	UINT8 tWRCK;
	UINT8 tCK;
	UINT8 tCALS2;
	UINT8 tDQSRE;
	UINT8 tWPRE2;
	UINT8 tRPRE2;
	UINT8 tCEH;
};

struct command_queue_feature {
	UINT32 Complete_interrupt_enable:1;	/* 0 */
	UINT32 Reversed1:1;					/* 1 */
	UINT32 Command_incremental_scale:2;	/* 3:2 */
	UINT32 Command_handshake_mode:1;	/* 4 */
	UINT32 Flash_type:3;				/* 7:5 */
	UINT32 Command_index:10;			/* 17:8 */
	UINT32 Program_flow:1;				/* 18 */
	UINT32 Spare_size:5;				/* 23:19 */
	UINT32 BMC_region:3;				/* 26:24 */
	UINT32 BMC_ignore:1;				/* 27 */
	UINT32 Byte_mode:1;					/* 28 */
	UINT32 Command_start_ce:3;			/* 31:29 */
//	UINT32 Spare_sram_region:2;				/* 31:30 */
};

struct command_queue {
	UINT32 Row_address1;
	UINT32 Row_address2;
	UINT32 Row_address3;
	UINT32 Row_address4;
	UINT8 Ce_decrease_value;
	UINT8 Ce_increase_value;
	UINT8 Sector_offset1;
	UINT8 Sector_offset2;
	UINT16 Sector_counter;
	struct command_queue_feature feature;
};

struct flash_id {
	/*Byte 0 */
	UINT8 ManufactureID;
	/*Byte 1 */
	UINT8 DeviceID;
	/*Byte 2 */
	UINT32 Num_of_die_per_ce:2;
	UINT32 Cell_type:2;
	UINT32 Num_of_simulataneously_prog_pages:2;
	UINT32 Support_interleave_between_multi_die:1;
	UINT32 Support_cache_prog:1;
	/*Byte 3 */
	UINT32 Page_size:2;
	UINT32 Spare_size:1;
	UINT32 Serial_access_lo:1;
	UINT32 Block_size:2;
	UINT32 Organization:1;
	UINT32 Serial_access_hi:1;
//      UINT8   Memory_attribute;
	/*Byte 4 */
	UINT32 Reserve1:2;
	UINT32 Plane_per_ce:2;
	UINT32 Plane_size:3;
	UINT32 Reserve2:1;
};

struct flash_info {
	UINT8 	u8_warmup_wr_cyc;			// 0: feature disable,	1: 1 warmup cyc.
	UINT8 	u8_warmup_rd_cyc;			// 2: 2 warmup cyc.,	3: 4 warmup cyc.

	UINT8 	u8_flash_access_mode;		// 0: Legacy Flash, 1: ONFI v2, 2: ONFI v3
										// 3: Toggle v1,	4: Toggle v2
	UINT8	u8_eD3_toshiba;
	UINT8   u8_sector_in_page;	//sector number in page
	UINT8   u8_plane_num;
	UINT8 	u8_firflash_ch;
	UINT8 	u8_firflash_ce;
	UINT16 u16_sector_in_block;	//sector number in block
	UINT16 u16_page_in_pseuplane;		// for Pseudo-plane in Toshiba eD3
	UINT16 u16_page_in_slcblock;		// for Samsung eD3
	UINT16 u16_page_in_block;
	UINT16 u16_page_boundary_in_block;	// for TLC
	UINT16 u16_sector_size;	// unint: B
	UINT16 u16_page_size;	// unint: KB
	UINT16 u16_block_size;	// unint: KB
	UINT16 u16_spare_size;
	UINT32 u32_spare_start_in_byte_pos;
	UINT32 u32_block_num_in_chip;
	UINT32 u32_page_num_in_chip;
	UINT32 u32_chip_size;				// The unit is Kbytes.
	UINT32 u32_ahb_memory_space;
	UINT32 u32_plane_size;				// The unit is Kbytes.
	struct flash_id raw_id;
};

typedef struct cmd {
	INT8 *name;
	INT8 *usage;
	UINT8(*func) (INT32 argc, INT8 ** argv, struct flash_info *);

} cmd_t;

// ===================================== Inner Variable and Struct =====================================

struct channel_status {
	volatile UINT8 u8_auto_compare_failed;
	volatile UINT8 u8_command_complete;
	volatile UINT32 u32_command_complete_counter;
	volatile UINT8 u8_status_failed;
	volatile UINT8 u8_ecc_correction_failed_for_data;
	volatile UINT8 u8_error_hit_threshold_for_data;
	volatile UINT8 u8_ecc_correction_failed_for_spare;
	volatile UINT8 u8_error_hit_threshold_for_spare;
};

#endif

