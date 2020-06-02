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

// CARD_CTRL: General Registers
#define card_mediatype(adrs)          (((volatile struct card_ctl_regs *)(adrs))->card_mediatype)
#define card_cpu_page_cnt(adrs)       (((volatile struct card_ctl_regs *)(adrs))->card_cpu_page_cnt)
#define card_fm_page_cnt(adrs)        (((volatile struct card_ctl_regs *)(adrs))->card_ctl_page_cnt)
#define card_ring_page_limit(adrs)    (((volatile struct card_ctl_regs *)(adrs))->sdram_sector0_sz)

// CARD_CTL: SD controller
#define sd_rst(adrs)                  (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_rst)
#define sd_config(adrs)               (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_config)
#define sd_ctrl(adrs)                 (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_ctrl)
#define sd_status0(adrs)              (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_status)
#define sd_status1(adrs)              (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_state)
#define sd_blocksize(adrs)            (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_blocksize)
#define sd_hw_config(adrs)            (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_hwdma_config)
#define sd_timing_config0(adrs)       (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_timing_config0)
#define sd_timing_config1(adrs)       (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_timing_config1)
#define sd_piodatatx(adrs)            (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_piodatatx)
#define sd_piodatarx(adrs)            (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_piodatarx)
#define sd_cmdbuf0(adrs)              (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_cmdbuf[0])
#define sd_cmdbuf1(adrs)              (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_cmdbuf[1])
#define sd_cmdbuf2(adrs)              (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_cmdbuf[2])
#define sd_cmdbuf3(adrs)              (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_cmdbuf[3])
#define sd_cmdbuf4(adrs)              (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_cmdbuf[4])
#define sd_config0(adrs)              (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_config0)
#define sd_page_num(adrs)          (((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_page_num)
#define sd_int(adrs) 					(((volatile struct card_sd_regs *)(adrs+1*32*4))->sd_int)

// CARD_CTL: SD controller and MS controller
#define sd_rspbuf0_3(adrs)            (((volatile struct card_sdms_regs *)(adrs+2*32*4))->sd_rspbuf0_3)
#define sd_rspbuf4_5(adrs)            (((volatile struct card_sdms_regs *)(adrs+2*32*4))->sd_rspbuf4_5)
#define sd_crc7buf(adrs)              (((volatile struct card_sdms_regs *)(adrs+2*32*4))->sd_crc7buf)
#define sd_crc16buf0lsb(adrs)         (((volatile struct card_sdms_regs *)(adrs+2*32*4))->sd_crc16buf0)
#define sd_hw_state(adrs)             (((volatile struct card_sdms_regs *)(adrs+2*32*4))->sd_hw_state)
#define sd_crc16buf1lsb(adrs)         (((volatile struct card_sdms_regs *)(adrs+2*32*4))->sd_crc16buf1)
#define sd_hw_cmd13_rca(adrs)         (((volatile struct card_sdms_regs *)(adrs+2*32*4))->sd_hw_cmd13_rca)
#define sd_crc16buf2lsb(adrs)         (((volatile struct card_sdms_regs *)(adrs+2*32*4))->sd_crc16buf2)
#define sd_tx_dummy_num(adrs)         (((volatile struct card_sdms_regs *)(adrs+2*32*4))->sd_tx_dummy_num)
#define sd_crc16buf3lsb(adrs)         (((volatile struct card_sdms_regs *)(adrs+2*32*4))->sd_crc16buf3)
#define sd_clk_dly(adrs)              (((volatile struct card_sdms_regs *)(adrs+2*32*4))->sd_clk_dly)
#define ms_piodmarst(adrs)            (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_piodmarst)
#define ms_cmd(adrs)                  (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_cmd)
#define ms_hw_state(adrs)             (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_hw_state)
#define ms_modespeed(adrs)            (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_modespeed)
#define ms_timout(adrs)               (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_timeout)
#define ms_state1(adrs)               (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_state)
#define ms_state2(adrs)               (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_status)
#define ms_rddata0(adrs)              (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_rddata[0])
#define ms_rddata1(adrs)              (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_rddata[1])
#define ms_rddata2(adrs)              (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_rddata[2])
#define ms_rddata3(adrs)              (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_rddata[3])
#define ms_crcbuflsb(adrs)            (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_crcbuf[0])
#define ms_crcbufmsb(adrs)            (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_crcbuf[1])
#define ms_crcerror(adrs)             (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_crcerror)
#define ms_piordy(adrs)               (((volatile struct card_sdms_regs *)(adrs+2*32*4))->ms_piordy)

