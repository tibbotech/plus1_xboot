#include <common.h>

/**************************************************************************
 *                             M A C R O S                                *
 **************************************************************************/

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
extern void setSystemPara(UINT8 *prData);
extern UINT32 nfvalshift(UINT32 x);
extern int get_xboot_size(u8 *img);
extern int verify_xboot_img(u8 *img);

/*********************Basic function********************/
UINT8 para_nand_ecc_setting(UINT8 u8_channel_list, UINT8 u8_enable, INT8 s8_correct_bits, INT8 s8_threshold_bits,
							UINT8 u8_leave_space_for_ecc, struct flash_info *flash_readable_info)
{
	UINT8 u8_i;

	flash_readable_info->u32_spare_start_in_byte_pos = flash_readable_info->u16_page_size << 10;

	for (u8_i = 0; u8_i < NANDC_MAX_CHANNEL; u8_i++) {

		if (!(u8_channel_list & (1 << u8_i))) {
			continue;
		}

		ECC_function(u8_i, u8_enable);
		ECC_mask(u8_i, u8_enable);
		ECC_threshold_ecc_error(u8_i, s8_threshold_bits);
		ECC_correct_ecc_error(u8_i, s8_correct_bits);
	}

	if(u8_enable == 1){
		ECC_leave_space(ecc_existence_between_sector);
	}
	else{
		ECC_leave_space(u8_leave_space_for_ecc);
	}

	return 1;
}

void ECC_related_setting(struct flash_info *flash_readable_info) {

	flash_readable_info->u16_sector_size = 1024;
	// Setting ECC
	ECC_setting_base_size(ecc_base_1kbyte);
	para_nand_ecc_setting(0xF, 1, 60, 1, ecc_existence_between_sector, flash_readable_info);

	// Setting the proper ecc protection capability for spare
	para_nand_spare_ecc_setting(0, 4, 1);
}

UINT8 para_nand_init(struct flash_info * flash_readable_info)
{
	// Setting the interrupt
	ECC_enable_interrupt(hit_threshold_intr_enable, correct_fail_intr_enable,
			     hit_spare_threshold_intr_enable, correct_spare_fail_intr_enable);

	para_nand_interrupt_enable(0, flash_status_fail_intr_enable);

	ECC_related_setting(flash_readable_info);

	// Setting Genereral setting of NANDC

	para_nand_general_setting_for_all_channel(flash_write_protect_disable, data_inverse_mode_disable,
						 			         data_scrambler_disable, busy_ready_bit_location_bit6, cmd_pass_fail_bit_location_bit0);
	// Setting the AHB slave space
	AHB_slave_memory_space(AHB_Memory_32KByte);

	return 1;
}

UINT8 Resetflash(struct flash_info *flash_readable_info)
{
	UINT8 u8_channel = 0;
	UINT8 u8_starting_ce = 0;

	struct command_queue_feature reset_cmd_feature = { 0 };
	struct command_queue_feature *p_reset_cmd_feature = &reset_cmd_feature;

	reset_cmd_feature.Flash_type = flash_readable_info->u8_flash_access_mode;
	reset_cmd_feature.Complete_interrupt_enable = Command_Complete_Intrrupt_Enable;

	Setting_feature_and_fire(u8_channel, Command(RESET), u8_starting_ce, *((UINT32 *)p_reset_cmd_feature));

	if(Check_command_status(u8_channel) & Cmd_status_err_occur){
		prn_string("Reset is failed\n");
		return 0;
	}

	return 1;
}

