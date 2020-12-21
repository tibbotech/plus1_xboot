#ifndef _EHCI_USB
#define _EHCI_USB

#include <regmap.h>

// support multiple usb ports
#define EHCI_LEN_REV            (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_len_rev)
#define EHCI_SPARAMS            (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_sparams)
#define EHCI_CPARAMS            (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_cparams)
#define EHCI_PORTROUTE          (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_portroute)
#define EHCI_USBCMD             (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_usbcmd)
#define EHCI_USBSTS             (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_usbsts)
#define EHCI_USBINTR            (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_usbintr)
#define EHCI_FRAMEIDX           (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_frameidx)
#define EHCI_CTRL_DS_SEGMENT    (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_ctrl_ds_segment)
#define EHCI_PRD_LISTBASE       (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_prd_listbase)
#define EHCI_ASYNC_LISTADDR     (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_async_listaddr)
#define EHCI_CONFIG             (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_config)
#define EHCI_PORTSC             (g_io_buf.usb.ehci.ehci_hcd_regs->ehci_portsc)

////////////////////////////////////////

//EHCI[08]
#define SET_INT_THRESHOLD       (1<<16)     // set INT threshold
#define EN_ASYNC_SCHEDULE       (1<<5)      // async schedule enable
#define EN_PERIODIC_SCHEDULE    (1<<4)      // periodic schedule enable
#define EHCI_SFT_RESET          (1<<1)      // Host Controller Reset
#define EN_SCHEDULE             (1<<0)      // run schedule
//EHCI[09]
#define STS_ASS		        (1<<15)     // status of Async Schedule
#define HCHALTED                (1<<12)     // HC is in Halted status
#define CLEAR_PCD               (1<<2)      // clear port change detect status
#define STS_ERR		        (1<<1)      // "error" completion (overflow, ...)
#define STS_PASS		(1<<0)      // "normal" completion (short, ...)
#define STS_INT_MASK            (0x03)
//EHCI[10]
#define IAAE                    (1<<5)
#define HSEE                    (1<<4)
#define FLRE                    (1<<3)
#define PCIE                    (1<<2)
#define UEIE                    (1<<1)
#define EN_INT                  (UEIE|PCIE|FLRE|HSEE|IAAE)
//EHCI[24]
#define SET_EHCI                (1<<0)      // set port to EHCI
//EHCI[25]
#define PORT_CONNECT	        (1<<0)      // status of connect
#define PORT_ENABLE             (1<<2)      // status of port
#define PORT_RESET              (1<<8)      // bus reset start
//EHCI[28]
#define EN_EHCI_MSI             (1<<4)      // enable MSI
//EHCI[30]
#define CB_SELECT               (1<<1)
#define UPHY1_SUSPENDM          (1<<0)

////////////////////////////////////////

// QTD: describe data transfer components (buffer, direction, ...)
struct ehci_qtd {
	UINT32			hw_qtd_next;
	UINT32			hw_alt_next;
	UINT32			hw_token;
	UINT32			hw_buf [5];
} __attribute__ ((aligned (32)));

// *_next
#define EHCI_LIST_END           (1<<0)

// token
#define	TD_TOGGLE	    (1 << 31)	    // data toggle
#define TD_TX_BYTE_MASK     (0x7FFF<<16)    // TX byte length
#define TD_PID_CODE_MASK    (0x03<<8)       // PID Code
#define	TD_STS_HALT	    (1 << 6)	    // halted on error

// QH: describes control/bulk/interrupt endpoints
struct ehci_qh {
	UINT32			hw_next;
	UINT32			hw_info1;
	UINT32			hw_info2;
	UINT32			hw_current;

	UINT32			hw_qtd_next;
	UINT32			hw_alt_next;
	UINT32			hw_token;
	UINT32			hw_buf [5];
} __attribute__ ((aligned (32)));   // 48 bytes

// hw_info1
#define	QH_HEAD		            (1<<15)

////////////////////////////////////////

#define EHCI_QH_info0_Default   (0x4040E000)
#define EHCI_TD_info0_Default   (0x00008E80)

#define EHCI_QH_infoOut_Default (0x4200E000)
#define EHCI_TD_infoOut_Default (0x02008C80)

#define EHCI_QH_infoIn_Default  (0x4200E000)
#define EHCI_TD_infoIn_Default  (0x02008D80)

////////////////////////////////////////


#define STRUCT_BUF_ADDR     (&g_io_buf.usb.ehci.reserved[16])
#define EHCI_TD             (g_io_buf.usb.ehci.td)
#define EHCI_QH             (g_io_buf.usb.ehci.qh)
#define EHCI_QH_DUMMY       (g_io_buf.usb.ehci.qh_dumy)
#define EHCI_TD_info0       (g_io_buf.usb.ehci.tdInfo0)
#define EHCI_TD_infoOut     (g_io_buf.usb.ehci.tdInfoOut)
#define EHCI_TD_infoIn      (g_io_buf.usb.ehci.tdInfoIn)
#define EHCI_QH_info0       (g_io_buf.usb.ehci.qhInfo0)
#define EHCI_QH_infoOut     (g_io_buf.usb.ehci.qhInfoOut)
#define EHCI_QH_infoIn      (g_io_buf.usb.ehci.qhInfoIn)
#define EHCI_TD_info_ptr    (g_io_buf.usb.ehci.tdInfoPtr)
#define EHCI_QH_info_ptr    (g_io_buf.usb.ehci.qhInfoPtr)
#define EHCI_endpoint       (g_io_buf.usb.ehci.endpoint)
#define EHCI_outEndpoint    (g_io_buf.usb.ehci.outEndpoint)
#define EHCI_inEndpoint     (g_io_buf.usb.ehci.inEndpoint)
#define EHCI_addr           (g_io_buf.usb.ehci.addr)
#define EHCI_sectorSize     (g_io_buf.usb.ehci.sectorSize)
#define EHCI_clusterSize    (g_io_buf.usb.ehci.clusterSiz)
#define EHCI_fileInfo       (g_io_buf.usb.ehci.fileInfo)
#define EHCI_regAddr        (g_io_buf.usb.ehci.fileInfo[0][0])
#define EHCI_regSize        (g_io_buf.usb.ehci.fileInfo[0][1])
#define EHCI_loader2Addr    (g_io_buf.usb.ehci.fileInfo[1][0])
#define EHCI_loader2Size    (g_io_buf.usb.ehci.fileInfo[1][1])
#define EHCI_romBinAddr     (g_io_buf.usb.ehci.fileInfo[2][0])
#define EHCI_romBinSize     (g_io_buf.usb.ehci.fileInfo[2][1])

