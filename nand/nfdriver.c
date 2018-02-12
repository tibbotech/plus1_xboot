#include <nand_boot/nfdriver.h>
#include <nand_boot/nandop.h>
#include <nand_boot/hal_nand_error.h>
#include <regmap.h>
#include <common.h>


/**************************************************************************
 *                             M A C R O S                                *
 **************************************************************************/
#define NANDREG_W(reg, value)	Xil_Out32(reg, value)
#define NANDREG_R(reg) 	    	Xil_In32(reg)

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
void hwNandCmdSend(UINT8 which_cs, UINT8 cmd);
void hal_nand_set_rdy_pin_enable(UINT32 enable);
UINT32 hal_nand_controller_get_op_mode(void);
void hal_nand_controller_set_op_mode(UINT32 op_mode);
UINT32 hal_nand_get_rdy_pin_enable(void);

extern UINT32 Load_Header_Profile(SINT32 type);
extern UINT32 GetNANDACReadTiming(void);
extern UINT32 GetNANDACWriteTiming(void);
extern void setEccMode_op(UINT8 mode);
extern UINT8 getSDevinfo(UINT8 flg);
extern void Xil_Out32(unsigned int OutAddress, unsigned int Value);
extern unsigned int Xil_In32(unsigned int Addr);
extern UINT32 nfvalshift(UINT32 x);
extern void setSystemPara(UINT8 *prData);

void hal_nand_desc_set_addr(desc_command_t *p_desc_command, UINT32 rowAddr, UINT32 colAddr, UINT32 nrAddr, UINT32 option)
{
	UINT32 addr;

	p_desc_command->u32_3.field.addr_number = (nrAddr) ? nrAddr - 1 : 0;
	if (p_desc_command->u32_0.field.cmd_type == CMD_TYPE_SUPER_MANUAL_MODE) {
		p_desc_command->u32_0.field_super_cmd_ctl.have_addr = (nrAddr == 0) ? 0 : 1;
	}

	if (option & ( HAL_NAND_ADDR_COL_ONLY | HAL_NAND_ADDR_ROW_ONLY) ) {
		addr = (option & ( HAL_NAND_ADDR_COL_ONLY) ) ? colAddr : rowAddr;

		if (nrAddr <= 3) {
			p_desc_command->u32_4.field.addr_2 = addr & 0xff;
			p_desc_command->u32_4.field.addr_3 = (addr >> 8) & 0xff;
			p_desc_command->u32_3.field.addr_4 = (addr >> 16) & 0xff;
		} else {
			p_desc_command->u32_4.field.addr_0 = addr & 0xff;
			p_desc_command->u32_4.field.addr_1 = (addr >> 8) & 0xff;
			p_desc_command->u32_4.field.addr_2 = (addr >> 16) & 0xff;
			p_desc_command->u32_4.field.addr_3 = (addr >> 24) & 0xff;
		}
	} else {
		if (option & HAL_NAND_ADDR_COL_1_BYTE) {
			if (nrAddr <= 3) {
				p_desc_command->u32_4.field.addr_2 = colAddr & 0xff;
				p_desc_command->u32_4.field.addr_3 = rowAddr & 0xff;
				p_desc_command->u32_3.field.addr_4 = (rowAddr >> 8) & 0xff;
			} else {
				p_desc_command->u32_4.field.addr_0 = colAddr & 0xff;
				p_desc_command->u32_4.field.addr_1 = rowAddr & 0xff;
				p_desc_command->u32_4.field.addr_2 = (rowAddr >> 8) & 0xff;
				p_desc_command->u32_4.field.addr_3 = (rowAddr >> 16) & 0xff;
				p_desc_command->u32_3.field.addr_4 = (rowAddr >> 24) & 0xff;
			}
		} else {
			p_desc_command->u32_4.field.addr_0 = colAddr & 0xff;
			p_desc_command->u32_4.field.addr_1 = (colAddr >> 8) & 0xff;

			p_desc_command->u32_4.field.addr_2 = rowAddr & 0xff;
			p_desc_command->u32_4.field.addr_3 = (rowAddr >> 8) & 0xff;
			p_desc_command->u32_3.field.addr_4 = (rowAddr >> 16) & 0xff;
			p_desc_command->u32_3.field.addr_5 = (rowAddr >> 24) & 0xff;
		}
	}
}

void hal_nand_desc_set_redunt(desc_command_t *p_desc_command, UINT32 redunt_length, void *redunt_bp, UINT32 redunt_sector_address_offset)
{
	p_desc_command->u32_3.field.redunt_enable = (redunt_length) ? 1 : 0;
	p_desc_command->u32_1.field.redunt_length = (redunt_length) ? redunt_length-1 : 0;
	p_desc_command->u32_6.field.redunt_bp = (UINT32)redunt_bp;
	p_desc_command->u32_7.field.redunt_sector_address_offset = redunt_sector_address_offset;
}

void hal_nand_desc_set_payload(desc_command_t *p_desc_command, UINT32 payload_length, void *payload_bp)
{
	// To allow reading page > nand buffer size, limit read size.
	// If Page Size > 16KB, we only read 16KB!
	if (payload_length > (16 * 1024)) {
		payload_length = 16 * 1024;
	}

	p_desc_command->u32_1.field.payload_length = (payload_length) ? payload_length-1 : 0;
	p_desc_command->u32_5.field.payload_bp = (UINT32)payload_bp;
}

void hal_nand_desc_set_interrupt_mask(desc_command_t *p_desc_command, UINT32 interrupt_mask)
{
	p_desc_command->u32_2.field.interrupt_status = 0;
	p_desc_command->u32_2.field.interrupt_mask = interrupt_mask;
}

