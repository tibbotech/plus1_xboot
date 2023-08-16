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
/* to reduce code size, just open the one you need. */
//#define CFG_SUPPORT_PIO_MODE
//#define CFG_SUPPORT_PIO_AUTO_MODE
#define CFG_SUPPORT_DMA_MODE
//#define CFG_SUPPORT_DMA_AUTOBCH_MODE

//#define SUPPORT_SPINAND_TEST

#define writel(value,reg)           Xil_Out32((unsigned int)reg, value)
#define readl(reg)                  Xil_In32((unsigned int)reg)

/**************************************************************************
 *                         D A T A   T Y P E S                            *
 **************************************************************************/

/**************************************************************************
 *                        G L O B A L   D A T A                           *
 **************************************************************************/
static const char *reg_name[] = {
	"spi_ctrl",
	"spi_timing",
	"spi_page_addr",
	"spi_data",
	"spi_status",
	"spi_auto_cfg",
	"spi_cfg0",
	"spi_cfg1",
	"spi_cfg2",
	"spi_data_64",
	"spi_buf_addr",
	"spi_statu_2",
	"spi_err_status",
	"mem_data_addr",
	"mem_parity_addr",
	"spi_col_addr",
	"spi_bch",
	"spi_intr_msk",
	"spi_intr_sts",
	"spi_page_size",
};


/**************************************************************************
 *                 E X T E R N A L   R E F E R E N C E S                  *
 **************************************************************************/
extern UINT32 Load_Header_Profile(SINT32 type);
extern void Xil_Out32(unsigned int OutAddress, unsigned int Value);
extern unsigned int Xil_In32(unsigned int Addr);
extern void setSystemPara(UINT8 *prData);
extern UINT32 nfvalshift(UINT32 x);
extern UINT8 GetNANDPageCount_1K60(UINT32 pageSize);
extern int verify_xboot_img(u8 *img);

/**************************************************************************
 *               F U N C T I O N   D E C L A R A T I O N S                *
 **************************************************************************/


/**************************************************************************
 *                   F U N C T I O N   D E F I N E S                      *
 **************************************************************************/
__attribute__((unused))
static void dump_spi_regs(struct sp_spinand_info *info)
{
	struct sp_spinand_regs *regs = info->regs;
	u32 *p = (u32 *)regs;
	int i, value;

	for (i=0; i<20; i++, p++) {
		value = readl(p);
		prn_string(reg_name[i]);
		prn_string(" = ");
		prn_dword0(value);
		prn_string("\n");
	}
}

static u32 get_timer(u32 base)
{
	u32 now = AV1_GetStc32()/90;
	return (now - base);
}

static int get_iomode_cfg(u32 io_mode)
{
	int cfg = -1;
	if (io_mode == SPINAND_1BIT_MODE) {
		cfg = SPINAND_CMD_BITMODE(1)
			| SPINAND_CMD_DQ(1)
			| SPINAND_ADDR_BITMODE(1)
			| SPINAND_ADDR_DQ(1)
			| SPINAND_DATA_BITMODE(1)
			| SPINAND_DATAOUT_DQ(1)
			| SPINAND_DATAIN_DQ(2);
	} else if (io_mode == SPINAND_2BIT_MODE) {
		cfg = SPINAND_CMD_BITMODE(1)
			| SPINAND_CMD_DQ(1)
			| SPINAND_ADDR_BITMODE(1)
			| SPINAND_ADDR_DQ(1)
			| SPINAND_DATA_BITMODE(2);
	} else if (io_mode == SPINAND_4BIT_MODE) {
		cfg = SPINAND_CMD_BITMODE(1)
			| SPINAND_CMD_DQ(1)
			| SPINAND_ADDR_BITMODE(1)
			| SPINAND_ADDR_DQ(1)
			| SPINAND_DATA_BITMODE(3);
	} else if (io_mode == SPINAND_DUAL_MODE) {
		cfg = SPINAND_CMD_BITMODE(1)
			| SPINAND_ADDR_BITMODE(2)
			| SPINAND_DATA_BITMODE(2);
	} else if (io_mode == SPINAND_QUAD_MODE) {
		cfg = SPINAND_CMD_BITMODE(1)
			| SPINAND_ADDR_BITMODE(3)
			| SPINAND_DATA_BITMODE(3);
	}

	return cfg;
}

