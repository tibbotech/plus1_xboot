#include <common.h>

////////////////////////////////////////     HAL       ///////////////////////////////////////////////

//Number of the ECC error bits on spare (RO)
UINT8 ECC_spare_error_status(UINT8 u8_channel)
{
	return (FTNANDC024_8BIT(OFFSET_ECC_STATUS_SP + u8_channel) & 0x7F);
}

//Number of the ECC error bits (RO)
UINT8 ECC_error_status(UINT8 u8_channel)
{
	return (FTNANDC024_8BIT(OFFSET_ECC_STATUS + u8_channel) & 0x7F);
}

//clear the ECC error bits (clear 0x0 and ox2c)
void ECC_clear_error_status(UINT8 u8_channel)
{
	FTNANDC024_8BIT(OFFSET_ECC_STATUS_CLR) |= (1 << u8_channel);
}

void ECC_clear_spare_error_status(UINT8 u8_channel)
{
	FTNANDC024_8BIT(OFFSET_ECC_STATUS_CLR + 1) |= (1 << u8_channel);
}

UINT8 ECC_threshold_ecc_error(UINT8 u8_channel, UINT8 u8_threshold_num)
{
	// Clear the specified field.
	FTNANDC024_8BIT(OFFSET_ECC_THRES + u8_channel) &= ~0x7F;
	FTNANDC024_8BIT(OFFSET_ECC_THRES + u8_channel) |= ((u8_threshold_num - 1) & 0x7F);
	return 1;
}

//set number of ECC correct capability bits
UINT8 ECC_correct_ecc_error(UINT8 u8_channel, UINT8 u8_correct_num)
{
	// Clear the specified field.
	FTNANDC024_8BIT(OFFSET_ECC_CORR + u8_channel) &= ~0x7F;
	FTNANDC024_8BIT(OFFSET_ECC_CORR + u8_channel) |= ((u8_correct_num - 1) & 0x7F);
	return 1;
}

UINT8 Report_ECC_capability_error(UINT8 u8_channel)
{
	return (FTNANDC024_8BIT(OFFSET_ECC_CORR + u8_channel) & 0x7F) + 1;
}

UINT8 ECC_setting_base_size(UINT8 u8_ecc_base_size)
{
	// Clear the ecc base field
	FTNANDC024_32BIT(OFFSET_ECC_CTRL) &= ~(1 << 16);
	// Set the value
	FTNANDC024_32BIT(OFFSET_ECC_CTRL) |= (UINT32) ((u8_ecc_base_size & 0x1) << 16);
	return 1;
}

UINT8 ECC_leave_space(UINT8 u8_choice)
{
	// Clear the field
	FTNANDC024_32BIT(OFFSET_ECC_CTRL) &= ~(1 << 17);
	// Set the value
	FTNANDC024_32BIT(OFFSET_ECC_CTRL) |= ((u8_choice & 0x1) << 17);
	return 1;
}

UINT8 ECC_function(UINT8 u8_channel, UINT8 u8_choice)
{
	// Clear the field
	FTNANDC024_8BIT(OFFSET_ECC_CTRL + 1) &= ~(1 << u8_channel);
	// Set the value
	FTNANDC024_8BIT(OFFSET_ECC_CTRL + 1) |= ((u8_choice & 0x1) << u8_channel);
	return 1;
}

UINT8 ECC_mask(UINT8 u8_channel, UINT8 u8_mask)
{
	// Clear the field
	FTNANDC024_8BIT(OFFSET_ECC_CTRL) &= ~(1 << u8_channel);
	// Set the value
	FTNANDC024_8BIT(OFFSET_ECC_CTRL) |= ((u8_mask & 0x1) << u8_channel);
	return 1;
}

UINT8 ECC_mask_status(UINT8 u8_channel)
{
	if(FTNANDC024_8BIT(OFFSET_ECC_CTRL) & (1 << u8_channel)) {
		return ecc_mask;
	}
	else {
		return ecc_unmask;
	}
}

