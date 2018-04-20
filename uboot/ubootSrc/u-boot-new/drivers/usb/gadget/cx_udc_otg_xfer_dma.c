/*
 * cloudersemi on-chip full/high speed USB OTG 2.0 device controllers
 *
 * Copyright (C) 2015 for cloudersemi co. Ltd
 *
 * BSP Support for Samsung's UDC driver
 * available at:
 * git://git.kernel.org/pub/scm/linux/kernel/git/kki_ap/linux-2.6-samsung.git
 *
 * State machine bugfixes:
 * Marek Szyprowski <m.szyprowski@samsung.com>
 *
 * Ported to u-boot:
 * Marek Szyprowski <m.szyprowski@samsung.com>
 * Lukasz Majewski <l.majewski@samsumg.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <asm/errno.h>
#include <linux/list.h>
#include <malloc.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>
#include <asm/types.h>
#include <asm/io.h>
#include <usb/lin_gadget_compat.h>
#include "cx-regs-otg.h"

#define DEBUG_INIT 0
#define DEBUG_SETUP 0
#define DEBUG_EP0 0
#define DEBUG_ISR 0
#define DEBUG_QUEUE 0
#define DEBUG_OUT_EP 0
#define DEBUG_IN_EP 0
#define DEBUG_RT 0

#include <usb/cx_udc.h>

#define EP0_CON		0
#define EP_MASK		0xF

static char *state_names[] = {
	"WAIT_FOR_SETUP",
	"DATA_STATE_XMIT",
	"DATA_STATE_NEED_ZLP",
	"WAIT_FOR_OUT_STATUS",
	"DATA_STATE_RECV",
	"WAIT_FOR_COMPLETE",
	"WAIT_FOR_OUT_COMPLETE",
	"WAIT_FOR_IN_COMPLETE",
	"WAIT_FOR_NULL_COMPLETE",
};

#define DRIVER_DESC "CX HS USB OTG Device Driver, (c) Cloudersemi Electronics"
#define DRIVER_VERSION "15 March 2009"

static struct cx_udc	*the_controller;

static const char driver_name[] = "cx-hsotg";
static const char driver_desc[] = DRIVER_DESC;
static const char ep0name[] = "ep0-control";

/* Max packet size*/
static unsigned int ep0_fifo_size = 64;
static unsigned int ep_fifo_size =  512;
static unsigned int ep_fifo_size2 = 1024;
static int reset_available = 1;

static struct usb_ctrlrequest *usb_ctrl;
static dma_addr_t usb_ctrl_dma_addr;

/*
  Local declarations.
*/
static int cx_ep_enable(struct usb_ep *ep,
			 const struct usb_endpoint_descriptor *);
static int cx_ep_disable(struct usb_ep *ep);
static struct usb_request *cx_alloc_request(struct usb_ep *ep,
					     gfp_t gfp_flags);
static void cx_free_request(struct usb_ep *ep, struct usb_request *);

static int cx_queue(struct usb_ep *ep, struct usb_request *, gfp_t gfp_flags);
static int cx_dequeue(struct usb_ep *ep, struct usb_request *);
static int cx_fifo_status(struct usb_ep *ep);
static void cx_fifo_flush(struct usb_ep *ep);
static void cx_ep0_read(struct cx_udc *dev);
static void cx_ep0_kick(struct cx_udc *dev, struct cx_ep *ep);
static void cx_handle_ep0(struct cx_udc *dev);
static int cx_ep0_write(struct cx_udc *dev);
static int write_fifo_ep0(struct cx_ep *ep, struct cx_request *req);
static void done(struct cx_ep *ep, struct cx_request *req, int status);
static void stop_activity(struct cx_udc *dev,
			  struct usb_gadget_driver *driver);
static int udc_enable(struct cx_udc *dev);
static void udc_set_address(struct cx_udc *dev, unsigned char address);
static void reconfig_usbd(void);
static void set_max_pktsize(struct cx_udc *dev, enum usb_device_speed speed);
static void nuke(struct cx_ep *ep, int status);
static int cx_udc_set_halt(struct usb_ep *_ep, int value);
static void cx_udc_set_nak(struct cx_ep *ep);

void set_udc_gadget_private_data(void *p)
{
	debug_cond(DEBUG_SETUP != 0,
		   "%s: the_controller: 0x%p, p: 0x%p\n", __func__,
		   the_controller, p);
	the_controller->gadget.dev.device_data = p;
}

void *get_udc_gadget_private_data(struct usb_gadget *gadget)
{
	return gadget->dev.device_data;
}

static struct usb_ep_ops cx_ep_ops = {
	.enable = cx_ep_enable,
	.disable = cx_ep_disable,

	.alloc_request = cx_alloc_request,
	.free_request = cx_free_request,

	.queue = cx_queue,
	.dequeue = cx_dequeue,

	.set_halt = cx_udc_set_halt,
	.fifo_status = cx_fifo_status,
	.fifo_flush = cx_fifo_flush,
};

/***********************************************************/

void __iomem		*regs_otg;
struct cx_usbotg_reg *reg;
struct cx_usbotg_phy *phy;
static unsigned int usb_phy_ctrl;

static u8 clear_feature_num;
int clear_feature_flag;

/* Bulk-Only Mass Storage Reset (class-specific request) */
#define GET_MAX_LUN_REQUEST	0xFE
#define BOT_RESET_REQUEST	0xFF

static inline void cx_udc_ep0_zlp(struct cx_udc *dev)
{
	u32 ep_ctrl;

	writel(usb_ctrl_dma_addr, &reg->in_endp[EP0_CON].diepdma);
	writel(DIEPT_SIZ_PKT_CNT(1), &reg->in_endp[EP0_CON].dieptsiz);

	ep_ctrl = readl(&reg->in_endp[EP0_CON].diepctl);
	writel(ep_ctrl|DEPCTL_EPENA|DEPCTL_CNAK,
	       &reg->in_endp[EP0_CON].diepctl);

	debug_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		__func__, readl(&reg->in_endp[EP0_CON].diepctl));
	dev->ep0state = WAIT_FOR_IN_COMPLETE;
}

void cx_udc_pre_setup(void)
{
	u32 ep_ctrl;

	debug_cond(DEBUG_IN_EP,
		   "%s : Prepare Setup packets.\n", __func__);

	writel(DOEPT_SIZ_PKT_CNT(1) | sizeof(struct usb_ctrlrequest),
	       &reg->out_endp[EP0_CON].doeptsiz);
	writel(usb_ctrl_dma_addr, &reg->out_endp[EP0_CON].doepdma);

	ep_ctrl = readl(&reg->out_endp[EP0_CON].doepctl);
	writel(ep_ctrl|DEPCTL_EPENA, &reg->out_endp[EP0_CON].doepctl);

	debug_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		__func__, readl(&reg->in_endp[EP0_CON].diepctl));
	debug_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DOEPCTL0 = 0x%x\n",
		__func__, readl(&reg->out_endp[EP0_CON].doepctl));

}

static inline void cx_ep0_complete_out(void)
{
	u32 ep_ctrl;

	debug_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		__func__, readl(&reg->in_endp[EP0_CON].diepctl));
	debug_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DOEPCTL0 = 0x%x\n",
		__func__, readl(&reg->out_endp[EP0_CON].doepctl));

	debug_cond(DEBUG_IN_EP,
		"%s : Prepare Complete Out packet.\n", __func__);

	writel(DOEPT_SIZ_PKT_CNT(1) | sizeof(struct usb_ctrlrequest),
	       &reg->out_endp[EP0_CON].doeptsiz);
	writel(usb_ctrl_dma_addr, &reg->out_endp[EP0_CON].doepdma);

	ep_ctrl = readl(&reg->out_endp[EP0_CON].doepctl);
	writel(ep_ctrl|DEPCTL_EPENA|DEPCTL_CNAK,
	       &reg->out_endp[EP0_CON].doepctl);

	debug_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		__func__, readl(&reg->in_endp[EP0_CON].diepctl));
	debug_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DOEPCTL0 = 0x%x\n",
		__func__, readl(&reg->out_endp[EP0_CON].doepctl));

}


static int setdma_rx(struct cx_ep *ep, struct cx_request *req)
{
	u32 *buf, ctrl;
	u32 length, pktcnt;
	u32 ep_num = ep_index(ep);

	buf = req->req.buf + req->req.actual;
	length = min(req->req.length - req->req.actual,
		     ep_num ? DMA_BUFFER_SIZE : ep->ep.maxpacket);

	ep->len = length;
	ep->dma_buf = buf;

	if (ep_num == EP0_CON || length == 0)
		pktcnt = 1;
	else
		pktcnt = (length - 1)/(ep->ep.maxpacket) + 1;

	ctrl =  readl(&reg->out_endp[ep_num].doepctl);

	writel((unsigned int) ep->dma_buf, &reg->out_endp[ep_num].doepdma);
	writel(DOEPT_SIZ_PKT_CNT(pktcnt) | DOEPT_SIZ_XFER_SIZE(length),
	       &reg->out_endp[ep_num].doeptsiz);
	writel(DEPCTL_EPENA|DEPCTL_CNAK|ctrl, &reg->out_endp[ep_num].doepctl);

	debug_cond(DEBUG_OUT_EP != 0,
		   "%s: EP%d RX DMA start : DOEPDMA = 0x%x,"
		   "DOEPTSIZ = 0x%x, DOEPCTL = 0x%x\n"
		   "\tbuf = 0x%p, pktcnt = %d, xfersize = %d\n",
		   __func__, ep_num,
		   readl(&reg->out_endp[ep_num].doepdma),
		   readl(&reg->out_endp[ep_num].doeptsiz),
		   readl(&reg->out_endp[ep_num].doepctl),
		   buf, pktcnt, length);
	return 0;

}