static int get_iomode_readcmd(u32 io_mode)
{
	int cmd = -1;
	if (io_mode == SPINAND_1BIT_MODE) {
		cmd = SPINAND_CMD_PAGEREAD;
	} else if (io_mode == SPINAND_2BIT_MODE) {
		cmd = SPINAND_CMD_PAGEREAD_X2;
	} else if (io_mode == SPINAND_4BIT_MODE) {
		cmd = SPINAND_CMD_PAGEREAD_X4;
	} else if (io_mode == SPINAND_DUAL_MODE) {
		cmd = SPINAND_CMD_PAGEREAD_DUAL;
	} else if (io_mode == SPINAND_QUAD_MODE) {
		cmd = SPINAND_CMD_PAGEREAD_QUAD;
	}
	return cmd;
}

static int wait_spi_idle(struct sp_spinand_info *info)
{
	struct sp_spinand_regs *regs = info->regs;
	u32 now = get_timer(0);
	int ret = -1;

	do {
		if (!(readl(&regs->spi_ctrl) & SPINAND_BUSY_MASK)) {
			ret = 0;
			break;
		}
	} while (get_timer(now) < CONFIG_SPINAND_TIMEOUT);

	if (ret < 0) {
		prn_string("wait_spi_idle timeout!\n");
	}

	return ret;
}

static int spi_nand_trigger_and_wait_dma(struct sp_spinand_info *info)
{
	struct sp_spinand_regs *regs = info->regs;
	unsigned long timeout_ms = CONFIG_SPINAND_TIMEOUT;
	unsigned long now;
	u32 value;
	int ret = -1;

	value = ~SPINAND_DMA_DONE_MASK;
	writel(value, &regs->spi_intr_msk);

	value = readl(&regs->spi_intr_sts);
	writel(value, &regs->spi_intr_sts);

	value = readl(&regs->spi_auto_cfg);
	value |= SPINAND_DMA_TRIGGER;
	writel(value, &regs->spi_auto_cfg);

	now = get_timer(0);
	do {
		if ((readl(&regs->spi_intr_sts) & SPINAND_DMA_DONE_MASK)) {
			ret = 0;
			break;
		}
	} while(get_timer(now) < timeout_ms);

	if(ret < 0) {
		//dump_spi_regs(info);
		prn_string("spi_nand_trigger_and_wait_dma timeout\n");
	}

	return ret;
}

static int spi_nand_trigger_and_wait_pio(struct sp_spinand_info *info)
{
	struct sp_spinand_regs *regs = info->regs;
	unsigned long timeout_ms = CONFIG_SPINAND_TIMEOUT;
	unsigned long now;
	u32 value;
	int ret = -1;

	value = ~SPINAND_PIO_DONE_MASK;
	writel(value, &regs->spi_intr_msk);

	value = readl(&regs->spi_intr_sts);
	writel(value, &regs->spi_intr_sts);

	value = readl(&regs->spi_auto_cfg);
	value |= SPINAND_USR_CMD_TRIGGER;
	writel(value, &regs->spi_auto_cfg);

	now = get_timer(0);
	do {
		if ((readl(&regs->spi_intr_sts) & SPINAND_PIO_DONE_MASK)) {
			ret = 0;
			break;
		}
	} while(get_timer(now) < timeout_ms);

	if(ret < 0) {
		//dump_spi_regs(info);
		prn_string("spi_nand_trigger_and_wait_pio timeout\n");
	}

	return ret;
}

