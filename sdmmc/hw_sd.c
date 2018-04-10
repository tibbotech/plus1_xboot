/******************************************************************************
*                          Include File
*******************************************************************************/
#include <nand_boot/nandop.h>
#include <sdmmc_boot/hw_sd.h>
#include <sdmmc_boot/hal_sd_mmc.h>
#include <regmap.h>
#include <common.h>


/******************************************************************************
*                          MACRO Function Define
*******************************************************************************/
#if 0
#define EPRINTK(args...) printk(args)
#else
#define EPRINTK(args...)
#endif
#define SD_MAX_RESPONSE_TIME   1000  //ms

/******************************************************************************
*                          Global Variabl
*******************************************************************************/

/******************************************************************************
*                         Function Prototype
*******************************************************************************/
void hwSdInit(unsigned int);
void hwSdConfig(unsigned int, unsigned int);
unsigned int hwSdRxResponse(unsigned char *, unsigned int);
unsigned int hwSdTxDummy(void);
unsigned int hwSdCmdSend(unsigned int, unsigned int, unsigned int, unsigned char *);
unsigned int hwSdIdleWait(void);
unsigned int sdStateGet(void);
int get_card_number(void);

void prn_sd_status(void)
{
	prn_string(" CMD:"); prn_decimal(SD_CMD_BUF0_GET());
	prn_string(" SD status0="); prn_dword0(SD_STATUS0_GET());
	prn_string(" status1="); prn_dword(SD_STATUS1_GET());
}

/**************************************************************************
 *                                                                        *
 *  Function Name: hwSdInit                                               *
 *                                                                        *
 *  Purposes:                                                             *
 *    SD/MMC interface initialization                                     *
 *  Descriptions:                                                         *
 *    SD/MMC interface initialization                                     *
 *  Arguments:    mmcMode=1:MMC mmcMode=0:SD                              *
 *  Returns:   NONE                                                       *
 *                                                                        *
 *  See also:  NONE                                                       *
 *                                                                        *
 **************************************************************************/
void hwSdInit(unsigned int mmcMode)
{
	SD_WAIT_RSP_TIME_SET(0x7ff);
	SD_WAIT_CRC_TIME_SET(0x3ff);
	// sd_config
	// Bit17: rx4B_en    - 1: 4 bytes, 0: 2 bytes
	// Bit14: sdmmcmode  - 0: sd mode, 1: mmc mode
	// Bit13: sdcrctmren - crc timer enable
	// Bit12: sdrsptmren - rsp timer enable
#ifdef PLATFORM_8388
	SD_CONFIG_SET((SD_CONFIG_GET() & ~ (0x7000 | (1<<17))) | 0x3000 | (1 << 17));
	SD_RXDATTMR_SET(3);
#else
	RX4_EN(1);
	SDRSPTMREN(1);
	SDCRCTMREN(1);
	SD_RXDATTMR_SET(SD_RXDATTMR_MASK);
#endif
	SD_TXDUMMY_SET(8);
	SD_RST();
}

/**************************************************************************
 *                                                                        *
 *  Function Name: hwSdBusWidthSet                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *    Set SD data bus width                                               *
 *  Descriptions:                                                         *
 *    Set SD data bus width                                               *
 *  Arguments:                                                            *
 *  sdBusWidth     0:1 bit data bus, 1:4 bits data bus                    *
 *  Returns:     NONE                                                     *
 *                                                                        *
 *  See also:    NONE                                                     *
 *                                                                        *
 **************************************************************************/
void hwSdBusWidthSet(unsigned char sdBusWidth)
{
#ifdef PLATFORM_8388
	unsigned int value;
	unsigned int tval;

	tval = (sdBusWidth==BUS_WIDTH_4BIT) ? 1 : 0;
	value = SD_CONFIG_GET();
	value = (value & ~(1<<10)) | (tval << 10);
	SD_CONFIG_SET(value);
#else
	SDDATAWD((sdBusWidth==BUS_WIDTH_4BIT) ? 1 : 0);
#endif
}

