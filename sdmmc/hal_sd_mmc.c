/******************************************************************************
 *                          Include File
 *******************************************************************************/
#include <nand_boot/nandop.h>
#include <sdmmc_boot/hal_sd_mmc.h>
#include <sdmmc_boot/hw_sd.h>
#include <regmap.h>
#include <common.h>
#include <config.h>
//#include <config_xboot.h>  //for xboot. marked in iboot
#include <stdarg.h>
/******************************************************************************
 *                          MACRO Function Define
 *******************************************************************************/
//#define SDEMMC_DBUG_ON // verbose
#ifdef  SDEMMC_DBUG_ON
#define DEBUG_PRINTF_E(fmt, args...)           diag_printf(fmt, ##args)
#define DEBUG_PRINTF_FUNLINE(fmt, args...)     diag_printf(fmt, ##args)
#define DEBUG_PRINTF(fmt, args...)             diag_printf(fmt, ##args)
#else
#define DEBUG_PRINTF_E(fmt, args...) 		do {} while (0)
#define DEBUG_PRINTF_FUNLINE(fmt, args...) 	do {} while (0)
#define DEBUG_PRINTF(fmt, args...)		do {} while (0)
#endif

#define BSWAP_CONSTANT_32(x)			\
	(unsigned int)((((unsigned int)(x) & 0xff000000u) >> 24) |	\
			(((unsigned int)(x) & 0x00ff0000u) >>  8) |	\
			(((unsigned int)(x) & 0x0000ff00u) <<  8) |	\
			(((unsigned int)(x) & 0x000000ffu) << 24))

#define IS_DMA_ADDR_2BYTE_ALIGNED(x)  (!((x) & 0x1))
extern void _delay_1ms(UINT32 period);
extern unsigned int sdTranStateWait(void);
extern void prn_sd_status(void);
/******************************************************************************
 *                          Global Variabl
 *******************************************************************************/
static const unsigned int tran_exp[] =
{
	10000,		100000,		1000000,	10000000,
	0,		0,		0,		0
};

static const unsigned char tran_mant[] =
{
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

static const unsigned int tacc_exp[] =
{
	1,	10,	100,	1000,	10000,	100000,	1000000, 10000000,
};

static const unsigned int tacc_mant[] =
{
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};


#define SUPPORT_RD_DLY_TESTING 1
#define SD_RD_CLK_DLY_BEG       2 // for SDCARD
#define EMMC_RD_CLK_DLY_BEG     0 // for eMMC

const inline int get_SD_RD_CLK_DLY_BEG(int card_num)
{
#ifdef CONFIG_HAVE_EMMC
	if (card_num == EMMC_SLOT_NUM)
		return EMMC_RD_CLK_DLY_BEG;
#endif
	return SD_RD_CLK_DLY_BEG;
}

/******************************************************************************
 *                          External Definition
 *******************************************************************************/

/******************************************************************************
 *                         Function Prototype
 *******************************************************************************/
int ReadSectorByPolling(unsigned char* pRecBuf,unsigned int*  pRecLenInByte);
int SetSD74Clk(void);
int DoMMCSwitch(struct STORAGE_DEVICE* pStroage_dev,unsigned char CmdSet,unsigned char Index,unsigned char Value);
void SetChipCtrlBusWidth(int BusWidth);
void SetChipCtrlDummyClk(void);
void SetChipCtrlClk(unsigned int peri_clk,unsigned int bus_clk);
void SetChipCtrlBlkLen(int blklen);
unsigned int hwSdIdleWait(void);
void hwSdBusWidthSet(unsigned char sdBusWidth);
unsigned int sdStateCheck(unsigned int switch_status);

unsigned int hwSdTxDummy(void);
unsigned int hwSdCmdSend(unsigned int, unsigned int, unsigned int, unsigned char *);
void hwSdFreqSet(unsigned int sdFreq);
void hwSdConfig(unsigned int, unsigned int);
void hwSdInit(unsigned int);
void SetMediaTypeMode(unsigned char ActFlash, unsigned short PageSize);

#if 0
void prn_sd_reg_dump(void)
{
	int sd_grp = (gDEV_SDCTRL_BASE_ADRS - REG_BASE) / (32*4);
	int i, j;

	prn_string("SD Register Dump:\n");
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 32; j++) {
			prn_string("G"); prn_decimal(sd_grp + i);
			prn_string("."); prn_decimal(j);
			prn_string("="); prn_dword(*(volatile u32 *)RF_GRP(sd_grp + i, j));
		}
	}
}
#endif

/**
 *	UNSTUFF_BITS - Get n bits
 *  @resp: buffer pointer
 *  @resplen:  buffer len
 *	@start: index of start bit
 *	@size: bit count
 *
 */
int UNSTUFF_BITS(unsigned int* resp, int resplen, int start, int size)
{
	const int __size = size;
	const unsigned int __mask = (__size < 32 ? 1 << __size : 0) - 1;
	const int __off = ((start) / 32); //(resplen-1) - ((start) / 32);
	const int __shft = (start) & 31;
	unsigned int __res;

	__res = *(resp+__off) >> __shft;

	if (__size + __shft > 32)
		__res |= resp[__off-1] << ((32 - __shft) % 32);

	return (__res & __mask);
}

unsigned int GetTotalSector(void)
{
	return gStorage_dev.total_sector;
}

int get_card_number(void)
{
	if (gDEV_SDCTRL_BASE_ADRS == (unsigned int)ADDRESS_CONVERT(CARD0_CTL_REG)) return 0;
	if (gDEV_SDCTRL_BASE_ADRS == (unsigned int)ADDRESS_CONVERT(CARD1_CTL_REG)) return 1;
	return 0;
}

//-------------------------------------
/**
 *	InitChipCtrl - Init chip ctrl & clk
 *
 */
void InitChipCtrl(void)
{
	int i;

	CSTAMP(0xCAD00000);
	prn_string("InitChipCtrl\n");

	//restore default value
#if SUPPORT_RD_DLY_TESTING
	for (i = 0; i < 4; i++) {
		g_bootinfo.gSD_HIGHSPEED_EN_SET_val[i] = 0;
		g_bootinfo.gSD_RD_CLK_DELAY_TIME_val[i] = get_SD_RD_CLK_DLY_BEG(i);
	}

	i = get_card_number();

	SD_HIGHSPEED_EN_SET(g_bootinfo.gSD_HIGHSPEED_EN_SET_val[i]);        // highspeed disable
	SD_RD_CLK_DELAY_TIME_SET(g_bootinfo.gSD_RD_CLK_DELAY_TIME_val[i]);  // set sd read clk delay
	SD_WT_CLK_DELAY_TIME_SET(1);  // set sd write clk delay
#else
	SD_HIGHSPEED_EN_SET(0);       // highspeed disable
	SD_RD_CLK_DELAY_TIME_SET(0);  // set sd read clk delay
	SD_WT_CLK_DELAY_TIME_SET(1);  // set sd write clk delay
#endif

	SetMediaTypeMode(MEDIA_SD, 512);


	// ----- Set Initial clock 200KHz (ok:400KHz) -----
	if (get_card_number() == EMMC_SLOT_NUM) {
		hwSdInit(SD_MODE);
		SetChipCtrlClk(CARD012_CLK, 200000);
	} else {
		hwSdInit(SD_MODE);
		SetChipCtrlClk(CARD012_CLK,200000);
	}
	/* set `sdmmcmode', as it will sample data at falling edge
	 * of SD bus clock if `sdmmcmode' is not set when
	 * `sd_high_speed_en' is not set, which is not compliant
	 * with SD specification */
	hwSdConfig(SD_1BIT_MODE, MMC_MODE);

	/*----- Set block size -----*/
	SetChipCtrlBlkLen(BLOCK_LEN_BYTES_512);

	// Transaction Mode
	SD_TRANS_SDDDRMODE(0); // 0: SDR mode with Tx/Rx data
	SD_TRANS_SDLENMODE(1); // 1: hw will not send stop cmd
	SD_TRANS_SDPIOMODE(1); // 1: enable PIO

	/* ----- Start 74 cycles on SD Clk Bus ----- */
	CSTAMP(0xCAD00001);
	_delay_1ms(20); //SD ISP, wait for Vdd stable
	if (get_card_number() == EMMC_SLOT_NUM) {
		// CMD0 with arg 0xf0f0f0f0 let emmc go pre idle state
		unsigned int ret = hwSdCmdSend(CMD0, 0xf0f0f0f0, RSP_TYPE_NORSP, NULL);
		if (ret) {
			prn_string("emmc go pre idle state fail\n");
		}
	}
	SetSD74Clk();
}


//-------------------------------------
/**
 *	SetChipCtrlClk - Set chip ctrl's clk
 *
 */
	void
SetChipCtrlClk(unsigned int sysClock, unsigned int busSpeed)
{
	unsigned int speedFactor;
	speedFactor = ((sysClock+busSpeed-1)/busSpeed) - 1;

	//prn_string("sysclk="); prn_decimal_ln(sysClock);
	prn_string("busclk="); prn_decimal_ln(busSpeed);
	prn_string("div="); prn_decimal_ln(speedFactor);

	hwSdFreqSet(speedFactor);
}

//-------------------------------------
/**
 *	SetChipCtrlBlkLen - Set chip ctrl's block length
 *
 */
void SetChipCtrlBlkLen(int blklen)
{
	SD_BLOCK_SIZE_SET((blklen - 1) & 0x3ff);
}

//-------------------------------------
/**
 *	SetSD74Clk - Initialize chip's SD controller clock
 *
 */
int SetSD74Clk(void)
{
	int i;

	for ( i = 10; i > 0;  i--) { /* Tx 80 clock for power on sequence  */
		hwSdTxDummy();
	}

	return ERR_SUCCESS;
}

/**
 *	CheckSDAppOpCond - Send CMD8 & ACMD41 to check physical spec version
 *
 */