int setdma_tx(struct cx_ep *ep, struct cx_request *req)
{
	u32 *buf, ctrl = 0;
	u32 length, pktcnt;
	u32 ep_num = ep_index(ep);

	buf = req->req.buf + req->req.actual;
	length = req->req.length - req->req.actual;

	if (ep_num == EP0_CON)
		length = min(length, (u32)ep_maxpacket(ep));

	ep->len = length;
	ep->dma_buf = buf;

	flush_dcache_range((unsigned long) ep->dma_buf,
			   (unsigned long) ep->dma_buf +
			   ROUND(ep->len, CONFIG_SYS_CACHELINE_SIZE));

	if (length == 0)
		pktcnt = 1;
	else
		pktcnt = (length - 1)/(ep->ep.maxpacket) + 1;

	/* Flush the endpoint's Tx FIFO */
	writel(TX_FIFO_NUMBER(ep->fifo_num), &reg->grstctl);
	writel(TX_FIFO_NUMBER(ep->fifo_num) | TX_FIFO_FLUSH, &reg->grstctl);
	while (readl(&reg->grstctl) & TX_FIFO_FLUSH)
		;

	writel((unsigned long)ep->dma_buf & ~KSEG0, &reg->in_endp[ep_num].diepdma);
	writel(DIEPT_SIZ_PKT_CNT(pktcnt) | DIEPT_SIZ_XFER_SIZE(length),
	       &reg->in_endp[ep_num].dieptsiz);

	ctrl = readl(&reg->in_endp[ep_num].diepctl);

	/* Write the FIFO number to be used for this endpoint */
	ctrl &= DIEPCTL_TX_FIFO_NUM_MASK;
	ctrl |= DIEPCTL_TX_FIFO_NUM(ep->fifo_num);

	/* Clear reserved (Next EP) bits */
	ctrl = (ctrl&~(EP_MASK<<DEPCTL_NEXT_EP_BIT));

	writel(DEPCTL_EPENA|DEPCTL_CNAK|ctrl, &reg->in_endp[ep_num].diepctl);

	debug_cond(DEBUG_IN_EP,
		"%s:EP%d TX DMA start : DIEPDMA0 = 0x%x,"
		"DIEPTSIZ0 = 0x%x, DIEPCTL0 = 0x%x\n"
		"\tbuf = 0x%p, pktcnt = %d, xfersize = %d\n",
		__func__, ep_num,
		readl(&reg->in_endp[ep_num].diepdma),
		readl(&reg->in_endp[ep_num].dieptsiz),
		readl(&reg->in_endp[ep_num].diepctl),
		buf, pktcnt, length);

	return length;
}

static void complete_rx(struct cx_udc *dev, u8 ep_num)
{
	struct cx_ep *ep = &dev->ep[ep_num];
	struct cx_request *req = NULL;
	u32 ep_tsr = 0, xfer_size = 0, is_short = 0;

	if (list_empty(&ep->queue)) {
		debug_cond(DEBUG_OUT_EP != 0,
			   "%s: RX DMA done : NULL REQ on OUT EP-%d\n",
			   __func__, ep_num);
		return;

	}

	req = list_entry(ep->queue.next, struct cx_request, queue);
	ep_tsr = readl(&reg->out_endp[ep_num].doeptsiz);

	if (ep_num == EP0_CON)
		xfer_size = (ep_tsr & DOEPT_SIZ_XFER_SIZE_MAX_EP0);
	else
		xfer_size = (ep_tsr & DOEPT_SIZ_XFER_SIZE_MAX_EP);

	xfer_size = ep->len - xfer_size;

	/*
	 * NOTE:
	 *
	 * Please be careful with proper buffer allocation for USB request,
	 * which needs to be aligned to CONFIG_SYS_CACHELINE_SIZE, not only
	 * with starting address, but also its size shall be a cache line
	 * multiplication.
	 *
	 * This will prevent from corruption of data allocated immediatelly
	 * before or after the buffer.
	 *
	 * For armv7, the cache_v7.c provides proper code to emit "ERROR"
	 * message to warn users.
	 */
	invalidate_dcache_range((unsigned long) ep->dma_buf,
				(unsigned long) ep->dma_buf +
				ROUND(xfer_size, CONFIG_SYS_CACHELINE_SIZE));

	req->req.actual += min(xfer_size, req->req.length - req->req.actual);
	is_short = (xfer_size < ep->ep.maxpacket);

	debug_cond(DEBUG_OUT_EP != 0,
		   "%s: RX DMA done : ep = %d, rx bytes = %d/%d, "
		   "is_short = %d, DOEPTSIZ = 0x%x, remained bytes = %d\n",
		   __func__, ep_num, req->req.actual, req->req.length,
		   is_short, ep_tsr, xfer_size);

	if (is_short || req->req.actual == req->req.length) {
		if (ep_num == EP0_CON && dev->ep0state == DATA_STATE_RECV) {
			debug_cond(DEBUG_OUT_EP != 0, "	=> Send ZLP\n");
			cx_udc_ep0_zlp(dev);
			/* packet will be completed in complete_tx() */
			dev->ep0state = WAIT_FOR_IN_COMPLETE;
		} else {
			done(ep, req, 0);

			if (!list_empty(&ep->queue)) {
				req = list_entry(ep->queue.next,
					struct cx_request, queue);
				debug_cond(DEBUG_OUT_EP != 0,
					   "%s: Next Rx request start...\n",
					   __func__);
				setdma_rx(ep, req);
			}
		}
	} else
		setdma_rx(ep, req);
}

static void complete_tx(struct cx_udc *dev, u8 ep_num)
{
	struct cx_ep *ep = &dev->ep[ep_num];
	struct cx_request *req;
	u32 ep_tsr = 0, xfer_size = 0, is_short = 0;
	u32 last;

	if (dev->ep0state == WAIT_FOR_NULL_COMPLETE) {
		dev->ep0state = WAIT_FOR_OUT_COMPLETE;
		cx_ep0_complete_out();
		return;
	}

	if (list_empty(&ep->queue)) {
		debug_cond(DEBUG_IN_EP,
			"%s: TX DMA done : NULL REQ on IN EP-%d\n",
			__func__, ep_num);
		return;

	}

	req = list_entry(ep->queue.next, struct cx_request, queue);

	ep_tsr = readl(&reg->in_endp[ep_num].dieptsiz);

	xfer_size = ep->len;
	is_short = (xfer_size < ep->ep.maxpacket);
	req->req.actual += min(xfer_size, req->req.length - req->req.actual);

	debug_cond(DEBUG_IN_EP,
		"%s: TX DMA done : ep = %d, tx bytes = %d/%d, "
		"is_short = %d, DIEPTSIZ = 0x%x, remained bytes = %d\n",
		__func__, ep_num, req->req.actual, req->req.length,
		is_short, ep_tsr, xfer_size);

	if (ep_num == 0) {
		if (dev->ep0state == DATA_STATE_XMIT) {
			debug_cond(DEBUG_IN_EP,
				"%s: ep_num = %d, ep0stat =="
				"DATA_STATE_XMIT\n",
				__func__, ep_num);
			last = write_fifo_ep0(ep, req);
			if (last)
				dev->ep0state = WAIT_FOR_COMPLETE;
		} else if (dev->ep0state == WAIT_FOR_IN_COMPLETE) {
			debug_cond(DEBUG_IN_EP,
				"%s: ep_num = %d, completing request\n",
				__func__, ep_num);
			done(ep, req, 0);
			dev->ep0state = WAIT_FOR_SETUP;
		} else if (dev->ep0state == WAIT_FOR_COMPLETE) {
			debug_cond(DEBUG_IN_EP,
				"%s: ep_num = %d, completing request\n",
				__func__, ep_num);
			done(ep, req, 0);
			dev->ep0state = WAIT_FOR_OUT_COMPLETE;
			cx_ep0_complete_out();
		} else {
			debug_cond(DEBUG_IN_EP,
				"%s: ep_num = %d, invalid ep state\n",
				__func__, ep_num);
		}
		return;
	}

	if (req->req.actual == req->req.length)
		done(ep, req, 0);

	if (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct cx_request, queue);
		debug_cond(DEBUG_IN_EP,
			"%s: Next Tx request start...\n", __func__);
		setdma_tx(ep, req);
	}
}

static inline void cx_udc_check_tx_queue(struct cx_udc *dev, u8 ep_num)
{
	struct cx_ep *ep = &dev->ep[ep_num];
	struct cx_request *req;

	debug_cond(DEBUG_IN_EP,
		"%s: Check queue, ep_num = %d\n", __func__, ep_num);

	if (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct cx_request, queue);
		debug_cond(DEBUG_IN_EP,
			"%s: Next Tx request(0x%p) start...\n",
			__func__, req);

		if (ep_is_in(ep))
			setdma_tx(ep, req);
		else
			setdma_rx(ep, req);
	} else {
		debug_cond(DEBUG_IN_EP,
			"%s: NULL REQ on IN EP-%d\n", __func__, ep_num);

		return;
	}

}

static void process_ep_in_intr(struct cx_udc *dev)
{
	u32 ep_intr, ep_intr_status;
	u8 ep_num = 0;

	ep_intr = readl(&reg->daint);
	debug_cond(DEBUG_IN_EP,
		"*** %s: EP In interrupt : DAINT = 0x%x\n", __func__, ep_intr);

	ep_intr &= DAINT_MASK;

	while (ep_intr) {
		if (ep_intr & DAINT_IN_EP_INT(1)) {
			ep_intr_status = readl(&reg->in_endp[ep_num].diepint);
			debug_cond(DEBUG_IN_EP,
				   "\tEP%d-IN : DIEPINT = 0x%x\n",
				   ep_num, ep_intr_status);

			/* Interrupt Clear */
			writel(ep_intr_status, &reg->in_endp[ep_num].diepint);

			if (ep_intr_status & TRANSFER_DONE) {
				complete_tx(dev, ep_num);

				if (ep_num == 0) {
					if (dev->ep0state ==
					    WAIT_FOR_IN_COMPLETE)
						dev->ep0state = WAIT_FOR_SETUP;

					if (dev->ep0state == WAIT_FOR_SETUP)
						cx_udc_pre_setup();

					/* continue transfer after
					   set_clear_halt for DMA mode */
					if (clear_feature_flag == 1) {
						cx_udc_check_tx_queue(dev,
							clear_feature_num);
						clear_feature_flag = 0;
					}
				}
			}
		}
		ep_num++;
		ep_intr >>= 1;
	}
}

