/*
 * (C) Copyright 2015
 * Sunplus Technology
 * Mason Yang <mason.yang@sunplus.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __SP_SPINAND_H
#define __SP_SPINAND_H


//#ifndef BIT(x)
#define BIT(x)	(1<<x)
//#endif

// ctrl
#define SPI_DEVICE_IDLE  (1<<31)


// wait
#define SPI_NAND_ENABLE  (1<<11)

//spi_nand_ctrl
#define SPI_NAND_CHIP_A	 	(1<<24)	
#define SPI_NAND_AUTO_WEL	(1<<19)
#define SPI_NAND_CLK_32DIV	(0x7<<16)
#define SPI_NAND_DMA_OWNER	(0x1<<17)

// spi_cust_cmd
#define SPI_CUSTCMD_SHIFT          8
#define SPI_CUSTCMD_FN_SHIFT       7 
#define SPI_CUSTCMD_RW_SHIFT       2 
#define CUSTCMD_BYTECNT_DATA_SHIFT 4
#define CUSTCMD_BYTECNT_ADDR_SHIFT 0

#define SPI_NAND_CTRL_EN       	(1<<SPI_CUSTCMD_FN_SHIFT)
#define SPI_NAND_READ_MDOE      (0<<SPI_CUSTCMD_RW_SHIFT) 
#define SPI_NAND_WRITE_MDOE     (1<<SPI_CUSTCMD_RW_SHIFT)

#define SPINAND_CUSTCMD_NO_DATA	(0<<CUSTCMD_BYTECNT_DATA_SHIFT)
#define SPINAND_CUSTCMD_1_DATA	(1<<CUSTCMD_BYTECNT_DATA_SHIFT)
#define SPINAND_CUSTCMD_2_DATA	(2<<CUSTCMD_BYTECNT_DATA_SHIFT)
#define SPINAND_CUSTCMD_3_DATA	(3<<CUSTCMD_BYTECNT_DATA_SHIFT)
#define SPINAND_CUSTCMD_4_DATA	(4<<CUSTCMD_BYTECNT_DATA_SHIFT)

#define SPINAND_CUSTCMD_NO_ADDR	(0<<CUSTCMD_BYTECNT_ADDR_SHIFT)
#define SPINAND_CUSTCMD_1_ADDR	(1<<CUSTCMD_BYTECNT_ADDR_SHIFT)
#define SPINAND_CUSTCMD_2_ADDR	(2<<CUSTCMD_BYTECNT_ADDR_SHIFT)
#define SPINAND_CUSTCMD_3_ADDR	(3<<CUSTCMD_BYTECNT_ADDR_SHIFT)


// CMD list
#define SPINAND_CMD_RESET		  0xff	
#define SPINAND_CMD_READID		  0x9f	
#define SPINAND_CMD_GETFEATURES   0x0F
#define SPINAND_CMD_SETFEATURES   0x1F
#define SPINAND_CMD_BLKERASE      0xD8
#define SPINAND_CMD_PAGE2CACHE    0x13
#define SPINAND_CMD_PAGEREAD	  0x3
#define SPINAND_CMD_RDCACHEQUADIO 0xEB

#define SPINAND_CMD_PROLOADx4     0x32
#define SPINAND_CMD_PROEXECUTE    0x10

//AUTOCFG
#define SPINAND_AUTOCFG_CMDEN		(1<<21)	
#define SPINAND_AUTOCFG_RDCACHE		(1<<20)
#define SPINAND_AUTOCFG_RDSTATUS	(1<<18)


//CFG01 & CFG02 default value;
#define SPINAND_CFG01_DEFAULT	  0x150085	//CMD 1bit DQ0 output, ADDR 1bit DQ0 output, DATA 1bit DQ1 INTPUT
											//cmd 1bit mode, addr 1bit mode, data 1 bit.

#define SPINAND_CFG01_DEFAULT1	  0x150015	//CMD 1bit DQ0 output, ADDR 1bit DQ0 output, DATA 1bit DQ0 OUTPUT
											//cmd 1bit mode, addr 1bit mode, data 1 bit.

#define SPINAND_CFG01_DEFAULT2	  0x50005   //CMD 1bit DQ0 output, ADDR 1bit DQ0 output,
											//cmd 1bit mode, addr 1bit mode, 
											
#define SPINAND_CFG01_DEFAULT3	  0x10001   //CMD 1bit DQ0 output,
											//cmd 1bit mode, 

#define SPINAND_CFG02_DEFAULT	  0x8150085 //CMD 1bit DQ0 output, ADDR 1bit DQ0 output, DATA 1bit DQ1 INTPUT, 
											//cmd 1bit mode, addr 1bit mode, data 1 bit. 8 dummy cycle


// =============================
#define CSR_EN			BIT(0)
#define CSR_MODE_PIO	BIT(1)
#define CSR_MODE_DESC	0
#define CSR_MODE_PIO1	(BIT(1))
#define CSR_MODE_PIO2	(BIT(1) | BIT(2))
#define CSR_BURST4		0
#define CSR_BURST8		BIT(3)
#define CSR_DESC_FETCH	BIT(4)
#define CSR_NAND_EDO	BIT(5)
#define CSR_DUMMY_RD	BIT(6)
#define CSR_RESET		BIT(8)
#define EDO_TYPE01		BIT(9) // Mason for 8388
#define EDO_TYPE10		BIT(10)
#define EDO_TYPE11		(BIT(9) | BIT(10))

#define ACTR_CLE(t)		(((t) >> 1) & 0xf)
#define ACTR_ALE(t)		(((t) & 0xf) << 4)
#define ACTR_ACT(t)		(((t) & 0xf) << 8)
#define ACTR_REC(t)		(((t) & 0xf) << 12)
#define ACTR_WAIT(t)	(((t) & 0xff) << 16)
#define ACTR_RDST(t)	(((t) & 0xff) << 24)

#define PIOCR0_CS(n)		(1 << ((n) & 3))
#define PIOCR0_CS_MASK		0x0f
#define PIOCR0_CLE			BIT(4)
#define PIOCR0_ALE			BIT(5)
#define PIOCR0_WE			BIT(6)
#define PIOCR0_RE			BIT(7)
#define PIOCR0_OE			BIT(8)
#define PIOCR0_WP			BIT(9)
#define PIOCR1_WR(n)		((n) & 0xff)
#define PIOCR2_RD(n)		((n) & 0xff)
#define PIOCR3_CMD(n)		((n) & 0xff)
#define PIOCR4_ADDR_RAW(n)	(n)
#define PIOCR5_ADDR_COL(n)	((n) & 0xffff)
#define PIOCR5_ADDR_CYC(n)	((((n) - 1) & 7) << 16)
#define PIOCR6_DMA_ADDR(n)	(n)
#define PIOCR7_DMA_SIZE(n)	(((n) - 1) & 0xffff)
#define PIOCR7_DMA_RD		0
#define PIOCR7_DMA_WR		BIT(16)

#define ISR_DESC_DONE		BIT(0)
#define ISR_DESC_END		BIT(1)
#define ISR_DESC_ERR		BIT(2)
#define ISR_DESC_INV		BIT(3)
#define ISR_DESC_INVCMD		BIT(4)
#define ISR_DESC			0x2 //0x1f
#define ISR_BUS_BUSY		BIT(5)
#define ISR_FL_ERROR		BIT(6)  /* NAND flash reports error */
#define ISR_HW_BUSY			BIT(7)  /* NAND controller busy */
#define ISR_CMD				BIT(8)  /* CMD complete (PIO MODE2) */
#define ISR_ADDR			BIT(9)  /* ADDR complete (PIO MODE2) */
#define ISR_WR				BIT(10) /* Write complete (PIO MODE2) */
#define ISR_RD				BIT(11) /* Read complete (PIO MODE2) */
#define ISR_RB0				BIT(12)
#define ISR_RB1				BIT(13)
#define ISR_RB2				BIT(14)
#define ISR_RB3				BIT(15)
#define ISR_DESC_ADDR(x)	(((x) >> 16) & 0xffff)

