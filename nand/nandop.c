/******************************************************************************
 *                          Include File
 *******************************************************************************/
#include <nand_boot/nfdriver.h>
#include <nand_boot/nandop.h>
#include <regmap.h>
#include <common.h>
#include <bootmain.h>
#include <tcpsum.h>

/**************************************************************************
 *                        G L O B A L   D A T A                           *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L   R E F E R E N C E S                  *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N   D E C L A R A T I O N S                *
 **************************************************************************/
extern unsigned int getBootDevID(void);
extern void setEccMode(UINT8 mode);
extern void hwNandCmdSend(UINT8 which_cs, UINT8 cmd);
extern UINT32 nfvalshift(UINT32 x);

void MainInitBufferAddr_nand(void)
{
	//init initial value of global variables
	g_bootinfo.gInitNand_flag = 0;
	g_bootinfo.gheader_already_read_out_flag = 0;

	if (getBootDevID() == DEVICE_PARA_NAND) {
		g_pSysInfo->gNANDACReadTiming = 0x18187c44; //0x1f1f9f55; //0x1f0fffff;
		g_pSysInfo->gNANDACWriteTiming = 0x18187c44; //0x1f1f9f55; //0x1f0fffff;
	} else {
		g_pSysInfo->gNANDACReadTiming = 0x4b4bcc31; //0x5b5bff42; //0x1f0fffff;
		g_pSysInfo->gNANDACWriteTiming = 0x4b4bcc31; //0x5b5bff42; //0x1f0fffff;
	}

	g_bootinfo.software_protect_byte[0]=5;//23;
	g_bootinfo.software_protect_byte[1]=2;//10;
	g_bootinfo.software_protect_byte[2]=1;//6;
	g_bootinfo.software_protect_byte[3]=0;//4;
	g_bootinfo.software_protect_byte[4]=0;//2;
	g_bootinfo.software_protect_byte[5]=2;//9;

	#ifndef PARTIAL_NAND_INIT
	rd_memset((UINT8*)g_boothead, 0, sizeof(struct BootProfileHeader));
	#endif
}

SDev_t* getSDev(void)
{
        return &g_bootinfo.gsdev;
}

UINT8 getSDevinfo(UINT8 flg)
{
        SDev_t* pSDev = getSDev();
        switch(flg) {
        case SUPPORTBCHFLG:
                return pSDev->IsSupportBCH;
        default://UNKNOWN
                break;
        }
        return 0;
}

void DelayUS(UINT32 delay)
{
#if 1
	STC_delay_us(delay);
#else /* Use CPU nop wait */
	UINT32 k, nsecs;
#ifdef CSIM_NEW
	nsecs = 1;
#else
	nsecs = delay << 10;
#endif

	for (k=0; k < nsecs; k++) {
		__asm__ volatile ("nop");
	}
#endif
}

UINT8 GetNANDPageCount_1K60(UINT32 pageSize)
{
	if (pageSize == 2048) return 1;
	else if (pageSize == 4096) return 3;
	else if (pageSize == 8192) return 7;
	else if (pageSize == 16384) return 14;

	return 14;
}

struct BootProfileHeader *Get_Header_Profile_Ptr(void)
{
	return ((struct BootProfileHeader *)g_boothead);
}