void hal_nand_desc_set_command(desc_command_t *p_desc_command, UINT8 nrCmd, UINT8 cmd_0, UINT8 cmd_1)
{
	p_desc_command->u32_0.field.cmd_0 = cmd_0;
	p_desc_command->u32_0.field.cmd_1 = cmd_1;
	if (p_desc_command->u32_0.field.cmd_type == CMD_TYPE_SUPER_MANUAL_MODE) {
		p_desc_command->u32_0.field_super_cmd_ctl.is_two_command = (nrCmd == 2) ? 1 : 0;
	}
}

void hal_nand_clear_interrupt(UINT32 intr_sts)
{
	UINT32 k;

	for (k=0; k < 20; k++) {
		NANDREG_W(FM_INTR_STS, intr_sts);
		if ((NANDREG_R(FM_INTR_STS) & intr_sts) == 0) {
			if (k > 1) {
				DBGPRINT("interrupt clear after [%d] time\n", k);
			}
			return;
		}
	}
	DBGPRINT("nand warning:interrupt not clear after %d time\n", k);
}

void hal_nand_set_interrupt_mask(UINT32 interrupt_mask)
{
	NANDREG_W(FM_INTRMSK, interrupt_mask);
}

UINT32 hal_nand_get_interrupt_mask(void)
{
	return NANDREG_R(FM_INTRMSK);
}

#if 0
void hal_nand_dump_desc(char *message)
{
	UINT32 k = 0;

	p_desc_comm = (desc_command_t *)&g_DescInfo[0];

	printk(message);

	while (1)
	{
		dump_buffer(p_desc_comm, sizeof(desc_command_t));
		if (p_desc_comm->u32_3.field.end_desc == 1)
		{
			if (p_desc_comm->u32_7.field.next_desc_offset != 0)
			{
				DBGPRINT("end_desc is 1, but next_desc_offset != 0\n");
			}
			break;
		}
		p_desc_comm++;
		k++;
		if (k >= MAX_DESC_SIZE)
		{
			DBGPRINT("didn't found end_desc is 1\n");
		}
	}
	printk("\n");
}
#endif

void hal_nand_run_descriptor(UINT32 desc_base_addr)
{
#if defined(HAL_NAND_DUMP_DESC)
	printk("\ndump desc:\n");
	dump_buffer((UINT8 *)desc_base_addr, sizeof(desc_command_t));
	printk("\n");
#endif

	NANDREG_W(FM_DESC_BA, desc_base_addr);
	NANDREG_W(FM_CSR, NANDREG_R(FM_CSR) | 0x10);
}

int hal_nand_desc_basic_trigger(void)
{
	UINT32 k;

	CSTAMP(0xAA55BA00); // wait 1 before trigger
	k = 0;
	while (NANDREG_R(FM_INTR_STS) & ND_INTR_DESC_NFC_BUSY) {
		DBGPRINT("nand controller is busy sts[0x%08x]k[%d]\n",NANDREG_R(FM_INTR_STS), k);

		k++;
		if (k > MAX_NAND_WAIT_CNT) {
			//hal_nand_dump_controller_debug_regs();
			prn_string("nand trigger busy timeout\n");
			CSTAMP(0xAA55BAE0); // wait 1 failed
			break;
		}
	}

	CSTAMP(0xAA55BA01); // wait 2 before trigger
	k = 0;
	while (NANDREG_R(FM_INTR_STS)) {
		k++;
		hal_nand_clear_interrupt(0xffff);
		if (NANDREG_R(FM_INTR_STS)) {
			DBGPRINT( "nand controller is still busy sts[0x%08x]\n",NANDREG_R(FM_INTR_STS));
			if (k > MAX_NAND_WAIT_CNT) {
				//hal_nand_dump_controller_debug_regs();
				CSTAMP(0xAA55BAE1); // wait 2 failed
				prn_string("nand trigger timeout\n");
				//break;
				return ROM_FAIL;
			}
		}
	}

	CSTAMP(0xAA55BA02); // trigger cmd
	hal_nand_run_descriptor((UINT32) &g_io_buf.nand.descInfo[0]);
	CSTAMP(0xAA55BA03); // end trigger cmd

	return ROM_SUCCESS;
}

SINT32 hal_nand_polling_desc_done(UINT32 ms)
{
	UINT32 loop_cnt = 0;
	//UINT32 loop_timeout_cnt = 0x140000; // ~608ms on CA9
	UINT32 loop_timeout_cnt = 0x8A000; // ~262ms on CA9 500MHz

	volatile SINT32 r_nand_INTR_STS;

	//dbg();
	//STC_REG->stc_15_0 = 0;
	while (1) {
		r_nand_INTR_STS = NANDREG_R(FM_INTR_STS);

		if ((r_nand_INTR_STS & g_bootinfo.g_int_wake_up_flag) == g_bootinfo.g_int_wake_up_flag) {

			hal_nand_clear_interrupt(0xffff);
			return loop_timeout_cnt - loop_cnt;

		}

		loop_cnt++;

		if (loop_cnt>=loop_timeout_cnt) {
			//prn_decimal(STC_REG->stc_15_0/90); prn_string("ms");
			DBGPRINT("dump_intr_sts nStatus[0x%08x]int_wake_up_flag[0x%08x]rFM_INTRMSK[0x%08x]\n", r_nand_INTR_STS, g_bootinfo.g_int_wake_up_flag, NANDREG_R(FM_INTRMSK));
			DBGPRINT("%s, polling time out[%u].\n",  __FUNCTION__, 0x140000);

			hal_nand_clear_interrupt(0xffff);
			dbg();
			break;
		}
	} // end while

	return 0;
}