static void process_ep_out_intr(struct cx_udc *dev)
{
	u32 ep_intr, ep_intr_status;
	u8 ep_num = 0;

	ep_intr = readl(&reg->daint);
	debug_cond(DEBUG_OUT_EP != 0,
		   "*** %s: EP OUT interrupt : DAINT = 0x%x\n",
		   __func__, ep_intr);

	ep_intr = (ep_intr >> DAINT_OUT_BIT) & DAINT_MASK;

	while (ep_intr) {
		if (ep_intr & 0x1) {
			ep_intr_status = readl(&reg->out_endp[ep_num].doepint);
			debug_cond(DEBUG_OUT_EP != 0,
				   "\tEP%d-OUT : DOEPINT = 0x%x\n",
				   ep_num, ep_intr_status);

			/* Interrupt Clear */
			writel(ep_intr_status, &reg->out_endp[ep_num].doepint);

			if (ep_num == 0) {
				if (ep_intr_status & TRANSFER_DONE) {
					if (dev->ep0state !=
					    WAIT_FOR_OUT_COMPLETE)
						complete_rx(dev, ep_num);
					else {
						dev->ep0state = WAIT_FOR_SETUP;
						cx_udc_pre_setup();
					}
				}

				if (ep_intr_status &
				    CTRL_OUT_EP_SETUP_PHASE_DONE) {
					debug_cond(DEBUG_OUT_EP != 0,
						   "SETUP packet arrived\n");
					cx_handle_ep0(dev);
				}
			} else {
				if (ep_intr_status & TRANSFER_DONE)
					complete_rx(dev, ep_num);
			}
		}
		ep_num++;
		ep_intr >>= 1;
	}
}

int usb_gadget_cable_connected(void)                    
{
	u32 usb_status = readl(&reg->gotgctl);
	if (usb_status & (B_SESSION_VALID | A_SESSION_VALID))
		return 1;
	return 0;
}


/*
 *	usb client interrupt handler.
 */
static int cx_udc_irq(int irq, void *_dev)
{
	struct cx_udc *dev = _dev;
	u32 intr_status;
	u32 usb_status, gintmsk;

	intr_status = readl(&reg->gintsts);
	gintmsk = readl(&reg->gintmsk);

	debug_cond(DEBUG_ISR,
		  "\n*** %s : GINTSTS=0x%x(on state %s), GINTMSK : 0x%x,"
		  "DAINT : 0x%x, DAINTMSK : 0x%x\n",
		  __func__, intr_status, state_names[dev->ep0state], gintmsk,
		  readl(&reg->daint), readl(&reg->daintmsk));

	if (!intr_status) {
		return IRQ_HANDLED;
	}

	if (intr_status & INT_ENUMDONE) {
		debug_cond(DEBUG_ISR, "\tSpeed Detection interrupt\n");

		writel(INT_ENUMDONE, &reg->gintsts);
		usb_status = (readl(&reg->dsts) & 0x6);

		if (usb_status & (USB_FULL_30_60MHZ | USB_FULL_48MHZ)) {
			debug_cond(DEBUG_ISR,
				   "\t\tFull Speed Detection\n");
			set_max_pktsize(dev, USB_SPEED_FULL);

		} else {
			debug_cond(DEBUG_ISR,
				"\t\tHigh Speed Detection : 0x%x\n",
				usb_status);
			set_max_pktsize(dev, USB_SPEED_HIGH);
		}
	}

	if (intr_status & INT_EARLY_SUSPEND) {
		debug_cond(DEBUG_ISR, "\tEarly suspend interrupt\n");
		writel(INT_EARLY_SUSPEND, &reg->gintsts);
	}

	debug_cond(DEBUG_ISR, "doepint0 = 0x%08x\n", 
		readl(&reg->out_endp[0].doepint));

	if (intr_status & INT_SUSPEND) {
		usb_status = readl(&reg->dsts);
		debug_cond(DEBUG_ISR,
			"\tSuspend interrupt :(DSTS):0x%x\n", usb_status);
		writel(INT_SUSPEND, &reg->gintsts);

		if (dev->gadget.speed != USB_SPEED_UNKNOWN
		    && dev->driver) {
			if (dev->driver->suspend)
				dev->driver->suspend(&dev->gadget);

			/* HACK to let gadget detect disconnected state */
			if (dev->driver->disconnect) {
				dev->driver->disconnect(&dev->gadget);
			}
		}
	}

	if (intr_status & INT_RESUME) {
		debug_cond(DEBUG_ISR, "\tResume interrupt\n");
		writel(INT_RESUME, &reg->gintsts);

		if (dev->gadget.speed != USB_SPEED_UNKNOWN
		    && dev->driver
		    && dev->driver->resume) {

			dev->driver->resume(&dev->gadget);
		}
	}

	if (intr_status & INT_RESET) {
		usb_status = readl(&reg->gotgctl);
		debug_cond(DEBUG_ISR,
			"\tReset interrupt - (GOTGCTL):0x%x\n", usb_status);
		writel(INT_RESET, &reg->gintsts);

		if ((usb_status & 0xc0000) == (0x3 << 18)) {
			if (reset_available) {
				debug_cond(DEBUG_ISR,
					"\t\tOTG core got reset (%d)...\n",
					reset_available);
				reconfig_usbd();
				dev->ep0state = WAIT_FOR_SETUP;
				reset_available = 0;
				cx_udc_pre_setup();
			} else
				reset_available = 1;

		} else {
			reset_available = 1;
			debug_cond(DEBUG_ISR,
				   "\t\tRESET handling skipped\n");
		}
	}

	if (intr_status & INT_IN_EP)
		process_ep_in_intr(dev);

	if (intr_status & INT_OUT_EP)
		process_ep_out_intr(dev);

	return IRQ_HANDLED;
}

/** Queue one request
 *  Kickstart transfer if needed
 */
static int cx_queue(struct usb_ep *_ep, struct usb_request *_req,
			 gfp_t gfp_flags)
{
	struct cx_request *req;
	struct cx_ep *ep;
	struct cx_udc *dev;
	u32 ep_num, gintsts;

	req = container_of(_req, struct cx_request, req);
	if (unlikely(!_req || !_req->complete || !_req->buf
		     || !list_empty(&req->queue))) {

		debug("%s: bad params\n", __func__);
		return -EINVAL;
	}

	ep = container_of(_ep, struct cx_ep, ep);

	if (unlikely(!_ep || (!ep->desc && ep->ep.name != ep0name))) {

		debug("%s: bad ep: %s, %d, %p\n", __func__,
		      ep->ep.name, !ep->desc, _ep);
		return -EINVAL;
	}

	ep_num = ep_index(ep);
	dev = ep->dev;
	if (unlikely(!dev->driver || dev->gadget.speed == USB_SPEED_UNKNOWN)) {

		debug("%s: bogus device state %p\n", __func__, dev->driver);
		return -ESHUTDOWN;
	}

	_req->status = -EINPROGRESS;
	_req->actual = 0;

	/* kickstart this i/o queue? */
	debug_cond(DEBUG_QUEUE, "\n*** %s: %s-%s req = %p, len = %d, buf = %p"
		"Q empty = %d, stopped = %d\n",
		__func__, _ep->name, ep_is_in(ep) ? "in" : "out",
		_req, _req->length, _req->buf,
		list_empty(&ep->queue), ep->stopped);

#if  DEBUG_RT
	{
		int i, len = _req->length;

		printf("pkt = ");
		if (len > 64)
			len = 64;
		for (i = 0; i < len; i++) {
			printf("%02x", ((u8 *)_req->buf)[i]);
			if ((i & 7) == 7)
				printf(" ");
		}
		printf("\n");
	}
#endif

	if (list_empty(&ep->queue) && !ep->stopped) {

		if (ep_num == 0) {
			/* EP0 */
			list_add_tail(&req->queue, &ep->queue);
			cx_ep0_kick(dev, ep);
			req = 0;

		} else if (ep_is_in(ep)) {
			gintsts = readl(&reg->gintsts);
			debug_cond(DEBUG_IN_EP,
				   "%s: ep_is_in, CX_UDC_OTG_GINTSTS=0x%x\n",
				   __func__, gintsts);

			setdma_tx(ep, req);
		} else {
			gintsts = readl(&reg->gintsts);
			debug_cond(DEBUG_OUT_EP != 0,
				   "%s:ep_is_out, CX_UDC_OTG_GINTSTS=0x%x\n",
				   __func__, gintsts);

			setdma_rx(ep, req);
		}
	}

	/* pio or dma irq handler advances the queue. */
	if (likely(req != 0))
		list_add_tail(&req->queue, &ep->queue);

	return 0;
}

/****************************************************************/
/* End Point 0 related functions                                */
/****************************************************************/

/* return:  0 = still running, 1 = completed, negative = errno */
static int write_fifo_ep0(struct cx_ep *ep, struct cx_request *req)
{
	u32 max;
	unsigned count;
	int is_last;

	max = ep_maxpacket(ep);

	debug_cond(DEBUG_EP0 != 0, "%s: max = %d\n", __func__, max);

	count = setdma_tx(ep, req);

	/* last packet is usually short (or a zlp) */
	if (likely(count != max))
		is_last = 1;
	else {
		if (likely(req->req.length != req->req.actual + count)
		    || req->req.zero)
			is_last = 0;
		else
			is_last = 1;
	}

	debug_cond(DEBUG_EP0 != 0,
		   "%s: wrote %s %d bytes%s %d left %p\n", __func__,
		   ep->ep.name, count,
		   is_last ? "/L" : "",
		   req->req.length - req->req.actual - count, req);

	/* requests complete when all IN data is in the FIFO */
	if (is_last) {
		ep->dev->ep0state = WAIT_FOR_SETUP;
		return 1;
	}

	return 0;
}

