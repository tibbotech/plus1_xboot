#include <nand_boot/nandop.h>
#include <sdmmc_boot/drv_sd_mmc.h>
#include <sdmmc_boot/hal_sd_mmc.h>
#include <sdmmc_boot/hw_sd.h>
#include <common.h>
#include <config.h>

// This constant is to limit infinite read retry
// while still allowing rd_clk_dly adjustment.
#define MAX_SD_READ_RETRY    32

extern int get_card_number(void);

int initDriver_SD(int port)
{
	int ret;
	int retry = 8;

	memset((u8 *)&gStorage_dev, 0, sizeof(gStorage_dev));

	dbg();
	InitChipCtrl();
	dbg();
RETRY_IDENTIFY:
	ret = IdentifyStorage();
	if(ret != SD_SUCCESS){
		if((ret != SD_RSP_TIMEOUT) && (retry-->0)){
			dbg();
			goto RETRY_IDENTIFY;
		}
	}

	dbg();
	return !!ret;
}

int ReadSDSector(unsigned int sectorNo, unsigned int pageCount, unsigned int *ptrPyldData)
{
	unsigned int SectorIndex = 0;
	int Ret = 0;
	int retry = 0;

	SectorIndex = sectorNo;
_Label_ReadSDSector_Again:
	SD_RST();
#if defined(CONFIG_HAVE_EMMC) && defined(EMMC_USE_DMA_READ)
	if (EMMC_SLOT_NUM == get_card_number())
		Ret = ReadSDMultipleSectorDma(SectorIndex, pageCount, (unsigned char*)ptrPyldData);
	else
#endif
		Ret = ReadSDMultipleSector(SectorIndex, pageCount, (unsigned char*)ptrPyldData);
	if (Ret != ERR_SUCCESS) {
		dbg();
		if (retry++ < MAX_SD_READ_RETRY) {
			prn_string("sdRead: retry="); prn_decimal(retry); prn_string("\n");
			goto _Label_ReadSDSector_Again;
		}
		return ROM_FAIL;
	}

	return Ret;
}