SINT32 hal_nand_check_nfc_finish_last_desc(UINT8 control)
{
	UINT32 j;
	SINT32 ret = HAL_NAND_SUCCESS;
	UINT32 timeout_cnt = 1000;

	if ((control & (1<<1)))
		timeout_cnt = 10;

	while (1) {
		if (p_desc_comm->u32_3.field.end_desc == 1) {
			j = 0;
			while (1) {
				//hal_nand_sync_work_buf_for_cpu(&p_hal_nand_control_data->desc_work_buf);
				if (p_desc_comm->u32_3.field.owner == 0) {
					if (j > 4)
					{
						DBGPRINT("owner bit become 0 after [%d].command[%02x][%02x]\n", j, p_desc_comm->u32_0.field.cmd_0, p_desc_comm->u32_0.field.cmd_1);
					}
					break;
				}
				//hal_nand_sync_work_buf_for_device(&p_hal_nand_control_data->desc_work_buf);
				j++;
				if (j >= timeout_cnt) {
					//hal_nand_sync_work_buf_for_cpu(&p_hal_nand_control_data->desc_work_buf);
					DBGPRINT("end_desc is 1, but owner bit is 1.[%d]\n", j);
					ret = -HAL_NAND_ERR_CONTROLLER_BUSY;
					if ((control & (1<<0)) == 0)
					{
						//hal_nand_dump_controller_debug_regs();
					}
					break;
				}
			} // end while
			if (p_desc_comm->u32_7.field.next_desc_offset != 0)
			{
				DBGPRINT("end_desc is 1, but next_desc_offset != 0\n");
			}
			break; // break out outest while loop
		}
	} // end while
	return ret;
}

SINT32 hal_nand_desc_wait_complete(UINT32 which_ch, UINT8 use_poll, UINT32 timeout_ms, UINT8 control)
{
	UINT8 do_controller_busy_check = 1;
	SINT32 ret, left_ticks;


	left_ticks = hal_nand_polling_desc_done(timeout_ms);

	if (left_ticks == 0) {
		prn_string("wait next time out\n");
		ret = -HAL_NAND_ERR_TIME_OUT;
		do_controller_busy_check = 1;
	} else if (left_ticks < 0) {
		prn_string("wait next aborted\n");
		ret = -HAL_NAND_ERR_ABORTED;
	} else {
		ret = HAL_NAND_SUCCESS;
	}

	if (do_controller_busy_check) {
		UINT32 time_out_cnt, k;

		time_out_cnt = 500;
		for (k=0; k< time_out_cnt; k++) {
			if ((NANDREG_R(FM_INTR_STS) & ND_INTR_DESC_NFC_BUSY) == 0) {
				break;
			}
		}

		if (k == time_out_cnt) {
			if (ret == HAL_NAND_SUCCESS)
				ret = -HAL_NAND_ERR_CONTROLLER_BUSY;
		}
	}

	/* since NFC have updated gp_Desc[DESC_CMD] without inform cpu, do invalidate here to
	   force cpu to read from dram instead of cpu write back buffer, then we will get correct value.  */

	// TODO: check descriptor owner bit for real idle
	if (ret == HAL_NAND_SUCCESS) {
		ret = hal_nand_check_nfc_finish_last_desc(control);
	} else {
		hal_nand_check_nfc_finish_last_desc(control);
	}

	return ret;
}

UINT32 hal_nand_set_wake_up_flag(desc_command_t *p_desc_command, UINT32 wake_up_flag)
{
	UINT32 old_interrupt_mask;

	old_interrupt_mask = hal_nand_get_interrupt_mask();

	g_bootinfo.g_int_wake_up_flag = wake_up_flag;
	hal_nand_desc_set_interrupt_mask(p_desc_command, g_bootinfo.g_int_wake_up_flag);
	hal_nand_set_interrupt_mask(wake_up_flag);

	return old_interrupt_mask;
}

#if 0 // unused
SINT32 hal_nand_base_read_status(UINT8 which_cs, UINT8 *p_status_byte)
{
	int ret;

	p_desc_comm = (desc_command_t *)&g_io_buf.nand.descInfo[0];
	rd_memset((UINT8 *)p_desc_comm, 0x00, sizeof(desc_command_t));

	p_desc_comm->u32_0.field_read_status_set.cmd_0 = 0x70;
	//p_desc_comm->u32_0.field_read_status_set.cmd_1 = 0x00;
	//p_desc_comm->u32_0.field_read_status_set.have_addr = 0;
	//p_desc_comm->u32_0.field_read_status_set.reserve = 0;
	p_desc_comm->u32_0.field_read_status_set.multi_func_sel = (1<<which_cs);
	p_desc_comm->u32_0.field_read_status_set.cmd_type = 0x4;

	//p_desc_comm->u32_1.field.redunt_length = 0;
	//p_desc_comm->u32_1.field.payload_length = 0;

	//p_desc_comm->u32_2.field.interrupt_status = 0;
	p_desc_comm->u32_2.field.interrupt_mask = ND_INTR_DESC_END;

	//p_desc_comm->u32_3.field.addr_4 = 0;
	//p_desc_comm->u32_3.field.addr_5 = 0;
	//p_desc_comm->u32_3.field.data_sector_number = 0;
	//p_desc_comm->u32_3.field.data_structure_type = 0;
	p_desc_comm->u32_3.field.addr_number = 4;
	//p_desc_comm->u32_3.field.check_ff = 0;
	//p_desc_comm->u32_3.field.redunt_enable = 0;
	p_desc_comm->u32_3.field.cs_change_ctrl = 1;
	p_desc_comm->u32_3.field.end_desc = 1;
	p_desc_comm->u32_3.field.owner = 1;

	//p_desc_comm->u32_4.field.addr_0 = 0;
	//p_desc_comm->u32_4.field.addr_1 = 0;
	//p_desc_comm->u32_4.field.addr_2 = 0;
	//p_desc_comm->u32_4.field.addr_3 = 0;

	//p_desc_comm->u32_5.field.payload_bp = 0;

	//p_desc_comm->u32_6.field.redunt_bp = 0;

	//p_desc_comm->u32_7.field.next_desc_offset = 0;
	//p_desc_comm->u32_7.field.redunt_sector_address_offset = 0;

	hal_nand_set_wake_up_flag(p_desc_comm, ND_INTR_DESC_END);

	hal_nand_desc_basic_trigger();

	ret = hal_nand_desc_wait_complete(0, 1, MAX_NAND_WAIT_CNT, 0);

	if (ret == HAL_NAND_SUCCESS)
		*p_status_byte = (UINT8)(p_desc_comm->u32_0.field.read_status_data);

	return (ret);

}
#endif