int CheckSDAppOpCond(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret;
	int loop_count=0;
	unsigned char rsp_buf[17];
	unsigned int cmd_args = ARGS_STUFF_BITS;
	int args_check_pattern = 0;
	int args_voltage_supplied = 0;
	int host_support_high_capacity_flag = 0;
	int card_return_response_flag = 0;
	int check_card_powerup_busy_bit_flag = 0;
	int check_ccs_respose_flag = 0;

	CSTAMP(0xCAD05000);

	ret = hwSdCmdSend(CMD8, 0x1AA, RSP_TYPE_R7, rsp_buf);
	// The card will not response CMD8 if the supply voltage does not support in 4.3.13.
	// But the iboot does not care the support voltage, ignore the error check of CMD8. CRC error included.
	// The card process CMD8 only in idle state.
	if (ret == SD_CRC_ERROR)
		return ret;

	args_check_pattern = rsp_buf[4];
	args_voltage_supplied = rsp_buf[3] & 0x0f;
	DEBUG_PRINTF_FUNLINE("args_check_pattern = ");prn_dword(args_check_pattern);//prn_string("\n");
	DEBUG_PRINTF_FUNLINE("args_voltage_supplied = ");prn_dword(args_voltage_supplied);//prn_string("\n");

	// No response or voltage mismatch 2.7-3.6
	if ((ret != SD_SUCCESS) || (args_voltage_supplied != ARGS_VS_1B)) {
		CSTAMP(0xCAD05001);
		DEBUG_PRINTF_FUNLINE("Send CMD8 Faild!\n");
		card_return_response_flag = 0;
	} else { // Card returns response (Ver2.0 or later SD Card)
		// non-compatible voltabe range or check pattern is not correct
		if ((args_check_pattern != ARGS_CHECK_PATTERN) || (args_voltage_supplied != ARGS_VS_1B)) {
			CSTAMP(0xCAD05002);
			DEBUG_PRINTF_FUNLINE("Unusable card \n");
			goto FAIL_CHECK_SD_APP_OP_COND;
		} else {
			CSTAMP(0xCAD05003);
			host_support_high_capacity_flag = 1;
			card_return_response_flag = 1;
		}
	}

	do {
		CSTAMP(0xCAD05004);

		//CMD55: Indicates to the card that the next command is an application specific command
		ret = hwSdCmdSend(CMD55, 0, RSP_TYPE_R1, rsp_buf);
		if (ret == SD_CRC_ERROR)
			return ret;

		//ACMD41: Sends host capacity support information(HCS)
		cmd_args = ARGS_STUFF_BITS;
		// Support V3.2 ~ 3.4
		cmd_args |= (ARGS_OCR_VDD_WIN_V32_33|ARGS_OCR_VDD_WIN_V33_34);

		if (host_support_high_capacity_flag == 1) {
#ifdef PLATFORM_I143
			// Not support SDXC:
			cmd_args |= ARGS_HOST_CAPACITY_SUPPORT|ARGS_S18R(1)|ARGS_XPC(0);  // add S18R
#else
			// Not support SDXC:
	                cmd_args |= ARGS_HOST_CAPACITY_SUPPORT|ARGS_S18R(0)|ARGS_XPC(0);  // add S18R
#endif
		}

		CSTAMP(0xCAD05005);

		ret = hwSdCmdSend(ACMD41, cmd_args, RSP_TYPE_R3, rsp_buf);
		if (ret) {
			CSTAMP(0xCAD05006);
			DEBUG_PRINTF_FUNLINE("Send ACMD41 Faild!\n");
			if (ret == SD_RSP_TIMEOUT)
				return ret; //stop only no response(no card case), other continue and retry

			if (ret == SD_CRC_ERROR)
				return ret;
		} else {
			//when send command failed, do not check the busy bit flag
			check_card_powerup_busy_bit_flag = (rsp_buf[1] & 0x80) >> 7;
		}
		loop_count++;
		_delay_1ms(1);
		/*	spec says it needs 1 second */
		if (loop_count > (TIME_OUT_LOOP_COUNT_SHORT*3)) {
			CSTAMP(0xCAD05007);
			DEBUG_PRINTF_FUNLINE("ACMD41: Cards with non compatible voltage range or time out coocurs \n");
			goto FAIL_CHECK_SD_APP_OP_COND;
		}
	} while (check_card_powerup_busy_bit_flag != 1);

	CSTAMP(0xCAD05008);
	printk("ACMD41: OK, loop_count=%d\n",loop_count);

	// Check CCS
	check_ccs_respose_flag = (rsp_buf[1]&0x40)>>6;

	if (card_return_response_flag == 1) {
		if (check_ccs_respose_flag == 1) {
			CSTAMP(0xCAD05009);
			prn_string("High Cap\n");
			pStroage_dev->what_dev = WHAT_STORAGE_HIGH_CAPACITY_SD;
		} else {
			CSTAMP(0xCAD0500A);
			prn_string("Standard Cap\n");
			pStroage_dev->what_dev = WHAT_STORAGE_STANDARD_CAPACITY_SD;
		}
	} else { // card_return_response_flag = 0 CMD8: no response
		// ACMD41: has response
		if (ret == SD_SUCCESS) {
			CSTAMP(0xCAD0500B);
			prn_string("V1.x Standard Cap\n");
			pStroage_dev->what_dev = WHAT_STORAGE_STANDARD_CAPACITY_SD;
		} else {// ACMD41: no response
			CSTAMP(0xCAD0500C);
			prn_string("Not SD\n");
			pStroage_dev->what_dev = ERR_NEGATIVE_VALUE;
		}
	}

#ifdef PLATFORM_I143    // 1.8v switch 
#if (0)
	if (rsp_buf[1]&0x01) {
		prn_string("Get A18R\n");

		SD_VOL_TMR_SET(1);
		SD_HW_VOL_SET(1);
		ret = hwSdCmdSend(CMD11, 0, RSP_TYPE_R1, rsp_buf);

		if (ret == SD_SUCCESS) {
			_delay_1ms(20);
			SD_TXDUMMY_SET(401);
			hwSdTxDummy();
			SD_TXDUMMY_SET(8);


			if (SD_VOL_RESULT_GET() == 0x01) {
				prn_string("switch 1.8v success\n");
				prn_string(" i143-1 status0="); prn_dword0(SD_STATUS0_GET());
				prn_string(" i143-1 status1="); prn_dword(SD_STATUS1_GET());
			} else {
				prn_string("switch 1.8v result\n");	prn_dword0(SD_VOL_RESULT_GET()); prn_string("\n");
				prn_string(" i143-2 status0="); prn_dword0(SD_STATUS0_GET());
				prn_string(" i143-2 status1="); prn_dword(SD_STATUS1_GET());
			}
		} else {
			return ret;		
		}
		//_delay_1ms(500);
	} else {
		prn_string(" Not Get A18R\n");
	}

#endif
#endif

	return ret;

FAIL_CHECK_SD_APP_OP_COND:
	pStroage_dev->what_dev = ERR_NEGATIVE_VALUE;
	ret = ERR_FAIL;
	return ret;
}

//-------------------------------------
/**
 *	SetBlkLen512B - Send CMD16 to set block length for SD/MMC
 *
 */
int SetBlkLen512B(void)
{
	int ret;
	unsigned char rsp_buf[17];

	SetChipCtrlBlkLen(BLOCK_LEN_BYTES_512);

	// Send CMD16 to set block len
	ret = hwSdCmdSend(CMD16, BLOCK_LEN_BYTES_512, RSP_TYPE_R1, rsp_buf);
	if (ret) {
		return -CMD16;
	}

	return ret;
}

//-------------------------------------
/**
 *	GetAllSendCIDNum - Send CMD2 to get CID numbers
 *
 */
int GetAllSendCIDNum(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret;
	unsigned char rsp_buf[17];

	rd_memset((unsigned char *)pStroage_dev->dev_card.raw_cid,0,sizeof(unsigned int)*4);

	//CMD2: Asks any card to send CID numbers (128Bits)
	ret = hwSdCmdSend(CMD2, 0, RSP_TYPE_R2, rsp_buf);
	if (ret != SD_SUCCESS) {
		dbg();
		if (ret == SD_CRC_ERROR)
			return ret;

		return -CMD2;
	}

	pStroage_dev->dev_card.raw_cid[3] = (rsp_buf[ 1] << 24) + (rsp_buf[ 2] << 16) + (rsp_buf[ 3] << 8) + rsp_buf[ 4];
	pStroage_dev->dev_card.raw_cid[2] = (rsp_buf[ 5] << 24) + (rsp_buf[ 6] << 16) + (rsp_buf[ 7] << 8) + rsp_buf[ 8];
	pStroage_dev->dev_card.raw_cid[1] = (rsp_buf[ 9] << 24) + (rsp_buf[10] << 16) + (rsp_buf[11] << 8) + rsp_buf[12];
	pStroage_dev->dev_card.raw_cid[0] = (rsp_buf[13] << 24) + (rsp_buf[14] << 16) + (rsp_buf[15] << 8) + rsp_buf[16];

	/*
	   prn_string("CID[3]="); prn_dword(pStroage_dev->dev_card.raw_cid[3]);
	   prn_string("CID[2]="); prn_dword(pStroage_dev->dev_card.raw_cid[2]);
	   prn_string("CID[1]="); prn_dword(pStroage_dev->dev_card.raw_cid[1]);
	   prn_string("CID[0]="); prn_dword(pStroage_dev->dev_card.raw_cid[0]);
	 */

	return SD_SUCCESS;
}

//-------------------------------------
/**
 *	SendSetRelativeAddr - Send CMD3 to get RCA or set RCA
 *
 */
int GetSetRelativeAddr(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret;
	unsigned char rsp_buf[17];

	rd_memset((unsigned char *)&(pStroage_dev->dev_card.RCA_Address),0,sizeof(int));

	dbg();
	if (pStroage_dev->what_dev >= WHAT_STORAGE_STANDARD_CAPACITY_MMC) {
		//CMD3: Assigns relative address to the card
		pStroage_dev->dev_card.RCA_Address = 0x01; // Set value of starting RCA

		ret = hwSdCmdSend(CMD3, (pStroage_dev->dev_card.RCA_Address << 16), RSP_TYPE_R1, rsp_buf);
		if (ret != SD_SUCCESS) {
			pStroage_dev->dev_card.RCA_Address = ERR_NEGATIVE_VALUE;
			return -CMD3;
		}
	} else { // SD Card
		dbg();
		//CMD3: Asks the card to publish a new relative address(RCA)
		ret = hwSdCmdSend(CMD3, 0, RSP_TYPE_R6, rsp_buf);
		if (ret) {
			pStroage_dev->dev_card.RCA_Address = ERR_NEGATIVE_VALUE;
			return -CMD3;
		}

		pStroage_dev->dev_card.RCA_Address = (rsp_buf[1] << 8) + rsp_buf[2];
	}

	return ret;
}

//-------------------------------------
/**
 *	GetSDCSDNum - Send CMD9 to get CSD
 *
 */
int GetSendCSDNum(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret;
	unsigned char rsp_buf[17];

	if (pStroage_dev->dev_card.RCA_Address <= ERR_NEGATIVE_VALUE) {
		return SD_FAIL;
	}

	rd_memset((unsigned char *)pStroage_dev->dev_card.raw_csd,0,sizeof(unsigned int)*4);

	//CMD9: Addressed card sends its card-specific data(CSD)
	ret = hwSdCmdSend(CMD9, (pStroage_dev->dev_card.RCA_Address <<16), RSP_TYPE_R2, rsp_buf);
	if (ret != SD_SUCCESS) {
		return -CMD9;
	}

	pStroage_dev->dev_card.raw_csd[3] = (rsp_buf[ 1] << 24) + (rsp_buf[ 2] << 16) + (rsp_buf[ 3] << 8) + rsp_buf[ 4];
	pStroage_dev->dev_card.raw_csd[2] = (rsp_buf[ 5] << 24) + (rsp_buf[ 6] << 16) + (rsp_buf[ 7] << 8) + rsp_buf[ 8];
	pStroage_dev->dev_card.raw_csd[1] = (rsp_buf[ 9] << 24) + (rsp_buf[10] << 16) + (rsp_buf[11] << 8) + rsp_buf[12];
	pStroage_dev->dev_card.raw_csd[0] = (rsp_buf[13] << 24) + (rsp_buf[14] << 16) + (rsp_buf[15] << 8) + rsp_buf[16];

	/*
	   prn_string("CSD[3]="); prn_dword(pStroage_dev->dev_card.raw_csd[3]);
	   prn_string("CSD[2]="); prn_dword(pStroage_dev->dev_card.raw_csd[2]);
	   prn_string("CSD[1]="); prn_dword(pStroage_dev->dev_card.raw_csd[1]);
	   prn_string("CSD[0]="); prn_dword(pStroage_dev->dev_card.raw_csd[0]);
	 */

	return ret;
}

//-------------------------------------
/**
 *	GetSDSCRNum - Send ACMD51 to read the SD configuration register
 *
 */