#define RBTR_WB(t)			((t) & 0xff)
#define RBTR_IRQ_INVDESC	BIT(8)

/*
 * Descriptor Mode
*/
#if 0
struct sp_spinand_desc {
	uint32_t cmd1:8;
	uint32_t cmd0:8;
	uint32_t ctrl:8;
	uint32_t mfs:4;
	uint32_t cmd:4; /* i.e. SP_NAND_CMD_XXX */

	uint32_t oob_ssz:16; /* oob sector size */
	uint32_t buf_ssz:16; /* data sector size */

	uint32_t isr:16;
	uint32_t ier:16;

	uint32_t addr4:8;
	uint32_t addr5:8;
	uint32_t sect_nr:7; /* sector number */
	uint32_t type:1;
	uint32_t addr_len:3;
	uint32_t ff_check:1;
	uint32_t oob_en:1;
	uint32_t cs_ctrl:1;
	uint32_t end:1;
	uint32_t owner:1;

	uint32_t addr0:8;
	uint32_t addr1:8;
	uint32_t addr2:8;
	uint32_t addr3:8;

	uint32_t buf_dma:32;

	uint32_t oob_dma:32;

	uint32_t desc_sz:14;  /* descriptor size */
	uint32_t rand_en:1;   /* enable scrambler/randomizer */
	uint32_t ac_timing_select:1;
	uint32_t rsvd:16;
} __attribute__ ((__packed__));
#endif

#define SP_NAND_CMD_READ	0x1
#define SP_NAND_CMD_WRITE	0x2  /* page write */
#define SP_NAND_CMD_ERASE	0x3  /* block erase */
#define SP_NAND_CMD_RDST	0x4  /* read status */
#define SP_NAND_CMD_SUPER	0xd  /* super manual */
#define SP_NAND_CMD_CACHE_RD 0xe /* cache read */
//extern const struct nand_flash_dev sp_nand_ids[];

/* Q628 spi nand driver */
struct sp_spinand_regs {
	unsigned int spi_ctrl;       // 87.0
	unsigned int spi_timing;     // 87.1
	unsigned int spi_page_addr;  // 87.2
	unsigned int spi_data;       // 87.3
	unsigned int spi_status;     // 87.4
	unsigned int spi_auto_cfg;   // 87.5
	unsigned int spi_cfg[3];     // 87.6
	unsigned int spi_data_64;    // 87.9
	unsigned int spi_buf_addr;   // 87.10
	unsigned int spi_statu_2;    // 87.11
	unsigned int spi_err_status; // 87.12
	unsigned int mem_data_addr;  // 87.13
	unsigned int mem_parity_addr;// 87.14
	unsigned int spi_col_addr;   // 87.15
	unsigned int spi_bch;        // 87.16
	unsigned int spi_intr_msk;   // 87.17
	unsigned int spi_intr_sts;   // 87.18
	unsigned int spi_page_size;  // 87.19
};


struct sp_spinand_info {
	struct sp_spinand_regs *regs;
	/*
	   struct {
	   uint32_t idx;
	   uint32_t size;
	   void *virt;
	   dma_addr_t phys;
	   } buff;
	 */

	//      int cmd; /* current command code */
	//      int cac; /* col address cycles */
	//      int rac; /* row address cycles */
	//      int cs;
	int id;  // Vendor ID for 6B/EB Winbond or GigaDevice
	int row; // row address for pagewrite
};

#endif /* __SP_SPINAND_H */