int cx_fifo_read(struct cx_ep *ep, u32 *cp, int max)
{
	invalidate_dcache_range((unsigned long)cp, (unsigned long)cp +
				ROUND(max, CONFIG_SYS_CACHELINE_SIZE));

	debug_cond(DEBUG_EP0 != 0,
		   "%s: bytes=%d, ep_index=%d 0x%p\n", __func__,
		   max, ep_index(ep), cp);

	return max;
}

/**
 * udc_set_address - set the USB address for this device
 * @address:
 *
 * Called from control endpoint function
 * after it decodes a set address setup packet.
 */
static void udc_set_address(struct cx_udc *dev, unsigned char address)
{
	u32 ctrl = readl(&reg->dcfg);
	writel(DEVICE_ADDRESS(address) | ctrl, &reg->dcfg);

	cx_udc_ep0_zlp(dev);

	debug_cond(DEBUG_EP0 != 0,
		   "%s: USB OTG 2.0 Device address=%d, DCFG=0x%x\n",
		   __func__, address, readl(&reg->dcfg));

	dev->usb_address = address;
}

static inline void cx_udc_ep0_set_stall(struct cx_ep *ep)
{
	struct cx_udc *dev;
	u32		ep_ctrl = 0;

	dev = ep->dev;
	ep_ctrl = readl(&reg->in_endp[EP0_CON].diepctl);

	/* set the disable and stall bits */
	if (ep_ctrl & DEPCTL_EPENA)
		ep_ctrl |= DEPCTL_EPDIS;

	ep_ctrl |= DEPCTL_STALL;

	writel(ep_ctrl, &reg->in_endp[EP0_CON].diepctl);

	debug_cond(DEBUG_EP0 != 0,
		   "%s: set ep%d stall, DIEPCTL0 = 0x%p\n",
		   __func__, ep_index(ep), &reg->in_endp[EP0_CON].diepctl);
	/*
	 * The application can only set this bit, and the core clears it,
	 * when a SETUP token is received for this endpoint
	 */
	dev->ep0state = WAIT_FOR_SETUP;

	cx_udc_pre_setup();
}

static void cx_ep0_read(struct cx_udc *dev)
{
	struct cx_request *req;
	struct cx_ep *ep = &dev->ep[0];

	if (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct cx_request, queue);

	} else {
		debug("%s: ---> BUG\n", __func__);
		BUG();
		return;
	}

	debug_cond(DEBUG_EP0 != 0,
		   "%s: req = %p, req.length = 0x%x, req.actual = 0x%x\n",
		   __func__, req, req->req.length, req->req.actual);

	if (req->req.length == 0) {
		/* zlp for Set_configuration, Set_interface,
		 * or Bulk-Only mass storge reset */

		ep->len = 0;
		cx_udc_ep0_zlp(dev);

		debug_cond(DEBUG_EP0 != 0,
			   "%s: req.length = 0, bRequest = %d\n",
			   __func__, usb_ctrl->bRequest);
		return;
	}

	setdma_rx(ep, req);
}

/*
 * DATA_STATE_XMIT
 */
static int cx_ep0_write(struct cx_udc *dev)
{
	struct cx_request *req;
	struct cx_ep *ep = &dev->ep[0];
	int ret, need_zlp = 0;

	if (list_empty(&ep->queue))
		req = 0;
	else
		req = list_entry(ep->queue.next, struct cx_request, queue);

	if (!req) {
		debug_cond(DEBUG_EP0 != 0, "%s: NULL REQ\n", __func__);
		return 0;
	}

	debug_cond(DEBUG_EP0 != 0,
		   "%s: req = %p, req.length = 0x%x, req.actual = 0x%x\n",
		   __func__, req, req->req.length, req->req.actual);

	if (req->req.length - req->req.actual == ep0_fifo_size) {
		/* Next write will end with the packet size, */
		/* so we need Zero-length-packet */
		need_zlp = 1;
	}

	ret = write_fifo_ep0(ep, req);

	if ((ret == 1) && !need_zlp) {
		/* Last packet */
		dev->ep0state = WAIT_FOR_COMPLETE;
		debug_cond(DEBUG_EP0 != 0,
			   "%s: finished, waiting for status\n", __func__);

	} else {
		dev->ep0state = DATA_STATE_XMIT;
		debug_cond(DEBUG_EP0 != 0,
			   "%s: not finished\n", __func__);
	}

	return 1;
}

int cx_udc_get_status(struct cx_udc *dev,
		struct usb_ctrlrequest *crq)
{
	u8 ep_num = crq->wIndex & 0x7F;
	u16 g_status = 0;
	u32 ep_ctrl;

	debug_cond(DEBUG_SETUP != 0,
		   "%s: *** USB_REQ_GET_STATUS\n", __func__);
	printf("crq->brequest:0x%x\n", crq->bRequestType & USB_RECIP_MASK);
	switch (crq->bRequestType & USB_RECIP_MASK) {
	case USB_RECIP_INTERFACE:
		g_status = 0;
		debug_cond(DEBUG_SETUP != 0,
			   "\tGET_STATUS:USB_RECIP_INTERFACE, g_stauts = %d\n",
			   g_status);
		break;

	case USB_RECIP_DEVICE:
		g_status = 0x1; /* Self powered */
		debug_cond(DEBUG_SETUP != 0,
			   "\tGET_STATUS: USB_RECIP_DEVICE, g_stauts = %d\n",
			   g_status);
		break;

	case USB_RECIP_ENDPOINT:
		if (crq->wLength > 2) {
			debug_cond(DEBUG_SETUP != 0,
				   "\tGET_STATUS:Not support EP or wLength\n");
			return 1;
		}

		g_status = dev->ep[ep_num].stopped;
		debug_cond(DEBUG_SETUP != 0,
			   "\tGET_STATUS: USB_RECIP_ENDPOINT, g_stauts = %d\n",
			   g_status);

		break;

	default:
		return 1;
	}

	memcpy(usb_ctrl, &g_status, sizeof(g_status));

	flush_dcache_range((unsigned long) usb_ctrl,
			   (unsigned long) usb_ctrl +
			   ROUND(sizeof(g_status), CONFIG_SYS_CACHELINE_SIZE));

	writel(usb_ctrl_dma_addr, &reg->in_endp[EP0_CON].diepdma);
	writel(DIEPT_SIZ_PKT_CNT(1) | DIEPT_SIZ_XFER_SIZE(2),
	       &reg->in_endp[EP0_CON].dieptsiz);

	ep_ctrl = readl(&reg->in_endp[EP0_CON].diepctl);
	writel(ep_ctrl|DEPCTL_EPENA|DEPCTL_CNAK,
	       &reg->in_endp[EP0_CON].diepctl);
	dev->ep0state = WAIT_FOR_NULL_COMPLETE;

	return 0;
}

static void cx_udc_set_nak(struct cx_ep *ep)
{
	u8		ep_num;
	u32		ep_ctrl = 0;

	ep_num = ep_index(ep);
	debug_cond(DEBUG_RT, "%s: ep_num = %d, ep_type = %d\n", __func__, ep_num, ep->ep_type);

	if (ep_is_in(ep)) {
		ep_ctrl = readl(&reg->in_endp[ep_num].diepctl);
		ep_ctrl |= DEPCTL_SNAK;
		writel(ep_ctrl, &reg->in_endp[ep_num].diepctl);
		debug_cond(DEBUG_RT, "%s: set NAK, DIEPCTL%d = 0x%x\n",
			__func__, ep_num, readl(&reg->in_endp[ep_num].diepctl));
	} else {
		ep_ctrl = readl(&reg->out_endp[ep_num].doepctl);
		ep_ctrl |= DEPCTL_SNAK;
		writel(ep_ctrl, &reg->out_endp[ep_num].doepctl);
		debug_cond(DEBUG_RT, "%s: set NAK, DOEPCTL%d = 0x%x\n",
		      __func__, ep_num, readl(&reg->out_endp[ep_num].doepctl));
	}

	return;
}


void cx_udc_ep_set_stall(struct cx_ep *ep)
{
	u8		ep_num;
	u32		ep_ctrl = 0;

	ep_num = ep_index(ep);
	debug_cond(DEBUG_RT, "%s: ep_num = %d, ep_type = %d\n", __func__, ep_num, ep->ep_type);

	if (ep_is_in(ep)) {
		ep_ctrl = readl(&reg->in_endp[ep_num].diepctl);

		/* set the disable and stall bits */
		if (ep_ctrl & DEPCTL_EPENA)
			ep_ctrl |= DEPCTL_EPDIS;

		ep_ctrl |= DEPCTL_STALL;

		writel(ep_ctrl, &reg->in_endp[ep_num].diepctl);
		debug_cond(DEBUG_RT, "%s: set stall, DIEPCTL%d = 0x%x\n",
		      __func__, ep_num, readl(&reg->in_endp[ep_num].diepctl));

	} else {
		ep_ctrl = readl(&reg->out_endp[ep_num].doepctl);

		/* set the stall bit */
		ep_ctrl |= DEPCTL_STALL;

		writel(ep_ctrl, &reg->out_endp[ep_num].doepctl);
		debug_cond(DEBUG_RT, "%s: set stall, DOEPCTL%d = 0x%x\n",
		      __func__, ep_num, readl(&reg->out_endp[ep_num].doepctl));
	}

	return;
}

