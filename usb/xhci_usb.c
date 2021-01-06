#include <types.h>
#include <usb/xhci_usb.h>
#include <fat/fat.h>
#include <common.h>

#ifdef FPGA
#define XHCI_DEBUG
#endif

#ifdef XBOOT_BUILD
#define USB_DBG()
#else
#define USB_DBG()   dbg()
#define USB_PRN
#endif

void USB_vendorCmd(u8 bReq, u8 bCmd, u16 wValue, u16 wIndex, u16 wLen);
void stor_BBB_transport(u32 datalen, u32 cmdlen, u8 dir_in, u8 *buf);
extern void _delay_1ms(UINT32 period); // force delay even in CSIM
extern void boot_reset(void);

#define MAX_TEST_UNIT_READY_TRY   5

// UPHY 2 & 3 init (dh_feng)
#if defined(PLATFORM_I143)
void uphy_init(void)
{
	// 1. enable UPHY 2/3 & USBC 0/1 HW CLOCK */
	MOON0_REG->clken[2] = RF_MASK_V_SET(1 << 15);
	MOON0_REG->clken[2] = RF_MASK_V_SET(1 << 9);
	_delay_1ms(1);

	// 2. reset UPHY 2/3
	MOON0_REG->reset[2] = RF_MASK_V_SET(1 << 15);
	MOON0_REG->reset[2] = RF_MASK_V_SET(1 << 9);
	_delay_1ms(1);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(1 << 15);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(1 << 9);
	_delay_1ms(1);

	// 3. Default value modification
	UPHY2_RN_REG->gctrl[0] = 0x18888002;
	_delay_1ms(1);

	// 4. PLL power off/on twice
	UPHY2_RN_REG->gctrl[2] = 0x88;
	_delay_1ms(1);
	UPHY2_RN_REG->gctrl[2] = 0x80;
	_delay_1ms(1);
	UPHY2_RN_REG->gctrl[2] = 0x88;
	_delay_1ms(1);
	UPHY2_RN_REG->gctrl[2] = 0x80;
	_delay_1ms(20);
	UPHY2_RN_REG->gctrl[2] = 0x0;

	// 5. USBC 0/1 reset
	//MOON0_REG->reset[2] = RF_MASK_V_SET(3 << 10);
	//_delay_1ms(1);
	//MOON0_REG->reset[2] = RF_MASK_V_CLR(3 << 10);
	//_delay_1ms(1);

	// 6. HW workaround
	UPHY2_RN_REG->cfg[19] |= 0x0f;

	// 7. USB DISC (disconnect voltage)
	UPHY2_RN_REG->cfg[7] = 0x8b;

	// 8. RX SQUELCH LEVEL
	UPHY2_RN_REG->cfg[25] = 0x4;
	
	//U3 phy settings
	UPHY3_U3_REG->cfg[0]   = 0x43;
	UPHY3_U3_REG->cfg[11]  = 0x21;
	UPHY3_U3_REG->cfg[13]  = 0x5;
	UPHY3_U3_REG->cfg[17]  = 0x1f;
	UPHY3_U3_REG->cfg[18]  = 0x0;
	UPHY3_U3_REG->cfg[95]  = 0x33;
	UPHY3_U3_REG->cfg[112] = 0x11;
	UPHY3_U3_REG->cfg[113] = 0x0;
	UPHY3_U3_REG->cfg[114] = 0x1;
	UPHY3_U3_REG->cfg[115] = 0x0;
	UPHY3_U3_REG->cfg[128] = 0x9;		
}
#endif

void usb_power_init(void)
{
	// a. enable pin mux control
	//    Host: enable
	//    Device: disable
#ifdef PLATFORM_I143
	/* I143 USBC0_OTG_EN_SEL USBC1_OTG_EN_SEL */
	MOON0_REG->clken[2] = RF_MASK_V_SET(1 << 12);
	
	// b. USB control register:
	/* I143 USBC0_TYPE, USBC0_SEL, USBC1_TYPE, USBC1_SEL */
	MOON0_REG->reset[2] = RF_MASK_V_SET(1 << 12);
	_delay_1ms(1);
	MOON0_REG->reset[2] = RF_MASK_V_CLR(1 << 12);
#endif
}

void handshake(volatile u32 *ptr, u32 mask, u32 done, int usec)
{
	u32 result;
	
	do {
		result = *ptr;
		if (result == ~(u32)0) {
			prn_string("\n		ERR 0 ");
			break;
		}
		result &= mask;
		if (result == done)
			break;
		usec--;
		_delay_1ms(1);
	} while (usec > 0);
	if (usec <= 0)
		prn_string("\n  	ERR 1 ");
}

u32 xhci_td_remainder(unsigned int remainder)
{
	u32 max = (1 << (21 - 17 + 1)) - 1;

	if ((remainder >> 10) >= max)
		return max << 17;
	else
		return (remainder >> 10) << 17;
}

struct xhci_ring *xhci_ring_alloc(int link_trbs, int no)
{
	struct xhci_ring *tmpring;
	u32 ringtmp;
	u64 ringtmp_64;
	
	prn_string("\n*ring setting no "); prn_dword(no);
	if (no == 0x10) {
		//memset32(g_io_buf.usb.xhci.pcmdtrb, 0, sizeof(g_io_buf.usb.xhci.pcmdtrb));
		tmpring = &g_io_buf.usb.xhci.pcmd_ring;
		tmpring->first_seg = &g_io_buf.usb.xhci.pcmd_ring_first_seg;
		tmpring->first_seg->trbs = g_io_buf.usb.xhci.pcmdtrb;
		tmpring->first_seg->next = NULL;
		tmpring->first_seg->next = tmpring->first_seg;
	} else if (no == 0x11) {
		tmpring = &g_io_buf.usb.xhci.pevent_ring;
		tmpring->first_seg = &g_io_buf.usb.xhci.pevent_ring_first_seg;
		tmpring->first_seg->trbs = g_io_buf.usb.xhci.peventtrb;
		tmpring->first_seg->next = NULL;
		tmpring->first_seg->next = tmpring->first_seg;
	} else if (no == 0) {
		tmpring = &g_io_buf.usb.xhci.pep0_ring;
		tmpring->first_seg = &g_io_buf.usb.xhci.pep0_ring_first_seg;
		tmpring->first_seg->trbs = g_io_buf.usb.xhci.pep0trb;
		tmpring->first_seg->next = NULL;
		tmpring->first_seg->next = tmpring->first_seg;
	} else if ((no <= 2)) {
		tmpring = &g_io_buf.usb.xhci.pep1_ring;
		tmpring->first_seg = &g_io_buf.usb.xhci.pep1_ring_first_seg;
		tmpring->first_seg->trbs = g_io_buf.usb.xhci.pep1trb;
		tmpring->first_seg->next = NULL;
		tmpring->first_seg->next = tmpring->first_seg;
	} else if ((no <= 4)) {
		tmpring = &g_io_buf.usb.xhci.pep2_ring;
		tmpring->first_seg = &g_io_buf.usb.xhci.pep2_ring_first_seg;
		tmpring->first_seg->trbs = g_io_buf.usb.xhci.pep2trb;
		tmpring->first_seg->next = NULL;
		tmpring->first_seg->next = tmpring->first_seg;
	} else if (no <= 6) {
		tmpring = &g_io_buf.usb.xhci.pep3_ring;
		tmpring->first_seg = &g_io_buf.usb.xhci.pep3_ring_first_seg;
		tmpring->first_seg->trbs = g_io_buf.usb.xhci.pep3trb;
		tmpring->first_seg->next = NULL;
		tmpring->first_seg->next = tmpring->first_seg;
	} else {
		prn_string("\n!!!!!ring no overflow!!!!! ");
		return NULL;
	}
	// link = true
	if (link_trbs) {
		ringtmp_64 = (intptr_t) tmpring->first_seg->trbs;
		tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.segment_ptr = ringtmp_64;
		//prn_string("\n  *ringtmp_64 "); prn_dword(ringtmp_64);
	
		/*
	 	 * Set the last TRB in the segment to
	 	 * have a TRB type ID of Link TRB
		*/
		ringtmp = tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control;
		ringtmp &= ~TRB_TYPE_BITMASK;
		ringtmp |= (TRB_LINK << TRB_TYPE_SHIFT);
		tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control = ringtmp;	
		tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control |= LINK_TOGGLE;
	}
	//xhci_initialize_ring_info
	tmpring->enqueue = tmpring->first_seg->trbs;
	tmpring->enq_seg = tmpring->first_seg;
	tmpring->dequeue = tmpring->enqueue;
	tmpring->deq_seg = tmpring->first_seg;
	tmpring->cycle_state = 1;
	
	prn_string("  	*tmpring->first_seg->trbs[0].link.control "); prn_dword(tmpring->first_seg->trbs[0].link.control);
	prn_string("  	*tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control "); prn_dword(tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control);
	prn_string("  	*tmpring.enqueue "); prn_dword((intptr_t)tmpring->enqueue);
	prn_string("  	*tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1] "); prn_dword((intptr_t)&tmpring->first_seg->trbs[TRBS_PER_SEGMENT-1]);
	return tmpring;
}

void giveback_first_trb(int ep_index, int start_cycle, struct xhci_generic_trb *start_trb)
{
	/*
	 * Pass all the TRBs to the hardware at once and make sure this write
	 * isn't reordered.
	 */
	if (start_cycle)
		start_trb->field[3] |= start_cycle;
	else
		start_trb->field[3] &= ~TRB_CYCLE;

	/* Ringing EP doorbell here */
	g_io_buf.usb.xhci.dba->doorbell[g_io_buf.usb.xhci.udev.slot_id] = DB_VALUE(ep_index, 0);
	//xhci_writel(&ctrl->dba->doorbell[udev->slot_id], DB_VALUE(ep_index, 0));

}

int last_trb_on_last_seg(struct xhci_ring *ring, struct xhci_segment *seg, union xhci_trb *trb)
{
	//prn_string("\n  		***last_trb_on_last_seg ring seg trb\n"); 
	//prn_string("  		"); prn_dword(ring); 
	//prn_string("  		"); prn_dword(seg); 
	//prn_string("  		"); prn_dword(trb);
	if (ring == g_io_buf.usb.xhci.event_ring)
		return ((trb == &seg->trbs[TRBS_PER_SEGMENT]) &&
			(seg->next == ring->first_seg));
	else
		return trb->link.control & LINK_TOGGLE;
}

int last_trb(struct xhci_ring *ring, struct xhci_segment *seg, union xhci_trb *trb)
{
	//prn_string("\n		**last_trb ring, seg, trb\n");
	//prn_string("		"); prn_dword(ring); 
	//prn_string("		"); prn_dword(seg); 
	//prn_string("		"); prn_dword(trb);

	if (ring == g_io_buf.usb.xhci.event_ring)
		return trb == &seg->trbs[TRBS_PER_SEGMENT];
	else
		return TRB_TYPE_LINK_LE32(trb->link.control);
}

void prepare_ring(struct xhci_ring *ep_ring)
{
	union xhci_trb *next = ep_ring->enqueue;
	
	//prn_string("\n  	*>>>prepare_ring");
	while (last_trb(ep_ring, ep_ring->enq_seg, next)) {
		next->link.control &= ~TRB_CHAIN;

		next->link.control ^= TRB_CYCLE;
		
		if (last_trb_on_last_seg(ep_ring, ep_ring->enq_seg, next))
			ep_ring->cycle_state = (ep_ring->cycle_state ? 0 : 1);
		ep_ring->enq_seg = ep_ring->enq_seg->next;
		ep_ring->enqueue = ep_ring->enq_seg->trbs;
		next = ep_ring->enqueue;
	}
	//prn_string("\n  	prepare_ring end<<<*");
}

void inc_enq(struct xhci_ring *ring, int more_trbs_coming)
{
	u32 chain;
	union xhci_trb *next;
        
        //if (ring == g_io_buf.usb.xhci.cmd_ring)
        //{
        	//prn_string("\n  generic.field[0] "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue->generic.field[0]);
        	//prn_string("\n  generic.field[1] "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue->generic.field[1]);
        	//prn_string("\n  generic.field[2] "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue->generic.field[2]);
        	//prn_string("\n  generic.field[3] "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue->generic.field[3]);
        //}
        //else if (ring == g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[0].ring)
        //{
        	//prn_string("\n  *epgeneric.field[0][1][2][3]\n"); 
        	//prn_dword(g_io_buf.usb.xhci.devs[1]->eps[0].ring->enqueue->generic.field[0]); 
        	//prn_dword(g_io_buf.usb.xhci.devs[1]->eps[0].ring->enqueue->generic.field[1]); 
        	//prn_dword(g_io_buf.usb.xhci.devs[1]->eps[0].ring->enqueue->generic.field[2]); 
        	//prn_dword(g_io_buf.usb.xhci.devs[1]->eps[0].ring->enqueue->generic.field[3]);
        //}
  	//prn_string("\n  last_trb ring "); prn_dword(ring);
	chain = ring->enqueue->generic.field[3] & TRB_CHAIN;	
	next = ++(ring->enqueue);
	/*
	 * Update the dequeue pointer further if that was a link TRB or we're at
	 * the end of an event ring segment (which doesn't have link TRBS)
	 */
	while (last_trb(ring, ring->enq_seg, next)) {
		if (ring != g_io_buf.usb.xhci.event_ring) {
			//prn_string("\n  @@@cmd ring full ");
			/*
			 * If the caller doesn't plan on enqueueing more
			 * TDs before ringing the doorbell, then we
			 * don't want to give the link TRB to the
			 * hardware just yet.  We'll give the link TRB
			 * back in prepare_ring() just before we enqueue
			 * the TD at the top of the ring.
			 */
			if (!chain && !more_trbs_coming)
				break;

			/*
			 * If we're not dealing with 0.95 hardware or
			 * isoc rings on AMD 0.96 host,
			 * carry over the chain bit of the previous TRB
			 * (which may mean the chain bit is cleared).
			 */
			next->link.control &= ~TRB_CHAIN;
			next->link.control |= chain;

			next->link.control ^= TRB_CYCLE;			
		}
		/* Toggle the cycle bit after the last ring segment. */
		if (last_trb_on_last_seg(ring, ring->enq_seg, next))
		{
			ring->cycle_state = (ring->cycle_state ? 0 : 1);
		}

		ring->enq_seg = ring->enq_seg->next;
		ring->enqueue = ring->enq_seg->trbs;
		next = ring->enqueue;
	}
}

