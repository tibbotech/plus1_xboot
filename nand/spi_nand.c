#include <nand_boot/nfdriver.h>
#include <nand_boot/nandop.h>
#include <nand_boot/hal_nand_error.h>
#include <spinand_boot/sp_spinand.h>
#include <regmap.h>
#include <common.h>
#include <bootmain.h>


/**************************************************************************
 *                             M A C R O S                                *
 **************************************************************************/
#define writel(value,reg)           Xil_Out32((unsigned int)reg, value)
#define readl(reg)                  Xil_In32((unsigned int)reg)

#define CFG45_DefaultValue   0x15
#define CFG67_DefaultValue   0x95

#define USE_DESCRIPTOR_MODE 0
#define DEVICE_STS_AUTO_CHK 0
#define USE_SP_BCH          1   // 1:Using BCH, 0:using Device internal ECC


/**************************************************************************
 *                         D A T A   T Y P E S                            *
 **************************************************************************/

/**************************************************************************
 *                        G L O B A L   D A T A                           *
 **************************************************************************/


/**************************************************************************
 *                 E X T E R N A L   R E F E R E N C E S                  *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N   D E C L A R A T I O N S                *
 **************************************************************************/
extern UINT32 Load_Header_Profile(SINT32 type);
extern void Xil_Out32(unsigned int OutAddress, unsigned int Value);
extern unsigned int Xil_In32(unsigned int Addr);
extern void setSystemPara(UINT8 *prData);
extern UINT32 nfvalshift(UINT32 x);
extern UINT8 GetNANDPageCount_1K60(UINT32 pageSize);
extern int verify_xboot_img(u8 *img);

/*********************Basic function********************/

/*******************************************************/
/* check SPI NAND Device status by ctrl_status_registers, */
void wait_spi_idle(struct sp_spinand_info *info)
{
	struct sp_spinand_regs *regs = info->regs;

	while (readl(&regs->spi_ctrl) & SPI_DEVICE_IDLE) {	/* --> ctrl bit-31, wait spi_ctrl idle */
		/* wait */;
	}
}

int spi_nand_getfeatures(struct sp_spinand_info *info,uint32_t addr)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;
	
	CSTAMP(0x5910FE00);
	CSTAMP(addr);

	value = (SPI_NAND_CHIP_A)|(SPI_NAND_AUTO_WEL)|(SPI_NAND_CLK_32DIV)|(SPINAND_CMD_GETFEATURES<<8)|(SPI_NAND_CTRL_EN)|(SPINAND_CUSTCMD_1_DATA)|(SPINAND_CUSTCMD_1_ADDR);
	writel(value ,&regs->spi_ctrl);

	writel(addr ,&regs->spi_page_addr);

	value = SPINAND_CFG01_DEFAULT;
	writel(value ,&regs->spi_cfg[1]);

	value = SPINAND_AUTOCFG_CMDEN;
	writel(value ,&regs->spi_auto_cfg);

	wait_spi_idle(info);

	CSTAMP(0x5910FE01);

	return (readl(&regs->spi_data) & 0xFF);

}

void spi_nand_setfeatures(struct sp_spinand_info *info,uint32_t addr, uint32_t data)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;

	CSTAMP(0x5910FE10);
	CSTAMP(addr);

	value = (SPI_NAND_CHIP_A)|(SPI_NAND_AUTO_WEL)|(SPI_NAND_CLK_32DIV)|(SPINAND_CMD_SETFEATURES<<8)|(SPI_NAND_CTRL_EN)|(SPINAND_CUSTCMD_1_DATA)|(SPI_NAND_WRITE_MDOE)|(SPINAND_CUSTCMD_1_ADDR);
	writel(value ,&regs->spi_ctrl);

	writel(addr ,&regs->spi_page_addr);

	writel(data ,&regs->spi_data);

	value = SPINAND_CFG01_DEFAULT1;
	writel(value ,&regs->spi_cfg[1]);

	value = SPINAND_AUTOCFG_CMDEN;
	writel(value ,&regs->spi_auto_cfg);

	wait_spi_idle(info);

	CSTAMP(0x5910FE11);
}