UINT8 ECC_enable_interrupt(UINT8 u8_ecc_error_bit_hit_the_threshold, UINT8 u8_ecc_correct_failed,
			  UINT8 u8_ecc_error_bit_hit_the_threshold_for_spare, UINT8 u8_ecc_correct_failed_for_spare)
{
	FTNANDC024_32BIT(OFFSET_ECC_INTR_EN) =
		(UINT32) (((u8_ecc_error_bit_hit_the_threshold_for_spare) << 3) |
			  ((u8_ecc_correct_failed_for_spare) << 2) |
			  ((u8_ecc_error_bit_hit_the_threshold) << 1) |
			  (u8_ecc_correct_failed));
	return 1;
}

UINT8 ECC_correction_failed(UINT8 u8_channel)
{
	return (((g_bootinfo.ftnandc024_status.u8_ecc_correction_failed_for_data) >> u8_channel) & 0x1);
}

UINT8 ECC_failed_hit_threshold(UINT8 u8_channel){
	return (((g_bootinfo.ftnandc024_status.u8_error_hit_threshold_for_data) >> u8_channel) & 0x1);
}

UINT8 ECC_correction_failed_for_spare(UINT8 u8_channel){
	return (((g_bootinfo.ftnandc024_status.u8_ecc_correction_failed_for_spare) >> u8_channel) & 0x1);
}

UINT8 ECC_failed_hit_threshold_for_spare(UINT8 u8_channel){
	return (((g_bootinfo.ftnandc024_status.u8_error_hit_threshold_for_spare) >> u8_channel) & 0x1);
}

UINT8 NANDC024_general_setting_for_all_channel(UINT8 u8_flash_write_protection,
					      UINT8 u8_data_inverse, UINT8 u8_scrambler,
					      UINT8 u8_device_busy_ready_status_bit_location,
					      UINT8 u8_command_pass_failed_status_bit_location)
{
	FTNANDC024_32BIT(OFFSET_GENERAL_SET) =
	    (UINT32) ((u8_device_busy_ready_status_bit_location << 12) |
		      (u8_command_pass_failed_status_bit_location << 8) |
		      (u8_flash_write_protection << 2) | (u8_data_inverse << 1) | (u8_scrambler));
	return 1;
}

UINT8 NANDC024_chip_num(UINT8 u8_chip_num)
{
	// Clear
	FTNANDC024_8BIT(OFFSET_GENERAL_SET + 3) &= ~(0x0F);
	// Setting
	FTNANDC024_8BIT(OFFSET_GENERAL_SET + 3) |= u8_chip_num;

	return 1;
}

UINT8 NANDC024_spare_ecc_setting(UINT8 u8_channel, UINT8 u8_ecc_correct_err_bit_for_spare,
				UINT8 u8_ecc_thres_bit_for_spare)
{
	// Clear the field
	FTNANDC024_8BIT(OFFSET_ECC_CORR_SP + u8_channel) &= ~(0x7F);
	FTNANDC024_8BIT(OFFSET_ECC_THRES_SP + u8_channel) &= ~(0x7F);

	// The max. correct capability for spare is 74bits.
	if(u8_ecc_correct_err_bit_for_spare > 74){
		u8_ecc_correct_err_bit_for_spare = 74;
	}

	if(u8_ecc_thres_bit_for_spare > 74){
		u8_ecc_thres_bit_for_spare = 74;
	}

	// Fill the field with the specified value
	FTNANDC024_8BIT(OFFSET_ECC_CORR_SP + u8_channel) |= ((UINT8)((u8_ecc_correct_err_bit_for_spare - 1) & 0x7F));
	FTNANDC024_8BIT(OFFSET_ECC_THRES_SP + u8_channel) |= ((UINT8)((u8_ecc_thres_bit_for_spare - 1) & 0x7F));
	return 1;
}

UINT8 NANDC024_ecc_occupy_for_sector(UINT8 u8_channel, UINT8 u8_flash_access_mode)
{
	UINT8 u8_ecc_correct_bits, u8_occupy_byte;
	u8_ecc_correct_bits = Report_ECC_capability_error(u8_channel);

	u8_occupy_byte = (u8_ecc_correct_bits * 14) / 8;
	if((u8_ecc_correct_bits * 14) % 8 != 0){
		u8_occupy_byte ++;
	}

	// The amount of data-payload in each (sector+sector_parity) or
	// (spare + spare_parity) on Toggle/ONFI mode must be even.
	if(u8_flash_access_mode == Toggle1_flash ||u8_flash_access_mode == Toggle2_flash ||
		u8_flash_access_mode == ONFI2_flash || u8_flash_access_mode == ONFI3_flash)
	{
		if(u8_occupy_byte & 0x1)
			u8_occupy_byte ++;
	}

	return u8_occupy_byte;
}