void inc_deq(struct xhci_ring *ring)
{
	do {
		/*
		 * Update the dequeue pointer further if that was a link TRB or
		 * we're at the end of an event ring segment (which doesn't have
		 * link TRBS)
		 */
		if (last_trb(ring, ring->deq_seg, ring->dequeue)) {
			//prn_string("\n  @@@event ring full ");
			if (ring == g_io_buf.usb.xhci.event_ring && last_trb_on_last_seg(ring, ring->deq_seg, ring->dequeue)) {
				ring->cycle_state = (ring->cycle_state ? 0 : 1);
			}
			ring->deq_seg = ring->deq_seg->next;
			ring->dequeue = ring->deq_seg->trbs;
		} else {
			ring->dequeue++;
		}
	} while (last_trb(ring, ring->deq_seg, ring->dequeue));
}

void queue_trb(struct xhci_ring *ring, int more_trbs_coming, unsigned int *trb_fields)
{
	struct xhci_generic_trb *trb;	
        int i;
        
	trb = &ring->enqueue->generic;
	
	for (i = 0; i < 4; i++) {
		trb->field[i] = trb_fields[i];
		//prn_string("\n  	***trb->field[i] "); prn_dword(trb->field[i]);
	}
#if 0
	if (ring == g_io_buf.usb.xhci.cmd_ring)
		prn_string("\n  	***queue CMD trb "); 
	else if (ring == g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[0].ring)
		prn_string("\n  	***queue EP0 trb ");
	else if (ring == g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[1].ring)
		prn_string("\n  	***queue EP1 trb ");
	else if (ring == g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[4].ring)
		prn_string("\n  	***queue EP4 trb ");
	else if (ring == g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->eps[6].ring)
		prn_string("\n  	***queue EP6 trb ");
	//prn_dword(ring->enqueue);
#endif
	inc_enq(ring, more_trbs_coming);
}

void queue_command(u8 *ptr, u32 slot_id, u32 ep_index, trb_type cmd)
{
	//queue command
	u32 trb_fields[4];
	//u32 tmpqueue;
	u64 tmpqueue_64;
	
	prepare_ring(g_io_buf.usb.xhci.cmd_ring);
	tmpqueue_64 = (intptr_t) ptr;
	trb_fields[0] = tmpqueue_64;
	trb_fields[1] = tmpqueue_64 >> 32;
	trb_fields[2] = 0;
	trb_fields[3] = TRB_TYPE(cmd) | SLOT_ID_FOR_TRB(slot_id) | g_io_buf.usb.xhci.cmd_ring->cycle_state;	
	//prn_string("\n  	*trb_fields[0][1][2][3]\n"); 
	//prn_string("  	"); prn_dword(trb_fields[0]); prn_string("  	"); prn_dword(trb_fields[1]);
	//prn_string("  	"); prn_dword(trb_fields[2]); prn_string("  	"); prn_dword(trb_fields[3]);
	//prn_string("\n  	***queue command "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue);
	queue_trb(g_io_buf.usb.xhci.cmd_ring, 0, trb_fields);
			
	//prn_string("\n  	***queue command inc "); prn_dword(g_io_buf.usb.xhci.cmd_ring->enqueue);
	//
	g_io_buf.usb.xhci.dba->doorbell[0] = DB_VALUE_HOST;
	
}
//xhci_acknowledge_event
void acknowledge_event(void)
{
	u64 tmpack;
	
	inc_deq(g_io_buf.usb.xhci.event_ring);	
	//
	tmpack = ((intptr_t) (g_io_buf.usb.xhci.event_ring->dequeue) | ERST_EHB);
	g_io_buf.usb.xhci.ir_set->erst_dequeue = tmpack;
}

union xhci_trb *wait_for_event(trb_type expected)
{
	trb_type type;
	
	do {
		union xhci_trb *event = g_io_buf.usb.xhci.event_ring->dequeue;

                //prn_string("\n  	*xhci_wait_for_event "); prn_dword((u64)g_io_buf.usb.xhci.event_ring->dequeue);
		if ((event->event_cmd.flags & TRB_CYCLE) != g_io_buf.usb.xhci.event_ring->cycle_state)
			continue;

		type = TRB_FIELD_TO_TYPE(event->event_cmd.flags);
		//prn_string("\n  	*trbtype "); prn_dword(type);
		if (type == expected)
			return event;
                #if 0
		if (type == TRB_PORT_STATUS)
		/* TODO: remove this once enumeration has been reworked */
			/*
			 * Port status change events always have a
			 * successful completion code
			 */
			BUG_ON(GET_COMP_CODE(
				le32_to_cpu(event->generic.field[2])) !=
								COMP_SUCCESS);
		else
			printf("Unexpected XHCI event TRB, skipping... "
				"(%08x %08x %08x %08x)\n",
				le32_to_cpu(event->generic.field[0]),
				le32_to_cpu(event->generic.field[1]),
				le32_to_cpu(event->generic.field[2]),
				le32_to_cpu(event->generic.field[3]));
                #endif
                
		//xhci_acknowledge_event(ctrl);
		acknowledge_event();
	} while (1);
}

//xhci_get_slot_ctx
struct xhci_slot_ctx *get_slot_ctx(xhci_usb *ctrl, struct xhci_container_ctx *ctx) 
{
	if (ctx->type == XHCI_CTX_TYPE_DEVICE)
		return (struct xhci_slot_ctx *)ctx->bytes;

	return (struct xhci_slot_ctx *) (ctx->bytes + CTX_SIZE(ctrl->hccr->cr_hccparams));
}
//xhci_get_ep_ctx
struct xhci_ep_ctx *get_ep_ctx(xhci_usb *ctrl, struct xhci_container_ctx *ctx, unsigned int ep_index)
{
	/* increment ep index by offset of start of ep ctx array */
	//prn_string("\n  *epctx->type "); prn_dword(ctx->type);
	ep_index++;
	if (ctx->type == XHCI_CTX_TYPE_INPUT)
		ep_index++;
	//prn_string("\n  *ep&ctrl->hccr->cr_hccparams "); prn_dword(&ctrl->hccr->cr_hccparams);
        //prn_string("\n  *epCTX_SIZE "); prn_dword(ctrl->hccr->cr_hccparams);
	return (struct xhci_ep_ctx *) (ctx->bytes + (ep_index * CTX_SIZE(ctrl->hccr->cr_hccparams)));
}
//usb_set_address
void set_address(int slot_id)
{	
	struct xhci_input_control_ctx *ctrl_ctx;
        struct xhci_virt_device *virt_dev;
	// xhci_setup_addressable_virt_dev
	struct xhci_ep_ctx *ep0_ctx;
	struct xhci_slot_ctx *slot_ctx;
	union xhci_trb *pevent;
	u64 tmp_64;
	
	virt_dev = g_io_buf.usb.xhci.devs[slot_id];
	//prn_string("\n  virt_dev "); prn_dword(virt_dev);
//xhci_setup_addressable_virt_dev
	//xhci_get_ep_ctx
	ep0_ctx = get_ep_ctx(&g_io_buf.usb.xhci, virt_dev->in_ctx, 0);
	//ep0_ctx = virt_dev->in_ctx->bytes + 2 * CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams);
	//prn_string("\n  	ep0_ctx "); prn_dword(ep0_ctx);
	// xhci_get_slot_ctx
	slot_ctx = get_slot_ctx(&g_io_buf.usb.xhci, virt_dev->in_ctx);
	//slot_ctx = virt_dev->in_ctx->bytes + CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams);
	//prn_string("\n  	slot_ctx "); prn_dword(slot_ctx);
	
	slot_ctx->dev_info |= LAST_CTX(1);
	//slot_ctx->dev_info |= route;
	slot_ctx->dev_info2 |= (g_io_buf.usb.xhci.udev.portnr & ROOT_HUB_PORT_MASK) << ROOT_HUB_PORT_SHIFT;//(port_num & ROOT_HUB_PORT_MASK) << ROOT_HUB_PORT_SHIFT;
	ep0_ctx->ep_info2 = CTRL_EP << EP_TYPE_SHIFT;
	if (g_io_buf.usb.xhci.udev.speed == USB_SPEED_SUPER) {
		slot_ctx->dev_info |= SLOT_SPEED_SS;
		prn_string("\n  	slot_ctx->dev_info "); prn_dword(slot_ctx->dev_info);		
		ep0_ctx->ep_info2 |= ((512 & MAX_PACKET_MASK) << MAX_PACKET_SHIFT);
	} else if (g_io_buf.usb.xhci.udev.speed == USB_SPEED_HIGH) {
		slot_ctx->dev_info |= SLOT_SPEED_HS;
		prn_string("\n  	slot_ctx->dev_info "); prn_dword(slot_ctx->dev_info);
		ep0_ctx->ep_info2 |= ((64 & MAX_PACKET_MASK) << MAX_PACKET_SHIFT);
	} else {
		prn_string("\n  	unknown speed "); prn_dword(g_io_buf.usb.xhci.udev.speed);
	}
			
	ep0_ctx->ep_info2 |= ((0 & MAX_BURST_MASK) << MAX_BURST_SHIFT) | ((3 & ERROR_COUNT_MASK) << ERROR_COUNT_SHIFT);
	
	tmp_64 = (intptr_t)virt_dev->eps[0].ring->first_seg->trbs;
	ep0_ctx->deq = tmp_64 | virt_dev->eps[0].ring->cycle_state;
	prn_string("  	ep0_ctx->deq "); prn_dword(ep0_ctx->deq);
	
	ep0_ctx->tx_info = EP_AVG_TRB_LENGTH(8);
//xhci_get_input_control_ctx
	ctrl_ctx = (struct xhci_input_control_ctx *) virt_dev->in_ctx->bytes;
	//prn_string("\n  ctrl_ctx "); prn_dword(ctrl_ctx);
	ctrl_ctx->add_flags = SLOT_FLAG | EP0_FLAG;
	ctrl_ctx->drop_flags = 0;
	
	queue_command((void *)ctrl_ctx, slot_id, 0, TRB_ADDR_DEV);	
	//even ring
	//trb_type type;
	pevent = wait_for_event(TRB_COMPLETION);
	//slot_id = TRB_TO_SLOT_ID(event->event_cmd.flags);
	//tmp2 = GET_COMP_CODE(event->event_cmd.status);
	if (GET_COMP_CODE(pevent->event_cmd.status) != 1) {
		prn_string("\n!!!!!set_address GET_COMP_CODE!!!!! "); prn_dword(GET_COMP_CODE(pevent->event_cmd.status));
	}

	acknowledge_event();
//xhci_get_slot_ctx
	_delay_1ms(20);	
	slot_ctx = get_slot_ctx(&g_io_buf.usb.xhci, virt_dev->out_ctx);
	prn_string("  xHCI internal address is: "); prn_dword(slot_ctx->dev_state & DEV_ADDR_MASK);
	//slot_ctx = (struct xhci_slot_ctx *)virt_dev->out_ctx->bytes;
	
}

void slot_copy(struct xhci_container_ctx *in_ctx, struct xhci_container_ctx *out_ctx)
{
	struct xhci_slot_ctx *in_slot_ctx;
	struct xhci_slot_ctx *out_slot_ctx;

	in_slot_ctx = get_slot_ctx(&g_io_buf.usb.xhci, in_ctx);
	out_slot_ctx = get_slot_ctx(&g_io_buf.usb.xhci, out_ctx);

	in_slot_ctx->dev_info = out_slot_ctx->dev_info;
	in_slot_ctx->dev_info2 = out_slot_ctx->dev_info2;
	in_slot_ctx->tt_info = out_slot_ctx->tt_info;
	in_slot_ctx->dev_state = out_slot_ctx->dev_state;
}
//xhci_endpoint_copy
void endpoint_copy(struct xhci_container_ctx *in_ctx, struct xhci_container_ctx *out_ctx, unsigned int ep_index)
{
	struct xhci_ep_ctx *out_ep_ctx;
	struct xhci_ep_ctx *in_ep_ctx;

	out_ep_ctx = get_ep_ctx(&g_io_buf.usb.xhci, out_ctx, ep_index);
	in_ep_ctx = get_ep_ctx(&g_io_buf.usb.xhci, in_ctx, ep_index);

	in_ep_ctx->ep_info = out_ep_ctx->ep_info;
	in_ep_ctx->ep_info2 = out_ep_ctx->ep_info2;
	in_ep_ctx->deq = out_ep_ctx->deq;
	in_ep_ctx->tx_info = out_ep_ctx->tx_info;
}

