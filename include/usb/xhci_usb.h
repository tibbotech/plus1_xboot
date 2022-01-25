#ifndef _XHCI_USB
#define _XHCI_USB

#include <regmap.h>

#define USB_dataBuf         			(g_io_buf.usb.cmd_buf)

/* dwc3 register bits operation */
/* Global HWPARAMS1 Register */
#define DWC3_GHWPARAMS1_EN_PWROPT(n)		(((n) & (3 << 24)) >> 24)
//#define DWC3_GHWPARAMS1_EN_PWROPT_NO		0
#define DWC3_GHWPARAMS1_EN_PWROPT_CLK		1
/* Global USB2 PHY Configuration Register */
#define DWC3_GUSB2PHYCFG_PHYSOFTRST		(1 << 31)
//#define DWC3_GUSB2PHYCFG_U2_FREECLK_EXISTS	(1 << 30)
#define DWC3_GUSB2PHYCFG_ENBLSLPM		(1 << 8)
#define DWC3_GUSB2PHYCFG_SUSPHY			(1 << 6)
#define DWC3_GUSB2PHYCFG_PHYIF			(1 << 3)
/* Global USB2 PHY Configuration Mask */
#define DWC3_GUSB2PHYCFG_USBTRDTIM_MASK		(0xf << 10)
/* Global USB2 PHY Configuration Offset */
#define DWC3_GUSB2PHYCFG_USBTRDTIM_OFFSET	10

#define DWC3_GUSB2PHYCFG_USBTRDTIM_16BIT 	(0x5 << DWC3_GUSB2PHYCFG_USBTRDTIM_OFFSET)
/* Global USB3 PIPE Control Register */
#define DWC3_GUSB3PIPECTL_PHYSOFTRST		(1 << 31)
//#define DWC3_GUSB3PIPECTL_DISRXDETP3		(1 << 28)
//#define DWC3_GUSB3PIPECTL_SUSPHY		(1 << 17)
/* Global Configuration Register */
//#define DWC3_GCTL_PWRDNSCALE(n)			((n) << 19)
#define DWC3_GCTL_U2RSTECN			(1 << 16)
//#define DWC3_GCTL_RAMCLKSEL(x)			(((x) & DWC3_GCTL_CLK_MASK) << 6)
//#define DWC3_GCTL_CLK_BUS			(0)
//#define DWC3_GCTL_CLK_PIPE			(1)
//#define DWC3_GCTL_CLK_PIPEHALF			(2)
//#define DWC3_GCTL_CLK_MASK			(3)
//#define DWC3_GCTL_PRTCAP(n)			(((n) & (3 << 12)) >> 12)
#define DWC3_GCTL_PRTCAPDIR(n)			((n) << 12)
#define DWC3_GCTL_PRTCAP_HOST			1
//#define DWC3_GCTL_PRTCAP_DEVICE			2
#define DWC3_GCTL_PRTCAP_OTG			3
#define DWC3_GCTL_CORESOFTRESET			(1 << 11)
#define DWC3_GCTL_SCALEDOWN(n)			((n) << 4)
#define DWC3_GCTL_SCALEDOWN_MASK		DWC3_GCTL_SCALEDOWN(3)
#define DWC3_GCTL_DISSCRAMBLE			(1 << 3)
#define DWC3_GCTL_DSBLCLKGTNG			(1 << 0)

/* xhci register bit operation */ 
#define MAX_EP_CTX_NUM				15
#define XHCI_ALIGNMENT				64
/* Max number of USB devices for any host controller - limit in section 6.1 */
#define MAX_HC_SLOTS            		8//256
/* Section 5.3.3 - MaxPorts */
#define MAX_HC_PORTS            		2//255

/* wPortStatus bits */
#define USB_PORT_STAT_CONNECTION    		0x0001
#define USB_PORT_STAT_ENABLE        		0x0002
#define USB_PORT_STAT_SUSPEND       		0x0004
#define USB_PORT_STAT_OVERCURRENT   		0x0008
#define USB_PORT_STAT_RESET         		0x0010
#define USB_PORT_STAT_POWER         		0x0100
#define USB_PORT_STAT_LOW_SPEED     		0x0200
#define USB_PORT_STAT_HIGH_SPEED    		0x0400	/* support for EHCI */
#define USB_PORT_STAT_SUPER_SPEED   		0x0600	/* faking support to XHCI */
/* wPortChange bits */
//#define USB_PORT_STAT_C_CONNECTION  0x0001
//#define USB_PORT_STAT_C_ENABLE      0x0002
//#define USB_PORT_STAT_C_SUSPEND     0x0004
//#define USB_PORT_STAT_C_OVERCURRENT 0x0008
#define USB_PORT_STAT_C_RESET       		0x0010

/*
 * Changes to wPortChange bit fields in USB 3.0
 * See USB 3.0 spec Table 10-12
 */
//#define USB_SS_PORT_STAT_C_BH_RESET	0x0020
//#define USB_SS_PORT_STAT_C_LINK_STATE	0x0040
//#define USB_SS_PORT_STAT_C_CONFIG_ERROR	0x0080
/*
 * These bits are Read Only (RO) and should be saved and written to the
 * registers: 0, 3, 10:13, 30
 * connect status, over-current status, port speed, and device removable.
 * connect status and port speed are also sticky - meaning they're in
 * the AUX well and they aren't changed by a hot, warm, or cold reset.
 */
#define XHCI_PORT_RO		((1 << 0) | (1 << 3) | (0xf << 10) | (1 << 30))
/*
 * These bits are RW; writing a 0 clears the bit, writing a 1 sets the bit:
 * bits 5:8, 9, 14:15, 25:27
 * link state, port power, port indicator state, "wake on" enable state
 */
#define XHCI_PORT_RWS 		((0xf << 5) | (1 << 9) | (0x3 << 14) | (0x7 << 25))
/* USBCMD - USB command - command bitmasks */
/* start/stop HC execution - do not write unless HC is halted*/
#define CMD_RUN			XHCI_CMD_RUN
/* Reset HC - resets internal HC state machine and all registers (except
 * PCI config regs).  HC does NOT drive a USB reset on the downstream ports.
 * The xHCI driver must reinitialize the xHC after setting this bit.
 */
#define CMD_RESET		(1 << 1)
/* bits 12:31 are reserved (and should be preserved on writes). */

/* USBSTS - USB status - status bitmasks */
/* HC not running - set to 1 when run/stop bit is cleared. */
#define STS_HALT		XHCI_STS_HALT
/* true: Controller Not Ready to accept doorbell or op reg writes after reset */
#define STS_CNR			XHCI_STS_CNR
/* CRCR - Command Ring Control Register - cmd_ring bitmasks */
/* bits 4:5 reserved and should be preserved */
/* Command Ring pointer - bit mask for the lower 32 bits. */
#define CMD_RING_RSVD_BITS	(0x3f)
/* PORTSC - Port Status and Control Register - port_status_base bitmasks */
/* true: port enabled */
#define PORT_PE			(1 << 1)
/* bit 2 reserved and zeroed */
/* true: port has an over-current condition */
#define PORT_OC			(1 << 3)
/* true: port reset signaling asserted */
#define PORT_RESET		(1 << 4)
/* Port Link State - bits 5:8
 * A read gives the current link PM state of the port,
 * a write with Link State Write Strobe set sets the link state.
 */