static int spi_nand_getfeatures(struct sp_spinand_info *info, u32 addr)
{
	struct sp_spinand_regs *regs = info->regs;
	u32 value = 0;

	value = SPINAND_SEL_CHIP_A
		| SPINAND_SCK_DIV(info->spi_clk_div)
		| SPINAND_USR_CMD(SPINAND_CMD_GETFEATURES)
		| SPINAND_CTRL_EN
		| SPINAND_USRCMD_DATASZ(1)
		| SPINAND_READ_MODE
		| SPINAND_USRCMD_ADDRSZ(1);
	writel(value, &regs->spi_ctrl);

	writel(addr, &regs->spi_page_addr);

	value = SPINAND_LITTLE_ENDIAN
		| SPINAND_TRS_MODE;
	writel(value, &regs->spi_cfg[0]);

	value = SPINAND_CMD_BITMODE(1)
		| SPINAND_CMD_DQ(1)
		| SPINAND_ADDR_BITMODE(1)
		| SPINAND_ADDR_DQ(1)
		| SPINAND_DATA_BITMODE(1)
		| SPINAND_DATAIN_DQ(2);
	writel(value, &regs->spi_cfg[1]);

	value = SPINAND_USR_CMD_TRIGGER;
	writel(value, &regs->spi_auto_cfg);

	wait_spi_idle(info);

	return (readl(&regs->spi_data) & 0xFF);
}

static int spi_nand_setfeatures(struct sp_spinand_info *info, u32 addr, u32 data)
{
	struct sp_spinand_regs *regs = info->regs;
	u32 value = 0;

	value = SPINAND_SEL_CHIP_A
		| SPINAND_AUTOWEL_EN
		| SPINAND_SCK_DIV(info->spi_clk_div)
		| SPINAND_USR_CMD(SPINAND_CMD_SETFEATURES)
		| SPINAND_CTRL_EN
		| SPINAND_USRCMD_DATASZ(1)
		| SPINAND_WRITE_MODE
		| SPINAND_USRCMD_ADDRSZ(1);
	writel(value, &regs->spi_ctrl);

	writel(addr, &regs->spi_page_addr);

	writel(data, &regs->spi_data);

	value = SPINAND_LITTLE_ENDIAN
		| SPINAND_TRS_MODE;
	writel(value, &regs->spi_cfg[0]);

	value = SPINAND_CMD_BITMODE(1)
		| SPINAND_CMD_DQ(1)
		| SPINAND_ADDR_BITMODE(1)
		| SPINAND_ADDR_DQ(1)
		| SPINAND_DATA_BITMODE(1)
		| SPINAND_DATAOUT_DQ(1);
	writel(value, &regs->spi_cfg[1]);

	value = SPINAND_USR_CMD_TRIGGER;
	writel(value, &regs->spi_auto_cfg);

	return wait_spi_idle(info);
}

static int spi_nand_reset(struct sp_spinand_info *info)
{
	struct sp_spinand_regs *regs = info->regs;
	u32 value = 0;

	value = SPINAND_SEL_CHIP_A
		| SPINAND_SCK_DIV(7)
		| SPINAND_USR_CMD(SPINAND_CMD_RESET)
		| SPINAND_CTRL_EN
		| SPINAND_USRCMD_DATASZ(0)
		| SPINAND_WRITE_MODE
		| SPINAND_USRCMD_ADDRSZ(0);
	writel(value, &regs->spi_ctrl);

	value = SPINAND_READ_TIMING(CONFIG_SPINAND_READ_TIMING_SEL);
	writel(value ,&regs->spi_timing);

	value = SPINAND_LITTLE_ENDIAN
		| SPINAND_TRS_MODE;
	writel(value, &regs->spi_cfg[0]);

	value = SPINAND_CMD_BITMODE(1)
		| SPINAND_CMD_DQ(1)
		| SPINAND_ADDR_BITMODE(0)
		| SPINAND_ADDR_DQ(0)
		| SPINAND_DATA_BITMODE(0)
		| SPINAND_DATAIN_DQ(0)
		| SPINAND_DATAOUT_DQ(0);
	writel(value, &regs->spi_cfg[1]);

	writel(0, &regs->spi_bch);

	value = SPINAND_USR_CMD_TRIGGER;
	writel(value, &regs->spi_auto_cfg);

	wait_spi_idle(info);

	value = SPINAND_CHECK_OIP_EN
		| SPINAND_USR_CMD_TRIGGER;
	writel(value, &regs->spi_auto_cfg);

	return wait_spi_idle(info);
}