//usb_parse_config
int usb_parse_config(unsigned char *buffer, int cfgno)
{
	struct usb_descriptor_header *head;
	int index, ifno, epno, curr_if_num;
	//u16 ep_wMaxPacketSize;
	struct usb_interface *if_desc = NULL;

	ifno = -1;
	epno = -1;
	curr_if_num = -1;

	//dev->configno = cfgno;
	head = (struct usb_descriptor_header *) &buffer[0];
	
	memcpy((u8 *)&g_io_buf.usb.xhci.udev.config, (u8 *)head, USB_DT_CONFIG_SIZE);
	g_io_buf.usb.xhci.udev.config.no_of_if = 0;

	index = g_io_buf.usb.xhci.udev.config.desc.bLength;
	prn_string("\n  	config.desc.bLength "); prn_dword(g_io_buf.usb.xhci.udev.config.desc.bLength);
	/* Ok the first entry must be a configuration entry,
	 * now process the others */
	head = (struct usb_descriptor_header *) &buffer[index];
	while (index + 1 < g_io_buf.usb.xhci.udev.config.desc.wTotalLength && head->bLength) {
		prn_string("  	config.desc.bLength "); prn_dword(head->bDescriptorType);
		prn_string("  	index "); prn_dword(index);
		
		switch (head->bDescriptorType) {
		case USB_DT_INTERFACE:
			if (head->bLength != USB_DT_INTERFACE_SIZE) {
				prn_string("\n!!!!!ERROR: Invalid USB IF length!!!!!");
				break;
			}
			if (index + USB_DT_INTERFACE_SIZE > g_io_buf.usb.xhci.udev.config.desc.wTotalLength) {
				prn_string("\n!!!!!USB IF descriptor overflowed buffer!!!!!");
				break;
			}
			if (((struct usb_interface_descriptor *) head)->bInterfaceNumber != curr_if_num) {
				/* this is a new interface, copy new desc */
				ifno = g_io_buf.usb.xhci.udev.config.no_of_if;
				if (ifno >= USB_MAXINTERFACES) {
					prn_string("\n!!!!!Too many USB interfaces!!!!!");
					break;
				}
				if_desc = &g_io_buf.usb.xhci.udev.config.if_desc[ifno];
				g_io_buf.usb.xhci.udev.config.no_of_if++;
				memcpy((u8 *)if_desc, (u8 *)head, USB_DT_INTERFACE_SIZE);
				if_desc->no_of_ep = 0;
				if_desc->num_altsetting = 1;
				curr_if_num = if_desc->desc.bInterfaceNumber;
			} else {
				/* found alternate setting for the interface */
				if (ifno >= 0) {
					if_desc = &g_io_buf.usb.xhci.udev.config.if_desc[ifno];
					if_desc->num_altsetting++;
				}
			}
			break;
		case USB_DT_ENDPOINT:
			if (head->bLength != USB_DT_ENDPOINT_SIZE &&
			    head->bLength != USB_DT_ENDPOINT_AUDIO_SIZE) {
			    	prn_string("\n!!!!!ERROR: Invalid USB EP length!!!!!");
				break;
			}
			if (index + head->bLength > g_io_buf.usb.xhci.udev.config.desc.wTotalLength) {
				prn_string("\n!!!!!USB EP descriptor overflowed buffer!!!!!");
				break;
			}
			if (ifno < 0) {
				prn_string("\n!!!!!Endpoint descriptor out of order!!!!!");
				break;
			}
			epno = g_io_buf.usb.xhci.udev.config.if_desc[ifno].no_of_ep;
			if_desc = &g_io_buf.usb.xhci.udev.config.if_desc[ifno];
			if (epno >= USB_MAXENDPOINTS) {
				prn_string("\n!!!!!Interface has too many endpoints!!!!!");
				break;
			}
			/* found an endpoint */
			if_desc->no_of_ep++;
			memcpy((u8 *)&if_desc->ep_desc[epno], (u8 *)head, USB_DT_ENDPOINT_SIZE);
			#if 0
			ep_wMaxPacketSize = get_unaligned(&dev->config.\
							if_desc[ifno].\
							ep_desc[epno].\
							wMaxPacketSize);
			put_unaligned(le16_to_cpu(ep_wMaxPacketSize),
					&dev->config.\
					if_desc[ifno].\
					ep_desc[epno].\
					wMaxPacketSize);
			#endif
			prn_string("  	ifno "); prn_dword(ifno);
			prn_string("  	epno "); prn_dword(epno);
			break;
		case USB_DT_SS_ENDPOINT_COMP:
			if (head->bLength != USB_DT_SS_EP_COMP_SIZE) {
				prn_string("\n!!!!!ERROR: Invalid USB EPC length!!!!!");
				break;
			}
			if (index + USB_DT_SS_EP_COMP_SIZE > g_io_buf.usb.xhci.udev.config.desc.wTotalLength) {
				prn_string("\n!!!!!USB EPC descriptor overflowed buffer!!!!!");
				break;
			}
			if (ifno < 0 || epno < 0) {
				prn_string("\n!!!!!EPC descriptor out of order!!!!!");
				break;
			}
			if_desc = &g_io_buf.usb.xhci.udev.config.if_desc[ifno];
			memcpy((u8 *)&if_desc->ss_ep_comp_desc[epno], (u8 *)head, USB_DT_SS_EP_COMP_SIZE);
			break;
		default:
			if (head->bLength == 0) {
				prn_string("\n!!!!!head->bLength = 0!!!!!");
				break;
			}
                        prn_string("\n!!!!!unknown Description Type!!!!!");

			break;
		}
		index += head->bLength;
		head = (struct usb_descriptor_header *)&buffer[index];
	}
	return 0;
}

//usb_set_maxpacket_ep
void usb_set_maxpacket_ep(usb_device *dev, int if_idx, int ep_idx)
{
	int b;
	struct usb_endpoint_descriptor *ep;
	u16 ep_wMaxPacketSize;

	ep = &dev->config.if_desc[if_idx].ep_desc[ep_idx];

	b = ep->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
	ep_wMaxPacketSize = ep->wMaxPacketSize;

	if ((ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_CONTROL) {
		/* Control => bidirectional */
		dev->epmaxpacketout[b] = ep_wMaxPacketSize;
		dev->epmaxpacketin[b] = ep_wMaxPacketSize;
		prn_string("\n  	no "); prn_dword(b);
		prn_string("  	##Control EP epmaxpacketout/in "); prn_dword(dev->epmaxpacketout[b]);
		prn_string("  	##Control EP epmaxpacketin "); prn_dword(dev->epmaxpacketin[b]);
	} else {
		if ((ep->bEndpointAddress & 0x80) == 0) {
			/* OUT Endpoint */
			if (ep_wMaxPacketSize > dev->epmaxpacketout[b]) {
				dev->epmaxpacketout[b] = ep_wMaxPacketSize;
				prn_string("\n  	no "); prn_dword(b);
				prn_string("  	##Control EP epmaxpacketout "); prn_dword(dev->epmaxpacketout[b]);

			}
		} else {
			/* IN Endpoint */
			if (ep_wMaxPacketSize > dev->epmaxpacketin[b]) {
				dev->epmaxpacketin[b] = ep_wMaxPacketSize;
				prn_string("\n  	no "); prn_dword(b);
				prn_string("  	##Control EP epmaxpacketin "); prn_dword(dev->epmaxpacketin[b]);
			}
		} /* if out */
	} /* if control */
}

//usb_set_maxpacket
int usb_set_maxpacket(usb_device *dev)
{
	int i, ii;
	
	prn_string("\n  	config.desc.bNumInterfaces "); prn_dword(dev->config.desc.bNumInterfaces);
	
	for (i = 0; i < g_io_buf.usb.xhci.udev.config.desc.bNumInterfaces; i++) {
		prn_string("\n  	config.if_desc[i].desc.bNumEndpoints "); prn_dword(dev->config.if_desc[i].desc.bNumEndpoints);
		for (ii = 0; ii < dev->config.if_desc[i].desc.bNumEndpoints; ii++)
			usb_set_maxpacket_ep(dev, i, ii);
	}

	return 0;
}

int configure_endpoints(int slot_id, int ctx_change)
{
	struct xhci_container_ctx *in_ctx;
	struct xhci_virt_device *virt_dev;
	
	union xhci_trb *pevent;

	virt_dev = g_io_buf.usb.xhci.devs[slot_id];
	in_ctx = virt_dev->in_ctx;

	queue_command(in_ctx->bytes, slot_id, 0, ctx_change ? TRB_EVAL_CONTEXT : TRB_CONFIG_EP);
	pevent = wait_for_event(TRB_COMPLETION);

	prn_string("\n  	*TRB_TO_SLOT_ID "); prn_dword(TRB_TO_SLOT_ID(pevent->event_cmd.flags));
	if (GET_COMP_CODE(pevent->event_cmd.status) != 1) {
		prn_string("\n!!!!!GET_COMP_CODE!!!!! "); prn_dword(GET_COMP_CODE(pevent->event_cmd.status));
	}

	//switch (GET_COMP_CODE(le32_to_cpu(event->event_cmd.status))) {
	//case COMP_SUCCESS:
	//	debug("Successful %s command\n",
	//		ctx_change ? "Evaluate Context" : "Configure Endpoint");
	//	break;
	//default:
	//	printf("ERROR: %s command returned completion code %d.\n",
	//		ctx_change ? "Evaluate Context" : "Configure Endpoint",
	//		GET_COMP_CODE(le32_to_cpu(event->event_cmd.status)));
	//	return -EINVAL;
	//}

	acknowledge_event();

	return 0;
}

int usb_endpoint_dir_in(struct usb_endpoint_descriptor *epd)
{
	return ((epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN);
}

int usb_endpoint_num(struct usb_endpoint_descriptor *epd)
{
	return epd->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
}

int usb_endpoint_xfer_control(struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_CONTROL);
}

int usb_endpoint_xfer_int(struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_INT);
}

int usb_endpoint_xfer_isoc(struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_ISOC);
}

unsigned int xhci_get_ep_index(struct usb_endpoint_descriptor *desc)
{
	unsigned int index;

	if (usb_endpoint_xfer_control(desc))
		index = (unsigned int)(usb_endpoint_num(desc) * 2);
	else
		index = (unsigned int)((usb_endpoint_num(desc) * 2) -
				(usb_endpoint_dir_in(desc) ? 0 : 1));

	return index;
}

int usb_endpoint_xfer_bulk(struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_BULK);
}

int usb_endpoint_maxp(struct usb_endpoint_descriptor *epd)
{
	return epd->wMaxPacketSize;
}

int usb_endpoint_maxp_mult(struct usb_endpoint_descriptor *epd)
{
	int maxp = epd->wMaxPacketSize;

	return USB_EP_MAXP_MULT(maxp) + 1;
}

u32 xhci_get_max_esit_payload(usb_device *udev,
	struct usb_endpoint_descriptor *endpt_desc,
	struct usb_ss_ep_comp_descriptor *ss_ep_comp_desc)
{
	int max_burst;
	int max_packet;

	/* Only applies for interrupt or isochronous endpoints */
	if (usb_endpoint_xfer_control(endpt_desc) ||
	    usb_endpoint_xfer_bulk(endpt_desc))
		return 0;

	/* SuperSpeed Isoc ep with less than 48k per esit */
	if (udev->speed >= USB_SPEED_SUPER)
		return ss_ep_comp_desc->wBytesPerInterval;

	max_packet = usb_endpoint_maxp(endpt_desc);
	max_burst = usb_endpoint_maxp_mult(endpt_desc);

	/* A 0 in max burst means 1 transfer per ESIT */
	return max_packet * max_burst;
}

u32 clamp_val(u32 interval, u32 min_v, u32 max_v)
{
	if (interval < min_v)
		return min_v;
	if (interval > max_v)
		return max_v;
	return interval;
}

unsigned int xhci_microframes_to_exponent(unsigned int desc_interval,
					  unsigned int min_exponent,
					  unsigned int max_exponent)
{
	unsigned int interval;
	u32 tmp_interval;
	
	tmp_interval = desc_interval;
	//interval = fls(desc_interval) - 1;
	int r = 32;

	if (!tmp_interval) {
		r = 0;
	} else {
		if (!(tmp_interval & 0xffff0000u)) {
			tmp_interval <<= 16;
			r -= 16;	
		}	
		if (!(tmp_interval & 0xff000000u)) {
			tmp_interval <<= 8;
			r -= 8;
		} 
		if (!(tmp_interval & 0xf0000000u)) {
			tmp_interval <<= 4;
			r -= 4;
		}
		if (!(tmp_interval & 0xc0000000u)) {
			tmp_interval <<= 2;
			r -= 2;
		}
		if (!(tmp_interval & 0x80000000u)) {
			tmp_interval <<= 1;
			r -= 1;
		}
	}
	interval = r - 1;

	interval = clamp_val(interval, min_exponent, max_exponent);
	if ((1 << interval) != desc_interval)
		prn_string("\n!!!!!rounding interval to microframes!!!!!");

	return interval;
}

unsigned int xhci_parse_microframe_interval(usb_device *udev, struct usb_endpoint_descriptor *endpt_desc)
{
	if (endpt_desc->bInterval == 0)
		return 0;

	return xhci_microframes_to_exponent(endpt_desc->bInterval, 0, 15);
}

unsigned int xhci_parse_exponent_interval(usb_device *udev, struct usb_endpoint_descriptor *endpt_desc)
{
	unsigned int interval;

	interval = clamp_val(endpt_desc->bInterval, 1, 16) - 1;
	if (interval != endpt_desc->bInterval - 1)
		prn_string("\n!!!!!ep - rounding interval to sframes!!!!!");

	if (udev->speed == USB_SPEED_FULL) {
		/*
		 * Full speed isoc endpoints specify interval in frames,
		 * not microframes. We are using microframes everywhere,
		 * so adjust accordingly.
		 */
		interval += 3;	/* 1 frame = 2^3 uframes */
	}

	return interval;
}

unsigned int xhci_parse_frame_interval(usb_device *udev, struct usb_endpoint_descriptor *endpt_desc)
{
	return xhci_microframes_to_exponent(endpt_desc->bInterval * 8, 3, 10);
}

unsigned int xhci_get_endpoint_interval(usb_device *udev, struct usb_endpoint_descriptor *endpt_desc)
{
	unsigned int interval = 0;

	switch (udev->speed) {
	case USB_SPEED_HIGH:
		/* Max NAK rate */
		if (usb_endpoint_xfer_control(endpt_desc) || usb_endpoint_xfer_bulk(endpt_desc)) {
			interval = xhci_parse_microframe_interval(udev, endpt_desc);
			break;
		}
		/* Fall through - SS and HS isoc/int have same decoding */

	case USB_SPEED_SUPER:
		if (usb_endpoint_xfer_int(endpt_desc) || usb_endpoint_xfer_isoc(endpt_desc)) {
			interval = xhci_parse_exponent_interval(udev, endpt_desc);
		}
		break;

	case USB_SPEED_FULL:
		if (usb_endpoint_xfer_isoc(endpt_desc)) {
			interval = xhci_parse_exponent_interval(udev, endpt_desc);
			break;
		}
		/*
		 * Fall through for interrupt endpoint interval decoding
		 * since it uses the same rules as low speed interrupt
		 * endpoints.
		 */

	case USB_SPEED_LOW:
		if (usb_endpoint_xfer_int(endpt_desc) || usb_endpoint_xfer_isoc(endpt_desc)) {
			interval = xhci_parse_frame_interval(udev, endpt_desc);
		}
		break;

	default:
		prn_string("\n!!!!!unknown speed!!!!!");
	}

	return interval;
}