void hwSdFreqSet(unsigned int sdFreq)
{
#ifdef PLATFORM_8388
	unsigned int value;

	value = SD_CONFIG_GET();
	value = (value & 0xfffffc00) | (sdFreq & 0x3ff);
	SD_CONFIG_SET(value);
#else
	SDFQSEL(sdFreq & MMC_FREQ_DIV_MASK);
#endif
}

/**************************************************************************
 *                                                                        *
 *  Function Name: hwSdConfig                                             *
 *                                                                        *
 *  Purposes:                                                             *
 *    Set SD operation frequency , data bus width, MMC mode               *
 *  Descriptions:                                                         *
 *    Set SD operation frequency , data bus width, MMC mode               *
 *  Arguments:                                                            *
 *  sdFreq         0:24MHz, 1:12MHz, 2:6MHz 3:375KHz                      *
 *  sdBusWidth     0:1 bit data bus, 1:4 bits data bus                    *
 *  mmcMode        0:SD mode 1:MMC mode                                   *
 *  Returns:     NONE                                                     *
 *                                                                        *
 *  See also:    NONE                                                     *
 *                                                                        *
 **************************************************************************/
//void hwSdConfig(unsigned int sdFreq, unsigned int sdBusWidth, unsigned int mmcMode)
void hwSdConfig(unsigned int sdBusWidth, unsigned int mmcMode)
{
#ifdef PLATFORM_8388
	unsigned int tmp;

	tmp = SD_CONFIG_GET();

	tmp &= 0xffff8800;

	//tmp |= (unsigned short) ((mmcMode << 14) | (sdBusWidth << 10) | (sdFreq | 0x3000));
	tmp |= (unsigned short) ((mmcMode << 14) | (sdBusWidth << 10) | 0x3000);

	SD_CONFIG_SET(tmp);
#else
	SDDATAWD(sdBusWidth);
	SDMMCMODE(mmcMode);
	SDRSPTMREN(1);
	SDCRCTMREN(1);
#endif
}

unsigned int hwSdBlockSizeGet()
{
	return SD_BLOCK_SIZE_GET();
}


/**************************************************************************
 *                                                                        *
 *  Function Name: hwSdRxResponse                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *    Receive SD card response                                            *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    rspType - 0: length of 6 bytes, 1: length of 17 bytes               *
 *                                                                        *
 *  Returns:                                                              *
 *      0x00 - No error                                                   *
 *      0x01 - crc7 error                                                 *
 *      0x02 - response timeout                                           *
 *                                                                        *
 *  See also:    NONE                                                     *
 *                                                                        *
 **************************************************************************/