SINT32 hal_nand_reset_nand(UINT8 which_cs)
{
	SINT32 ret;
	UINT32 old_interrupt_mask;

	p_desc_comm = (desc_command_t *)&g_io_buf.nand.descInfo[0];
	rd_memset((UINT8 *)p_desc_comm, 0x00, sizeof(desc_command_t));

	dbg_info();
	p_desc_comm->u32_0.field_super_cmd_ctl.cmd_0 = 0xff;
	//p_desc_comm->u32_0.field_super_cmd_ctl.cmd_1 = 0x00;
	//p_desc_comm->u32_0.field_super_cmd_ctl.have_addr = 0;
	//p_desc_comm->u32_0.field_super_cmd_ctl.is_two_command = 0;
	p_desc_comm->u32_0.field_super_cmd_ctl.wait_ready = 1;
	//p_desc_comm->u32_0.field_super_cmd_ctl.have_read = 0;
	//p_desc_comm->u32_0.field_super_cmd_ctl.have_write = 0;
	//p_desc_comm->u32_0.field_super_cmd_ctl.reserve = 0;
	p_desc_comm->u32_0.field_super_cmd_ctl.multi_func_sel = (1<<which_cs);
	p_desc_comm->u32_0.field_super_cmd_ctl.cmd_type = 0xd; /* super manual mode */

	//p_desc_comm->u32_1.field.redunt_length = 0;
	//p_desc_comm->u32_1.field.payload_length = 0;

	//p_desc_comm->u32_2.field.interrupt_status = 0;
	p_desc_comm->u32_2.field.interrupt_mask = ND_INTR_DESC_END;

	//p_desc_comm->u32_3.field.addr_4 = 0;
	//p_desc_comm->u32_3.field.addr_5 = 0;
	//p_desc_comm->u32_3.field.data_sector_number = 0;
	//p_desc_comm->u32_3.field.data_structure_type = 0;
	//p_desc_comm->u32_3.field.addr_number = 0;
	//p_desc_comm->u32_3.field.check_ff = 0;
	//p_desc_comm->u32_3.field.redunt_enable = 0;
	p_desc_comm->u32_3.field.cs_change_ctrl = 1;
	p_desc_comm->u32_3.field.end_desc = 1;
	p_desc_comm->u32_3.field.owner = 1;

	//p_desc_comm->u32_4.field.addr_0 = 0;
	//p_desc_comm->u32_4.field.addr_1 = 0;
	//p_desc_comm->u32_4.field.addr_2 = 0;
	//p_desc_comm->u32_4.field.addr_3 = 0;

	//p_desc_comm->u32_5.field.payload_bp = 0;

	//p_desc_comm->u32_6.field.redunt_bp = 0;

	//p_desc_comm->u32_7.field.next_desc_offset = 0;
	//p_desc_comm->u32_7.field.redunt_sector_address_offset = 0;

	old_interrupt_mask = hal_nand_set_wake_up_flag(p_desc_comm, ND_INTR_DESC_END | (ND_INTR_DESC_RB1<<which_cs));

	dbg();
	if (hal_nand_desc_basic_trigger() == ROM_FAIL) {
		dbg();
		return ROM_FAIL;
	}

	dbg();
	ret = hal_nand_desc_wait_complete(0, 0, MAX_NAND_WAIT_CNT, 0);

	dbg();
	hal_nand_set_interrupt_mask(old_interrupt_mask);

	return ret;
}