#define PORT_PLS_MASK		(0xf << 5)
//#define XDEV_U0			(0x0 << 5)
//#define XDEV_U2			(0x2 << 5)
#define XDEV_U3			(0x3 << 5)
//#define XDEV_RESUME		(0xf << 5)
/* true: port has power (see HCC_PPC) */
#define PORT_POWER		(1 << 9)
/* bits 10:13 indicate device speed:
 * 0 - undefined speed - port hasn't be initialized by a reset yet
 * 1 - full speed
 * 2 - low speed
 * 3 - high speed
 * 4 - super speed
 * 5-15 reserved
 */
#define DEV_SPEED_MASK		(0xf << 10)
#define	XDEV_FS			(0x1 << 10)
#define	XDEV_LS			(0x2 << 10)
#define	XDEV_HS			(0x3 << 10)
#define	XDEV_SS			(0x4 << 10)
/* Bits 20:23 in the Slot Context are the speed for the device */
#define	SLOT_SPEED_FS		(XDEV_FS << 10)
#define	SLOT_SPEED_LS		(XDEV_LS << 10)
#define	SLOT_SPEED_HS		(XDEV_HS << 10)
#define	SLOT_SPEED_SS		(XDEV_SS << 10)
/* true: connect status change */
#define PORT_CSC		(1 << 17)
/* true: port enable change */
#define PORT_PEC		(1 << 18)
/* true: over-current change */
#define PORT_OCC		(1 << 20)
/* true: reset change - 1 to 0 transition of PORT_RESET */
#define PORT_RC			(1 << 21)
/* erst_size bitmasks */
/* Preserve bits 16:31 of erst_size */
#define	ERST_SIZE_MASK		(0xffff << 16)
/* Event Handler Busy (EHB) - is the event ring scheduled to be serviced by
 * a work queue (or delayed service routine)?
 */
#define ERST_EHB		(1 << 3)
#define ERST_PTR_MASK		(0xf)

/**
 * struct xhci_slot_ctx
 * @dev_info:	Route string, device speed, hub info, and last valid endpoint
 * @dev_info2:	Max exit latency for device number, root hub port number
 * @tt_info:	tt_info is used to construct split transaction tokens
 * @dev_state:	slot state and device address
 *
 * Slot Context - section 6.2.1.1.  This assumes the HC uses 32-byte context
 * structures.  If the HC uses 64-byte contexts, there is an additional 32 bytes
 * reserved at the end of the slot context for HC internal use.
 */
struct xhci_slot_ctx {
	u32	dev_info;
	u32	dev_info2;
	u32	tt_info;
	u32	dev_state;
	/* offset 0x10 to 0x1f reserved for HC internal use */
	u32	reserved[4];
};
#define DEV_HUB			(0x1 << 26)
/* Index of the last valid endpoint context in this device context - 27:31 */
#define LAST_CTX_MASK		(0x1f << 27)
#define LAST_CTX(p)		((p) << 27)
//#define LAST_CTX_TO_EP_NUM(p)	(((p) >> 27) - 1)
#define SLOT_FLAG		(1 << 0)
#define EP0_FLAG		(1 << 1)

/* dev_info2 bitmasks */
/* Root hub port number that is needed to access the USB device */
//#define ROOT_HUB_PORT(p)		(((p) & 0xff) << 16)
#define ROOT_HUB_PORT_MASK	(0xff)
#define ROOT_HUB_PORT_SHIFT	(16)
//#define DEVINFO_TO_ROOT_HUB_PORT(p)	(((p) >> 16) & 0xff)

/* dev_state bitmasks */
/* USB device address - assigned by the HC */
#define DEV_ADDR_MASK		(0xff)

enum usb_device_speed {
	USB_SPEED_UNKNOWN = 0,			/* enumerating */
	USB_SPEED_LOW, USB_SPEED_FULL,		/* usb 1.1 */
	USB_SPEED_HIGH,				/* usb 2.0 */
	USB_SPEED_WIRELESS,			/* wireless (usb 2.5) */
	USB_SPEED_SUPER,			/* usb 3.0 */
};

/* usb host */
/**
 * struct xhci_protocol_caps
 * @revision:		major revision, minor revision, capability ID,
 *			and next capability pointer.
 * @name_string:	Four ASCII characters to say which spec this xHC
 *			follows, typically "USB ".
 * @port_info:		Port offset, count, and protocol-defined information.
 */
//struct xhci_protocol_caps {
//	u32	revision;
//	u32	name_string;
//	u32	port_info;
//};

/**
 * struct xhci_ep_ctx
 * @ep_info:	endpoint state, streams, mult, and interval information.
 * @ep_info2:	information on endpoint type, max packet size, max burst size,
 *		error count, and whether the HC will force an event for all
 *		transactions.
 * @deq:	64-bit ring dequeue pointer address.  If the endpoint only
 *		defines one stream, this points to the endpoint transfer ring.
 *		Otherwise, it points to a stream context array, which has a
 *		ring pointer for each flow.
 * @tx_info:
 *		Average TRB lengths for the endpoint ring and
 *		max payload within an Endpoint Service Interval Time (ESIT).
 *
 * Endpoint Context - section 6.2.1.2.This assumes the HC uses 32-byte context
 * structures.If the HC uses 64-byte contexts, there is an additional 32 bytes
 * reserved at the end of the endpoint context for HC internal use.
 */
struct xhci_ep_ctx {
	u32	ep_info;
	u32	ep_info2;
	u64	deq;
	u32	tx_info;
	/* offset 0x14 - 0x1f reserved for HC internal use */
	u32	reserved[3];
};

/* ep_info bitmasks */
/* Mult - Max number of burtst within an interval, in EP companion desc. */
#define EP_MULT(p)		(((p) & 0x3) << 8)
/* bits 10:14 are Max Primary Streams */
/* bit 15 is Linear Stream Array */
/* Interval - period between requests to an endpoint - 125u increments. */
#define EP_INTERVAL(p)		(((p) & 0xff) << 16)
/* ep_info2 bitmasks */
/*
 * Force Event - generate transfer events for all TRBs for this endpoint
 * This will tell the HC to ignore the IOC and ISP flags (for debugging only).
 */
#define ERROR_COUNT(p)		(((p) & 0x3) << 1)
#define ERROR_COUNT_SHIFT	(1)
#define ERROR_COUNT_MASK	(0x3)
#define EP_TYPE_SHIFT		(3)
#define CTRL_EP			4
/* bit 6 reserved */
/* bit 7 is Host Initiate Disable - for disabling stream selection */
#define MAX_BURST(p)		(((p)&0xff) << 8)
#define MAX_BURST_MASK		(0xff)
#define MAX_BURST_SHIFT		(8)
#define MAX_PACKET(p)		(((p)&0xffff) << 16)
#define MAX_PACKET_MASK		(0xffff)
#define MAX_PACKET_SHIFT	(16)
/* tx_info bitmasks */
#define EP_AVG_TRB_LENGTH(p)		((p) & 0xffff)
#define EP_MAX_ESIT_PAYLOAD_LO(p)	(((p) & 0xffff) << 16)
#define EP_MAX_ESIT_PAYLOAD_HI(p)	((((p) >> 16) & 0xff) << 24)