UINT8 NANDC024_interrupt_enable(UINT8 u8_channel, UINT8 u8_flash_status_failed)
{
	FTNANDC024_32BIT(OFFSET_INTR_EN) &= ~(1 << u8_channel);
	FTNANDC024_32BIT(OFFSET_INTR_EN) |= (UINT32) (u8_flash_status_failed << u8_channel);

	return 1;
}

UINT8 Status_failed(UINT8 u8_channel)
{
	return (((g_bootinfo.ftnandc024_status.u8_status_failed) >> u8_channel) & 0x1);
}

UINT8 Command_complete(UINT8 u8_channel)
{
	return (((g_bootinfo.ftnandc024_status.u8_command_complete) >> u8_channel) & 0x1);
}

UINT8 Command_queue_status_full(UINT8 u8_channel)
{
	if (((FTNANDC024_32BIT(OFFSET_CMDQ_STS) >> (u8_channel + 8)) & 0x1) == 1) {
		return 1;	// It's full
	} else {
		return 0;	// It's not full
	}
}

UINT8 Command_queue_flush(UINT8 u8_channel)
{
	FTNANDC024_32BIT(OFFSET_CMDQ_FLUSH) = (UINT32) (1 << u8_channel);
	// Wait until flash is ready!!
	while (!(FTNANDC024_32BIT(OFFSET_DEV_STS) & (1 << u8_channel)));

	return 1;
}

UINT8 Soft_reset(UINT8 u8_channel)
{
	FTNANDC024_32BIT(OFFSET_SW_RESET) = (UINT32) (1 << u8_channel);
	// Wait for the NANDC023 reset is complete
	while ((FTNANDC024_32BIT(OFFSET_SW_RESET) & (1 << u8_channel))) ;
	return 1;
}

UINT8 Memory_attribute_setting(struct flash_info *flash_readable_info)
{
	UINT8 u8_togbit, u8_pg_sz, u8_eD3_toshiba;
	UINT16 u16_flash_block_size, u16_block_boundary, u16_planesz;
	UINT32 u32_setting;

	u8_pg_sz		= (UINT8)(flash_readable_info->u16_page_size);
	u8_eD3_toshiba		= flash_readable_info->u8_eD3_toshiba;
	u16_flash_block_size	= flash_readable_info->u16_page_in_block;
	u16_block_boundary	= flash_readable_info->u16_page_boundary_in_block;

	if(u8_pg_sz == page_size_512byte)
		u8_pg_sz = 0;
	else if (u8_pg_sz == page_size_2kbyte)
		u8_pg_sz = 1;
	else if (u8_pg_sz == page_size_4kbyte)
		u8_pg_sz = 2;
	else if (u8_pg_sz == page_size_8kbyte)
		u8_pg_sz = 3;
	else if (u8_pg_sz == page_size_16kbyte) {
		if(u8_eD3_toshiba == 0)
			u8_pg_sz = 4;
		else if(u8_eD3_toshiba == 1)
			u8_pg_sz = 3;
	}

	u32_setting = FTNANDC024_32BIT(OFFSET_MEM_ATTR_SET1);
	// Clear and set the field of page size
	u32_setting &= ~(0x7 << 16);
	u32_setting |= ((u8_pg_sz & 0x7) << 16);
	// Clear and set the field of block size
	u32_setting &= ~(0x3FF << 2);
	u32_setting |= (((u16_block_boundary - 1) & 0x3FF) << 2);
	FTNANDC024_32BIT(OFFSET_MEM_ATTR_SET1) = u32_setting;

	u32_setting = FTNANDC024_32BIT(OFFSET_MEM_ATTR_SET2);
	u32_setting &= ~(0x3FF << 16);
	u32_setting |= ((u16_flash_block_size - 1) & 0x3FF) << 16;
	u32_setting &= ~(1 << 15);//disable dqs clk out
	FTNANDC024_32BIT(OFFSET_MEM_ATTR_SET2) = u32_setting;

	// Calculate the toggle bit from the num of pg in a plane
	if(u8_eD3_toshiba == 0) {
		u16_planesz = u16_block_boundary;
	} else {
		u16_planesz = flash_readable_info->u16_page_in_pseuplane;
	}

	u8_togbit = 0;
	do{
		if(u16_planesz & (1 << u8_togbit))
			break;
		u8_togbit ++;
	} while(u8_togbit < 24);

	FTNANDC024_32BIT(OFFSET_ADDR_TOGGLE_BIT) = (u8_togbit & 0x1F);

	return 1;
}

