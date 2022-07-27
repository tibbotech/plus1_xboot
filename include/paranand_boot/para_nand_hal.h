#ifndef PARA_NAND_HAL_H
#define PARA_NAND_HAL_H

#include <paranand_boot/para_nand.h>

// ===================================== Function declaration =====================================
/*
	Function Name	: ECC_spare_error_status

	Effect 			: Return the the num of error bits in the specified channel

	Parameter		:
		u8_channel	--> Channel ID

	Return value	:
		0		--> No ecc error happends in the specified channel
		positive	--> Number of ecc error bit happends in the channel
*/
UINT8 ECC_spare_error_status(UINT8 u8_channel);

/*
	Function Name	: ECC_error_status

	Effect 			: Return the ecc error status in the specified channel

	Parameter		:
		u8_channel	--> Channel ID

	Return value	:
		0		--> No ecc error happends in the specified channel
		positive	--> Number of ecc error bit happends in the channel
		negative	--> Function failed
*/
UINT8 ECC_error_status(UINT8 u8_channel);

/*
	Function Name	: ECC_clear_error_status

	Effect 			: Clear the ecc error status in the specified channel

	Parameter		:
		u8_channel	--> Channel ID

	Return value	:
		Null
*/
void ECC_clear_error_status(UINT8 u8_channel);

/*
	Function Name	: ECC_clear_spare_error_status

	Effect 			: Clear the ecc error status for spare in the specified channel

	Parameter		:
		u8_channel	--> Channel ID

	Return value	:
		Null
*/
void ECC_clear_spare_error_status(UINT8 u8_channel);

/*
	Function Name	: ECC_threshold_ecc_error

	Effect 			: Configurate the threshold number of ECC error bit for specified channel

	Parameter		:
		u8_channel		--> Channel ID
		u8_threshold_num	--> The threshold number of ECC error bit
					1bit	: 5'b00000
					2bit	: 5'b00001
					...(on the analogy of this)
					24bit	: 5'b10111
	Return value	:
		1		--> Function success
		0		--> Function fail
*/
UINT8 ECC_threshold_ecc_error(UINT8 u8_channel, UINT8 u8_threshold_num);

/*
	Function Name	: ECC_correct_ecc_error

	Effect 			: Configurate the capability of ECC correction for specified channel

	Parameter		:
		u8_channel		--> Channel ID
		u8_correct_num		--> The upper bound of ecc correction
					6bit	: 3'b000
					10bit	: 3'b001
					12bit	: 3'b010
					13bit	: 3'b011
					14bit	: 3'b100
					16bit	: 3'b101
					17bit	: 3'b110
					24bit	: 3'b111
	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 ECC_correct_ecc_error(UINT8 u8_channel, UINT8 u8_correct_num);

/*
	Function Name	: Report_ECC_correct_err_error

	Effect 			: Return the capability of ECC correction.

	Parameter		:
		u8_channel		--> Channel ID

	Return value	:
		ECC capability for data
*/
UINT8 Report_ECC_capability_error(UINT8 u8_channel);

/*
	Function Name	: ECC_setting_base_size

	Effect 			: Configurate the ECC base size

	Parameter		:
		u8_ecc_base_size 	--> ECC unit
					0: 512 bytes
					1: 1K  bytes
	Return value	:
		1		--> Function success
		0		--> Function fail
*/
UINT8 ECC_setting_base_size(UINT8 u8_ecc_base_size);

/*
	Function Name	: ECC_leave_space

	Effect			: Insert the ecc parity sector by sector.

	Parameter		:

		u8_choice	--> Enable ECC function for specified channel
				0: ECC exist within the interval of sectors
				1: ECC parity doesn't exist the interval of sectors.
	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 ECC_leave_space(UINT8 u8_choice);

/*
	Function Name	: ECC_function

	Effect 			: Enable ECC function

	Parameter		:
		u8_channel	--> Channel ID
		u8_choice	--> Enable ECC function for specified channel
				0: disable
				1: enable
	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 ECC_function(UINT8 u8_channel, UINT8 u8_choice);

/*
	Function Name	: ECC_mask

	Effect 			: Mask the ECC detection

	Parameter		:
		u8_channel	--> Channel ID
		u8_mask 	--> Mask ECC detection
				0: unmask
				1: mask
	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 ECC_mask(UINT8 u8_channel, UINT8 u8_mask);

/*
	Function Name	: ECC_mask_status

	Effect 			: Report the ECC mask status

	Parameter		:
		u8_channel	--> Channel ID

	Return value	:
		1	--> Mask ECC correction failed
		0	--> No mask ECC correction failed
*/
UINT8 ECC_mask_status(UINT8 u8_channel);