void cx_udc_ep_clear_stall(struct cx_ep *ep)
{
	u8		ep_num;
	u32		ep_ctrl = 0;

	ep_num = ep_index(ep);
	debug_cond(DEBUG_RT, "%s: ep_num = %d, ep_type = %d\n", __func__, ep_num, ep->ep_type);

	if (ep_is_in(ep)) {
		ep_ctrl = readl(&reg->in_endp[ep_num].diepctl);

		/* clear stall bit */
		ep_ctrl &= ~DEPCTL_STALL;

		/*
		 * USB Spec 9.4.5: For endpoints using data toggle, regardless
		 * of whether an endpoint has the Halt feature set, a
		 * ClearFeature(ENDPOINT_HALT) request always results in the
		 * data toggle being reinitialized to DATA0.
		 */
		if (ep->bmAttributes == USB_ENDPOINT_XFER_INT
		    || ep->bmAttributes == USB_ENDPOINT_XFER_BULK) {
			ep_ctrl |= DEPCTL_SETD0PID; /* DATA0 */
		}

		writel(ep_ctrl, &reg->in_endp[ep_num].diepctl);
		debug_cond(DEBUG_RT, "%s: cleared stall, DIEPCTL%d = 0x%x\n",
			__func__, ep_num, readl(&reg->in_endp[ep_num].diepctl));

	} else {
		ep_ctrl = readl(&reg->out_endp[ep_num].doepctl);

		/* clear stall bit */
		ep_ctrl &= ~DEPCTL_STALL;

		if (ep->bmAttributes == USB_ENDPOINT_XFER_INT
		    || ep->bmAttributes == USB_ENDPOINT_XFER_BULK) {
			ep_ctrl |= DEPCTL_SETD0PID; /* DATA0 */
		}

		writel(ep_ctrl, &reg->out_endp[ep_num].doepctl);
		debug_cond(DEBUG_RT, "%s: cleared stall, DOEPCTL%d = 0x%x\n",
		      __func__, ep_num, readl(&reg->out_endp[ep_num].doepctl));
	}

	return;
}

static int cx_udc_set_halt(struct usb_ep *_ep, int value)
{
	struct cx_ep	*ep;
	struct cx_udc	*dev;
	u8		ep_num;

	ep = container_of(_ep, struct cx_ep, ep);
	ep_num = ep_index(ep);

	if (unlikely(!_ep || !ep->desc || ep_num == EP0_CON ||
		     ep->desc->bmAttributes == USB_ENDPOINT_XFER_ISOC)) {
		debug("%s: %s bad ep or descriptor\n", __func__, ep->ep.name);
		return -EINVAL;
	}

	/* Attempt to halt IN ep will fail if any transfer requests
	 * are still queue */
	if (value && ep_is_in(ep) && !list_empty(&ep->queue)) {
		debug("%s: %s queue not empty, req = %p\n",
			__func__, ep->ep.name,
			list_entry(ep->queue.next, struct cx_request, queue));

		return -EAGAIN;
	}

	dev = ep->dev;
	debug_cond(DEBUG_RT, "%s: ep_num = %d, value = %d\n", __func__, ep_num, value);

	if (value == 0) {
		ep->stopped = 0;
		cx_udc_ep_clear_stall(ep);
	} else {
		if (ep_num == 0)
			dev->ep0state = WAIT_FOR_SETUP;

		ep->stopped = 1;
		cx_udc_ep_set_stall(ep);
	}

	return 0;
}

void cx_udc_ep_activate(struct cx_ep *ep)
{
	u8 ep_num;
	u32 ep_ctrl = 0, daintmsk = 0;

	ep_num = ep_index(ep);

	/* Read DEPCTLn register */
	if (ep_is_in(ep)) {
		ep_ctrl = readl(&reg->in_endp[ep_num].diepctl);
		daintmsk = 1 << ep_num;
	} else {
		ep_ctrl = readl(&reg->out_endp[ep_num].doepctl);
		daintmsk = (1 << ep_num) << DAINT_OUT_BIT;
	}

	debug_cond(DEBUG_RT, "%s: EPCTRL%d = 0x%x, ep_is_in = %d\n",
		__func__, ep_num, ep_ctrl, ep_is_in(ep));

	/* If the EP is already active don't change the EP Control
	 * register. */
	if (!(ep_ctrl & DEPCTL_USBACTEP)) {
		ep_ctrl = (ep_ctrl & ~DEPCTL_TYPE_MASK) |
			(ep->bmAttributes << DEPCTL_TYPE_BIT);
		ep_ctrl = (ep_ctrl & ~DEPCTL_MPS_MASK) |
			(ep->ep.maxpacket << DEPCTL_MPS_BIT);
		ep_ctrl |= (DEPCTL_SETD0PID | DEPCTL_USBACTEP | DEPCTL_SNAK);

		if (ep_is_in(ep)) {
			writel(ep_ctrl, &reg->in_endp[ep_num].diepctl);
			debug_cond(DEBUG_RT, "%s: USB Ative EP%d, DIEPCTRL%d = 0x%x\n",
			      __func__, ep_num, ep_num,
			      readl(&reg->in_endp[ep_num].diepctl));
		} else {
			writel(ep_ctrl, &reg->out_endp[ep_num].doepctl);
			debug_cond(DEBUG_RT, "%s: USB Ative EP%d, DOEPCTRL%d = 0x%x\n",
			      __func__, ep_num, ep_num,
			      readl(&reg->out_endp[ep_num].doepctl));
		}
	}

	/* Unmask EP Interrtupt */
	writel(readl(&reg->daintmsk)|daintmsk, &reg->daintmsk);
	debug_cond(DEBUG_RT, "%s: DAINTMSK = 0x%x\n", __func__, readl(&reg->daintmsk));

}

static int cx_udc_clear_feature(struct usb_ep *_ep)
{
	struct cx_udc	*dev;
	struct cx_ep	*ep;
	u8		ep_num;

	ep = container_of(_ep, struct cx_ep, ep);
	ep_num = ep_index(ep);

	dev = ep->dev;
	debug_cond(DEBUG_SETUP != 0,
		   "%s: ep_num = %d, is_in = %d, clear_feature_flag = %d\n",
		   __func__, ep_num, ep_is_in(ep), clear_feature_flag);

	if (usb_ctrl->wLength != 0) {
		debug_cond(DEBUG_SETUP != 0,
			   "\tCLEAR_FEATURE: wLength is not zero.....\n");
		return 1;
	}

	switch (usb_ctrl->bRequestType & USB_RECIP_MASK) {
	case USB_RECIP_DEVICE:
		switch (usb_ctrl->wValue) {
		case USB_DEVICE_REMOTE_WAKEUP:
			debug_cond(DEBUG_SETUP != 0,
				   "\tOFF:USB_DEVICE_REMOTE_WAKEUP\n");
			break;

		case USB_DEVICE_TEST_MODE:
			debug_cond(DEBUG_SETUP != 0,
				   "\tCLEAR_FEATURE: USB_DEVICE_TEST_MODE\n");
			/** @todo Add CLEAR_FEATURE for TEST modes. */
			break;
		}

		cx_udc_ep0_zlp(dev);
		break;

	case USB_RECIP_ENDPOINT:
		debug_cond(DEBUG_SETUP != 0,
			   "\tCLEAR_FEATURE:USB_RECIP_ENDPOINT, wValue = %d\n",
			   usb_ctrl->wValue);

		if (usb_ctrl->wValue == USB_ENDPOINT_HALT) {
			if (ep_num == 0) {
				cx_udc_ep0_set_stall(ep);
				return 0;
			}

			cx_udc_ep0_zlp(dev);

			cx_udc_ep_clear_stall(ep);
			cx_udc_ep_activate(ep);
			ep->stopped = 0;

			clear_feature_num = ep_num;
			clear_feature_flag = 1;
		}
		break;
	}

	return 0;
}

static int cx_udc_set_feature(struct usb_ep *_ep)
{
	struct cx_udc	*dev;
	struct cx_ep	*ep;
	u8		ep_num;

	ep = container_of(_ep, struct cx_ep, ep);
	ep_num = ep_index(ep);
	dev = ep->dev;

	debug_cond(DEBUG_SETUP != 0,
		   "%s: *** USB_REQ_SET_FEATURE , ep_num = %d\n",
		    __func__, ep_num);

	if (usb_ctrl->wLength != 0) {
		debug_cond(DEBUG_SETUP != 0,
			   "\tSET_FEATURE: wLength is not zero.....\n");
		return 1;
	}

	switch (usb_ctrl->bRequestType & USB_RECIP_MASK) {
	case USB_RECIP_DEVICE:
		switch (usb_ctrl->wValue) {
		case USB_DEVICE_REMOTE_WAKEUP:
			debug_cond(DEBUG_SETUP != 0,
				   "\tSET_FEATURE:USB_DEVICE_REMOTE_WAKEUP\n");
			break;
		case USB_DEVICE_B_HNP_ENABLE:
			debug_cond(DEBUG_SETUP != 0,
				   "\tSET_FEATURE: USB_DEVICE_B_HNP_ENABLE\n");
			break;

		case USB_DEVICE_A_HNP_SUPPORT:
			/* RH port supports HNP */
			debug_cond(DEBUG_SETUP != 0,
				   "\tSET_FEATURE:USB_DEVICE_A_HNP_SUPPORT\n");
			break;

		case USB_DEVICE_A_ALT_HNP_SUPPORT:
			/* other RH port does */
			debug_cond(DEBUG_SETUP != 0,
				   "\tSET: USB_DEVICE_A_ALT_HNP_SUPPORT\n");
			break;
		}

		cx_udc_ep0_zlp(dev);
		return 0;

	case USB_RECIP_INTERFACE:
		debug_cond(DEBUG_SETUP != 0,
			   "\tSET_FEATURE: USB_RECIP_INTERFACE\n");
		break;

	case USB_RECIP_ENDPOINT:
		debug_cond(DEBUG_SETUP != 0,
			   "\tSET_FEATURE: USB_RECIP_ENDPOINT\n");
		if (usb_ctrl->wValue == USB_ENDPOINT_HALT) {
			if (ep_num == 0) {
				cx_udc_ep0_set_stall(ep);
				return 0;
			}
			ep->stopped = 1;
			cx_udc_ep_set_stall(ep);
		}

		cx_udc_ep0_zlp(dev);
		return 0;
	}

	return 1;
}