static int sp_spinand_reset(struct sp_spinand_info *info)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;
	int ret = -1;


	CSTAMP(0x59105201);

	//initial
	while (readl(&regs->spi_ctrl) & SPI_DEVICE_IDLE) {
		/* wait */ ;
	}

	CSTAMP(0x59105202);

	/* ==== Flash reset ==== */
	value = (SPI_NAND_CHIP_A)|(SPI_NAND_CLK_32DIV)|(SPINAND_CMD_RESET<<8)|(SPI_NAND_CTRL_EN)|(SPI_NAND_WRITE_MDOE);
	writel(value, &regs->spi_ctrl);

	value = SPINAND_CFG01_DEFAULT3;
	writel(value, &regs->spi_cfg[1]);	

	value = SPINAND_AUTOCFG_CMDEN;
	writel(value, &regs->spi_auto_cfg);

	wait_spi_idle(info);
	DelayUS(200); /* wait 200us */

	CSTAMP(0x59105203);

#if DEVICE_STS_AUTO_CHK
	//writel(0x0583, &regs->cust_cmd);  // spi nand not include 0x05 cmd
	wait_spi_idle(info);
	ret = spi_nand_getfeatures(info, 0xc0);
#else
	STC_REG->stc_15_0 = 0;
	do {
		ret = spi_nand_getfeatures(info, 0xc0);

		if (STC_REG->stc_15_0 > (90 * 10)) {	/* 10ms timeout */
			ret = -1;
			break;
		}
	} while (ret & 0x01);
#endif

	CSTAMP(0x59105204);

	return ret;
}

UINT16 spi_nand_readid(struct sp_spinand_info *info, uint32_t addr, uint8_t *data)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;
	CSTAMP(0x59105501);

	writel(addr, &regs->spi_page_addr);
	/*read 3 byte cycle same to 8388 */
	value = SPI_NAND_CHIP_A|SPI_NAND_AUTO_WEL|(SPI_NAND_CLK_32DIV)|(SPINAND_CMD_READID<<8)|SPI_NAND_CTRL_EN|(SPINAND_CUSTCMD_3_DATA)|(SPINAND_CUSTCMD_1_ADDR);
	writel(value, &regs->spi_ctrl);

	value = SPINAND_CFG01_DEFAULT;
	writel(value ,&regs->spi_cfg[1]);

	value = SPINAND_AUTOCFG_CMDEN;
	writel(value ,&regs->spi_auto_cfg);

	wait_spi_idle(info);

	CSTAMP(0x59105502);

	value = readl(&regs->spi_data);

	data[0] = (uint8_t)(value & 0xFF);
	data[1] = (uint8_t)((value >> 8)& 0xFF);
	data[2] = (uint8_t)((value >> 16)& 0xFF);
	data[3] = (uint8_t)((value >> 24)& 0xFF);
	data[4] = 0;

	CSTAMP(value);

	return 4;
}