UINT32 hw_init_and_setting(void)
{
	struct flash_timing_setting_new timing;

	memset(&g_bootinfo.flash_readable_info, 0, sizeof(g_bootinfo.flash_readable_info));

	g_bootinfo.flash_readable_info.u16_page_size = (g_pSysInfo->u16PyldLen >> 10);
	g_bootinfo.flash_readable_info.u8_eD3_toshiba = 0;
	g_bootinfo.flash_readable_info.u16_page_in_block = g_pSysInfo->u16ReduntLen;
	g_bootinfo.flash_readable_info.u16_page_boundary_in_block = g_pSysInfo->u16PageNoPerBlk;
	g_bootinfo.flash_readable_info.u8_flash_access_mode = Legacy_flash;

	/* config nand controller(interrupt/ecc/general_setting/ahb size) */
	if (!para_nand_init(&g_bootinfo.flash_readable_info)) {
		prn_string("Init failed\n");
		return ROM_FAIL;
	}

	/* Q654 NAND controller only use one 1 channel and 1 chip enable */
	para_nand_chip_num(each_channel_have_1CE);

	Memory_attribute_setting(&g_bootinfo.flash_readable_info);

	/* set read header AC timing, depend on HCLK 200 000 000*/
	timing.AC_Timing0 = g_pSysInfo->u32AC_Timing0;
	timing.AC_Timing1 = g_pSysInfo->u32AC_Timing1;
	timing.AC_Timing2 = g_pSysInfo->u32AC_Timing2;
	timing.AC_Timing3 = g_pSysInfo->u32AC_Timing3;

	Flash_AC_timing_setting_new(0, &timing);

	return ROM_SUCCESS;
}

SINT32 PNAND_InitDriver(void)
{
	int adjust_cnt = 3;

	dbg_info();

	//xt: NAND CTRL soft reset, 0 is channel 0
	Soft_reset(0);//xt: need?

	dbg();

	/* give a initial value to read header */
	g_pSysInfo->u16PyldLen = 2048;		/* 2K Page */
	g_pSysInfo->u16ReduntLen = 64;
	g_pSysInfo->u16PageNoPerBlk = 64;	/* 64 pages per block */
	g_pSysInfo->u8PagePerBlkShift = nfvalshift(64);
	g_pSysInfo->u8addrCycle = 5;		/* 3 row + 2 col */
	g_pSysInfo->u32AC_Timing0 = 0x0f1f0f1f;//0x02020204
	g_pSysInfo->u32AC_Timing1 = 0x00007f7f;//0x00001401
	g_pSysInfo->u32AC_Timing2 = 0x7f7f7f7f;//0x0c140414
	g_pSysInfo->u32AC_Timing3 = 0xff1f001f;//0x00040014

	/* config the nand flash attribute */
	if (hw_init_and_setting() == ROM_FAIL)
		return ROM_FAIL;

	Resetflash(&g_bootinfo.flash_readable_info);

	while(adjust_cnt--) {
		if (Load_Header_Profile(PARA_NAND_BOOT) == ROM_SUCCESS) {
			CSTAMP(0xAA551013);
			setSystemPara(g_pSysInfo->IdBuf);
			CSTAMP(0xAA551014);
			break;
		} else {
			//Adjust actiming by failing to read SIGNATURE of header
		}
	}
	if(adjust_cnt == 0)
		return ROM_FAIL;

	/* Get para from the header and set again */
	if (hw_init_and_setting() == ROM_FAIL)
		return ROM_FAIL;

	return ROM_SUCCESS;
}

#include <config.h>
#include <types.h>
#include <regmap.h>
static void uart0_wait(void)
{
	unsigned int lsr = 0;

	while (!lsr) {
		lsr = UART0_REG->lsr;
		lsr &= 1;
	}
}

void uart0_putc(unsigned char c)
{
	uart0_wait();
	UART0_REG->dr = c;
}
#define UART_put_byte(x) uart0_putc(x)
void prn_dword0_byte(unsigned int w)
{
	char c, i;

	for (i = 7; i <= 8; i++) {
		c = (w >> (32 - (i << 2))) & 0xF;
		if (c < 0xA)
			UART_put_byte(c + 0x30);
		else
			UART_put_byte(c + 0x37);
	}
}

