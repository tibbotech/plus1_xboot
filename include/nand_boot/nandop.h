#ifndef __NANDOP_H__
#define __NANDOP_H__

#define DBGPRINT(args...) //printf(args)
#define DBGOUT(args...) //printf(args)
#define printk(args...) //printf(args)

#ifndef SUPPORT_RD_MEMFUNC
#define rd_memcpy(a, b, c) memcpy32(a, b, (c >> 2))
#define rd_memcmp memcmp
#define rd_memset memset
#else
void rd_memcpy(UINT32*, UINT32*, UINT32);
void rd_memset(UINT8* pDst, UINT8 ch, UINT32 size);
#endif

inline static void Xil_Out32(unsigned int OutAddress, unsigned int Value)
{
        *(volatile unsigned int *) OutAddress = Value;
}

inline static unsigned int Xil_In32(unsigned int Addr)
{
        return *(volatile unsigned int *) Addr;
}

#include <types.h>
#include <nand_boot/nfdriver.h>

enum boot_code_parameter {
	VIRTUAL_PAGE_SIZE       = 2048,
	VIRTUAL_PAGE_SHIFT      = 11,
	VIRTUAL_PAGE_PER_BLOCK  = 32,
	VIRTUAL_PAGE_PER_BLOCK_SHIFT = 5
};

enum SDev_flg {
	SMALLBLKFLG = 0,
	SUPPORTBCHFLG
};

typedef UINT8 (*predInitDriver_t)(void);
typedef void (*predEraseBlk_t)(UINT8, UINT32);
typedef SINT32 (*predReadWritePage_t)(UINT8, UINT32 ,UINT32* ,UINT32* ,UINT8 );
typedef void (*predGetInfo_t)(void);

typedef struct SDev_s
{
	UINT8 IsSupportBCH;
	UINT8 DeviceID;
	UINT8 reserved;

	predInitDriver_t predInitDriver;
	predEraseBlk_t predEraseBlk ;
	predReadWritePage_t predReadWritePage ;
	predGetInfo_t predGetInfo;
}SDev_t;

typedef struct nand_advance_command_bits_s
{
	UINT32	have_interleave : 1; // Interleaved Program between Multiple dice
	UINT32	have_cache_program : 1;
	UINT32	have_cache_read : 1;
	UINT32	have_two_plane_program : 1;
	UINT32	have_two_plane_read : 1;
	UINT32	have_two_plane_erase : 1;
} nand_advance_command_bits_t;

typedef union nand_advance_command_S
{
	UINT32 _u32;
	nand_advance_command_bits_t  _bits;
} nand_advance_command_t;

/* 16-byte optional boot entry */
struct OptBootEntry16
{
	UINT32    opt0_copies;
	UINT32    opt0_pg_off;
	UINT32    opt0_pg_cnt;
	UINT32    reserved;
};

/*
 * Romcode needs to know NAND's page size & addressing cycle to
 * be able to read pages with offset.
 * Romcode will search for this header since Block 0.
 */
#define BHDR_BEG_SIGNATURE    0x52444842  // BHDR
#define BHDR_END_SIGNATURE    0x444e4548  // HEND
struct BootProfileHeader
{
	// 0
	UINT32    Signature;     // BHDR_BEG_SIGNATURE
	UINT32    Length;        // 256
	UINT32    Version;       // 0
	UINT32    reserved12;

	// 16
	UINT8     BchType;       // BCH method: 0=1K60, 0xff=BCH OFF
	UINT8     addrCycle;     // NAND addressing cycle
	UINT16    ReduntSize;    // depricated, keep it 0
	UINT32    BlockNum;      // Total NAND block number
	UINT32    BadBlockNum;   // not used now
	UINT32    PagePerBlock;  // NAND Pages per Block

	// 32
	UINT32    PageSize;       // NAND Page size
	UINT32    ACWriteTiming;  // not used now
	UINT32    ACReadTiming;   // not used now
	UINT32    PlaneSelectMode;// special odd blocks read mode (bit 0: special sw flow en. bit 1 read mode en. bit 2~bit 5 plane select bit addr) 