/**
 * struct xhci_input_control_context
 * Input control context; see section 6.2.5.
 *
 * @drop_context:	set the bit of the endpoint context you want to disable
 * @add_context:	set the bit of the endpoint context you want to enable
 */
struct xhci_input_control_ctx {
	volatile u32	drop_flags;
	volatile u32	add_flags;
	u32		rsvd2[6];
};

/* TODO: write function to set the 64-bit device DMA address */
/*
 * TODO: change this to be dynamically sized at HC mem init time since the HC
 * might not be able to handle the maximum number of devices possible.
 */
/* Transfer event TRB length bit mask */
/* bits 0:23 */
#define EVENT_TRB_LEN(p)	((p) & 0xffffff)

/* Completion Code - only applicable for some types of TRBs */
#define	COMP_CODE_MASK		(0xff << 24)
#define GET_COMP_CODE(p)	(((p) & COMP_CODE_MASK) >> 24)

typedef enum {
	COMP_SUCCESS = 1,
	/* Data Buffer Error */
	COMP_DB_ERR, /* 2 */
	/* Babble Detected Error */
	COMP_BABBLE, /* 3 */
	/* USB Transaction Error */
	COMP_TX_ERR, /* 4 */
	/* TRB Error - some TRB field is invalid */
	COMP_TRB_ERR, /* 5 */
	/* Stall Error - USB device is stalled */
	COMP_STALL, /* 6 */
	/* Resource Error - HC doesn't have memory for that device configuration */
	COMP_ENOMEM, /* 7 */
	/* Bandwidth Error - not enough room in schedule for this dev config */
	COMP_BW_ERR, /* 8 */
	/* No Slots Available Error - HC ran out of device slots */
	COMP_ENOSLOTS, /* 9 */
	/* Invalid Stream Type Error */
	COMP_STREAM_ERR, /* 10 */
	/* Slot Not Enabled Error - doorbell rung for disabled device slot */
	COMP_EBADSLT, /* 11 */
	/* Endpoint Not Enabled Error */
	COMP_EBADEP,/* 12 */
	/* Short Packet */
	COMP_SHORT_TX, /* 13 */
	/* Ring Underrun - doorbell rung for an empty isoc OUT ep ring */
	COMP_UNDERRUN, /* 14 */
	/* Ring Overrun - isoc IN ep ring is empty when ep is scheduled to RX */
	COMP_OVERRUN, /* 15 */
	/* Virtual Function Event Ring Full Error */
	COMP_VF_FULL, /* 16 */
	/* Parameter Error - Context parameter is invalid */
	COMP_EINVAL, /* 17 */
	/* Bandwidth Overrun Error - isoc ep exceeded its allocated bandwidth */
	COMP_BW_OVER,/* 18 */
	/* Context State Error - illegal context state transition requested */
	COMP_CTX_STATE,/* 19 */
	/* No Ping Response Error - HC didn't get PING_RESPONSE in time to TX */
	COMP_PING_ERR,/* 20 */
	/* Event Ring is full */
	COMP_ER_FULL,/* 21 */
	/* Incompatible Device Error */
	COMP_DEV_ERR,/* 22 */
	/* Missed Service Error - HC couldn't service an isoc ep within interval */
	COMP_MISSED_INT,/* 23 */
	/* Successfully stopped command ring */
	COMP_CMD_STOP, /* 24 */
	/* Successfully aborted current command and stopped command ring */
	COMP_CMD_ABORT, /* 25 */
	/* Stopped - transfer was terminated by a stop endpoint command */
	COMP_STOP,/* 26 */
	/* Same as COMP_EP_STOPPED, but the transferred length in the event
	 * is invalid */
	COMP_STOP_INVAL, /* 27*/
	/* Control Abort Error - Debug Capability - control pipe aborted */
	COMP_DBG_ABORT, /* 28 */
	/* Max Exit Latency Too Large Error */
	COMP_MEL_ERR,/* 29 */
	/* TRB type 30 reserved */
	/* Isoc Buffer Overrun - an isoc IN ep sent more data than could fit in TD */
	COMP_BUFF_OVER = 31,
	/* Event Lost Error - xHC has an "internal event overrun condition" */
	COMP_ISSUES, /* 32 */
	/* Undefined Error - reported when other error codes don't apply */
	COMP_UNKNOWN, /* 33 */
	/* Invalid Stream ID Error */
	COMP_STRID_ERR, /* 34 */
	/* Secondary Bandwidth Error - may be returned by a Configure Endpoint cmd */
	COMP_2ND_BW_ERR, /* 35 */
	/* Split Transaction Error */
	COMP_SPLIT_ERR /* 36 */

} xhci_comp_code;

/* control bitfields */
#define LINK_TOGGLE 		(0x1 << 1)


/* flags bitmasks */
/* bits 16:23 are the virtual function ID */
/* bits 24:31 are the slot ID */
#define	TRB_TO_SLOT_ID(p)	(((p) & (0xff << 24)) >> 24)
#define	SLOT_ID_FOR_TRB(p)	(((p) & 0xff) << 24)

/* Stop Endpoint TRB - ep_index to endpoint ID for this TRB */
#define TRB_TO_EP_INDEX(p)	((((p) & (0x1f << 16)) >> 16) - 1)
#define	EP_ID_FOR_TRB(p)	((((p) + 1) & 0x1f) << 16)

/* Normal TRB fields */
/* transfer_len bitmasks - bits 0:16 */
#define	TRB_LEN(p)		((p) & 0x1ffff)
#define	TRB_LEN_MASK		(0x1ffff)
/* Interrupter Target - which MSI-X vector to target the completion event at */
#define	TRB_INTR_TARGET_SHIFT	(22)
#define	TRB_INTR_TARGET_MASK	(0x3ff)
#define TRB_INTR_TARGET(p)	(((p) & 0x3ff) << 22)
/* Cycle bit - indicates TRB ownership by HC or HCD */
#define TRB_CYCLE		(1<<0)

/* Interrupt on short packet */
#define TRB_ISP			(1<<2)
/* Chain multiple TRBs into a TD */
#define TRB_CHAIN		(1<<4)
/* Interrupt on completion */
#define TRB_IOC			(1<<5)
/* The buffer pointer contains immediate data */
#define TRB_IDT			(1<<6)

/* Control transfer TRB specific fields */
#define TRB_DIR_IN		(1<<16)
#define	TRB_TX_TYPE_SHIFT	(16)

/* TRB bit mask */
#define	TRB_TYPE_BITMASK	(0xfc00)
#define TRB_TYPE(p)		((p) << 10)
#define TRB_TYPE_SHIFT		(10)
#define TRB_FIELD_TO_TYPE(p)	(((p) & TRB_TYPE_BITMASK) >> 10)