u32 xhci_get_endpoint_mult(usb_device *udev,
	struct usb_endpoint_descriptor *endpt_desc,
	struct usb_ss_ep_comp_descriptor *ss_ep_comp_desc)
{
	if (udev->speed < USB_SPEED_SUPER || !usb_endpoint_xfer_isoc(endpt_desc))
		return 0;

	return ss_ep_comp_desc->bmAttributes;
}

u32 xhci_get_endpoint_max_burst(usb_device *udev,
	struct usb_endpoint_descriptor *endpt_desc,
	struct usb_ss_ep_comp_descriptor *ss_ep_comp_desc)
{
	/* Super speed and Plus have max burst in ep companion desc */
	if (udev->speed >= USB_SPEED_SUPER)
		return ss_ep_comp_desc->bMaxBurst;

	if (udev->speed == USB_SPEED_HIGH && (usb_endpoint_xfer_isoc(endpt_desc) || usb_endpoint_xfer_int(endpt_desc)))
		return usb_endpoint_maxp_mult(endpt_desc) - 1;

	return 0;
}

int set_configuration(usb_device *dev, int slot_id)
{
	struct xhci_virt_device *virt_dev;
	struct xhci_container_ctx *in_ctx;
	struct xhci_container_ctx *out_ctx;
	struct xhci_input_control_ctx *ctrl_ctx;
	struct xhci_slot_ctx *slot_ctx;
	struct xhci_ep_ctx *ep_ctx[MAX_EP_CTX_NUM];
	struct usb_interface *ifdesc;
	int ep_index;
	int cur_ep;
	int num_of_ep;
	unsigned int interval;
	unsigned int mult;
	unsigned int max_burst;
	unsigned int avg_trb_len;
	unsigned int err_count = 0;
	unsigned int dir;
	unsigned int ep_type;
	int ep_flag = 0;
	int max_ep_flag = 0;
	u32 max_esit_payload;
	u64 ptrb_64; 
	
	virt_dev = g_io_buf.usb.xhci.devs[slot_id];
	out_ctx = virt_dev->out_ctx;
	in_ctx = virt_dev->in_ctx;
	
	num_of_ep = dev->config.if_desc[0].no_of_ep;
	ifdesc = &dev->config.if_desc[0];
	
	ctrl_ctx = (struct xhci_input_control_ctx *) in_ctx->bytes;//xhci_get_input_control_ctx(in_ctx);
	/* Initialize the input context control */
	ctrl_ctx->add_flags = SLOT_FLAG;
	ctrl_ctx->drop_flags = 0;
	
	/* EP_FLAG gives values 1 & 4 for EP1OUT and EP2IN */
	for (cur_ep = 0; cur_ep < num_of_ep; cur_ep++) { 
		ep_flag = xhci_get_ep_index(&ifdesc->ep_desc[cur_ep]);
		ctrl_ctx->add_flags |= (1 << (ep_flag + 1));
		prn_string("\n  	cur_ep "); prn_dword(cur_ep);
		prn_string("  	ep_flag "); prn_dword(ep_flag);
		if (max_ep_flag < ep_flag)
			max_ep_flag = ep_flag;
	}
	
	slot_copy(in_ctx, out_ctx);
	slot_ctx = get_slot_ctx(&g_io_buf.usb.xhci, in_ctx);
	slot_ctx->dev_info &= ~(LAST_CTX_MASK);
	slot_ctx->dev_info |= LAST_CTX(max_ep_flag + 1) | 0;
        
	endpoint_copy(in_ctx, out_ctx, 0);
	
	/* filling up ep contexts */
	#if 0 //hard code
	ep_index = 1;
	ep_ctx[ep_index] = get_ep_ctx(&g_io_buf.usb.xhci, in_ctx, ep_index);
	virt_dev->eps[ep_index].ring = xhci_ring_alloc(1, ep_index);
	ep_ctx[ep_index]->ep_info = 0;
	ep_ctx[ep_index]->ep_info2 = 0x2000016;
	ptrb_64 = (u64)virt_dev->eps[ep_index].ring->enqueue;
	ep_ctx[ep_index]->deq = ptrb_64 | virt_dev->eps[ep_index].ring->cycle_state;
	ep_ctx[ep_index]->tx_info = 0;
	
	ep_index = 4;
	ep_ctx[ep_index] = get_ep_ctx(&g_io_buf.usb.xhci, in_ctx, ep_index);
	virt_dev->eps[ep_index].ring = xhci_ring_alloc(1, ep_index);
	ep_ctx[ep_index]->ep_info = 0;
	ep_ctx[ep_index]->ep_info2 = 0x2000036;
	ptrb_64 = (u64)virt_dev->eps[ep_index].ring->enqueue;
	ep_ctx[ep_index]->deq = ptrb_64 | virt_dev->eps[ep_index].ring->cycle_state;
	ep_ctx[ep_index]->tx_info = 0;
	
	ep_index = 6;
	ep_ctx[ep_index] = get_ep_ctx(&g_io_buf.usb.xhci, in_ctx, ep_index);
	virt_dev->eps[ep_index].ring = xhci_ring_alloc(1, ep_index);
	ep_ctx[ep_index]->ep_info = 0x70000;
	ep_ctx[ep_index]->ep_info2 = 0x40003e;
	ptrb_64 = (u64)virt_dev->eps[ep_index].ring->enqueue;
	ep_ctx[ep_index]->deq = ptrb_64 | virt_dev->eps[ep_index].ring->cycle_state;
	ep_ctx[ep_index]->tx_info = 0x400040;
	#else
	for (cur_ep = 0; cur_ep < num_of_ep; cur_ep++) {
		struct usb_endpoint_descriptor *endpt_desc = NULL;
		struct usb_ss_ep_comp_descriptor *ss_ep_comp_desc = NULL;

		endpt_desc = &ifdesc->ep_desc[cur_ep];
		ss_ep_comp_desc = &ifdesc->ss_ep_comp_desc[cur_ep];
		ptrb_64 = 0;

		/*
		 * Get values to fill the endpoint context, mostly from ep
		 * descriptor. The average TRB buffer lengt for bulk endpoints
		 * is unclear as we have no clue on scatter gather list entry
		 * size. For Isoc and Int, set it to max available.
		 * See xHCI 1.1 spec 4.14.1.1 for details.
		 */
		max_esit_payload = xhci_get_max_esit_payload(dev, endpt_desc, ss_ep_comp_desc);
		interval = xhci_get_endpoint_interval(dev, endpt_desc);
		//interval = 0;
		//if (cur_ep == 2)
		//	interval = 7;
		mult = xhci_get_endpoint_mult(dev, endpt_desc, ss_ep_comp_desc);
		max_burst = xhci_get_endpoint_max_burst(dev, endpt_desc, ss_ep_comp_desc);
		avg_trb_len = max_esit_payload;

		ep_index = xhci_get_ep_index(endpt_desc);
		ep_ctx[ep_index] = get_ep_ctx(&g_io_buf.usb.xhci, in_ctx, ep_index);

		/* Allocate the ep rings */
		//virt_dev->eps[ep_index].ring = xhci_ring_alloc(1, true);
		virt_dev->eps[ep_index].ring = xhci_ring_alloc(1, ep_index);	

		/*NOTE: ep_desc[0] actually represents EP1 and so on */
		dir = (((endpt_desc->bEndpointAddress) & (0x80)) >> 7);
		ep_type = (((endpt_desc->bmAttributes) & (0x3)) | (dir << 2));

		ep_ctx[ep_index]->ep_info = EP_MAX_ESIT_PAYLOAD_HI(max_esit_payload) | EP_INTERVAL(interval) | EP_MULT(mult);

		ep_ctx[ep_index]->ep_info2 = ep_type << EP_TYPE_SHIFT;
		ep_ctx[ep_index]->ep_info2 |= MAX_PACKET(endpt_desc->wMaxPacketSize);

		/* Allow 3 retries for everything but isoc, set CErr = 3 */
		if (!usb_endpoint_xfer_isoc(endpt_desc))
			err_count = 3;
		ep_ctx[ep_index]->ep_info2 |= MAX_BURST(max_burst) | ERROR_COUNT(err_count);

		ptrb_64 = (intptr_t) virt_dev->eps[ep_index].ring->enqueue;
		ep_ctx[ep_index]->deq = ptrb_64 | virt_dev->eps[ep_index].ring->cycle_state;

		/*
		 * xHCI spec 6.2.3:
		 * 'Average TRB Length' should be 8 for control endpoints.
		 */
		if (usb_endpoint_xfer_control(endpt_desc))
			avg_trb_len = 8;
		ep_ctx[ep_index]->tx_info = EP_MAX_ESIT_PAYLOAD_LO(max_esit_payload) | EP_AVG_TRB_LENGTH(avg_trb_len);
		prn_string("\n  	ep_index "); prn_dword(ep_index);
		prn_string("  	ep_info "); prn_dword(ep_ctx[ep_index]->ep_info);
		prn_string("  	ep_info2 "); prn_dword(ep_ctx[ep_index]->ep_info2);
		prn_string("  	tx_info "); prn_dword(ep_ctx[ep_index]->tx_info);
		
	}
	#endif
	return configure_endpoints(g_io_buf.usb.xhci.udev.slot_id, 0);
}