int GetSDSCRNum(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret = SD_FAIL;
	unsigned int bytes_count = BLOCK_LEN_BYTES_8;
	unsigned char rsp_buf[17];
	unsigned int response_buf[2], tempval;
	unsigned int scr_struct;
	int i = 0;

	rd_memset((unsigned char *) response_buf, 0, sizeof(unsigned int)*2);

	if (pStroage_dev->dev_card.RCA_Address <= ERR_NEGATIVE_VALUE) {
		goto FAIL_GET_SCR;
	}

	rd_memset((unsigned char *)&(pStroage_dev->dev_card.reg_SCR),0,sizeof(struct REG_SCR));

	SetChipCtrlBlkLen(BLOCK_LEN_BYTES_8);

	//CMD55: Indicates to the card that the next command is an application specific command
	ret = hwSdCmdSend(CMD55, (pStroage_dev->dev_card.RCA_Address <<16), RSP_TYPE_R1, rsp_buf);
	if (ret)
		goto FAIL_GET_SCR;
	SD_TRANS_SDPIOMODE(1); // pio
	//ACMD51: Read the SD configuration register(SCR)
	ret = hwSdCmdSend(ACMD51, 0, RSP_TYPE_R1, rsp_buf);
	if (ret)
		goto FAIL_GET_SCR;

	ret = ReadSectorByPolling((unsigned char*)response_buf,&bytes_count);
	if (ret != ERR_SUCCESS)
		goto FAIL_GET_SCR;

	// Send dummy to allow receiving RSP
	//to prevent from the late response after data finished, spec 2-64 clock, send 48 clocks more
	for (i = 0; i < 10; i++)
		hwSdTxDummy();

	// Note: We can retrive RSP here but we have no such need
	// rsp[0] = SD_RSP_BUF0_3_GET()
	// rsp[1] = SD_RSP_BUF4_5_GET()

	tempval = response_buf[0];
	response_buf[0] = BSWAP_CONSTANT_32(response_buf[1]);
	response_buf[1] = BSWAP_CONSTANT_32(tempval);

	scr_struct = UNSTUFF_BITS(response_buf,2, 60, 4);
	if (scr_struct != 0) {
		prn_string("Unknown SCR ver="); prn_decimal_ln(scr_struct); //prn_string("\n");
		goto FAIL_GET_SCR;
	}

	pStroage_dev->dev_card.reg_SCR.sd_spec = UNSTUFF_BITS(response_buf,2, 56, 4);
	pStroage_dev->dev_card.reg_SCR.sd_bus_widths = UNSTUFF_BITS(response_buf,2, 48, 4);

	//prn_string("rsp[0]="); prn_dword(response_buf[0]);
	//prn_string("rsp[1]="); prn_dword(response_buf[1]);
	//prn_string("SCR Ver="); prn_decimal(scr_struct);
	prn_string("SPEC="); prn_decimal_ln(pStroage_dev->dev_card.reg_SCR.sd_spec);
	prn_string("BUS_WIDTHS="); prn_decimal_ln(pStroage_dev->dev_card.reg_SCR.sd_bus_widths);

	SetChipCtrlBlkLen(BLOCK_LEN_BYTES_512);

	return ret;

FAIL_GET_SCR:
	SD_RST();
	// Send dummy to allow receiving RSP
	//to prevent from the late response after data finished, spec 2-64 clock, send 48 clocks more
	for (i = 0; i < 10; i++)
		hwSdTxDummy();

	SetChipCtrlBlkLen(BLOCK_LEN_BYTES_512);
	return ret;
}

//-------------------------------------
/**
 *	GetSDCSDNum - Send CMD13 to addressed card for getting status register
 *  @pReg_Status:
 *
 */
	int
CheckCardStatus(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret, nrRetry;
	unsigned char rsp_buf[17];
	unsigned int* presponse_buf = 0;

	if (pStroage_dev->dev_card.RCA_Address <= ERR_NEGATIVE_VALUE) {
		goto FAIL_CHECK_CARD_STATUS;
	}

	//CMD13: Addressed card sends its status register
#if 0
	ret = hwSdCmdSend(CMD13, (pStroage_dev->dev_card.RCA_Address <<16), RSP_TYPE_R1, rsp_buf);
	if (ret) {
		return -CMD13;
	}
	for (i = 0; i < 50; i++)
		hwSdTxDummy();
#else
	nrRetry = 10;
	ret = SD_FAIL;
	while ((ret != SD_SUCCESS) && (nrRetry > 0)) {
		ret = hwSdCmdSend(CMD13, (pStroage_dev->dev_card.RCA_Address <<16), RSP_TYPE_R1, rsp_buf);
		nrRetry--;
	}

	if (nrRetry == 0) {
		DEBUG_PRINTF_E("nr retry is 0\n");
		return SD_STATE_TIMEOUT;
	}
#endif

	presponse_buf = (unsigned int*)&(pStroage_dev->dev_card.reg_STATUS);
	*presponse_buf = (rsp_buf[ 1] << 24) + (rsp_buf[ 2] << 16) + (rsp_buf[ 3] << 8) + rsp_buf[ 4];

#if 0
	//DEBUG_PRINTF_FUNLINE_E("*presponse_buf = 0x%x \n",*presponse_buf);
	DEBUG_PRINTF_E("OUT_OF_RANGE = %d \n",pStroage_dev->dev_card.reg_STATUS.out_of_range);
	DEBUG_PRINTF_E("ADDRESS_ERROR = %d \n",pStroage_dev->dev_card.reg_STATUS.address_error);
	DEBUG_PRINTF_E("BLOCK_LEN_ERROR = %d \n",pStroage_dev->dev_card.reg_STATUS.block_len_error);
	DEBUG_PRINTF_E("ERASE_SEQ_ERROR = %d \n",pStroage_dev->dev_card.reg_STATUS.erase_seq_error);
	DEBUG_PRINTF_E("ERASE_PARAM = %d \n",pStroage_dev->dev_card.reg_STATUS.erase_param);
	DEBUG_PRINTF_E("WP_VIOLATION = %d \n",pStroage_dev->dev_card.reg_STATUS.wp_violation);
	DEBUG_PRINTF_E("CARD_IS_LOCKED = %d \n",pStroage_dev->dev_card.reg_STATUS.card_is_locked);
	DEBUG_PRINTF_E("LOCK_UNLOCK_FAILED = %d \n",pStroage_dev->dev_card.reg_STATUS.lock_unlock_failed);
	DEBUG_PRINTF_E("COM_CRC_ERROR = %d \n",pStroage_dev->dev_card.reg_STATUS.com_crc_error);
	DEBUG_PRINTF_E("ILLEGAL_COMMAND = %d \n",pStroage_dev->dev_card.reg_STATUS.illegal_command);
	DEBUG_PRINTF_E("CARD_ECC_FAILED = %d \n",pStroage_dev->dev_card.reg_STATUS.card_ecc_failed);
	DEBUG_PRINTF_E("CC_ERROR = %d \n",pStroage_dev->dev_card.reg_STATUS.cc_error);
	DEBUG_PRINTF_E("ERROR = %d \n",pStroage_dev->dev_card.reg_STATUS.error);
	DEBUG_PRINTF_E("CSD_OVERWRITE = %d \n",pStroage_dev->dev_card.reg_STATUS.csd_overwrite);
	DEBUG_PRINTF_E("WP_ERASE_SKIP = %d \n",pStroage_dev->dev_card.reg_STATUS.wp_erase_skip);
	DEBUG_PRINTF_E("CARD_ECC_DISABLED = %d \n",pStroage_dev->dev_card.reg_STATUS.card_ecc_disabled);
	DEBUG_PRINTF_E("ERASE_RESET = %d \n",pStroage_dev->dev_card.reg_STATUS.erase_reset);
	DEBUG_PRINTF_E("CURRENT_STATE = %d \n",pStroage_dev->dev_card.reg_STATUS.current_state);
	DEBUG_PRINTF_E("READY_FOR_DATA = %d \n",pStroage_dev->dev_card.reg_STATUS.ready_for_data);
	DEBUG_PRINTF_E("APP_CMD = %d \n",pStroage_dev->dev_card.reg_STATUS.app_cmd);
	DEBUG_PRINTF_E("AKE_SEQ_ERROR = %d \n",pStroage_dev->dev_card.reg_STATUS.ake_seq_error);
#endif

	return ret;

FAIL_CHECK_CARD_STATUS:
	ret = ERR_FAIL;
	return ret;
}

//-------------------------------------
/**
 *	ChangeCardToTransferState - Send CMD7 to change card to transfer state
 *
 */
int ChangeCardToTransferState(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret;
	unsigned char rsp_buf[17];
	int loop_count = 0;

	if (pStroage_dev->dev_card.RCA_Address <= ERR_NEGATIVE_VALUE) {
		goto FAIL_CHANGE_TO_TRANSFER_STATE;
	}

	//CMD7: Command toggles a card between the stand-by and transfer states
	ret = hwSdCmdSend(CMD7, (pStroage_dev->dev_card.RCA_Address<<16), RSP_TYPE_R1, rsp_buf);
	if (ret) {
		return -CMD7;
	}

	do {
		ret = CheckCardStatus(pStroage_dev);
		if (ret != ERR_SUCCESS) {
			goto FAIL_CHANGE_TO_TRANSFER_STATE;
		}

		if (pStroage_dev->dev_card.reg_STATUS.current_state == CURRENT_STATE_TRAN) {
			break;
		}

		loop_count++;
		if (loop_count > TIME_OUT_LOOP_COUNT_SHORT) {
			goto FAIL_CHANGE_TO_TRANSFER_STATE;
		}
	} while (1);

	return ret;

FAIL_CHANGE_TO_TRANSFER_STATE:
	DEBUG_PRINTF_FUNLINE("[CMD7] Card can't chagne to transfer state \n");
	ret = ERR_FAIL;
	return ret;
}

//-------------------------------------
/**
 *	StopCardTransmission - Send CMD12 to forces the card to stop transmission
 *
 */
int StopCardTransmission(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret_value = ERR_SUCCESS;
	unsigned char rsp_buf[17];

	//CMD12: Forces the card to stop transmission
	ret_value = hwSdCmdSend(CMD12, 0, RSP_TYPE_R1B, rsp_buf);
	if (ret_value != ERR_SUCCESS) {
		goto FAIL_STOP_TRANMISSION;
	}

	return ret_value;

FAIL_STOP_TRANMISSION:
	DEBUG_PRINTF_FUNLINE("[CMD12] Card can't chagne to transfer state \n");
	ret_value = ERR_FAIL;
	return ret_value;
}

//-------------------------------------
/**
 *	ChangeCardToHiSpeedMode - Send CMD6 to check or switch function
 *	@Args: Send SD's argument
 *	@pResult: return result
 *
 */
int CheckSDHiSpeedSupport(struct STORAGE_DEVICE* pStroage_dev, unsigned int Args,
		unsigned int* pResult)
{
	int ret;
	unsigned char rsp_buf[17];
	unsigned int bytes_count = BLOCK_LEN_BYTES_64;
	unsigned char tmp_buf[BLOCK_LEN_BYTES_64];

	rd_memset((unsigned char *) tmp_buf, 0, BLOCK_LEN_BYTES_64);

	*pResult = ERR_SUCCESS;
	SD_TRANS_SDPIOMODE(1); // pio
	//CMD6: Checks swichable function and switch card function
	ret = hwSdCmdSend(CMD6, Args, RSP_TYPE_R1, rsp_buf);
	if (ret) {
		goto FAIL_CHECK_HISPEED_SUPPORT;
	}

	ret = ReadSectorByPolling(tmp_buf,&bytes_count);
	if (ret != ERR_SUCCESS) {
		goto FAIL_CHECK_HISPEED_SUPPORT;
	}

#if 0
	// Check Bit 376~379 (group1): Mode0: The function which can be switched
	// Mode1: The function which is result of the switch command
	if ((tmp_buf[16] & 0x0F) != 1) {
		goto FAIL_CHECK_HISPEED_SUPPORT;
	}
#else
	if ((tmp_buf[13] & 0x02) == 0) { //check bit401, 1:support high speed; 0:don't support high speed
		goto FAIL_CHECK_HISPEED_SUPPORT;
	}
#endif

	return ret;

FAIL_CHECK_HISPEED_SUPPORT:
	*pResult = ERR_FAIL;
	ret = ERR_FAIL;
	return ret;
}

//-------------------------------------
/**
 *	ChangeSDToHiSpeedMode - Change the SD to high speed mode
 *
 */