UINT16 hal_nand_read_id(UINT8 which_cs, UINT8 *id_info)
{
	UINT32	ret = -HAL_NAND_FAIL;
	UINT16	k, id_buf_len, id_len = 0, copy_id_len = 0;
	UINT8   id_buf[MAX_ID_LEN];

	id_buf_len = MAX_ID_LEN;
	rd_memset(id_buf, 0x00, id_buf_len);

	p_desc_comm = (desc_command_t *)&g_io_buf.nand.descInfo[0];
	rd_memset((UINT8 *)p_desc_comm, 0x00, sizeof(desc_command_t));

	p_desc_comm->u32_0.field_super_cmd_ctl.cmd_0 = 0x90;
	//p_desc_comm->u32_0.field_super_cmd_ctl.cmd_1 = 0x00;
	p_desc_comm->u32_0.field_super_cmd_ctl.have_addr = 1;
	//p_desc_comm->u32_0.field_super_cmd_ctl.is_two_command = 0;
	//p_desc_comm->u32_0.field_super_cmd_ctl.wait_ready = 0;
	p_desc_comm->u32_0.field_super_cmd_ctl.have_read = 1;
	//p_desc_comm->u32_0.field_super_cmd_ctl.have_write = 0;
	//p_desc_comm->u32_0.field_super_cmd_ctl.reserve = 0;
	p_desc_comm->u32_0.field_super_cmd_ctl.multi_func_sel = (1<<which_cs);
	p_desc_comm->u32_0.field_super_cmd_ctl.cmd_type = 0xd; /* super manual mode */

	//p_desc_comm->u32_1.field.redunt_length = 0;
	p_desc_comm->u32_1.field.payload_length = 31;

	//p_desc_comm->u32_2.field.interrupt_status = 0;
	p_desc_comm->u32_2.field.interrupt_mask = ND_INTR_DESC_END;

	//p_desc_comm->u32_3.field.addr_4 = 0;
	//p_desc_comm->u32_3.field.addr_5 = 0;
	//p_desc_comm->u32_3.field.data_sector_number = 0;
	//p_desc_comm->u32_3.field.data_structure_type = 0;
	//p_desc_comm->u32_3.field.addr_number = 4;
	//p_desc_comm->u32_3.field.check_ff = 0;
	//p_desc_comm->u32_3.field.redunt_enable = 0;
	p_desc_comm->u32_3.field.cs_change_ctrl = 1;
	p_desc_comm->u32_3.field.end_desc = 1;
	p_desc_comm->u32_3.field.owner = 1;

	//p_desc_comm->u32_4.field.addr_0 = 0;
	//p_desc_comm->u32_4.field.addr_1 = 0;
	//p_desc_comm->u32_4.field.addr_2 = 0;
	//p_desc_comm->u32_4.field.addr_3 = 0;

	//p_desc_comm->u32_5.field.payload_bp = 0;

	//p_desc_comm->u32_6.field.redunt_bp = 0;

	//p_desc_comm->u32_7.field.next_desc_offset = 0;
	//p_desc_comm->u32_7.field.redunt_sector_address_offset = 0;

	hal_nand_desc_set_addr(p_desc_comm, 0x00, 0x00, 1, HAL_NAND_ADDR_COL_ONLY);

	hal_nand_desc_set_payload(p_desc_comm, id_buf_len, id_buf);
	hal_nand_set_wake_up_flag(p_desc_comm, ND_INTR_DESC_END);
	p_desc_comm->u32_0.field.multi_func_sel = (1<<which_cs);

	hal_nand_desc_basic_trigger();

	ret = hal_nand_desc_wait_complete(0, 0, MAX_NAND_WAIT_CNT, 0);

	if (ret == HAL_NAND_SUCCESS) {
		if (id_buf[0] != 0) {
			for (k=1; k<id_buf_len; k++) {
				if (id_buf[k] == id_buf[0] || id_buf[k] == 0x00 || (k>=5 && id_buf[k]==0x08))
					break;
			}
			id_len = k;
		}

		// find first byte that is 0
		for (k=0; k < id_len; k++) {
			if (id_buf[k]==0) {
				break;
			}
		}
		id_len = k;

		if (id_len != 0) {
			if (id_len > MAX_ID_LEN) {
				DBGPRINT("nand id len more than buf. id_len[%d], user_id_len[%d]\n", id_len, MAX_ID_LEN);
				copy_id_len = MAX_ID_LEN;
			} else
				copy_id_len = id_len;

			if (copy_id_len)
				rd_memcpy((UINT32 *)id_info, (UINT32 *)id_buf, ((copy_id_len+3)>>2)<<2);
			else
				rd_memset((UINT8 *)id_info, 0x00, ((MAX_ID_LEN+3)>>2)<<2);
		}
	}

	//	if (copy_id_len != 0)
	//		dump_buffer(id_info, copy_id_len);

	return id_len;
}

UINT16 hal_nand_super_set_timing_mode(UINT8 which_cs, UINT8 timing_mode, UINT8 address)
{
	UINT32	ret = -HAL_NAND_FAIL;
	UINT8	temp_buf[4];

	p_desc_comm = (desc_command_t *)&g_io_buf.nand.descInfo[0];
	rd_memset((UINT8 *)p_desc_comm, 0x00, sizeof(desc_command_t));

	p_desc_comm->u32_0.field_super_cmd_ctl.cmd_0 = 0xef;
	//p_desc_comm->u32_0.field_super_cmd_ctl.cmd_1 = 0x00;
	p_desc_comm->u32_0.field_super_cmd_ctl.have_addr = 1;
	//p_desc_comm->u32_0.field_super_cmd_ctl.is_two_command = 0;
	p_desc_comm->u32_0.field_super_cmd_ctl.wait_ready = 1;
	//p_desc_comm->u32_0.field_super_cmd_ctl.have_read = 0;
	p_desc_comm->u32_0.field_super_cmd_ctl.have_write = 1;
	//p_desc_comm->u32_0.field_super_cmd_ctl.reserve = 0;
	p_desc_comm->u32_0.field_super_cmd_ctl.multi_func_sel = (1<<which_cs);
	p_desc_comm->u32_0.field_super_cmd_ctl.cmd_type = 0xd; /* super manual mode */

	//p_desc_comm->u32_1.field.redunt_length = 0;
	p_desc_comm->u32_1.field.payload_length = 3;

	//p_desc_comm->u32_2.field.interrupt_status = 0;
	p_desc_comm->u32_2.field.interrupt_mask = ND_INTR_DESC_END;

	//p_desc_comm->u32_3.field.addr_4 = 0;
	//p_desc_comm->u32_3.field.addr_5 = 0;
	//p_desc_comm->u32_3.field.data_sector_number = 0;
	//p_desc_comm->u32_3.field.data_structure_type = 0;
	//p_desc_comm->u32_3.field.addr_number = 4;
	//p_desc_comm->u32_3.field.check_ff = 0;
	//p_desc_comm->u32_3.field.redunt_enable = 0;
	p_desc_comm->u32_3.field.cs_change_ctrl = 1;
	p_desc_comm->u32_3.field.end_desc = 1;
	p_desc_comm->u32_3.field.owner = 1;

	//p_desc_comm->u32_4.field.addr_0 = 0;
	//p_desc_comm->u32_4.field.addr_1 = 0;
	p_desc_comm->u32_4.field.addr_2 = 0x01;
	//p_desc_comm->u32_4.field.addr_3 = 0;

	//p_desc_comm->u32_5.field.payload_bp = 0;

	//p_desc_comm->u32_6.field.redunt_bp = 0;

	//p_desc_comm->u32_7.field.next_desc_offset = 0;
	//p_desc_comm->u32_7.field.redunt_sector_address_offset = 0;


	temp_buf[0] = timing_mode;
	temp_buf[1] = 0;
	temp_buf[2] = 0;
	temp_buf[3] = 0;

	hal_nand_desc_set_payload(p_desc_comm, 4, temp_buf);
	hal_nand_set_wake_up_flag(p_desc_comm, ND_INTR_DESC_END);

	p_desc_comm->u32_0.field.multi_func_sel = (1<<which_cs);
	p_desc_comm->u32_4.field.addr_2 = address; // 0x01;  // FA of timing mode is 1

	hal_nand_desc_basic_trigger();

	ret = hal_nand_desc_wait_complete(0, 0, MAX_NAND_WAIT_CNT, 0);

	return ret;
}