unsigned int hwSdRxResponse(unsigned char * rspBuf, unsigned int rspType)
{
	unsigned int status = SD_SUCCESS;
	unsigned int rspNum;
	unsigned int value;
	unsigned int time0 = AV1_GetStc32() / 90;  //ms
	unsigned int time1 = 0;

	while (1) {
		value = SD_STATUS0_GET();

		if ((value & 0x02) == 0x02) {
			break;	/* Response buffers are full, break*/
		} else if ((value & 0x40) == 0x40) {
			prn_string("Rsp timeout (0)\n"); // No card!
			prn_sd_status();
			return SD_RSP_TIMEOUT;
		} else {
			time1 = AV1_GetStc32() / 90;
			if ((time1 - time0) >= SD_MAX_RESPONSE_TIME){
				prn_string("timeout!\n");
				prn_sd_status();
				return SD_FAIL;
			}
		}
	}

	value = SD_RSP_BUF0_3_GET;
	rspBuf[0] = (value >> 24) & 0xff;
	rspBuf[1] = (value >> 16) & 0xff;
	rspBuf[2] = (value >>  8) & 0xff;
	rspBuf[3] = (value)       & 0xff;
	CSTAMP(*((unsigned int *)rspBuf));
	value = SD_RSP_BUF4_5_GET;
	rspBuf[4] = (value >>  8) & 0xff;

#ifdef SD_VERBOSE
	// SD RSP print start
	prn_string("SD RSP=");
	for (rspNum = 0; rspNum < 5; rspNum++)
		prn_byte(rspBuf[rspNum]);
#endif

	if (rspType == RSP_TYPE_R2) {
#ifdef PLATFORM_8388
		for (rspNum = 5; rspNum < 16; rspNum++) {
				while (1) {
							value = SD_STATUS0_GET();
							if ((value & 0x02) == 0x02) {
									break;	/* Wait until response buffer full*/
							} else if ((value & 0x40) == 0x40) {
									EPRINTK("Rsp timeout (1)...\n");
									return SD_RSP_TIMEOUT;
							}
					}
					rspBuf[rspNum] = (SD_RSP_BUF4_5_GET & 0xff);
#ifdef SD_VERBOSE
					prn_byte(rspBuf[rspNum]); // conti. SD RSP print
#endif
			}
#else
		for (rspNum = 5; rspNum < 16; ) {
			value = SD_RSP_BUF0_3_GET;
			rspBuf[rspNum++] = (value >> 24) & 0xff;
			rspBuf[rspNum++] = (value >> 16) & 0xff;
			rspBuf[rspNum++] = (value >>  8) & 0xff;
			rspBuf[rspNum++] = (value)       & 0xff;
			value = SD_RSP_BUF4_5_GET;
			rspBuf[rspNum++] = (value >>  8) & 0xff;
		}
#ifdef SD_VERBOSE
		for (rspNum = 5; rspNum < 16; rspNum++)
			prn_byte(rspBuf[rspNum]);
#endif
#endif
	}
#ifdef SD_VERBOSE
	// SD RSP print end
	prn_string("\n");
#endif

	// Check RSP CRC7 error
	if ((rspType == RSP_TYPE_R1) || (rspType == RSP_TYPE_R6) || rspType == RSP_TYPE_R7) {
		if (SD_STATUS0_GET() & (1 << 9)) {
			prn_string("RSP CRC7 err\n");
			status = SD_CRC_ERROR;	/*CRC7 error*/
		}
	}

	return status;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: hwSdTxDummy                                            *
 *                                                                        *
 *  Purposes:                                                             *
 *    Trigger hardware to transmit 8 dummy clock cycles                   *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:    NONE                                                    *
 *  Returns:      NONE                                                    *
 *  See also:     NONE                                                    *
 *                                                                        *
 **************************************************************************/
unsigned int hwSdTxDummy(void)
{
	SD_CTRL_SET(0x2);	/*trigger TxDummy command*/
	return hwSdIdleWait();
}

/**************************************************************************
 *                                                                        *
 *  Function Name: hwSdIdleWait                                           *
 *                                                                        *
 *  Purposes:                                                             *
 *              Wait SD/MMC card idle                                     *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:   NONE                                                     *
 *                                                                        *
 *  Returns:     SUCCESS/ FAIL                                            *
 *                                                                        *
 *  See also:    NONE                                                     *
 *                                                                        *
 **************************************************************************/
unsigned int hwSdIdleWait(void)
{
	unsigned int status, time0, time1;

	/* Wait hw state machine become idle
	 * rd says:it only need 5 clocks to become idle
	 * anway set timeout to 10ms there
	 */

	time0 = AV1_GetStc32() / 90;
	while (1) {
		status = SD_STATUS1_GET();
		if (status & (0x1 << 13)) { // error/timeout
			prn_string("SDCTL error\n");
			prn_sd_status();
			return SD_FAIL;
		}
		if (status & (0x1 << 14))   // finish/idle
			return SD_SUCCESS;

		time1 = AV1_GetStc32() / 90;
		if ((time1 - time0) >= 10){
			break;
		}
	}
	prn_string("SDCTL wait timeout\n");
	prn_sd_status();

	return SD_FAIL;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: SetMediaTypeMode                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *      set card media's transfers mode                                   *
 *  Descriptions:                                                         *
 *      In dma mode, set card media's dma buffer                          *
 *  Arguments:                                                            *
 *      ActFlash: current media(MEDIA_CF\MEDIA_SD\MEDIA_MS\MEDIA_SMC)     *
 *      PageSize: dma buffer size                                         *
 *  Returns:                                                              *
 *      None                                                              *
 *  See also:                                                             *
 *      None                                                              *
 **************************************************************************/
void SetMediaTypeMode(unsigned char ActFlash, unsigned short PageSize)
{
	CARD_MEDIA_TYPE_SET(ActFlash);
#ifdef PLATFORM_8388
	DMA_SIZE_SET((PageSize - 1) & 0x7ff);	/*setup dma buffer size*/
#endif
}

/**************************************************************************
 *                                                                        *
 *  Function Name: sdStateGet                                             *
 *                                                                        *
 *  Purposes:       Get SD/MMC card status                                *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:    NONE                                                    *
 *                                                                        *
 *  Returns:      Card status                                             *
 *                                                                        *
 *  See also:     NONE                                                    *
 *                                                                        *
 **************************************************************************/
unsigned int sdStateGet(void)
{
	unsigned char rspBuf[6];
	//	unsigned int tmp;
	unsigned int ret, nrRetry;
	struct STORAGE_DEVICE* pStroage_dev = 0;

	pStroage_dev = &gStorage_dev;

	nrRetry = 100;
	ret = SD_FAIL;
	while ((ret != SD_SUCCESS) && (nrRetry > 0)) {
		ret = hwSdCmdSend(CMD13, (pStroage_dev->dev_card.RCA_Address <<16), RSP_TYPE_R1, rspBuf);
		nrRetry--;
	}

	if (nrRetry == 0) {
		EPRINTK("nr tetry is 0\n");
		return SD_STATE_TIMEOUT;
	}

	//	tmp = (rspBuf[1] << 24) +
	//	      (rspBuf[2] << 16) +
	//	      (rspBuf[3] << 8 ) +
	//	      (rspBuf[4]      );

	return (rspBuf[3] >> 1) & 0x0f;
}


/**************************************************************************
 *                                                                        *
 *  Function Name: hwSdCmdSend                                            *
 *                                                                        *
 *  Purposes:                                                             *
 *    Send a command to SD card and verify the reponse's validity.        *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    cmd: in, SD command with transmission bit                           *
 *    arg: in, argument of SD command                                     *
 *    rspType: in, type of response, it can be RSP_TYPE_7 / RSP_TYPE_16   *
 *    prspBuf: out, pointer to buffer of storing reponse. If the ponter   *
 *             is NULL, no response will be read out.                     *
 *                                                                        *
 *  Returns:                                                              *
 *    SUCCESS: command is sent successfully                               *
 *    FAIL: command's response is wrong                                   *
 *                                                                        *
 *  See also:     NONE                                                    *
 *                                                                        *
 **************************************************************************/
unsigned int hwSdCmdSend(unsigned int cmd, unsigned int arg,
		unsigned int rspType, unsigned char * prspBuf)
{
	unsigned int err;
	unsigned int ori_cmd;
#ifdef PLATFORM_8388
	unsigned int value;
#endif

	CSTAMP(0xCCAD0001); // prepare to send CMD
	CSTAMP(cmd);
	CSTAMP(arg);

	ori_cmd = cmd;
	// Remove 0xFxxxxxxxx for ACMD & MMCCMD
	cmd = (cmd & 0x0FFFFFFF);

	err = 0;
	SD_RST();
#ifdef SD_VERBOSE
	prn_string("SD CMD=");
	prn_decimal(cmd);
	prn_string(", arg=");
	prn_dword(arg);
	//prn_string("\n");
#endif

	//
	// Set Transcation Mode
	//

	// Case 1: NO RSP
	if (rspType == RSP_TYPE_NORSP) {
		SD_TRANS_SDDUMMY(1);   // 1: auto transmit dummy after cmd or rsp
		SD_TRANS_SDAUTORSP(0); // 0: cmd0 has no rsp, 1: other cmds - auto receive rsp
		SD_TRANS_MODE_SET(0);
	} else {
		// Case 2: RSP only
		// CMD17: read one page
		// CMD18: read multiple pages
		// ACMD51: get SCR (8 byte data)
		// ACMD13: send SD status(64byte)
		// emmc cmd8
		if ((cmd != CMD17) && (cmd != CMD18) &&
			(ori_cmd != ACMD51) && (ori_cmd != ACMD13) && (NULL != prspBuf)) {
			SD_TRANS_SDDUMMY(1);
			SD_TRANS_SDAUTORSP(1);
			SD_TRANS_MODE_SET(0); // only cmd/rsp
		// Case 3: RSP + DATA
		} else {
			SD_TRANS_SDDUMMY(0);
			SD_TRANS_SDAUTORSP(0);
			SD_TRANS_MODE_SET(2); // read data transfer
		}
	}

	// CMD RSP with CRC7?
	if (rspType == RSP_TYPE_R1 ||rspType == RSP_TYPE_R6 ||
			rspType == RSP_TYPE_R7) {
		SD_TRANS_SDRSPCHK_EN(1); // enable hw check rsp crc7
	} else {
		SD_TRANS_SDRSPCHK_EN(0); // disable hw check rsp crc7
	}

	// Set RSP type
	if (rspType != RSP_TYPE_R2) {
#ifdef PLATFORM_8388
		value = SD_CONFIG_GET();
		SD_CONFIG_SET(value & 0xfffff7ff);	/* Set response type to 6 bytes*/
#else
		SDRSPTYPE_R2(0);
#endif
	} else {
#ifdef PLATFORM_8388
		value = SD_CONFIG_GET();
		SD_CONFIG_SET(value | 0x00000800);	/*Set response type to 17 bytes*/
#else
		SDRSPTYPE_R2(1);
#endif
	}

	SD_CMD_BUF0_SET( (unsigned char) (cmd + 0x40));
	SD_CMD_BUF1_SET( (unsigned char) ((arg >> 24) & 0x000000ff));
	SD_CMD_BUF2_SET( (unsigned char) ((arg >> 16) & 0x000000ff));
	SD_CMD_BUF3_SET( (unsigned char) ((arg >> 8) & 0x000000ff));
	SD_CMD_BUF4_SET( (unsigned char) ((arg) & 0x000000ff));

	SD_CTRL_SET(0x01);	/* Trigger TX command*/

	CSTAMP(0xCCAD0002); // tx cmd triggered

	err = 0xffff;		/* Check if timeout occurs */

	if (rspType == RSP_TYPE_NORSP) {
		// Wait until SM idle
		while ((SD_STATUS1_GET() & 0x0f) != 0x00) {
			err--;		/* Wait for response */
			if (err == 0) {
				prn_string("Timeout cmd="); prn_dword(cmd);
				prn_sd_status();
				return SD_FAIL;
			}
		}
		return SD_SUCCESS;
	}

	CSTAMP(0xCCAD0003); // wait end

	// No data path: recive RSP only
	if ((cmd != CMD17) && (cmd != CMD18) && (ori_cmd != ACMD51)) {
		if (prspBuf != NULL) { /* Read the response if the buf is not NULL */
			CSTAMP(0xCCAD0004); // wait rx rsp
			err = hwSdRxResponse(prspBuf, rspType);
			if (err) {
				SD_RST();
				CSTAMP(0xCCAD0005); // rx rsp failed
				prn_string("RxRsp end err="); prn_dword(err);
				prn_string("cmd:"); prn_decimal(cmd);
				if (err == SD_CRC_ERROR) {
					sdTryNextReadDelay(get_card_number());
				} else if ((err == SD_RSP_TIMEOUT) && (cmd == 1)){
					sdTryNextReadDelay(get_card_number());
					return SD_RSP_TIMEOUT;
				}
			}

			hwSdIdleWait(); /* wait state idle after receive response */

			hwSdTxDummy();	/*SD spec, section 4.4*/

			if (err != SD_SUCCESS) {
				EPRINTK("cmd%d error %d\n", cmd, err);
				//return SD_FAIL;
				return err;
			}

			CSTAMP(0xCCAD0006); // rx rsp got

			if ((rspType == RSP_TYPE_R1) || (rspType == RSP_TYPE_R6) || (rspType == RSP_TYPE_R7)) {
				if (prspBuf[0] != cmd) {
					CSTAMP(0xCCAD0007); // rx rsp incorrect 1
					EPRINTK("Cmd not equal to response\n");
					return SD_FAIL;
				}
			} else {
				if (prspBuf[0] != 0x3f) {
					/* Response R2 or R3 */
					CSTAMP(0xCCAD0008); // rx rsp incorrect 2
					EPRINTK("R2/R3 err, %x\n",prspBuf[0]);
					sdTryNextReadDelay(get_card_number()); //the only reason for buf[0] is not 0x3f is rd clk error
					return SD_CRC_ERROR;
				}
			}
		}
	}

	CSTAMP(0xCCAD000A); // sd cmd success
	return SD_SUCCESS;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: sdStateCheck                                           *
 *                                                                        *
 *  Purposes:       Check SD/MMC card status                              *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:    NONE                                                    *
 *                                                                        *
 *  Returns:      Card status                                             *
 *                                                                        *
 *  See also:     NONE                                                    *
 *                                                                        *
 **************************************************************************/
unsigned int sdStateCheck(unsigned int switch_status)
{
	unsigned char rspBuf[6];
	unsigned int ret, state_check_retry;
	struct STORAGE_DEVICE* pStroage_dev = 0;

	pStroage_dev = &gStorage_dev;

	switch (switch_status) {
	case SD_STATE_TRAN:
		ret = sdStateGet();

		if (ret == SD_STATE_STBY) {
			state_check_retry = 1000;
			do {
				ret = hwSdCmdSend(CMD7, (pStroage_dev->dev_card.RCA_Address<<16), RSP_TYPE_R1, rspBuf);
				if (ret != SD_SUCCESS) {
					EPRINTK("sd cmd 7 error for stack switch(2).\n");
				}

				ret = sdStateGet();

				if (state_check_retry > 0) {
					state_check_retry--;
				} else {
					EPRINTK("statecheck fail 1......\n");
					return SD_FAIL;
				}
			} while (ret != SD_STATE_TRAN);

		} else if (ret == SD_STATE_PRG) {
			state_check_retry = 1000;

			do {
				ret = sdStateGet();

				if (state_check_retry > 0) {
					state_check_retry--;
				} else {
					EPRINTK("statecheck fail 2......\n");
					return SD_FAIL;
				}
			} while (ret != SD_STATE_TRAN);
		} else if (ret != SD_STATE_TRAN) {
			EPRINTK("card status is error ret:%d (2).\n", ret);
			return SD_FAIL;
		}

		break;

	default:
		EPRINTK("check status don't support.\n");
		return SD_FAIL;
	}

	return SD_SUCCESS;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: sdTranStateWait                                        *
 *                                                                        *
 *  Purposes:   Wait SD card to TranState                                 *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: NONE                                                       *
 *                                                                        *
 *  Returns:   SUCCESS/FAIL                                               *
 *                                                                        *
 *  See also:  NONE                                                       *
 *                                                                        *
 **************************************************************************/
unsigned int sdTranStateWait(void)
{
	unsigned int retry;
	unsigned int state;
	unsigned int ret;
	EPRINTK("sdTranStateWait\n");

	ret = SD_FAIL;
	retry = 10000;
	while (retry > 0) {
		state = sdStateGet();
		if (state == SD_STATE_TRAN) {
			ret = SD_SUCCESS;
			break;
		}

		if (state == SD_STATE_TIMEOUT) {
			prn_string("timeout\n");
			ret = SD_FAIL;
			break;
		}

		retry--;
	}

	return ret;
}