UINT8 Row_addr_cycle(UINT8 u8_cycle)
{
	// Clear the value setted before
	FTNANDC024_32BIT(OFFSET_MEM_ATTR_SET1) &= ~(0x3 << 13);
	// Set the value
	FTNANDC024_32BIT(OFFSET_MEM_ATTR_SET1) |= ((u8_cycle & 0x3) << 13);
	return 1;
}

UINT8 Column_addr_cycle(UINT8 u8_cycle)
{
	// Clear the value setted before
	FTNANDC024_32BIT(OFFSET_MEM_ATTR_SET1) &= ~(0x1 << 12);
	// Set the value
	FTNANDC024_32BIT(OFFSET_MEM_ATTR_SET1) |= ((u8_cycle & 0x1) << 12);
	return 1;
}

UINT8 Flash_AC_timing_setting_new(UINT8 u8_channel, struct flash_timing_setting_new * flash_timing)
{
	FTNANDC024_32BIT(OFFSET_AC_TIMING0(u8_channel)) = flash_timing->AC_Timing0;
	FTNANDC024_32BIT(OFFSET_AC_TIMING1(u8_channel)) = flash_timing->AC_Timing1;
	FTNANDC024_32BIT(OFFSET_AC_TIMING2(u8_channel)) = flash_timing->AC_Timing2;
	FTNANDC024_32BIT(OFFSET_AC_TIMING3(u8_channel)) = flash_timing->AC_Timing3;
	return 1;
}

UINT8 BMC_region_status_full(UINT8 u8_region_num)
{
	if (((FTNANDC024_32BIT(OFFSET_BMC_STS) >> (u8_region_num + 8)) & 0x1) == 1) {
		return 1;	// It's full
	} else {
		return 0;	// It's not full
	}
}

UINT8 BMC_region_status_empty(UINT8 u8_region_num)
{
	if (((FTNANDC024_32BIT(OFFSET_BMC_STS) >> u8_region_num) & 0x1) == 1) {
		return 1;	// It's empty
	} else {
		return 0;	// It's not empty
	}
}

UINT8 BMC_region_software_reset(UINT8 u8_region_num)
{
	FTNANDC024_32BIT(OFFSET_BMC_RESET) = (1 << u8_region_num);
	return 1;
}

UINT8 Report_AHBSlave_port_mode(UINT8 u8_port) {
	if(FTNANDC024_32BIT(OFFSET_FEATURE) & (1 << (27 + u8_port)))
		return AHB_Slave_Port_Async;
	else
		return AHB_Slave_Port_Sync;
}

UINT8 AHB_data_slave_reset(UINT8 u8_port_num) {

	FTNANDC024_32BIT(OFFSET_AHB_RST) |= 1 << (u8_port_num);

	while(FTNANDC024_32BIT(OFFSET_AHB_RST) & (1 << u8_port_num));
	return 1;
}

UINT8 AHB_slave_memory_space(UINT8 u8_ahb_slave_memory_size)
{

	FTNANDC024_8BIT(OFFSET_AHB_MEM) &= ~0xff;
	FTNANDC024_8BIT(OFFSET_AHB_MEM) |= u8_ahb_slave_memory_size;

	g_bootinfo.g_u32_ahb_memory_space = (u8_ahb_slave_memory_size << 9);//xt: why << 9

	return 1;
}