/*
	Function Name	: ECC_enable_interrupt

	Effect 			: Enable the ECC interrupt

	Parameter		:
		u8_ecc_error_bit_threhold-> Switch ECC threshold error interrupt
									0:disable
									1:enable
		u8_ecc_correct_failed	--> Switch ECC correct failed interrupt
									0:disable
									1:enable
		u8_ecc_error_bit_threhold_for_spare --> Switch "ECC for spare area" threshold error interrupt
									0:disable
									1:enable
		u8_ecc_correct_failed_for_spare		--> Switch "ECC for spare area" correct failed interrupt
									0:disable
									1:enable

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 ECC_enable_interrupt(UINT8 u8_ecc_error_bit_hit_the_threshold, UINT8 u8_ecc_correct_failed,
			  UINT8 u8_ecc_error_bit_hit_the_threshold_for_spare, UINT8 u8_ecc_correct_failed_for_spare);


/*
	Function Name	: para_nand_general_setting_for_all_channel

	Effect 			: Configurate the NANDC023 control register

	Parameter		:
		u8_flash_write_protection		--> Flash write protect pin control
		u8_data_inverse					--> Data inverse enable
		u8_scrambler					--> Data scrambler mode enable
		u8_device_busy_ready_status_bit_location--> Device busy/ready status bit location on Flash data bus
								3'b000 ~3'b111 : bit 0 ~ bit 7

		u8_command_pass_failed_status_bit_location> Command pass/fail status bit location on Flash data bus
								3'b000 ~3'b111 : bit 0 ~ bit 7

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 para_nand_general_setting_for_all_channel(UINT8 u8_flash_write_protection,
					      UINT8 u8_data_inverse, UINT8 u8_scrambler,
					      UINT8 u8_device_busy_ready_status_bit_location,
					      UINT8 u8_command_pass_failed_status_bit_location);

/*
	Function Name	: para_nand_chip_num

	Effect 			: Setting the numbers of ce in channel

	Parameter		:
		u8_chip_num	--> chip number in each channel

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 para_nand_chip_num(UINT8 u8_chip_num);

/*
	Function Name	: para_nand_spare_ecc_setting

	Effect 			: ECC setting for spare area

	Parameter		:
		u8_channel							--> Channel
		u8_ecc_correct_err_bit_for_spare	--> Correct bit of ECC
		u8_ecc_thres_bit_for_spare			--> Threahole of ECC
	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 para_nand_spare_ecc_setting(UINT8 u8_channel,
				UINT8 u8_ecc_correct_err_bit_for_spare,
				UINT8 u8_ecc_thres_bit_for_spare);

/*
	Function Name	: para_nand_ecc_occupy_for_sector

	Effect 			: Return how many bits does ecc occupied for data

	Parameter		:
		u8_channel				--> channel
		u8_flash_access_mode	--> flash access mode
	Return value	:
		Bytes needs to be supplied for ecc protection
*/
UINT8 para_nand_ecc_occupy_for_sector(UINT8 u8_channel, UINT8 u8_flash_access_mode);

UINT8 para_nand_interrupt_enable(UINT8 u8_channel, UINT8 u8_flash_status_failed);

/*
	Function Name	: Command_queue_status_full

	Effect 			: Return the full status of command queue in specified channel

	Parameter		:
		u8_channel	--> Channel ID

	Return value	:
		1	--> Command queue is full
		0	--> Command queue isn't full
*/
UINT8 Command_queue_status_full(UINT8 u8_channel);