// CARD_CTL: MS controller
#define ms_wd_data0(adrs)             (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[0])
#define ms_wd_data1(adrs)             (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[1])
#define ms_wd_data2(adrs)             (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[2])
#define ms_wd_data3(adrs)             (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[3])
#define ms_wd_data4(adrs)             (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[4])
#define ms_wd_data5(adrs)             (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[5])
#define ms_wd_data6(adrs)             (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[6])
#define ms_wd_data7(adrs)             (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[7])
#define ms_wd_data8(adrs)             (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[8])
#define ms_wd_data9(adrs)             (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[9])
#define ms_wd_data10(adrs)            (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[10])
#define ms_wd_data11(adrs)            (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[11])
#define ms_wd_data12(adrs)            (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[12])
#define ms_wd_data13(adrs)            (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[13])
#define ms_wd_data14(adrs)            (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[14])
#define ms_wd_data15(adrs)            (((volatile struct card_ms_regs *)(adrs+3*32*4))->ms_wd_data[15])

// CARD_CTL: DMA Controller
#define dma_data(adrs)                (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_data)
#define dma_srcdst(adrs)              (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_srcdst)
#define dma_size(adrs)                (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_size)
#define dma_hw_stop_rst(adrs)         (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_stop_rst)
#define dma_ctrl(adrs)                (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_ctrl)
#define dma_base_addrl(adrs)          (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_base_addr[0])
#define dma_base_addrh(adrs)          (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_base_addr[1])
#define dma_hw_en(adrs)               (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_en)
#define dma_hw_page_addrl(adrs)       (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_addr0[0])
#define dma_hw_page_addrh(adrs)       (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_addr0[1])
#define dma_hw_page_addrl1(adrs)      (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_addr1[0])
#define dma_hw_page_addrh1(adrs)      (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_addr1[1])
#define dma_hw_page_addrl2(adrs)      (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_addr2[0])
#define dma_hw_page_addrh2(adrs)      (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_addr2[1])
#define dma_hw_page_addrl3(adrs)      (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_addr3[0])
#define dma_hw_page_addrh3(adrs)      (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_addr3[1])
#define dma_hw_page_num0(adrs)        (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_num[0])
#define dma_hw_page_num1(adrs)        (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_num[1])
#define dma_hw_page_num2(adrs)        (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_num[2])
#define dma_hw_page_num3(adrs)        (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_num[3])
#define dma_hw_block_num(adrs)        (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_block_num)
#define dma_start(adrs)               (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_start)
#define dma_hw_page_cnt(adrs)         (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_page_cnt)
#define dma_cmp(adrs)                 (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_cmp)
#define dma_cmpen(adrs)               (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_int_en)
#define dma_hw_wait_numl(adrs)        (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_wait_num[0])
#define dma_hw_wait_numh(adrs)        (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_wait_num[1])
#define dma_hw_delay_num(adrs)        (((volatile struct card_dma_regs *)(adrs+4*32*4))->dma_hw_delay_num)


/******************************************************************************
*                     EMMC  MACRO Function Define
*******************************************************************************/
#define TO_EMMC_REG(adrs)				((volatile struct emmc_ctl_regs *)(adrs))

/* emmc */
#define dmasrc(adrs)					(TO_EMMC_REG(adrs)->dmasrc)
#define dmadst(adrs)					(TO_EMMC_REG(adrs)->dmadst)
#define sd_cmp(adrs) 					(TO_EMMC_REG(adrs)->sd_cmp)
#define sdio_int_en(adrs) 				(TO_EMMC_REG(adrs)->sdio_int_en)
#define hwdmacmpen(adrs) 				(TO_EMMC_REG(adrs)->hwdmacmpen)
#define rx4_en(adrs)					(TO_EMMC_REG(adrs)->rx4_en)
#define sdrsptmren(adrs)				(TO_EMMC_REG(adrs)->sdrsptmren)
#define sdcrctmren(adrs)				(TO_EMMC_REG(adrs)->sdcrctmren)
#define sddatawd(adrs)					(TO_EMMC_REG(adrs)->sddatawd)
#define sdfqsel(adrs)					(TO_EMMC_REG(adrs)->sdfqsel)
#define sdmmcmode(adrs)					(TO_EMMC_REG(adrs)->sdmmcmode)
#define sdrsptype(adrs)					(TO_EMMC_REG(adrs)->sdrsptype)