int initSPIDriver(void)
{
	CSTAMP(0x59100001);

	struct sp_spinand_info *info = &g_bootinfo.our_spinfc;
	info->regs = (struct sp_spinand_regs * )CONFIG_SP_SPINAND_BASE;

	dbg_info();

	/* NAND CTRL soft reset */
	if (sp_spinand_reset(info) < 0 ) {
		CSTAMP(0x591000EE);
		prn_string("no spi nand\n");
		return ROM_FAIL;
	}

	CSTAMP(0x59100002);

	/* ==== Read ID ==== */
	g_bootinfo.gNANDIDLength = spi_nand_readid(info, 0, g_pyldData);

	prn_string("SPI ID=");
	prn_byte(g_pyldData[0]);
	prn_byte(g_pyldData[1]);
	prn_byte(g_pyldData[2]);
	prn_byte(g_pyldData[3]);
	prn_string("\n");

	if ((g_pyldData[0] == 0xFF) && (g_pyldData[1] == 0xFF)) {
		CSTAMP(0x591000FF);
		prn_string("no spi id\n");
		return ROM_FAIL;
	}

	if (g_pyldData[0] == 0xEF) {
		CSTAMP(0x59100003);

		info->id = g_pyldData[0];
		prn_string("WSPI NAND found\n");
#if USE_SP_BCH
		prn_string("Set BUF-1..& ECC-OFF,using BCH\n");
		spi_nand_setfeatures(info, 0xb0, 0x08); // Dis-able ECC, enable BUF-1
#else
		prn_string("Set WB in BUF-1..& ECC-ON");
		spi_nand_setfeatures(info, 0xb0, 0x18); // enable ECC & BUF-1
#endif
	} else if ((g_pyldData[0] == 0xC8) && (g_pyldData[1] != 0x21)) {
		CSTAMP(0x59100004);

		info->id = g_pyldData[0];
		prn_string("GSPI NAND found & enable QuadIO mode,");
#if USE_SP_BCH
		prn_string("using BCH\n");
		spi_nand_setfeatures(info, 0xb0, 0x01);  /* Dis-able ECC & ebable QuadIO */
#else
		prn_string("using device internal ECC\n");
		spi_nand_setfeatures(info, 0xb0, 0x11);  /* enable ECC & QuadIO */
#endif
	} else if (g_pyldData[0] != 0) {
		CSTAMP(0x59100005);

		if(g_pyldData[0] == 0xC2)	/* MXIC */
			info->id = g_pyldData[0];
		else if(g_pyldData[0] == 0x2C) /* MICRON */
			info->id = g_pyldData[0];
		else
			info->id = 0xFF;	/* ESMT,tmp */
		prn_string("SPI NAND found...");
#if USE_SP_BCH
		prn_string("using BCH\n");
		spi_nand_setfeatures(info, 0xB0, 0x01);	/* Dis-able ECC & ebable QuadIO */
#else
		prn_string("using device internal ECC\n");
		spi_nand_setfeatures(info, 0xB0, 0x11);	/* enable ECC & QuadIO */
#endif
	} else {
		CSTAMP(0x59100006);

		prn_string("Unknow SPI NAND device\n");
		return ROM_FAIL;
	}

	CSTAMP(0x59100007);

	//rd_memcpy((UINT32 *)g_pSysInfo->IdBuf, (UINT32 *)g_pyldData, MAX_ID_LEN);
	rd_memcpy((UINT32 *)g_pSysInfo->IdBuf, (UINT32 *)g_pyldData, ((g_bootinfo.gNANDIDLength + 3) >> 2) << 2);

#ifdef PARTIAL_NAND_INIT
    return ROM_SUCCESS; /* xboot */
#else
	CSTAMP(0x59100009);

	setSystemPara(g_pSysInfo->IdBuf);

	CSTAMP(0x5910000A);

	/* give a initial value to read header */
	g_pSysInfo->u16PyldLen = 2048;		/* 2K Page */
	g_pSysInfo->u16ReduntLen = 64;
	g_pSysInfo->u16PageNoPerBlk = 64;	/* 64 pages per block */
	g_pSysInfo->u8PagePerBlkShift = nfvalshift(64);
	g_pSysInfo->u8addrCycle = 5;		/* 3 row + 2 col */

	CSTAMP(0x5910000B);

	/* before load header */
	if (Load_Header_Profile(SPINAND_BOOT) == ROM_SUCCESS) {

		CSTAMP(0x5910000C);

		setSystemPara(g_pSysInfo->IdBuf);

		return ROM_SUCCESS;
	}

	CSTAMP(0x5910000D);

	/* failed to load header */
	return ROM_FAIL;
#endif
}


void spi_nand_pageread2cache(struct sp_spinand_info *info, uint32_t addr)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;
#if 0
	/* RGST bus */
	writel(0x05, &regs->cfg4);
	writel(0x05, &regs->cfg6);
	info->row = addr;

	addr &= 0xFFFFFF;	/* 3 byte addr cnt */
	writel(addr, &regs->addr_low);
	writel(addr >> 16, &regs->addr_high);

	writel(0x1383, &regs->cust_cmd);  /* 3 byte addr count -> 1Gbit:1 dummy byte address + 2 byte(page/block) address */
	wait_spi_idle(info);

