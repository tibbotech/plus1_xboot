#ifndef _HWSD_H_
#define _HWSD_H_

#ifndef NULL
#define NULL	((void *)0)
#endif

#define SD_SUCCESS				0
#define SD_FAIL 				1
#define OS_TICK					1

enum {
	MEDIA_NONE = 0,
	MEDIA_SMC  = 1,
	MEDIA_RESERVED1 = 2,
	MEDIA_CF   = 3,
	MEDIA_SPI  = 4,
	MEDIA_RESERVED2 = 5,
	MEDIA_SD   = 6,
	MEDIA_MS   = 7,
};

enum SD_HANDLE_STATE {
	SD_WRITE_HANDLE_IDLE = 0,
	SD_WAIT_TRAN_STATE0,
	SD_SEND_WRITE_CMD,
	SD_WRITE_DATA,
	SD_SEND_END_CMD,
	SD_WAIT_TRAN_STATE1,
	SD_WAIT_READY,
};

#define SD_MODE      			0
#define MMC_MODE     			1

#define SD_1BIT_MODE 			0
#define SD_4BIT_MODE 			1

#define RSP_TYPE_NORSP     	     0xff
#define RSP_TYPE_R1  			0
#define RSP_TYPE_R1B 			0
#define RSP_TYPE_R2  			1
#define RSP_TYPE_R3  			2
#define RSP_TYPE_R6  			3
#define RSP_TYPE_R7	 		4
#define RSP_ECHO_BYTE			0xaa
#define SD_STATE_IDLE    		0
#define SD_STATE_READY 			1
#define SD_STATE_IDENT   		2
#define SD_STATE_STBY    		3
#define SD_STATE_TRAN			4
#define SD_STATE_DATA			5
#define SD_STATE_RCV			6
#define SD_STATE_PRG			7
#define SD_STATE_DIS			8
#define SD_STATE_TIMEOUT 		9

#define SD_RSP_TIMEOUT     		0x02
#define SD_CRC_ERROR			0x03

#define DMA_SUCCESS			0
#define DMA_FAIL 			1

#define DMA_DRAM      			1
#define DMA_FLASH     			2

/******************************************************************************
*                        SD  MACRO Function Define
*******************************************************************************/

#define sd_config1(adrs)              (((volatile struct card_sd_regs *)ADDRESS_CONVERT(adrs+1*32*4))->sd_config1)




/******************************************************************************
*                     EMMC  MACRO Function Define
*******************************************************************************/
#define TO_EMMC_REG(adrs)				((volatile struct emmc_ctl_regs *)(adrs))

/* emmc */
#define boot_ack_en(adrs)     (TO_EMMC_REG(adrs)->boot_ack_en)
#define boot_ack_tmr(adrs)    (TO_EMMC_REG(adrs)->boot_ack_tmr)
#define boot_data_tmr(adrs)   (TO_EMMC_REG(adrs)->boot_data_tmr)
#define fast_boot(adrs)       (TO_EMMC_REG(adrs)->fast_boot)
#define boot_mode(adrs)       (TO_EMMC_REG(adrs)->boot_mode)
#define bootack(adrs)         (TO_EMMC_REG(adrs)->bootack)
#define resume_boot(adrs)     (TO_EMMC_REG(adrs)->resume_boot)
#define stop_page_num(adrs)     (TO_EMMC_REG(adrs)->stop_page_num)

#define dmasrc(adrs)					(TO_EMMC_REG(adrs)->dmasrc)
#define dmadst(adrs)					(TO_EMMC_REG(adrs)->dmadst)
#define sdcmpen(adrs) 				(TO_EMMC_REG(adrs)->sdcmpen)
#define sd_cmp(adrs) 					(TO_EMMC_REG(adrs)->sd_cmp)
#define sd_cmp_clr(adrs) 	  	(TO_EMMC_REG(adrs)->sd_cmp_clr)
#define sdio_int_en(adrs) 				(TO_EMMC_REG(adrs)->sdio_int_en)
#define hwdmacmpen(adrs) 				(TO_EMMC_REG(adrs)->hwdmacmpen)
#define rx4_en(adrs)					(TO_EMMC_REG(adrs)->rx4_en)
#define sdrsptmren(adrs)				(TO_EMMC_REG(adrs)->sdrsptmren)
#define sdcrctmren(adrs)				(TO_EMMC_REG(adrs)->sdcrctmren)
#define sddatawd(adrs)					(TO_EMMC_REG(adrs)->sddatawd)
#define sdfqsel(adrs)					(TO_EMMC_REG(adrs)->sdfqsel)
#define sdmmcmode(adrs)					(TO_EMMC_REG(adrs)->sdmmcmode)
#define sdrsptype(adrs)					(TO_EMMC_REG(adrs)->sdrsptype)

