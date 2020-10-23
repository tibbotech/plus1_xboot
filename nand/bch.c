#include <nand_boot/nandop.h>
#include <regmap.h>
#include <common.h>

/**************************************************************************
 *                             M A C R O S                                *
 **************************************************************************/
#define NANDREG_W(reg, value)	Xil_Out32(reg, value)
#define NANDREG_R(reg) 	    	Xil_In32(reg)

int bch_s338_process(UINT8 *data, UINT8 *parity, int data_size, int codec_mode, int correct_mode, int software_byte)
{
	UINT32 val;
	UINT8 sector_num;
	UINT32 _repeat_=0;

	val = BCH_S338_RESET;	
	NANDREG_W(BCH_S338_SOFT_RESET, val);
	while((NANDREG_R(BCH_S338_SOFT_RESET)&BCH_S338_SOFT_RESET)!=0);


	NANDREG_W(BCH_S338_DATA_PTR, (UINT32)data);
	NANDREG_W(BCH_S338_PARITY_PTR, (UINT32)parity);

	val = ~(BCH_S338_FINISH_MASK(1) | BCH_S338_DECODE_FAIL_MASK(1));
	NANDREG_W(BCH_S338_INT_MASK, val);

	if ( (correct_mode==BCH_S338_512B8_BITS_MODE) || (correct_mode==BCH_S338_512B4_BITS_MODE) ) {
		sector_num = (data_size >> 9) - 1;
	} else {
		sector_num = (data_size >> 10) - 1;
	}

	NANDREG_W(BCH_S338_INT_STATUS, 1);

	val = BCH_S338_SECTOR_NUMBER(sector_num) |
		BCH_S338_CORRECT_MODE(correct_mode) |
		BCH_S338_SOFTWARE_PROTECTED_BYTES(software_byte) |
		BCH_S338_ENC_DEC(codec_mode);

	//DBGPRINT("Before Trigger : rBCH_S338_REPORT_STATUS(0x%x)\n", rBCH_S338_REPORT_STATUS);

	NANDREG_W(BCH_S338_CFG, val);

	val |= BCH_S338_START;

	NANDREG_W(BCH_S338_CFG, val);

	// polling busy
	while((NANDREG_R(BCH_S338_INT_STATUS)&BCH_S338_INT)==0);
	

	if (codec_mode == BCH_DECODE) {
		if ((NANDREG_R(BCH_S338_REPORT_STATUS) & BCH_S338_DECODE_FAIL) == 0) {
			//unsigned int last_address;
			//unsigned int last_data;
			//unsigned int timeout_count;

#if 0
			if (((rBCH_S338_REPORT_STATUS>>8) & 0x7ff) != 0) {
				timeout_count = 0;
				do {
					last_address = rBCH_S338_LAST_ADDRESS_REG;
					last_data = rBCH_S338_LAST_DATA_REG;
					if (last_data == 0xffffffff) {
						//prn_string("last address = ");prn_dword(last_address);prn_string("\n");
						//prn_string("last address's data = ");prn_dword(*(volatile unsigned int *)last_address);prn_string("\n");
						//prn_string("last data = ");prn_dword(last_data);prn_string("\n");
						break;
					}

					timeout_count++;
					if (timeout_count > 100) {
						break;
					}
				}while(*(volatile unsigned int *)last_address != last_data);
			}
#endif

			//DBGPRINT("decode success rFM_INTRMSK(0x%x)\n",rFM_INTRMSK);
			//DBGPRINT("decode success rFM_INTR_STS(0x%x)\n",rFM_INTR_STS);
			//DBGPRINT("decode success, error count = %d\n", rBCH_S338_ERROR_COUNT_REG);
			return ret_BCH_S338_OK;
		} else {
			//DBGPRINT("decode fail rFM_INTRMSK(0x%x) is Timeout\n",rFM_INTRMSK);
			DBGPRINT("decode fail, rBCH_S338_REPORT_STATUS(0x%x), error count = %d, 00_flag = %x, ff_flag = %x\n", NANDREG_R(BCH_S338_REPORT_STATUS), NANDREG_R(BCH_S338_ERROR_COUNT_REG), (NANDREG_R(BCH_S338_REPORT_STATUS)>>28)&0x01, (NANDREG_R(BCH_S338_REPORT_STATUS)>>24)&0x01);

			NANDREG_W(BCH_S338_SOFT_RESET, 1);
			NANDREG_W(BCH_S338_REPORT_STATUS, BCH_S338_DECODE_FAIL);

			_repeat_ = 0;

			while(NANDREG_R(BCH_S338_SOFT_RESET) != 0) {
				if (_repeat_ >= MAX_BCH_WAIT_CNT) {
					return ret_BCH_S338_FAIL;
				}
				_repeat_ += BCH_WAIT_CNT;
				DelayUS(BCH_WAIT_CNT);
			}

			return ret_BCH_S338_FAIL;
		}
	}

	if (codec_mode == BCH_ENCODE) {
		return ret_BCH_S338_OK;
	}

	return ret_BCH_S338_FAIL;
}