#if DEVICE_STS_AUTO_CHK
	writel(0x0583, &regs->cust_cmd);
	wait_spi_idle(info);
#else
	int ret;
	STC_REG->stc_15_0 = 0;
	do {
		ret = spi_nand_getfeatures(info, 0xc0);
		if (STC_REG->stc_15_0 > (90 * 100)) {	/* 100ms timeout */
			dbg();
                        break;
                }
	} while (ret & 0x01);
#endif
#else
#if USE_SP_BCH
	spi_nand_setfeatures(info, 0xB0, 0x00); /* en-able QuadIO,ECC-off */
#else
	spi_nand_setfeatures(info, 0xB0, 0x10); /* en-able QuadIO,ECC-on */
#endif

	info->row = addr;

	value = (SPI_NAND_CHIP_A)|(SPI_NAND_CLK_32DIV)|(SPINAND_CMD_PAGE2CACHE<<8)|(SPI_NAND_CTRL_EN)|(SPINAND_CUSTCMD_3_ADDR);
	writel(value, &regs->spi_ctrl);

	value = (1<<23)|(1<<19);
	writel(value ,&regs->spi_cfg[0]);

	value = SPINAND_CFG01_DEFAULT2;
	writel(value ,&regs->spi_cfg[1]);

	writel(addr, &regs->spi_page_addr);

	value = (SPINAND_CMD_PAGE2CACHE<<24)|(SPINAND_AUTOCFG_CMDEN)|(SPINAND_AUTOCFG_RDCACHE)|(SPINAND_AUTOCFG_RDSTATUS);
	writel(value ,&regs->spi_auto_cfg);

	wait_spi_idle(info);
#endif

}

void spi_nand_readcacheQuadIO_byMapping(struct sp_spinand_info *info, uint32_t addr, unsigned int size, uint32_t *pbuf)
{
	struct sp_spinand_regs *regs = info->regs;
	int value = 0;
	int i;
	struct BootProfileHeader *ptr = (struct BootProfileHeader *)g_boothead;
	int pagemark = 0;
	int colmark = 0;

	if((ptr->PlaneSelectMode & 0x1)== 0x1)
	{
		pagemark = (ptr->PlaneSelectMode>>2)&0xfff;
		colmark = (ptr->PlaneSelectMode>>16)&0xffff;		
	}
	
#if 0
	unsigned int cfg1,cfg5,cfg7;

	if (info->id == 0xC8) {	/* GD */
#if USE_SP_BCH
		spi_nand_setfeatures(info, 0xB0, 0x01);	/* en-able QuadIO,ECC-off */
#else
		spi_nand_setfeatures(info, 0xB0, 0x11);	/* en-able QuadIO,ECC-on */
#endif
		/* GD SPI QuadIO: 2 dummy address cycle, 4 bit data, 4 bit address, 1 bit cmd */
		cfg5 = 0x013D;
		cfg7 = 0x01;
		cfg1 = 0xEB10;
	} else if (info->id == 0xEF) {	/* WB */
#if USE_SP_BCH
		spi_nand_setfeatures(info, 0xB0, 0x08); /* ECC-off ,BUF-1 */
#else
		spi_nand_setfeatures(info, 0xB0, 0x18); /* ECC on & BUF-1 mode on */
#endif
		/* Winbond SPI QuadIO: 4 dummy address cycle, 4 bit data, 4 bit address, 1 bit cmd */
		cfg5 = 0x023D;
		cfg7 = 0x01;
		cfg1 = 0xEB10;
	} else if (info->id == 0xC2) { /* MXIC */
#if USE_SP_BCH
		spi_nand_setfeatures(info, 0xB0, 0x01);	/* en-able QuadIO,ECC-off */
#else
		spi_nand_setfeatures(info, 0xB0, 0x11);	/* en-able QuadIO,ECC-on */
#endif
		cfg5 = 0x0435;
		cfg7 = 0x05;
		cfg1 = 0x6B10;
	} else { /* other SPI using Cachex4, 4-bit data, 1 bit addr & cmd */
		spi_nand_setfeatures(info, 0xB0, 0x01); /* en-able QuadIO,ECC-off */
		cfg5 = 0x0435;
		cfg7 = 0x05;
		cfg1 = 0x6B10;
	}

	writel(cfg5, &regs->cfg5);
	writel(cfg7, &regs->cfg7);
	writel(cfg1, &regs->cfg1);
#else
	value = SPINAND_CFG02_DEFAULT;
	writel(value, &regs->spi_cfg[2]);

	value = (SPINAND_CMD_PAGEREAD <<24)|(SPINAND_AUTOCFG_RDCACHE)|(SPINAND_AUTOCFG_RDSTATUS);
	writel(value, &regs->spi_auto_cfg);
	
	value = readl(&regs->spi_auto_cfg);

	STC_REG->stc_15_0 = 0;
	while((value>>24)!= 0x3)
	{
		if (STC_REG->stc_15_0 > (90 * 10)) {	/* 10ms timeout */
			break;
		}	
	}
#endif
	if((ptr->PlaneSelectMode & 0x1)== 0){
		if ((info->row & (0x40)) && (((info->id & 0xFF) == 0xC2)||((info->id & 0xFF) == 0x2C))) {
			for (i = addr ; i < (addr + size) ; i += 4)
				*(unsigned int *)pbuf++ = *(unsigned int *)(0x94001000 + i);
		} else {
			for (i = addr ; i < (addr + size) ; i += 4)
				*(unsigned int *)pbuf++ = *(unsigned int *)(0x94000000 + i);
		}
	}
	else{
		if ((info->row & pagemark) && ((ptr->PlaneSelectMode&0x2)==0x2)){
			for (i = addr ; i < (addr + size) ; i += 4)
				*(unsigned int *)pbuf++ = *(unsigned int *)(0x94000000 + i +colmark);
		}else{
			for (i = addr ; i < (addr + size) ; i += 4)
				*(unsigned int *)pbuf++ = *(unsigned int *)(0x94000000 + i);
		}
	}

	wait_spi_idle(info);
}