/*
	Function Name	: Command_queue_flush

	Effect 			: Flush the flash in specified channel

	Parameter		:
		u8_channel	--> Channel ID

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Command_queue_flush(UINT8 u8_channel);

/*
	Function Name	: Soft_reset

	Effect 			: Software reset in specified channel

	Parameter		:
		u8_channel	--> Channel ID

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Soft_reset(UINT8 u8_channel);

/*
	Function Name	: Memory_attribute_setting

	Effect 			: Setting the memory attribute

	Parameter		:
		flash_readable_info		--> flash information

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Memory_attribute_setting(struct flash_info *flash_readable_info);

/*
	Function Name	: Row_addr_cycle

	Effect 			: Setting row address cycle for fetching

	Parameter		:
		u8_cycle	--> cycle number(using the define in ftnandc023.h)

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Row_addr_cycle(UINT8 u8_cycle);

/*
	Function Name	: Column_addr_cycle

	Effect 			: Setting column address cycle for fetching

	Parameter		:
		u8_cycle	--> cycle number(using the define in ftnandc023.h)

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Column_addr_cycle(UINT8 u8_cycle);

/*
	Function Name	: Flash_AC_timing_setting

	Effect 			: Setting the AC timing of flash for specified channel

	Parameter		:
		u8_channel		--> Channel ID
		flash_timing	--> Struct to store the several timing feature

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Flash_AC_timing_setting(UINT8 u8_channel, struct flash_timing_setting *flash_timing);
UINT8 Flash_AC_timing_setting_new(UINT8 u8_channel, struct flash_timing_setting_new * flash_timing);

/*
	Function Name	: BMC_region_status_full

	Effect 			: Return the full status of specified BMC region

	Parameter		:
		u8_region_num		--> Region ID

	Return value	:
		1	--> The specified BMC region is full
		0	--> It's not full.
*/
UINT8 BMC_region_status_full(UINT8 u8_region_num);

/*
	Function Name	: BMC_region_status_empty

	Effect 			: Return the empty of specified BMC region

	Parameter		:
		u8_region_num		--> Region ID

	Return value	:
		1	--> The specified BMC region is empty
		0	--> It's not empty
*/
UINT8 BMC_region_status_empty(UINT8 u8_region_num);

/*
	Function Name	: BMC_region_software_reset

	Effect 			: Software reset for specified region

	Parameter		:
		u8_region_num	--> Region ID

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 BMC_region_software_reset(UINT8 u8_region_num);

/*
	Function Name	: Report_AHBSlave_port_mode

	Effect 			: Report AHB Slave port is Synchronous or Asynchronous mode.

	Parameter		: u8_port_num --> Slave port number
	Return value	:
		1	--> AHB_Slave_Port_Async
		0	--> AHB_Slave_Port_Sync
*/
UINT8 Report_AHBSlave_port_mode(UINT8 u8_port_num);