UINT8 Manual_command_queue_setting(UINT8 u8_channel, UINT32 * u32_manual_command_queue)
{
	FTNANDC024_32BIT(OFFSET_CMDQ1(u8_channel)) = *u32_manual_command_queue;
	FTNANDC024_32BIT(OFFSET_CMDQ2(u8_channel)) = *(u32_manual_command_queue + 1);
	FTNANDC024_32BIT(OFFSET_CMDQ3(u8_channel)) = *(u32_manual_command_queue + 2);
	FTNANDC024_32BIT(OFFSET_CMDQ5(u8_channel)) = *(u32_manual_command_queue + 3);
	FTNANDC024_32BIT(OFFSET_CMDQ6(u8_channel)) = *(u32_manual_command_queue + 4);
	return 1;
}

UINT8 Manual_command_feature_setting_and_fire(UINT8 u8_channel, UINT32 u32_manual_feature)
{
	FTNANDC024_32BIT(OFFSET_CMDQ4(u8_channel)) = u32_manual_feature;
	return 1;
}

static void ECC_failed_in_unmask_handler(UINT8 u8_channel, UINT8 u8_port_num, struct collect_data_buf *data_buf)
{

	// Step3. Reset the BMC region
	BMC_region_software_reset(data_buf->u8_data_region_num);
	// Step4. Reset the AHB slave
	if(Report_AHBSlave_port_mode(0) == AHB_Slave_Port_Async)
		AHB_data_slave_reset(0);
	// Step4. Flush Command queue & Poll whether Command queue is ready
	Command_queue_flush(u8_channel);
	// Step5. Reset Nandc & Poll whether the "Reset of NANDC" returns to 0
	Soft_reset(u8_channel);
}

static UINT8 ftnand_Start_PIO(UINT8 u8_Type, UINT8 u8_channel, UINT32 u32_transfer_bytes, UINT32 u32_buf_addr,
							UINT8 u8_data_region_num, struct flash_info *flash_readable_info)
{
	UINT8 u8_status = Trans_Ok;
	UINT32 u32_i, u32_j, u32_offset;

	UINT32 t0;

	u32_offset = g_bootinfo.g_u32_ahb_memory_space * u8_data_region_num;

	if(u8_Type == Trans_Read) {
		for (u32_i = 0; u32_i < (u32_transfer_bytes / flash_readable_info->u16_sector_size); u32_i++) {//xt: num of sector

			t0 = AV1_GetStc32();

			// Read the sector sized data from BMC when every time the bmc region isn't empty
			do {
			} while(BMC_region_status_empty(u8_data_region_num) && ((AV1_GetStc32() - t0) < (90 * 100)));

			if(!BMC_region_status_empty(u8_data_region_num)) {
				for(u32_j = 0; u32_j < (flash_readable_info->u16_sector_size >> 2); u32_j ++) {//sector_size/4 = u32 data
					*((UINT32 *)(u32_buf_addr)) = FTNANDC024_DATA_PORT(u32_offset);
					u32_buf_addr += 4;

					#ifndef FTNANDC024_Data_Port_Fixed
					{
						u32_offset += 4;
					}
					#endif
				} // for(u32_j)
			}
			else {
				u8_status = Trans_Err_Timeout;
				break;
			}
		} // for(u32_i)
	}
	else if (u8_Type == Trans_Write) {
		for (u32_i = 0; u32_i < (u32_transfer_bytes / flash_readable_info->u16_sector_size); u32_i++) {

			t0 = AV1_GetStc32();

			// Write the sector sized data from BMC except the bmc region is full.
			do {
				int temp = BMC_region_status_full(u8_data_region_num);
				prn_decimal_ln(temp);
			} while(BMC_region_status_full(u8_data_region_num) && ((AV1_GetStc32() - t0) < (90 * 100)));

			if(!BMC_region_status_full(u8_data_region_num)) {
				for(u32_j = 0; u32_j < (flash_readable_info->u16_sector_size >> 2); u32_j ++) {
					FTNANDC024_DATA_PORT(u32_offset) = *((UINT32 *)(u32_buf_addr));
					u32_buf_addr += 4;
					#ifndef FTNANDC024_Data_Port_Fixed
					{
						u32_offset += 4;
					}
					#endif
				} // for(u32_j)
			}
			else {
				u8_status = Trans_Err_Timeout;
				break;
			}
		} // for(u32_i)
	}
	else {
		prn_string("Warning: Transfer type is neither Read nor Write\n");
	}

	return u8_status;
}

