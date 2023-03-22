#include <common.h>
#include <i2c/sp_i2c.h>

static u32 i2c_sp_read_clear_intrbits(unsigned int i2c_no, volatile struct dw_i2c_regs *i2c_regs)
{
	unsigned int stat;
	unsigned int temp;

	/*
	 * The IC_INTR_STAT register just indicates "enabled" interrupts.
	 * The unmasked raw version of interrupt status bits is available
	 * in the IC_RAW_INTR_STAT register.
	 *
	 * That is,
	 *   stat = readl(IC_INTR_STAT);
	 * equals to,
	 *   stat = readl(IC_RAW_INTR_STAT) & readl(IC_INTR_MASK);
	 *
	 * The raw version might be useful for debugging purposes.
	 */
	stat = (u32)i2c_regs->ic_intr_stat;
	temp = stat;
	/*
	 * Do not use the IC_CLR_INTR register to clear interrupts, or
	 * you'll miss some interrupts, triggered during the period from
	 * readl(IC_INTR_STAT) to readl(IC_CLR_INTR).
	 *
	 * Instead, use the separately-prepared IC_CLR_* registers.
	 */
	if (temp & SP_IC_INTR_RX_UNDER)
		temp = (u32)i2c_regs->ic_clr_rx_under;
	if (stat & SP_IC_INTR_RX_OVER)
		temp = (u32)i2c_regs->ic_clr_rx_over;
	if (stat & SP_IC_INTR_TX_OVER)
		temp = (u32)i2c_regs->ic_clr_tx_over;
	if (stat & SP_IC_INTR_RD_REQ)
		temp = (u32)i2c_regs->ic_clr_rd_req;
	if (stat & SP_IC_INTR_TX_ABRT) {
		/*
		 * The IC_TX_ABRT_SOURCE register is cleared whenever
		 * the IC_CLR_TX_ABRT is read.  Preserve it beforehand.
		 */
		i2c_mas_ctlr[i2c_no].Abort_Source = (unsigned int)i2c_regs->ic_tx_abrt_source;
		temp = (u32)i2c_regs->ic_clr_tx_abrt;
	}
	if (stat & SP_IC_INTR_RX_DONE)
		temp = (u32)i2c_regs->ic_clr_rx_done;
	if (stat & SP_IC_INTR_ACTIVITY)
		temp = (u32)i2c_regs->ic_clr_tx_activity;
	if (stat & SP_IC_INTR_STOP_DET)
		temp = (u32)i2c_regs->ic_clr_stop_det;
	if (stat & SP_IC_INTR_START_DET)
		temp = (u32)i2c_regs->ic_clr_start_dft;
	if (stat & SP_IC_INTR_GEN_CALL)
		temp = (u32)i2c_regs->ic_clr_gen_call;

	return stat;
}

void i2c_dw_handle_tx_abort(unsigned int i2c_no)
{
	unsigned int abort_source = i2c_mas_ctlr[i2c_no].Abort_Source;

	if (abort_source & SP_IC_TX_ABRT_NOACK) {
			prn_string("NACK fail \n");
		return;
	}
		prn_string("NACK fail 02\n");

	return;

}

