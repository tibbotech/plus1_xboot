#ifndef DRV_SD_MMC_H
#define DRV_SD_MMC_H

enum mmc_area {
	MMC_USER_AREA = 0,
	MMC_BOOT_AREA1 = 1,
};

int initDriver_SD(int port, int mmc_part);

int ReadSDSector(
	unsigned int sectorNo,
	unsigned int pageCount,
	unsigned int* ptrPyldData
);

#endif		//DRV_SD_MMC_H
