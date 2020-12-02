#include <types.h>
#include <fat/fat.h>
#include <config.h>
#include <common.h>
#include <usb/ehci_usb.h>
#include <sdmmc_boot/drv_sd_mmc.h>

/********************************************************************************
 * FAT32 and short file name only
 ********************************************************************************/

static const unsigned char FILENAMES[FAT_FILES][12] =
{
	"ISPBOOOTBIN","U-BOOT  IMG","UIMAGE     ","DTB        "
};

static u32 search_fat32_files(fat_info *info, u8 *buffer, u8 type);
#ifdef FAT16_SUPPORT
static u32 search_fat16_files(fat_info *info, u8 *buffer, u8 type);
#endif

static u32 next_cluster(fat_info *info, u32 currentClus, u8 *buffer)
{
	u32 cluster = currentClus;
	u32 sector = 0;
	fat32table_info *fat32table;
#ifdef FAT16_SUPPORT
	fat16table_info *fat16table;
#endif

	if (info->fatType == FAT_32) {
		sector = info->fat1Sect +
				(cluster >> (info->bytePerSectInPower - 2));
	}
#ifdef FAT16_SUPPORT
	else if (info->fatType == FAT_16) {
		sector = info->fat1Sect +
				(cluster >> (info->bytePerSectInPower - 1));
	}
#endif

	info->read_sector(sector, 1, (u32 *)buffer);

	if (info->fatType == FAT_32) {
		fat32table = (fat32table_info *)(buffer);
		cluster = fat32table->fat[cluster & ((info->bytePerSect >> 2) - 1)];	/* X mod 2^n == X & (2^n - 1) */
	}
#ifdef FAT16_SUPPORT
	else if (info->fatType == FAT_16) {
		fat16table = (fat16table_info *)(buffer);
		cluster = fat16table->fat[cluster & ((info->bytePerSect >> 1) - 1)];	/* X mod 2^n == X & (2^n - 1) */
	}
#endif

	return cluster;
}

/*
 * fat_read_file
 * @idx:	file number
 * @info:	fat_info structure 
 * @buffer	io buffer whose size is equal to sector size
 * @offset	start position of target file
 * @length	read length
 * @dest	destination buffer
 */