#define DMA_SRCDST_MASK				0x7
#define DMA_SRC(x)						dmasrc(gDEV_SDCTRL_BASE_ADRS)		= (x) & DMA_SRCDST_MASK
#define DMA_DST(x)						dmadst(gDEV_SDCTRL_BASE_ADRS)			= (x) & DMA_SRCDST_MASK
#define SDIO_INT_EN(x)					sdio_int_en(gDEV_SDCTRL_BASE_ADRS)		= !!(x)
#define SD_CMP_EN(x)					sd_cmp(gDEV_SDCTRL_BASE_ADRS)			= !!(x)
#define RX4_EN(x)						rx4_en(gDEV_SDCTRL_BASE_ADRS)			= !!(x)
#define SDRSPTMREN(x)					sdrsptmren(gDEV_SDCTRL_BASE_ADRS)		= !!(x)
#define SDCRCTMREN(x)					sdcrctmren(gDEV_SDCTRL_BASE_ADRS)		= !!(x)
#define SDDATAWD(x)						sddatawd(gDEV_SDCTRL_BASE_ADRS)			= !!(x)
#define MMC_FREQ_DIV_MASK				0xFFF
#define SDFQSEL(x)						sdfqsel(gDEV_SDCTRL_BASE_ADRS)			= (x) & MMC_FREQ_DIV_MASK
#define SDFQSEL_GET()					sdfqsel(gDEV_SDCTRL_BASE_ADRS)
#define SDMMCMODE(x)					sdmmcmode(gDEV_SDCTRL_BASE_ADRS)		= !!(x)
#define	SDRSPTYPE_R2(x)					sdrsptype(gDEV_SDCTRL_BASE_ADRS)		= !!(x)

/******************************************************************************
*                SD AND EMMC MACRO Function Define
*******************************************************************************/
#define IS_EMMC_SLOT()  (gDEV_SDCTRL_BASE_ADRS == (unsigned int)CARD0_CTL_REG)

#define SD_WAIT_RSP_TIME_SET(x) \
	do { \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdrsptmr) = (x) & SD_WAIT_RSP_TIME_MASK; \
		} \
		else { \
			sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) = sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) & 0xF800; \
			sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) = sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) | (x & 0x7FF); \
		} \
	} while (0)
#define SD_HIGHSPEED_EN_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_high_speed_en) = !!(x); \
		} \
		else { \
			sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) = sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) & 0xF7FF; \
			sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) = sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) | ((x & 0x1)<<11); \
		} \
	}while(0)

#define SD_DELAY_MASK					0x7
#define SD_WT_CLK_DELAY_TIME_SET(x)	\
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_wr_dly_sel) = (x) & SD_DELAY_MASK; \
		} \
		else { \
			sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) = sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) & 0x0FFF; \
			sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) = sd_timing_config0(gDEV_SDCTRL_BASE_ADRS) | ((x & 0x7)<<12); \
		} \
	}while(0)


#define SD_RD_CLK_DELAY_TIME_SET(x)	\
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rd_dly_sel) = (x) & SD_DELAY_MASK; \
		} \
		else { \
			sd_timing_config1(gDEV_SDCTRL_BASE_ADRS) = sd_timing_config1(gDEV_SDCTRL_BASE_ADRS) & 0x1FFF; \
			sd_timing_config1(gDEV_SDCTRL_BASE_ADRS) = sd_timing_config1(gDEV_SDCTRL_BASE_ADRS) | (((x) & 0x7)<<13); \
		} \
	}while(0)

#define SD_RD_CLK_DELAY_TIME_GET()	\
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rd_dly_sel); \
		} \
		else { \
			ret = ((sd_timing_config1(gDEV_SDCTRL_BASE_ADRS) >> 13) & 7); \
		} \
		(ret);\
	})