#define SDIO_INT_EN(x)					sdio_int_en(gDEV_SDCTRL_BASE_ADRS)		= !!(x)
#define SD_CMP_EN(x)					sdcmpen(gDEV_SDCTRL_BASE_ADRS)			= !!(x)
#define SD_CMP_CLR()					sd_cmp_clr(gDEV_SDCTRL_BASE_ADRS)	= 1
#define SD_CMP()					    sd_cmp(gDEV_SDCTRL_BASE_ADRS)
#define RX4_EN(x)						rx4_en(gDEV_SDCTRL_BASE_ADRS)			= !!(x)
#define SDRSPTMREN(x)					sdrsptmren(gDEV_SDCTRL_BASE_ADRS)		= !!(x)
#define SDCRCTMREN(x)					sdcrctmren(gDEV_SDCTRL_BASE_ADRS)		= !!(x)
#define SDDATAWD(x)						sddatawd(gDEV_SDCTRL_BASE_ADRS)			= !!(x)
#define MMC_FREQ_DIV_MASK				0xFFF
#define SDFQSEL(x)						sdfqsel(gDEV_SDCTRL_BASE_ADRS)			= (x) & MMC_FREQ_DIV_MASK
#define SDFQSEL_GET()					sdfqsel(gDEV_SDCTRL_BASE_ADRS)
#define SDMMCMODE(x)					sdmmcmode(gDEV_SDCTRL_BASE_ADRS)		= !!(x)
#define	SDRSPTYPE_R2(x)					sdrsptype(gDEV_SDCTRL_BASE_ADRS)		= !!(x)

#define BOOT_ACK_EN(x)     boot_ack_en(gDEV_SDCTRL_BASE_ADRS) = !!(x)
#define BOOT_ACK_TMR(x)    boot_ack_tmr(gDEV_SDCTRL_BASE_ADRS) = !!(x)
#define BOOT_DATA_TMR(x)   boot_data_tmr(gDEV_SDCTRL_BASE_ADRS) = !!(x)
#define FAST_BOOT(x)       fast_boot(gDEV_SDCTRL_BASE_ADRS) = !!(x)
#define BOOT_MODE(x)       boot_mode(gDEV_SDCTRL_BASE_ADRS) = !!(x)
#define BOOTACK()         bootack(gDEV_SDCTRL_BASE_ADRS)
#define RESUME_BOOT(x)     resume_boot(gDEV_SDCTRL_BASE_ADRS) = !!(x)
#define STOP_PAGE_NUM(x)   stop_page_num(gDEV_SDCTRL_BASE_ADRS) = (x)

/******************************************************************************
*                SD AND EMMC MACRO Function Define
*******************************************************************************/
#define IS_EMMC_SLOT()    ((gDEV_SDCTRL_BASE_ADRS == (unsigned int)ADDRESS_CONVERT(CARD0_CTL_REG)) || (gDEV_SDCTRL_BASE_ADRS == (unsigned int)ADDRESS_CONVERT(CARD1_CTL_REG)))


#define SD_WAIT_RSP_TIME_SET(x) \
	do { \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdrsptmr) = (x) & SD_WAIT_RSP_TIME_MASK; \
	} while (0)
#define SD_HIGHSPEED_EN_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_high_speed_en) = !!(x); \
	}while(0)

#define SD_DELAY_MASK					0x7

#define SD_CLK_DLY_SET(x) \
	do { \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_clk_dly_sel) = (x); \
	}while(0)
#define SD_CLK_DLY_GET(x) \
	({ \
		unsigned int ret; \
		ret = TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_clk_dly_sel; \
		(ret); \
	})

#define SD_WR_DAT_DLY_SET(x) \
	do { \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_wr_dat_dly_sel) = (x); \
	}while(0)
#define SD_WR_DAT_DLY_GET(x) \
	({ \
		unsigned int ret; \
		ret = TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_wr_dat_dly_sel; \
		(ret); \
	})