u32 fat_read_file(u32 idx, fat_info *info, u8 *buffer, u32 offset, u32 length, u8 *dest)
{
	u32 j, txfer_sects = 1;
	u32 sector = 0;
	u32 off = offset >> info->bytePerSectInPower;
	u32 remain = offset & (info->bytePerSect -1);
	u32 size = length;
	u32 cluster;
	u32 bytesPerClus = info->bytePerSect * info->sectPerClus;
	u32 count;

	if (size == 0) {
		return FAIL;
	}

	/* go to target cluster */
	cluster = info->fileInfo[idx].cluster;
	while (off >= info->sectPerClus) {
		if (((info->fatType == FAT_32) && (cluster >= 0x0FFFFFF8))
#ifdef FAT16_SUPPORT
			|| ((info->fatType == FAT_16) && (cluster >= 0xFFF5))
#endif
			) {
			return FAIL;
		}

		cluster = next_cluster(info, cluster, (u8*)buffer);
		off -= info->sectPerClus;
	}

	/* deal with blocks whose size is smaller than one sector */
	if (remain != 0) {
		if ((remain + length) < info->bytePerSect) {
			size = 0;
			count = remain + length;
		} else {
			size -= (info->bytePerSect - remain);
			count = info->bytePerSect;
		}

		if (info->fatType == FAT_32) {
			sector = info->clust0Sect +
					((cluster - info->rootClus) * info->sectPerClus);
		}
#ifdef FAT16_SUPPORT
		else if (info->fatType == FAT_16) {
			sector = info->rootSectStart + info->rootSect +
					((cluster - FAT_DATA_1ST_CLUS_NUM) * info->sectPerClus);
		}
#endif

		info->read_sector((sector + off), 1, (u32 *)buffer);

		for (j = remain; j < count; j++) {
			dest[j - remain] = buffer[j];
		}
		off += 1;   /* finished one sector */
		dest += (info->bytePerSect - remain);

		if ((off & (info->sectPerClus -1)) == 0) {
			cluster = next_cluster(info, cluster, (u8*)buffer);
			off = 0;
		}

		/* when size is eqaul to zero, job done! */
		if (size == 0) {
			return PASS;
		}
	}

	/* deal with blocks whose size is smaller than a cluster */
	if (off & (info->sectPerClus - 1)) { // Not on cluster
		if (info->fatType == FAT_32) {
			sector = off + info->clust0Sect +
					((cluster - info->rootClus) * info->sectPerClus);
		}
#ifdef FAT16_SUPPORT
		else if (info->fatType == FAT_16) {
			sector = off + info->rootSectStart + info->rootSect +
					((cluster - FAT_DATA_1ST_CLUS_NUM) * info->sectPerClus);
		}
#endif

		// read until next cluster
		while (off & (info->sectPerClus - 1)) {
			if (size < info->bytePerSect) {
				info->read_sector(sector, 1, (u32 *)buffer);
				memcpy(dest, buffer, size);
				return PASS;
			}

			info->read_sector(sector, 1, (u32 *)dest);
			sector++;
			off++;
			dest += info->bytePerSect;
			size -= info->bytePerSect;
		}
		cluster = next_cluster(info, cluster, (u8 *)buffer);
	}

#if defined(CONFIG_HAVE_USB_DISK) && defined(FAT_USB_4K_READ)
	/* USB: transfer clusters in 4K unit to speed up */
	if (info->read_sector == usb_readSector && !(info->sectPerClus % 8)) {
		txfer_sects = 8;
	}
#endif

	/* deal with blocks whose size is bigger or equal to a cluster */
	while (size >= bytesPerClus) {
		size -= bytesPerClus;
		if (info->fatType == FAT_32) {
			sector = info->clust0Sect +
					((cluster - info->rootClus) * info->sectPerClus);
		}
#ifdef FAT16_SUPPORT
		else if (info->fatType == FAT_16) {
			sector = info->rootSectStart + info->rootSect +
					((cluster - FAT_DATA_1ST_CLUS_NUM) * info->sectPerClus);
		}
#endif

		for (j = 0; j < info->sectPerClus; j += txfer_sects) {
			info->read_sector((sector + j), txfer_sects, (u32 *)dest);
			dest += (txfer_sects * info->bytePerSect);
		}

		if (size == 0) {
			return PASS;
		} else {
			cluster = next_cluster(info, cluster, (u8 *)buffer);
		}
	}

	if (info->fatType == FAT_32) {
		sector = info->clust0Sect +
				((cluster - info->rootClus) * info->sectPerClus);
	}
#ifdef FAT16_SUPPORT
	else if (info->fatType == FAT_16) {
		sector = info->rootSectStart + info->rootSect +
				((cluster - FAT_DATA_1ST_CLUS_NUM) * info->sectPerClus);
	}
#endif

	/* deal with remaining sectors which is smaller than a cluster*/
	while (size >= info->bytePerSect) {
		info->read_sector(sector, 1, (u32 *)dest);
		sector++;
		dest += info->bytePerSect;
		size -= info->bytePerSect;
	}

	if (size == 0) {
		return PASS;
	} else {
		info->read_sector(sector, 1, (u32 *)buffer);
		memcpy(dest, buffer, size);
	}
	return PASS;
}

#if 0	//for debug
static void prn_data(u8 *buffer, int length)
{
	int i;
	u32 j = 0;	/* initial address */

	for (i = 0; i < length; i++) {
		if (i == 0) {
			prn_dword0(j); prn_string(" : ");
			prn_byte(buffer[i]);
			j += 0x10;
		} else if ((((i + 1) % 16) == 0)) {
			prn_byte(buffer[i]);
			prn_string("\n");

			if (i < (length - 1)) {
				prn_dword0(j); prn_string(" : ");
				j += 0x10;
			}
		} else if (((i + 1) % 8) == 0) {
			prn_byte(buffer[i]);
			prn_string(" ");
		} else
			prn_byte(buffer[i]);
	}

	if ((length % 16) != 0)
		prn_string("\n");
}
#endif

