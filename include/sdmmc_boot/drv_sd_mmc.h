#ifndef DRV_SD_MMC_H
#define DRV_SD_MMC_H

#define MAX_SD_COUNT 8

int initDriver_SD(int port);

int ReadSDMultipleSector(
	unsigned int SectorIdx,
	unsigned int SectorNum,
	unsigned char* pRecBuff
);

int ReadSDSector(
	unsigned int sectorNo,
	unsigned int pageCount,
	unsigned int* ptrPyldData
);

#endif		//DRV_SD_MMC_H