/*
	Function Name	: AHB_data_slave_reset

	Effect 			: Reset AHB slave data port.

	Parameter		: u8_port_num --> Slave port number
	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 AHB_data_slave_reset(UINT8 u8_port_num);

/*
	Function Name	: AHB_slave_memory_space

	Effect 			: Setting the memory space for AHB slave data port.

	Parameter		:  u8_ahb_slave_memory_size --> AHB slave data port size
								3'b000: 512 bytes
								3'b001: 1K bytes
								3'b010: 2K bytes
								3'b011: 4K bytes
								3'b100: 8K bytes
								3'b101: 16K bytes
								3'b110: 32K bytes
								3'b111: 64K bytes
	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 AHB_slave_memory_space(UINT8 u8_ahb_slave_memory_size);

/*
	Function Name	: Manual_command_queue_setting

	Effect 			: Setting the command queue for specified channel.
					  It's used when you set "Program Flow selection" to 1 in the command queue.

	Parameter		:
		u8_channel		--> Channel ID
		u32_manual_command_queue--> Accept the addr. of array which is used to setting the first five words in command queue.
	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Manual_command_queue_setting(UINT8 u8_channel, UINT32 * u32_manual_command_queue);

/*
	Function Name	: Manual_command_feature_setting_and_fire

	Effect 			: Setting the feature of manual command for specified channel.

	Parameter		:
		u8_channel			--> Channel ID
		u32_manual_feature		--> The content of sixth word in command queue
	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Manual_command_feature_setting_and_fire(UINT8 u8_channel, UINT32 u32_manual_feature);

/*
	Function Name	: Data_read

	Effect 			: Fetch the data from flash after read command is fired.

	Parameter		:
		u8_channel		    --> Channel ID
		data_buf		    --> The struture to store the data from flash
		flash_readable_info --> The structure of flash info
		u8_data_presence	-->
					1: Read data from flash
					0: No data from flash
		u8_spare_presence	-->
					1: Read spare data from flash
					0: No spare data from flash

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Data_read(UINT8 u8_channel, struct collect_data_buf *data_buf, UINT8 u8_data_presence, UINT8 u8_spare_presence,
		struct flash_info *flash_readable_info);

/*
	Function Name	: Check_command_status

	Effect 			: Checking the command is complete or failed.

	Parameter		:
		u8_channel	--> Channel ID

	Return value	:
		1	--> Command is complete
		0	--> Command is failed
*/
UINT8 Check_command_status(UINT8 u8_channel);

/*
	Function Name	: Setting_feature_and_fire

	Effect 			: Setting the transfer feature and then fire

	Parameter		:
		u8_channel			--> Channel ID
		u8_command_index		--> Specified command index
		u32_sixth_word			--> The sixth word value
	Note			: All features are assigned by the pre-define characeters in ftnandc024.h except the parameter of u8_channel
	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Setting_feature_and_fire(UINT8 u8_channel, UINT16 u16_command_index, UINT8 u8_starting_ce, UINT32 u32_sixth_word);

/*
	Function Name	: Page_read_setting
	Effect 			: Reading the specified page from flash

	Parameter		:
		u8_channel		--> Channel ID
		u32_row_addr		--> Row addr
		u8_sector_offset	--> Column addr.
		data_buf		--> The data buf for storing data from flash

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Page_read_setting(UINT8 u8_channel, UINT32 u32_row_addr, UINT8 u8_sector_offset,
		        struct collect_data_buf * data_buf);
#ifdef TEST_NAND
/*
	Function Name	: Data_write

	Effect 			: Put the data to flash after read command is fired.

	Parameter		:
		u8_channel		    --> Channel ID
		data_buf		    --> The structure to store the data from flash
		flash_readable_info --> The structure of flash info
		u8_data_presence	-->
					1: Write data to flash
					0: No data to flash
		u8_spare_presence	-->
					1: Write spare data to flash
					0: No spare data to flash

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Data_write(UINT8 u8_channel, struct collect_data_buf *data_buf, struct flash_info *flash_readable_info,
                UINT8 u8_data_presence, UINT8 u8_spare_presence);

/*
	Function Name	: Page_write_setting

	Effect 			: Writing the data area into the flash

	Parameter		:
		u8_channel			--> Channel ID
		u32_row_addr		--> Row addr.
		u8_sector_offset	-->	Column addr.
		data_buf			--> The data buf for storing data to write

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Page_write_setting(UINT8 u8_channel, UINT32 u32_row_addr, UINT8 u8_sector_offset,
			 struct collect_data_buf * data_buf);

/*
	Function Name	: Read_flash_ID_setting
	Effect 			: Read flash ID
	Parameter		:
		u8_channel			--> Channel ID

	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Read_flash_ID_setting(UINT8 u8_channel);

/*
	Function Name	: Block_earse_setting

	Effect 			: Erasing the specific block

	Parameter		:
		u8_channel				--> Channel ID
		u32_row_addr			--> Row addr
		u16_block_cnt			--> Number of blocks to be erased
	Return value	:
		1	--> Function success
		0	--> Function fail
*/
UINT8 Block_erase_setting(UINT8 u8_channel, UINT32 u32_row_addr, UINT16 u16_block_cnt);

#endif
#endif
