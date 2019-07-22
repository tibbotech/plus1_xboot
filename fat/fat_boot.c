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
	"ISPBOOOTBIN","U-BOOT  IMG",
};

static u32 search_files(fat_info *info, u8 *buffer, u8 type);

static u32 next_cluster(fat_info *info, u32 currentClus, u8 *buffer)
{
	u32 cluster = currentClus;
	u32 sector;
	fatable_info *fatable;

	sector = info->fat1Sect +
		(cluster >> (info->bytePerSectInPower - 2));
	info->read_sector(sector, 1, (u32 *)buffer);
	fatable = (fatable_info *)(buffer);
	cluster = fatable->fat[cluster & ((info->bytePerSect >> 2) - 1)];	/* X mod 2^n == X & (2^n - 1) */

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
		if (cluster >= 0x0FFFFFF8) {
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

		sector = info->clust0Sect +
			((cluster - info->rootClus) * info->sectPerClus);
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
		sector = off + info->clust0Sect +
			((cluster - info->rootClus) * info->sectPerClus);

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
		cluster = next_cluster(info, cluster, (u8*)buffer);
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
		sector = info->clust0Sect +
			((cluster - info->rootClus) * info->sectPerClus);

		for (j = 0; j < info->sectPerClus; j += txfer_sects) {
			info->read_sector((sector + j), txfer_sects, (u32 *)dest);
			dest += (txfer_sects * info->bytePerSect);
		}

		if (size == 0) {
			return PASS;
		} else {
			cluster = next_cluster(info, cluster, (u8*)buffer);
		}
	}

	sector = info->clust0Sect +
		((cluster - info->rootClus) * info->sectPerClus);

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

/*
 * fat_boot
 * @type	USB_ISP or SDCARD_ISP
 * @info	Returned struct fat_info
 * @buffer	IO buffer
 */
u32 fat_boot(u32 type, u32 port, fat_info *info, u8 *buffer)
{
	fat32_bpb *bpb;
	u32 tmp;
	u32 ret;

	CSTAMP(0xFAB00000);

	dbg_info();
	info->startSector = 0;
	info->bytePerSect = 0;
	info->init = NULL;

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

	memcpy(g_bootinfo.fat_fileName[0], (u8 *)FILENAMES[0], 12);
	memcpy(g_bootinfo.fat_fileName[1], (u8 *)FILENAMES[1], 12);
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

	/*
	 * Check MBR
	 */

	// MBR or BPB ?
	bpb = (fat32_bpb*)buffer;

	// MBR criteria:
	// 1. signature        : 55 aa
	// 2. partition status : 80=active, 00=inactive, 00~7f=invalid
	// 3. partition type   : non-zero
	if (buffer[0x1fe] == 0x55 && buffer[0x1ff] == 0xaa && 
		(buffer[0x1be] & ~0x80) == 0 && buffer[0x1c2] != 0) {
		/* get start sector */
		info->startSector = buffer[457] << 24 |
			buffer[456] << 16 |
			buffer[455] << 8  |
			buffer[454];

		prn_string("part@"); prn_dword(info->startSector);

		/* get BPB */
		info->read_sector(info->startSector, 1, (u32 *)buffer);
	} else {
		prn_string("no mbr\n");
	}

	dbg_info();

	/*
	 * check FAT32
	 */
	if (!((((bpb->systemType[1] << 16) | bpb->systemType[0]) == FAT32_L) &&
				(((bpb->systemType[3] << 16) | bpb->systemType[2]) == FAT32_U))) {
		prn_string("Not fat32\n");
		return FAIL;
		/* Not FAT32...*/
	}

	/*
	 * Get infomation of BPB
	 */
	info->bytePerSect = (bpb->bytePerSect[1] << 8) + bpb->bytePerSect[0];
	info->sectPerClus = bpb->sectPerClus;
	info->numOfReserveSect = bpb->sectResearve;
	info->numFatCopy = bpb->numFat;
	info->sectPerFat =bpb->sectPerFat;
	info->rootClus = bpb->rootClusNum;

	info->fat1Sect = info->startSector + info->numOfReserveSect;
	info->clust0Sect = info->fat1Sect + (info->sectPerFat * info->numFatCopy);

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
	ret = search_files(info, (u8*)buffer, type);
	if ( ret == FAIL) {
		dbg_info();
		return FAIL;
	}
	return ret;
}

static u32 search_files(fat_info *info, u8 *buffer, u8 type)
{
	u32 i;
	u32 count;
	u32 sectOffset;
	u32 nextClus;
	u32 fdbOffset;
	u32 currentSect;
	fdb_info *fdb;
	u32 filecount = (type == SDCARD_ISP)?FAT_FILES:1;
	
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
					if (memcmp((u8 *)fdb->name, (u8 *)g_bootinfo.fat_fileName[i], FAT_FILENAMELEN) == 0) {
						info->fileInfo[i].size = fdb->fileSize;
						info->fileInfo[i].cluster = (fdb->clusterH << 16) + fdb->clusterL;
						info->fileInfo[i].sectPos = info->clust0Sect +
							((info->fileInfo[i].cluster - info->rootClus) * info->sectPerClus);
						count++;
						if (count == filecount) {
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
	dbg_info();
	/* can't find files that we need.... */
	return FAIL;
}