#define SD_WR_CMD_DLY_SET(x) \
	do { \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_wr_cmd_dly_sel) = (x);  \
	}while(0)
#define SD_WR_CMD_DLY_GET(x) \
	({ \
		unsigned int ret; \
		ret = TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_wr_cmd_dly_sel; \
		(ret); \
	})

#define SD_RD_DAT_DLY_SET(x) \
	do { \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rd_dat_dly_sel) = (x); \
	}while(0)
#define SD_RD_DAT_DLY_GET(x)																				\
	({ \
		unsigned int ret; \
		ret = TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rd_dat_dly_sel; \
		(ret); \
	})

#define SD_RD_RSP_DLY_SET(x) \
	do { \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rd_rsp_dly_sel) = (x); \
	}while(0)
#define SD_RD_RSP_DLY_GET(x) \
	({ \
		unsigned int ret; \
		ret = TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rd_rsp_dly_sel; \
		(ret); \
	})

#define SD_RD_CRC_DLY_SET(x) \
	do { \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rd_crc_dly_sel) = (x); \
	}while(0)
#define SD_RD_CRC_DLY_GET(x) \
	({ \
		unsigned int ret; \
		ret = TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rd_crc_dly_sel; \
		(ret); \
	})

#define SD_WT_CLK_DELAY_TIME_SET(x) \
	do { \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_wr_cmd_dly_sel) = (x); \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_wr_dat_dly_sel) = (x); \
	}while(0)


#define SD_RD_CLK_DELAY_TIME_SET(x) \
	do { \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rd_rsp_dly_sel) = (x); \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rd_dat_dly_sel) = (x); \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rd_crc_dly_sel) = (x); \
	}while(0)

#define SD_WAIT_RSP_TIME_MASK			0x7FF
#define SD_WAIT_CRC_TIME_MASK			0x7FF
#define SD_WAIT_CRC_TIME_SET(x)	\
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdcrctmr) = (x) & SD_WAIT_CRC_TIME_MASK; \
	}while(0)

#define SD_BLOCK_SIZE_MASK				0x7FF

#define SD_BLOCK_SIZE_SET(x) \
	 do { \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_blocksize) = (x) & SD_BLOCK_SIZE_MASK; \
	} while (0)

#define SD_BLOCK_SIZE_GET() \
	 ({ \
		unsigned int ret; \
		ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_blocksize); \
		(ret); \
	})

#define SD_TRANS_SDRSPCHK_EN(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdrspchk_en) = !!(x); \
	}while(0)

#define SD_TRANS_SDDUMMY(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdcmddummy) = !!(x); \
	}while(0)

#define SD_TRANS_SDAUTORSP(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdautorsp) = !!(x); \
	}while(0)

#define SD_TRANS_MODE_MASK				0x3
#define SD_TRANS_MODE_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_trans_mode) = (x) & SD_TRANS_MODE_MASK; \
	}while(0)

#define SD_TRANS_SDLENMODE(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_len_mode) = !!(x); \
	}while(0)

#define SD_TRANS_SDDDRMODE(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdddrmode) = !!(x); \
	}while(0)

#define SD_TRANS_SDPIOMODE(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdpiomode) = !!(x); \
	}while(0)

#define SD_CMD_BUF0_GET()      \
	({ \
		unsigned int ret; \
		ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[3]) & 0x3f; \
		(ret); \
	})

#define SD_CMD_BUF0_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[3]) = (x); \
	}while(0)
#define SD_CMD_BUF1_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[2]) = (x); \
	}while(0)
#define SD_CMD_BUF2_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[1]) = (x); \
	}while(0)
#define SD_CMD_BUF3_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[0]) = (x); \
	}while(0)
#define SD_CMD_BUF4_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[4]) = (x); \
	}while(0)


#define SD_PAGE_NUM_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_page_num) = (x); \
	}while(0)


#define SD_RSP_BUF0_3_GET \
	({ \
		unsigned int ret; \
		ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rspbuf0_3); \
		(ret);\
	})
#define SD_RSP_BUF4_5_GET \
	({ \
		unsigned int ret; \
		ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rspbuf4_5); \
		(ret);\
	})
#define SD_HW_STATE_GET() \
	({ \
		unsigned int ret; \
		ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_hw_state); \
		(ret);\
	})