/*
 * fat_boot
 * @type	USB_ISP or SDCARD_ISP
 * @info	Returned struct fat_info
 * @buffer	IO buffer
 */
u32 fat_boot(u32 type, u32 port, fat_info *info, u8 *buffer)
{
	fat32_bpb *bpb32;
#ifdef FAT16_SUPPORT
	fat16_bpb *bpb16;
#endif
	u32 tmp;
	u32 ret;
	int entry = 0;

	CSTAMP(0xFAB00000);

	dbg_info();
	info->startSector = 0;
	info->bytePerSect = 0;
	info->init = NULL;
	info->fatType = FAT_UNKNOW;

	/* Set init function and read_sector function*/
	if (type == USB_ISP) {
#ifdef CONFIG_HAVE_USB_DISK
		dbg_info();
		info->init = usb_init;
		info->read_sector = usb_readSector;
#endif
	} else if (type == SDCARD_ISP) {
#ifdef CONFIG_HAVE_SDCARD
		dbg_info();
		info->init = initDriver_SD;
		info->read_sector = ReadSDSector;
#endif
	}

	if (info->init == NULL) {
		dbg_info();
		return FAIL;
	}

	memset(info->fileInfo,0,sizeof(info->fileInfo));
	
	CSTAMP(0xFAB00001);

	/*
	 * Initialization
	 */
	if (info->init(port) != ROM_SUCCESS) {
		dbg_info();
		return FAIL;
	}

	CSTAMP(0xFAB00002);
	dbg_info();

	/*
	 * Get first sector
	 */
	ret = info->read_sector(info->startSector, 1, (u32 *)buffer);
	if (ret != ROM_SUCCESS) {
		dbg();
		return FAIL;
	}

	CSTAMP(0xFAB00003);
	dbg_info();

#if 0	//for debug
	prn_string("/* MBR */\n");
	prn_data(buffer, 512);
#endif

	/*
	 * Check MBR
	 */

	// MBR
	bpb32 = (fat32_bpb *)buffer;
#ifdef FAT16_SUPPORT
	bpb16 = (fat16_bpb *)buffer;
#endif

check_partition_entry:
	// MBR criteria:
	// 1. signature        : 55 aa
	// 2. partition status : 80=active, 00=inactive, 00~7f=invalid
	// 3. partition type   : non-zero
	if (buffer[0x1fe] == 0x55 && buffer[0x1ff] == 0xaa &&
		(buffer[0x1be + entry * FAT_PART_ENTRY_SIZE] & ~0x80) == 0 &&
		buffer[0x1c2 + entry * FAT_PART_ENTRY_SIZE] != 0 &&
		buffer[0x1c2 + entry * FAT_PART_ENTRY_SIZE] != 0xF) {
		/* get start sector */
		info->startSector = buffer[457 + entry * FAT_PART_ENTRY_SIZE] << 24 |
			buffer[456 + entry * FAT_PART_ENTRY_SIZE] << 16 |
			buffer[455 + entry * FAT_PART_ENTRY_SIZE] << 8  |
			buffer[454 + entry * FAT_PART_ENTRY_SIZE];

		prn_string("part"); prn_byte((u8)entry); prn_string("@");
		prn_dword(info->startSector);

		/* get BPB */
		info->read_sector(info->startSector, 1, (u32 *)buffer);
	} else {
		if (buffer[0x1fe] != 0x55 || buffer[0x1ff] != 0xaa ||
			(buffer[0x1be + entry * FAT_PART_ENTRY_SIZE] & ~0x80) != 0)
			prn_string("no MBR\n");
		else if (buffer[0x1c2 + entry * FAT_PART_ENTRY_SIZE] == 0)
			prn_string("no PARTITION\n");
		else if (buffer[0x1c2 + entry * FAT_PART_ENTRY_SIZE] == 0xF)
			prn_string("extended PARTITION (not supported)\n");

		return FAIL;
	}

	dbg_info();

#if 0	//for debug
	prn_string("/* DBR */\n");
	prn_data(buffer, 512);
#endif

	/*
	 * check FAT32
	 */
	if ((((bpb32->systemType[1] << 16) | bpb32->systemType[0]) == FAT32_L) &&
				(((bpb32->systemType[3] << 16) | bpb32->systemType[2]) == FAT32_U)) {
		info->fatType = FAT_32;

		/* Get infomation of BPB */
		info->bytePerSect = (bpb32->bytePerSect[1] << 8) + bpb32->bytePerSect[0];
		info->sectPerClus = bpb32->sectPerClus;
		info->numOfReserveSect = bpb32->sectResearve;
		info->numFatCopy = bpb32->numFat;
		info->sectPerFat =bpb32->sectPerFat;
		info->rootClus = bpb32->rootClusNum;

		info->fat1Sect = info->startSector + info->numOfReserveSect;
		info->clust0Sect = info->fat1Sect + (info->sectPerFat * info->numFatCopy);

		prn_string("FAT32 file system\n");
	}
#ifdef FAT16_SUPPORT
	else if ((((bpb16->systemType[1] << 16) | bpb16->systemType[0]) == FAT16_L) &&
				(((bpb16->systemType[3] << 16) | bpb16->systemType[2]) == FAT16_U)) {
		info->fatType = FAT_16;

		/* Get infomation of BPB */
		info->bytePerSect = ((u16)bpb16->bytePerSect[1] << 8) + bpb16->bytePerSect[0];
		info->sectPerClus = bpb16->sectPerClus;
		info->numOfReserveSect = bpb16->sectResearve;
		info->numFatCopy = bpb16->numFat;
		info->sectPerFat = ((u32)bpb16->fatSz16[1] << 8) + bpb16->fatSz16[0];
		info->rootEntCnt = ((u16)bpb16->rootEntCnt[1] << 8) + bpb16->rootEntCnt[0];
		info->rootSectStart = info->startSector + info->numOfReserveSect + (info->sectPerFat * info->numFatCopy);
		info->rootSect = ((u32)info->rootEntCnt * 32 + (u32)info->bytePerSect - 1) / (u32)info->bytePerSect;
		info->fat1Sect = info->startSector + info->numOfReserveSect;

		prn_string("FAT16 file system\n");
	} else {
		prn_string("Not FAT32/FAT16\n");
		return FAIL;
		/* Not FAT32/FAT16...*/
	}
#else
	else {
		prn_string("Not FAT32\n");
		return FAIL;
		/* Not FAT32...*/
	}
#endif

	/* Because of lack of mod and divide operations */
	tmp = info->bytePerSect;
	info->bytePerSectInPower = 0;
	while(tmp > 1) {
		tmp = tmp >> 1;
		(info->bytePerSectInPower)++;
	}

	/*
	 * Search files
	 */
	dbg_info();
	if (info->fatType == FAT_32)
		ret = search_fat32_files(info, (u8*)buffer, type);
#ifdef FAT16_SUPPORT
	else if (info->fatType == FAT_16)
		ret = search_fat16_files(info, (u8*)buffer, type);
#endif

	if (ret == FAIL) {
		info->startSector = 0;
		info->bytePerSect = 0;
		info->fatType = FAT_UNKNOW;
		entry++;

		if (entry >= FAT_MAX_PARTITION) {
			dbg_info();
			return FAIL;
		}

		ret = info->read_sector(info->startSector, 1, (u32 *)buffer);
		if (ret != ROM_SUCCESS) {
			dbg();
			return FAIL;
		}

		goto check_partition_entry;
	}
	return ret;
}