/* TRB type IDs */
typedef enum {
	/* bulk, interrupt, isoc scatter/gather, and control data stage */
	TRB_NORMAL = 1,
	/* setup stage for control transfers */
	TRB_SETUP, /* 2 */
	/* data stage for control transfers */
	TRB_DATA, /* 3 */
	/* status stage for control transfers */
	TRB_STATUS, /* 4 */
	/* isoc transfers */
	TRB_ISOC, /* 5 */
	/* TRB for linking ring segments */
	TRB_LINK, /* 6 */
	/* TRB for EVENT DATA */
	TRB_EVENT_DATA, /* 7 */
	/* Transfer Ring No-op (not for the command ring) */
	TRB_TR_NOOP, /* 8 */
	/* Command TRBs */
	/* Enable Slot Command */
	TRB_ENABLE_SLOT, /* 9 */
	/* Disable Slot Command */
	TRB_DISABLE_SLOT, /* 10 */
	/* Address Device Command */
	TRB_ADDR_DEV, /* 11 */
	/* Configure Endpoint Command */
	TRB_CONFIG_EP, /* 12 */
	/* Evaluate Context Command */
	TRB_EVAL_CONTEXT, /* 13 */
	/* Reset Endpoint Command */
	TRB_RESET_EP, /* 14 */
	/* Stop Transfer Ring Command */
	TRB_STOP_RING, /* 15 */
	/* Set Transfer Ring Dequeue Pointer Command */
	TRB_SET_DEQ, /* 16 */
	/* Reset Device Command */
	TRB_RESET_DEV, /* 17 */
	/* Force Event Command (opt) */
	TRB_FORCE_EVENT, /* 18 */
	/* Negotiate Bandwidth Command (opt) */
	TRB_NEG_BANDWIDTH, /* 19 */
	/* Set Latency Tolerance Value Command (opt) */
	TRB_SET_LT, /* 20 */
	/* Get port bandwidth Command */
	TRB_GET_BW, /* 21 */
	/* Force Header Command - generate a transaction or link management packet */
	TRB_FORCE_HEADER, /* 22 */
	/* No-op Command - not for transfer rings */
	TRB_CMD_NOOP, /* 23 */
	/* TRB IDs 24-31 reserved */
	/* Event TRBS */
	/* Transfer Event */
	TRB_TRANSFER = 32,
	/* Command Completion Event */
	TRB_COMPLETION, /* 33 */
	/* Port Status Change Event */
	TRB_PORT_STATUS, /* 34 */
	/* Bandwidth Request Event (opt) */
	TRB_BANDWIDTH_EVENT, /* 35 */
	/* Doorbell Event (opt) */
	TRB_DOORBELL, /* 36 */
	/* Host Controller Event */
	TRB_HC_EVENT, /* 37 */
	/* Device Notification Event - device sent function wake notification */
	TRB_DEV_NOTE, /* 38 */
	/* MFINDEX Wrap Event - microframe counter wrapped */
	TRB_MFINDEX_WRAP, /* 39 */
	/* TRB IDs 40-47 reserved, 48-63 is vendor-defined */
	/* Nec vendor-specific command completion event. */
	TRB_NEC_CMD_COMP = 48, /* 48 */
	/* Get NEC firmware revision. */
	TRB_NEC_GET_FW, /* 49 */
} trb_type;

/* Above, but for u32 types -- can avoid work by swapping constants: */
#define TRB_TYPE_LINK_LE32(x)	(((x) & TRB_TYPE_BITMASK) == TRB_TYPE(TRB_LINK))

/*
 * TRBS_PER_SEGMENT must be a multiple of 4,
 * since the command ring is 64-byte aligned.
 * It must also be greater than 16.
 */
#define TRBS_PER_SEGMENT	16
/* Allow two commands + a link TRB, along with any reserved command TRBs */
//#define MAX_RSVD_CMD_TRBS	(TRBS_PER_SEGMENT - 3)
//#define SEGMENT_SIZE		(TRBS_PER_SEGMENT*16)
/* SEGMENT_SHIFT should be log2(SEGMENT_SIZE).
 * Change this if you change TRBS_PER_SEGMENT!
 */
//#define SEGMENT_SHIFT		10
/* TRB buffer pointers can't cross 64KB boundaries */
#define TRB_MAX_BUFF_SHIFT	16
#define TRB_MAX_BUFF_SIZE	(1 << TRB_MAX_BUFF_SHIFT)

/*
 * Each segment table entry is 4*32bits long.  1K seems like an ok size:
 * (1K bytes * 8bytes/bit) / (4*32 bits) = 64 segment entries in the table,
 * meaning 64 ring segments.
 * Initial allocated size of the ERST, in number of entries */
#define	ERST_NUM_SEGS		4
/* Initial number of event segment rings allocated */
//#define	ERST_ENTRIES	1
/* Initial allocated size of the ERST, in number of entries */
//#define	ERST_SIZE	64

#define CTX_SIZE(_hcc) 		(HCC_64BYTE_CONTEXT(_hcc) ? 64 : 32)

/*************************************************************
	EXTENDED CAPABILITY DEFINITIONS
*************************************************************/
/* Up to 16 ms to halt an HC */
#define XHCI_MAX_HALT_USEC	(16*1000)
/* HC not running - set to 1 when run/stop bit is cleared. */
#define XHCI_STS_HALT		(1 << 0)

/* HCCPARAMS offset from PCI base address */
 //#define XHCI_HCC_PARAMS_OFFSET	0x10
/* HCCPARAMS contains the first extended capability pointer */
//#define XHCI_HCC_EXT_CAPS(p)	(((p)>>16)&0xffff)

/* command register values to disable interrupts and halt the HC */
/* start/stop HC execution - do not write unless HC is halted*/
#define XHCI_CMD_RUN		(1 << 0)
/* Event Interrupt Enable - get irq when EINT bit is set in USBSTS register */
//#define XHCI_CMD_EIE		(1 << 2)
/* Host System Error Interrupt Enable - get irq when HSEIE bit set in USBSTS */
//#define XHCI_CMD_HSEIE		(1 << 3)
/* Enable Wrap Event - '1' means xHC generates an event when MFINDEX wraps. */
//#define XHCI_CMD_EWE		(1 << 10)

//#define XHCI_IRQS		(XHCI_CMD_EIE | XHCI_CMD_HSEIE | XHCI_CMD_EWE)

/* true: Controller Not Ready to accept doorbell or op reg writes after reset */
#define XHCI_STS_CNR		(1 << 11)

////////////////////////////////////////
#define USB_MAXENDPOINTS		16
#define USB_MAXINTERFACES		8

#define USB_DT_CONFIG_SIZE		9
#define USB_DT_INTERFACE_SIZE		9
#define USB_DT_SS_EP_COMP_SIZE		6

#define USB_DT_ENDPOINT_SIZE		7
#define USB_DT_ENDPOINT_AUDIO_SIZE	9

#define USB_DT_INTERFACE		0x04
#define USB_DT_ENDPOINT			0x05
#define	USB_DT_SS_ENDPOINT_COMP		0x30

/* Endpoints */
#define USB_ENDPOINT_NUMBER_MASK  	0x0f	/* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK     	0x80