void sp_i2c_en(unsigned int i2c_no)
{

	switch (i2c_no) {
		case 0:
			i2c_mas_ctlr[i2c_no].reg = I2C0_REG_AO;
			MOON1_REG_AO->sft_cfg[3] = RF_MASK_V(0x1 << 3, 0x1 << 3); // cfg3[4:3]=1  i2c1 pin
			MOON2_REG_AO->clken[8] =  RF_MASK_V_SET(1 << 9);
			MOON2_REG_AO->gclken[8] = RF_MASK_V_CLR(1 << 9);
			MOON0_REG_AO->reset[8] = RF_MASK_V_CLR(1 << 9);
		break;
		case 1:
			i2c_mas_ctlr[i2c_no].reg = I2C1_REG_AO;
			MOON1_REG_AO->sft_cfg[3] = RF_MASK_V(1 << 5, 1 << 5);
			MOON2_REG_AO->clken[8] =  RF_MASK_V_SET(1 << 10);
			MOON2_REG_AO->gclken[8] = RF_MASK_V_CLR(1 << 10);
			MOON0_REG_AO->reset[8] = RF_MASK_V_CLR(1 << 10);
		break;
		case 2:
			i2c_mas_ctlr[i2c_no].reg = I2C2_REG_AO;
			MOON1_REG_AO->sft_cfg[3] = RF_MASK_V(0x1 << 6, 0x1 << 6);
			MOON2_REG_AO->clken[8] =  RF_MASK_V_SET(1 << 11);
			MOON2_REG_AO->gclken[8] = RF_MASK_V_CLR(1 << 11);
			MOON0_REG_AO->reset[8] = RF_MASK_V_CLR(1 << 11);
		break;
		case 3:
			i2c_mas_ctlr[i2c_no].reg = I2C3_REG_AO;
			MOON1_REG_AO->sft_cfg[3] = RF_MASK_V(1 << 8, 1 << 8);
			MOON2_REG_AO->clken[8] =  RF_MASK_V_SET(1 << 12);
			MOON2_REG_AO->gclken[8] = RF_MASK_V_CLR(1 << 12);
			MOON0_REG_AO->reset[8] = RF_MASK_V_CLR(1 << 12);
		break;
		case 4:
			i2c_mas_ctlr[i2c_no].reg = I2C4_REG_AO;
			MOON1_REG_AO->sft_cfg[3] = RF_MASK_V(1 << 9, 1 << 9);
			MOON2_REG_AO->clken[8] =  RF_MASK_V_SET(1 << 13);
			MOON2_REG_AO->gclken[8] = RF_MASK_V_CLR(1 << 13);
			MOON0_REG_AO->reset[8] = RF_MASK_V_CLR(1 << 13);
		break;
		case 5:
			i2c_mas_ctlr[i2c_no].reg = I2C5_REG_AO;
			MOON1_REG_AO->sft_cfg[3] = RF_MASK_V(1 << 10, 1 << 10);
			MOON2_REG_AO->clken[8] =  RF_MASK_V_SET(1 << 14);
			MOON2_REG_AO->gclken[8] = RF_MASK_V_CLR(1 << 14);
			MOON0_REG_AO->reset[8] = RF_MASK_V_CLR(1 << 14);
		break;
		case 6:
			i2c_mas_ctlr[i2c_no].reg = I2C6_REG_AO;
			MOON1_REG_AO->sft_cfg[3] = RF_MASK_V(0x1 << 11, 0x1 << 11);
			MOON2_REG_AO->clken[8] =  RF_MASK_V_SET(1 << 15);
			MOON2_REG_AO->gclken[8] = RF_MASK_V_CLR(1 << 15);
			MOON0_REG_AO->reset[8] = RF_MASK_V_CLR(1 << 15);
		break;
		case 7:
			i2c_mas_ctlr[i2c_no].reg = I2C7_REG_AO;
			MOON1_REG_AO->sft_cfg[3] = RF_MASK_V(0x1 << 13, 0x1 << 13);
			MOON2_REG_AO->clken[9] =  RF_MASK_V_SET(1 << 0);
			MOON2_REG_AO->gclken[9] = RF_MASK_V_CLR(1 << 0);
			MOON0_REG_AO->reset[9] = RF_MASK_V_CLR(1 << 0);
		break;
		case 8:
			i2c_mas_ctlr[i2c_no].reg = I2C8_REG_AO;
			MOON1_REG_AO->sft_cfg[4] = RF_MASK_V(0x1 << 0, 0x1 << 0);
			MOON2_REG_AO->clken[9] =  RF_MASK_V_SET(1 << 1);
			MOON2_REG_AO->gclken[9] = RF_MASK_V_CLR(1 << 1);
			MOON0_REG_AO->reset[9] = RF_MASK_V_CLR(1 << 1);
		break;
		case 9:
			i2c_mas_ctlr[i2c_no].reg = I2C9_REG_AO;
			MOON1_REG_AO->sft_cfg[4] = RF_MASK_V(0x1 << 2, 0x1 << 2);
			MOON2_REG_AO->clken[9] =  RF_MASK_V_SET(1 << 2);
			MOON2_REG_AO->gclken[9] = RF_MASK_V_CLR(1 << 2);
			MOON0_REG_AO->reset[9] = RF_MASK_V_CLR(1 << 2);
		break;
		}
}