UINT8 Data_read(UINT8 u8_channel, struct collect_data_buf *data_buf, UINT8 u8_data_presence, UINT8 u8_spare_presence,
		struct flash_info *flash_readable_info)
{
	UINT8  u8_ce_index, u8_status;
	UINT32 u32_i, u32_j;
	UINT32 u32_spare_cha_offs;
	UINT32 offset, u32_start_offset_in_bmc_region, *u32p_data_buf;

	if (u8_data_presence == 1) {

		if(data_buf->u8_user_mode == 0) {

			// copy data into *data_buf
			// data_buf.u16_data_length_in_sector and data_buf.u16_spare_length indicate length
			//offset = g_bootinfo.g_u32_ahb_memory_space * data_buf->u8_data_region_num;

			UINT32 u32_buf_addr;
			u32_buf_addr = (UINT32)data_buf->u8_data_buf;
			//xt: 从port读数据出来
			u8_status = ftnand_Start_PIO(Trans_Read, u8_channel, data_buf->u32_data_length_in_bytes,
						     u32_buf_addr, data_buf->u8_data_region_num, flash_readable_info);
			if((u8_status == Trans_Err_Timeout) && ECC_correction_failed(u8_channel)) {
				ECC_failed_in_unmask_handler(u8_channel, 0, data_buf);
			}

		}
		else{ // if(data_buf->u8_user_mode == 0) {
			u32p_data_buf = (UINT32 *)data_buf->u8_data_buf;
			// Every grib in BMC region is 512 bytes.
			u32_start_offset_in_bmc_region = data_buf->u8_user_mode_pointer * 512;
			if (FTNANDC024_32BIT(OFFSET_REV_NUM) >= 0x020400)
				offset = data_buf->u8_data_region_num * 0x8000 + u32_start_offset_in_bmc_region;
			else
				offset = data_buf->u8_data_region_num * 0x4000 + u32_start_offset_in_bmc_region;

			for (u32_i = 0; (u32_i < data_buf->u32_data_length_in_bytes >> 2); u32_i++) {
				*u32p_data_buf = FTNANDC024_32BIT(OFFSET_DATA_SRAM + (offset + (u32_i << 2 )));
				u32p_data_buf ++;
			}
		}
	}

	if (u8_spare_presence == 1) {
		if (FTNANDC024_32BIT(OFFSET_REV_NUM) >= 0x020400)
			u32_spare_cha_offs = u8_channel * 0x80;
		else
			u32_spare_cha_offs = u8_channel * 0x20;

		if(data_buf->u8_bytemode == Byte_Mode_Enable){
			for(u32_i = 0; u32_i < data_buf->u16_spare_length; u32_i ++){
				*((volatile UINT8 *)(data_buf->u8_spare_data_buf + u32_i)) = FTNANDC024_8BIT(OFFSET_SPARE_SRAM + u32_spare_cha_offs + u32_i);
			}
		}
		else {
			for(u32_i = 0, u8_ce_index = data_buf->u8_ce_num;
				u32_i < data_buf->u8_interleave_level;
				u32_i++, u8_ce_index = (u8_ce_index + data_buf->u8_ce_inc_mode) % FTNANDC024_MAX_CE)
			{
				for (u32_j = 0; u32_j < (data_buf->u16_spare_length); u32_j++) {
					*((volatile UINT8 *)(data_buf->u8_spare_data_buf + (u32_i * data_buf->u16_spare_length) + u32_j)) =
					    FTNANDC024_8BIT(((OFFSET_SPARE_SRAM + u32_spare_cha_offs) + u32_j));
				}
			}
		}
	}
	return 1;
}

