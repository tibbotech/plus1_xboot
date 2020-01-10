#include <types.h>
#include <config.h>
#include <common.h>

//#define OTP_WR_DEBUG_OPEN

#ifdef OTP_WR_DEBUG_OPEN
#define OTP_WR_DEBUG diag_printf
#else
#define OTP_WR_DEBUG(s...) ((void)0)
#endif

#define PACKAGE_SIZE_MAX 150
#define HEADER_SIZE 6

enum {
	EVENT_SEND = 0x01,
	EVENT_ACK = 0x02,
};

enum {
	COMMAND_ID_CONNECT = 0x01,
	COMMAND_ID_READ = 0x02,
	COMMAND_ID_WRITE = 0x03,
};

enum {
	OTP_PARSER_ERROR = 0,
	OTP_PARSER_OK = 1,
	OTP_DISCONNECT = 2,
	OTP_CONNECT = 3,
};


#define PACKAGE_STARTFLAG_BTTE1 0xFF
#define PACKAGE_STARTFLAG_BTTE2 0xFE

#define UART_LSR_RX             (1 << 1)
#define UART_RX_READY()         ((DBG_UART_REG->lsr) & UART_LSR_RX)
#define UART_GET_ERROR()        (((DBG_UART_REG->lsr) << 3) & 0xE0)

extern int sunplus_otprx_read(int addr, char *value);
extern int sunplus_otprx_write(int addr, char value);


static inline void reset_STC()
{
	STC_REG->stc_15_0 = 0;
}

static int get_STC()
{
	return STC_REG->stc_15_0;
}

/*static int istimeout(int msec)
{
	if (msec * 90 < get_STC())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}*/

static int interval_time(int STC)
{
	int cur_stc = get_STC();

	if (cur_stc < STC)
	{
		return (0xffff - STC + cur_stc)/90;
	}
	else
	{
		return (cur_stc - STC)/90;
	}
}

static int dbg_uart_getc(u8* uart_data,int timeout)
{
	int STCtime = get_STC();

	while (!(UART_RX_READY()))
	{
		if (interval_time(STCtime)>timeout)
		{
			return FALSE;
		}
	};

	*uart_data = DBG_UART_REG->dr;

	if (UART_GET_ERROR()) {
		*uart_data = 'E';
		return FALSE;
	}

	return TRUE;
}

extern void dbg_uart_putc(unsigned char c);

static void uart_dump_buffer(unsigned char *buf, int len)
{
	int i;

	if (g_bootinfo.mp_flag) {
		return;
	}

	for (i = 0; i < len; i++) {
		dbg_uart_putc(buf[i]);
	}
}

static u8 checksum(char *data,int len)
{
	u8 checksum = data[0];

	for (int i = 1; i< len; i++)
	{
		checksum = checksum^data[i];
	}

	return checksum;
}


static int receive_package(char *data,int bufsize,int timeout)
{
	int payloadlen = 0;
	int packagelen = 0;
	int STCtime = get_STC();
	u8 byte;
	int readnum = 0;
	memset(data,0,bufsize);
	readnum = 0;

	OTP_WR_DEBUG("[otp_rw.c]%s in\n",__FUNCTION__);
	do
	{
		int rlt = dbg_uart_getc(&byte,50);
		if (rlt)
		{
			if ((byte == PACKAGE_STARTFLAG_BTTE1)&&(readnum == 0))
			{
				data[readnum] = byte;
				readnum ++;
			}
			else if ((byte == PACKAGE_STARTFLAG_BTTE2)&&(readnum == 1))
			{
				data[readnum] = byte;
				readnum ++;
			}
			else if ((readnum > 1)&&(packagelen == 0))
			{
				data[readnum] = byte;
				readnum ++;

				if (readnum == 6)
				{
					payloadlen = (data[4]&0xff)|((data[5]&0xff)<<8);
					packagelen = payloadlen + 7;
				}
			}
			else if ((readnum > 1)&&(packagelen))
			{
				data[readnum] = byte;
				readnum ++;

				if (readnum == packagelen)
				{
					OTP_WR_DEBUG("[otp_rw.c]%s  packagelen=%d\n",__FUNCTION__,packagelen);
					break;
				}
			}
			else
			{
				readnum = 0;
				payloadlen = 0;
				packagelen = 0;
			}

			if (readnum>=bufsize)
			{
				readnum = 0;
				payloadlen = 0;
				packagelen = 0;
			}

		}
		if ((timeout!=0)&&(interval_time(STCtime)>timeout))
		{
			break;
		}
	}
	while (1);

	OTP_WR_DEBUG("[otp_rw.c]%s out\n",__FUNCTION__);
	return packagelen;
}

static void write_package(u8 event,u8 command_id,char *data,int len)
{
	char package[PACKAGE_SIZE_MAX];
	package[0] = PACKAGE_STARTFLAG_BTTE1;
	package[1] = PACKAGE_STARTFLAG_BTTE2;
	package[2] = event;
	package[3] = command_id;
	package[4] = len&0xff;
	package[5] = (len>>8)&0xff;
	memcpy(&(package[6]),data,len);
	package[6+len] = checksum(package,len+HEADER_SIZE);
	//prn_dump_buffer((unsigned char *)package, len+HEADER_SIZE+1);
	uart_dump_buffer((unsigned char *)package, len+HEADER_SIZE+1);
}

static int write_otp_data(u16 startbyte,u16 endbyte,char *data)
{
	int rlt = TRUE;
	for (u16 address = startbyte;address<=endbyte;address++)
	{
		int writerlt = sunplus_otprx_write(address,data[address-startbyte]);
		if (writerlt == -1)
		{
			rlt = FALSE;
		}
		diag_printf("[otp_rw.c]%s       address=0x%x,data=0x%x,writerlt=%d\n",__FUNCTION__,address,data[address-startbyte],writerlt);
	}

	return rlt;
}