int bch_s338_config(UINT8 *data, UINT8 *parity, int data_size, int codec_mode, int correct_mode, int software_byte)
{	
	UINT32 val;	
	UINT8 sector_num;	

	val = BCH_S338_RESET;	
	NANDREG_W(BCH_S338_SOFT_RESET, val);	
	while((NANDREG_R(BCH_S338_SOFT_RESET)&BCH_S338_RESET)!=0);

	NANDREG_W(BCH_S338_DATA_PTR, (UINT32)data);	
	NANDREG_W(BCH_S338_PARITY_PTR, (UINT32)parity);	

	val = ~(BCH_S338_FINISH_MASK(1) | BCH_S338_DECODE_FAIL_MASK(1));
	NANDREG_W(BCH_S338_INT_MASK, val);	

	if ( (correct_mode==BCH_S338_512B8_BITS_MODE) || (correct_mode==BCH_S338_512B4_BITS_MODE) ) 
	{
		sector_num = (data_size >> 9) - 1;	
	} 
	else 
	{
		sector_num = (data_size >> 10) - 1;	
	}	

	NANDREG_W(BCH_S338_INT_STATUS, 1);
	val = BCH_S338_SECTOR_NUMBER(sector_num)|
		BCH_S338_CORRECT_MODE(correct_mode) |
		BCH_S338_SOFTWARE_PROTECTED_BYTES(software_byte) |
		BCH_S338_ENC_DEC(codec_mode);
	
	NANDREG_W(BCH_S338_CFG, val);

	val |= (1<<1); //auto bch mode ,config ready bit
	NANDREG_W(BCH_S338_CFG, val);

	return ret_BCH_S338_OK;
}


SINT32 BCHProcess(UINT32* PyldBuffer, UINT32* ReduntBuffer, UINT32 len, SINT32 op, SINT32 ecc_mode)
{
	if(getSDevinfo(SUPPORTBCHFLG)) {
		return bch_s338_process((UINT8 *)PyldBuffer, (UINT8 *)ReduntBuffer, len, op, ecc_mode, g_bootinfo.software_protect_byte[ecc_mode]);
	}

	return ret_BCH_S338_OK;
}

SINT32 BCHConfig(UINT32* PyldBuffer, UINT32* ReduntBuffer, UINT32 len, SINT32 op, SINT32 ecc_mode)
{
	
	if(getSDevinfo(SUPPORTBCHFLG)) {
		return bch_s338_config((UINT8 *)PyldBuffer, (UINT8 *)ReduntBuffer, len, op, ecc_mode, g_bootinfo.software_protect_byte[ecc_mode]);
	}

	return ret_BCH_S338_OK;
}


SINT32 BCHCheckStatus(int codec_mode)
{
	if (codec_mode == BCH_DECODE) 
	{
		if ((NANDREG_R(BCH_S338_REPORT_STATUS) & BCH_S338_DECODE_FAIL) == 0) 
		{
			return ret_BCH_S338_OK;
		} 
		else 
		{
			//DBGPRINT("decode fail rFM_INTRMSK(0x%x) is Timeout\n",rFM_INTRMSK);
			DBGPRINT("decode fail, rBCH_S338_REPORT_STATUS(0x%x), error count = %d, 00_flag = %x, ff_flag = %x\n", NANDREG_R(BCH_S338_REPORT_STATUS), NANDREG_R(BCH_S338_ERROR_COUNT_REG), (NANDREG_R(BCH_S338_REPORT_STATUS)>>28)&0x01, (NANDREG_R(BCH_S338_REPORT_STATUS)>>24)&0x01);
	
			NANDREG_W(BCH_S338_SOFT_RESET, 1);
			NANDREG_W(BCH_S338_REPORT_STATUS, BCH_S338_DECODE_FAIL);
			return ret_BCH_S338_FAIL;
		}
	}

	return ret_BCH_S338_OK;
}