#define USB_ENDPOINT_XFERTYPE_MASK 	0x03
#define USB_ENDPOINT_XFER_CONTROL	0
#define USB_ENDPOINT_XFER_ISOC		1
#define USB_ENDPOINT_XFER_BULK		2
#define USB_ENDPOINT_XFER_INT      	3

//#define USB_ENDPOINT_MAXP_MASK		0x07ff
#define USB_EP_MAXP_MULT_SHIFT		11
#define USB_EP_MAXP_MULT_MASK		(3 << USB_EP_MAXP_MULT_SHIFT)
#define USB_EP_MAXP_MULT(m)		(((m) & USB_EP_MAXP_MULT_MASK) >> USB_EP_MAXP_MULT_SHIFT)
// Setup packet format
struct devrequest {
	u8	requesttype;
	u8	request;
	u16	value;
	u16	index;
	u16	length;
} __attribute__ ((packed));

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
	UINT32	dCBWSignature;
	UINT32  dCBWTag;
	UINT32  dCSWDataResidue;
	UINT8	bCSWStatus;
}sCSW;

#define USB_MAXCHILDREN 8
/* Hub descriptor */
struct usb_hub_descriptor {
	u8  bLength;
	u8  bDescriptorType;
	u8  bNbrPorts;
	u16 wHubCharacteristics;
	u8  bPwrOn2PwrGood;
	u8  bHubContrCurrent;
	/* 2.0 and 3.0 hubs differ here */
	union {
		struct {
			/* add 1 bit for hub status change; round to bytes */
			u8 DeviceRemovable[(USB_MAXCHILDREN + 1 + 7) / 8];
			u8 PortPowerCtrlMask[(USB_MAXCHILDREN + 1 + 7) / 8];
		} __attribute__ ((packed)) hs;

		struct {
			u8 bHubHdrDecLat;
			u16 wHubDelay;
			u16 DeviceRemovable;
		} __attribute__ ((packed)) ss;
	} u;
} __attribute__ ((packed));
// Standard Device Descriptor
typedef struct
{
	UINT8	bLength;
	UINT8 	bDescriptorType;
	UINT16 	bcdUSB;
	UINT8 	bDeviceClass;
	UINT8 	bDeviceSubClass;
	UINT8 	bDeviceProtocol;
	UINT8 	bMaxPacketSize0;
	UINT16 	idVendor;
	UINT16 	idProduct;
	UINT16 	bcdDevice;
	UINT8 	iManufacturer;
	UINT8 	iProduct;
	UINT8 	iSerialNumber;
	UINT8 	bNumConfigurations;
} *pUSB_DevDesc;

// Standard Configuration Descriptor
typedef struct
{
	UINT8 	bLength;
	UINT8 	bType;
	UINT16 	wLength;
	UINT8 	bNumIntf;
	UINT8 	bCV;
	UINT8 	bIndex;
	UINT8 	bAttr;
	UINT8 	bMaxPower;
}  __attribute__ ((packed)) *pUSB_CfgDesc;

/* All standard descriptors have these 2 fields at the beginning */
struct usb_descriptor_header {
	u8  bLength;
	u8  bDescriptorType;
} __attribute__ ((packed));

struct usb_config_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u16 wTotalLength;
	u8  bNumInterfaces;
	u8  bConfigurationValue;
	u8  iConfiguration;
	u8  bmAttributes;
	u8  bMaxPower;
} __attribute__ ((packed));

/* USB_DT_INTERFACE: Interface descriptor */
struct usb_interface_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  bInterfaceNumber;
	u8  bAlternateSetting;
	u8  bNumEndpoints;
	u8  bInterfaceClass;
	u8  bInterfaceSubClass;
	u8  bInterfaceProtocol;
	u8  iInterface;
} __attribute__ ((packed));

/* USB_DT_ENDPOINT: Endpoint descriptor */
struct usb_endpoint_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  bEndpointAddress;
	u8  bmAttributes;
	u16 wMaxPacketSize;
	u8  bInterval;

	/* NOTE:  these two are _only_ in audio endpoints. */
	/* use USB_DT_ENDPOINT*_SIZE in bLength, not sizeof. */
	u8  bRefresh;
	u8  bSynchAddress;
} __attribute__ ((packed));

/* USB_DT_SS_ENDPOINT_COMP: SuperSpeed Endpoint Companion descriptor */
struct usb_ss_ep_comp_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  bMaxBurst;
	u8  bmAttributes;
	u16 wBytesPerInterval;
} __attribute__ ((packed));

/* Interface */
struct usb_interface {
	struct usb_interface_descriptor desc;

	u8				no_of_ep;
	u8				num_altsetting;
	u8				act_altsetting;

	struct usb_endpoint_descriptor 	ep_desc[USB_MAXENDPOINTS];
	/*
	 * Super Speed Device will have Super Speed Endpoint
	 * Companion Descriptor  (section 9.6.7 of usb 3.0 spec)
	 * Revision 1.0 June 6th 2011
	 */
	struct usb_ss_ep_comp_descriptor ss_ep_comp_desc[USB_MAXENDPOINTS];
} __attribute__ ((packed));

struct usb_port_status {
	u16 wPortStatus;
	u16 wPortChange;
} __attribute__ ((packed));

/* Configuration information.. */
struct usb_config {
	struct usb_config_descriptor 	desc;

	u8				no_of_if;	/* number of interfaces */
	struct usb_interface 		if_desc[USB_MAXINTERFACES];
} __attribute__ ((packed));

typedef struct {
	int			devnum;			/* Device number on USB bus */
	int			speed;			/* full/low/high */
	char			mf[32];			/* manufacturer */
	char			prod[32];		/* product */
	char			serial[32];		/* serial number */

	/* Maximum packet size; one of: PACKET_SIZE_* */
	int 			maxpacketsize;
	/* [0] = IN, [1] = OUT */
	int     		ep_dir[2];
	int 			lun;
	/* one bit for each endpoint ([0] = IN, [1] = OUT) */
	//unsigned int toggle[2];
	/* endpoint halts; one bit per endpoint # & direction;
	 * [0] = IN, [1] = OUT
	 */
	//unsigned int halted[2];
	int 			epmaxpacketin[4];//[16];		/* INput endpoint specific maximums */
	int 			epmaxpacketout[4];//[16];		/* OUTput endpoint specific maximums */

	//int configno;			/* selected config number */
	/* Device Descriptor */
	//struct usb_device_descriptor descriptor
	//	__attribute__((aligned(ARCH_DMA_MINALIGN)));
	struct usb_config 	config; /* config descriptor */

	//int have_langid;		/* whether string_langid is valid yet */
	int 			string_langid;		/* language ID for strings */
	
	//unsigned long status;
	int 			act_len;			/* transferred bytes */
	int 			portnr[8];			/* Port number, 1=first */
	int 			depth;
	/* slot_id - for xHCI enabled devices */
	unsigned int 		r_slot_id;
	unsigned int 		slot_id;
	u32 			CBWTag;
	u8 			Hub_NbrPorts;
} usb_device;
////////////////////////////////////////
#define USB_RECIP_ENDPOINT    		0x02

#define USB_DIR_OUT			0		/* to device */
#define USB_DIR_IN			0x80		/* to host */