#define SD_WAIT_RSP_TIME_MASK			0x7FF
#define SD_WAIT_CRC_TIME_MASK			0x7FF
#define SD_WAIT_CRC_TIME_SET(x)	\
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdcrctmr) 	= (x) & SD_WAIT_CRC_TIME_MASK; \
		} \
		else { \
			sd_timing_config1(gDEV_SDCTRL_BASE_ADRS) = sd_timing_config1(gDEV_SDCTRL_BASE_ADRS) & 0xF800; \
			sd_timing_config1(gDEV_SDCTRL_BASE_ADRS) = sd_timing_config1(gDEV_SDCTRL_BASE_ADRS) | ((x) & 0x7FF); \
		} \
	}while(0)

#define SD_BLOCK_SIZE_MASK				0x7FF

#define SD_BLOCK_SIZE_SET(x) \
	 do { \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_blocksize) 	= (x) & SD_BLOCK_SIZE_MASK; \
		} \
		else { \
			sd_blocksize(gDEV_SDCTRL_BASE_ADRS) = ((x) & SD_BLOCK_SIZE_MASK); \
		} \
	} while (0)

#define SD_BLOCK_SIZE_GET() \
	 ({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_blocksize); \
		} \
		else { \
			ret = sd_blocksize(gDEV_SDCTRL_BASE_ADRS) & SD_BLOCK_SIZE_MASK; \
		} \
		(ret); \
	})

#define SD_TRANS_SDRSPCHK_EN(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdrspchk_en) = !!(x); \
		} \
		else { \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) & 0x0FF; \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) | (((x) & 0x1) << 8); \
		} \
	}while(0)

#define SD_TRANS_SDDUMMY(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdcmddummy) = !!(x); \
		} \
		else { \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) & 0x17F; \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) | (((x) & 0x1) << 7); \
		} \
	}while(0)

#define SD_TRANS_SDAUTORSP(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdautorsp) = !!(x); \
		} \
		else { \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) & 0x1BF; \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) | (((x) & 0x1) << 6); \
		} \
	}while(0)

#define SD_TRANS_MODE_MASK				0x3
#define SD_TRANS_MODE_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_trans_mode) = (x) & SD_TRANS_MODE_MASK; \
		} \
		else { \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) & 0x1CF; \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) | (((x) & 0x3)<<4); \
		} \
	}while(0)

#define SD_TRANS_SDLENMODE(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_len_mode) = !!(x); \
		} \
		else { \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) & 0x1FB; \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) | (((x) & 0x1) << 2); \
		} \
	}while(0)

#define SD_TRANS_SDDDRMODE(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdddrmode) = !!(x); \
		} \
		else { \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) & 0x1FD; \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) | (((x) & 0x1) << 1); \
		} \
	}while(0)

#define SD_TRANS_SDPIOMODE(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sdpiomode) = !!(x); \
		} \
		else { \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) & 0x1FE; \
			sd_config0(gDEV_SDCTRL_BASE_ADRS) = sd_config0(gDEV_SDCTRL_BASE_ADRS) | ((x) & 0x1); \
		} \
	}while(0)

#define SD_CMD_BUF0_GET()      \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[3]) & 0x3f; \
		} \
		else { \
			ret = (sd_cmdbuf0(gDEV_SDCTRL_BASE_ADRS) & 0x3f); \
		} \
		(ret); \
	})

#define SD_CMD_BUF0_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[3]) = (x); \
		} \
		else { \
			sd_cmdbuf0(gDEV_SDCTRL_BASE_ADRS) = (x); \
		} \
	}while(0)
#define SD_CMD_BUF1_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[2]) = (x); \
		} \
		else { \
			sd_cmdbuf1(gDEV_SDCTRL_BASE_ADRS) = (x); \
		} \
	}while(0)
#define SD_CMD_BUF2_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[1]) = (x); \
		} \
		else { \
			sd_cmdbuf2(gDEV_SDCTRL_BASE_ADRS) = (x); \
		} \
	}while(0)
#define SD_CMD_BUF3_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[0]) = (x); \
		} \
		else { \
			sd_cmdbuf3(gDEV_SDCTRL_BASE_ADRS) = (x); \
		} \
	}while(0)