UINT32 Load_Header_Profile(SINT32 type)
{
	dbg();

#ifdef PARTIAL_NAND_INIT
	return ROM_SUCCESS; /* xboot */
#else
	BUILD_BUG_ON(sizeof(struct BootProfileHeader) != 256);

	struct BootProfileHeader *ptr = (struct BootProfileHeader *)g_boothead;
	UINT16 idx, maxidx, idxstep;
	u32 checksum;
	int ret = -1;

	if (g_bootinfo.gheader_already_read_out_flag == 1) {
		dbg();
		return ROM_SUCCESS;
	}

	dbg();

	idxstep = 4;  // page step
	maxidx = 32; // max idxstep

	// Search for BootProfileHeader
	for (idx = 0; idx < maxidx; idx += idxstep) {

		// verbose
		//prn_string("pg idx="); prn_dword(idx);

		if (type == PARA_NAND_BOOT) { // Parallel NAND
#ifdef CONFIG_HAVE_PARA_NAND
			ret = ReadPNANDSector_1K60((UINT32 *)g_pyldData, idx);
#endif
		} else { // SPI NAND
#ifdef CONFIG_HAVE_SPI_NAND
			ret = ReadSPINANDSector_1K60((UINT32 *)g_pyldData, idx);
#endif
		}

		// g_pyldData == g_io_buf.nand.data
		if (ret == ROM_SUCCESS) {

			// Copy from NAND buffer to global header
			memcpy32((UINT32 *)ptr, (UINT32 *)g_pyldData, sizeof(struct BootProfileHeader) / 4 );

			// verbose
			//prn_string("sig beg = "); prn_dword(ptr->Signature);
			//prn_string("sig end = "); prn_dword(ptr->EndSignature);

			if ((ptr->Signature == BHDR_BEG_SIGNATURE) &&
					(ptr->EndSignature == BHDR_END_SIGNATURE)) {

				// Verify Signature & Checksum
				checksum = tcpsum((u8 *)ptr, 252);

				if (checksum == ptr->CheckSum) {
					prn_string("=== Header (idx="); prn_decimal(idx); prn_string(") ===\n");
					prn_string("PageSize = "); 		prn_decimal_ln(ptr->PageSize); 			//prn_string("\n");
					prn_string("PagePerBlock = ");  prn_decimal_ln(ptr->PagePerBlock); 		//prn_string("\n");
					prn_string("ReduntSize = ");  	prn_decimal_ln(ptr->ReduntSize); 		//prn_string("\n");
					prn_string("BlockNum = "); 		prn_decimal_ln(ptr->BlockNum); 			//prn_string("\n");
					prn_string("addrCycle = "); 	prn_decimal(ptr->addrCycle); prn_string("\n");
					prn_string("BchType = "); 		prn_decimal_ln(ptr->BchType); 			//prn_string("\n");
					prn_string("SpecialMode = "); 	prn_dword(ptr->PlaneSelectMode); 		//prn_string("\n");
					prn_string("xboot_copies = "); 	prn_dword(ptr->xboot_copies);
					prn_string("xboot_pg_off = "); 	prn_dword(ptr->xboot_pg_off);
					prn_string("xboot_pg_cnt = "); 	prn_dword(ptr->xboot_pg_cnt);

					if (!ptr->xboot_pg_off || !ptr->xboot_pg_cnt || !ptr->xboot_copies) {
						// default xboot to Block 1
						ptr->xboot_pg_off = ptr->PagePerBlock;
						ptr->xboot_pg_cnt = XBOOT_BUF_SIZE / ptr->PageSize;
						if (ptr->xboot_pg_cnt > ptr->PagePerBlock)
							ptr->xboot_pg_cnt = ptr->PagePerBlock; // 1 block at most
						ptr->xboot_copies = 2;

						prn_string("default xboot to block 1, ");
						prn_decimal(ptr->xboot_pg_cnt); prn_string("pages, ");
						prn_decimal(ptr->xboot_copies); prn_string("copies\n");
					}

					if (ptr->BchType == 0xff) {
						SDev_t* pSDev = getSDev();
						pSDev->IsSupportBCH = 0;
					}

					g_bootinfo.gheader_already_read_out_flag = 1; /* header already read out, no need to read again */

					return ROM_SUCCESS;
				} else {
					prn_string("chksum= "); prn_dword(ptr->CheckSum);
					prn_string("calc chksum= "); prn_dword(checksum);
				}
			}
		}
	}

	prn_string("Header not found!\n");
	return ROM_FAIL;
#endif
}

void SetNANDACReadTiming(UINT32 val)
{
	g_pSysInfo->gNANDACReadTiming = val;
}

UINT32 GetNANDACReadTiming(void)
{
	return g_pSysInfo->gNANDACReadTiming;
}

void SetNANDACWriteTiming(UINT32 val)
{
	g_pSysInfo->gNANDACWriteTiming = val;
}

UINT32 GetNANDACWriteTiming(void)
{
	return g_pSysInfo->gNANDACWriteTiming;
}

void setEccMode_op(UINT8 mode)
{
	SDev_t* pSDev = getSDev();

	switch(pSDev->DeviceID) {
	case DEVICE_PARA_NAND:
		setEccMode(mode);
		break;
	default://UNKNOWN
		break;
	}
}

int InitNand(void)
{
	UINT8 ret;
	SDev_t* pSDev = getSDev();

	if (g_bootinfo.gInitNand_flag == 1) {
		return ROM_SUCCESS;
	}

	// always disable Randomize
	if (getBootDevID() == DEVICE_PARA_NAND)
		g_bootinfo.gRandomizeFlag = 0;

	dbg_info();
	ret = pSDev->predInitDriver();

	dbg_info();
	if(ret == ROM_SUCCESS) {
		dbg_info();
		g_bootinfo.gInitNand_flag = 1;
		return ROM_SUCCESS;
	} else {
		dbg_info();
		g_bootinfo.gInitNand_flag = 0;
		return ROM_FAIL;
	}
}