#define USB_REQ_GET_STATUS		0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE		0x03
#define USB_REQ_SET_ADDRESS		0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE		0x0A
#define USB_REQ_SET_INTERFACE		0x0B
#define USB_REQ_SYNCH_FRAME		0x0C
#define USB_REQ_SET_SEL			0x30
#define USB_REQ_SET_ISOCH_DELAY		0x31

// Enum cmd
#define DESC_DEVICE                 	0x0100
#define DESC_CONFIGURATION          	0x0200

#define CBWFLAGS_OUT			0x00
#define CBWFLAGS_IN			0x80
#define CBWSignature                	0x43425355
#define CSWSignature			0x53425355

#define UMASS_BBB_CBW_SIZE		31
#define UMASS_BBB_CSW_SIZE		13
// scsi command
#define SCSI_INQUIRY			0x12
#define SCSI_RD_CAPAC			0x25
#define SCSICMD_READ_10                 0x28
#define SCSICMD_TEST_UNIT_READY	        0x00
#define SCSICMD_REQUEST_SENSE           0x03

#define DWC3_REG_OFFSET			0xC100

/*
 * XHCI Register Space.
 */
struct xhci_hccr {
	u32 cr_capbase;
	u32 cr_hcsparams1;
	u32 cr_hcsparams2;
	u32 cr_hcsparams3;
	u32 cr_hccparams;
	u32 cr_dboff;
	u32 cr_rtsoff;

/* hc_capbase bitmasks */
/* bits 7:0 - how long is the Capabilities register */
#define HC_LENGTH(p)		XHCI_HC_LENGTH(p)
/* Capability Register */
/* bits 7:0 - how long is the Capabilities register */
#define XHCI_HC_LENGTH(p)	(((p) >> 00) & 0x00ff)
/* bits 31:16	*/
#define HC_VERSION(p)		(((p) >> 16) & 0xffff)

/* HCSPARAMS1 - hcs_params1 - bitmasks */
/* bits 0:7, Max Device Slots */
#define HCS_MAX_SLOTS(p)	(((p) >> 0) & 0xff)
#define HCS_SLOTS_MASK		0xff
/* bits 8:18, Max Interrupters */
#define HCS_MAX_INTRS(p)	(((p) >> 8) & 0x7ff)
/* bits 24:31, Max Ports - max value is 0x7F = 127 ports */
#define HCS_MAX_PORTS_SHIFT	24
#define HCS_MAX_PORTS_MASK	(0xff << HCS_MAX_PORTS_SHIFT)
#define HCS_MAX_PORTS(p)	(((p) >> 24) & 0xff)

/* HCSPARAMS2 - hcs_params2 - bitmasks */
/* bits 0:3, frames or uframes that SW needs to queue transactions
 * ahead of the HW to meet periodic deadlines */
#define HCS_IST(p)		(((p) >> 0) & 0xf)
/* bits 4:7, max number of Event Ring segments */
#define HCS_ERST_MAX(p)		(((p) >> 4) & 0xf)
/* bits 21:25 Hi 5 bits of Scratchpad buffers SW must allocate for the HW */
/* bit 26 Scratchpad restore - for save/restore HW state - not used yet */
/* bits 27:31 Lo 5 bits of Scratchpad buffers SW must allocate for the HW */
#define HCS_MAX_SCRATCHPAD(p)	((((p) >> 16) & 0x3e0) | (((p) >> 27) & 0x1f))

/* HCSPARAMS3 - hcs_params3 - bitmasks */
/* bits 0:7, Max U1 to U0 latency for the roothub ports */
#define HCS_U1_LATENCY(p)	(((p) >> 0) & 0xff)
/* bits 16:31, Max U2 to U0 latency for the roothub ports */
#define HCS_U2_LATENCY(p)	(((p) >> 16) & 0xffff)

/* HCCPARAMS - hcc_params - bitmasks */
/* true: HC can use 64-bit address pointers */
#define HCC_64BIT_ADDR(p)	((p) & (1 << 0))
/* true: HC can do bandwidth negotiation */
#define HCC_BANDWIDTH_NEG(p)	((p) & (1 << 1))
/* true: HC uses 64-byte Device Context structures
 * FIXME 64-byte context structures aren't supported yet.
 */
#define HCC_64BYTE_CONTEXT(p)	((p) & (1 << 2))
/* true: HC has port power switches */
#define HCC_PPC(p)		((p) & (1 << 3))
/* true: HC has port indicators */
#define HCS_INDICATOR(p)	((p) & (1 << 4))
/* true: HC has Light HC Reset Capability */
#define HCC_LIGHT_RESET(p)	((p) & (1 << 5))
/* true: HC supports latency tolerance messaging */
#define HCC_LTC(p)		((p) & (1 << 6))
/* true: no secondary Stream ID Support */
#define HCC_NSS(p)		((p) & (1 << 7))
/* Max size for Primary Stream Arrays - 2^(n+1), where n is bits 12:15 */
#define HCC_MAX_PSA(p)		(1 << ((((p) >> 12) & 0xf) + 1))
/* Extended Capabilities pointer from PCI base - section 5.3.6 */
//#define HCC_EXT_CAPS(p)		XHCI_HCC_EXT_CAPS(p)
/* Maximum number of ports under a hub device */
#define XHCI_MAX_PORTS(p)	(((p) & 0xff) << 24)
/*
 * The number of the downstream facing port of the high-speed hub
 * '0' if the device is not low or full speed.
 */
//#define TT_PORT(p)		(((p) & 0xff) << 8)
#define TT_THINK_TIME(p)	(((p) & 0x3) << 16)
/* db_off bitmask - bits 0:1 reserved */
#define	DBOFF_MASK		(~0x3)

/* run_regs_off bitmask - bits 0:4 reserved */
#define	RTSOFF_MASK		(~0x1f)
};

struct xhci_hcor_port_regs {
	volatile u32 or_portsc;
	volatile u32 or_portpmsc;
	volatile u32 or_portli;
	volatile u32 reserved_3;
};

struct xhci_hcor {
	volatile u32 or_usbcmd;
	volatile u32 or_usbsts;
	volatile u32 or_pagesize;
	volatile u32 reserved_0[2];
	volatile u32 or_dnctrl;
	union
	{		
		struct
		{	
			volatile u64 or_crcrL:32;
			volatile u64 or_crcrH:32;
		};
		volatile u64 or_crcr;
	};
	volatile u32 reserved_1[4];
	union
	{		
		struct
		{	
			volatile u64 or_dcbaapL:32;
			volatile u64 or_dcbaapH:32;
		};
		volatile u64 or_dcbaap;
	};	
	volatile u32 or_config;
	volatile u32 reserved_2[241];
	struct xhci_hcor_port_regs portregs[MAX_HC_PORTS];
};
/**
 * struct doorbell_array
 *
 * Bits  0 -  7: Endpoint target
 * Bits  8 - 15: RsvdZ
 * Bits 16 - 31: Stream ID
 *
 * Section 5.6
 */
struct xhci_doorbell_array {
	volatile u32	doorbell[256];
};

#define DB_VALUE(ep, stream)	((((ep) + 1) & 0xff) | ((stream) << 16))
#define DB_VALUE_HOST		0x00000000