#define SD_CMD_BUF4_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_cmdbuf[4]) = (x); \
		} \
		else { \
			sd_cmdbuf4(gDEV_SDCTRL_BASE_ADRS) = (x); \
		} \
	}while(0)


#define SD_PAGE_NUM_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_page_num) = (x); \
		} \
		else { \
			sd_page_num(gDEV_SDCTRL_BASE_ADRS) = (x); \
		} \
	}while(0)


#define SD_RSP_BUF0_3_GET \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rspbuf0_3); \
		} \
		else { \
			ret = sd_rspbuf0_3(gDEV_SDCTRL_BASE_ADRS); \
		} \
		(ret);\
	})
#define SD_RSP_BUF4_5_GET \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rspbuf4_5); \
		} \
		else { \
			ret = sd_rspbuf4_5(gDEV_SDCTRL_BASE_ADRS); \
		} \
		(ret);\
	})
#define SD_HW_STATE_GET() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_hw_state); \
		} \
		else { \
			ret = sd_hw_state(gDEV_SDCTRL_BASE_ADRS); \
		} \
		(ret);\
	})


#define SD_HW_CMD13_RCA_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_hw_cmd13_rca) = (x); \
		} \
		else { \
			sd_hw_cmd13_rca(gDEV_SDCTRL_BASE_ADRS) = (x); \
		} \
	}while(0)

#define SD_RSP_TIMEOUT_ERROR() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = ((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 6) & 0x1; \
		} \
		else { \
			ret = ((sd_status0(gDEV_SDCTRL_BASE_ADRS) >> 6) & 0x01); \
		} \
		(ret);\
	})

#define SD_DATA_CRC_TIMEOUT_ERROR() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 7) & 0x1); \
		} \
		else { \
			ret = ((sd_status0(gDEV_SDCTRL_BASE_ADRS) >> 7) & 0x01); \
		} \
		(ret);\
	})

#define SD_DATA_STB_TIMEOUT_ERROR() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = ((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 8) & 0x1; \
		} \
		else { \
			ret = ((sd_status0(gDEV_SDCTRL_BASE_ADRS) >> 8) & 0x01); \
		} \
		(ret);\
	})

#define SD_DATA_CRC7_ERROR() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = ((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 9) & 0x1; \
		} \
		else { \
			ret = ((sd_status0(gDEV_SDCTRL_BASE_ADRS) >> 9) & 0x01); \
		} \
		(ret);\
	})

#define SD_DATA_CRC_ERROR() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = ((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 11) & 0x1; \
		} \
		else { \
			ret = ((sd_status0(gDEV_SDCTRL_BASE_ADRS) >> 11) & 0x01); \
		} \
		(ret);\
	})


#define CARD_MEDIA_TYPE_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->mediatype) = (x); \
		} \
		else { \
			card_mediatype(gDEV_SDCTRL_BASE_ADRS) = (x); \
		} \
	}while(0)

#define hwSdCardCrcState() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = ((((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_state) & 0x70) >> 4) ==  0x02); \
		} \
		else { \
			ret = ((((sd_status1(gDEV_SDCTRL_BASE_ADRS)) & 0x70) >> 4) == 0x02); \
		} \
		(ret);\
	})

#define hwSdDat0Check() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) & 0x20) >> 5); \
		} \
		else { \
			ret = (((sd_status0(gDEV_SDCTRL_BASE_ADRS)) & 0x20) >> 5); \
		} \
		(ret);\
	})

#define hwSdTimeOut() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (((TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status) >> 7) & 0x01); \
		} \
		else { \
			ret = (((sd_status0(gDEV_SDCTRL_BASE_ADRS)) >> 7) & 0x01); \
		} \
		(ret);\
	})


#define SD_CONFIG_SET(x)         sd_config(gDEV_SDCTRL_BASE_ADRS) = (x)
#define SD_CONFIG_GET() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_config1); \
		} \
		else { \
			ret = sd_config(gDEV_SDCTRL_BASE_ADRS); \
		} \
		(ret);\
	})

#define SD_CONFIG0_GET() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_config0); \
		} \
		else { \
			ret = sd_config0(gDEV_SDCTRL_BASE_ADRS); \
		} \
		(ret);\
	})


#define SD_RST() \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rst) = 0x03; \
		} \
		else { \
            sd_rst(gDEV_SDCTRL_BASE_ADRS) = 0x03; \
		} \
	}while(0)