static int spi_nand_readid(struct sp_spinand_info *info, u32 addr, u8 *data)
{
	struct sp_spinand_regs *regs = info->regs;
	u32 value = 0;

	/*read 3 byte cycle  */
	value = SPINAND_SEL_CHIP_A
	        | SPINAND_SCK_DIV(7)
	        | SPINAND_USR_CMD(SPINAND_CMD_READID)
	        | SPINAND_CTRL_EN
	        | SPINAND_USRCMD_DATASZ(3)
	        | SPINAND_READ_MODE
	        | SPINAND_USRCMD_ADDRSZ(1);
	writel(value, &regs->spi_ctrl);

	writel(addr, &regs->spi_page_addr);

	value = SPINAND_LITTLE_ENDIAN
		| SPINAND_TRS_MODE;
	writel(value, &regs->spi_cfg[0]);

	value = SPINAND_CMD_BITMODE(1)
		| SPINAND_CMD_DQ(1)
		| SPINAND_ADDR_BITMODE(1)
		| SPINAND_ADDR_DQ(1)
		| SPINAND_DATA_BITMODE(1)
		| SPINAND_DATAIN_DQ(2)
		| SPINAND_DATAOUT_DQ(0);
	writel(value, &regs->spi_cfg[1]);

	value = SPINAND_USR_CMD_TRIGGER;
	writel(value, &regs->spi_auto_cfg);

	wait_spi_idle(info);

	value = readl(&regs->spi_data);

	*(u32 *)data = value;

	return 4;
}

int spi_nand_pagecache(struct sp_spinand_info *info, u32 row)
{
	struct sp_spinand_regs *regs = info->regs;
	u32 value = 0;

	value = SPINAND_SEL_CHIP_A
		| SPINAND_SCK_DIV(info->spi_clk_div)
		| SPINAND_USR_CMD(SPINAND_CMD_PAGE2CACHE)
		| SPINAND_CTRL_EN
		| SPINAND_USRCMD_DATASZ(0)
		| SPINAND_READ_MODE
		| SPINAND_USRCMD_ADDRSZ(3);
	writel(value, &regs->spi_ctrl);

	writel(row, &regs->spi_page_addr);

	value = SPINAND_LITTLE_ENDIAN
		| SPINAND_TRS_MODE;
	writel(value, &regs->spi_cfg[0]);

	value = SPINAND_CMD_BITMODE(1)
		| SPINAND_CMD_DQ(1)
		| SPINAND_ADDR_BITMODE(1)
		| SPINAND_ADDR_DQ(1)
		| SPINAND_DATA_BITMODE(1)
		| SPINAND_DATAIN_DQ(2)
		| SPINAND_DATAOUT_DQ(1);
	writel(value, &regs->spi_cfg[1]);

	value = SPINAND_AUTO_RDSR_EN;
	writel(value, &regs->spi_auto_cfg);

	return spi_nand_trigger_and_wait_pio(info);
}

int spi_nand_readcache(struct sp_spinand_info *info, u32 io_mode,
				u32 col, u8 *buf, u32 size)
{
	struct sp_spinand_regs *regs = info->regs;
	int cfg = get_iomode_cfg(io_mode);
	int cmd = get_iomode_readcmd(io_mode);
	u32 value = 0;
	u32 i;

	if (cfg < 0 || cmd < 0)
		return -1;

	value = SPINAND_LITTLE_ENDIAN
		| SPINAND_TRS_MODE;
	writel(value, &regs->spi_cfg[0]);

	value = SPINAND_DUMMY_CYCLES(8) | cfg;
	writel(value, &regs->spi_cfg[2]);

	value = SPINAND_USR_READCACHE_CMD(cmd)
		| SPINAND_USR_READCACHE_EN
		| SPINAND_AUTO_RDSR_EN;
	writel(value, &regs->spi_auto_cfg);

	do {
		value = readl(&regs->spi_auto_cfg);
	} while((value >> 24) != cmd);

	i = col;
	if (!(col&0x03) && !((u32)buf&0x03)) {
		/* 4 byte aligned case */
		for (; i<(col+size)>>2<<2; i+=4,buf+=4) {
			*(u32*)buf = *(u32*)(SPI_NAND_DIRECT_MAP + i);
		}
	}
	for (; i<(col+size); i++,buf++)
		*buf = *(u8*)(SPI_NAND_DIRECT_MAP + i);

	return wait_spi_idle(info);
}