void ctrl_tx(struct devrequest *req, int length, void *buffer, u32 pipe)
{
	struct xhci_ring *ep_ring;
	int ep_index;
	//struct xhci_ep_ctx *ep_ctx = NULL;
	struct xhci_generic_trb *start_trb;
	int start_cycle;
	int num_trbs = 2;
	u32 field;
	u32 trb_fields[4];
	u64 buf_64 = 0;
	union xhci_trb *pevent;
	u32 length_field;
	
	ep_index = usb_pipe_ep_index(pipe);
	//prn_string("\n  g_io_buf.usb.xhci.devs[1]->eps[ep_index].ring "); prn_dword(g_io_buf.usb.xhci.devs[1]->eps[ep_index].ring);
	prn_string("\n  	*pipe "); prn_dword(pipe);
	prn_string("  	*ep_index "); prn_dword(ep_index);
	ep_ring = g_io_buf.usb.xhci.devs[1]->eps[ep_index].ring;
	//ep_ctx = g_io_buf.usb.xhci.devs[1]->out_ctx->bytes + CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams);
	if (length > 0) 
		num_trbs++;
	
	prepare_ring(ep_ring);
	start_trb = &ep_ring->enqueue->generic;
	start_cycle = ep_ring->cycle_state;
	//prn_string("\n  	start_trb "); prn_dword(start_trb);
	//prn_string("\n  	start_cycle "); prn_dword(start_cycle);
	
	field = 0;	
	field |= TRB_IDT | (TRB_SETUP << TRB_TYPE_SHIFT);
	if (start_cycle == 0)
		field |= 0x1;
	trb_fields[0] = req->requesttype | req->request << 8 | req->value << 16;
	trb_fields[1] = req->index | req->length << 16;
	/* TRB_LEN | (TRB_INTR_TARGET) */
	trb_fields[2] = (8 | ((0 & TRB_INTR_TARGET_MASK) << TRB_INTR_TARGET_SHIFT));
	/* Immediate data in pointer */
	trb_fields[3] = field;
	queue_trb(ep_ring, 1, trb_fields);
	
	/* Re-initializing field to zero */
	field = 0;
	/* If there's data, queue data TRBs */
	/* Only set interrupt on short packet for IN endpoints */
	if (usb_pipein(pipe))
		field = TRB_ISP | (TRB_DATA << TRB_TYPE_SHIFT);
	else
		field = (TRB_DATA << TRB_TYPE_SHIFT);

	length_field = (length & TRB_LEN_MASK) | xhci_td_remainder(length) |
			((0 & TRB_INTR_TARGET_MASK) << TRB_INTR_TARGET_SHIFT);
			
	if (length > 0) {
		if (req->requesttype & USB_DIR_IN)
			field |= TRB_DIR_IN;
		buf_64 = (intptr_t)buffer;

		trb_fields[0] = buf_64;
		trb_fields[1] = buf_64 >> 32;
		trb_fields[2] = length_field;
		trb_fields[3] = field | ep_ring->cycle_state;

		queue_trb(ep_ring, 1, trb_fields);
	}
	
	/*
	 * Queue status TRB -
	 * see Table 7 and sections 4.11.2.2 and 6.4.1.2.3
	 */

	/* If the device sent data, the status stage is an OUT transfer */
	field = 0;
	if (length > 0 && (req->requesttype & USB_DIR_IN))
		field = 0;
	else
		field = TRB_DIR_IN;

	trb_fields[0] = 0;	
	trb_fields[1] = 0;
	trb_fields[2] = ((0 & TRB_INTR_TARGET_MASK) << TRB_INTR_TARGET_SHIFT);
		/* Event on completion */
	trb_fields[3] = field | TRB_IOC | (TRB_STATUS << TRB_TYPE_SHIFT) | ep_ring->cycle_state;

	queue_trb(ep_ring, 0, trb_fields);
	
	giveback_first_trb(ep_index, start_cycle, start_trb);

	pevent = wait_for_event(TRB_TRANSFER);
	if (!pevent)
		prn_string("\n  XXXwait_for_event err ");
		
	field = EVENT_TRB_LEN(pevent->trans_event.transfer_len);
	if (field)
		g_io_buf.usb.xhci.udev.act_len = length - field;
	else		
		g_io_buf.usb.xhci.udev.act_len = length;

	//prn_string("\n  	EVENT_TRB_LEN "); prn_dword(EVENT_TRB_LEN(pevent->trans_event.transfer_len));
	//prn_string("\n  	g_io_buf.usb.xhci.udev.act_len "); prn_dword(g_io_buf.usb.xhci.udev.act_len);
	//prn_string("\n  buffer L "); prn_dword(pevent->trans_event.buffer);
	//prn_string("\n  buffer H "); prn_dword(pevent->trans_event.buffer >> 32);
	acknowledge_event();
	
	if (GET_COMP_CODE(pevent->trans_event.transfer_len) == COMP_SHORT_TX) {
		/* Short data stage, clear up additional status stage event */
		prn_string("  	GET_COMP_CODE "); prn_dword(GET_COMP_CODE(pevent->trans_event.transfer_len));
		pevent = wait_for_event(TRB_TRANSFER);
		if (!pevent)
			prn_string("\n  XXXwait_for_event err ");
			//goto abort;
		//BUG_ON(TRB_TO_SLOT_ID(field) != slot_id);
		//BUG_ON(TRB_TO_EP_INDEX(field) != ep_index);
		acknowledge_event();
	}
	//return (udev->status != USB_ST_NOT_PROC) ? 0 : -1;
}
//xhci_bulk_tx
int bulk_tx(u32 pipe, int length, void *buffer)
{
	int num_trbs = 0;
	struct xhci_generic_trb *start_trb;
	int first_trb = 0;
	int start_cycle;
	u32 field = 0;
	u32 length_field = 0;
	int ep_index;
	struct xhci_virt_device *virt_dev;
	//struct xhci_ep_ctx *ep_ctx; 
	struct xhci_ring *ring;
	union xhci_trb *pevent;
	
	int running_total, trb_buff_len;
	//unsigned int total_packet_count;
	//int maxpacketsize;
	u64 addr;
	u32 trb_fields[4];
	u64 pval_64 = (intptr_t)buffer;
	
	ep_index = usb_pipe_ep_index(pipe);
	virt_dev = g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id];
	//prn_string("\n		*pipe "); prn_dword(pipe);
	//prn_string("\n		*ep_index "); prn_dword(ep_index);
	//prn_string("\n		*g_io_buf.usb.xhci.udev.slot_id "); prn_dword(g_io_buf.usb.xhci.udev.slot_id);
  
	//ep_ctx = xhci_get_ep_ctx(ctrl, virt_dev->out_ctx, ep_index);
	//ep_ctx = virt_dev->out_ctx->bytes + CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams);

	ring = virt_dev->eps[ep_index].ring;
	/*
	 * How much data is (potentially) left before the 64KB boundary?
	 * XHCI Spec puts restriction( TABLE 49 and 6.4.1 section of XHCI Spec)
	 * that the buffer should not span 64KB boundary. if so
	 * we send request in more than 1 TRB by chaining them.
	 */
	running_total = TRB_MAX_BUFF_SIZE - (((u32)pval_64) & (TRB_MAX_BUFF_SIZE - 1));
	trb_buff_len = running_total;
	running_total &= TRB_MAX_BUFF_SIZE - 1;
	//prn_string("\n		*running_total "); prn_dword(running_total);
	//prn_string("\n		*trb_buff_len "); prn_dword(trb_buff_len);
	//prn_string("\n		*pval_64 H "); prn_dword(pval_64>>32);
	//prn_string("\n		*pval_64 L "); prn_dword(pval_64);
	/*
	 * If there's some data on this 64KB chunk, or we have to send a
	 * zero-length transfer, we need at least one TRB
	 */
	if (running_total != 0 || length == 0)
		num_trbs++;
	/* How many more 64KB chunks to transfer, how many more TRBs? */
	while (running_total < length) {
		num_trbs++;
		running_total += TRB_MAX_BUFF_SIZE;
	}
	/*
	 * XXX: Calling routine prepare_ring() called in place of
	 * prepare_trasfer() as there in 'Linux' since we are not
	 * maintaining multiple TDs/transfer at the same time.
	 */
	prepare_ring(ring);
	
	start_trb = &ring->enqueue->generic;
	start_cycle = ring->cycle_state;
	//prn_string("\n		start_trb "); prn_dword(start_trb);
	//prn_string("\n		start_cycle "); prn_dword(start_cycle);
	running_total = 0;
	
	//maxpacketsize = usb_maxpacket(udev, pipe);
	//maxpacketsize = 0x200;
	//total_packet_count = DIV_ROUND_UP(length, maxpacketsize);
	//total_packet_count = 1;
	/* How much data is in the first TRB? */
	/*
	 * How much data is (potentially) left before the 64KB boundary?
	 * XHCI Spec puts restriction( TABLE 49 and 6.4.1 section of XHCI Spec)
	 * that the buffer should not span 64KB boundary. if so
	 * we send request in more than 1 TRB by chaining them.
	 */
	addr = pval_64;

	if (trb_buff_len > length)
		trb_buff_len = length;

	first_trb = 1;
	/* Queue the first TRB, even if it's zero-length */
	do {
		u32 remainder = 0;
		field = 0;
		/* Don't change the cycle bit of the first TRB until later */
		if (first_trb) {
			first_trb = 0;
			if (start_cycle == 0)
				field |= TRB_CYCLE;
		} else {
			field |= ring->cycle_state;
		}

		/*
		 * Chain all the TRBs together; clear the chain bit in the last
		 * TRB to indicate it's the last TRB in the chain.
		 */
		if (num_trbs > 1)
			field |= TRB_CHAIN;
		else
			field |= TRB_IOC;

		/* Only set interrupt on short packet for IN endpoints */
		if (usb_pipein(pipe))
			field |= TRB_ISP;

		/* Set the TRB length, TD size, and interrupter fields. */
		if (HC_VERSION(g_io_buf.usb.xhci.hccr->cr_capbase) < 0x100)
			remainder = xhci_td_remainder(length - running_total);
		else
			remainder = 0;//xhci_v1_0_td_remainder(running_total, trb_buff_len, total_packet_count, maxpacketsize, num_trbs - 1);

		length_field = ((trb_buff_len & TRB_LEN_MASK) |
				remainder |
				((0 & TRB_INTR_TARGET_MASK) <<
				TRB_INTR_TARGET_SHIFT));

		trb_fields[0] = addr;
		trb_fields[1] = addr >> 32;
		trb_fields[2] = length_field;
		trb_fields[3] = field | (TRB_NORMAL << TRB_TYPE_SHIFT);

		queue_trb(ring, (num_trbs > 1), trb_fields);

		--num_trbs;

		running_total += trb_buff_len;

		/* Calculate length for next transfer */
		addr += trb_buff_len;
		
		//trb_buff_len = min((length - running_total), TRB_MAX_BUFF_SIZE);
		if (TRB_MAX_BUFF_SIZE >= (length - running_total))
			trb_buff_len = length - running_total;
		else
			trb_buff_len = TRB_MAX_BUFF_SIZE;
	} while (running_total < length);

	giveback_first_trb(ep_index, start_cycle, start_trb);

	pevent = wait_for_event(TRB_TRANSFER);
	//if (!event) {
	//	debug("XHCI bulk transfer timed out, aborting...\n");
	//	abort_td(udev, ep_index);
	//	udev->status = USB_ST_NAK_REC;  /* closest thing to a timeout */
	//	udev->act_len = 0;
	//	return -ETIMEDOUT;
	//}
	field = pevent->trans_event.flags;
	
	//BUG_ON(TRB_TO_SLOT_ID(field) != slot_id);
	//BUG_ON(TRB_TO_EP_INDEX(field) != ep_index);
	//BUG_ON(*(void **)(uintptr_t)le64_to_cpu(event->trans_event.buffer) -
	//	buffer > (size_t)length);
        //prn_string("\n		TRB_TO_SLOT_ID "); prn_dword(TRB_TO_SLOT_ID(field));
	//prn_string("\n		TRB_TO_EP_INDEX "); prn_dword(TRB_TO_EP_INDEX(field));
	//record_transfer_result(udev, event, length);
	field = EVENT_TRB_LEN(pevent->trans_event.transfer_len);
	if (field)
		g_io_buf.usb.xhci.udev.act_len = length - field;
	else		
		g_io_buf.usb.xhci.udev.act_len = length;

	if (GET_COMP_CODE(pevent->trans_event.transfer_len) != 0x1) {
		prn_string("\n  !!!!!GET_COMP_CODE!!!!! "); prn_dword(GET_COMP_CODE(pevent->trans_event.transfer_len));
	}

	//prn_string("\n  	EVENT_TRB_LEN "); prn_dword(EVENT_TRB_LEN(pevent->trans_event.transfer_len));
	//prn_string("\n  	g_io_buf.usb.xhci.udev.act_len "); prn_dword(g_io_buf.usb.xhci.udev.act_len);
	//xhci_acknowledge_event(ctrl);
	acknowledge_event();
	
	//return (udev->status != USB_ST_NOT_PROC) ? 0 : -1;
	return field;
}
//usb_string
void usb_string(int j, char *buf, int size)
{
	unsigned int u, idx, err;

	
	if (g_io_buf.usb.xhci.udev.string_langid == 0) {
		prn_string("\n    **<usb_get_string langid>**");
		USB_vendorCmd(USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, 0x300, 0, 0xff);
		g_io_buf.usb.xhci.udev.string_langid = (g_io_buf.usb.cmd_buf[3]<<8)|g_io_buf.usb.cmd_buf[2];
		prn_string("\n    g_io_buf.usb.cmd_buf[0] "); prn_dword(g_io_buf.usb.cmd_buf[0]);
		prn_string("\n    language ID "); prn_dword(g_io_buf.usb.xhci.udev.string_langid);
	}
	
	prn_string("\n    **<usb_get_string>**");
	USB_vendorCmd(USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, 0x300 + j, g_io_buf.usb.xhci.udev.string_langid, 0xff);

	err = 0xff - g_io_buf.usb.xhci.udev.act_len;
	size--;		/* leave room for trailing NULL char in output buffer */
	for (idx = 0, u = 2; u < err; u += 2) {
		if (idx >= size)
			break;
		if (g_io_buf.usb.cmd_buf[u+1])			/* high byte */
			buf[idx++] = '?';  /* non-ASCII character */
		else
			buf[idx++] = g_io_buf.usb.cmd_buf[u];
	}
	buf[idx] = 0;
	err = idx;
}

//usb_inquiry
void usb_inquiry(void)
{
	g_io_buf.usb.xhci.reserved[0] = SCSI_INQUIRY;
	g_io_buf.usb.xhci.reserved[1] = 0;
	g_io_buf.usb.xhci.reserved[4] = 36;
	stor_BBB_transport(36, 12, 1, NULL);
	memset(g_io_buf.usb.xhci.reserved, 0, 12);
}
//usb_test_unit_ready
void usb_test_unit_ready(void)
{
	g_io_buf.usb.xhci.reserved[0] = SCSICMD_TEST_UNIT_READY;
	g_io_buf.usb.xhci.reserved[1] = 0;
	stor_BBB_transport(0, 12, 0, NULL);
	memset(g_io_buf.usb.xhci.reserved, 0, 12);		
}
//usb_read_capacity
void usb_read_capacity(void)
{
	g_io_buf.usb.xhci.reserved[0] = SCSI_RD_CAPAC;
	g_io_buf.usb.xhci.reserved[1] = 0;
	stor_BBB_transport(8, 12, 1, NULL);
	memset(g_io_buf.usb.xhci.reserved, 0, 12);
	//prn_string("\n    g_io_buf.usb.cmd_buf[0] ");prn_dword((g_io_buf.usb.cmd_buf[1]<<8)|g_io_buf.usb.cmd_buf[0]);
	//prn_string("\n    g_io_buf.usb.cmd_buf[2] ");prn_dword((g_io_buf.usb.cmd_buf[3]<<8)|g_io_buf.usb.cmd_buf[2]);
}
//usb_stor_BBB_transport
void stor_BBB_transport(u32 datalen, u32 cmdlen, u8 dir_in, u8 *buf)
{
	//int result;
	sCBW *pcbw;
	sCSW *pcsw;
	int ep_in, ep_out;
	
	if (usb_endpoint_dir_in(&g_io_buf.usb.xhci.udev.config.if_desc[0].ep_desc[0])) {
		ep_out = usb_sndbulkpipe(&g_io_buf.usb.xhci.udev, usb_endpoint_num(&g_io_buf.usb.xhci.udev.config.if_desc[0].ep_desc[1]));//1;
        	ep_in = usb_rcvbulkpipe(&g_io_buf.usb.xhci.udev, usb_endpoint_num(&g_io_buf.usb.xhci.udev.config.if_desc[0].ep_desc[0]));//2;
	} else {
		ep_out = usb_sndbulkpipe(&g_io_buf.usb.xhci.udev, usb_endpoint_num(&g_io_buf.usb.xhci.udev.config.if_desc[0].ep_desc[0]));//1;
        	ep_in = usb_rcvbulkpipe(&g_io_buf.usb.xhci.udev, usb_endpoint_num(&g_io_buf.usb.xhci.udev.config.if_desc[0].ep_desc[1]));//2;
	}
// 1	
	//prn_string("\n  	COMMAND phase==>");
	pcbw = (sCBW*)(&g_io_buf.usb.xhci.reserved[16]);
        pcsw = (sCSW*)(&g_io_buf.usb.xhci.reserved[16]);
        memset(g_io_buf.usb.cmd_buf, 0, 1024);
        //if (g_io_buf.usb.xhci.udev.speed == USB_SPEED_SUPER)
        //	pipe = 0xc0010203;
        //else 
        //	pipe = 0xc0008203;
	pcbw->dCBWSignature = CBWSignature;
	pcbw->dCBWTag = g_io_buf.usb.xhci.udev.CBWTag++;
	pcbw->dCBWDataTransferLength = datalen;//0x24;
	pcbw->bmCBWFlags = dir_in ? CBWFLAGS_IN : CBWFLAGS_OUT;
	pcbw->bCBWLUN = 0;
	pcbw->bCBWCBLength = cmdlen;//0xc;
	/* copy the command data into the CBW command data buffer */
	/* DST SRC LEN!!! */  
	//prn_string("\n  	pcbw->dCBWTag "); prn_dword(pcbw->dCBWTag);             	
	memset(pcbw->CBWCB, 0, sizeof(pcbw->CBWCB));
	memcpy(pcbw->CBWCB, g_io_buf.usb.xhci.reserved, cmdlen);
	//prn_string("\n  	pcbw->CBWCB \n");
	//for (i = 0; i < pcbw->bCBWCBLength; i++) {
	//	prn_string("  	"); prn_dword(pcbw->CBWCB[i]);
	//}
	//result = bulk_tx(ep_out, UMASS_BBB_CBW_SIZE, pcbw);
	bulk_tx(ep_out, UMASS_BBB_CBW_SIZE, pcbw);
	//if (result < 0)
	//	debug("usb_stor_BBB_comdat:usb_bulk_msg error\n");
	//_delay_1ms(20);
//2
	if (datalen > 0) {
		//prn_string("\n  	DATA phase==>");
		//prn_string("\n  	pcbw->dCBWDataTransferLength "); prn_dword(pcbw->dCBWDataTransferLength);
		if (buf == NULL) {
			//prn_string("\n  	buf = NULL ");
			//result = bulk_tx(ep_in, pcbw->dCBWDataTransferLength, g_io_buf.usb.cmd_buf);
			bulk_tx(ep_in, pcbw->dCBWDataTransferLength, g_io_buf.usb.cmd_buf);
		} else {
			//prn_string("\n  	buf != NULL ");
			//result = bulk_tx(ep_in, pcbw->dCBWDataTransferLength, buf);
			bulk_tx(ep_in, pcbw->dCBWDataTransferLength, buf);
		}
		//_delay_1ms(20);
	}
//3
	//prn_string("\n  	STATUS phase==>");
	//result = bulk_tx(ep_in, UMASS_BBB_CSW_SIZE, pcsw);
	bulk_tx(ep_in, UMASS_BBB_CSW_SIZE, pcsw);
	//_delay_1ms(20);
  	//prn_string("\n  	pcsw->dCSWDataResidue "); prn_dword(pcsw->dCSWDataResidue);
  	//prn_string("\n  	pcsw->dCBWSignature "); prn_dword(pcsw->dCBWSignature);
  	//prn_string("\n  	pcsw->dCBWTag "); prn_dword(pcsw->dCBWTag);
  	//prn_string("\n  	pcsw->bCSWStatus "); prn_dword(pcsw->bCSWStatus);
	//prn_string("\n    ISO Vers, Response Data ");prn_string((g_io_buf.usb.cmd_buf[3]<<8)|g_io_buf.usb.cmd_buf[2]);	
		
	//return result;
}