/*********************Basic function********************/

void hal_nand_controller_set_op_mode(UINT32 op_mode)
{
	NANDREG_W(FM_CSR, op_mode);
}

UINT32 hal_nand_controller_get_op_mode(void)
{
	return NANDREG_R(FM_CSR);
}

void hal_nand_set_rdy_pin_enable(UINT32 enable)
{
	NANDREG_W(FM_RDYBSY_EN, enable);
}

UINT32 hal_nand_get_rdy_pin_enable(void)
{
	return NANDREG_R(FM_RDYBSY_EN);
}

UINT8 hal_nand_randomizer_enable(UINT8 enable)
{
	UINT8 old_en;

	old_en = ((NANDREG_R(FM_RANDOM_EN) & 1) != 0) ? 1 : 0;

	if (enable)
		NANDREG_W(FM_RANDOM_EN, NANDREG_R(FM_RANDOM_EN) | 1);
	else
		NANDREG_W(FM_RANDOM_EN, NANDREG_R(FM_RANDOM_EN) & (~1));

	return old_en;
}

void hal_nand_randomizer_off(void)
{
	hal_nand_randomizer_enable(0);
}

SINT32 initDriver_nand(void)
{
	UINT32 i = 0;

	dbg_info();

	// NAND CTRL soft reset
	NANDREG_W(FM_CSR, 1<<8);
	while(((NANDREG_R(FM_CSR) & (1<<8)) != 0) && (i++ < 0xfffff)) ;

	dbg();

	// moved to SetBootDev()
	//regs0->pad_ctrl[3] |= 0x1fffe000;	//G4.3[28..13] set 1, NAND pin slew rate set fast

	hal_nand_controller_set_op_mode(0x09); 	// enable controller, use descriptor method, burst 8, no EDO
	hal_nand_set_interrupt_mask(0xf05f); // manual mode use this to enable invalid, complete interruptle. all mode use this to enable ready int.
	g_pSysInfo->u16InterruptMask = 0xf05f;
	hal_nand_clear_interrupt(0xffff);

	hal_nand_randomizer_off();

	NANDREG_W(FM_AC_TIMING0, GetNANDACReadTiming());
	NANDREG_W(FM_AC_TIMING1, GetNANDACWriteTiming());

	hal_nand_set_rdy_pin_enable(0xF);

	CSTAMP(0xAA550000); // before reset
	if (hal_nand_reset_nand(NAND_CS0) != ROM_SUCCESS) {
		CSTAMP(0xAA550001); // reset target nok
		return ROM_FAIL;
	}

	CSTAMP(0xAA550002); // reset target ok

	DelayUS(200); /* wait 200us */

	CSTAMP(0xAA550003);
	g_bootinfo.gNANDIDLength = hal_nand_read_id(0, g_pyldData);

#if 0  // optional Mode 5 is for Micron speedup
	if ((g_pyldData[0] != NAND_TOSHIBA) &&
			(g_pyldData[0] != NAND_SAMSUNG) &&
			(g_pyldData[0] != NAND_THOMSON) &&
			(g_pyldData[0] != NAND_HYNIX) &&
			(g_pyldData[0] != NAND_INFINEON) &&
			(g_pyldData[0] != NAND_INTEL) &&
			(g_pyldData[0] != NAND_MXIC) &&
			(g_pyldData[0] != NAND_SANDISK) &&
			(g_pyldData[0] != NAND_POWERFLASH))
	{
		CSTAMP(0xAA550004); // set feature
		/* This is Micron NAND or unknown NAND */
		/* Need change tREA from Mode0(40ns) to Mode5(16ns) and try again */
		prn_string("Set Timing mode to Mode5!!\n");
		ret = hal_nand_super_set_timing_mode(0, 5, 1);
		DelayUS(10); /* wait 10us */
		prn_string("Set Timing mode finish, ret=");prn_dword(ret);

		if (ret != ROM_SUCCESS) {
			NANDREG_W(FM_CSR, 1<<8);

			while((NANDREG_R(FM_CSR) & (1<<8)) != 0) ;

			if (hal_nand_reset_nand(NAND_CS0) != ROM_SUCCESS) return ROM_FAIL;

			DelayUS(200); /* wait 200us */
		}

		CSTAMP(0xAA550005); // read id again
		g_bootinfo.gNANDIDLength = hal_nand_read_id(0, g_pyldData);
	}
#endif

	CSTAMP(0xAA550006);
	if (g_bootinfo.gNANDIDLength == 0) return ROM_FAIL;

	CSTAMP(0xAA550007);
	//rd_memcpy((UINT32 *)g_pSysInfo->IdBuf, (UINT32 *)g_pyldData, MAX_ID_LEN);
	rd_memcpy((UINT32 *)g_pSysInfo->IdBuf, (UINT32 *)g_pyldData, ((g_bootinfo.gNANDIDLength+3)>>2)<<2);
	prn_string("ID = ");
	for (i = 0; i < g_bootinfo.gNANDIDLength && i < 8; i++) {
		prn_byte(g_pyldData[i]);
	}
	prn_string("\n");

	hal_nand_clear_interrupt(0xffff);
	hal_nand_set_interrupt_mask(0xf05f);

#ifdef PARTIAL_NAND_INIT
	return ROM_SUCCESS; /* xboot */
#else
	CSTAMP(0xAA551010);
	setSystemPara(g_pSysInfo->IdBuf);
	CSTAMP(0xAA551011);

	/* Francis : give a initial value to read header */
	g_pSysInfo->u16PyldLen = 2048;	/* 2K Page */
	g_pSysInfo->u16ReduntLen = 0;
	g_pSysInfo->u16PageNoPerBlk = 32; /* 32 pages per block */
	g_pSysInfo->u8PagePerBlkShift = nfvalshift(32);
	g_pSysInfo->u8addrCycle = 6;

	CSTAMP(0xAA551012); // before load header
	if (Load_Header_Profile(0) == ROM_SUCCESS) {
		CSTAMP(0xAA551013);
		setSystemPara(g_pSysInfo->IdBuf);
		CSTAMP(0xAA551014);
		return ROM_SUCCESS;
	}

	CSTAMP(0xAA551015); // failed to load header
	return ROM_FAIL;
#endif
}