int spi_nand_read_by_pio(struct sp_spinand_info *info, u32 io_mode,
				u32 row, u32 col, u8 *buf, u32 size)
{
	u32 plane_sel_mode = info->plane_sel_mode;
	int ret = 0;

	if ((plane_sel_mode & 0x1)) {
		u32 pagemark = (plane_sel_mode >> 2) & 0xfff;
		u32 colmark = (plane_sel_mode >> 16) & 0xffff;
		col |= ((row & pagemark) != 0) ? colmark : 0;
	}
	ret = spi_nand_pagecache(info, row);
	if (!ret)
		ret = spi_nand_readcache(info, io_mode, col, buf, size);

	return ret;
}

int spi_nand_read_by_pio_auto(struct sp_spinand_info *info, u32 io_mode,
				u32 row, u32 col, u8 *buf, u32 size)
{
	struct sp_spinand_regs *regs = info->regs;
	u32 plane_sel_mode = info->plane_sel_mode;
	u32 page_size = info->page_size;
	int cfg = get_iomode_cfg(io_mode);
	int cmd = get_iomode_readcmd(io_mode);
	u32 value;
	u32 i;

	if (cfg < 0 || cmd < 0)
		return -1;

	value = SPINAND_AUTOMODE_EN
		| SPINAND_AUTOCMD_EN
		| SPINAND_SEL_CHIP_A
		| SPINAND_SCK_DIV(info->spi_clk_div)
		| SPINAND_USR_CMD(SPINAND_CMD_PAGE2CACHE)
		| SPINAND_CTRL_EN
		| SPINAND_READ_MODE
		| SPINAND_USRCMD_ADDRSZ(2);
	writel(value, &regs->spi_ctrl);

	value = (size+3) & (~3);
	value = SPINAND_LITTLE_ENDIAN
		| SPINAND_DATA64_EN
		| SPINAND_TRS_MODE
		| SPINAND_DATA_LEN(value);
	writel(value, &regs->spi_cfg[0]);

	cfg |= SPINAND_DUMMY_CYCLES(8);
	writel(cfg, &regs->spi_cfg[1]);

	writel(row, &regs->spi_page_addr);

	if ((plane_sel_mode & 0x1)) {
		u32 pagemark = (plane_sel_mode >> 2) & 0xfff;
		u32 colmark = (plane_sel_mode >> 16) & 0xffff;
		col |= ((row & pagemark) != 0) ? colmark : 0;
		page_size += ((row & pagemark) != 0) ? colmark : 0;
	}
	writel(col, &regs->spi_col_addr);

	value = SPINAND_SPARE_SIZE(info->oob_size)
		| SPINAND_PAGE_SIZE((page_size >> 10) - 1);
	writel(value, &regs->spi_page_size);

	value = SPINAND_USR_CMD_TRIGGER
		| SPINAND_USR_READCACHE_CMD(cmd)
		| SPINAND_USR_READCACHE_EN;
	writel(value, &regs->spi_auto_cfg);

	for(i=0; i<size; i++) {
		if ((i&0x03) == 0)
			value = readl(&regs->spi_data_64);
		buf[i] = value & 0xff;
		value >>= 8;
	}

	return wait_spi_idle(info);
}