UINT8 Check_command_status(UINT8 u8_channel)
{
	//prn_string("Entry Check_command_status\n");
	UINT8 u8_error_record = 0;
	// Poll the the status
	// The command is finished only in two situations:
	// 1. Command complete
	// 2. ECC failed and the ECC mask isn't enable. (It means the command complete doesnt' be triggered.)
	volatile UINT8 u8_ecc_correction_failed_for_data, u8_ecc_hit_threshold_for_data;
	volatile UINT8 u8_ecc_correction_failed_for_spare, u8_ecc_hit_threshold_for_spare;
	volatile UINT8 u8_status_failed, u8_command_complete;
	volatile UINT32 u32_command_complete_counter;

	u8_ecc_correction_failed_for_data = FTNANDC024_8BIT(OFFSET_ECC_INTR_STS);
	u8_ecc_hit_threshold_for_data = FTNANDC024_8BIT(OFFSET_ECC_INTR_STS + 1);
	u8_ecc_correction_failed_for_spare = FTNANDC024_8BIT(OFFSET_ECC_INTR_STS_SP);
	u8_ecc_hit_threshold_for_spare = FTNANDC024_8BIT(OFFSET_ECC_INTR_STS_SP + 1);
	u8_status_failed = FTNANDC024_8BIT(OFFSET_INTR_STATUS);
	u8_command_complete = FTNANDC024_8BIT(OFFSET_INTR_STATUS + 2);
	u32_command_complete_counter = FTNANDC024_32BIT(OFFSET_CMDCMPLT_CNT);

	do {

		if (u8_ecc_correction_failed_for_data) {
			g_bootinfo.ftnandc024_status.u8_ecc_correction_failed_for_data |= u8_ecc_correction_failed_for_data;

			FTNANDC024_8BIT(OFFSET_ECC_INTR_STS) |= u8_ecc_correction_failed_for_data;
		}

		if (u8_ecc_hit_threshold_for_data) {
			g_bootinfo.ftnandc024_status.u8_error_hit_threshold_for_data |= u8_ecc_hit_threshold_for_data;

			FTNANDC024_8BIT(OFFSET_ECC_INTR_STS + 1) |= u8_ecc_hit_threshold_for_data;
		}

		if (u8_ecc_correction_failed_for_spare) {
			g_bootinfo.ftnandc024_status.u8_ecc_correction_failed_for_spare |= u8_ecc_correction_failed_for_spare;

			FTNANDC024_8BIT(OFFSET_ECC_INTR_STS_SP) |= u8_ecc_correction_failed_for_spare;
		}

		if (u8_ecc_hit_threshold_for_spare) {
			g_bootinfo.ftnandc024_status.u8_error_hit_threshold_for_spare |= u8_ecc_hit_threshold_for_spare;

			FTNANDC024_8BIT(OFFSET_ECC_INTR_STS_SP + 1) |= u8_ecc_hit_threshold_for_spare;
		}

		if (u8_status_failed) {
			g_bootinfo.ftnandc024_status.u8_status_failed |= u8_status_failed;

			FTNANDC024_8BIT(OFFSET_INTR_STATUS) |= u8_status_failed;
		}

		if (u8_command_complete) {
			g_bootinfo.ftnandc024_status.u8_command_complete |= u8_command_complete;
			g_bootinfo.ftnandc024_status.u32_command_complete_counter = u32_command_complete_counter;

			FTNANDC024_8BIT(OFFSET_INTR_STATUS + 2) |= u8_command_complete;
		}
	}while(!(Command_complete(u8_channel)) && !(ECC_correction_failed(u8_channel) && !ECC_mask_status(u8_channel)));
	//prn_string("EXit while\n");
	if ((ECC_failed_hit_threshold(u8_channel)) == 1) {
		prn_string("Threshold Bits hit\n");
		u8_error_record |= Cmd_status_hit_threshold;
		// Clearing the record and interrupt
		g_bootinfo.ftnandc024_status.u8_error_hit_threshold_for_data &= ~(1 << u8_channel);
	}
	if ((ECC_failed_hit_threshold_for_spare(u8_channel)) == 1) {
		prn_string("Threshold Bits hit for spare\n");

		u8_error_record |= Cmd_status_hit_threshold_in_spare;
		// Clearing the record and interrupt
		g_bootinfo.ftnandc024_status.u8_error_hit_threshold_for_spare &= ~(1 << u8_channel);
	}


	if ((Command_complete(u8_channel)) == 1) {//xt: 为什么Command_complete和ECC_correction_failed_for_spare可以同时满足？
		u8_error_record |= Cmd_status_cmd_complete;
		// Clearing the record and interrupt
		g_bootinfo.ftnandc024_status.u8_command_complete &= ~(1 << u8_channel);

		if ((ECC_correction_failed_for_spare(u8_channel)) == 1) {
			prn_string("ECC correction for spare is failed\n");
			u8_error_record |= Cmd_status_ecc_correct_failed_in_spare;
			// Clearing the record and interrupt
			g_bootinfo.ftnandc024_status.u8_ecc_correction_failed_for_spare &= ~(1 << u8_channel);
		}
		if ((Status_failed(u8_channel)) == 1) {
			prn_string("Status is failed\n");
			u8_error_record |= Cmd_status_status_failed;
			// Clearing the record and interrupt
			g_bootinfo.ftnandc024_status.u8_status_failed &= ~(1 << u8_channel);
		}
		// It needs to clear the ECC_correction_failed when the ECC mask is enable.
		// Command complete signal will be set no matter whether the ECC correction is success when the ECC mask is enable.
		if(ECC_mask_status(u8_channel))
		{
			if ((ECC_correction_failed(u8_channel)) == 1) {
				prn_string("ECC correction is failed, but it's masked\n");
				u8_error_record |= Cmd_status_ecc_correct_failed;
				g_bootinfo.ftnandc024_status.u8_ecc_correction_failed_for_data &= ~(1 << u8_channel);
			}
		}
	}
	else {
		if ((ECC_correction_failed(u8_channel)) == 1) {
			prn_string("ECC correction is failed\n");
			u8_error_record |= Cmd_status_ecc_correct_failed;
			// Clearing the record and interrupt
			g_bootinfo.ftnandc024_status.u8_ecc_correction_failed_for_data &= ~(1 << u8_channel);
		}
	}
	return u8_error_record;

}