UINT32 nfvalshift(UINT32 x)
{
	int i=0;
	while(x) {
		x >>= 1;
		if(x)
			i++;
	}
	return i;
}

void setEccMode(UINT8 mode)
{
	if(getSDevinfo(SUPPORTBCHFLG)==0) {
		g_pSysInfo->ecc_mode = mode;
		g_pSysInfo->u16Redunt_Sector_Len = 0;
		g_pSysInfo->u16Redunt_Sector_Addr_Offset = 0;
		g_pSysInfo->u16ReduntLen = 0;
	} else {
		switch(mode) {
			case BCH_S338_1K60_BITS_MODE:
				DBGPRINT("BCH_S338_1K60_BITS_MODE\n");
				g_pSysInfo->ecc_mode = BCH_S338_1K60_BITS_MODE;
				g_pSysInfo->u16Redunt_Sector_Len = 128;
				g_pSysInfo->u16Redunt_Sector_Addr_Offset = 128;
				break;

			case BCH_S338_1K40_BITS_MODE:
				DBGPRINT("BCH_S338_1K40_BITS_MODE\n");
				g_pSysInfo->ecc_mode = BCH_S338_1K40_BITS_MODE;
				g_pSysInfo->u16Redunt_Sector_Len = 80;
				g_pSysInfo->u16Redunt_Sector_Addr_Offset = 96;
				break;

			case BCH_S338_1K24_BITS_MODE:
				DBGPRINT("BCH_S338_1K24_BITS_MODE\n");
				g_pSysInfo->ecc_mode = BCH_S338_1K24_BITS_MODE;
				g_pSysInfo->u16Redunt_Sector_Len = 48;
				g_pSysInfo->u16Redunt_Sector_Addr_Offset = 64;
				break;

			default:
			case BCH_S338_1K16_BITS_MODE:
				DBGPRINT("BCH_S338_1K16_BITS_MODE\n");
				g_pSysInfo->ecc_mode = BCH_S338_1K16_BITS_MODE;
				g_pSysInfo->u16Redunt_Sector_Len = 32;
				g_pSysInfo->u16Redunt_Sector_Addr_Offset = 64;
				break;

			case BCH_S338_512B8_BITS_MODE:
				DBGPRINT("BCH_S338_512B8_BITS_MODE\n");
				g_pSysInfo->ecc_mode = BCH_S338_512B8_BITS_MODE;
				g_pSysInfo->u16Redunt_Sector_Len = 16;
				g_pSysInfo->u16Redunt_Sector_Addr_Offset = 32;
				break;

			case BCH_S338_512B4_BITS_MODE:
				DBGPRINT("BCH_S338_512B4_BITS_MODE\n");
				g_pSysInfo->ecc_mode = BCH_S338_512B4_BITS_MODE;
				g_pSysInfo->u16Redunt_Sector_Len = 16;
				g_pSysInfo->u16Redunt_Sector_Addr_Offset = 32;
				break;
		}
	}
}

void setSystemPara(UINT8 *prData)
{
	struct BootProfileHeader *ptr = Get_Header_Profile_Ptr();

	g_pSysInfo->u16PageSize = ptr->PageSize;
	g_pSysInfo->u16PyldLen  = ptr->PageSize;
	g_pSysInfo->u16ReduntLen = ptr->ReduntSize;
	//g_pSysInfo->u16PageNoPerBlk = ptr->PagePerBlockWithGap; //depricated
	g_pSysInfo->u16PageNoPerBlk = ptr->PagePerBlock;
	g_pSysInfo->u16TotalBlkNo = ptr->BlockNum;

	g_pSysInfo->u8addrCycle = ptr->addrCycle;

	g_pSysInfo->u8Support_TwoPlan =0;
	g_pSysInfo->u8Internal_Chip_Number = 0;
	g_pSysInfo->u8Support_Internal_Interleave = 0;
	g_pSysInfo->u8Support_External_Interleave = 0;

	CSTAMP(0xAA55DD0A);
	g_pSysInfo->u8TotalBlkNoShift = nfvalshift(g_pSysInfo->u16TotalBlkNo);
	CSTAMP(0xAA55DE01);
	g_pSysInfo->u8PagePerBlkShift = nfvalshift(g_pSysInfo->u16PageNoPerBlk);

	CSTAMP(0xAA55DD0B);
	setEccMode_op(ptr->BchType);
	CSTAMP(0xAA55DD0C);
}