int spi_nand_read_by_dma(struct sp_spinand_info *info, u32 io_mode,
				u32 row, u32 col, u8 *buf, u32 size)
{
	struct sp_spinand_regs *regs = info->regs;
	u32 plane_sel_mode = info->plane_sel_mode;
	u32 page_size = info->page_size;
	int cmd = get_iomode_readcmd(io_mode);
	int cfg = get_iomode_cfg(io_mode);
	u32 value = 0;

	if (cmd < 0 || cfg < 0)
		return -1;

	value = SPINAND_SEL_CHIP_A
		| SPINAND_SCK_DIV(info->spi_clk_div)
		| SPINAND_CTRL_EN
		| SPINAND_USRCMD_ADDRSZ(2);
	writel(value, &regs->spi_ctrl);

	writel(row, &regs->spi_page_addr);

	value = SPINAND_LITTLE_ENDIAN
		| SPINAND_TRS_MODE
		| SPINAND_DATA_LEN(size);
	writel(value, &regs->spi_cfg[0]);

	value = cfg | SPINAND_DUMMY_CYCLES(8);
	writel(value, &regs->spi_cfg[1]);

	if ((plane_sel_mode & 0x1) != 0) {
		u32 pagemark = (plane_sel_mode>>2)&0xfff;
		u32 colmark = (plane_sel_mode>>16)&0xffff;
		col |= ((row & pagemark) != 0) ? colmark : 0;
		page_size += ((row & pagemark) != 0) ? colmark : 0;
	}
	writel(col, &regs->spi_col_addr);

	value = SPINAND_SPARE_SIZE(info->oob_size)
		| SPINAND_PAGE_SIZE((page_size >> 10) - 1);
	writel(value, &regs->spi_page_size);

	writel((u32)buf, &regs->mem_data_addr);

	value = SPINAND_USR_READCACHE_CMD(cmd)
		| SPINAND_USR_READCACHE_EN;
	writel(value, &regs->spi_auto_cfg);

	return spi_nand_trigger_and_wait_dma(info);
}

int spi_nand_pageread_1k60_autobch(struct sp_spinand_info *info,u32 io_mode,
					u32 row, u8 *data_buf, u8* redunt_buf)
{
	struct sp_spinand_regs *regs = info->regs;
	u32 plane_sel_mode = info->plane_sel_mode;
	u32 page_size = info->page_size;
	u32 data_size = GetNANDPageCount_1K60(info->page_size) << 10;
	int cmd = get_iomode_readcmd(io_mode);
	int cfg = get_iomode_cfg(io_mode);
	u32 value = 0;
	int ret;

	if (cmd < 0 || cfg < 0)
		return -1;

	value = SPINAND_SEL_CHIP_A
		| SPINAND_SCK_DIV(info->spi_clk_div)
		| SPINAND_CTRL_EN
		| SPINAND_USRCMD_ADDRSZ(2);
	writel(value, &regs->spi_ctrl);

	value = SPINAND_LITTLE_ENDIAN
		| SPINAND_TRS_MODE
		| SPINAND_DATA_LEN(data_size);
	writel(value, &regs->spi_cfg[0]);

	value = cfg | SPINAND_DUMMY_CYCLES(8);
	writel(value, &regs->spi_cfg[1]);

	writel(row, &regs->spi_page_addr);

	value = 0;
	if ((plane_sel_mode & 0x1) != 0) {
		u32 pagemark = (plane_sel_mode>>2)&0xfff;
		u32 colmark = (plane_sel_mode>>16)&0xffff;
		value |= ((row & pagemark) != 0) ? colmark : 0;
		page_size += ((row & pagemark) != 0) ? colmark : 0;
	}
	writel(value, &regs->spi_col_addr);

	value = SPINAND_SPARE_SIZE(info->oob_size)
		| SPINAND_PAGE_SIZE((page_size >> 10) - 1);
	writel(value, &regs->spi_page_size);

	writel((u32)data_buf, &regs->mem_data_addr);
	writel((u32)redunt_buf, &regs->mem_parity_addr);
	BCHConfig((u32*)data_buf, (u32*)redunt_buf, data_size,
			BCH_DECODE, BCH_S338_1K60_BITS_MODE);

	value = SPINAND_BCH_1K_MODE
		| SPINAND_BCH_BLOCKS((data_size>>10) - 1)
		| SPINAND_BCH_DATA_LEN(128)
		| SPINAND_BCH_ALIGN_32B
		| SPINAND_BCH_AUTO_EN;
	writel(value, &regs->spi_bch);

	value = SPINAND_USR_READCACHE_CMD(cmd)
		| SPINAND_USR_READCACHE_EN;
	writel(value, &regs->spi_auto_cfg);

	ret = spi_nand_trigger_and_wait_dma(info);
	if (!ret) {
		if (BCHCheckStatus(BCH_DECODE)==ret_BCH_S338_FAIL)
			ret = -1;
		else
			ret = data_size;
	}
	writel(0, &regs->spi_bch);

	return ret;
}