void sp_i2c_write(unsigned int i2c_no, u8  slave_addr , u8  *data_buf , unsigned int len)
{

        unsigned int temp_reg;			
        unsigned int xfer_cnt;
        unsigned int xfer_wait;
        u32 stat;

	volatile struct dw_i2c_regs *i2c_regs = i2c_mas_ctlr[i2c_no].reg;
	i2c_mas_ctlr[i2c_no].buf = data_buf;
	i2c_mas_ctlr[i2c_no].DataIndex = 0;
	i2c_mas_ctlr[i2c_no].Abort_Source =  0;
	i2c_mas_ctlr[i2c_no].DataTotalLen = len;
	i2c_mas_ctlr[i2c_no].ReadTxlen = len;
	i2c_mas_ctlr[i2c_no].RxOutStanding = 0;
	i2c_mas_ctlr[i2c_no].xfet_action = 1;

	i2c_regs->ic_tx_tl = I2C_TX_FIFO_DEPTH / 2;
	//i2c_regs->SP_IC_RX_TL = 0;

        temp_reg = SP_IC_CON_MASTER | SP_IC_CON_SLAVE_DISABLE | SP_IC_CON_RESTART_EN;
	temp_reg |= SP_IC_CON_SPEED_STD;

	i2c_regs->ic_con = temp_reg;
	i2c_regs->ic_tar = slave_addr;
	i2c_regs->ic_enable = 1;

	temp_reg = i2c_regs->ic_enable_status;
	temp_reg = i2c_regs->ic_clr_intr;

	temp_reg = SP_IC_INTR_DEFAULT_MASK | SP_IC_INTR_MASTER_MASK;
	i2c_regs->ic_intr_mask = temp_reg;

	stat = i2c_sp_read_clear_intrbits(i2c_no ,i2c_regs);

	while(i2c_mas_ctlr[i2c_no].DataTotalLen)
	{
		xfer_cnt = I2C_TX_FIFO_DEPTH - i2c_regs->ic_txflr;
		while(xfer_cnt >0 && i2c_mas_ctlr[i2c_no].DataTotalLen > 0)
		{
			i2c_regs->ic_data_cmd = i2c_mas_ctlr[i2c_no].buf[i2c_mas_ctlr[i2c_no].DataIndex];
			i2c_mas_ctlr[i2c_no].DataIndex++;
			i2c_mas_ctlr[i2c_no].DataTotalLen--;
			xfer_cnt--;
			//diag_printf("xfer_cnt %d\n",xfer_cnt);
		}
		stat = i2c_regs->ic_status;
		xfer_wait = 0;
		while((stat & SP_IC_STATUS_MST_ACT) != SP_IC_STATUS_MST_ACT){
			stat = i2c_regs->ic_status;
			xfer_wait++;
			if(xfer_wait > (4*I2C_TX_FIFO_DEPTH))
				break;
		}
		stat = i2c_sp_read_clear_intrbits(i2c_no ,i2c_regs);
		if (stat & SP_IC_INTR_TX_ABRT) {
			i2c_dw_handle_tx_abort(i2c_no);
			i2c_mas_ctlr[i2c_no].xfet_action = 0;
			break;
		}
	}

	while(i2c_mas_ctlr[i2c_no].xfet_action)
	{
		stat = i2c_regs->ic_status;
		if ((stat & SP_IC_STATUS_MST_ACT) != SP_IC_STATUS_MST_ACT)
			i2c_mas_ctlr[i2c_no].xfet_action = 0;
		
		//diag_printf("RAW_INTR_STAT %x  action : %d \n",stat ,i2c_mas_ctlr[i2c_no].xfet_action);
		stat = i2c_sp_read_clear_intrbits(i2c_no , i2c_regs);
		if (stat & SP_IC_INTR_TX_ABRT) {
			i2c_dw_handle_tx_abort(i2c_no);
			i2c_mas_ctlr[i2c_no].xfet_action = 0;
			break;
		}		
	}

	i2c_regs->ic_intr_mask = 0;
	temp_reg = i2c_regs->ic_clr_intr;
	i2c_regs->ic_enable = 0;
}