#define SD_HW_CMD13_RCA_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_hw_cmd13_rca) = (x); \
	}while(0)

#define SD_RSP_TIMEOUT_ERROR() \
	({ \
		unsigned int ret; \
		ret = ((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 6) & 0x1; \
		(ret);\
	})

#define SD_DATA_CRC_TIMEOUT_ERROR() \
	({ \
		unsigned int ret; \
		ret = (((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 7) & 0x1); \
		(ret);\
	})

#define SD_DATA_STB_TIMEOUT_ERROR() \
	({ \
		unsigned int ret; \
		ret = ((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 8) & 0x1; \
		(ret);\
	})

#define SD_DATA_CRC7_ERROR() \
	({ \
		unsigned int ret; \
		ret = ((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 9) & 0x1; \
		(ret);\
	})

#define SD_DATA_CRC_ERROR() \
	({ \
		unsigned int ret; \
		ret = ((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 11) & 0x1; \
		(ret);\
	})

#define CARD_MEDIA_TYPE_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->mediatype) = (x); \
	}while(0)

#define hwSdCardCrcState() \
	({ \
		unsigned int ret; \
		ret = ((((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_state) & 0x70) >> 4) ==  0x02); \
		(ret);\
	})

#define hwSdDat0Check() \
	({ \
		unsigned int ret; \
		ret = (((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) & 0x20) >> 5); \
		(ret);\
	})

#define hwSdTimeOut() \
	({ \
		unsigned int ret; \
		ret = (((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 7) & 0x01); \
		(ret);\
	})


#define SD_CONFIG_SET(x)         sd_config1(gDEV_SDCTRL_BASE_ADRS) = (x)
#define SD_CONFIG_GET() \
	({ \
		unsigned int ret; \
		ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_config1); \
		(ret);\
	})

#define SD_CONFIG0_GET() \
	({ \
		unsigned int ret; \
		ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_config0); \
		(ret);\
	})


#define SD_RST() \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rst) = 0x03; \
	}while(0)

#define SD_CTRL_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_ctrl) = (x); \
	}while(0)

#define SD_STATUS0_GET() \
	({ \
		unsigned int ret; \
		ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status); \
		(ret);\
	})



#define SD_STATUS1_GET() \
	({ \
		unsigned int ret; \
		ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_state); \
		(ret);\
	})

#define SD_STATUS_NEW_ERR()     ((SD_STATUS1_GET()>>13)&0x01)

#define SD_PIO_RX_DATA_GET() \
	({ \
		unsigned int ret; \
		ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_piodatarx); \
		(ret);\
	})


#define DMA_FROM_DEVICE					0
#define DMA_TO_DEVICE					1

#define DMA_MEMORY						1
#define DMA_FLASH						2

#define DMA_SRCDST_SET(x) \
	do { \
		if ((x) ==  DMA_TO_DEVICE) { \
			dmasrc(gDEV_SDCTRL_BASE_ADRS) = DMA_MEMORY; \
			dmadst(gDEV_SDCTRL_BASE_ADRS) = DMA_FLASH; \
		} else { \
			dmasrc(gDEV_SDCTRL_BASE_ADRS) = DMA_FLASH; \
			dmadst(gDEV_SDCTRL_BASE_ADRS) = DMA_MEMORY; \
		} \
	} while (0)

/* only sd card need this */
#define DMA_SIZE_SET(x)					/* for compile warning*/
#define SD_INT_CONTROL_SET(x)			/* for compile warning*/
#define SD_INT_CONTROL_GET()			/* for compile warning*/

#define SET_HW_DMA_BASE_ADDR(DRAMAddr) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->dma_base_addr) = (UINT32)(DRAMAddr); \
	}while(0)

#define DMA_HW_EN(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->hwdmacmpen) = !!(x); \
	}while(0)


#define SD_TXDUMMY_MASK					0x1FF
#define SD_TXDUMMY_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->tx_dummy_num) = (x); \
	}while(0)


#define SD_RXDATTMR_GET(x) \
	({ \
		unsigned int ret; \
		ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rxdattmr); \
		(ret);\
	})

#define SD_RXDATTMR_MASK				0x1FFFFFFF
#define SD_RXDATTMR_SET(x) \
	do{ \
		(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rxdattmr) = (x) & SD_RXDATTMR_MASK; \
	}while(0)

#endif //#ifndef _HWSD_H_