UINT8 PNAND_ReadPage(UINT32 u32_row_addr, UINT16 u16_sector_offset,
			  UINT16 u16_sector_cnt, struct collect_data_buf * read_data_buf,
			  struct flash_info * flash_readable_info)
{
	UINT8 u8_channel = 0;
	UINT8 u8_starting_ce = 0;
	UINT8 check_data_0xFF = 1;
	UINT8 u8_cmd_status;
	struct command_queue_feature cmd_feature = { 0 };
	//To solve warning: dereferencing type-punned pointer will break strict-aliasing rules [-Wstrict-aliasing]
	//Setting_feature_and_fire(u8_channel, Command(PAGE_READ), u8_starting_ce, *((u32 *)&cmd_feature));
	struct command_queue_feature *p_cmd_feature = &cmd_feature;

	cmd_feature.Flash_type = flash_readable_info->u8_flash_access_mode;
	cmd_feature.Complete_interrupt_enable = Command_Complete_Intrrupt_Enable;
	cmd_feature.Command_incremental_scale = Command_Inc_By_Page;
	cmd_feature.BMC_region = BMC_Region0;
	cmd_feature.BMC_ignore = No_Ignore_BMC_Region_Status;
	cmd_feature.Byte_mode = Byte_Mode_Disable;

	read_data_buf->u8_handshake_mode = cmd_feature.Command_handshake_mode = Command_Handshake_Disable;

	read_data_buf->u8_ce_num = u8_starting_ce;
	read_data_buf->u16_col_addr = u16_sector_offset;
	read_data_buf->u32_data_length_in_bytes = u16_sector_cnt * flash_readable_info->u16_sector_size;
	read_data_buf->u16_data_length_in_sector = u16_sector_cnt;
	read_data_buf->u8_interleave_level = 1;
	read_data_buf->u8_user_mode = 0;
	read_data_buf->u8_bytemode = cmd_feature.Byte_mode;
	read_data_buf->u16_spare_length = 0;
	read_data_buf->u8_data_region_num = BMC_Region0;

	// Checking the command queue isn't full
	while (Command_queue_status_full(u8_channel));

	Page_read_setting(u8_channel, u32_row_addr, (UINT8)u16_sector_offset, read_data_buf);
	Setting_feature_and_fire(u8_channel, Command(PAGE_READ), u8_starting_ce, *((u32 *)p_cmd_feature));

	u8_cmd_status = Check_command_status(u8_channel);
	Data_read(u8_channel, read_data_buf, 1, 0, flash_readable_info);

	if(u8_cmd_status & Cmd_status_ecc_correct_failed_in_spare){
		prn_string("Page num:");
		prn_decimal_ln(u32_row_addr);
		if(ECC_spare_error_status(u8_channel) != 0) {
			prn_string("Error bit for Spare:(for reference only)");
			prn_dword(ECC_spare_error_status(u8_channel));
		}
	}
	ECC_clear_spare_error_status(u8_channel);

	if(u8_cmd_status & (Cmd_status_ecc_correct_failed)) {
		/* Consider the case that data is 0xFF */
		u32 ecc_original_setting = NANDC_32BIT(OFFSET_ECC_CTRL);
		/* Disable the ECC engine, temporarily */
		u32 val = NANDC_32BIT(OFFSET_ECC_CTRL);
		val = val & ~(0xFF << 8);
		NANDC_32BIT(OFFSET_ECC_CTRL) = val;

		Page_read_setting(u8_channel, u32_row_addr, (UINT8)u16_sector_offset, read_data_buf);
		Setting_feature_and_fire(u8_channel, Command(PAGE_READ), u8_starting_ce, *((u32 *)p_cmd_feature));

		Check_command_status(u8_channel);
		Data_read(u8_channel, read_data_buf, 1, 0, flash_readable_info);

		for(int i = 0; i < (read_data_buf->u32_data_length_in_bytes >> 2) ; i++) {
			val = *((volatile u32 *)read_data_buf->u8_data_buf + i);
			if(val != 0xFFFFFFFF) {
				check_data_0xFF = 0;
				break;
			}
		}
#if 0/* Dump data */
		for(int j = 0; j < 3072 / 16; j++) {
			prn_dword0(j * 16);prn_string(":");
			for(int i = 0; i < 16; i++) {
				val = *(read_data_buf->u8_data_buf + i + j*16);
				prn_dword0_byte(val);prn_string(" ");
			}
			prn_string("\n");
		}
#endif
		if(check_data_0xFF) {
			u8_cmd_status &= ~(Cmd_status_ecc_correct_failed);
		} else {
			prn_string("ECC correction is failed\n");
			prn_string("Page num:");
			prn_decimal_ln(u32_row_addr);
			if(ECC_error_status(u8_channel) != 0) {
				prn_string("Error bit:(for reference only)");
				prn_dword(ECC_error_status(u8_channel));
			}
		}

		// Restore the ecc original setting.
		NANDC_32BIT(OFFSET_ECC_CTRL) = ecc_original_setting;
	}
	ECC_clear_error_status(u8_channel);