void sp_i2c_read(unsigned int i2c_no, u8  slave_addr , u8  *data_buf , unsigned int len)
{
        unsigned int temp_reg;
        unsigned int xfer_cnt;
        unsigned int xfer_wait;
        u32 stat;

	volatile struct dw_i2c_regs *i2c_regs = i2c_mas_ctlr[i2c_no].reg;
	//diag_printf("i2c_regs %x\n",i2c_regs);
	//printf("i2cread\n");
	//printf("i2c_no : %d, slave_addr: 0x%x , len %d\n", i2c_no, slave_addr,len);
	//printf("sp_i2c_read 0x%x 0x%x\n",i2c_regs,i2c_mas_ctlr[i2c_no].reg_adr);
	i2c_mas_ctlr[i2c_no].buf = data_buf;
	i2c_mas_ctlr[i2c_no].DataIndex = 0;
	i2c_mas_ctlr[i2c_no].Abort_Source =  0;
	i2c_mas_ctlr[i2c_no].DataTotalLen = len;
	i2c_mas_ctlr[i2c_no].ReadTxlen = len;
	i2c_mas_ctlr[i2c_no].RxOutStanding = 0;
	i2c_mas_ctlr[i2c_no].xfet_action = 1;

	i2c_regs->ic_tx_tl = I2C_TX_FIFO_DEPTH / 2;
	i2c_regs->ic_rx_tl = 0;

        temp_reg = SP_IC_CON_MASTER | SP_IC_CON_SLAVE_DISABLE | SP_IC_CON_RESTART_EN;
	temp_reg |= SP_IC_CON_SPEED_STD;

	i2c_regs->ic_con = temp_reg;
	i2c_regs->ic_tar = slave_addr;
	i2c_regs->ic_enable = 1;

	temp_reg = i2c_regs->ic_enable_status;
	temp_reg = i2c_regs->ic_clr_intr;

	temp_reg = SP_IC_INTR_DEFAULT_MASK | SP_IC_INTR_MASTER_MASK;
	i2c_regs->ic_intr_mask = temp_reg;

	stat = i2c_sp_read_clear_intrbits(i2c_no , i2c_regs);

	while(i2c_mas_ctlr[i2c_no].DataTotalLen)
	{
		xfer_cnt = I2C_TX_FIFO_DEPTH - i2c_regs->ic_txflr;
		while(xfer_cnt >0 && i2c_mas_ctlr[i2c_no].DataTotalLen > 0)
		{
			i2c_regs->ic_data_cmd = I2C_READ_TX_DATA;
			i2c_mas_ctlr[i2c_no].DataTotalLen--;
			xfer_cnt--;
		}
		stat = i2c_regs->ic_status;
		xfer_wait = 0;
		while((stat & SP_IC_STATUS_MST_ACT) != SP_IC_STATUS_MST_ACT){
			stat = i2c_regs->ic_status;
			xfer_wait++;
			if(xfer_wait > (4*I2C_TX_FIFO_DEPTH))
				break;
		}
		stat = i2c_regs->ic_status;
		if (stat & SP_IC_STATUS_RFNE){
			i2c_mas_ctlr[i2c_no].buf[i2c_mas_ctlr[i2c_no].DataIndex] = i2c_regs->ic_data_cmd;
			i2c_mas_ctlr[i2c_no].DataIndex++;
			//diag_printf("data 0x%x\n", i2c_regs->ic_data_cmd);
			i2c_mas_ctlr[i2c_no].ReadTxlen--;
		}
		stat = i2c_sp_read_clear_intrbits(i2c_no ,i2c_regs);
		if (stat & SP_IC_INTR_TX_ABRT) {
			i2c_dw_handle_tx_abort(i2c_no);
			i2c_mas_ctlr[i2c_no].xfet_action = 0;
			break;
		}		
	}

	while(i2c_mas_ctlr[i2c_no].xfet_action)
	{
		stat = i2c_regs->ic_status;

		if (stat & SP_IC_STATUS_TFE)
			i2c_mas_ctlr[i2c_no].xfet_action = 0;
		
		//diag_printf("RAW_INTR_STAT %x  action : %d \n",stat ,i2c_mas_ctlr[i2c_no].xfet_action);
		stat = i2c_sp_read_clear_intrbits(i2c_no , i2c_regs);
		if (stat & SP_IC_INTR_TX_ABRT) {
			i2c_dw_handle_tx_abort(i2c_no);
			i2c_mas_ctlr[i2c_no].xfet_action = 0;
			break;
		}		
	}

	while(i2c_mas_ctlr[i2c_no].ReadTxlen)
	{
		stat = i2c_regs->ic_status;
		if (stat & SP_IC_STATUS_RFNE){
			i2c_mas_ctlr[i2c_no].buf[i2c_mas_ctlr[i2c_no].DataIndex] = i2c_regs->ic_data_cmd;
			i2c_mas_ctlr[i2c_no].DataIndex++;
			//diag_printf("data 0x%x\n", i2c_regs->ic_data_cmd);
			i2c_mas_ctlr[i2c_no].ReadTxlen--;
		}
	}
	
	i2c_regs->ic_intr_mask = 0;
	temp_reg = i2c_regs->ic_clr_intr;
	i2c_regs->ic_enable = 0;
}