void USB_vendorCmd(u8 bReq, u8 bCmd, u16 wValue, u16 wIndex, u16 wLen)
{
	struct devrequest psetup_packet;
	u32 pipe;
	
	psetup_packet.requesttype = bReq;
	psetup_packet.request = bCmd;
	psetup_packet.value = wValue;
	psetup_packet.index = wIndex;
	psetup_packet.length = wLen;
	
	if (bReq & USB_DIR_IN)
		pipe = usb_rcvctrlpipe(&g_io_buf.usb.xhci.udev, 0);
	else 
		pipe = usb_sndctrlpipe(&g_io_buf.usb.xhci.udev, 0);
	
	if (bCmd == USB_REQ_SET_CONFIGURATION)
		set_configuration(&g_io_buf.usb.xhci.udev, g_io_buf.usb.xhci.udev.slot_id);
	memset(g_io_buf.usb.cmd_buf, 0, 1024);
	ctrl_tx(&psetup_packet, psetup_packet.length, g_io_buf.usb.cmd_buf, pipe);
	if (g_io_buf.usb.xhci.udev.act_len != psetup_packet.length)
		prn_string("\n!!!!!usb_get_configuration_len != length!!!!!");
}

int usb_init(int port, int next_port_in_hub)
{
	UINT32 tmp1, tmp2, tmp3;
        struct dwc3 *dwc3_reg;	
	u64 trb_64, val_64;
	union xhci_trb *event;
	
	CSTAMP(0xE5B00000);
	dbg();

	//memset32((u32 *)&g_io_buf.usb.ehci, 0, sizeof(g_io_buf.usb.ehci)/4);
        memset32((u32 *)&g_io_buf.usb.xhci, 0, sizeof(g_io_buf.usb.xhci)/4);//need to check
        dbg();
#ifndef XHCI_DEBUG
//tryagain:
#endif
	CSTAMP(0xE5B00001);
	uphy_init();	
        prn_string("uphy_init \n");
	CSTAMP(0xE5B00002);
	usb_power_init();
        prn_string("usb_power_init \n"); 
                
	// xhci register base
	//g_io_buf.usb.ehci.ehci_hcd_regs = port ? EHCI1_REG : EHCI0_REG;
	g_io_buf.usb.xhci.hccr = (struct xhci_hccr *) XHCI_REG;
	g_io_buf.usb.xhci.hcor = (struct xhci_hcor *)((char *)g_io_buf.usb.xhci.hccr +
				  HC_LENGTH(g_io_buf.usb.xhci.hccr->cr_capbase));
        dwc3_reg = (struct dwc3 *)((char *)(g_io_buf.usb.xhci.hccr) + DWC3_REG_OFFSET);
        prn_string("\n#hccr "); prn_dword((intptr_t)g_io_buf.usb.xhci.hccr);
        prn_string("#hcor "); prn_dword((intptr_t)g_io_buf.usb.xhci.hcor);
        prn_string("#dwc3_reg "); prn_dword((intptr_t)dwc3_reg);
        prn_string("g_io_buf.usb.xhci.hccr->cr_capbase "); prn_dword((u64)g_io_buf.usb.xhci.hccr->cr_capbase);
	/*Enable ehci test*/
#if 0
	{
		u32 reg_tmp;
		reg_tmp = EHCI_PORTSC;
		reg_tmp &= (7<<16);
		reg_tmp |= (4<<16);
		EHCI_PORTSC = reg_tmp;

		prn_dword(EHCI_PORTSC);
		prn_dword(reg_tmp);
		prn_string("Test mode \n"); 
		while(1);
	}
#endif

	CSTAMP(0xE5B00003);
// init DWC3
	tmp1 = dwc3_reg->g_usb2phycfg[0];
	tmp1 &= ~DWC3_GUSB2PHYCFG_SUSPHY;
	tmp1 |= DWC3_GUSB2PHYCFG_PHYIF;
	tmp1 &= ~DWC3_GUSB2PHYCFG_USBTRDTIM_MASK;
	tmp1 |= DWC3_GUSB2PHYCFG_USBTRDTIM_16BIT;
	dwc3_reg->g_usb2phycfg[0] = tmp1;
	prn_string("dwc3_reg->g_usb2phycfg "); prn_dword(dwc3_reg->g_usb2phycfg[0]);
//dwc3_core_init
	//prn_string("\ndwc3_reg->g_snpsid "); prn_dword(dwc3_reg->g_snpsid);
	prn_string("\ndwc3 reset");
	dwc3_reg->g_ctl |= DWC3_GCTL_CORESOFTRESET; 
	dwc3_reg->g_usb3pipectl[0] |= DWC3_GUSB3PIPECTL_PHYSOFTRST;
	dwc3_reg->g_usb2phycfg[0] |= DWC3_GUSB2PHYCFG_PHYSOFTRST;
	_delay_1ms(100);
	dwc3_reg->g_usb3pipectl[0] &= ~DWC3_GUSB3PIPECTL_PHYSOFTRST;
	dwc3_reg->g_usb2phycfg[0] &= ~DWC3_GUSB2PHYCFG_PHYSOFTRST;
	_delay_1ms(100);
	dwc3_reg->g_ctl &= ~DWC3_GCTL_CORESOFTRESET;
	
	tmp1 = dwc3_reg->g_hwparams1;
	tmp2 = dwc3_reg->g_ctl;
	tmp2 &= ~DWC3_GCTL_SCALEDOWN_MASK;
	tmp2 &= ~DWC3_GCTL_DISSCRAMBLE;
	switch (DWC3_GHWPARAMS1_EN_PWROPT(tmp1)) {
	case DWC3_GHWPARAMS1_EN_PWROPT_CLK:
		tmp2 &= ~DWC3_GCTL_DSBLCLKGTNG;
		break;
	default:
		prn_string("\n	No power optimization available\n");
	}
	dwc3_reg->g_ctl = tmp2;
	//prn_string("\ndwc3 reset end\n");
//set dr mode
	prn_string("\nset dr mode "); prn_dword(dwc3_reg->g_ctl);
	dwc3_reg->g_ctl &= ~DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG);
	dwc3_reg->g_ctl |= DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_HOST);
	prn_string("set dr mode end "); prn_dword(dwc3_reg->g_ctl);
//xhci reset
	prn_string("\nxhci reset");
	tmp1 = g_io_buf.usb.xhci.hcor->or_usbsts & STS_HALT;
	if (!tmp1) {
		tmp1 = g_io_buf.usb.xhci.hcor->or_usbcmd;
		tmp1 &= ~CMD_RUN;
		prn_string("\n		or_usbcmd "); prn_dword(tmp1);
		g_io_buf.usb.xhci.hcor->or_usbcmd = tmp1;
	}
//handshake
	handshake(&g_io_buf.usb.xhci.hcor->or_usbsts, STS_HALT, STS_HALT, XHCI_MAX_HALT_USEC/1000);

	tmp1 = g_io_buf.usb.xhci.hcor->or_usbcmd;
	tmp1 |= CMD_RESET;
	g_io_buf.usb.xhci.hcor->or_usbcmd = tmp1;
//handshake
	handshake(&g_io_buf.usb.xhci.hcor->or_usbcmd, CMD_RESET, 0, XHCI_MAX_HALT_USEC/1000);
	//prn_string("\n  or_usbcmd end "); prn_dword(g_io_buf.usb.xhci.hcor->or_usbcmd);
//handshake
	handshake(&g_io_buf.usb.xhci.hcor->or_usbsts, STS_CNR, 0, XHCI_MAX_HALT_USEC/1000);
	//prn_string("\n  or_usbsts end "); prn_dword(g_io_buf.usb.xhci.hcor->or_usbsts);
	//xhci_lowlevel_init
	/*
	 * Program the Number of Device Slots Enabled field in the CONFIG
	 * register with the max value of slots the HC can handle.
	 */