/**
* struct xhci_intr_reg - Interrupt Register Set
* @irq_pending:	IMAN - Interrupt Management Register.  Used to enable
*			interrupts and check for pending interrupts.
* @irq_control:	IMOD - Interrupt Moderation Register.
*			Used to throttle interrupts.
* @erst_size:		Number of segments in the
			Event Ring Segment Table (ERST).
* @erst_base:		ERST base address.
* @erst_dequeue:	Event ring dequeue pointer.
*
* Each interrupter (defined by a MSI-X vector) has an event ring and an Event
* Ring Segment Table (ERST) associated with it.
* The event ring is comprised of  multiple segments of the same size.
* The HC places events on the ring and  "updates the Cycle bit in the TRBs to
* indicate to software the current  position of the Enqueue Pointer."
* The HCD (Linux) processes those events and  updates the dequeue pointer.
*/
struct xhci_intr_reg {
	volatile u32	irq_pending;
	volatile u32	irq_control;
	volatile u32	erst_size;
	volatile u32	rsvd;
	volatile u64	erst_base;
	volatile u64	erst_dequeue;
};
/**
 * struct xhci_run_regs
 * @microframe_index:	MFINDEX - current microframe number
 *
 * Section 5.5 Host Controller Runtime Registers:
 * "Software should read and write these registers using only Dword (32 bit)
 * or larger accesses"
 */
struct xhci_run_regs {
	u32			microframe_index;
	u32			rsvd[7];
	struct xhci_intr_reg	ir_set[128];
};
/**
 * struct xhci_device_context_array
 * @dev_context_ptr	array of 64-bit DMA addresses for device contexts
 */
struct xhci_device_context_array {
	/* 64-bit device addresses; we only write 32-bit addresses */
	u64			dev_context_ptrs[MAX_HC_SLOTS];
};

struct xhci_link_trb {
	/* 64-bit segment pointer*/
	volatile u64 segment_ptr;
	volatile u32 intr_target;
	volatile u32 control;
};
struct xhci_transfer_event {
	/* 64-bit buffer address, or immediate data */
	u64		buffer;
	u32		transfer_len;
	/* This field is interpreted differently based on the type of TRB */
	volatile u32	flags;
};
/* Command completion event TRB */
struct xhci_event_cmd {
	/* Pointer to command TRB, or the value passed by the event data trb */
	volatile u64 cmd_trb;
	volatile u32 status;
	volatile u32 flags;
};
struct xhci_generic_trb {
	volatile u32 field[4];
};
union xhci_trb {
	struct xhci_link_trb		link;
	struct xhci_transfer_event	trans_event;
	struct xhci_event_cmd		event_cmd;
	struct xhci_generic_trb		generic;
};
struct xhci_segment {
	union xhci_trb		*trbs;
	/* private to HCD */
	struct xhci_segment	*next;
};
struct xhci_ring {
	struct xhci_segment	*first_seg;
	union  xhci_trb		*enqueue;
	struct xhci_segment	*enq_seg;
	union  xhci_trb		*dequeue;
	struct xhci_segment	*deq_seg;
	/*
	 * Write the cycle state into the TRB cycle field to give ownership of
	 * the TRB to the host controller (if we are the producer), or to check
	 * if we own the TRB (if we are the consumer).  See section 4.9.1.
	 */
	volatile u32		cycle_state;
	unsigned int		num_segs;
};

struct xhci_erst_entry {
	/* 64-bit event ring segment address */
	u64	seg_addr;
	u32	seg_size;
	/* Set to zero */
	u32	rsvd;
};

struct xhci_erst {
	struct xhci_erst_entry	*entries;
	unsigned int		num_entries;
	/* Num entries the ERST can contain */
	unsigned int		erst_size;
};

struct xhci_scratchpad {
	u64 *sp_array;
};

/**
 * struct xhci_container_ctx
 * @type: Type of context.  Used to calculated offsets to contained contexts.
 * @size: Size of the context data
 * @bytes: The raw context data given to HW
 *
 * Represents either a Device or Input context.  Holds a pointer to the raw
 * memory used for the context (bytes).
 */
struct xhci_container_ctx {
	unsigned 	type;
#define XHCI_CTX_TYPE_DEVICE  0x1
#define XHCI_CTX_TYPE_INPUT   0x2

	int 		size;
	u8 		*bytes;
};
struct xhci_virt_ep {
	struct xhci_ring	*ring;
	unsigned int		ep_state;
#define SET_DEQ_PENDING		(1 << 0)
#define EP_HALTED		(1 << 1)	/* For stall handling */
#define EP_HALT_PENDING		(1 << 2)	/* For URB cancellation */
/* Transitioning the endpoint to using streams, don't enqueue URBs */
#define EP_GETTING_STREAMS	(1 << 3)
#define EP_HAS_STREAMS		(1 << 4)
/* Transitioning the endpoint to not using streams, don't enqueue URBs */
#define EP_GETTING_NO_STREAMS	(1 << 5)
};

struct xhci_virt_device {
	/*
	 * Commands to the hardware are passed an "input context" that
	 * tells the hardware what to change in its data structures.
	 * The hardware will return changes in an "output context" that
	 * software must allocate for the hardware.  We need to keep
	 * track of input and output contexts separately because
	 * these commands might fail and we don't trust the hardware.
	 */
	struct xhci_container_ctx       *out_ctx;
	/* Used for addressing devices and configuration changes */
	struct xhci_container_ctx       *in_ctx;
	/* Rings saved to ensure old alt settings can be re-instated */
//#define	XHCI_MAX_RINGS_CACHED		31
	struct xhci_virt_ep		eps[MAX_EP_CTX_NUM];//eps[31];
};