int ChangeSDToHiSpeedMode(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret_value = ERR_SUCCESS;
	int cmd_args = ARGS_STUFF_BITS;
	unsigned int ret_result = 0;

	cmd_args = ARGS_STUFF_BITS;
	cmd_args |= 0x00FFFFF1; // Bit31 0: check function  , Check High-Speed

	SetChipCtrlBlkLen(BLOCK_LEN_BYTES_64);

	ret_value = CheckSDHiSpeedSupport(pStroage_dev,cmd_args,&ret_result);
	if (ret_value != ERR_SUCCESS) {
		goto FAIL_CHANGE_TO_SD_HISPEED;
	}

	// This card supports high speed , we switch it into high speed
	if (ret_result == ERR_SUCCESS) {
		cmd_args = ARGS_STUFF_BITS;
		cmd_args |= 0x80FFFFF1; //Bit31 1: switch function
		ret_value = CheckSDHiSpeedSupport(pStroage_dev,cmd_args,&ret_result);
		if (ret_value != ERR_SUCCESS) {
			goto FAIL_CHANGE_TO_SD_HISPEED;
		}
	}

	SetChipCtrlBlkLen(BLOCK_LEN_BYTES_512);

	return ret_value;

FAIL_CHANGE_TO_SD_HISPEED:
	SetChipCtrlBlkLen(BLOCK_LEN_BYTES_512);
	StopCardTransmission(&gStorage_dev);

	ret_value = ERR_FAIL;
	return ret_value;
}

#ifdef CONFIG_HAVE_EMMC
//-------------------------------------
/**
 */
int DoMMCSwitch(struct STORAGE_DEVICE* pStroage_dev, unsigned char CmdSet,
		unsigned char Index, unsigned char Value)
{
	int ret_value = ERR_SUCCESS;
	unsigned char rsp_buf[17];
	int cmd_args = ARGS_STUFF_BITS;

	if (pStroage_dev->dev_card.RCA_Address <= ERR_NEGATIVE_VALUE) {
		goto FAIL_DO_MMC_SWITCH;
	}

	//CMD6: Modify the EXT_CSD register
	cmd_args = ARGS_STUFF_BITS;
	cmd_args = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
			(Index << 16) |
			(Value << 8) |
			CmdSet;

	ret_value = hwSdCmdSend(MMCCMD6, cmd_args, RSP_TYPE_R1B, rsp_buf);
	if (ret_value != ERR_SUCCESS) {
		goto FAIL_DO_MMC_SWITCH;
	}

	// Check the status of card
	do {
		ret_value = CheckCardStatus(pStroage_dev);
		if (ret_value != ERR_SUCCESS) {
			goto FAIL_DO_MMC_SWITCH;
		}
	} while (pStroage_dev->dev_card.reg_STATUS.current_state == 7); // in programming state

	if (pStroage_dev->dev_card.reg_STATUS.mmc_switch_error == 1) {
		goto FAIL_DO_MMC_SWITCH;
	}

	return ret_value;

FAIL_DO_MMC_SWITCH:
	ret_value = ERR_FAIL;
	return ret_value;
}

//-------------------------------------
/**
 *	ChangeMMCToHiSpeedMode - Send CMD6 to change the MMC to high speed mode
 *
 */
int ChangeMMCToHiSpeedMode(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret_value = ERR_SUCCESS;

	ret_value = DoMMCSwitch(pStroage_dev, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, 1);

	return ret_value;
}
#endif /* CONFIG_HAVE_EMMC */

#if 1 // unused
//-------------------------------------
/**
 */
int SetMMCEraseGroup2Hi(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret_value = ERR_SUCCESS;

	ret_value = DoMMCSwitch(pStroage_dev,EXT_CSD_CMD_SET_NORMAL,EXT_CSD_ERASE_GROUP,1);

	return ret_value;
}

//-------------------------------------
/**
 *  0: device not boot enable
 *  1: boot partition 1 enabled for boot
 *  2: boot partition 2 enabled for boot
 *  3-6: reserved
 *  7: user area enabled for boot
 */
int GetBootPartitionNum(void)
{
	int ret_value = EMMC_ACCESS_BOOT_PARTITION_1;
	struct STORAGE_DEVICE* pstorage_device = 0;

	pstorage_device = &gStorage_dev;

	ret_value = pstorage_device->dev_card.ext_csd.partition_config.boot_partition_enable;
	if (ret_value == 7) { // access partition set to 0
		ret_value = EMMC_ACCESS_USER_AREA;
	}

	DEBUG_PRINTF("Boot_partition_enable ret_value = %d \n",ret_value);
	return ret_value;
}

//-------------------------------------
/**
 * 0: no access to boot partition(default: user area)
 * 1: R/W boot partition 1
 * 2: R/W boot partition 2
 * 3: R/W Replay Protected Memory Block(RPMB)
 * 4: Access to General Purpose partition 1
 * 5: Access to General Purpose partition 2
 * 6: Access to General Purpose partition 3
 * 7: Access to General Purpose partition 4
 */
int SetMMCPartitionNum(unsigned char PartitionNum)
{
	int ret_value = ERR_SUCCESS;
	struct STORAGE_DEVICE* pstorage_device = 0;
	unsigned char* tmp_pointer;

	pstorage_device = &gStorage_dev;
	pstorage_device->dev_card.ext_csd.partition_config.partition_access = PartitionNum;
	tmp_pointer = (unsigned char*)&pstorage_device->dev_card.ext_csd.partition_config;

	ret_value = SetMMCEraseGroup2Hi(pstorage_device);
	if (ret_value != ERR_SUCCESS) {
		return ret_value;
	}

	ret_value = DoMMCSwitch(pstorage_device,EXT_CSD_CMD_SET_NORMAL,EXT_CSD_BOOT_CONFIG,(unsigned char)*tmp_pointer);

	return ret_value;
}
#endif

//-------------------------------------
/**
 *	SetSDBusWidth - Send ACMD6 to set bus width
 *
 */
int SetSDBusWidth(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret;
	unsigned char rsp_buf[17];

	if (pStroage_dev->dev_card.RCA_Address <= ERR_NEGATIVE_VALUE) {
		return ERR_FAIL;
	}

	//CMD55: Indicates to the card that the next command is an application specific command
	ret = hwSdCmdSend(CMD55, (pStroage_dev->dev_card.RCA_Address  <<16), RSP_TYPE_R1, rsp_buf);
	if (ret)
		return ret;

	//ACMD6: Set the data bus width
	ret = hwSdCmdSend(ACMD6, pStroage_dev->dev_card.bus_width, RSP_TYPE_R1, rsp_buf);
	if (ret) {
		DEBUG_PRINTF_FUNLINE("Send ACMD6 Faild!\n");
		return ret;
	}

	return ret;
}

//-------------------------------------
/**
 *	SetMMCBusWidth - Send CMD6 to set bus width
 *
 */
int SetMMCBusWidth(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret_value = ERR_SUCCESS;

	ret_value = DoMMCSwitch(pStroage_dev,EXT_CSD_CMD_SET_NORMAL,EXT_CSD_BUS_WIDTH,pStroage_dev->dev_card.bus_width);

	return ret_value;
}

//-------------------------------------
/**
 *	CheckMMCOpCond - Send CMD1 to ask the card to sen its Operating Conditions Register Conterns in the response
 *
 */
int CheckMMCOpCond(struct STORAGE_DEVICE* pStroage_dev)
{
	int ret;
	int cmd_args;
	int loop_count = 0;
	int check_card_powerup_busy_bit_flag = 0;
	int check_card_capacity_flag = 0;
	unsigned char rsp_buf[17];
	unsigned int response_buf;

	//OCR[30:29] Access mode: 00b:byte mode 10b: sector mode
	// 0x00FF(capacity less thea or equal to 2GB) 0x40FF(capacity greater than 2GB)
CHECK_MMC_AGAIN:
	if (check_card_capacity_flag == 0) {
		cmd_args = 0x40FF8000;
	} else {
		cmd_args = 0x00FF8000;
	}

	do {
		// Send OCR[23:15]: 2.7-3.6V
		ret = hwSdCmdSend(CMD1, cmd_args, RSP_TYPE_R3, rsp_buf);

		if (ret == SD_SUCCESS)
			check_card_powerup_busy_bit_flag = (rsp_buf[1]&0x80)>>7;
		else if (ret == SD_RSP_TIMEOUT) {
			return SD_FAIL;
		}

		loop_count++;
		_delay_1ms(1);
		if ((loop_count > 5)&& (ret != SD_SUCCESS)) {
			//FIXME for different size eMMC
			loop_count+=200; //speed up the counter
		}


		if (loop_count > TIME_OUT_LOOP_COUNT_SHORT * 3) {
			if (check_card_capacity_flag == 0) {
				check_card_capacity_flag = 1;
				loop_count = 0;
				goto CHECK_MMC_AGAIN;
			} else {
				goto FAIL_CHECK_MMC_OP_COND;
			}
		}

	} while (check_card_powerup_busy_bit_flag != 1);

	response_buf = (rsp_buf[ 1] << 24) + (rsp_buf[ 2] << 16) + (rsp_buf[ 3] << 8) + rsp_buf[ 4];

	if (check_card_capacity_flag == 0) {
		if ((response_buf & cmd_args) == 0x40FF8000) {
			pStroage_dev->what_dev = WHAT_STORAGE_HIGH_CAPACITY_MMC;
			DEBUG_PRINTF("High Capacity MMC Card (eMMC) \n");
		} else {
			pStroage_dev->what_dev = WHAT_STORAGE_STANDARD_CAPACITY_MMC;
			DEBUG_PRINTF("Standard MMC Card \n");
		}
	} else { // check_card_capacity_flag = 1
		pStroage_dev->what_dev = WHAT_STORAGE_STANDARD_CAPACITY_MMC;
		DEBUG_PRINTF("Standard MMC Card \n");
	}

	return ret;

FAIL_CHECK_MMC_OP_COND:
	printk("FAIL_CHECK_MMC_OP_COND\n");
	pStroage_dev->what_dev = ERR_NEGATIVE_VALUE;
	return ERR_FAIL;
}

//-------------------------------------
/**
 *	DecodeCSD - Decode raw csd to our csd structure
 *
 */