static int spi_nand_pageread_1k60(struct sp_spinand_info *info,
	u32 io_mode, u32 trs_mode, u32 row, u8 *data_buf, u8* redunt_buf)
{
	u32 data_size = GetNANDPageCount_1K60(info->page_size) << 10;
	u32 redunt_size = (data_size >> 10) * 128;
	int ret = -1;

	if (trs_mode == SPINAND_TRS_DMA_AUTOBCH) {
		#ifdef CFG_SUPPORT_DMA_AUTOBCH_MODE
		return spi_nand_pageread_1k60_autobch(info,
			io_mode, row, data_buf, redunt_buf);
		#else
		return -1;
		#endif
	} else if (trs_mode == SPINAND_TRS_DMA) {
		#ifdef CFG_SUPPORT_DMA_MODE
		ret = spi_nand_read_by_dma(info,
			io_mode, row, 0, data_buf, data_size);
		ret |= spi_nand_read_by_dma(info,
			io_mode, row, data_size, redunt_buf, redunt_size);
		#else
		ret = -1;
		#endif
	} else if (trs_mode == SPINAND_TRS_PIO_AUTO) {
		#ifdef CFG_SUPPORT_PIO_AUTO_MODE
		ret = spi_nand_read_by_pio_auto(info,
			io_mode, row, 0, data_buf, data_size);
		ret |= spi_nand_read_by_pio_auto(info,
			io_mode, row, data_size, redunt_buf, redunt_size);
		#else
		ret = -1;
		#endif
	} else if (trs_mode == SPINAND_TRS_PIO) {
		ret = spi_nand_read_by_pio(info,
			io_mode, row, 0, data_buf, data_size);
		ret |= spi_nand_read_by_pio(info,
			io_mode, row, data_size, redunt_buf, redunt_size);
	}

	if (ret < 0)
		return ret;

	ret = BCHProcess((u32*)data_buf, (u32 *)redunt_buf, data_size,
				BCH_DECODE, BCH_S338_1K60_BITS_MODE);
	if (ret != ROM_SUCCESS)
		return -1;
	else
		return data_size;
}