#define SD_CTRL_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_ctrl) = (x); \
		} \
		else { \
			sd_ctrl(gDEV_SDCTRL_BASE_ADRS) = (x); \
		} \
	}while(0)

#define SD_STATUS0_GET() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_status); \
		} \
		else { \
			ret = sd_status0(gDEV_SDCTRL_BASE_ADRS); \
		} \
		(ret);\
	})



#define SD_STATUS1_GET() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_state); \
		} \
		else { \
			ret = sd_status1(gDEV_SDCTRL_BASE_ADRS); \
		} \
		(ret);\
	})

#define SD_STATUS_NEW_ERR()     ((SD_STATUS1_GET()>>13)&0x01)

#define SD_PIO_RX_DATA_GET() \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_piodatarx); \
		} \
		else { \
			ret = sd_piodatarx(gDEV_SDCTRL_BASE_ADRS); \
		} \
		(ret);\
	})


#define DMA_FROM_DEVICE					0
#define DMA_TO_DEVICE					1

#define Q628_MEMORY						1
#define MMC_FLASH						2

#define DMA_SRCDST_SET(x) \
	do { \
		if (IS_EMMC_SLOT()) { \
			if ((x) ==  DMA_TO_DEVICE) { \
				DMA_SRC(Q628_MEMORY); \
				DMA_DST(MMC_FLASH); \
			} else { \
				DMA_SRC(MMC_FLASH); \
				DMA_DST(Q628_MEMORY); \
			} \
		} \
		else { \
			dma_srcdst(gDEV_SDCTRL_BASE_ADRS) = ((x) & 0x3ff); \
		} \
	} while (0)

/* only sd card need this */
#define DMA_SIZE_SET(x)					dma_size(gDEV_SDCTRL_BASE_ADRS) = (x)
#define SD_INT_CONTROL_SET(x)           sd_int(gDEV_SDCTRL_BASE_ADRS) = (x)
#define SD_INT_CONTROL_GET()            sd_int(gDEV_SDCTRL_BASE_ADRS)


#define SET_HW_DMA_BASE_ADDR(DRAMAddr) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->dma_base_addr) = (UINT32)(DRAMAddr); \
		} \
		else { \
			dma_base_addrl(gDEV_SDCTRL_BASE_ADRS) = (UINT32) DRAMAddr & 0xffff; \
			dma_base_addrh(gDEV_SDCTRL_BASE_ADRS) = ((UINT32)DRAMAddr >> 16) & 0xffff; \
		} \
	}while(0)

#define DMA_HW_EN(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->hwdmacmpen) = !!(x); \
		} \
		else { \
			dma_hw_en(gDEV_SDCTRL_BASE_ADRS) =  (x); \
		} \
	}while(0)


#define SD_TXDUMMY_MASK					0x1FF
#define SD_TXDUMMY_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->tx_dummy_num) = (x); \
		} \
		else { \
			sd_tx_dummy_num(gDEV_SDCTRL_BASE_ADRS) = (x); \
		} \
	}while(0)


#define SD_RXDATTMR_GET(x) \
	({ \
		unsigned int ret; \
		if (IS_EMMC_SLOT()) { \
			ret = (TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rxdattmr); \
		} \
		else { \
			ret = sd_clk_dly(gDEV_SDCTRL_BASE_ADRS); \
		} \
		(ret);\
	})

#define SD_RXDATTMR_MASK				0x1FFFFFFF
#define SD_RXDATTMR_SET(x) \
	do{ \
		if (IS_EMMC_SLOT()) { \
			(TO_EMMC_REG(gDEV_SDCTRL_BASE_ADRS)->sd_rxdattmr) = (x) & SD_RXDATTMR_MASK; \
		} \
		else { \
			sd_clk_dly(gDEV_SDCTRL_BASE_ADRS) = sd_clk_dly(gDEV_SDCTRL_BASE_ADRS) &(~0x3); \
			sd_clk_dly(gDEV_SDCTRL_BASE_ADRS) = sd_clk_dly(gDEV_SDCTRL_BASE_ADRS) | (0x3 & x); \
		} \
	}while(0)


#endif //#ifndef _HWSD_H_