void DecodeCSD(struct STORAGE_DEVICE* pStorage_device)
{
	unsigned int what_dev = pStorage_device->what_dev;
	unsigned int e, m, csd_struct;
	unsigned int* resp = pStorage_device->dev_card.raw_csd;

	if (what_dev >= WHAT_STORAGE_STANDARD_CAPACITY_MMC) {
		csd_struct = UNSTUFF_BITS(resp,4, 126, 2);
		if (csd_struct == 0) {
			DEBUG_PRINTF("MMC:Unrecognised CSD structure version = %d \n",csd_struct);
			return;
		}

		pStorage_device->dev_card.reg_CSD.mmca_vsn	 = UNSTUFF_BITS(resp,4, 122, 4);
		m = UNSTUFF_BITS(resp,4, 115, 4);
		e = UNSTUFF_BITS(resp,4, 112, 3);
		pStorage_device->dev_card.reg_CSD.tacc_ns	 = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
		pStorage_device->dev_card.reg_CSD.tacc_clks	 = UNSTUFF_BITS(resp,4, 104, 8) * 100;

		m = UNSTUFF_BITS(resp,4, 99, 4);
		e = UNSTUFF_BITS(resp,4, 96, 3);
		pStorage_device->dev_card.reg_CSD.max_dtr	  = tran_exp[e] * tran_mant[m];
		pStorage_device->dev_card.reg_CSD.cmdclass	  = UNSTUFF_BITS(resp,4, 84, 12);

		e = UNSTUFF_BITS(resp,4, 47, 3);
		m = UNSTUFF_BITS(resp,4, 62, 12);
		pStorage_device->dev_card.reg_CSD.capacity	  = (1 + m) << (e + 2);

		pStorage_device->dev_card.reg_CSD.read_blkbits = UNSTUFF_BITS(resp,4, 80, 4);
		pStorage_device->dev_card.reg_CSD.read_partial = UNSTUFF_BITS(resp,4, 79, 1);
		pStorage_device->dev_card.reg_CSD.write_misalign = UNSTUFF_BITS(resp,4, 78, 1);
		pStorage_device->dev_card.reg_CSD.read_misalign = UNSTUFF_BITS(resp,4, 77, 1);
		pStorage_device->dev_card.reg_CSD.r2w_factor = UNSTUFF_BITS(resp,4, 26, 3);
		pStorage_device->dev_card.reg_CSD.write_blkbits = UNSTUFF_BITS(resp,4, 22, 4);
		pStorage_device->dev_card.reg_CSD.write_partial = UNSTUFF_BITS(resp,4, 21, 1);

		pStorage_device->total_sector = (pStorage_device->dev_card.reg_CSD.capacity<<(pStorage_device->dev_card.reg_CSD.read_blkbits-9));

#if 0
		DEBUG_PRINTF_E("mmca_vsn = %d \n",pStorage_device->dev_card.reg_CSD.mmca_vsn);
		DEBUG_PRINTF_E("tacc_ns = %d \n",pStorage_device->dev_card.reg_CSD.tacc_ns);
		DEBUG_PRINTF_E("tacc_clks = %d \n",pStorage_device->dev_card.reg_CSD.tacc_clks);
		DEBUG_PRINTF_E("max_dtr = %d \n",pStorage_device->dev_card.reg_CSD.max_dtr);
		DEBUG_PRINTF_E("cmdclass = %d \n",pStorage_device->dev_card.reg_CSD.cmdclass);
		DEBUG_PRINTF_E("capacity = %d \n",pStorage_device->dev_card.reg_CSD.capacity);
		DEBUG_PRINTF_E("read_blkbits = %d \n",pStorage_device->dev_card.reg_CSD.read_blkbits);
		DEBUG_PRINTF_E("read_partial = %d \n",pStorage_device->dev_card.reg_CSD.read_partial);
		DEBUG_PRINTF_E("write_misalign = %d \n",pStorage_device->dev_card.reg_CSD.write_misalign);
		DEBUG_PRINTF_E("read_misalign = %d \n",pStorage_device->dev_card.reg_CSD.read_misalign);
		DEBUG_PRINTF_E("r2w_factor = %d \n",pStorage_device->dev_card.reg_CSD.r2w_factor);
		DEBUG_PRINTF_E("write_blkbits = %d \n",pStorage_device->dev_card.reg_CSD.write_blkbits);
		DEBUG_PRINTF_E("write_partial = %d \n",pStorage_device->dev_card.reg_CSD.write_partial);
#endif
	} else { // SD Card
		csd_struct = UNSTUFF_BITS(resp,4, 126, 2);

		DEBUG_PRINTF_E("csd_struct version = %d \n",(csd_struct==0?1:2));

		switch (csd_struct) {
		case 0:
			m = UNSTUFF_BITS(resp,4, 115, 4);
			e = UNSTUFF_BITS(resp,4, 112, 3);
			pStorage_device->dev_card.reg_CSD.tacc_ns	 = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
			pStorage_device->dev_card.reg_CSD.tacc_clks	 = UNSTUFF_BITS(resp,4, 104, 8) * 100;

			m = UNSTUFF_BITS(resp,4, 99, 4);
			e = UNSTUFF_BITS(resp,4, 96, 3);
			pStorage_device->dev_card.reg_CSD.max_dtr	  = tran_exp[e] * tran_mant[m];
			pStorage_device->dev_card.reg_CSD.cmdclass	  = UNSTUFF_BITS(resp,4, 84, 12);

			e = UNSTUFF_BITS(resp,4, 47, 3);
			m = UNSTUFF_BITS(resp,4, 62, 12);
			pStorage_device->dev_card.reg_CSD.capacity	  = (1 + m) << (e + 2);

			pStorage_device->dev_card.reg_CSD.read_blkbits = UNSTUFF_BITS(resp,4, 80, 4);
			pStorage_device->dev_card.reg_CSD.read_partial = UNSTUFF_BITS(resp,4, 79, 1);
			pStorage_device->dev_card.reg_CSD.write_misalign = UNSTUFF_BITS(resp,4, 78, 1);
			pStorage_device->dev_card.reg_CSD.read_misalign = UNSTUFF_BITS(resp,4, 77, 1);
			pStorage_device->dev_card.reg_CSD.r2w_factor = UNSTUFF_BITS(resp,4, 26, 3);
			pStorage_device->dev_card.reg_CSD.write_blkbits = UNSTUFF_BITS(resp,4, 22, 4);
			pStorage_device->dev_card.reg_CSD.write_partial = UNSTUFF_BITS(resp,4, 21, 1);

			pStorage_device->total_sector = (pStorage_device->dev_card.reg_CSD.capacity<<(pStorage_device->dev_card.reg_CSD.read_blkbits-9));

#if 0
			DEBUG_PRINTF_E("tacc_ns = %d \n",pStorage_device->dev_card.reg_CSD.tacc_ns);
			DEBUG_PRINTF_E("tacc_clks = %d \n",pStorage_device->dev_card.reg_CSD.tacc_clks);
			DEBUG_PRINTF_E("max_dtr = %d \n",pStorage_device->dev_card.reg_CSD.max_dtr);
			DEBUG_PRINTF_E("cmdclass = %d \n",pStorage_device->dev_card.reg_CSD.cmdclass);
			DEBUG_PRINTF_E("capacity = %d \n",pStorage_device->dev_card.reg_CSD.capacity);
			DEBUG_PRINTF_E("read_blkbits = %d \n",pStorage_device->dev_card.reg_CSD.read_blkbits);
			DEBUG_PRINTF_E("read_partial = %d \n",pStorage_device->dev_card.reg_CSD.read_partial);
			DEBUG_PRINTF_E("write_misalign = %d \n",pStorage_device->dev_card.reg_CSD.write_misalign);
			DEBUG_PRINTF_E("read_misalign = %d \n",pStorage_device->dev_card.reg_CSD.read_misalign);
			DEBUG_PRINTF_E("r2w_factor = %d \n",pStorage_device->dev_card.reg_CSD.r2w_factor);
			DEBUG_PRINTF_E("write_blkbits = %d \n",pStorage_device->dev_card.reg_CSD.write_blkbits);
			DEBUG_PRINTF_E("write_partial = %d \n",pStorage_device->dev_card.reg_CSD.write_partial);
#endif
			break;
		case 1:
			/*
			 * This is a block-addressed SDHC card. Most
			 * interesting fields are unused and have fixed
			 * values. To avoid getting tripped by buggy cards,
			 * we assume those fixed values ourselves.
			 */

			pStorage_device->dev_card.reg_CSD.tacc_ns	 = 0; /* Unused */
			pStorage_device->dev_card.reg_CSD.tacc_clks	 = 0; /* Unused */

			m = UNSTUFF_BITS(resp,4, 99, 4);
			e = UNSTUFF_BITS(resp,4, 96, 3);
			pStorage_device->dev_card.reg_CSD.max_dtr	  = tran_exp[e] * tran_mant[m];
			pStorage_device->dev_card.reg_CSD.cmdclass	  = UNSTUFF_BITS(resp,4, 84, 12);

			m = UNSTUFF_BITS(resp,4, 48, 22);
			pStorage_device->dev_card.reg_CSD.capacity     = (1 + m) << 10;

			pStorage_device->dev_card.reg_CSD.read_blkbits = 9;
			pStorage_device->dev_card.reg_CSD.read_partial = 0;
			pStorage_device->dev_card.reg_CSD.write_misalign = 0;
			pStorage_device->dev_card.reg_CSD.read_misalign = 0;
			pStorage_device->dev_card.reg_CSD.r2w_factor = 4; /* Unused */
			pStorage_device->dev_card.reg_CSD.write_blkbits = 9;
			pStorage_device->dev_card.reg_CSD.write_partial = 0;

			pStorage_device->total_sector = pStorage_device->dev_card.reg_CSD.capacity;

#if 0
			DEBUG_PRINTF_E("tacc_ns = %d \n",pStorage_device->dev_card.reg_CSD.tacc_ns);
			DEBUG_PRINTF_E("tacc_clks = %d \n",pStorage_device->dev_card.reg_CSD.tacc_clks);
			DEBUG_PRINTF_E("max_dtr = %d \n",pStorage_device->dev_card.reg_CSD.max_dtr);
			DEBUG_PRINTF_E("cmdclass = %d \n",pStorage_device->dev_card.reg_CSD.cmdclass);
			DEBUG_PRINTF_E("capacity = %d \n",pStorage_device->dev_card.reg_CSD.capacity);
			DEBUG_PRINTF_E("read_blkbits = %d \n",pStorage_device->dev_card.reg_CSD.read_blkbits);
			DEBUG_PRINTF_E("read_partial = %d \n",pStorage_device->dev_card.reg_CSD.read_partial);
			DEBUG_PRINTF_E("write_misalign = %d \n",pStorage_device->dev_card.reg_CSD.write_misalign);
			DEBUG_PRINTF_E("read_misalign = %d \n",pStorage_device->dev_card.reg_CSD.read_misalign);
			DEBUG_PRINTF_E("r2w_factor = %d \n",pStorage_device->dev_card.reg_CSD.r2w_factor);
			DEBUG_PRINTF_E("write_blkbits = %d \n",pStorage_device->dev_card.reg_CSD.write_blkbits);
			DEBUG_PRINTF_E("write_partial = %d \n",pStorage_device->dev_card.reg_CSD.write_partial);
#endif
			break;
		default:
			DEBUG_PRINTF("SD:Unrecognised CSD structure version = %d \n",csd_struct);
			return;
		} // switch
	}
}

//-------------------------------------
/**
 *	DecodeCID - Decode raw cid to our cid structure
 *
 */