static u32 search_fat32_files(fat_info *info, u8 *buffer, u8 type)
{
	u32 i;
	u32 count;
	u32 sectOffset;
	u32 nextClus;
	u32 fdbOffset;
	u32 currentSect;
	fdb_info *fdb;
	u32 filecount = (type == SDCARD_ISP) ? FAT_FILES : 1;

	dbg_info();
	count = 0;
	nextClus = info->rootClus;
	while (nextClus < 0x0FFFFFF8) {
		dbg_info();
		sectOffset = 0;
		while (sectOffset < info->sectPerClus) {
			dbg_info();
			fdbOffset = 0;
			currentSect = info->clust0Sect +
				((nextClus - info->rootClus) * info->sectPerClus) +
				sectOffset;
			info->read_sector(currentSect, 1, (u32 *)buffer);
			while (fdbOffset < info->bytePerSect) {
				fdb = (fdb_info*)(&buffer[fdbOffset]);
				for (i = 0; i < filecount; i++) {
					if (memcmp((u8 *)fdb->name, FILENAMES[i], FAT_FILENAMELEN) == 0) {
						info->fileInfo[i].size = fdb->fileSize;
						info->fileInfo[i].cluster = (fdb->clusterH << 16) + fdb->clusterL;
						info->fileInfo[i].sectPos = info->clust0Sect +
							((info->fileInfo[i].cluster - info->rootClus) * info->sectPerClus);
						count++;
						if(type == USB_ISP && info->fileInfo[0].size != 0) {
							dbg_info();
							return PASS;
						}
						/* sdcard boot,check ispbooot.bin u-boot uimage dtb */
						if (type == SDCARD_ISP && count == filecount) {
							dbg_info();
							/*
							// only one return type
							if (fat_type == CARD0_ISP || fat_type == CARD1_ISP ||
								fat_type == CARD2_ISP || fat_type == CARD3_ISP)
								return PASS2;
							*/
							return PASS;
						}
					}
				}
				fdbOffset += FAT_FDB_SIZE;
			}
			sectOffset++;
		}
		/* next cluster */
		nextClus = next_cluster(info, nextClus, (u8*)buffer);
	}

	/* sdcard isp ,check ispbooot.bin. sdcard boot,check ispbooot.bin u-boot uimage dtb  */
	if(type == SDCARD_ISP && info->fileInfo[0].size !=0)
	{
		dbg_info();
		return PASS;
	}
	dbg_info();
	/* can't find files that we need.... */
	return FAIL;
}

