#include <types.h>
#include <config.h>
#include <common.h>
#include <cpu/cpu.h>

#ifdef XBOOT_BUILD
#define CMDLINE "XB> "
#else
#ifdef PLATFORM_SPIBAREMETAL
#define CMDLINE "NOR> "
#else
#define CMDLINE "ROM> "
#endif
#endif

#define UART_LSR_RX             (1 << 1)
#define UART_RX_READY()        ((DBG_UART_REG->lsr) & UART_LSR_RX)
#define UART_GET_ERROR()       (((DBG_UART_REG->lsr) << 3) & 0xE0)

#define IS_NEWLINE(_byte)       (((_byte) == 0x0D) || ((_byte) == 0x0A))

extern void dbg_uart_putc(unsigned char);

static u8 dbg_uart_getc(void)
{
        u8 uart_data;

        while (!(UART_RX_READY()));

        uart_data = DBG_UART_REG->dr;

        if (UART_GET_ERROR()) {
                uart_data = 'E';
        }

        return uart_data;
}

static u8 uart_getc_show_char(void)
{
	u8 byte;

	byte = dbg_uart_getc();

	if (IS_NEWLINE(byte)) {
		dbg_uart_putc('\n');
		dbg_uart_putc('\r');
	} else {
		dbg_uart_putc(byte);
	}

	return byte;
}

static void show_result_reg(u32 group, u32 reg)
{
	prn_string("G"); prn_decimal(group); prn_string("."); prn_decimal(reg);
	prn_string("=");
	prn_dword(*(volatile u32 *)(RF_GRP(group, reg)));
}

static u8 get_word_ex(const char *str, int is_hex, u32 *addr)
{
	u8 count = 0;
	u8 byte;
	u32 result = 0;
	u8 flag = 0;

	if (str) {
		prn_string(str);
	}

	while (1) {
		byte = uart_getc_show_char();
		if (IS_NEWLINE(byte) || (byte == ' ')) {
			if ((count == 0) && (flag == 0)) {
				continue;
			}
			*addr = result;
			return 1;
		}

		if (('0' <= byte) && (byte <= '9')) {
			if ((count == 0) && (byte == '0')) {
				flag = 1;
				continue;
			}
			byte -= '0';
		} else if (is_hex && ('A' <= byte) && (byte <= 'F')) {
			byte -= '7';
		} else if (is_hex && ('a' <= byte) && (byte <= 'f')) {
			byte -= 'W';
		} else if (byte == 0x08) { /* backspace */
			count--;
			result = is_hex ? (result >> 4) : (result / 10);
			continue;
		} else {
			prn_string("\n?\n");
			return 0;
		}

		result = is_hex ? ((result << 4) + byte) : ((result * 10) + byte);
		count++;
		if (count > 8) {
			prn_string("\n?\n");
			return 0;
		}
	}
	return 0; /* never */
}

static void mon_cmdline(void)
{
	prn_string("\n" CMDLINE "d: dump, r: read, w: write, l: lreg, W: wreg, m: menu, K: reset, q: quit\n");
	prn_string(CMDLINE);
}

void mon_shell(void)
{
	u8 byte;
	u32 address;
	u32 value;
	u32 group;
	u32 reg;
	while (1) {
		mon_cmdline();
		byte = uart_getc_show_char();

		switch (byte) {
		case 'r':
			if (get_word_ex(" addr=0x", 1, &address)) {
				prn_dword(*(volatile u32 *)address);
			}
			break;
		case 'w':
			if (get_word_ex(" addr=0x", 1, &address)) {
				if (get_word_ex("val=0x", 1, &value)) {
					*(volatile u32 *)address = (u32)value;
					prn_dword(*(volatile u32 *)address);
				}
			}
			break;
#ifdef CONFIG_ARCH_ARM
		case 'S':
			/* armv7 sev */
#ifdef __thumb__
			asm volatile (".short 0xbf40");
#else
			asm volatile (".word 0xe320f004");
#endif
			break;
#endif
		case 'g':
		case 'l':
			if (get_word_ex(" G=", 0, &group)) {
				int i;
				for (i = 0; i < 32; i++) {
					show_result_reg(group, i);
				}
			}
			break;
		case 'W':
			if (get_word_ex(" G=", 0, &group)) {
				prn_string("G"); prn_decimal(group);
				if (get_word_ex(".", 0, &reg)) {
					if (get_word_ex("val=0x", 1, &value)) {
						*(volatile u32 *)(RF_GRP(group, reg)) = value;
						show_result_reg(group, reg);
					}
				}
			}
			break;
		case 'd':
			if (get_word_ex(" addr=0x", 1, &address)) {
				if (get_word_ex("size=0x", 1, &value)) {
					prn_dump_buffer((u8 *)address, value);
				}
			}
			break;
#ifdef PLATFORM_SPIBAREMETAL
		case 'J':
			if (get_word_ex(" addr=0x", 1, &address)) {
				((void (*)(void))address)(); /* ARM: bx <addr> */
			}
			break;
#endif
		case 'K':
			boot_reset();
#ifndef XBOOT_BUILD
		case 'm': {
			const int bootmenu[] = {
				USB_ISP, SDCARD_ISP, SPI_NOR_BOOT, SPINAND_BOOT,
				PARA_NAND_BOOT, EMMC_BOOT, UART_ISP, AUTO_SCAN };
			prn_string("\n1:usb, 2:sdcard, 3:nor, 4:spinand, 5:paranand, 6:emmc, 7:uart, 8:auto\n");
			if (get_word_ex("choice=", 0, &value)) {
				if (--value < sizeof(bootmenu) / sizeof(bootmenu[0])) {
					cpu_invalidate_icache_all();
					do_boot_flow(bootmenu[value]);
				}
			}
			break;
		}
#endif
#if 0 /* single byte read/write */
		case 'b':
			  if (get_word_ex("read byte addr=0x", 1, &address)) {
				  prn_dword(*(volatile u8 *)address);
			  }
			  break;
		case 'B':
			  if (get_word_ex("write byte addr=0x", 1, &address)) {
				  if (get_word_ex("byte val=0x", 1, &value)) {
					  *(volatile u8 *)address = (u32)value;
					  prn_dword(*(volatile u8 *)address);
				  }
			  }
			  break;
#endif
		case 'q':
			return;
		default:
			prn_string("\n?");
			break;
		}
	}
}