	return u8_cmd_status;
}

void initPNandFunptr(void)
{
	SDev_t* pSDev = getSDev();

	pSDev->IsSupportBCH = 0;
	pSDev->DeviceID = DEVICE_PARA_NAND;

	pSDev->predInitDriver = (predInitDriver_t)PNAND_InitDriver;
	pSDev->predReadPage = (predReadPage_t)PNAND_ReadPage;
}

SINT32 ReadPNANDSector_1K60(UINT32 * ptrPyldData, UINT32 pageNo)
{
	int ret;
	struct collect_data_buf read_data_buf;

	SDev_t* pSDev = getSDev();

	read_data_buf.u8_data_buf = (UINT8 *)g_pyldData;
	memset(read_data_buf.u8_data_buf, 0, 1024);
	read_data_buf.u8_spare_data_buf = (UINT8 *)g_spareData;
	memset(read_data_buf.u8_spare_data_buf, 0, 64);

	ret = pSDev->predReadPage(pageNo, 0, 1, &read_data_buf, &g_bootinfo.flash_readable_info);

	if (ret & Cmd_status_err_occur)
	{
		prn_string("Page read FAILED\n");
		return ROM_FAIL;
	}

	return ROM_SUCCESS;
}

SINT32 PNANDReadNANDPage_1K60(UINT32 pageNo, UINT32 * ptrPyldData, UINT32 *read_bytes)
{
	int ret;
	struct collect_data_buf read_data_buf;
	u8 sector_cnt;

	SDev_t* pSDev = getSDev();

	sector_cnt = GetNANDPageCount_1K60(g_pSysInfo->u16PyldLen);
	*read_bytes = sector_cnt * g_bootinfo.flash_readable_info.u16_sector_size;

	read_data_buf.u8_data_buf = (UINT8 *)(ptrPyldData);
	memset(read_data_buf.u8_data_buf, 0, *read_bytes);
	read_data_buf.u8_spare_data_buf = (UINT8 *)g_spareData;
	memset(read_data_buf.u8_spare_data_buf, 0, 64);

	ret = pSDev->predReadPage(pageNo, 0, sector_cnt, &read_data_buf, &g_bootinfo.flash_readable_info);

	if (ret & Cmd_status_err_occur)
	{
		prn_string("Page read FAILED\n");
		return ROM_FAIL;
	}

	return ROM_SUCCESS;
}

SINT32 PNANDReadBootBlock(UINT32 *target_address)
{
	struct BootProfileHeader *ptr = Get_Header_Profile_Ptr();
	u32 i, j, read_bytes = 0, pg_off = 0, skip_blk;
	u8 *buf;
	int res;
	int sz_sect = GetNANDPageCount_1K60(g_pSysInfo->u16PyldLen) * 1024;
	int xbsize;

	//TODO: skip bad block by checking mark in OOB if mark position is indifferent
	//
	for (skip_blk = 0; skip_blk < 8; skip_blk++) {

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
				res = PNANDReadNANDPage_1K60(pg_off, (u32 *)buf, &read_bytes);
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

			dbg();
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