/*
 * WAIT_FOR_SETUP (OUT_PKT_RDY)
 */
void cx_ep0_setup(struct cx_udc *dev)
{
	struct cx_ep *ep = &dev->ep[0];
	int i;
	u8 ep_num;

	/* Nuke all previous transfers */
	nuke(ep, -EPROTO);

	/* read control req from fifo (8 bytes) */
	cx_fifo_read(ep, (u32 *)usb_ctrl, 8);

	debug_cond(DEBUG_SETUP != 0,
		   "%s: bRequestType = 0x%x(%s), bRequest = 0x%x"
		   "\twLength = 0x%x, wValue = 0x%x, wIndex= 0x%x\n",
		   __func__, usb_ctrl->bRequestType,
		   (usb_ctrl->bRequestType & USB_DIR_IN) ? "IN" : "OUT",
		   usb_ctrl->bRequest,
		   usb_ctrl->wLength, usb_ctrl->wValue, usb_ctrl->wIndex);

#if DEBUG_RT
	{
		int i, len = sizeof(*usb_ctrl);
		char *p = (char *)usb_ctrl;

		printf("pkt = ");
		for (i = 0; i < len; i++) {
			printf("%02x", ((u8 *)p)[i]);
			if ((i & 7) == 7)
				printf(" ");
		}
		printf("\n");
	}
#endif

	if (usb_ctrl->bRequest == GET_MAX_LUN_REQUEST &&
	    usb_ctrl->wLength != 1) {
		debug_cond(DEBUG_SETUP != 0,
			   "\t%s:GET_MAX_LUN_REQUEST:invalid",
			   __func__);
		debug_cond(DEBUG_SETUP != 0,
			   "wLength = %d, setup returned\n",
			   usb_ctrl->wLength);

		cx_udc_ep0_set_stall(ep);
		dev->ep0state = WAIT_FOR_SETUP;

		return;
	} else if (usb_ctrl->bRequest == BOT_RESET_REQUEST &&
		 usb_ctrl->wLength != 0) {
		/* Bulk-Only *mass storge reset of class-specific request */
		debug_cond(DEBUG_SETUP != 0,
			   "%s:BOT Rest:invalid wLength =%d, setup returned\n",
			   __func__, usb_ctrl->wLength);

		cx_udc_ep0_set_stall(ep);
		dev->ep0state = WAIT_FOR_SETUP;

		return;
	}

	/* Set direction of EP0 */
	if (likely(usb_ctrl->bRequestType & USB_DIR_IN)) {
		ep->bEndpointAddress |= USB_DIR_IN;
	} else {
		ep->bEndpointAddress &= ~USB_DIR_IN;
	}
	/* cope with automagic for some standard requests. */
	dev->req_std = (usb_ctrl->bRequestType & USB_TYPE_MASK)
		== USB_TYPE_STANDARD;

	dev->req_pending = 1;

	/* Handle some SETUP packets ourselves */
	if (dev->req_std) {
		switch (usb_ctrl->bRequest) {
		case USB_REQ_SET_ADDRESS:
		debug_cond(DEBUG_SETUP != 0,
			   "%s: *** USB_REQ_SET_ADDRESS (%d)\n",
			   __func__, usb_ctrl->wValue);
			if (usb_ctrl->bRequestType
				!= (USB_TYPE_STANDARD | USB_RECIP_DEVICE))
				break;

			udc_set_address(dev, usb_ctrl->wValue);
			return;

		case USB_REQ_SET_CONFIGURATION:
			debug_cond(DEBUG_SETUP != 0,
				   "=====================================\n");
			debug_cond(DEBUG_SETUP != 0,
				   "%s: USB_REQ_SET_CONFIGURATION (%d)\n",
				   __func__, usb_ctrl->wValue);

			if (usb_ctrl->bRequestType == USB_RECIP_DEVICE)
				reset_available = 1;

			break;

		case USB_REQ_GET_DESCRIPTOR:
			debug_cond(DEBUG_SETUP != 0,
				   "%s: *** USB_REQ_GET_DESCRIPTOR\n",
				   __func__);
			break;

		case USB_REQ_SET_INTERFACE:
			debug_cond(DEBUG_SETUP != 0,
				   "%s: *** USB_REQ_SET_INTERFACE (%d)\n",
				   __func__, usb_ctrl->wValue);

			if (usb_ctrl->bRequestType == USB_RECIP_INTERFACE)
				reset_available = 1;

			break;

		case USB_REQ_GET_CONFIGURATION:
			debug_cond(DEBUG_SETUP != 0,
				   "%s: *** USB_REQ_GET_CONFIGURATION\n",
				   __func__);
			break;

		case USB_REQ_GET_STATUS:
			if (!cx_udc_get_status(dev, usb_ctrl))
				return;

			break;

		case USB_REQ_CLEAR_FEATURE:
			ep_num = usb_ctrl->wIndex & 0x7f;

			if (!cx_udc_clear_feature(&dev->ep[ep_num].ep))
				return;

			break;

		case USB_REQ_SET_FEATURE:
			ep_num = usb_ctrl->wIndex & 0x7f;

			if (!cx_udc_set_feature(&dev->ep[ep_num].ep))
				return;

			break;

		default:
			debug_cond(DEBUG_SETUP != 0,
				   "%s: *** Default of usb_ctrl->bRequest=0x%x"
				   "happened.\n", __func__, usb_ctrl->bRequest);
			break;
		}
	}


	if (likely(dev->driver)) {
		/* device-2-host (IN) or no data setup command,
		 * process immediately */
		debug_cond(DEBUG_SETUP != 0,
			   "%s:usb_ctrlreq will be passed to composite_setup()\n",
			    __func__);

		i = dev->driver->setup(&dev->gadget, usb_ctrl);

		if (i < 0) {
			/* setup processing failed, force stall */
			cx_udc_ep0_set_stall(ep);
			dev->ep0state = WAIT_FOR_SETUP;

			debug_cond(DEBUG_SETUP != 0,
				   "\tdev->driver->setup failed (%d),"
				    " bRequest = %d\n",
				i, usb_ctrl->bRequest);


		} else if (dev->req_pending) {
			dev->req_pending = 0;
			debug_cond(DEBUG_SETUP != 0,
				   "\tdev->req_pending...\n");
		}

		debug_cond(DEBUG_SETUP != 0,
			   "\tep0state = %s\n", state_names[dev->ep0state]);

	}
}

/*
 * handle ep0 interrupt
 */
static void cx_handle_ep0(struct cx_udc *dev)
{
	if (dev->ep0state == WAIT_FOR_SETUP) {
		debug_cond(DEBUG_OUT_EP != 0,
			   "%s: WAIT_FOR_SETUP\n", __func__);
		cx_ep0_setup(dev);

	} else {
		debug_cond(DEBUG_OUT_EP != 0,
			   "%s: strange state!!(state = %s)\n",
			__func__, state_names[dev->ep0state]);
	}
}

static void cx_ep0_kick(struct cx_udc *dev, struct cx_ep *ep)
{
	debug_cond(DEBUG_EP0 != 0,
		   "%s: ep_is_in = %d\n", __func__, ep_is_in(ep));
	if (ep_is_in(ep)) {
		dev->ep0state = DATA_STATE_XMIT;
		cx_ep0_write(dev);

	} else {
		dev->ep0state = DATA_STATE_RECV;
		cx_ep0_read(dev);
	}
}

void otg_phy_init(struct cx_udc *dev)
{
	debug_cond(DEBUG_RT, "USB PHY0 Enable\n");
}

void otg_phy_off(struct cx_udc *dev)
{
	debug_cond(DEBUG_RT, "USB PHY0 Disable\n");
}

/*
 *	udc_disable - disable USB device controller
 */
static void udc_disable(struct cx_udc *dev)
{
	debug_cond(DEBUG_SETUP != 0, "%s: %p\n", __func__, dev);

	udc_set_address(dev, 0);

	dev->ep0state = WAIT_FOR_SETUP;
	dev->gadget.speed = USB_SPEED_UNKNOWN;
	dev->usb_address = 0;

	otg_phy_off(dev);
}

/*
 *	udc_reinit - initialize software state
 */
static void udc_reinit(struct cx_udc *dev)
{
	unsigned int i;

	debug_cond(DEBUG_SETUP != 0, "%s: %p\n", __func__, dev);

	/* device/ep0 records init */
	INIT_LIST_HEAD(&dev->gadget.ep_list);
	INIT_LIST_HEAD(&dev->gadget.ep0->ep_list);
	dev->ep0state = WAIT_FOR_SETUP;

	/* basic endpoint records init */
	for (i = 0; i < CX_MAX_ENDPOINTS; i++) {
		struct cx_ep *ep = &dev->ep[i];

		if (i != 0)
			list_add_tail(&ep->ep.ep_list, &dev->gadget.ep_list);

		ep->desc = 0;
		ep->stopped = 0;
		INIT_LIST_HEAD(&ep->queue);
		ep->pio_irqs = 0;
	}

	/* the rest was statically initialized, and is read-only */
}

#define BYTES2MAXP(x)	(x / 8)
#define MAXP2BYTES(x)	(x * 8)

/* until it's enabled, this UDC should be completely invisible
 * to any USB host.
 */
static int udc_enable(struct cx_udc *dev)
{
	debug_cond(DEBUG_SETUP != 0, "%s: %p\n", __func__, dev);

	otg_phy_init(dev);
	reconfig_usbd();

	debug_cond(DEBUG_SETUP != 0,
		   "CX USB 2.0 OTG Controller Core Initialized : 0x%x\n",
		    readl(&reg->gintmsk));

	dev->gadget.speed = USB_SPEED_UNKNOWN;

	return 0;
}

