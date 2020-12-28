#ifndef _FAT_INC_
#define _FAT_INC_

#include <types.h>
//#include <nand_boot/nandop.h>

#define FAT_UNKNOW		0
#define FAT_32			1
#define FAT_16			2

//#define MAX_SECTOR_SIZE     1024
#define FAT32_U 		0x20202032
#define FAT32_L 		0x33544146
#define FAT16_U 		0x20202036
#define FAT16_L 		0x31544146
#define FAT_FDB_SIZE 		32
#define FAT_DATA_1ST_CLUS_NUM	2
#define FAT_FILES 		2
#define FAT32_MAXSECTSIZE 	(16 * 1024)
#define FAT16_MAXSECTSIZE 	(32 * 1024)
#define FAT_FILENAMELEN 	11
//#define SECTORSIZE 		512

#define FAT_ISPBOOOT_INDEX		(0)
#define FAT_UBOOT_INDEX			(1)

typedef struct {
	u32 size;
	u32 sectPos;
	u32 cluster;
} file_info;

typedef struct {
	u32 startSector;
	u32 sectPerFat;
	u32 rootClus;
	u32 fat1Sect;
	u32 clust0Sect;
	u32 rootSectStart;
	u32 rootSect;
	u16 bytePerSect;
	u16 numOfReserveSect;
	u16 rootEntCnt;
	u8 fatType;
	u8 sectPerClus;
	u8 numFatCopy;
	u8 bytePerSectInPower;
	u8 reserved;
	file_info fileInfo[FAT_FILES];
	int (*read_sector)(u32, u32, u32*);
	int (*init_sd)(int type);
	int (*init_usb)(int port, int next_port_in_hub);
	void (*reset)();
} fat_info;

typedef struct {
	u8 info1[11];
	u8 bytePerSect[2];
	u8 sectPerClus;
	u16 sectResearve;
	u8 numFat;
	u8 info2[19];
	u32 sectPerFat;
	u8 info3[4];
	u32 rootClusNum;
	u8 info4[34];
	u16 systemType[4];
} fat32_bpb;

typedef struct {
	u8 info1[11];
	u8 bytePerSect[2];
	u8 sectPerClus;
	u16 sectResearve;
	u8 numFat;
	u8 rootEntCnt[2];
	u8 info2[3];
	u8 fatSz16[2];
	u8 info3[30];
	u16 systemType[4];
} fat16_bpb;

typedef struct {
	u8 name[11];	/* short file name only */
	u8 reserved[9];
	u16 clusterH;
	u16 createTime;
	u16 createDate;
	u16 clusterL;
	u32 fileSize;
} fdb_info;

typedef struct {
	u32 fat[FAT32_MAXSECTSIZE];
} fat32table_info;

typedef struct {
	u16 fat[FAT16_MAXSECTSIZE];
} fat16table_info;

//u32 search_files(u32 files, fat_info *info, u8 *buffer, u8 fat_type);
u32 fat_boot(u32 type, u32 port, fat_info *info, u8 *buffer);
u32 fat_read_file(u32 idx, fat_info *info, u8 *buffer, u32 offset, u32 length, u8 *dest);
u8 fat_sdcard_check_boot_mode(fat_info *info);

//int str_cmp(const char *s1, const char *s2, u32 n);

#endif /* _FAT_INC_ */
