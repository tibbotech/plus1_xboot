#ifndef _SP_OTP
#define _SP_OTP

#include <regmap.h>
#include <types.h>

/*
 * OTP memory
 * Each bank contains 4 words (32 bits).
 * Bank 0 starts at offset 0 from the base.
 *
 */
#define OTP_WORDS_PER_BANK		4
#define OTP_WORD_SIZE			sizeof(u32)
#define OTP_BIT_ADDR_OF_BANK		(8 * OTP_WORD_SIZE * \
						OTP_WORDS_PER_BANK)

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

int otprx_read(volatile struct hb_gp_regs *otp_data, volatile struct otprx_regs *regs, int addr, char *value);
int otprx_key_read(volatile struct otp_key_regs *otp_data, volatile struct otprx_regs *regs, int addr, char *value);
int otprx_write(volatile struct hb_gp_regs *otp_data, volatile struct otprx_regs *regs, int addr, char value);

#endif  //_SP_OTP