int SPINANDReadPage(UINT8 which_cs, UINT32 u32PhyAddr,UINT32 * PyldBuffer,UINT32 * DataBuffer,UINT32 u8RWMode)
{
	struct sp_spinand_info *info = &g_bootinfo.our_spinfc;

	info->row = (u32PhyAddr & 0x01FFFF);
	spi_nand_pageread2cache(info, u32PhyAddr);
	spi_nand_readcacheQuadIO_byMapping(info, 0, g_pSysInfo->u16PyldLen, PyldBuffer);
	spi_nand_readcacheQuadIO_byMapping(info, g_pSysInfo->u16PyldLen, g_pSysInfo->u16ReduntLen, DataBuffer);

	//prn_string(" iboot_TEST");  /*  prn_decimal(i); */  prn_string("\n");
	return ROM_SUCCESS;
}

void initSPINandFunptr(void)
{
	SDev_t* pSDev = getSDev();

	pSDev->IsSupportBCH = 1;
	pSDev->DeviceID = DEVICE_SPI_NAND;

	pSDev->predInitDriver    = (predInitDriver_t)initSPIDriver;
	pSDev->predReadWritePage = (predReadWritePage_t)SPINANDReadPage;
}

SINT32 ReadSPINANDSector_1K60(UINT32 * ptrPyldData, UINT32 pageNo)
{
	int ret;

	SDev_t* pSDev = getSDev();

	g_pSysInfo->ecc_mode = BCH_S338_1K60_BITS_MODE;

	pSDev->predReadWritePage(0, pageNo, (UINT32 *)ptrPyldData, (UINT32 *)g_spareData, 0);

	if (BCHProcess(ptrPyldData, (UINT32 *)(((UINT32)ptrPyldData) + 1024), 1024, BCH_DECODE, g_pSysInfo->ecc_mode) == ret_BCH_S338_FAIL) {
		ret = ROM_FAIL;
	} else {
		ret = ROM_SUCCESS;
	}
	
	return ret;
}