void hwNandCmdSend(UINT8 which_cs, UINT8 cmd)
{
	UINT32 old_interrupt_mask;

	p_desc_comm = (desc_command_t *)&g_io_buf.nand.descInfo[0];
	rd_memset((UINT8 *)p_desc_comm, 0x00, sizeof(desc_command_t));

	p_desc_comm->u32_0.field_super_cmd_ctl.cmd_0 = cmd;
	p_desc_comm->u32_0.field.multi_func_sel = (1<<which_cs);
	p_desc_comm->u32_0.field_super_cmd_ctl.cmd_type = CMD_TYPE_MANUAL_MODE_CMD;

	old_interrupt_mask = hal_nand_set_wake_up_flag(p_desc_comm, ND_INTR_DESC_END);

	p_desc_comm->u32_3.field.cs_change_ctrl = 1;
	p_desc_comm->u32_3.field.end_desc = 1;
	p_desc_comm->u32_3.field.owner = 1;

	hal_nand_desc_basic_trigger();

	hal_nand_set_interrupt_mask(old_interrupt_mask);
}

SINT32 ReadWritePage(UINT8 which_cs, UINT32 u32PhyAddr,UINT32* PyldBuffer,UINT32* DataBuffer,UINT32 u8RWMode)
{
	SINT32 ret;

	if (g_bootinfo.gRandomizeFlag == 1) {
		hal_nand_randomizer_enable(1);
	}

	if(u8RWMode==NF_READ) {
		p_desc_comm = (desc_command_t *)&g_io_buf.nand.descInfo[0];
		rd_memset((UINT8 *)p_desc_comm, 0x00, sizeof(desc_command_t));

		p_desc_comm->u32_0.field.cmd_type = CMD_TYPE_HALFAUTO_READ;
		p_desc_comm->u32_0.field.multi_func_sel = (1<<which_cs);
		p_desc_comm->u32_0.field.cmd_0 = 0x00;
		p_desc_comm->u32_0.field.cmd_1 = 0x30;
		//p_desc_comm->u32_3.field.addr_number = g_pSysInfo->u8addrCycle-1;
		p_desc_comm->u32_3.field.owner = 1;
		p_desc_comm->u32_3.field.end_desc = 1;
		p_desc_comm->u32_3.field.cs_change_ctrl = 1;
#if 0
		p_desc_comm->u32_3.field.addr_5 = ((u32PhyAddr>>24) & 0xff);
		p_desc_comm->u32_3.field.addr_4 = ((u32PhyAddr>>16) & 0xff);
		p_desc_comm->u32_4.field.addr_3 = ((u32PhyAddr>> 8) & 0xff);
		p_desc_comm->u32_4.field.addr_2 = ( u32PhyAddr      & 0xff);
#else
		hal_nand_desc_set_addr(p_desc_comm, u32PhyAddr, 0x00, g_pSysInfo->u8addrCycle, HAL_NAND_ADDR_COL_2_BYTE);
#endif

		p_desc_comm->u32_7.field.randomizer_en = 1;

		hal_nand_set_wake_up_flag(p_desc_comm, ND_INTR_DESC_END | ND_INTR_DESC_RB1);
		hal_nand_desc_set_payload(p_desc_comm, g_pSysInfo->u16PyldLen, PyldBuffer);
		hal_nand_desc_set_redunt(p_desc_comm, g_pSysInfo->u16ReduntLen, DataBuffer, 0);

		hal_nand_desc_basic_trigger();
		ret = hal_nand_desc_wait_complete(0, 1, MAX_NAND_WAIT_CNT, 0);
		if (ret != HAL_NAND_SUCCESS) {
			if (g_bootinfo.gRandomizeFlag == 1) {
				hal_nand_randomizer_off();
			}

			return ROM_FAIL;
		}

		//PYLOAD phase read
		p_desc_comm = (desc_command_t *)&g_io_buf.nand.descInfo[0];
		rd_memset((UINT8 *)p_desc_comm, 0x00, sizeof(desc_command_t));

		p_desc_comm->u32_0.field.cmd_type = CMD_TYPE_MANUAL_MODE_PYLOAD_READ;
		p_desc_comm->u32_0.field.multi_func_sel = (1<<which_cs);
		//p_desc_comm->u32_0.field.cmd_0 = 0x00;
		//p_desc_comm->u32_0.field.cmd_1 = 0x00;
		p_desc_comm->u32_3.field.owner = 1;
		//p_desc_comm->u32_3.field.end_desc = 0;
		p_desc_comm->u32_3.field.cs_change_ctrl = 1;
		hal_nand_set_wake_up_flag(p_desc_comm, ND_INTR_DESC_END);
		hal_nand_desc_set_payload(p_desc_comm, g_pSysInfo->u16PyldLen, PyldBuffer);
		hal_nand_desc_set_redunt(p_desc_comm, g_pSysInfo->u16ReduntLen, DataBuffer, g_pSysInfo->u16Redunt_Sector_Addr_Offset);
		p_desc_comm->u32_7.field.randomizer_en = 1;
		p_desc_comm->u32_7.field.next_desc_offset = sizeof(desc_command_t);

		//Redunt phase read
		p_desc_comm = p_desc_comm+1;
		rd_memset((UINT8 *)p_desc_comm, 0x00, sizeof(desc_command_t));

		p_desc_comm->u32_0.field.cmd_type = CMD_TYPE_MANUAL_MODE_REDUNT_READ;
		p_desc_comm->u32_0.field.multi_func_sel = (1<<which_cs);
		//p_desc_comm->u32_0.field.cmd_0 = 0x00;
		//p_desc_comm->u32_0.field.cmd_1 = 0x00;
		p_desc_comm->u32_3.field.owner = 1;
		p_desc_comm->u32_3.field.end_desc = 1;
		p_desc_comm->u32_3.field.cs_change_ctrl = 1;

		p_desc_comm->u32_7.field.randomizer_en = 1;

		hal_nand_set_wake_up_flag(p_desc_comm, ND_INTR_DESC_END);
		hal_nand_desc_set_payload(p_desc_comm, g_pSysInfo->u16PyldLen, PyldBuffer);
		hal_nand_desc_set_redunt(p_desc_comm, g_pSysInfo->u16ReduntLen, DataBuffer, g_pSysInfo->u16Redunt_Sector_Addr_Offset);

		hal_nand_desc_basic_trigger();

		ret = hal_nand_desc_wait_complete(0, 1, MAX_NAND_WAIT_CNT, 0);
		if (ret != HAL_NAND_SUCCESS) {
			if (g_bootinfo.gRandomizeFlag == 1) {
				hal_nand_randomizer_off();
			}

			return ROM_FAIL;
		}
	} else {
		p_desc_comm = (desc_command_t *)&g_io_buf.nand.descInfo[0];
		rd_memset((UINT8 *)p_desc_comm, 0x00, sizeof(desc_command_t));

		p_desc_comm->u32_0.field.cmd_type = CMD_TYPE_WRITE;
		p_desc_comm->u32_0.field.multi_func_sel = (1<<which_cs);
		p_desc_comm->u32_0.field.cmd_0 = 0x80;
		p_desc_comm->u32_0.field.cmd_1 = 0x10;
		//p_desc_comm->u32_3.field.addr_number = g_pSysInfo->u8addrCycle-1;
		p_desc_comm->u32_3.field.owner = 1;
		p_desc_comm->u32_3.field.end_desc = 1;
		p_desc_comm->u32_3.field.cs_change_ctrl = 1;
#if 0
		p_desc_comm->u32_3.field.addr_5 = ((u32PhyAddr>>24) & 0xff);
		p_desc_comm->u32_3.field.addr_4 = ((u32PhyAddr>>16) & 0xff);
		p_desc_comm->u32_4.field.addr_3 = ((u32PhyAddr>> 8) & 0xff);
		p_desc_comm->u32_4.field.addr_2 = ( u32PhyAddr      & 0xff);
#else
		hal_nand_desc_set_addr(p_desc_comm, u32PhyAddr, 0x00, g_pSysInfo->u8addrCycle, HAL_NAND_ADDR_COL_2_BYTE);
#endif

		p_desc_comm->u32_7.field.randomizer_en = 1;

		hal_nand_set_wake_up_flag(p_desc_comm, ND_INTR_DESC_END | ND_INTR_DESC_RB1);
		hal_nand_desc_set_payload(p_desc_comm, g_pSysInfo->u16PyldLen, PyldBuffer);
		hal_nand_desc_set_redunt(p_desc_comm, g_pSysInfo->u16ReduntLen, DataBuffer, 0);

		hal_nand_desc_basic_trigger();

		if (g_bootinfo.gRandomizeFlag == 1)
		{
			ret = hal_nand_desc_wait_complete(0, 1, MAX_NAND_WAIT_CNT, 0);
			if (ret != HAL_NAND_SUCCESS)
			{
				if (g_bootinfo.gRandomizeFlag == 1)
				{
					hal_nand_randomizer_off();
				}

				return ROM_FAIL;
			}
		}
	}

	if (g_bootinfo.gRandomizeFlag == 1) {
		hal_nand_randomizer_off();
	}

	return ROM_SUCCESS;
}

void initNandFunptr(void)
{
	SDev_t* pSDev = getSDev();

	pSDev->IsSupportBCH = 1;
	pSDev->DeviceID = DEVICE_PARA_NAND;

	pSDev->predInitDriver    = (predInitDriver_t)initDriver_nand;
	pSDev->predReadWritePage = (predReadWritePage_t)ReadWritePage;
}