/*
  Register entry point for the peripheral controller driver.
*/
int usb_gadget_register_driver(struct usb_gadget_driver *driver)
{
	struct cx_udc *dev = the_controller;
	int retval = 0;

	debug_cond(DEBUG_SETUP != 0, "%s\n", __func__);

	if (!driver
	    || (driver->speed != USB_SPEED_FULL
		&& driver->speed != USB_SPEED_HIGH)
	    || !driver->bind || !driver->disconnect || !driver->setup)
		return -EINVAL;
	if (!dev)
		return -ENODEV;
	if (dev->driver)
		return -EBUSY;

	/* first hook up the driver ... */
	dev->driver = driver;

	if (retval) { /* TODO */
		printf("target device_add failed, error %d\n", retval);
		return retval;
	}

	retval = driver->bind(&dev->gadget);
	if (retval) {
		debug_cond(DEBUG_SETUP != 0,
			   "%s: bind to driver --> error %d\n",
			    dev->gadget.name, retval);
		dev->driver = 0;
		return retval;
	}

	debug_cond(DEBUG_SETUP != 0,
		   "registered gadget driver %s\n", dev->gadget.name);
	udc_enable(dev);

	return 0;
}

/*
 * Unregister entry point for the peripheral controller driver.
 */
int usb_gadget_unregister_driver(struct usb_gadget_driver *driver)
{
	struct cx_udc *dev = the_controller;

	if (!dev)
		return -ENODEV;
	if (!driver || driver != dev->driver)
		return -EINVAL;

	dev->driver = 0;
	stop_activity(dev, driver);

	driver->unbind(&dev->gadget);

	udc_disable(dev);
	return 0;
}

/*
 *	done - retire a request; caller blocked irqs
 */
static void done(struct cx_ep *ep, struct cx_request *req, int status)
{
	unsigned int stopped = ep->stopped;

	debug_cond(DEBUG_RT, "%s: %s %p, req = %p, stopped = %d\n",
	      __func__, ep->ep.name, ep, &req->req, stopped);

	list_del_init(&req->queue);

	if (likely(req->req.status == -EINPROGRESS))
		req->req.status = status;
	else
		status = req->req.status;

	if (status && status != -ESHUTDOWN) {
		debug("complete %s req %p stat %d len %u/%u\n",
		      ep->ep.name, &req->req, status,
		      req->req.actual, req->req.length);
	}

	/* don't modify queue heads during completion callback */
	ep->stopped = 1;

#if DEBUG_RT
	printf("calling complete callback\n");
	{
		int i, len = req->req.length;

		printf("pkt[%d] = ", req->req.length);
		if (len > 64)
			len = 64;
		for (i = 0; i < len; i++) {
			printf("%02x", ((u8 *)req->req.buf)[i]);
			if ((i & 7) == 7)
				printf(" ");
		}
		printf("\n");
	}
#endif
	req->req.complete(&ep->ep, &req->req);

	debug_cond(DEBUG_RT, "callback completed\n");

	ep->stopped = stopped;
}

/*
 *	nuke - dequeue ALL requests
 */
static void nuke(struct cx_ep *ep, int status)
{
	struct cx_request *req;

	debug_cond(DEBUG_RT, "%s: %s %p\n", __func__, ep->ep.name, ep);

	/* called with irqs blocked */
	while (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct cx_request, queue);
		done(ep, req, status);
	}
}

static void stop_activity(struct cx_udc *dev,
			  struct usb_gadget_driver *driver)
{
	int i;

	/* don't disconnect drivers more than once */
	if (dev->gadget.speed == USB_SPEED_UNKNOWN)
		driver = 0;
	dev->gadget.speed = USB_SPEED_UNKNOWN;

	/* prevent new request submissions, kill any outstanding requests  */
	for (i = 0; i < CX_MAX_ENDPOINTS; i++) {
		struct cx_ep *ep = &dev->ep[i];
		ep->stopped = 1;
		nuke(ep, -ESHUTDOWN);
	}

	/* report disconnect; the driver is already quiesced */
	if (driver) {
		driver->disconnect(&dev->gadget);
	}

	/* re-init driver-visible data structures */
	udc_reinit(dev);
}

static void otg_reset(void)
{
	debug_cond(DEBUG_INIT, "resetting core...\n");
	writel(CORE_SOFT_RESET, &reg->grstctl);
	while(readl(&reg->grstctl) & CORE_SOFT_RESET);
	debug_cond(DEBUG_INIT, "wait for AHB master state machine idle\n");
	while(!(readl(&reg->grstctl) & AHB_MASTER_IDLE));
	debug_cond(DEBUG_INIT, "reset otg core done.\n");
}

static void reconfig_usbd(void)
{
	/* 2. Soft-reset OTG Core and then unreset again. */
	int i;
	unsigned int uTemp = 0;

	debug_cond(DEBUG_INIT, "Reseting OTG controller\n");

	otg_reset(); /* core reset */

	writel(1<<30		/* force device mode */
		|0<<15		/* PHY Low Power Clock sel*/
		|1<<14		/* Non-Periodic TxFIFO Rewind Enable*/
		|0x5<<10	/* Turnaround time*/
		|0<<9		/* [0:HNP disable,1:HNP enable][ 0:SRP disable*/
	        |0<<8		/* 1:SRP enable] H1= 1,1*/
		|0<<7		/* Ulpi DDR sel*/
		|0<<6		/* 0: high speed utmi+, 1: full speed serial*/
		|0<<4		/* 0: utmi+, 1:ulpi*/
		|1<<3		/* phy i/f  0:8bit, 1:16bit*/
		|0x5<<0,	/* HS/FS Timeout**/
		&reg->gusbcfg);

	/* 3. Put the OTG device core in the disconnected state.*/
	uTemp = readl(&reg->dctl);
	uTemp |= SOFT_DISCONNECT;
	writel(uTemp, &reg->dctl);

	udelay(20);

	/* 4. Make the OTG device core exit from the disconnected state.*/
	uTemp = readl(&reg->dctl);
	uTemp = uTemp & ~SOFT_DISCONNECT;
	writel(uTemp, &reg->dctl);

	/* 5. Configure OTG Core to initial settings of device mode.*/
	/* [][1: full speed(30Mhz) 0:high speed]*/
	writel(EP_MISS_CNT(1) | DEV_SPEED_HIGH_SPEED_20, &reg->dcfg);

	mdelay(1);

	/* 6. Unmask the core interrupts*/
	writel(GINTMSK_INIT, &reg->gintmsk);

	/* 7. Set NAK bit of EP0, EP1, EP2*/
	writel(DEPCTL_EPDIS|DEPCTL_SNAK, &reg->out_endp[EP0_CON].doepctl);
	writel(DEPCTL_EPDIS|DEPCTL_SNAK, &reg->in_endp[EP0_CON].diepctl);

	for (i = 1; i < CX_MAX_ENDPOINTS; i++) {
		writel(DEPCTL_EPDIS|DEPCTL_SNAK, &reg->out_endp[i].doepctl);
		writel(DEPCTL_EPDIS|DEPCTL_SNAK, &reg->in_endp[i].diepctl);
	}

	/* 8. Unmask EPO interrupts*/
	writel(((1 << EP0_CON) << DAINT_OUT_BIT)
	       | (1 << EP0_CON), &reg->daintmsk);

	/* 9. Unmask device OUT EP common interrupts*/
	writel(DOEPMSK_INIT, &reg->doepmsk);

	/* 10. Unmask device IN EP common interrupts*/
	writel(DIEPMSK_INIT, &reg->diepmsk);

	/* 11. Set Rx FIFO Size (in 32-bit words) */
	writel(RX_FIFO_SIZE >> 2, &reg->grxfsiz);

	/* 12. Set Non Periodic Tx FIFO Size */
	writel((NPTX_FIFO_SIZE >> 2) << 16 | ((RX_FIFO_SIZE >> 2)) << 0,
	       &reg->gnptxfsiz);

	for (i = 1; i < CX_MAX_HW_ENDPOINTS; i++)
		writel((PTX_FIFO_SIZE >> 2) << 16 |
		       ((RX_FIFO_SIZE + NPTX_FIFO_SIZE +
			 PTX_FIFO_SIZE*(i-1)) >> 2) << 0,
		       &reg->dieptxf[i-1]);

	/* Flush the RX FIFO */
	writel(RX_FIFO_FLUSH, &reg->grstctl);
	while (readl(&reg->grstctl) & RX_FIFO_FLUSH)
		debug("\r Waiting for RX fifo Flush......" 
				"%8s" "\b\b\b\b\b\b\b\b\b\b", "");

	/* Flush all the Tx FIFO's */
	writel(TX_FIFO_FLUSH_ALL, &reg->grstctl);
	writel(TX_FIFO_FLUSH_ALL | TX_FIFO_FLUSH, &reg->grstctl);
	while (readl(&reg->grstctl) & TX_FIFO_FLUSH)
		debug("\r Waiting for TX fifo Flush..."
				"%8s" "\b\b\b\b\b\b\b\b\b\b", "");

	/* 13. Clear NAK bit of EP0, EP1, EP2*/
	/* For Slave mode*/
	/* EP0: Control OUT */
	writel(DEPCTL_EPDIS | DEPCTL_CNAK,
	       &reg->out_endp[EP0_CON].doepctl);

	/* 14. Initialize OTG Link Core.*/
	writel(GAHBCFG_INIT_MASK, &reg->gahbcfg);
}

static void set_max_pktsize(struct cx_udc *dev, enum usb_device_speed speed)
{
	unsigned int ep_ctrl;
	int i;

	if (speed == USB_SPEED_HIGH) {
		ep0_fifo_size = 64;
		ep_fifo_size = 512;
		ep_fifo_size2 = 1024;
		dev->gadget.speed = USB_SPEED_HIGH;
	} else {
		ep0_fifo_size = 64;
		ep_fifo_size = 64;
		ep_fifo_size2 = 64;
		dev->gadget.speed = USB_SPEED_FULL;
	}

	dev->ep[0].ep.maxpacket = ep0_fifo_size;
	for (i = 1; i < CX_MAX_ENDPOINTS; i++) {
		if (dev->ep[i].fastboot_bulk_epin)
			dev->ep[i].ep.maxpacket = ep0_fifo_size;
		else 
			dev->ep[i].ep.maxpacket = ep_fifo_size;
	}

	/* EP0 - Control IN (64 bytes)*/
	ep_ctrl = readl(&reg->in_endp[EP0_CON].diepctl);
	writel(ep_ctrl|(0<<0), &reg->in_endp[EP0_CON].diepctl);

	/* EP0 - Control OUT (64 bytes)*/
	ep_ctrl = readl(&reg->out_endp[EP0_CON].doepctl);
	writel(ep_ctrl|(0<<0), &reg->out_endp[EP0_CON].doepctl);
}

