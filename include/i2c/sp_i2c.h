#ifndef DRV_I2C_H
#define DRV_I2C_H

#include <config.h>
#include <regmap.h>
#include <types.h>

/****************************************
* I2C Master
****************************************/
#if defined(PLATFORM_SP7350)
#define I2C_NO	10
#else
#define I2C_NO	10
#endif

#define I2C_TX_FIFO_DEPTH	32
#define I2C_READ_TX_DATA	0x100

/* I2C Frequency Modes */
#define I2C_MAX_STANDARD_MODE_FREQ	100000
#define I2C_MAX_FAST_MODE_FREQ		400000
#define I2C_MAX_FAST_MODE_PLUS_FREQ	1000000


#define SP_IC_CON_MASTER		0x1
#define SP_IC_CON_SPEED_STD		0x2
#define SP_IC_CON_SPEED_FAST		0x4
#define SP_IC_CON_SPEED_HIGH		0x6
#define SP_IC_CON_SPEED_MASK		0x6
#define SP_IC_CON_10BITADDR_SLAVE		0x8
#define SP_IC_CON_10BITADDR_MASTER	0x10
#define SP_IC_CON_RESTART_EN		0x20
#define SP_IC_CON_SLAVE_DISABLE		0x40
#define SP_IC_CON_STOP_DET_IFADDRESSED		0x80
#define SP_IC_CON_TX_EMPTY_CTRL		0x100
#define SP_IC_CON_RX_FIFO_FULL_HLD_CTRL		0x200

#define SP_IC_INTR_RX_UNDER	0x01
#define SP_IC_INTR_RX_OVER	0x02
#define SP_IC_INTR_RX_FULL	0x04
#define SP_IC_INTR_TX_OVER	0x08
#define SP_IC_INTR_TX_EMPTY	0x10
#define SP_IC_INTR_RD_REQ	0x20
#define SP_IC_INTR_TX_ABRT	0x40
#define SP_IC_INTR_RX_DONE	0x80
#define SP_IC_INTR_ACTIVITY	0x100
#define SP_IC_INTR_STOP_DET	0x200
#define SP_IC_INTR_START_DET	0x400
#define SP_IC_INTR_GEN_CALL	0x800
#define SP_IC_INTR_RESTART_DET	0x1000

#define SP_IC_STATUS_TFE	0x004
#define SP_IC_STATUS_RFNE	0x008
#define SP_IC_STATUS_MST_ACT	0x020

#define SP_IC_INTR_DEFAULT_MASK		(SP_IC_INTR_RX_FULL | \
					 SP_IC_INTR_TX_ABRT | \
					 SP_IC_INTR_STOP_DET)
#define SP_IC_INTR_MASTER_MASK		(SP_IC_INTR_DEFAULT_MASK | \
					 SP_IC_INTR_TX_EMPTY)
#define SP_IC_INTR_SLAVE_MASK		(SP_IC_INTR_DEFAULT_MASK | \
					 SP_IC_INTR_RX_DONE | \
					 SP_IC_INTR_RX_UNDER | \
					 SP_IC_INTR_RD_REQ)

#define ABRT_7B_ADDR_NOACK	0
#define ABRT_10ADDR1_NOACK	1
#define ABRT_10ADDR2_NOACK	2
#define ABRT_TXDATA_NOACK	3
#define ABRT_GCALL_NOACK	4
#define ABRT_GCALL_READ		5

#define SP_IC_TX_ABRT_7B_ADDR_NOACK	(1UL << ABRT_7B_ADDR_NOACK)
#define SP_IC_TX_ABRT_10ADDR1_NOACK	(1UL << ABRT_10ADDR1_NOACK)
#define SP_IC_TX_ABRT_10ADDR2_NOACK	(1UL << ABRT_10ADDR2_NOACK)
#define SP_IC_TX_ABRT_TXDATA_NOACK	(1UL << ABRT_TXDATA_NOACK)
#define SP_IC_TX_ABRT_GCALL_NOACK	(1UL << ABRT_GCALL_NOACK)

#define SP_IC_TX_ABRT_NOACK		(SP_IC_TX_ABRT_7B_ADDR_NOACK | \
					 SP_IC_TX_ABRT_10ADDR1_NOACK | \
					 SP_IC_TX_ABRT_10ADDR2_NOACK | \
					 SP_IC_TX_ABRT_TXDATA_NOACK | \
					 SP_IC_TX_ABRT_GCALL_NOACK)

struct i2c_master_ctlr {
	volatile struct dw_i2c_regs *reg;
	unsigned int i2c_ip;

	unsigned int DataIndex;
	unsigned int RegDataIndex;
	unsigned int DataTotalLen;
	unsigned int Abort_Source;
	unsigned int ReadTxlen;
	unsigned int RxOutStanding;
	unsigned int xfet_action;

	u8 *buf;
};
struct i2c_master_ctlr i2c_mas_ctlr[I2C_NO];

void sp_i2c_en(unsigned int i2c_no);
void sp_i2c_write(unsigned int i2c_no, u8  slave_addr , u8  *data_buf , unsigned int len);
void sp_i2c_read(unsigned int i2c_no, u8  slave_addr , u8  *data_buf , unsigned int len);

#endif		//DRV_SD_MMC_H