void DecodeCID(struct STORAGE_DEVICE* pStorage_device)
{
#if 0
	unsigned int* resp = pStorage_device->dev_card.raw_cid;

	// Decode SD's CID
	if (pStorage_device->what_dev < WHAT_STORAGE_STANDARD_CAPACITY_MMC) {
		pStorage_device->dev_card.reg_CID.manfid = UNSTUFF_BITS(resp,4, 120, 8);
		pStorage_device->dev_card.reg_CID.oemid = UNSTUFF_BITS(resp,4, 104, 16);
		pStorage_device->dev_card.reg_CID.prod_name[0] = UNSTUFF_BITS(resp,4, 96, 8);
		pStorage_device->dev_card.reg_CID.prod_name[1] = UNSTUFF_BITS(resp,4, 88, 8);
		pStorage_device->dev_card.reg_CID.prod_name[2] = UNSTUFF_BITS(resp,4, 80, 8);
		pStorage_device->dev_card.reg_CID.prod_name[3] = UNSTUFF_BITS(resp,4, 72, 8);
		pStorage_device->dev_card.reg_CID.prod_name[4] = UNSTUFF_BITS(resp,4, 64, 8);

		pStorage_device->dev_card.reg_CID.hwrev = UNSTUFF_BITS(resp,4, 60, 4);
		pStorage_device->dev_card.reg_CID.fwrev = UNSTUFF_BITS(resp,4, 56, 4);
		pStorage_device->dev_card.reg_CID.serial = UNSTUFF_BITS(resp,4, 24, 32);
		pStorage_device->dev_card.reg_CID.year = (UNSTUFF_BITS(resp,4, 12, 8)+2000);
		pStorage_device->dev_card.reg_CID.month = UNSTUFF_BITS(resp,4, 8, 4);

		DEBUG_PRINTF_E("MID(Manufacturer ID) = %d \n",pStorage_device->dev_card.reg_CID.manfid);
		DEBUG_PRINTF_E("OID(OEM/Application ID) = %d \n",pStorage_device->dev_card.reg_CID.oemid);
		DEBUG_PRINTF_E("PVM0(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[0]);
		DEBUG_PRINTF_E("PVM1(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[1]);
		DEBUG_PRINTF_E("PVM2(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[2]);
		DEBUG_PRINTF_E("PVM3(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[3]);
		DEBUG_PRINTF_E("PVM4(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[4]);
		DEBUG_PRINTF_E("PRV(Product revision) = hw:%d/fw:%d \n",pStorage_device->dev_card.reg_CID.hwrev,pStorage_device->dev_card.reg_CID.fwrev);
		DEBUG_PRINTF_E("PSN(Product serial number) = %d \n",pStorage_device->dev_card.reg_CID.serial);
		DEBUG_PRINTF_E("MDT_Year(Manufacturing date) = %d \n",pStorage_device->dev_card.reg_CID.year );
		DEBUG_PRINTF_E("MDT_Month(Manufacturing date) = %d \n",pStorage_device->dev_card.reg_CID.month);
	} else {
		// Decode MMC's CID
		switch (pStorage_device->dev_card.reg_CSD.mmca_vsn) {
		case 0: /* MMC v1.0 - v1.2 */
		case 1: /* MMC v1.4 */
			pStorage_device->dev_card.reg_CID.manfid	= UNSTUFF_BITS(resp,4, 104, 24);
			pStorage_device->dev_card.reg_CID.prod_name[0]	= UNSTUFF_BITS(resp,4, 96, 8);
			pStorage_device->dev_card.reg_CID.prod_name[1]	= UNSTUFF_BITS(resp,4, 88, 8);
			pStorage_device->dev_card.reg_CID.prod_name[2]	= UNSTUFF_BITS(resp,4, 80, 8);
			pStorage_device->dev_card.reg_CID.prod_name[3]	= UNSTUFF_BITS(resp,4, 72, 8);
			pStorage_device->dev_card.reg_CID.prod_name[4]	= UNSTUFF_BITS(resp,4, 64, 8);
			pStorage_device->dev_card.reg_CID.prod_name[5]	= UNSTUFF_BITS(resp,4, 56, 8);
			pStorage_device->dev_card.reg_CID.prod_name[6]	= UNSTUFF_BITS(resp,4, 48, 8);
			pStorage_device->dev_card.reg_CID.hwrev		= UNSTUFF_BITS(resp,4, 44, 4);
			pStorage_device->dev_card.reg_CID.fwrev		= UNSTUFF_BITS(resp,4, 40, 4);
			pStorage_device->dev_card.reg_CID.serial	= UNSTUFF_BITS(resp,4, 16, 24);
			pStorage_device->dev_card.reg_CID.month		= UNSTUFF_BITS(resp,4, 12, 4);
			pStorage_device->dev_card.reg_CID.year		= UNSTUFF_BITS(resp,4, 8, 4) + 1997;

			DEBUG_PRINTF_E("MID(Manufacturer ID) = %d \n",pStorage_device->dev_card.reg_CID.manfid);
			DEBUG_PRINTF_E("PVM0(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[0]);
			DEBUG_PRINTF_E("PVM1(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[1]);
			DEBUG_PRINTF_E("PVM2(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[2]);
			DEBUG_PRINTF_E("PVM3(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[3]);
			DEBUG_PRINTF_E("PVM4(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[4]);
			DEBUG_PRINTF_E("PVM5(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[5]);
			DEBUG_PRINTF_E("PVM6(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[6]);
			DEBUG_PRINTF_E("PRV(Product revision) = hw:%d/fw:%d \n",pStorage_device->dev_card.reg_CID.hwrev,pStorage_device->dev_card.reg_CID.fwrev);
			DEBUG_PRINTF_E("PSN(Product serial number) = %d \n",pStorage_device->dev_card.reg_CID.serial);
			DEBUG_PRINTF_E("MDT_Year(Manufacturing date) = %d \n",pStorage_device->dev_card.reg_CID.year );
			DEBUG_PRINTF_E("MDT_Month(Manufacturing date) = %d \n",pStorage_device->dev_card.reg_CID.month);
			break;

		case 2: /* MMC v2.0 - v2.2 */
		case 3: /* MMC v3.1 - v3.3 */
		case 4: /* MMC v4 */
			pStorage_device->dev_card.reg_CID.manfid	= UNSTUFF_BITS(resp,4, 120, 8);
			pStorage_device->dev_card.reg_CID.oemid		= UNSTUFF_BITS(resp,4, 104, 16);
			pStorage_device->dev_card.reg_CID.prod_name[0]	= UNSTUFF_BITS(resp,4, 96, 8);
			pStorage_device->dev_card.reg_CID.prod_name[1]	= UNSTUFF_BITS(resp,4, 88, 8);
			pStorage_device->dev_card.reg_CID.prod_name[2]	= UNSTUFF_BITS(resp,4, 80, 8);
			pStorage_device->dev_card.reg_CID.prod_name[3]	= UNSTUFF_BITS(resp,4, 72, 8);
			pStorage_device->dev_card.reg_CID.prod_name[4]	= UNSTUFF_BITS(resp,4, 64, 8);
			pStorage_device->dev_card.reg_CID.prod_name[5]	= UNSTUFF_BITS(resp,4, 56, 8);
			pStorage_device->dev_card.reg_CID.serial	= UNSTUFF_BITS(resp,4, 16, 32);
			pStorage_device->dev_card.reg_CID.month		= UNSTUFF_BITS(resp,4, 12, 4);
			pStorage_device->dev_card.reg_CID.year		= UNSTUFF_BITS(resp,4, 8, 4) + 1997;

			DEBUG_PRINTF_E("MID(Manufacturer ID) = %d \n",pStorage_device->dev_card.reg_CID.manfid);
			DEBUG_PRINTF_E("OID(OEM/Application ID) = %d \n",pStorage_device->dev_card.reg_CID.oemid);
			DEBUG_PRINTF_E("PVM0(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[0]);
			DEBUG_PRINTF_E("PVM1(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[1]);
			DEBUG_PRINTF_E("PVM2(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[2]);
			DEBUG_PRINTF_E("PVM3(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[3]);
			DEBUG_PRINTF_E("PVM4(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[4]);
			DEBUG_PRINTF_E("PVM5(Product Name) = %c \n",pStorage_device->dev_card.reg_CID.prod_name[5]);
			DEBUG_PRINTF_E("PSN(Product serial number) = %d \n",pStorage_device->dev_card.reg_CID.serial);
			DEBUG_PRINTF_E("MDT_Year(Manufacturing date) = %d \n",pStorage_device->dev_card.reg_CID.year );
			DEBUG_PRINTF_E("MDT_Month(Manufacturing date) = %d \n",pStorage_device->dev_card.reg_CID.month);
			break;

		default:
			DEBUG_PRINTF("Card has unknown MMCA version = %d \n",pStorage_device->dev_card.reg_CSD.mmca_vsn);
			break;
		}
	}
#endif
}

#ifdef CONFIG_HAVE_EMMC // for eMMC
//-------------------------------------
/**
 *	GetMMCExtCSD - Send CMD8 to get its EXT_CSD register as a block of data
 *
 */
int GetMMCExtCSD(struct STORAGE_DEVICE* pStorage_device)
{
	int ret;
	unsigned int bytes_count = BLOCK_LEN_BYTES_512;
	unsigned char tmp_buf[BLOCK_LEN_BYTES_512];
	unsigned char* tmp_pointer;

	rd_memset((unsigned char *) tmp_buf, 0, BLOCK_LEN_BYTES_512);
	DEBUG_PRINTF("get SD_STATE_TRAN before send mmc cmd 8\n");
	ret = sdStateCheck(SD_STATE_TRAN);
	if (ret != SD_SUCCESS) {
		DEBUG_PRINTF("SD statecheck err\n");
		return SD_FAIL;
	}

	SD_TRANS_SDPIOMODE(1); // pio
	SD_PAGE_NUM_SET(0);
	SD_BLOCK_SIZE_SET(BLOCK_LEN_BYTES_512 - 1);

	//CMD8: Get EXT_CSD register
	ret = hwSdCmdSend(CMD8, 0, RSP_TYPE_R1, NULL);
	if (ret) {
		goto FAIL_GET_MMC_EXT_CSD;
	}

	ret = ReadSectorByPolling(tmp_buf,&bytes_count);
	if (ret != ERR_SUCCESS) {
		goto FAIL_GET_MMC_EXT_CSD;
	}

	pStorage_device->dev_card.ext_csd.rev = tmp_buf[EXT_CSD_REV];

	// Check card type
	switch (tmp_buf[EXT_CSD_CARD_TYPE]) {
	case EXT_CSD_CARD_TYPE_52 | EXT_CSD_CARD_TYPE_26:
		pStorage_device->dev_card.ext_csd.hs_max_clk = 52000000;
		break;
	case EXT_CSD_CARD_TYPE_26:
		pStorage_device->dev_card.ext_csd.hs_max_clk = 26000000;
		break;
	}

	if (pStorage_device->dev_card.ext_csd.rev >= 2) { // MMC version V4.2
		// Capacity
		pStorage_device->dev_card.ext_csd.capacity = (tmp_buf[215]<<24) | (tmp_buf[214]<<16) | (tmp_buf[213]<<8) | tmp_buf[212];
		pStorage_device->total_sector = pStorage_device->dev_card.ext_csd.capacity;
		// BOOT_SIZE_MULT
		pStorage_device->dev_card.ext_csd.boot_size_mult = tmp_buf[226];

		// Partition config
		tmp_pointer = (unsigned char*)&pStorage_device->dev_card.ext_csd.partition_config;
		*tmp_pointer = tmp_buf[179];

	}

	DEBUG_PRINTF_E("Card Type  = 0x%x\n",pStorage_device->dev_card.ext_csd.hs_max_clk);
	DEBUG_PRINTF_E("Capacity(Unit:sector) = %d \n",pStorage_device->dev_card.ext_csd.capacity);
	DEBUG_PRINTF_E("Boot_size_mult = %d \n",pStorage_device->dev_card.ext_csd.boot_size_mult);
	DEBUG_PRINTF_E("Boot_partition_enable = %d \n",pStorage_device->dev_card.ext_csd.partition_config.boot_partition_enable);
	DEBUG_PRINTF_E("partition_access = %d \n",pStorage_device->dev_card.ext_csd.partition_config.partition_access);

	return ret;

FAIL_GET_MMC_EXT_CSD:
	pStorage_device->dev_card.ext_csd.hs_max_clk = 0;
	pStorage_device->dev_card.ext_csd.capacity = 0;

	ret = ERR_FAIL;
	return ret;
}
#endif

//-------------------------------------
/**
 *	IdentifyStorage - Execute card's internal initialization and idenfification process
 *
 */
int IdentifyStorage(void)
{
	int i, ret;
	struct STORAGE_DEVICE* pstorage_device = 0;

	CSTAMP(0xCAD00002);
	SD_RST();
	prn_string("IdentifyStorage\n");
	pstorage_device = &gStorage_dev;

	// CMD0: Reset all cards to idle state
	ret = hwSdCmdSend(CMD0, 0, RSP_TYPE_NORSP, NULL);
	if (ret) {
		prn_string("CMD0 fail\n");
		return -CMD0;
	}

	CSTAMP(0xCAD00003);

	for (i = 0; i < 50; i++)
		ret = hwSdTxDummy();

	if (ret == SD_FAIL) {
		dbg();
		return SD_FAIL;
	}

	/*
	   prn_string("delay 2s before CMD1\n");
	   for (i= 0 ; i <10*2; i++) { // 2s
	   delay_1ms(100);
	   }
	 */
	CSTAMP(0xCAD00004); // Before CheckXXXOpCond

	pstorage_device->what_dev = ERR_NEGATIVE_VALUE;

#ifdef CONFIG_HAVE_EMMC // for eMMC
	if (get_card_number() == EMMC_SLOT_NUM) { // FIX: only emmc accept cmd1 ok
		// Check MMC_OP_COND
		ret = CheckMMCOpCond(pstorage_device);
		if (ret != ERR_SUCCESS) {
			DEBUG_PRINTF_FUNLINE("[MMC]: CMD1 failure ,check SD \n");
			return SD_FAIL;
		}
		DEBUG_PRINTF_FUNLINE("[MMC]:pstorage_device->what_dev  = %d \n",pstorage_device->what_dev);
	} else {
		pstorage_device->what_dev = ERR_NEGATIVE_VALUE;
	}
#endif

	// Check SD_APP_OP_COND
	if (pstorage_device->what_dev == ERR_NEGATIVE_VALUE) {
		dbg();
		ret = CheckSDAppOpCond(pstorage_device);
		if (ret != ERR_SUCCESS) {
			CSTAMP(0xCAD00005); // Failed Check SD Op
			DEBUG_PRINTF_FUNLINE("[SD/MMC]: CMD8 & ACMD41 failure \n");
			goto FAIL_IDENTIFY_STORAGE;
		}
	}

	// Get CID
	CSTAMP(0xCAD00006);
	ret = GetAllSendCIDNum(pstorage_device);
	if (ret != ERR_SUCCESS) {
		CSTAMP(0xCAD00007);
		//DEBUG_PRINTF_FUNLINE("[SD/MMC]: CMD2 failure \n");
		prn_string("CMD2 fail\n");
		goto FAIL_IDENTIFY_STORAGE;
	}

	// SD: Get RCA or MMC: Set RCA
	CSTAMP(0xCAD00008);
	ret = GetSetRelativeAddr(pstorage_device);
	if (ret != ERR_SUCCESS) {
		CSTAMP(0xCAD00009);
		//DEBUG_PRINTF_FUNLINE("[SD/MMC]: CMD3 failure \n");
		prn_string("CMD3 fail\n");
		goto FAIL_IDENTIFY_STORAGE;
	}

	// Get CSD
	CSTAMP(0xCAD0000A);
	ret = GetSendCSDNum(pstorage_device);
	if (ret != ERR_SUCCESS) {
		CSTAMP(0xCAD0000B);
		DEBUG_PRINTF_FUNLINE("[SD/MMC]: CMD9 failure \n");
		goto FAIL_IDENTIFY_STORAGE;
	}

	// Decode CSD and CID
	DecodeCSD(pstorage_device);
	DecodeCID(pstorage_device);

	// Change card to transfer state for sending or receiving data
	CSTAMP(0xCAD0000C);
	ret = ChangeCardToTransferState(pstorage_device);
	if (ret != ERR_SUCCESS) {
		CSTAMP(0xCAD0000D);
		DEBUG_PRINTF_FUNLINE("[SD/MMC]: CMD7 failure \n");
		goto FAIL_IDENTIFY_STORAGE;
	}

	// Block-addressed cards ignor CMD16 only in SD mode
	CSTAMP(0xCAD0000E);
	ret = SetBlkLen512B();
	if (ret != ERR_SUCCESS) {
		CSTAMP(0xCAD0000F);
		DEBUG_PRINTF_FUNLINE("[SD/MMC]: CMD16 failure \n");
		goto FAIL_IDENTIFY_STORAGE;
	}

#ifdef CONFIG_HAVE_EMMC // for eMMC
	// MMC
	if (pstorage_device->what_dev >= WHAT_STORAGE_STANDARD_CAPACITY_MMC) {
		// Get EXT CSD of MMC
		ret = GetMMCExtCSD(pstorage_device);
		if (ret != ERR_SUCCESS) {
			DEBUG_PRINTF_FUNLINE("[MMC]: CMD8 failure \n");
		}
		// Set 4bit bus width
		pstorage_device->dev_card.bus_width = EXT_CSD_BUS_WIDTH_4;
		ret = SetMMCBusWidth(pstorage_device);
		if (ret != ERR_SUCCESS) {
			DEBUG_PRINTF_FUNLINE("MMC Card is 1bit mode\n");
			//printf("MMC Card is 1bit mode\n");
		} else {
			// Change SD controller to 4bit bus width
			hwSdBusWidthSet(BUS_WIDTH_4BIT);
			DEBUG_PRINTF_FUNLINE("MMC Card is 4bit mode\n");
			//printf("MMC Card is 4bit mode\n");
		}

		// Set high speed
		ret = ChangeMMCToHiSpeedMode(pstorage_device);
		if (ret != ERR_SUCCESS) {
			DEBUG_PRINTF_FUNLINE("MMC run at low speed\n");
			//printf("MMC run at low speed\n");
		} else {
			// Set SD Controller to high clock
			// 52MHz
			//pstorage_device->dev_card.reg_CSD.max_dtr <<= 1;
			DEBUG_PRINTF_FUNLINE("MMC run at high speed\n");
			//printf("MMC run at high speed\n");

#if SUPPORT_RD_DLY_TESTING
			SD_HIGHSPEED_EN_SET(g_bootinfo.gSD_HIGHSPEED_EN_SET_val[get_card_number()]); 	//highspeed enable
			SD_RD_CLK_DELAY_TIME_SET(g_bootinfo.gSD_RD_CLK_DELAY_TIME_val[get_card_number()]);
#else
			if (get_card_number() == EMMC_SLOT_NUM) {
				SD_RD_CLK_DELAY_TIME_SET(3);	//set sd read clk delay
			} else {
				SD_RD_CLK_DELAY_TIME_SET(2);  //set sd read clk delay
			}
			SD_WT_CLK_DELAY_TIME_SET(1);
#endif
		}
		//SetChipCtrlClk(CARD012_CLK,pstorage_device->dev_card.reg_CSD.max_dtr);
		SetChipCtrlClk(CARD012_CLK, 25000000); //BootCode use 5MHz for safty
	} else
#endif
	{ // SD Card
		// Get SCR
		CSTAMP(0xCAD00010);
		ret = GetSDSCRNum(pstorage_device);
		if (ret != ERR_SUCCESS) {
			CSTAMP(0xCAD00011);
			//DEBUG_PRINTF_FUNLINE("[SD]: ACMD51 failure \n");
			prn_string("ACMD51 fail\n");
			goto FAIL_IDENTIFY_STORAGE;
		}
		CSTAMP(0xCAD00012);
		// SD_BUS_WIDTHS: Bit0:1bit Bit2:4bit
		// Card supports 4bit mode ,default bus width after power up is 1bit
		if ((pstorage_device->dev_card.reg_SCR.sd_bus_widths & 0x04) == 0x04) {
			CSTAMP(0xCAD00013);
			// Set 4bit bus width
			pstorage_device->dev_card.bus_width = BUS_WIDTH_4BIT;
			ret = SetSDBusWidth(pstorage_device);
			if (ret != ERR_SUCCESS) {
				if (ret == SD_CRC_ERROR)
					goto FAIL_IDENTIFY_STORAGE;
				CSTAMP(0xCAD00014);
				DEBUG_PRINTF_FUNLINE("SD Card is 1bit mode \n");
				prn_string("1bit mode\n\n\n\n");
			} else {
				CSTAMP(0xCAD00015);
				// Change SD controller to 4bit bus width
				hwSdBusWidthSet(BUS_WIDTH_4BIT);
				DEBUG_PRINTF_FUNLINE("SD Card is 4bit mode \n");
				prn_string("4bit mode\n");
			}
		}

		CSTAMP(0xCAD00016);
		// The version 1.10 and higher supports CMD6 , 12.5MB/s(default) -> 25MB/s(high speed)
		if (pstorage_device->dev_card.reg_SCR.sd_spec >= SD_SPEC_V1_10) {
			if (pstorage_device->dev_card.reg_CSD.max_dtr > 25000000) {
				CSTAMP(0xCAD00017);
				ret = ChangeSDToHiSpeedMode(pstorage_device);
				if (ret != ERR_SUCCESS) {
					CSTAMP(0xCAD00018);
					DEBUG_PRINTF_FUNLINE("SD run at low speed \n");
					prn_string("low speed\n");
				} else {
					CSTAMP(0xCAD00019);
					// Set SD Controller to high clock
					// 50MHz
					//pstorage_device->dev_card.reg_CSD.max_dtr <<= 1;
					//for some card, the clock 50MHZ is too high to normal read/write

					DEBUG_PRINTF_FUNLINE("SD run at high speed \n");
					prn_string("high speed\n");

#if SUPPORT_RD_DLY_TESTING
					SD_HIGHSPEED_EN_SET(g_bootinfo.gSD_HIGHSPEED_EN_SET_val[get_card_number()]); 	//highspeed enable
					SD_RD_CLK_DELAY_TIME_SET(g_bootinfo.gSD_RD_CLK_DELAY_TIME_val[get_card_number()]);
#else
					if (get_card_number() == 3) {
						SD_RD_CLK_DELAY_TIME_SET(4);	//set sd read clk delay
					} else {
						SD_RD_CLK_DELAY_TIME_SET(2);	//set sd read clk delay
					}
					SD_WT_CLK_DELAY_TIME_SET(1);	//set sd write clk delay
#endif
				}
			}
		}

		if (get_card_number() == EMMC_SLOT_NUM) {
			//SetChipCtrlClk(CARD_CLK,pstorage_device->dev_card.reg_CSD.max_dtr);
			SetChipCtrlClk(CARD_CLK,5000000); //BootCode use 5MHz for safty
		} else {
			CSTAMP(0xCAD0001A);
			//SetChipCtrlClk(CARD012_CLK,pstorage_device->dev_card.reg_CSD.max_dtr);
#if defined(PLATFORM_SP7350)
			SetChipCtrlClk(CARD012_CLK,35000000);
#else
			SetChipCtrlClk(CARD012_CLK,5000000); //BootCode use 5MHz for safty
#endif
			//SetChipCtrlClk(CARD012_CLK,160000); //BootCode use 160KHz for safty
			//SetChipCtrlClk(CARD012_CLK,1000000);
			//SetChipCtrlClk(CARD012_CLK,27000000);
		}
		CSTAMP(0xCAD0001B);
	}

	CSTAMP(0xCAD0001C);
	dbg();
	return SD_SUCCESS;

FAIL_IDENTIFY_STORAGE:
	CSTAMP(0xCAD0001D);
	dbg();
	//return SD_FAIL;
	return ret;
}

//-------------------------------------
/**
 *	ReadSectorByPolling - Read sector
 *  @pSendBuff: Buffer pointer for reading data from card
 *  @pSendLenInByte: Byte count to read and return REAd byte count
 */
int ReadSectorByPolling(unsigned char* pRecBuf, unsigned int*  pRecLenInByte)
{
	int ret=ERR_SUCCESS;
	unsigned int bytes_count = 0;
	unsigned int need_read_count;
	unsigned char* pTmp_buf = (unsigned char*)pRecBuf;
	int i = 0;
	int data_err = 0;
	unsigned int MAX_count = 10000; // 10000 * 100uS = 1000mS

	CSTAMP(0xCAD0DA00);

	need_read_count = (*pRecLenInByte);

	CSTAMP(0xCAD0DA01);
	while (need_read_count > 0) {
		if (SD_STATUS1_GET() & (1 << 13)) { // 13: new SM error/timeout
			CSTAMP(0xCAD0DA02);
			dbg();
			break;
		}

		i = 0; //reset the counter for every bytes. otherwiese the counter will accumulate and a late data response card will exceed MAX_count
		// Wait for Rx data buffer full
		while ((SD_STATUS0_GET() & 0x08) == 0x00 && (i++ <= MAX_count)) {
			//The timeout is 100ms in spec, we use 300ms in iboot to guarantee meeting the spec.
			//in xboot, this takes long time w/wo this print
			STC_delay_us(100);

			//check state_new, if error, break
			if (SD_STATUS_NEW_ERR() || (i >= MAX_count)) {
				data_err = 1;
				dbg();
				break;
			}
		}

		if (SD_STATUS_NEW_ERR() || (i >= MAX_count)) {
			dbg();
			DEBUG_PRINTF_E("bytes_count:0x%x\n", bytes_count);
			DEBUG_PRINTF_E("i = 0x%x, MAX_count = 0x%x\n", i, MAX_count);
			DEBUG_PRINTF_E("read data timeout status1=0x%x\n", SD_STATUS1_GET());
			DEBUG_PRINTF_E("config0 = 0x%x, config1=0x%x\n", SD_CONFIG0_GET(),SD_CONFIG_GET());
			//goto FAIL_READ_SECTOR_POLLING;
			break;
		}

		if ((SD_STATUS0_GET() & 0x08) == 0x00) {
			dbg();
			DEBUG_PRINTF_E("pio buf not full\n");
			goto FAIL_READ_SECTOR_POLLING;
		}

		CSTAMP(0xCAD0DA03);
		// PIO mode rx4B_en=1
		*(unsigned int *)pTmp_buf = SD_PIO_RX_DATA_GET();
		pTmp_buf += 4;
		need_read_count -= 4;
		bytes_count += 4;

		// RD suggests to check G119.20 sdstate_new
		// bit14: finish/idle
		// bit13: error/timeout
	}

	CSTAMP(0xCAD0DA04);

#ifdef SD_VERBOSE
	// Dump SD data
	prn_string("SD DATA("); prn_decimal(bytes_count); prn_string(")\n");
	//prn_dump_buffer(pRecBuf, bytes_count);
#endif

	// Check for DATA CRC error
	if (data_err) {
		CSTAMP(0xCAD0EC2C);
		DEBUG_PRINTF_E("config0 = 0x%x, config1=0x%x\n", SD_CONFIG0_GET(),SD_CONFIG_GET());

		if (SD_DATA_CRC_ERROR() || SD_DATA_CRC7_ERROR()) {
			prn_string("Data CRC err, resp:");
			prn_dword0(SD_RSP_BUF0_3_GET);
			prn_dword(SD_RSP_BUF4_5_GET);
			sdTryNextReadDelay(get_card_number());
		}

		if (SD_RSP_TIMEOUT_ERROR()) {
			prn_string("rsp timeout err\n");
		}

		if (SD_DATA_CRC_TIMEOUT_ERROR()) {
			prn_string("Data CRC timeout err\n");
		}

		if (SD_DATA_STB_TIMEOUT_ERROR()) {
			prn_string("Data STB err\n");
			if (IS_EMMC_SLOT()) {
				i = SDFQSEL_GET();
			} else {
				i = SD_CONFIG_GET();
				i &= 0x3ff;
			}

			i = i*2 + 1; 	//lower frequency
			prn_string(" freq div:");
			prn_dword(i);
			hwSdFreqSet(i);

			prn_string("read bytes:");
			prn_decimal(bytes_count);
		}
		//prn_sd_reg_dump();
		goto FAIL_READ_SECTOR_POLLING;
	} else {
		if (hwSdIdleWait() == SD_FAIL) {
			CSTAMP(0xCAD0DA05);
			prn_string("hwSdIdleWait err\n");
			goto FAIL_READ_SECTOR_POLLING;
		}
	}

	CSTAMP(0xCAD0DA06);
	*pRecLenInByte = bytes_count;
	return ret;

FAIL_READ_SECTOR_POLLING:
	CSTAMP(0xCAD0DA07);
	*pRecLenInByte = bytes_count;
	ret = ERR_FAIL;
	return ret;
}


//-------------------------------------
/**
 *	ReadSDMultipleSector - Send CMD18 to receive multiple blocks data
 *	@SectorIdx: Start index of sector for reading
 *	@SectorNum: Sector counts to read
 *	@pRedBuff: Buffer pointer for reading data from card
 *
 */
int ReadSDMultipleSectorDma(unsigned int SectorIdx, unsigned int SectorNum,
		unsigned char* pRecBuff)
{
	int ret_value = ERR_SUCCESS;
	int cmd_args = ARGS_STUFF_BITS;
	unsigned int time0, time1;

#ifdef SD_VERBOSE
	prn_string("SD Read blk="); prn_decimal_ln(SectorIdx);
	prn_string("num="); prn_decimal_ln(SectorNum); //prn_string("\n");
#endif
	if (!IS_DMA_ADDR_2BYTE_ALIGNED((unsigned int)ADDRESS_CONVERT(pRecBuff))) {
		prn_string("[sd err]dma addr is not 2 bytes aligned\n");
		return SD_FAIL;
	}
	sdTranStateWait();
	if ((gStorage_dev.what_dev == WHAT_STORAGE_STANDARD_CAPACITY_SD) ||
			(gStorage_dev.what_dev == WHAT_STORAGE_STANDARD_CAPACITY_MMC)) {
		// Standard Capacity SD: unit: byte
		cmd_args |= (SectorIdx<<9); // = SectorNum*512
	} else {
		// High Capacity SD: unit: block
		cmd_args |= SectorIdx;
	}
	SD_RST();
	SD_TRANS_SDDUMMY(0);
	SD_TRANS_SDAUTORSP(0);
	SD_TRANS_MODE_SET(2); // read
	SD_TRANS_SDPIOMODE(0); // dma
	SD_TRANS_SDRSPCHK_EN(1); // enable hw check rsp crc7
	if (IS_EMMC_SLOT()) {
		SDRSPTYPE_R2(0);
	} else {
		SD_CONFIG_SET(SD_CONFIG_GET() & (~(1ul << 13)));	/* Set response type to 6 bytes*/
	}
	SD_CMD_BUF0_SET( (unsigned char) (CMD18 + 0x40));
	SD_CMD_BUF1_SET( (unsigned char) ((cmd_args >> 24) & 0x000000ff));
	SD_CMD_BUF2_SET( (unsigned char) ((cmd_args >> 16) & 0x000000ff));
	SD_CMD_BUF3_SET( (unsigned char) ((cmd_args >> 8) & 0x000000ff));
	SD_CMD_BUF4_SET( (unsigned char) ((cmd_args) & 0x000000ff));
	SD_PAGE_NUM_SET(SectorNum - 1);

	SD_BLOCK_SIZE_SET(BLOCK_LEN_BYTES_512 - 1);
	/* Configure Group DMA Registers */
	if (IS_EMMC_SLOT()) {
		DMA_SRCDST_SET(DMA_FROM_DEVICE);
	} else {
		DMA_SRCDST_SET(0x12);
		DMA_SIZE_SET(BLOCK_LEN_BYTES_512 - 1);
	}

	SET_HW_DMA_BASE_ADDR(pRecBuff);
	if (IS_EMMC_SLOT()) {
		SD_CMP_EN(0);
		SDIO_INT_EN(0);
	} else {
		SD_INT_CONTROL_SET(SD_INT_CONTROL_GET() & (~0x11));
	}
	prn_string("eMMC DMA mode\n");
	DMA_HW_EN(0);
	SD_CTRL_SET(0x01);	/* Trigger TX command*/
	/* Wait till host controller becomes idle or error/timeout occurs */
	time0 = AV1_GetStc32() / 90;
	while ((SD_STATUS1_GET() & ((1 << 13) | (1 << 14))) == 0) {
		time1 = AV1_GetStc32() / 90;
		if ((time1 - time0) >= MMC_MAX_READ_TIME) {
			prn_string("timeout!\n");
			prn_sd_status();
			ret_value =  SD_FAIL;
			goto FAIL_MULITI_READ_SECTOR;
		}
	}
	if (SD_STATUS1_GET() & (1 << 13)) {
		prn_string("dma error!\n");
		prn_sd_status();
		ret_value =  SD_FAIL;
	}

#ifdef SD_VERBOSE
	dbg();
#endif

FAIL_MULITI_READ_SECTOR:

	if (ret_value != ERR_SUCCESS) {
		int i = 0;
		SD_RST();
		for (i = 0; i < 100; i++)
			hwSdTxDummy();
	}

	StopCardTransmission(&gStorage_dev);

	return ret_value;
}


//-------------------------------------
/**
 *	ReadSDMultipleSector - Send CMD18 to receive multiple blocks data
 *  @SectorIdx: Start index of sector for reading
 *  @SectorNum: Sector counts to read
 *  @pRedBuff: Buffer pointer for reading data from card
 *
 */
int ReadSDMultipleSector(unsigned int SectorIdx, unsigned int SectorNum,
		unsigned char* pRecBuff)
{
	int ret_value = ERR_SUCCESS;
	int cmd_args = ARGS_STUFF_BITS;
	unsigned int bytes_count = 0;
	unsigned int sector_index = 0;

#ifdef SD_VERBOSE
	//printk("[ReadSDMultipleSector] SectorIdx=%d, SectorNum=%d\n",SectorIdx,SectorNum);
	prn_string("SD Read blk="); prn_decimal_ln(SectorIdx);
	prn_string("num="); prn_decimal_ln(SectorNum);// prn_string("\n");
#endif

	if ((gStorage_dev.what_dev == WHAT_STORAGE_STANDARD_CAPACITY_SD) ||
		(gStorage_dev.what_dev == WHAT_STORAGE_STANDARD_CAPACITY_MMC)) {
		// Standard Capacity SD: unit: byte
		cmd_args |= (SectorIdx<<9); // = SectorNum*512
	} else {
		// High Capacity SD: unit: block
		cmd_args |= SectorIdx;
	}

	SD_RST();
	SD_TRANS_SDDUMMY(0);
	SD_TRANS_SDAUTORSP(0);
	SD_TRANS_MODE_SET(2); // read
	SD_TRANS_SDPIOMODE(1); // pio
	SD_TRANS_SDRSPCHK_EN(1); // enable hw check rsp crc7
	if (IS_EMMC_SLOT()) {
		SDRSPTYPE_R2(0);
	} else {
		SD_CONFIG_SET(SD_CONFIG_GET() & (~(1ul << 13)));	/* Set response type to 6 bytes*/
	}

	SD_CMD_BUF0_SET( (unsigned char) (CMD18 + 0x40));
	SD_CMD_BUF1_SET( (unsigned char) ((cmd_args >> 24) & 0x000000ff));
	SD_CMD_BUF2_SET( (unsigned char) ((cmd_args >> 16) & 0x000000ff));
	SD_CMD_BUF3_SET( (unsigned char) ((cmd_args >> 8) & 0x000000ff));
	SD_CMD_BUF4_SET( (unsigned char) ((cmd_args) & 0x000000ff));
	SD_PAGE_NUM_SET(SectorNum - 1);

	SD_BLOCK_SIZE_SET(BLOCK_LEN_BYTES_512 - 1);
	SD_CTRL_SET(0x01);	/* Trigger TX command */

	bytes_count = BLOCK_LEN_BYTES_512 * SectorNum;
	ret_value = ReadSectorByPolling(pRecBuff+(sector_index*bytes_count),&bytes_count);
	if (ret_value != ERR_SUCCESS) {
		dbg();
		goto FAIL_MULITI_READ_SECTOR;
	}

#ifdef SD_VERBOSE
	dbg();
#endif

FAIL_MULITI_READ_SECTOR:
	SD_RST();
	if (ret_value != ERR_SUCCESS) {
		int i = 0;
		for (i = 0; i < 100; i++)
			hwSdTxDummy();
	}

	StopCardTransmission(&gStorage_dev);
	//	if (ret_value != ERR_SUCCESS)
	//		sdTranStateWait();

	return ret_value;
}

/*
 * Try next combination of (rd_clk_delay, highspeed_en)
 */
void sdTryNextReadDelay(int card_num)
{
	g_bootinfo.gSD_RD_CLK_DELAY_TIME_val[card_num] =
		(g_bootinfo.gSD_RD_CLK_DELAY_TIME_val[card_num] + 1) % 8;

	if (g_bootinfo.gSD_RD_CLK_DELAY_TIME_val[card_num] == get_SD_RD_CLK_DLY_BEG(card_num)) {
		g_bootinfo.gSD_HIGHSPEED_EN_SET_val[card_num] =
			!g_bootinfo.gSD_HIGHSPEED_EN_SET_val[card_num];
	}

	SD_HIGHSPEED_EN_SET(g_bootinfo.gSD_HIGHSPEED_EN_SET_val[card_num]);
	SD_RD_CLK_DELAY_TIME_SET(g_bootinfo.gSD_RD_CLK_DELAY_TIME_val[card_num]);

	prn_string(" Set rd_clk_dly="); prn_decimal_ln(g_bootinfo.gSD_RD_CLK_DELAY_TIME_val[card_num]);
	prn_string(" high="); prn_decimal_ln(g_bootinfo.gSD_HIGHSPEED_EN_SET_val[card_num]);
}