static int cx_ep_enable(struct usb_ep *_ep,
			 const struct usb_endpoint_descriptor *desc)
{
	struct cx_ep *ep;
	struct cx_udc *dev;

	debug_cond(DEBUG_RT, "%s: %p\n", __func__, _ep);

	ep = container_of(_ep, struct cx_ep, ep);
	if (!_ep || !desc || ep->desc || _ep->name == ep0name
	    || desc->bDescriptorType != USB_DT_ENDPOINT
	    || ep->bEndpointAddress != desc->bEndpointAddress
	    || ep_maxpacket(ep) <
	    le16_to_cpu(get_unaligned(&desc->wMaxPacketSize))) {
		debug("%s: bad ep or descriptor\n", __func__);
		return -EINVAL;
	}

	/* xfer types must match, except that interrupt ~= bulk */
	if (ep->bmAttributes != desc->bmAttributes
	    && ep->bmAttributes != USB_ENDPOINT_XFER_BULK
	    && desc->bmAttributes != USB_ENDPOINT_XFER_INT) {
		debug("%s: %s type mismatch\n", __func__, _ep->name);
		return -EINVAL;
	}

	/* hardware _could_ do smaller, but driver doesn't */
	if ((desc->bmAttributes == USB_ENDPOINT_XFER_BULK
	     && le16_to_cpu(get_unaligned(&desc->wMaxPacketSize)) !=
	     ep_maxpacket(ep)) || !get_unaligned(&desc->wMaxPacketSize)) {
		debug("%s: bad %s maxpacket\n", __func__, _ep->name);
		return -ERANGE;
	}

	dev = ep->dev;
	if (!dev->driver || dev->gadget.speed == USB_SPEED_UNKNOWN) {
		debug("%s: bogus device state\n", __func__);
		return -ESHUTDOWN;
	}

	ep->stopped = 0;
	ep->desc = desc;
	ep->pio_irqs = 0;
	ep->ep.maxpacket = le16_to_cpu(get_unaligned(&desc->wMaxPacketSize));

	/* Reset halt state */
	cx_udc_set_nak(ep);
	cx_udc_set_halt(_ep, 0);

	cx_udc_ep_activate(ep);

	debug_cond(DEBUG_RT, "%s: enabled %s, stopped = %d, maxpacket = %d\n",
	      __func__, _ep->name, ep->stopped, ep->ep.maxpacket);
	return 0;
}

/*
 * Disable EP
 */
static int cx_ep_disable(struct usb_ep *_ep)
{
	struct cx_ep *ep;

	debug("%s: %p\n", __func__, _ep);

	ep = container_of(_ep, struct cx_ep, ep);
	if (!_ep || !ep->desc) {
		debug("%s: %s not enabled\n", __func__,
		      _ep ? ep->ep.name : NULL);
		return -EINVAL;
	}

	/* Nuke all pending requests */
	nuke(ep, -ESHUTDOWN);

	ep->desc = 0;
	ep->stopped = 1;

	debug("%s: disabled %s\n", __func__, _ep->name);
	return 0;
}

static struct usb_request *cx_alloc_request(struct usb_ep *ep,
					     gfp_t gfp_flags)
{
	struct cx_request *req;

	debug_cond(DEBUG_RT, "%s: %s %p\n", __func__, ep->name, ep);

	req = memalign(CONFIG_SYS_CACHELINE_SIZE, sizeof(*req));
	if (!req)
		return 0;

	memset(req, 0, sizeof *req);
	INIT_LIST_HEAD(&req->queue);

	return &req->req;
}

static void cx_free_request(struct usb_ep *ep, struct usb_request *_req)
{
	struct cx_request *req;

	debug("%s: %p\n", __func__, ep);

	req = container_of(_req, struct cx_request, req);
	WARN_ON(!list_empty(&req->queue));
	kfree(req);
}

/* dequeue JUST ONE request */
static int cx_dequeue(struct usb_ep *_ep, struct usb_request *_req)
{
	struct cx_ep *ep;
	struct cx_request *req;

	debug("%s: %p\n", __func__, _ep);

	ep = container_of(_ep, struct cx_ep, ep);
	if (!_ep || ep->ep.name == ep0name)
		return -EINVAL;

	/* make sure it's actually queued on this endpoint */
	list_for_each_entry(req, &ep->queue, queue) {
		if (&req->req == _req)
			break;
	}
	if (&req->req != _req) {
		return -EINVAL;
	}

	done(ep, req, -ECONNRESET);

	return 0;
}

/*
 * Return bytes in EP FIFO
 */
static int cx_fifo_status(struct usb_ep *_ep)
{
	int count = 0;
	struct cx_ep *ep;

	ep = container_of(_ep, struct cx_ep, ep);
	if (!_ep) {
		debug("%s: bad ep\n", __func__);
		return -ENODEV;
	}

	debug("%s: %d\n", __func__, ep_index(ep));

	/* LPD can't report unclaimed bytes from IN fifos */
	if (ep_is_in(ep))
		return -EOPNOTSUPP;

	return count;
}

/*
 * Flush EP FIFO
 */
static void cx_fifo_flush(struct usb_ep *_ep)
{
	struct cx_ep *ep;

	ep = container_of(_ep, struct cx_ep, ep);
	if (unlikely(!_ep || (!ep->desc && ep->ep.name != ep0name))) {
		debug("%s: bad ep\n", __func__);
		return;
	}

	debug("%s: %d\n", __func__, ep_index(ep));
}

static const struct usb_gadget_ops cx_udc_ops = {
	/* current versions must always be self-powered */
};

static struct cx_udc memory = {
	.usb_address = 0,
	.gadget = {
		.ops = &cx_udc_ops,
		.ep0 = &memory.ep[0].ep,
		.name = driver_name,
	},

	/* control endpoint */
	.ep[0] = {
		.ep = {
			.name = ep0name,
			.ops = &cx_ep_ops,
			.maxpacket = EP0_FIFO_SIZE,
		},
		.dev = &memory,

		.bEndpointAddress = 0,
		.bmAttributes = 0,

		.ep_type = ep_control,
	},

	/* first group of endpoints */
	.ep[1] = {
		.ep = {
			.name = "ep1in-bulk",
			.ops = &cx_ep_ops,
			.maxpacket = EP_FIFO_SIZE,
		},
		.dev = &memory,

		.fastboot_bulk_epin = 1,

		.bEndpointAddress = USB_DIR_IN | 1,
		.bmAttributes = USB_ENDPOINT_XFER_BULK,

		.ep_type = ep_bulk_out,
		.fifo_num = 1,
	},

	.ep[2] = {
		.ep = {
			.name = "ep2out-bulk",
			.ops = &cx_ep_ops,
			.maxpacket = EP_FIFO_SIZE,
		},
		.dev = &memory,

		.fastboot_bulk_epin = 0,

		.bEndpointAddress = USB_DIR_OUT | 2,
		.bmAttributes = USB_ENDPOINT_XFER_BULK,

		.ep_type = ep_bulk_in,
		.fifo_num = 2,
	},

	.ep[3] = {
		.ep = {
			.name = "ep3in-int",
			.ops = &cx_ep_ops,
			.maxpacket = EP_FIFO_SIZE,
		},
		.dev = &memory,

		.fastboot_bulk_epin = 0,

		.bEndpointAddress = USB_DIR_IN | 3,
		.bmAttributes = USB_ENDPOINT_XFER_INT,

		.ep_type = ep_interrupt,
		.fifo_num = 3,
	},
};

/*
 *	probe - binds to the platform device
 */

int cx_udc_init(struct cx_plat_otg_data *pdata)
{
	struct cx_udc *dev = &memory;
	int retval = 0;

	debug_cond(DEBUG_INIT, "%s: %p\n", __func__, pdata);

	dev->pdata = pdata;

	phy = (struct cx_usbotg_phy *)pdata->regs_phy;
	reg = (struct cx_usbotg_reg *)pdata->regs_otg;
	usb_phy_ctrl = pdata->usb_phy_ctrl;

	/* regs_otg = (void *)pdata->regs_otg; */

	dev->gadget.is_dualspeed = 1;	/* Hack only*/
	dev->gadget.is_otg = 0;
	dev->gadget.is_a_peripheral = 0;
	dev->gadget.b_hnp_enable = 0;
	dev->gadget.a_hnp_support = 0;
	dev->gadget.a_alt_hnp_support = 0;

	the_controller = dev;

	usb_ctrl = memalign(CONFIG_SYS_CACHELINE_SIZE,
			    ROUND(sizeof(struct usb_ctrlrequest),
				  CONFIG_SYS_CACHELINE_SIZE));
	if (!usb_ctrl) {
		error("No memory available for usb device controller!\n");
		return -ENOMEM;
	}

	usb_ctrl_dma_addr = (dma_addr_t)usb_ctrl & ~KSEG0;
	debug_cond(DEBUG_INIT, "usb ctrl dma %p - %x\n", 
		usb_ctrl, usb_ctrl_dma_addr);

	udc_reinit(dev);

	return retval;
}

int usb_gadget_handle_interrupts()
{
	u32 intr_status = readl(&reg->gintsts);
	u32 gintmsk = readl(&reg->gintmsk);

	if (intr_status & gintmsk)
		return cx_udc_irq(1, (void *)the_controller);
	return 0;
}