	// 48
	UINT32    xboot_copies;   // Number of Xboot copies. Copies are consecutive.
	UINT32    xboot_pg_off;   // page offset (usu. page offset to block 1) to Xboot
	UINT32    xboot_pg_cnt;   // page count of one copy of Xboot
	UINT32    reserved60;

	// 64
	struct OptBootEntry16 opt_entry[11]; // optional boot entries at 64, 80, ..., 224

	// 240
	UINT32    reserved240;
	UINT32    reserved244;
	UINT32    EndSignature;   // BHDR_END_SIGNATURE
	UINT32    CheckSum;       // TCP checksum (little endian)

	// 256
};

#define MAX_ID_LEN 32
struct SysInfo
{
	UINT8   IdBuf[MAX_ID_LEN];

	UINT16  u16PageNoPerBlk;
	UINT16  u16PageSize;

	UINT16  u16PyldLen;
	UINT16  u16ReduntLen;

	UINT16  u16Redunt_Sector_Addr_Offset;
	UINT16  u16Redunt_Sector_Len;

	UINT32  u16TotalBlkNo;

	UINT8   u8TotalBlkNoShift;
	UINT8   ecc_mode;
	UINT8   u8MultiChannel;//u8NFChannel;
	UINT8   u8Support_Internal_Interleave;

	UINT8   u8Support_External_Interleave;
	UINT8   u8Internal_Chip_Number;
	UINT16  u16InterruptMask;

	UINT8   u8PagePerBlkShift;
	UINT8   u8Support_TwoPlan;
	UINT8   vendor_no;
	UINT8   u8addrCycle;

	UINT32  gNANDACReadTiming;
	UINT32  gNANDACWriteTiming;
};


SINT32 InitNand(void);
SINT32 ReadNANDSector_1K60(UINT8 which_cs, UINT32* ptrPyldData, UINT32 pageNo);
SINT32 WriteNANDSector_1K60(UINT8 which_cs, UINT32* ptrPyldData, UINT32 pageNo);
SINT32 ReadNANDPage_1K60(UINT8 which_cs, UINT32 pageNo, UINT32* ptrPyldData, UINT32 *read_bytes);
SINT32 WriteNANDPage_1K60(UINT8 which_cs, UINT32 pageNo, UINT32* ptrPyldData, UINT32 *write_bytes);
SINT32 ReadNANDPage(UINT8 which_cs, UINT32 pageNo, UINT32 pageCount, UINT32* ptrPyldData, UINT32 *read_bytes);
SINT32 WriteNANDPage(UINT8 which_cs, UINT32 pageNo, UINT32 pageCount, UINT32* ptrPyldData, UINT32 *write_bytes);

void MainInitBufferAddr_nand(void);
UINT32 Load_Header_Profile(SINT32 type);
SDev_t* getSDev(void);
UINT8 getSDevinfo(UINT8 flg);

void DelayUS(UINT32 delay);
SDev_t* getSDev(void);

struct BootProfileHeader *Get_Header_Profile_Ptr(void);
UINT8* Get_PyldData_Ptr(void);
UINT8* Get_SpareData_Ptr(void);

SINT32 BCHProcess(UINT32* PyldBuffer, UINT32* ReduntBuffer, UINT32 len, SINT32 op, SINT32 ecc_mode);

void SetNANDRandomizeFlag(SINT32 val);

void SetNANDACReadTiming(UINT32 val);
void SetNANDACWriteTiming(UINT32 val);

SINT32 initDriver_nand(void);
SINT32 ReadWritePage(UINT8 which_cs, UINT32 u32PhyAddr,UINT32* PyldBuffer,UINT32* DataBuffer,UINT32 u8RWMode);

int InitDevice(int type);
void initNandFunptr(void);
SINT32 ReadBootBlock(UINT32 *target_address);

SINT32 SPINANDReadBootBlock(u32 *xbuffer);
void initSPINandFunptr(void);
SINT32 ReadSPINANDSector_1K60(UINT32* ptrPyldData, UINT32 pageNo);

UINT8 GetNANDPageCount_1K60(UINT32 pageSize);
SINT32 SPINANDReadNANDPage_1K60(UINT8 which_cs, UINT32 pageNo, UINT32 * ptrPyldData, UINT32 *read_bytes);

#endif