#define USB_dataBuf         (g_io_buf.usb.cmd_buf)

////////////////////////////////////////

//#define USB_HUB

// Setup packet format
typedef struct
{
	UINT8 bmRequest;
	UINT8 bRequest;
	UINT16 wValue;
	UINT16 wIndex;
	UINT16 wLength;
} USBsetup;

// CBW packet format
typedef struct
{
	UINT32  dCBWSignature;
	UINT32  dCBWTag;
	UINT32  dCBWDataTransferLength;
	UINT8	bmCBWFlags;
	UINT8	bCBWLUN;
	UINT8	bCBWCBLength;
	UINT8	CBWCB[16];
}sCBW;

// CSW packet format
typedef struct
{
	UINT32  dCBWSignature;
	UINT32  dCBWTag;
	UINT32  dCSWDataResidue;
	UINT8	 bCSWStatus;
}sCSW;

// Standard Device Descriptor
typedef struct
{
	UINT8 bLength;
	UINT8 bDescriptorType;
	UINT16 bcdUSB;
	UINT8 bDeviceClass;
	UINT8 bDeviceSubClass;
	UINT8 bDeviceProtocol;
	UINT8 bMaxPacketSize0;
	UINT16 idVendor;
	UINT16 idProduct;
	UINT16 bcdDevice;
	UINT8 iManufacturer;
	UINT8 iProduct;
	UINT8 iSerialNumber;
	UINT8 bNumConfigurations;
} *pUSB_DevDesc;

// Standard Configuration Descriptor
typedef struct
{
	UINT8 bLength;
	UINT8 bType;
	UINT16 wLength;
	UINT8 bNumIntf;
	UINT8 bCV;
	UINT8 bIndex;
	UINT8 bAttr;
	UINT8 bMaxPower;
} *pUSB_CfgDesc;

#ifdef USB_HUB
// Hub Device descriptor
typedef struct
{
	UINT8 bLength;
	UINT8 bType;
	UINT8 bNumPorts;
	UINT16 wHubChar;
	UINT8 bPO2PG;
	UINT8 bHubConCur;
	UINT8 DevRmv;
	UINT8 PortPwrPtrlMask;
} *pUSB_HubDesc;

typedef struct
{
	UINT16 wDevStatus;;
} *pUSB_DevStatus;

typedef struct
{
	UINT16 wPortStatus;
	UINT16 wPortChange;
} *pUSB_PortStatus;
#endif

////////////////////////////////////////

// USB address
#define HUB_ADRESS			0x02
#define STORAGE_ADDRESS			0x03

// Enum cmd
#define DESC_DEVICE			0x0100
#define DESC_CONFIGURATION		0x0200
#define DESC_HUB			0x2900

// standard request code
#define USB_REQ_GET_STATUS		0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE		0x03
#define USB_REQ_SET_ADDRESS		0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_CONFIG		0x09

// Device feature selector
#define DEVICE_REMOTE_WAKEUP		0x01

// Hub class feature selector
#define S_PORT_RESET			0x04
#define S_PORT_POWER			0x08
#define C_PORT_CONNECTION		0x10
#define C_PORT_RESET			0x14

// USB class code
#define USB_CLASS_MASS_STORAGE		0x08
#define USB_CLASS_HUB			0x09

#define CBWIn				0x80
#define CBWSignature			0x43425355

// scsi command
#define SCSICMD_READ10_CBWTag		0x22000120
#define SCSICMD_READ_10			0x28
#define SCSICMD_TEST_UNIT_READY_CBWTag	0x19821018
#define SCSICMD_TEST_UNIT_READY		0x00
#define SCSICMD_REQUEST_SENSE_CBWTag	0x02030120
#define SCSICMD_REQUEST_SENSE		0x03

typedef struct
{
    UINT8 reserved[64];

    struct ehci_qtd td;
    struct ehci_qh qh;
    struct ehci_qh qh_dumy;

    UINT32 tdInfo0;
    UINT32 tdInfoOut;
    UINT32 tdInfoIn;

    UINT32 qhInfo0;
    UINT32 qhInfoOut;
    UINT32 qhInfoIn;

    UINT32* tdInfoPtr;
    UINT32* qhInfoPtr;

    UINT8 endpoint;
    UINT8 outEndpoint;
    UINT8 inEndpoint;

    UINT8 addr;

    volatile struct ehci_regs *ehci_hcd_regs ;

    UINT32 sectorSize;
    UINT32 clusterSiz;
    UINT32 fileInfo[3][2];
} ehci_usb;


////////////////////////////////////////

int usb_init(int port);
int usb_readSector(u32 lba, u32 count, u32 *dest);


#endif  //_EHCI_USB