int initSPIDriver(void)
{
	struct sp_spinand_info *info = &g_bootinfo.our_spinfc;
	struct BootProfileHeader *boot_hdr = Get_Header_Profile_Ptr();
	s32 feature_b0;

	info->regs = (struct sp_spinand_regs * )CONFIG_SP_SPINAND_BASE;
	info->spi_clk_div = CONFIG_SPINAND_CLK_DIV;

	/* NAND CTRL soft reset */
	if (spi_nand_reset(info) < 0 ) {
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

	feature_b0 = spi_nand_getfeatures(info, 0xB0);
	prn_string("Default B0 feature: ");
	prn_dword(feature_b0);
	//prn_string("\n");

	switch (g_pyldData[0]) {
	case VID_WINBOND:
		feature_b0 |= 0x80;           /* enable BUF-1 */
		break;

	case VID_GD:
	//case VID_ESMT:
	//case VID_ISSI:
		/* GD,ESMT,ISSI use the same vendor id */
		if((g_pyldData[1] & 0xf0) == 0xd0)  /* gd device */
			feature_b0 |= 0x01;   /* enable QuadIO mode */
		break;

	case VID_MICRON:
		if(g_pyldData[1] == 0x34)
			feature_b0 &= ~0x01;  /* disable continues read */
		break;

	case VID_MXIC:
		feature_b0 |= 0x01;           /* enable QuadIO mode */
		break;

	case VID_ETRON:
		feature_b0 |= 0x01;           /* enable QuadIO mode */
		break;

	default:
		break;
	}

	feature_b0 &= ~0x10;                 /* disable internal ecc */
	prn_string("Configure B0 feature to:");
	prn_dword(feature_b0);
	//prn_string("\n");
	spi_nand_setfeatures(info, 0xB0, feature_b0);

	CSTAMP(0x59100007);

	rd_memcpy((UINT32 *)g_pSysInfo->IdBuf, (UINT32 *)g_pyldData, ((g_bootinfo.gNANDIDLength + 3) >> 2) << 2);

#ifdef PARTIAL_NAND_INIT
	setSystemPara(g_pSysInfo->IdBuf);
	info->page_size = g_pSysInfo->u16PyldLen;
	info->oob_size = g_pSysInfo->u16ReduntLen;
	info->plane_sel_mode = boot_hdr->PlaneSelectMode;
	return ROM_SUCCESS; /* xboot */
#else
	CSTAMP(0x59100009);

	setSystemPara(g_pSysInfo->IdBuf);

	CSTAMP(0x5910000A);

	/* give a initial value to read header */
	info->page_size = g_pSysInfo->u16PyldLen = 2048;   /* 2K Page */
	info->oob_size = g_pSysInfo->u16ReduntLen = 64;    /* 64 oob size */
	g_pSysInfo->u16PageNoPerBlk = 64;    /* 64 pages per block */
	g_pSysInfo->u8PagePerBlkShift = nfvalshift(64);

	CSTAMP(0x5910000B);

	/* before load header */
	if (Load_Header_Profile(SPINAND_BOOT) == ROM_SUCCESS) {

		CSTAMP(0x5910000C);

		setSystemPara(g_pSysInfo->IdBuf);
		info->page_size = g_pSysInfo->u16PyldLen;
		info->oob_size = g_pSysInfo->u16ReduntLen;
		info->plane_sel_mode = boot_hdr->PlaneSelectMode;

		return ROM_SUCCESS;
	}

	CSTAMP(0x5910000D);

	/* failed to load header */
	return ROM_FAIL;
#endif
}

void initSPINandFunptr(void)
{
	SDev_t* pSDev = getSDev();

	pSDev->IsSupportBCH = 1;
	pSDev->DeviceID = DEVICE_SPI_NAND;

	pSDev->predInitDriver    = (predInitDriver_t)initSPIDriver;
	pSDev->predReadWritePage = NULL;
}

SINT32 ReadSPINANDSector_1K60(UINT32 * ptrPyldData, UINT32 pageNo)
{
	struct sp_spinand_info *info = &g_bootinfo.our_spinfc;
	u32 data_size = 1024;
	u32 oob_size = 128;
	u8 *data_buf = (u8 *)ptrPyldData;
	u8 *oob_buf = data_buf + data_size;
	int ret;

	ret = spi_nand_read_by_pio(info, CONFIG_SPINAND_READ_BITMODE,
		pageNo, 0, data_buf, data_size+oob_size);
	if(ret < 0)
		return ROM_FAIL;

	ret = BCHProcess((u32*)data_buf, (u32*)oob_buf,
		data_size, BCH_DECODE, BCH_S338_1K60_BITS_MODE);
	if (ret == ret_BCH_S338_FAIL) {
		ret = ROM_FAIL;
	} else {
		ret = ROM_SUCCESS;
	}

	return ret;
}

SINT32 SPINANDReadNANDPage_1K60(UINT8 which_cs, UINT32 pageNo, UINT32 * ptrPyldData, UINT32 *read_bytes)
{
	struct sp_spinand_info *info = &g_bootinfo.our_spinfc;
	u32 data_size = GetNANDPageCount_1K60(info->page_size) << 10;
	u8 *data_buf = (u8 *)ptrPyldData;
	u8 *redunt_buf = data_buf + data_size;
	int ret;

	ret = spi_nand_pageread_1k60(info, CONFIG_SPINAND_READ_BITMODE,
		CONFIG_SPINAND_TRSMODE, pageNo, data_buf, redunt_buf);
	if (ret < 0) {
		*read_bytes = 0;
		ret = ROM_FAIL;
	} else {
		*read_bytes = ret;
		ret = ROM_SUCCESS;
	}

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

			prn_string("load xboot skip="); prn_decimal_ln(skip_blk);
			prn_string("copy="); 			prn_decimal_ln(i); //prn_string("\n");

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
					prn_string("cnt="); prn_decimal_ln(ptr->xboot_pg_cnt); //prn_string("\n");
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