static int read_otp_data(u16 startbyte,u16 endbyte,char *data)
{
	int rlt = TRUE;
	for(u16 address = startbyte;address<=endbyte;address++)
	{
		int readrlt = sunplus_otprx_read(address,&data[address-startbyte]);
		if (readrlt == -1)
		{
			rlt = FALSE;
		}
		diag_printf("[otp_rw.c]%s address=0x%x,data=0x%x,readrlt=%d\n",__FUNCTION__,address,data[address-startbyte],readrlt);
	}

	return rlt;
}

static int parser_package(char *data,int packagelen)
{
	//u16 startflag = (data[1]&0xff)|((data[0]&0xff)<<8);
	u8 event = data[2];
	u8 command_id = data[3];
	u16 payloadlen = (data[4]&0xff)|((data[5]&0xff)<<8);
	u8 check = checksum(data,payloadlen+HEADER_SIZE);

	if (check != data[packagelen-1])
	{
		return OTP_PARSER_ERROR;
	}

	OTP_WR_DEBUG("[otp_rw.c]%s  event=0x%x,command_id=0x%x\n",__FUNCTION__,event,command_id);
	if ((event == EVENT_SEND) && (command_id == COMMAND_ID_WRITE))
	{
		u16 startbyte = (data[6]&0xff)|((data[7]&0xff)<<8);
		u16 endbyte = (data[8]&0xff)|((data[9]&0xff)<<8);
		OTP_WR_DEBUG("[otp_rw.c]%s      startbyte=0x%x,endbyte=0x%x\n",__FUNCTION__,startbyte,endbyte);
		int rlt = write_otp_data(startbyte,endbyte,&data[10]);
		if (rlt)
		{
			data[0] = 0;
		}
		else
		{
			data[0] = 1;
		}
		write_package(EVENT_ACK,command_id,&data[0],1);
	}
	else if ((event == EVENT_SEND) && (command_id == COMMAND_ID_READ))
	{
		u16 startbyte = (data[6]&0xff)|((data[7]&0xff)<<8);
		u16 endbyte = (data[8]&0xff)|((data[9]&0xff)<<8);
		data[0] = data[6];
		data[1] = data[7];
		data[2] = data[8];
		data[3] = data[9];
		read_otp_data(startbyte,endbyte,&data[4]);
		write_package(EVENT_ACK,command_id,data,endbyte-startbyte+1+4);
	}
	else if ((event == EVENT_SEND) && (command_id == COMMAND_ID_CONNECT))
	{
		OTP_WR_DEBUG("[otp_rw.c]%s      payloadlen=0x%x,data[6]=0x%x\n",__FUNCTION__,payloadlen,data[6]);
		if ((payloadlen == 1)&&(data[6]==0x01))
		{
			write_package(EVENT_ACK,command_id,&data[6],1);
			return OTP_DISCONNECT;
		}
	}
	else if ((event == EVENT_ACK) && (command_id == COMMAND_ID_CONNECT))
	{
		OTP_WR_DEBUG("[otp_rw.c]%s      payloadlen=0x%x,data[6]=0x%x\n",__FUNCTION__,payloadlen,data[6]);
		if ((payloadlen == 1)&&(data[6]==0x00))
		{
			return OTP_CONNECT;
		}
	}
	return OTP_PARSER_OK;
}


static int receive_handshake_cmd(void)
{
	char data[PACKAGE_SIZE_MAX];

	reset_STC();
	//while (1)//istimeout(500))
	{
		/*u8 byte;
		int rlt = dbg_uart_getc(&byte,50);
		if (rlt == TRUE)
		{
			if (byte == 'o')
			{
				OTP_WR_DEBUG("[otp_rw.c]connect success!\n");
				return TRUE;
			}
		}*/
		int packagesize = receive_package(data,PACKAGE_SIZE_MAX,200);

		OTP_WR_DEBUG("[otp_rw.c]receive_handshake_cmd size = %d\n",packagesize);
		if (packagesize>=(HEADER_SIZE+2))
		{
			int rlt = parser_package(data,packagesize);
			if (rlt == OTP_CONNECT)
			{
				OTP_WR_DEBUG("[otp_rw.c]connect!\n");
				return TRUE;
			}
		}
	}

	OTP_WR_DEBUG("[otp_rw.c]connect fail!\n");
	return FALSE;
}


static int otp_handshake(void)
{
	char flag = 0;
	write_package(EVENT_SEND,COMMAND_ID_CONNECT,&flag,1);

	int rlt = receive_handshake_cmd();
	if (rlt)
	{
		char data = 0;;
		write_package(EVENT_ACK,COMMAND_ID_CONNECT,&data,1);
	}
	return rlt;
}

static void receive_rw_package(void)
{
	while (1) {
		char data[PACKAGE_SIZE_MAX];
		int packagesize = receive_package(data,PACKAGE_SIZE_MAX,0);

		OTP_WR_DEBUG("[otp_rw.c]receive_package size = %d\n",packagesize);
		int rlt = parser_package(data,packagesize);
		if (rlt == OTP_DISCONNECT)
		{
			OTP_WR_DEBUG("[otp_rw.c]disconnect!\n");
			break;
		}
	}
}

void mon_rw_otp(void)
{
	diag_printf("[otp_rw.c]otp_rw in\n");

	if (otp_handshake() == TRUE)
	{
		receive_rw_package();
	}

	diag_printf("[otp_rw.c]otp_rw out\n");
}