//xhci_lowlevel_init
	tmp1 = g_io_buf.usb.xhci.hccr->cr_hcsparams1 & HCS_SLOTS_MASK;
	tmp2 = g_io_buf.usb.xhci.hcor->or_config;
	tmp1 |= (tmp2 & ~HCS_SLOTS_MASK);
	g_io_buf.usb.xhci.hcor->or_config = tmp1;
	prn_string("\n  or_config "); prn_dword(g_io_buf.usb.xhci.hcor->or_config);
	//xhci_mem_init	
	g_io_buf.usb.xhci.dcbaa = (struct xhci_device_context_array *) g_io_buf.usb.xhci.dev_context_ptrs;
	g_io_buf.usb.xhci.hcor->or_dcbaap = (intptr_t)g_io_buf.usb.xhci.dev_context_ptrs;
	//prn_string("\n  pdcbaa "); prn_dword(g_io_buf.usb.xhci.dev_context_ptrs);
	//prn_string("\n or_dcbaapL "); prn_dword(g_io_buf.usb.xhci.hcor->or_dcbaapL);
	//prn_string("\n or_dcbaapH "); prn_dword(g_io_buf.usb.xhci.hcor->or_dcbaapH);

	g_io_buf.usb.xhci.cmd_ring = xhci_ring_alloc(1, 0x10);	
		
	val_64 = (u64)g_io_buf.usb.xhci.hcor->or_crcr; //need to check
	trb_64 = (intptr_t)g_io_buf.usb.xhci.cmd_ring->first_seg->trbs;
	//prn_string("\n g_io_buf.usb.xhci.cmd_ring->first_seg->trbs "); prn_dword(trb_64);
	val_64 = (val_64 & (u64) CMD_RING_RSVD_BITS) | (trb_64 & (u64) ~CMD_RING_RSVD_BITS) |
		  g_io_buf.usb.xhci.cmd_ring->cycle_state;
	//prn_string("\n val_64 H "); prn_dword(val_64>>32);
	//prn_string("\n val_64 L "); prn_dword(val_64);
	g_io_buf.usb.xhci.hcor->or_crcr = val_64;
	//prn_string("\n g_io_buf.usb.xhci.hcor->or_crcr H "); prn_dword(g_io_buf.usb.xhci.hcor->or_crcrH);
	//prn_string("\n g_io_buf.usb.xhci.hcor->or_crcr L "); prn_dword(g_io_buf.usb.xhci.hcor->or_crcrL);
	
	// write the address of db register
	tmp1 = g_io_buf.usb.xhci.hccr->cr_dboff;
	tmp1 &= DBOFF_MASK;
	g_io_buf.usb.xhci.dba = (struct xhci_doorbell_array *)((char *)g_io_buf.usb.xhci.hccr + tmp1);
	prn_string("\n#g_io_buf.usb.xhci.dba "); prn_dword((intptr_t)g_io_buf.usb.xhci.dba);
	
	// write the address of runtime register 
	tmp1 = g_io_buf.usb.xhci.hccr->cr_rtsoff;
	tmp1 &= RTSOFF_MASK;
	g_io_buf.usb.xhci.run_regs = (struct xhci_run_regs *)((char *)g_io_buf.usb.xhci.hccr + tmp1);
	prn_string("#g_io_buf.usb.xhci.run_regs "); prn_dword((intptr_t)g_io_buf.usb.xhci.run_regs);
	// writting the address of ir_set structure 
	g_io_buf.usb.xhci.ir_set = &g_io_buf.usb.xhci.run_regs->ir_set[0];
	prn_string("#g_io_buf.usb.xhci.ir_set "); prn_dword((intptr_t)g_io_buf.usb.xhci.ir_set);


	// Event ring does not maintain link TRB 
	g_io_buf.usb.xhci.event_ring = xhci_ring_alloc(0, 0x11);
	//prn_string("\n  g_io_buf.usb.xhci.event_ring "); prn_dword(g_io_buf.usb.xhci.event_ring);
	//prn_string("\n  g_io_buf.usb.xhci.event_ring->deq_seg "); prn_dword(g_io_buf.usb.xhci.event_ring->deq_seg);	
	//prn_string("\n  trb start "); prn_dword(&g_io_buf.usb.xhci.peventtrb[0]);
	//prn_string("\n  trb end "); prn_dword(&g_io_buf.usb.xhci.peventtrb[TRBS_PER_SEGMENT-1]);
	//prn_string("\n  even_ring.first_seg->trbs[0].link.control "); prn_dword(g_io_buf.usb.xhci.event_ring->first_seg->trbs[0].link.control);
	//prn_string("\n  even_ring.first_seg->trbs[TRBS_PER_SEGMENT-1].link.control "); prn_dword(g_io_buf.usb.xhci.event_ring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control);
	//prn_string("\n  g_io_buf.usb.xhci.event_ring->first_seg->trbs[TRBS_PER_SEGMENT-1] "); prn_dword(&g_io_buf.usb.xhci.event_ring->first_seg->trbs[TRBS_PER_SEGMENT-1]);
	//prn_string("\n  g_io_buf.usb.xhci.event_ring->dequeue "); prn_dword(g_io_buf.usb.xhci.event_ring->dequeue);
		
	memset32((u32 *)&g_io_buf.usb.xhci.pentries, 0, sizeof(g_io_buf.usb.xhci.pentries));
	g_io_buf.usb.xhci.erst.entries = g_io_buf.usb.xhci.pentries;
	//prn_string("\n  g_io_buf.usb.xhci.erst.entries "); prn_dword(g_io_buf.usb.xhci.pentries);
	g_io_buf.usb.xhci.erst.num_entries = ERST_NUM_SEGS;
	
	struct xhci_segment *seg;
	for (tmp1 = 0, seg = g_io_buf.usb.xhci.event_ring->first_seg; tmp1 < ERST_NUM_SEGS; tmp1++) {
		trb_64 = 0;
		trb_64 = (intptr_t)seg->trbs;
		//prn_string("\n  trb_64 H "); prn_dword(trb_64>>32);
		//prn_string("\n  trb_64 L "); prn_dword(trb_64);
		//struct xhci_erst_entry *entry = (struct xhci_erst_entry *) &g_io_buf.usb.xhci.erst.entries[tmp1];
		//prn_string("\n  entry "); prn_dword(entry);
		//prn_string("\n  seg_addr "); prn_dword(&g_io_buf.usb.xhci.erst.entries[tmp1].seg_addr);
		g_io_buf.usb.xhci.erst.entries[tmp1].seg_addr = trb_64;
		//prn_string("\n  g_io_buf.usb.xhci.erst.entries[tmp1].seg_addr "); prn_dword(g_io_buf.usb.xhci.erst.entries[tmp1].seg_addr);
		g_io_buf.usb.xhci.erst.entries[tmp1].seg_size = TRBS_PER_SEGMENT;
		g_io_buf.usb.xhci.erst.entries[tmp1].rsvd = 0;
		seg = seg->next;
	}
	// Event ring does not maintain link TRB
	
	//
	u64 deq;
	deq = (intptr_t) g_io_buf.usb.xhci.event_ring->dequeue;

	//prn_string("\n  deq "); prn_dword(deq);
	// Update HC event ring dequeue pointer 
	//prn_string("\n  bf ir_set->erst_dequeue "); prn_dword(g_io_buf.usb.xhci.ir_set->erst_dequeue);
	g_io_buf.usb.xhci.ir_set->erst_dequeue = (u64)deq & (u64)~ERST_PTR_MASK;
	//prn_string("\n  ir_set->erst_dequeue "); prn_dword(g_io_buf.usb.xhci.ir_set->erst_dequeue);
	// set ERST count with the number of entries in the segment table 
	tmp1 = g_io_buf.usb.xhci.ir_set->erst_size;
	tmp1 &= ERST_SIZE_MASK;
	tmp1 |= ERST_NUM_SEGS;
	g_io_buf.usb.xhci.ir_set->erst_size = tmp1;
	//prn_string("\n  ir_set->erst_size "); prn_dword(g_io_buf.usb.xhci.ir_set->erst_size);
	// this is the event ring segment table pointer 
	val_64 = g_io_buf.usb.xhci.ir_set->erst_base;//need to check
	//prn_string("\n  bf val_64 H "); prn_dword(val_64 >> 32);
	//prn_string("\n  bf val_64 L "); prn_dword(val_64);
	val_64 &= ERST_PTR_MASK;
	val_64 |= ((intptr_t)(g_io_buf.usb.xhci.erst.entries) & ~ERST_PTR_MASK);
        //prn_string("\n  val_64 H "); prn_dword(val_64>>32);
	//prn_string("\n  val_64 L "); prn_dword(val_64);
	g_io_buf.usb.xhci.ir_set->erst_base = val_64;//need to check
	//prn_string("\n ir_set->erst_base "); prn_dword(g_io_buf.usb.xhci.ir_set->erst_base);
	
// xhci_scratchpad_alloc
	//u32 num_sp;
	val_64 = HCS_MAX_SCRATCHPAD(g_io_buf.usb.xhci.hccr->cr_hcsparams2);
	prn_string("\n  #num_sp "); prn_dword(val_64);
	
	tmp1 = g_io_buf.usb.xhci.hcor->or_pagesize & 0xffff;
	prn_string("  #hcor->or_pagesize "); prn_dword(g_io_buf.usb.xhci.hcor->or_pagesize);
	for (tmp2 = 0; tmp2 < 16; tmp2++) {
		if ((0x1 & tmp1) != 0)
			break;
		tmp1 = tmp1 >> 1;
	}
	tmp1 = 1 << (tmp2 + 12);
	
	//u32 tmpbuf[num_sp];
	for (tmp2 = 0; tmp2 < val_64; tmp2++) {
		//g_io_buf.usb.xhci.pscratchpad.sp_array = (u64 *)(&tmpbuf + tmp2 * tmp1);//assume num_sp = 1
		g_io_buf.usb.xhci.sparray[tmp2] = (intptr_t)(g_io_buf.usb.xhci.sparraybuf + tmp2 * tmp1);
	}
	g_io_buf.usb.xhci.pscratchpad.sp_array = g_io_buf.usb.xhci.sparray;
	
	g_io_buf.usb.xhci.scratchpad = &g_io_buf.usb.xhci.pscratchpad;
	
	//prn_string("\n  g_io_buf.usb.xhci.sparray "); prn_dword((u64)g_io_buf.usb.xhci.sparray);
	g_io_buf.usb.xhci.dcbaa->dev_context_ptrs[0] = (intptr_t)g_io_buf.usb.xhci.pscratchpad.sp_array;
	//prn_string("\n  #g_io_buf.usb.xhci.dcbaa->dev_context_ptrs[0] "); prn_dword(g_io_buf.usb.xhci.dcbaa->dev_context_ptrs[0]);
		
// initializing the virtual devices to NULL
	for (tmp2 = 0; tmp2 < MAX_HC_SLOTS; ++tmp2)
		g_io_buf.usb.xhci.devs[tmp2] = NULL;

	/*
	 * Just Zero'ing this register completely,
	 * or some spurious Device Notification Events
	 * might screw things here.
	 */
	g_io_buf.usb.xhci.hcor->or_dnctrl = 0x0;
	
	tmp1 = g_io_buf.usb.xhci.hccr->cr_hcsparams1;
	//prn_string("\n hccr->cr_hcsparams1 "); prn_dword(tmp1);
	//descriptor.hub.bNbrPorts = ((tmp1 & HCS_MAX_PORTS_MASK) >>
	//					HCS_MAX_PORTS_SHIFT);
						
	// Port Indicators ????
	tmp1 = g_io_buf.usb.xhci.hccr->cr_hccparams;
	prn_string("\n  #hccr->cr_hccparams "); prn_dword(tmp1);
	//if (HCS_INDICATOR(tmp1))
	//	put_unaligned(get_unaligned(&descriptor.hub.wHubCharacteristics)
	//			| 0x80, &descriptor.hub.wHubCharacteristics);

	// Port Power Control ????
	//if (HCC_PPC(tmp1))
	//	put_unaligned(get_unaligned(&descriptor.hub.wHubCharacteristics)
	//			| 0x01, &descriptor.hub.wHubCharacteristics);
        
// xhci_start
	//if (xhci_start(hcor)) {
	//	xhci_reset(hcor);
	//	return -ENODEV;
	//}
	prn_string("\n**<xhci_start>**");
	tmp1 = g_io_buf.usb.xhci.hcor->or_usbcmd;
	tmp1 |= (CMD_RUN);
	g_io_buf.usb.xhci.hcor->or_usbcmd = tmp1;
	//prn_string("\n g_io_buf.usb.xhci.hcor->or_usbsts "); prn_dword(g_io_buf.usb.xhci.hcor->or_usbsts);
//handshake
        handshake(&g_io_buf.usb.xhci.hcor->or_usbsts, STS_HALT, 0, XHCI_MAX_HALT_USEC/1000);
        
	/* Zero'ing IRQ control register and IRQ pending register */
	g_io_buf.usb.xhci.ir_set->irq_control = 0x0;
	g_io_buf.usb.xhci.ir_set->irq_pending = 0x0;

	//reg = HC_VERSION(g_io_buf.usb.xhci.hccr->cr_capbase);
	//prn_string("\n XHCI "); prn_dword(HC_VERSION(g_io_buf.usb.xhci.hccr->cr_capbase));
//hub power on
	tmp3 = 0;
	do
	{
		prn_string("\n**<hub port power on 1/2>**");
		#if 0
		for (trb_64 = 0; trb_64 < 2; trb_64++) {
 			tmp1 = g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc;
			prn_dword(g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc);
			tmp1 = (tmp1 & XHCI_PORT_RO) | (tmp1 & XHCI_PORT_RWS);
			//prn_string("\n tmp1 "); prn_dword(tmp1);
			tmp1 |= PORT_POWER;
			g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc = tmp1;
			prn_string("af hub port 1/2 power on "); prn_dword(g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc);
		}
		#endif
		for (trb_64 = 0; trb_64 < 2; trb_64++) {
			prn_string("\n    port "); prn_dword((intptr_t)&g_io_buf.usb.xhci.hcor->portregs[trb_64]);					
			tmp1 = g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc;
			tmp1 = (tmp1 & XHCI_PORT_RO) | (tmp1 & XHCI_PORT_RWS);
			tmp1 |= PORT_POWER;
			g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc = tmp1;
//usb_get_port_status			
			for (tmp2 = 0; tmp2 < 10; tmp2++) {
				_delay_1ms(100);	
				//_delay_1ms(20);		
				tmp1 = g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc;
				if (!(tmp1 & 0x0001) && !((tmp1 >> 16) & 0x0001)) {
					prn_string("!!!!!no connection port no "); prn_dword(trb_64);
				} else
					break;
			}
			if (tmp2 >= 10)
				continue;
			
			tmp2 = g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc;
			tmp2 = (tmp2 & XHCI_PORT_RO) | (tmp2 & XHCI_PORT_RWS);
			tmp2 |= PORT_CSC;
			g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc = tmp2;
			//prn_string("\n    clear feature hcor->portregs[trb_64].or_portsc "); prn_dword(g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc);
			//if ((!(tmp1 & 0x0001)) && (!(tmp1 & 0x0002))) {
			//	prn_string(" disconnect!!!!!\n");
				/* Return now if nothing is connected */
			//	if (!(tmp1 & 0x0001)) {
			//		continue;
			//	}
			//}
// Reset the port
			val_64 = 0;
			tmp2 = 20;	
			do
			{
				tmp1 = g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc;
				tmp1 = (tmp1 & XHCI_PORT_RO) | (tmp1 & XHCI_PORT_RWS);
				tmp1 |= XHCI_PORT_RESET;
				g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc = tmp1;
				_delay_1ms(tmp2); //HUB_SHORT_RESET_TIME = 20
				tmp1 = g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc;
				//prn_string("\n hcor->portregs[trb_64].or_portsc "); prn_dword(tmp1);
				tmp2 = 200;
				if (tmp1 & PORT_PE)
					break;
			} while ((val_64++) < 5);
		
			if (val_64 < 5)
				break;
			else {
				prn_string("!!!!!port reset failed!!!!!");
			}
		} 
		
		if (trb_64 < 2)
			break;
		else {
			for (trb_64 = 0; trb_64 < 2; trb_64++) {
 				tmp1 = g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc;
				tmp1 = (tmp1 & XHCI_PORT_RO) | (tmp1 & XHCI_PORT_RWS);
				//prn_string("\n tmp1 "); prn_dword(tmp1);
				tmp1 &= ~PORT_POWER;
				g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc = tmp1;
			}
			_delay_1ms(100);
		}
	} while (tmp3++ < 2);
	
	if (tmp3 >= 2) {
		prn_string("\n!!!!!NO USB CONNECTION!!!!!"); 
		return -1;
	}
	
	tmp2 = g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc;
	tmp2 = (tmp2 & XHCI_PORT_RO) | (tmp2 & XHCI_PORT_RWS);
	tmp2 |= PORT_RC;
	g_io_buf.usb.xhci.hcor->portregs[trb_64].or_portsc = tmp2;
	//prn_string("\n port 1 Reset the port end "); prn_dword(g_io_buf.usb.xhci.hcor->portregs[0].or_portsc);
	prn_string("\n**<port 1/2 Reset the port end**> "); prn_dword(tmp1);
	g_io_buf.usb.xhci.udev.portnr = trb_64 + 1;
	switch (tmp1 & DEV_SPEED_MASK) {
		case XDEV_HS:
			g_io_buf.usb.xhci.udev.speed = USB_SPEED_HIGH;
			break;
		case XDEV_SS:
			g_io_buf.usb.xhci.udev.speed = USB_SPEED_SUPER;
			break;
	}
	prn_string("#####speed "); prn_dword(g_io_buf.usb.xhci.udev.speed);
	prn_string("#####portnr "); prn_dword(g_io_buf.usb.xhci.udev.portnr);