////////////////////////////////////////    FIXFLOW  HAL       ///////////////////////////////////////////////
UINT8 Setting_feature_and_fire(UINT8 u8_channel, UINT16 u16_command_index, UINT8 u8_starting_ce, UINT32 u32_sixth_word)
{
	UINT32 u32_cmd_queue_content;

	u32_cmd_queue_content = (u8_starting_ce << 29) | (u16_command_index << 8) | u32_sixth_word;
	// Checking the command queue isn't full
	while (Command_queue_status_full(u8_channel));

#ifdef xtdebug
	prn_string("CMDQ1:");
	prn_dword(FTNANDC024_32BIT(OFFSET_CMDQ1(u8_channel)));
	prn_string("CMDQ2:");
	prn_dword(FTNANDC024_32BIT(OFFSET_CMDQ2(u8_channel)));
	prn_string("CMDQ3:");
	prn_dword(FTNANDC024_32BIT(OFFSET_CMDQ3(u8_channel)));
	prn_string("CMDQ5:");
	prn_dword(FTNANDC024_32BIT(OFFSET_CMDQ5(u8_channel)));
	prn_string("CMDQ6:");
	prn_dword(FTNANDC024_32BIT(OFFSET_CMDQ6(u8_channel)));

	prn_string("Cmd index:");
	prn_dword(u16_command_index);
	prn_string("feature:");
	prn_dword(u32_cmd_queue_content);
#endif
	Manual_command_feature_setting_and_fire(u8_channel, u32_cmd_queue_content);

	return 1;
}

UINT8 Page_read_setting(UINT8 u8_channel, UINT32 u32_row_addr, UINT8 u8_sector_offset,
		       struct collect_data_buf * data_buf)
{
	UINT32 u32_command_queue[5] = { 0, 0, 0, 0, 0};

	Row_addr_cycle(Row_address_3cycle);
	Column_addr_cycle(Column_address_2cycle);

	u32_command_queue[0] |= (u32_row_addr & 0xFFFFFF);
	u32_command_queue[1] |= 0;

	u32_command_queue[2] |= u8_sector_offset;
	u32_command_queue[2] |= ((data_buf->u16_data_length_in_sector) << 16);
	Manual_command_queue_setting(u8_channel, u32_command_queue);

	return 1;
}