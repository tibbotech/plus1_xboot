#ifndef _SP_OTP
#define _SP_OTP

#include <regmap.h>
#include <types.h>

/*
 * QAC628 OTP memory contains 8 banks with 4 32-bit words. Bank 0 starts
 * at offset 0 from the base.
 *
 */
#define QAC628_OTP_NUM_BANKS            8
#define QAC628_OTP_WORDS_PER_BANK       4
#define QAC628_OTP_WORD_SIZE            sizeof(u32)
#define QAC628_OTP_SIZE                 (QAC628_OTP_NUM_BANKS * \
					QAC628_OTP_WORDS_PER_BANK * \
					QAC628_OTP_WORD_SIZE)
#define QAC628_OTP_BIT_ADDR_OF_BANK     (8 * QAC628_OTP_WORD_SIZE * \
					QAC628_OTP_WORDS_PER_BANK)

#define OTPRX2_BASE_ADR                 0x9C002800
#define OTPRX_BASE_ADR                  0x9C00AF80
#define HB_GPIO                         0x9C00AF00

/* OTP register map */
#define OTP_PROGRAM_CONTROL             0x0C
#define PIO_MODE                        0x07

#define OTP_PROGRAM_ADDRESS             0x10
#define PROG_OTP_ADDR                   0x1FFF

#define OTP_PROGRAM_PGENB               0x20
#define PIO_PGENB                       0x01

#define OTP_PROGRAM_ENABLE              0x24
#define PIO_WR                          0x01

#define OTP_PROGRAM_VDD2P5              0x28
#define PROGRAM_OTP_DATA                0xFF00
#define PROGRAM_OTP_DATA_SHIFT          8
#define REG25_PD_MODE_SEL               0x10
#define REG25_POWER_SOURCE_SEL          0x02
#define OTP2REG_PD_N_P                  0x01

#define OTP_PROGRAM_STATE               0x2C
#define OTPRSV_CMD3                     0xE0
#define OTPRSV_CMD3_SHIFT               5
#define TSMC_OTP_STATE                  0x1F

#define OTP_CONTROL                     0x44
#define PROG_OTP_PERIOD                 0xFFE0
#define PROG_OTP_PERIOD_SHIFT           5
#define OTP_EN_R                        0x01

#define OTP_CONTROL2                    0x48
#define OTP_RD_PERIOD                   0xFF00
#define OTP_RD_PERIOD_SHIFT             8
#define OTP_READ                        0x04

#define OTP_STATUS                      0x4C
#define OTP_READ_DONE                   0x10

#define OTP_READ_ADDRESS                0x50
#define RD_OTP_ADDRESS                  0x1F


#define OTP_READ_TIMEOUT                200
#define OTP_WRITE_TIMEOUT               200

struct otprx_sunplus {
	u32 sw_trim;
	u32 set_key;
	u32 otp_rsv;
	u32 otp_prog_ctl;
	u32 otp_prog_addr;
	u32 otp_prog_csb;
	u32 otp_prog_strobe;
	u32 otp_prog_load;
	u32 otp_prog_pgenb;
	u32 otp_prog_wr;
	u32 otp_prog_reg25;
	u32 otp_prog_state;
	u32 otp_usb_phy_trim;
	u32 otp_data2;
	u32 otp_pro_ps;
	u32 otp_rsv2;
	u32 key_srst;
	u32 otp_ctrl;
	u32 otp_cmd;
	u32 otp_cmd_status;
	u32 otp_addr;
	u32 otp_data;
};

struct sunplus_otp_priv {
	struct otprx2_sunplus *regs;
};

struct hbgpio_sunplus {
	u32 hb_gpio_rgst_bus32[13];
};

struct sunplus_hbgpio {
	struct hbgpio_sunplus *otp_data;
};

int sunplus_otprx_read(int addr, char *value);
int sunplus_otprx_write(int addr, char value);

#endif  //_SP_OTP