//_xhci_alloc_device
	prn_string("\n**<alloc_device>**");
	queue_command(NULL, 0, 0, TRB_ENABLE_SLOT);	
	event = wait_for_event(TRB_COMPLETION);
	if (GET_COMP_CODE(event->event_cmd.status) != 1) {
		prn_string("\n!!!!!GET_COMP_CODE!!!!! "); prn_dword(GET_COMP_CODE(event->event_cmd.status));
	}	
	g_io_buf.usb.xhci.udev.slot_id = TRB_TO_SLOT_ID(event->event_cmd.flags);
			
	acknowledge_event();
        
//xhci_alloc_virt_device
        memset32((u32 *)&g_io_buf.usb.xhci.pdevs, 0, sizeof(g_io_buf.usb.xhci.pdevs));
        //out ctx
        prn_string("\n  slot id "); prn_dword(g_io_buf.usb.xhci.udev.slot_id);
        g_io_buf.usb.xhci.pout_ctx.type = (int)XHCI_CTX_TYPE_DEVICE;
        g_io_buf.usb.xhci.pout_ctx.size = (MAX_EP_CTX_NUM + 1) *
					    CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams);        
        g_io_buf.usb.xhci.pout_ctx.bytes = g_io_buf.usb.xhci.poutbyte;//???
        g_io_buf.usb.xhci.pdevs.out_ctx = &g_io_buf.usb.xhci.pout_ctx;
        //in ctx
        g_io_buf.usb.xhci.pin_ctx.type = (int)XHCI_CTX_TYPE_INPUT;
        g_io_buf.usb.xhci.pin_ctx.size = (MAX_EP_CTX_NUM + 1) *
					    CTX_SIZE(g_io_buf.usb.xhci.hccr->cr_hccparams);
	g_io_buf.usb.xhci.pin_ctx.bytes = g_io_buf.usb.xhci.pinbyte;//???
        g_io_buf.usb.xhci.pdevs.in_ctx = &g_io_buf.usb.xhci.pin_ctx;
        //
        g_io_buf.usb.xhci.pdevs.eps[0].ring = xhci_ring_alloc(1, 0);
        //prn_string("\n  g_io_buf.usb.xhci.pdevs.eps[0].ring "); prn_dword(g_io_buf.usb.xhci.pdevs.eps[0].ring);
	//prn_string("\n  g_io_buf.usb.xhci.pdevs.eps[0].ring.enq_seg "); prn_dword(g_io_buf.usb.xhci.pdevs.eps[0].ring->enq_seg);	
	//prn_string("\n  trb start "); prn_dword(&g_io_buf.usb.xhci.pep0trb[0]);
	//prn_string("\n  trb end "); prn_dword(&g_io_buf.usb.xhci.pep0trb[TRBS_PER_SEGMENT-1]);
	//prn_string("\n  eps[0].ring.first_seg->trbs[0].link.control "); prn_dword(g_io_buf.usb.xhci.pdevs.eps[0].ring->first_seg->trbs[0].link.control);
	//prn_string("\n  eps[0].ring.first_seg->trbs[TRBS_PER_SEGMENT-1].link.control "); prn_dword(g_io_buf.usb.xhci.pdevs.eps[0].ring->first_seg->trbs[TRBS_PER_SEGMENT-1].link.control);
	//prn_string("\n  eps[0].ring.first_seg->trbs[TRBS_PER_SEGMENT-1] "); prn_dword(&g_io_buf.usb.xhci.pdevs.eps[0].ring->first_seg->trbs[TRBS_PER_SEGMENT-1]);
	//prn_string("\n  g_io_buf.usb.xhci.pdevs.eps[0].ring.enqueue "); prn_dword(g_io_buf.usb.xhci.pdevs.eps[0].ring->enqueue);
        
        g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id] = &g_io_buf.usb.xhci.pdevs;
        //prn_string("\n  g_io_buf.usb.xhci.devs[slot_id] "); prn_dword(g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]);
        //prn_string("\n  g_io_buf.usb.xhci.devs[slot_id]->out_ctx->type "); prn_dword(g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->out_ctx->type);
        //prn_string("\n  g_io_buf.usb.xhci.devs[slot_id]->out_ctx->size "); prn_dword(g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->out_ctx->size);
        //prn_string("\n  g_io_buf.usb.xhci.devs[slot_id]->out_ctx->bytes "); prn_dword(g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->out_ctx->bytes);
        //prn_string("\n  g_io_buf.usb.xhci.devs[slot_id]->in_ctx->type "); prn_dword(g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->in_ctx->type);
        //prn_string("\n  g_io_buf.usb.xhci.devs[slot_id]->in_ctx->size "); prn_dword(g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->in_ctx->size);
        //prn_string("\n  g_io_buf.usb.xhci.devs[slot_id]->in_ctx->bytes "); prn_dword(g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->in_ctx->bytes);
        g_io_buf.usb.xhci.dcbaa->dev_context_ptrs[g_io_buf.usb.xhci.udev.slot_id] = (intptr_t)g_io_buf.usb.xhci.devs[g_io_buf.usb.xhci.udev.slot_id]->out_ctx->bytes;
//usb_setup_descriptor
	g_io_buf.usb.xhci.udev.maxpacketsize = 3;//PACKET_SIZE_64;
	g_io_buf.usb.xhci.udev.epmaxpacketin[0] = 64;
	g_io_buf.usb.xhci.udev.epmaxpacketout[0] = 64;	
//set address    set_address(void)
        prn_string("\n**<set address>**");
        set_address(g_io_buf.usb.xhci.udev.slot_id);
	
//get_descriptor_len
	prn_string("\n**<get_descriptor_len>**");
	//xhci_ctrl_tx
	//pUSB_DevDesc pDev = (pUSB_DevDesc)(USB_dataBuf);
	USB_vendorCmd(USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, DESC_DEVICE, 0, 0x12);	

	//prn_string("\n    g_io_buf.usb.cmd_buf[0] "); prn_dword((g_io_buf.usb.cmd_buf[1]<<8)|g_io_buf.usb.cmd_buf[0]);
	//prn_string("\n    g_io_buf.usb.cmd_buf[2] "); prn_dword((g_io_buf.usb.cmd_buf[3]<<8)|g_io_buf.usb.cmd_buf[2]);
	//prn_string("\n    pDev.bcdUSB "); prn_dword(pDev->bcdUSB);
	//prn_string("\n    pDev.idVendor "); prn_dword(pDev->idVendor);
	//prn_string("\n    pDev.idProduct "); prn_dword(pDev->idProduct);
	//prn_string("\n    pDev.bcdDevice "); prn_dword(pDev->bcdDevice);
	_delay_1ms(1);
//usb_get_configuration_len
	prn_string("\n**<usb_get_configuration_len>**");	
	pUSB_CfgDesc pCfg = (pUSB_CfgDesc)(USB_dataBuf);
	
	USB_vendorCmd(USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, DESC_CONFIGURATION, 0, 0x9);
	
	//prn_string("\n    g_io_buf.usb.cmd_buf[0] "); prn_dword((g_io_buf.usb.cmd_buf[1]<<8)|g_io_buf.usb.cmd_buf[0]);  
	//prn_string("\n    g_io_buf.usb.cmd_buf[2] "); prn_dword((g_io_buf.usb.cmd_buf[3]<<8)|g_io_buf.usb.cmd_buf[2]);
	//prn_string("\n    pCfg->wLength "); prn_dword(pCfg->wLength);
	
//usb_get_configuration_no
	prn_string("\n**<usb_get_configuration_no>**");	
	//pUSB_CfgDesc pCfg = (pUSB_CfgDesc)(USB_dataBuf);
	USB_vendorCmd(USB_DIR_IN, USB_REQ_GET_DESCRIPTOR, DESC_CONFIGURATION, 0, pCfg->wLength);
	
	usb_parse_config(g_io_buf.usb.cmd_buf, 0);
	usb_set_maxpacket(&g_io_buf.usb.xhci.udev);
	//prn_string("\n    g_io_buf.usb.cmd_buf[0] "); prn_dword((g_io_buf.usb.cmd_buf[1]<<8)|g_io_buf.usb.cmd_buf[0]);
	//prn_string("\n    g_io_buf.usb.cmd_buf[2] "); prn_dword((g_io_buf.usb.cmd_buf[3]<<8)|g_io_buf.usb.cmd_buf[2]); 
	//prn_string("\n    pCfg->wLength "); prn_dword(pCfg->wLength);
	//prn_string("\n    pCfg->bLength "); prn_dword(pCfg->bLength);
	//prn_string("\n    pCfg->bNumIntf "); prn_dword(pCfg->bNumIntf);
	//prn_string("\n    pCfg->bConfigurationValue "); prn_dword(pCfg->bCV);
#if 0	
	tmp1 = pCfg->bLength;
	pCfg = (pUSB_CfgDesc)(&USB_dataBuf[tmp1]);
	prn_string("\n    tmp1 "); prn_dword(tmp1);
	prn_string("\n    pCfg->bType "); prn_dword(pCfg->bType);
	
	tmp1 += pCfg->bLength;
	pCfg = (pUSB_CfgDesc)(&USB_dataBuf[tmp1]);

	prn_string("\n    tmp1 0 "); prn_dword(tmp1);
	prn_string("\n    pCfg->bType "); prn_dword(pCfg->bType);
	prn_string("\n    pCfg->wMaxPacketSize "); prn_dword(((pCfg->bCV<<8) | pCfg->bNumIntf));
	prn_string("\n    pCfg->bmAttributes "); prn_dword((pCfg->wLength & 0x300) >> 8);
	prn_string("\n    pCfg->bEndpointAddress "); prn_dword(pCfg->wLength & 0xff);
	tmp1 += pCfg->bLength;
	pCfg = (pUSB_CfgDesc)(&USB_dataBuf[tmp1]);
	prn_string("\n    tmp1 1 "); prn_dword(tmp1);
	prn_string("\n    pCfg->bType "); prn_dword(pCfg->bType);
	prn_string("\n    pCfg->wMaxPacketSize "); prn_dword(((pCfg->bCV<<8) | pCfg->bNumIntf));
	prn_string("\n    pCfg->bmAttributes "); prn_dword((pCfg->wLength & 0x300) >> 8);
	prn_string("\n    pCfg->bEndpointAddress "); prn_dword(pCfg->wLength & 0xff);
	tmp1 += pCfg->bLength;
	pCfg = (pUSB_CfgDesc)(&USB_dataBuf[tmp1]);
	prn_string("\n    tmp1 2 "); prn_dword(tmp1);
	prn_string("\n    pCfg->bType "); prn_dword(pCfg->bType);
	prn_string("\n    pCfg->wMaxPacketSize "); prn_dword(((pCfg->bCV<<8) | pCfg->bNumIntf));
	prn_string("\n    pCfg->bmAttributes "); prn_dword((pCfg->wLength & 0x300) >> 8);
	prn_string("\n    pCfg->bEndpointAddress "); prn_dword(pCfg->wLength & 0xff);
#endif
//xhci_set_configuration
	prn_string("\n**<set_configuration>**");
	USB_vendorCmd(0, USB_REQ_SET_CONFIGURATION, 0x1, 0, 0);	
	_delay_1ms(10);
//usb_string
	prn_string("\n**<usb_string>**");	
	g_io_buf.usb.xhci.udev.string_langid = 0;
	usb_string(1, g_io_buf.usb.xhci.udev.mf, sizeof(g_io_buf.usb.xhci.udev.mf));
	usb_string(2, g_io_buf.usb.xhci.udev.prod, sizeof(g_io_buf.usb.xhci.udev.prod));
	usb_string(3, g_io_buf.usb.xhci.udev.serial, sizeof(g_io_buf.usb.xhci.udev.serial));
	prn_string("\n  mf     ");prn_string(g_io_buf.usb.xhci.udev.mf);
	prn_string("\n  prod   ");prn_string(g_io_buf.usb.xhci.udev.prod);
	prn_string("\n  serial ");prn_string(g_io_buf.usb.xhci.udev.serial);
	
//usb_get_max_lun
	prn_string("\n**<usb_get_max_lun>**");
	USB_vendorCmd(0xa1, 0xfe, 0, 0, 1);
	//prn_string("\n  g_io_buf.usb.cmd_buf ");prn_string(g_io_buf.usb.cmd_buf[0]);
//usb_inquiry
	prn_string("\n**<usb_inquiry>**");
	usb_inquiry();
//usb_test_unit_ready
	prn_string("\n**<usb_test_unit_ready>**");
	usb_test_unit_ready();	
//usb_read_capacity
	prn_string("\n**<usb_read_capacity>**");
	usb_read_capacity();
	prn_string("\n  #####end##### ");	
#if 0	
	// Test Unit Ready
	tmp2 = 0;
	for (tmp1 = 0; tmp1 < MAX_TEST_UNIT_READY_TRY; tmp1++) {
		prn_string("Test Unit Ready ("); prn_decimal(tmp1); prn_string(")\n");
		tmp2 = USB_testUnitReady();
#ifndef CSIM_NEW
		_delay_1ms(50);
#endif
	}
	if (tmp2) {
		prn_string("unit still not ready...\n");
		boot_reset();	
	}
#endif
	return 0;
}

/*
 * usb_readSector
 * @lba		LBA to read
 * @count       not used
 * @buf		destination buffer
 */
int usb_readSector(u32 lba, u32 count, u32 *dest)
{
	CSTAMP(0xE5B00013);

	USB_DBG();

	g_io_buf.usb.xhci.reserved[0] = SCSICMD_READ_10;
	g_io_buf.usb.xhci.reserved[1] = 0;
	g_io_buf.usb.xhci.reserved[2] = (u8)(lba >> 24) & 0xff;
	g_io_buf.usb.xhci.reserved[3] = (u8)(lba >> 16) & 0xff;
	g_io_buf.usb.xhci.reserved[4] = (u8)(lba >> 8) & 0xff;
	g_io_buf.usb.xhci.reserved[5] = (u8)(lba) & 0xff;
	g_io_buf.usb.xhci.reserved[7] = (u8)(count >> 8) & 0xff;
	g_io_buf.usb.xhci.reserved[8] = (u8)count & 0xff;

	stor_BBB_transport((count << 9), 12, 1, (u8 *)dest);
				
	CSTAMP(0xE5B00014);
	return 0;
}