SINT32 SPINANDReadNANDPage_1K60(UINT8 which_cs, UINT32 pageNo, UINT32 * ptrPyldData, UINT32 *read_bytes)
{
	SINT32 ret;
	SDev_t* pSDev = getSDev();

	*read_bytes = 0;

	ret = pSDev->predReadWritePage(which_cs, pageNo, (UINT32 *)ptrPyldData, (UINT32 *)g_spareData, 2);
	if (ret != ROM_SUCCESS)
		return ret;

	ret = BCHProcess((UINT32 *)ptrPyldData, (UINT32 *)&ptrPyldData[GetNANDPageCount_1K60(g_pSysInfo->u16PyldLen) * 1024 / 4],
			GetNANDPageCount_1K60(g_pSysInfo->u16PyldLen) * 1024, BCH_DECODE, BCH_S338_1K60_BITS_MODE);
	if (ret != ROM_SUCCESS)
		return ret;

	*read_bytes = GetNANDPageCount_1K60(g_pSysInfo->u16PyldLen) * 1024;

	return ret;
}

#ifndef XBOOT_BUILD
SINT32 SPINANDReadBootBlock(UINT32 *target_address)
{
	struct BootProfileHeader *ptr = Get_Header_Profile_Ptr();
	u32 i, j, read_bytes = 0, pg_off = 0, skip_blk;
	u8 *buf;
	int res;
	int sz_sect = GetNANDPageCount_1K60(g_pSysInfo->u16PyldLen) * 1024;
	int xbsize;

	//FIXME: skip bad block by checking mark in OOB instead of looping

	for (skip_blk = 0; skip_blk < 5; skip_blk++) {

		for (i = 0 ; i < ptr->xboot_copies ; i++) {

			prn_string("load xboot skip="); prn_decimal(skip_blk);
			prn_string(" copy="); prn_decimal(i); prn_string("\n");

			buf = (u8 *)target_address;
			xbsize = 0;

			/* To get one copy of xboot */
			for (j = 0 ; j < ptr->xboot_pg_cnt ; j++) {
				pg_off = (skip_blk * ptr->PagePerBlock) +
					ptr->xboot_pg_off + (i * ptr->xboot_pg_cnt) + j;

				//prn_string("pg="); prn_decimal(pg_off);

				/* Load bblk data from a page */
				res = SPINANDReadNANDPage_1K60(NAND_CS0, pg_off, (u32 *)buf, &read_bytes);
				if (res != ROM_SUCCESS) {
					prn_string("failed at pg="); prn_dword(pg_off);
					break;
				}

				/* If first page, parse xboot size */
				if (j == 0) {
					xbsize = get_xboot_size(buf);
					if (xbsize == 0) {
						dbg();
						break;	/* try next copy */
					}

					/* fixup real xboot page count */
					//ptr->xboot_pg_cnt = (xbsize + sz_sect - 1) / sz_sect; // CPU PC jumps!! div has bug?
					ptr->xboot_pg_cnt = (xbsize + sz_sect - 1);
					ptr->xboot_pg_cnt /= sz_sect;
					prn_string("cnt="); prn_decimal(ptr->xboot_pg_cnt); prn_string("\n");
				}

				//prn_string("data="); prn_dword(*(u32 *)buf);

				buf += read_bytes;
			}

			//dbg();
			/* verify img */
			if (xbsize && j == ptr->xboot_pg_cnt) {
				//dbg();
				if (verify_xboot_img((u8 *)target_address) == ROM_SUCCESS) {
					g_bootinfo.app_blk_start = 1 + (pg_off / ptr->PagePerBlock);
					prn_string("app_blk="); prn_dword(g_bootinfo.app_blk_start);
					return ROM_SUCCESS;
				}
			}

			// probably ecc error or no xboot data
			// try next copy...
			dbg();
		}
	}

	dbg();
	return ROM_FAIL;
}
#endif /* CONFIG_HAVE_PARA_NAND */