#define ep_num				5
typedef struct {
	u8 				sparraybuf[4096]; //assume num_sp = 1, 1 * (1 << 12)
		
	union xhci_trb 			pcmdtrb[TRBS_PER_SEGMENT] \
					__attribute__ ((aligned(XHCI_ALIGNMENT)));
	
	union xhci_trb 			peventtrb[ERST_NUM_SEGS][TRBS_PER_SEGMENT] \
					__attribute__ ((aligned(XHCI_ALIGNMENT)));
					
	union xhci_trb 			peptrb[ep_num][TRBS_PER_SEGMENT] \
					__attribute__ ((aligned(XHCI_ALIGNMENT)));
					
	u64				dev_context_ptrs[MAX_HC_SLOTS] \
					__attribute__ ((aligned(XHCI_ALIGNMENT)));
	u8 				reserved[64] \
					__attribute__ ((aligned(XHCI_ALIGNMENT)));
	//virt device
	struct xhci_virt_device 	pdevs[2];	
	struct xhci_container_ctx 	pout_ctx[2];	
	u8 				poutbyte[2][(MAX_EP_CTX_NUM + 1) * 64] \
					__attribute__ ((aligned(XHCI_ALIGNMENT))); // 32 * 64, (MAX_EP_CTX_NUM + 1) * CTX_SIZE(readl(&ctrl->hccr->cr_hccparams));
	struct xhci_container_ctx 	pin_ctx[2];
	u8 				pinbyte[2][(MAX_EP_CTX_NUM + 2) * 64] \
					__attribute__ ((aligned(XHCI_ALIGNMENT)));
	struct xhci_erst_entry 		pentries[ERST_NUM_SEGS] \
					__attribute__ ((aligned(XHCI_ALIGNMENT)));
	u64				sparray[8] \
					__attribute__ ((aligned(XHCI_ALIGNMENT))); //assume num_sp = 1
	struct xhci_device_context_array *dcbaa \
					__attribute__ ((aligned(XHCI_ALIGNMENT)));
									
	struct xhci_hccr 		*hccr;	/* R/O registers, not need for volatile */
	struct xhci_hcor 		*hcor;
	struct xhci_doorbell_array 	*dba;
	struct xhci_run_regs 		*run_regs;
	
	struct xhci_ring 		*event_ring;
	struct xhci_ring 		*cmd_ring;
	struct xhci_ring 		*transfer_ring;
	//struct xhci_segment 		*seg;
	struct xhci_intr_reg 		*ir_set;
	
	struct xhci_scratchpad 		*scratchpad;
	struct xhci_virt_device 	*devs[MAX_HC_SLOTS];
	struct xhci_erst 		erst;	
	//int rootdev;	
	struct xhci_scratchpad 		pscratchpad;	
	//cmd ring			
	struct xhci_ring 		pcmd_ring;
	struct xhci_segment 		pcmd_ring_first_seg;
	//event ring	
	struct xhci_ring 		pevent_ring;
	struct xhci_segment 		pevent_ring_first_seg[ERST_NUM_SEGS];
	//ep ring
	struct xhci_ring 		pep_ring[ep_num];
	struct xhci_segment 		pep_ring_first_seg[ep_num];
	usb_device 			udev;		
} xhci_usb;

struct g_event_buffer {
	u32 g_evntadrlo;
	u32 g_evntadrhi;
	u32 g_evntsiz;
	u32 g_evntcount;
};

struct d_physical_endpoint {
	u32 d_depcmdpar2;
	u32 d_depcmdpar1;
	u32 d_depcmdpar0;
	u32 d_depcmd;
};

struct dwc3 {					/* offset: 0xC100 */
	u32 				g_sbuscfg0;
	u32 				g_sbuscfg1;
	u32 				g_txthrcfg;
	u32 				g_rxthrcfg;
	u32 				g_ctl;

	u32 				reserved1;

	u32 				g_sts;

	u32 				reserved2;

	u32 				g_snpsid;
	u32 				g_gpio;
	u32 				g_uid;
	u32 				g_uctl;
	u64 				g_buserraddr;
	u64 				g_prtbimap;

	u32 				g_hwparams0;
	u32 				g_hwparams1;
	u32 				g_hwparams2;
	u32 				g_hwparams3;
	u32 				g_hwparams4;
	u32 				g_hwparams5;
	u32 				g_hwparams6;
	u32 				g_hwparams7;

	u32 				g_dbgfifospace;
	u32 				g_dbgltssm;
	u32 				g_dbglnmcc;
	u32 				g_dbgbmu;
	u32 				g_dbglspmux;
	u32 				g_dbglsp;
	u32 				g_dbgepinfo0;
	u32 				g_dbgepinfo1;

	u64 				g_prtbimap_hs;
	u64 				g_prtbimap_fs;

	u32 				reserved3[28];

	u32 				g_usb2phycfg[16];
	u32 				g_usb2i2cctl[16];
	u32 				g_usb2phyacc[16];
	u32 				g_usb3pipectl[16];

	u32 				g_txfifosiz[32];
	u32 				g_rxfifosiz[32];

	struct g_event_buffer 		g_evnt_buf[32];

	u32 				g_hwparams8;

	u32 				reserved4[11];

	u32 				g_fladj;

	u32 				reserved5[51];

	u32 				d_cfg;
	u32 				d_ctl;
	u32 				d_evten;
	u32 				d_sts;
	u32 				d_gcmdpar;
	u32 				d_gcmd;

	u32 				reserved6[2];

	u32 				d_alepena;

	u32 				reserved7[55];

	struct d_physical_endpoint 	d_phy_ep_cmd[32];

	u32 				reserved8[128];

	u32 				o_cfg;
	u32 				o_ctl;
	u32 				o_evt;
	u32 				o_evten;
	u32 				o_sts;

	u32 				reserved9[3];

	u32 				adp_cfg;
	u32 				adp_ctl;
	u32 				adp_evt;
	u32 				adp_evten;

	u32 				bc_cfg;

	u32 				reserved10;

	u32 				bc_evt;
	u32 				bc_evten;
};
////////////////////////////////////////
/*
 * "pipe" definitions, use unsigned so we can compare reliably, since this
 * value is shifted up to bits 30/31.
 */
//#define PIPE_ISOCHRONOUS    0U
//#define PIPE_INTERRUPT      1U
#define PIPE_CONTROL        	2U
#define PIPE_BULK           	3U
//#define PIPE_DEVEP_MASK     0x0007ff00

//#define usb_pipeout(pipe)	((((pipe) >> 7) & 1) ^ 1)
#define usb_pipein(pipe)	(((pipe) >> 7) & 1)
//#define usb_pipedevice(pipe)	(((pipe) >> 8) & 0x7f)
//#define usb_pipe_endpdev(pipe)	(((pipe) >> 8) & 0x7ff)
#define usb_pipeendpoint(pipe)	(((pipe) >> 15) & 0xf)
//#define usb_pipedata(pipe)	(((pipe) >> 19) & 1)
#define usb_pipetype(pipe)	(((pipe) >> 30) & 3)
//#define usb_pipeisoc(pipe)	(usb_pipetype((pipe)) == PIPE_ISOCHRONOUS)
//#define usb_pipeint(pipe)	(usb_pipetype((pipe)) == PIPE_INTERRUPT)
#define usb_pipecontrol(pipe)	(usb_pipetype((pipe)) == PIPE_CONTROL)
//#define usb_pipebulk(pipe)	(usb_pipetype((pipe)) == PIPE_BULK)

#define usb_pipe_ep_index(pipe)		usb_pipecontrol(pipe) ? (usb_pipeendpoint(pipe) * 2) : \
			        	((usb_pipeendpoint(pipe) * 2) - (usb_pipein(pipe) ? 0 : 1))
#define create_pipe(dev,endpoint) 	(((dev)->devnum << 8) | ((endpoint) << 15) | (dev)->maxpacketsize)
#define usb_sndctrlpipe(dev, endpoint)	((PIPE_CONTROL << 30) | create_pipe(dev, endpoint))
#define usb_rcvctrlpipe(dev, endpoint)	((PIPE_CONTROL << 30) | create_pipe(dev, endpoint) | USB_DIR_IN)
#define usb_sndbulkpipe(dev, endpoint)	((PIPE_BULK << 30) | create_pipe(dev, endpoint))
#define usb_rcvbulkpipe(dev, endpoint)	((PIPE_BULK << 30) | create_pipe(dev, endpoint) | USB_DIR_IN)
////////////////////////////////////////

int usb_init(int port, int next_port_in_hub);
int usb_readSector(u32 lba, u32 count, u32 *dest);

#endif  //_XHCI_USB