#ifdef FAT16_SUPPORT
static u32 search_fat16_files(fat_info *info, u8 *buffer, u8 type)
{
	u32 i;
	u32 count;
	u32 sectOffset;
	u32 fdbOffset;
	fdb_info *fdb;
	u32 filecount = (type == SDCARD_ISP) ? FAT_FILES : 1;

	dbg_info();
	count = 0;
	sectOffset = 0;

	while (sectOffset < info->rootSect) {
		dbg_info();
		fdbOffset = 0;
		info->read_sector(info->rootSectStart + sectOffset, 1, (u32 *)buffer);
		while (fdbOffset < info->bytePerSect) {
			fdb = (fdb_info *)(&buffer[fdbOffset]);
			for (i = 0; i < filecount; i++) {
				if (memcmp((u8 *)fdb->name, (u8 *)FILENAMES[i], FAT_FILENAMELEN) == 0) {
					info->fileInfo[i].size = fdb->fileSize;
					info->fileInfo[i].cluster = fdb->clusterL;
					info->fileInfo[i].sectPos = info->rootSectStart + info->rootSect +
									(info->fileInfo[i].cluster - FAT_DATA_1ST_CLUS_NUM) *
										info->sectPerClus;
					count++;
					if(type == USB_ISP && info->fileInfo[0].size != 0) {
						dbg_info();
						return PASS;
					}
					/* sdcard boot, check ispbooot.bin u-boot uimage dtb */
					if (type == SDCARD_ISP && count == filecount) {
						dbg_info();
						/*
						// only one return type
						if (fat_type == CARD0_ISP || fat_type == CARD1_ISP ||
							fat_type == CARD2_ISP || fat_type == CARD3_ISP)
							return PASS2;
						*/
						return PASS;
					}
				}
			}
			fdbOffset += FAT_FDB_SIZE;
		}
		sectOffset++;
	}
	dbg_info();
	/* can't find files that we need.... */
	return FAIL;
}
#endif

/*sdcard do isp or boot is decide by ISPBOOOT.BIN size
  do isp: ISPBOOOT.BIN size > xboot.img size (64k);
  do boot: ISPBOOOT.BIN size == xboot.img size (64k);
  for debug Uboot, do not check file
*/
u8 fat_sdcard_check_boot_mode(fat_info *info)
{

	if(info->fileInfo[0].size != 0x10000) 
	{
		dbg_info();
		return FALSE;
	}
	return TRUE;
}

