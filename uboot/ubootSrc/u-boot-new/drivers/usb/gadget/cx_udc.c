/**
 * Copyright (c) 2014 Cloudersemi(SH) Co., Ltd.
 *		http://www.samsung.com
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *      Ben Dooks <ben@simtec.co.uk>
 *      http://armlinux.simtec.co.uk/
 *
 * S3C USB2.0 High-speed / OtG driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <common.h>             
#include <errno.h>          
#include <linux/list.h>         
#include <malloc.h>             
#include <linux/usb/ch9.h>      
#include <linux/usb/gadget.h>   
#include <asm/byteorder.h>      
#include <asm/unaligned.h>      
#include <asm/io.h>             
#include <cx-hsotg.h>
#include <cx-udc.h>

#undef bool
typedef unsigned int bool; 
#undef false               
#define false 0
#undef true
#define true 1

//#define CONFIG_USB_DEBUG
#ifdef CONFIG_USB_DEBUG
#define usbd(x...)  printf(x) 
#else
#define usbd(x...)  do { } while (0)
#endif

//#define CONFIG_USB_DEBUGQ
#ifdef CONFIG_USB_DEBUGQ
#define usbdq(x...)  printf(x) 
#else
#define usbdq(x...)  do { } while (0)
#endif

#ifdef CONFIG_USB_DEBUG
#define usbde(x...)  printf(x) 
#else
#define usbde(x...)  do { } while (0)
#endif


/*
 * EP0_MPS_LIMIT
 *
 * Unfortunately there seems to be a limit of the amount of data that can
 * be transferred by IN transactions on EP0. This is either 127 bytes or 3
 * packets (which practically means 1 packet and 63 bytes of data) when the
 * MPS is set to 64.
 *
 * This means if we are wanting to move >127 bytes of data, we need to
 * split the transactions up, but just doing one packet at a time does
 * not work (this may be an implicit DATA0 PID on first packet of the
 * transaction) and doing 2 packets is outside the controller's limits.
 *
 * If we try to lower the MPS size for EP0, then no transfers work properly
 * for EP0, and the system will fail basic enumeration. As no cause for this
 * has currently been found, we cannot support any large IN transfers for
 * EP0.
 */
#define EP0_MPS_LIMIT   64

/**
 * struct cx_hsotg_req - data transfer request
 * @req: The USB gadget request
 * @queue: The list of requests for the endpoint this is queued for.
 * @in_progress: Has already had size/packets written to core
 * @mapped: DMA buffer for this request has been mapped via dma_map_single().
 */
struct cx_hsotg_req {
	struct usb_request      req;
	struct list_head        queue;
	unsigned char           in_progress;
	unsigned char           mapped;
};

/**
 * struct cx_hsotg_ep - driver endpoint definition.
 * @ep: The gadget layer representation of the endpoint.
 * @name: The driver generated name for the endpoint.
 * @queue: Queue of requests for this endpoint.
 * @parent: Reference back to the parent device structure.
 * @req: The current request that the endpoint is processing. This is
 *       used to indicate an request has been loaded onto the endpoint
 *       and has yet to be completed (maybe due to data move, or simply
 *       awaiting an ack from the core all the data has been completed).
 * @dir_in: Set to true if this endpoint is of the IN direction, which
 *          means that it is sending data to the Host.
 * @index: The index for the endpoint registers.
 * @mc: Multi Count - number of transactions per microframe
 * @interval - Interval for periodic endpoints
 * @name: The name array passed to the USB core.
 * @halted: Set if the endpoint has been halted.
 * @periodic: Set if this is a periodic ep, such as Interrupt
 * @isochronous: Set if this is a isochronous ep
 * @sent_zlp: Set if we've sent a zero-length packet.
 * @total_data: The total number of data bytes done.
 * @fifo_size: The size of the FIFO (for periodic IN endpoints)
 * @fifo_load: The amount of data loaded into the FIFO (periodic IN)
 * @last_load: The offset of data for the last start of request.
 * @size_loaded: The last loaded size for DxEPTSIZE for periodic IN
 *
 * This is the driver's state for each registered enpoint, allowing it
 * to keep track of transactions that need doing. Each endpoint has a
 * lock to protect the state, to try and avoid using an overall lock
 * for the host controller as much as possible.
 *
 * For periodic IN endpoints, we have fifo_size and fifo_load to try
 * and keep track of the amount of data in the periodic FIFO for each
 * of these as we don't have a status register that tells us how much
 * is in each of them. (note, this may actually be useless information
 * as in shared-fifo mode periodic in acts like a single-frame packet
 * buffer than a fifo)
 */
struct cx_hsotg_ep {
	struct usb_ep           ep;
	struct list_head        queue;
	struct cx_hsotg        *parent;
	struct cx_hsotg_req    *req;

	unsigned long           total_data;
	unsigned int            size_loaded;
	unsigned int            last_load;
	unsigned int            fifo_load;
	unsigned short          fifo_size;

	unsigned char           dir_in;
	unsigned char           index;
	unsigned char           mc;
	unsigned char           interval;

	unsigned int            halted:1;
	unsigned int            periodic:1;
	unsigned int            isochronous:1;
	unsigned int            sent_zlp:1;

	char                    name[10];
};

/**
 * struct cx_hsotg - driver state.
 * @driver: USB gadget driver
 * @uphy: The otg phy transceiver structure for old USB phy control.
 * @plat: The platform specific configuration data. This can be removed once
 * all SoCs support usb transceiver.
 * @regs: The memory area mapped for accessing registers.
 * @irq: The IRQ number we are using
 * @supplies: Definition of USB power supplies
 * @phyif: PHY interface width
 * @dedicated_fifos: Set if the hardware has dedicated IN-EP fifos.
 * @num_of_eps: Number of available EPs (excluding EP0)
 * @ep0_reply: Request used for ep0 reply.
 * @ep0_buff: Buffer for EP0 reply data, if needed.
 * @ctrl_buff: Buffer for EP0 control requests.
 * @ctrl_req: Request for EP0 control packets.
 * @setup: NAK management for EP0 SETUP
 * @last_rst: Time of last reset
 * @eps: The endpoints being supplied to the gadget framework
 */
struct cx_hsotg {
	struct usb_gadget_driver *driver;
	struct cx_hsotg_plat    *plat;
	void	                *regs;

	u32                     phyif;
	u32			force;
	unsigned int            dedicated_fifos:1;
	unsigned char           num_of_eps;

	struct usb_request      *ep0_reply;
	struct usb_request      *ctrl_req;
	u8                      ep0_buff[8];
	u8                      ctrl_buff[8];

	struct usb_gadget       gadget;
	unsigned int            setup;
	unsigned long           last_rst;
	struct cx_hsotg_ep     *eps;
};

/* usb hign speed otg */
static struct cx_hsotg *hsotg;

static int cx_hsotg_ep_queue(struct usb_ep *ep, struct usb_request *req,
		gfp_t gfp_flags);
static void cx_hsotg_complete_setup(struct usb_ep *ep,
		struct usb_request *req);
static void cx_hsotg_process_control(struct cx_hsotg *hsotg,
		struct usb_ctrlrequest *ctrl);

static void cx_push_data(void *addr, const void *src, unsigned count)
{
	unsigned int *buf = (unsigned int *)src;

	usbd("data push %d words, %p --> %p\n", count, addr, buf);

	while (count--) {
		writel(*buf, addr);
		buf++;
	}
}

static inline void cx_pop_data(void *addr, u32 *dst, int count)  
{
	while (--count >= 0) {
		u32 data = readl(addr);
		*dst = data;
		dst++;
	}
}

/* conversion functions */
static inline struct cx_hsotg_req *our_req(struct usb_request *req)
{
	return container_of(req, struct cx_hsotg_req, req);
}

static inline struct cx_hsotg_ep *our_ep(struct usb_ep *ep)
{
	return container_of(ep, struct cx_hsotg_ep, ep);
}

static inline struct cx_hsotg *to_hsotg(struct usb_gadget *gadget)
{
	return container_of(gadget, struct cx_hsotg, gadget);
}

static inline void __orr32(void __iomem *ptr, u32 val)
{
	writel(readl(ptr) | val, ptr);
}

static inline void __bic32(void __iomem *ptr, u32 val)
{
	writel(readl(ptr) & ~val, ptr);
}

/**
 * get_ep_head - return the first request on the endpoint
 * @hs_ep: The controller endpoint to get
 *
 * Get the first request on the endpoint.
 */
static struct cx_hsotg_req *get_ep_head(struct cx_hsotg_ep *hs_ep)
{
	if (list_empty(&hs_ep->queue))
		return NULL;

	return list_first_entry(&hs_ep->queue, struct cx_hsotg_req, queue);
}

/**
 * cx_hsotg_txfifo_flush - flush Tx FIFO
 * @hsotg: The driver state
 * @idx: The index for the endpoint (0..15)
 */
static void cx_hsotg_txfifo_flush(struct cx_hsotg *hsotg, unsigned int idx)
{
	int timeout;
	int val;

	debug("%s FIFO%d\n", __func__, idx);

	writel(GRSTCTL_TXFNUM(idx) | GRSTCTL_TXFFLSH,
		hsotg->regs + GRSTCTL);

	/* wait until the fifo is flushed */
	timeout = 100;

	while (1) {
		val = readl(hsotg->regs + GRSTCTL);

		if ((val & (GRSTCTL_TXFFLSH)) == 0)
			break;

		if (--timeout == 0) {
			debug( "%s: timeout flushing fifo\n", __func__);
			break;
		}

		udelay(1);
	}
}

/**
 * cx_hsotg_ctrl_epint - enable/disable an endpoint irq
 * @hsotg: The device state
 * @ep: The endpoint index
 * @dir_in: True if direction is in.
 * @en: The enable value, true to enable
 *
 * Set or clear the mask for an individual endpoint's interrupt
 * request.
 */
static void cx_hsotg_ctrl_epint(struct cx_hsotg *hsotg,
				 unsigned int ep, unsigned int dir_in,
				 unsigned int en)
{
	u32 bit = 1 << ep;
	u32 daint;

	if (!dir_in)
		bit <<= 16;

	daint = readl(hsotg->regs + DAINTMSK);
	if (en)
		daint |= bit;
	else
		daint &= ~bit;
	writel(daint, hsotg->regs + DAINTMSK);
}

/**
 * cx_hsotg_init_fifo - initialise non-periodic FIFOs
 * @hsotg: The device instance.
 */
static void cx_hsotg_init_fifo(struct cx_hsotg *hsotg)
{
	unsigned int ep;
	unsigned int addr;
	unsigned int size;
	int timeout;
	u32 val;

	/* set FIFO sizes to 1024/1024 */

	writel(1024, hsotg->regs + GRXFSIZ);
	writel((1024 << FIFOSIZE_STARTADDR_SHIFT) |
		(1024 << FIFOSIZE_DEPTH_SHIFT), hsotg->regs + GNPTXFSIZ);

	/*
	 * arange all the rest of the TX FIFOs, as some versions of this
	 * block have overlapping default addresses. This also ensures
	 * that if the settings have been changed, then they are set to
	 * known values.
	 */

	/* start at the end of the GNPTXFSIZ, rounded up */
	addr = 1024 + 1024;
	size = 768;

	/*
	 * currently we allocate TX FIFOs for all possible endpoints,
	 * and assume that they are all the same size.
	 * is rigth?? just no, i think.
	 */

	for (ep = 1; ep <= hsotg->num_of_eps; ep++) {
		unsigned short rb_size;

		/*
		 * P30, Dedicated TxFIFO Operation (OTG_EN_DED_TX_FIFO = 1)
		 */
		val = addr;
		val |= size << FIFOSIZE_DEPTH_SHIFT;
		writel(val, hsotg->regs + DPTXFSIZN(ep));

		/*
		 * read back, check.
		 */
		rb_size = FIFOSIZE_DEPTH_GET(readl(hsotg->regs + DPTXFSIZN(ep)));
		if (rb_size != size) { /* rb_sie < size */
			val = addr;
			val |= rb_size << FIFOSIZE_DEPTH_SHIFT;
			writel(val, hsotg->regs + DPTXFSIZN(ep));
			usbd("DPTx[%d] FSize=%d, StAddr=0x%08x\n", ep, rb_size, addr);
			addr += rb_size;
		} else {
			usbd("DPTx[%d] FSize=%d, StAddr=0x%08x\n", ep, size, addr);
			addr += size;
		}
	}

	/*
	 * according to p428 of the design guide, we need to ensure that
	 * all fifos are flushed before continuing
	 */

	writel(GRSTCTL_TXFNUM(0x10) | GRSTCTL_TXFFLSH |
	       GRSTCTL_RXFFLSH, hsotg->regs + GRSTCTL);

	/* wait until the fifos are both flushed */
	timeout = 100;
	while (1) {
		val = readl(hsotg->regs + GRSTCTL);

		if ((val & (GRSTCTL_TXFFLSH | GRSTCTL_RXFFLSH)) == 0)
			break;

		if (--timeout == 0) {
			printf("%s: timeout flushing fifos\n", __func__);
			return;
		}

		udelay(1);
	}

	usbd("FIFOs reset successed(%d)\n", timeout);
}

/**
 * @ep: USB endpoint to allocate request for.
 * @flags: Allocation flags
 *
 * Allocate a new USB request structure appropriate for 
 * the specified endpoint
 */
static struct usb_request *cx_hsotg_ep_alloc_request(struct usb_ep *ep,
						      gfp_t flags)
{
	struct cx_hsotg_req *req;

	req = malloc(sizeof(*req));
	if (!req)
		return NULL;

	memset(req, 0, sizeof *req);
	INIT_LIST_HEAD(&req->queue);

	return &req->req;
}

/**
 * is_ep_periodic - return true if the endpoint is in periodic mode.
 * @hs_ep: The endpoint to query.
 *
 * Returns true if the endpoint is in periodic mode, meaning it is being
 * used for an Interrupt or ISO transfer.
 */
static inline int is_ep_periodic(struct cx_hsotg_ep *hs_ep)
{
	return hs_ep->periodic;
}

/**
 * cx_hsotg_write_fifo - write packet Data to the TxFIFO
 * @hsotg: The controller state.
 * @hs_ep: The endpoint we're going to write for.
 * @hs_req: The request to write data for.
 *
 * This is called when the TxFIFO has some space in it to hold a new
 * transmission and we have something to give it. The actual setup of
 * the data size is done elsewhere, so all we have to do is to actually
 * write the data.
 *
 * The return value is zero if there is more space (or nothing was done)
 * otherwise -ENOSPC is returned if the FIFO space was used up.
 *
 * This routine is only needed for PIO
 */
static int cx_hsotg_write_fifo(struct cx_hsotg *hsotg,
				struct cx_hsotg_ep *hs_ep,
				struct cx_hsotg_req *hs_req)
{
	bool periodic = is_ep_periodic(hs_ep);
	u32 gnptxsts = readl(hsotg->regs + GNPTXSTS);
	int buf_pos = hs_req->req.actual;
	int to_write = hs_ep->size_loaded;
	void *data;
	int can_write;
	int pkt_round;
	int max_transfer;

	to_write -= (buf_pos - hs_ep->last_load);

	/* if there's nothing to write, get out early */
	if (to_write == 0)
		return 0;

	if (periodic && !hsotg->dedicated_fifos) {
		u32 epsize = readl(hsotg->regs + DIEPTSIZ(hs_ep->index));
		int size_left;
		int size_done;

		/*
		 * work out how much data was loaded so we can calculate
		 * how much data is left in the fifo.
		 */

		size_left = DXEPTSIZ_XFERSIZE_GET(epsize);

		/*
		 * if shared fifo, we cannot write anything until the
		 * previous data has been completely sent.
		 */
		if (hs_ep->fifo_load != 0) {
			return -ENOSPC;
		}

		usbd("%s: left=%d, load=%d, fifo=%d, size %d\n", __func__, size_left,
			hs_ep->size_loaded, hs_ep->fifo_load, hs_ep->fifo_size);

		/* how much of the data has moved */
		size_done = hs_ep->size_loaded - size_left;

		/* how much data is left in the fifo */
		can_write = hs_ep->fifo_load - size_done;
		usbd("%s: => can_write1=%d\n", __func__, can_write);

		can_write = hs_ep->fifo_size - can_write;
		usbd("%s: => can_write2=%d\n", __func__, can_write);

		if (can_write <= 0) {
			return -ENOSPC;
		}
	} else if (hsotg->dedicated_fifos && hs_ep->index != 0) {
		can_write = readl(hsotg->regs + DTXFSTS(hs_ep->index));

		can_write &= 0xffff;
		can_write *= 4;
	} else {
		if (GNPTXSTS_NP_TXQ_SPC_AVAIL_GET(gnptxsts) == 0) {
			usbd("%s: no queue slots available (0x%08x)\n",
				__func__, gnptxsts);
			return -ENOSPC;
		}

		can_write = GNPTXSTS_NP_TXF_SPC_AVAIL_GET(gnptxsts);
		can_write *= 4;	/* fifo size is in 32bit quantities. */
	}

	max_transfer = hs_ep->ep.maxpacket * hs_ep->mc;

	usbd("%s: GNPTXSTS=%08x, can=%d, to=%d, max_transfer %d\n",
		 __func__, gnptxsts, can_write, to_write, max_transfer);

	/*
	 * limit to 512 bytes of data, it seems at least on the non-periodic
	 * FIFO, requests of >512 cause the endpoint to get stuck with a
	 * fragment of the end of the transfer in it.
	 */
	if (can_write > 512 && !periodic)
		can_write = 512;

	/*
	 * limit the write to one max-packet size worth of data, but allow
	 * the transfer to return that it did not run out of fifo space
	 * doing it.
	 */
	if (to_write > max_transfer)
		to_write = max_transfer;

	/* see if we can write data */

	if (to_write > can_write) {
		to_write = can_write;
		pkt_round = to_write % max_transfer;

		/*
		 * Round the write down to an
		 * exact number of packets.
		 *
		 * Note, we do not currently check to see if we can ever
		 * write a full packet or not to the FIFO.
		 */

		if (pkt_round)
			to_write -= pkt_round;

		/*
		 * enable correct FIFO interrupt to alert us when there
		 * is more room left.
		 */
	}

	usbd("write %d/%d, can_write %d, done %d\n",
		 to_write, hs_req->req.length, can_write, buf_pos);

	if (to_write <= 0)
		return -ENOSPC;

	hs_req->req.actual = buf_pos + to_write;
	hs_ep->total_data += to_write;

	if (periodic)
		hs_ep->fifo_load += to_write;

	to_write = DIV_ROUND_UP(to_write, 4);
	data = hs_req->req.buf + buf_pos;

	cx_push_data(hsotg->regs + EPFIFO(hs_ep->index), data, to_write);

	usbd("write FIFO ok...\n");

	return (to_write >= can_write) ? -ENOSPC : 0;
}

/**
 * get_ep_limit - get the maximum data legnth for this endpoint
 * @hs_ep: The endpoint
 *
 * Return the maximum data that can be queued in one go on a given endpoint
 * so that transfers that are too long can be split.
 */
static unsigned get_ep_limit(struct cx_hsotg_ep *hs_ep)
{
	int index = hs_ep->index;
	unsigned maxsize;
	unsigned maxpkt;

	if (index != 0) {
		maxsize = DXEPTSIZ_XFERSIZE_LIMIT + 1;
		maxpkt = DXEPTSIZ_PKTCNT_LIMIT + 1;
	} else {
		maxsize = 64+64;
		if (hs_ep->dir_in)
			maxpkt = DIEPTSIZ0_PKTCNT_LIMIT + 1;
		else
			maxpkt = 2;
	}

	/* we made the constant loading easier above by using +1 */
	maxpkt--;
	maxsize--;

	/*
	 * constrain by packet count if maxpkts*pktsize is greater
	 * than the length register size.
	 */

	if ((maxpkt * hs_ep->ep.maxpacket) < maxsize)
		maxsize = maxpkt * hs_ep->ep.maxpacket;

	usbd("%s: %d\n", __func__, maxsize);

	return maxsize;
}

/**
 * cx_hsotg_start_req - start a USB request from an endpoint's queue
 * @hsotg: The controller state.
 * @hs_ep: The endpoint to process a request for
 * @hs_req: The request to start.
 * @continuing: True if we are doing more for the current request.
 *
 * Start the given request running by setting the endpoint registers
 * appropriately, and writing any data to the FIFOs.
 */
static void cx_hsotg_start_req(struct cx_hsotg *hsotg,
				struct cx_hsotg_ep *hs_ep,
				struct cx_hsotg_req *hs_req,
				bool continuing)
{
	struct usb_request *ureq = &hs_req->req;
	int index = hs_ep->index;
	int dir_in = hs_ep->dir_in;
	u32 epctrl_reg;
	u32 epsize_reg;
	u32 epsize;
	u32 ctrl;
	unsigned length;
	unsigned packets;
	unsigned maxreq;

	if (index != 0) {
		if (hs_ep->req && !continuing) {
			printf("%s: active request\n", __func__);
			return;
		} else if (hs_ep->req != hs_req && continuing) {
			printf("%s: continue different req\n", __func__);
			return;
		}
	}

	epctrl_reg = dir_in ? DIEPCTL(index) : DOEPCTL(index);
	epsize_reg = dir_in ? DIEPTSIZ(index) : DOEPTSIZ(index);

	usbd("%s(%d): DxEPCTL[%04x]=0x%08x, ep %d, dir %s\n",
		__func__, __LINE__, epctrl_reg, readl(hsotg->regs + epctrl_reg), index,
		hs_ep->dir_in ? "in" : "out");

	/* If endpoint is stalled, we will restart request later */
	ctrl = readl(hsotg->regs + epctrl_reg);

	if (ctrl & DXEPCTL_STALL) {
		printf("%s: ep%d is stalled\n", __func__, index);
		return;
	}

	length = ureq->length - ureq->actual;
	usbd("ureq->length:%d ureq->actual:%d\n",
		ureq->length, ureq->actual);
	usbd("REQ buf %p len %d dma %pad noi=%d zp=%d snok=%d\n",
		ureq->buf, length, &ureq->dma,
		ureq->no_interrupt, ureq->zero, ureq->short_not_ok);

	maxreq = get_ep_limit(hs_ep);
	if (length > maxreq) {
		int round = maxreq % hs_ep->ep.maxpacket;

		usbd("%s: length %d, max-req %d, r %d\n",
			__func__, length, maxreq, round);

		/* round down to multiple of packets */
		if (round)
			maxreq -= round;

		length = maxreq;
	}

	if (length)
		packets = DIV_ROUND_UP(length, hs_ep->ep.maxpacket);
	else
		packets = 1;	/* send one packet if length is zero. */

	if (hs_ep->isochronous && length > (hs_ep->mc * hs_ep->ep.maxpacket)) {
		printf("req length > maxpacket*mc\n");
		return;
	}

	if (dir_in && index != 0)
		if (hs_ep->isochronous)
			epsize = DXEPTSIZ_MC(packets);
		else
			epsize = DXEPTSIZ_MC(1);
	else
		epsize = 0;

	if (index != 0 && ureq->zero) {
		/*
		 * test for the packets being exactly right for the
		 * transfer
		 */

		if (length == (packets * hs_ep->ep.maxpacket))
			packets++;
	}

	epsize |= DXEPTSIZ_PKTCNT(packets);
	epsize |= DXEPTSIZ_XFERSIZE(length);

	usbd("%s(%d): %d@%d/%d, 0x%08x => 0x%08x\n",
		__func__, __LINE__, packets, length, 
		ureq->length, epsize, epsize_reg);

	/* store the request as the current one we're doing */
	hs_ep->req = hs_req;

	/* write size / packets */
	writel(epsize, hsotg->regs + epsize_reg);

	ctrl |= DXEPCTL_EPENA;	/* ensure ep enabled */
	ctrl |= DXEPCTL_USBACTEP;

	usbd("setup req:%d\n", hsotg->setup);

	/* For Setup request do not clear NAK */
	if (hsotg->setup && index == 0)
		hsotg->setup = 0;
	else
		ctrl |= DXEPCTL_CNAK;	/* clear NAK set by core */


	usbd("%s(%d): DxEPCTL=0x%08x\n", __func__, __LINE__, ctrl);
	writel(ctrl, hsotg->regs + epctrl_reg);

	/*
	 * set these, it seems that DMA support increments past the end
	 * of the packet buffer so we need to calculate the length from
	 * this information.
	 */
	hs_ep->size_loaded = length;
	hs_ep->last_load = ureq->actual;

	if (dir_in) {
		/* set these anyway, we may need them for non-periodic in */
		hs_ep->fifo_load = 0;

		cx_hsotg_write_fifo(hsotg, hs_ep, hs_req);
	}

	/*
	 * clear the INTknTXFEmpMsk when we start request, more as a aide
	 * to usbdging to see what is going on.
	 */
	if (dir_in)
		writel(DIEPMSK_INTKNTXFEMPMSK,
		       hsotg->regs + DIEPINT(index));

	/*
	 * Note, trying to clear the NAK here causes problems with transmit
	 * on the cx ending up with the TXFIFO becoming full.
	 */
	usbd("%s(%d): DXEPCTL=0x%08x\n", __func__, __LINE__, 
		readl(hsotg->regs + epctrl_reg));

	/* enable ep interrupts */
	cx_hsotg_ctrl_epint(hsotg, hs_ep->index, hs_ep->dir_in, 1);
}

/**
 * cx_hsotg_complete_request - complete a request given to us
 * @hsotg: The device state.
 * @hs_ep: The endpoint the request was on.
 * @hs_req: The request to complete.
 * @result: The result code (0 => Ok, otherwise errno)
 *
 * The given request has finished, so call the necessary completion
 * if it has one and then look to see if we can start a new request
 * on the endpoint.
 *
 * Note, expects the ep to already be locked as appropriate.
 */
static void cx_hsotg_complete_request(struct cx_hsotg *hsotg,
				       struct cx_hsotg_ep *hs_ep,
				       struct cx_hsotg_req *hs_req,
				       int result)
{
	bool restart;

	if (!hs_req) {
		usbd("%s: nothing to complete?\n", __func__);
		return;
	}

	usbd("%s(%d)-complete: ep %p %s, req %p, %d => %p\n",
		__func__, __LINE__, hs_ep, hs_ep->ep.name, 
		hs_req, result, hs_req->req.complete);

	/*
	 * only replace the status if we've not already set an error
	 * from a previous transaction
	 */

	if (hs_req->req.status == -EINPROGRESS)
		hs_req->req.status = result;

	hs_ep->req = NULL;
	list_del_init(&hs_req->queue); /* dequeue */

	/*
	 * call the complete request with the locks off, just in case the
	 * request tries to queue more work for this endpoint.
	 */

	if (hs_req->req.complete) {
		hs_req->req.complete(&hs_ep->ep, &hs_req->req);
	}

	/*
	 * Look to see if there is anything else to do. Note, the completion
	 * of the previous request may have caused a new request to be started
	 * so be careful when doing this.
	 */

	if (!hs_ep->req && result >= 0) {
		restart = !list_empty(&hs_ep->queue);
		usbd("%s - queue is %sempty\n", __func__, restart ? "not " : "");
		if (restart) {
			hs_req = get_ep_head(hs_ep);
			cx_hsotg_start_req(hsotg, hs_ep, hs_req, false);
		}
	}

	usbd("%s(%d)- event handle successed!\n", __func__, __LINE__);
}

/**
 * cx_hsotg_enqueue_setup - start a request for EP0 packets
 * @hsotg: The device state.
 *
 * Enqueue a request on EP0 if necessary to received any SETUP packets
 * received from the host.
 */
static void cx_hsotg_enqueue_setup(struct cx_hsotg *hsotg)
{
	struct usb_request *req = hsotg->ctrl_req;
	struct cx_hsotg_req *hs_req = our_req(req);
	int ret;

	usbd("%s(%d): queueing setup request\n", 
			__func__, __LINE__);

	req->zero = 0;
	req->length = 8;
	req->buf = hsotg->ctrl_buff;
	req->complete = cx_hsotg_complete_setup;

	if (!list_empty(&hs_req->queue)) {
		usbd("%s already queued?\n", __func__);
		return;
	}

	hsotg->eps[0].dir_in = 0; /* setup packet receive, out */

	ret = cx_hsotg_ep_queue(&hsotg->eps[0].ep, req, 0);
	if (ret < 0) {
		printf("%s: failed queue (%d)\n", __func__, ret);
		/*
		 * Don't think there's much we can do other than watch the
		 * driver fail.
		 */
	}
}

/**
 * cx_hsotg_complete_setup - completion of a setup transfer
 * @ep: The endpoint the request was on.
 * @req: The request completed.
 *
 * Called on completion of any requests the driver itself submitted for
 * EP0 setup packets
 */
static void cx_hsotg_complete_setup(struct usb_ep *ep,
				     struct usb_request *req)
{
	struct cx_hsotg_ep *hs_ep = our_ep(ep);
	struct cx_hsotg *hsotg = hs_ep->parent;

	usbd("%s: req %p ep %d\n", __func__, our_req(req), hs_ep->index);

	if (req->status < 0) {
		usbd("%s: failed %d\n", __func__, req->status);
		return;
	}

	if (req->actual == 0)
		cx_hsotg_enqueue_setup(hsotg);
	else
		cx_hsotg_process_control(hsotg, req->buf);
}

/**
 * kill_all_requests - remove all requests from the endpoint's queue
 * @hsotg: The device state.
 * @ep: The endpoint the requests may be on.
 * @result: The result code to use.
 * @force: Force removal of any current requests
 *
 * Go through the requests on the given endpoint and mark them
 * completed with the given result code.
 */
static void kill_all_requests(struct cx_hsotg *hsotg,
			      struct cx_hsotg_ep *ep,
			      int result, bool force)
{
	struct cx_hsotg_req *req, *treq;

	usbd("%s: %s@%d res(%d) %s\n",
			__func__, ep->name, ep->index,
			result, force ? "force" : ""
			);

	list_for_each_entry_safe(req, treq, &ep->queue, queue) {
		/*
		 * currently, we can't do much about an already
		 * running request on an in endpoint
		 */

		if (ep->req == req && ep->dir_in && !force)
			continue;

		cx_hsotg_complete_request(hsotg, ep, req,
					   result);
	}
	if (hsotg->dedicated_fifos)
		if ((readl(hsotg->regs + DTXFSTS(ep->index)) & 0xffff) * 4 < 3072)
			cx_hsotg_txfifo_flush(hsotg, ep->index);
}

/**
 * cx_hsotg_ep_disable - disable given endpoint
 * @ep: The endpoint to disable.
 */
static int cx_hsotg_ep_disable(struct usb_ep *ep)
{
	struct cx_hsotg_ep *hs_ep = our_ep(ep);
	struct cx_hsotg *hsotg = hs_ep->parent;
	int dir_in = hs_ep->dir_in;
	int index = hs_ep->index;
	u32 epctrl_reg;
	u32 ctrl;

	usbd("%s(ep %p)\n", __func__, ep);

	if (ep == &hsotg->eps[0].ep) {
		printf("%s: called for ep0\n", __func__);
		return -EINVAL;
	}

	epctrl_reg = dir_in ? DIEPCTL(index) : DOEPCTL(index);

	/* terminate all requests with shutdown */
	kill_all_requests(hsotg, hs_ep, -ESHUTDOWN, false);


	ctrl = readl(hsotg->regs + epctrl_reg);
	ctrl &= ~DXEPCTL_EPENA;
	ctrl &= ~DXEPCTL_USBACTEP;
	ctrl |= DXEPCTL_SNAK;

	usbd("%s: DxEPCTL=0x%08x\n", __func__, ctrl);
	writel(ctrl, hsotg->regs + epctrl_reg);

	/* disable endpoint interrupts */
	cx_hsotg_ctrl_epint(hsotg, hs_ep->index, hs_ep->dir_in, 0);

	return 0;
}

static int cx_hsotg_ep_queue(struct usb_ep *ep, struct usb_request *req,
			      gfp_t gfp_flags)
{
	struct cx_hsotg_req *hs_req = our_req(req);
	struct cx_hsotg_ep *hs_ep = our_ep(ep);
	struct cx_hsotg *hs = hs_ep->parent;
	bool first;

	usbd("%s(%d)->%s: req %p: %d@%p, noi=%d, zero=%d, snok=%d\n",
		__func__, __LINE__, ep->name, req, req->length, 
		req->buf, req->no_interrupt,
		req->zero, req->short_not_ok);

	/* initialise status of the request */
	INIT_LIST_HEAD(&hs_req->queue);
	req->actual = 0;
	req->status = -EINPROGRESS;

	first = list_empty(&hs_ep->queue);
	list_add_tail(&hs_req->queue, &hs_ep->queue);

	if (first) {
		cx_hsotg_start_req(hs, hs_ep, hs_req, false);
	} else if (!first && !hs_ep->req) {
		printf("%s(%d): ep %d queue list is not empty, sched request",
				__func__, __LINE__, hs_ep->index);
	} else if (hs_ep->req) {
		printf("%s(%d): ep %d process req %p\n", __func__, __LINE__,
				hs_ep->index, hs_ep->req);
		if (hs_ep->index == 0 && !hs_ep->dir_in) {
			struct cx_hsotg_req *ireq, *treq;

			/*
			 * EP0 setup toekn is high priority, means something
			 * exception on usb bus, so should kill the prev
			 * request.
			 * eg.1: in token process, host send in token excessed
			 * max times, then enum the device again.
			 */
			list_for_each_entry_safe(ireq, treq, &hs_ep->queue, queue) {
				if (ireq != hs_req) {
					cx_hsotg_complete_request(hs, hs_ep, 
						ireq, -ETIMEDOUT);
				}
			}

			/*
			 * enqueue setup token..
			 */
			cx_hsotg_start_req(hs, hs_ep, hs_req, false);
		}
	}

#ifdef DEBUG
	/*
	 * Just for usbd.
	 */
	{
		struct cx_hsotg_req *usbd;
		list_for_each_entry(usbd, &hs_ep->queue, queue)
			usbd("%s(%d): list req %p queue\n", __func__, __LINE__, usbd);
	}
#endif

	return 0;
}

static void cx_hsotg_ep_free_request(struct usb_ep *ep,
				      struct usb_request *req)
{
	struct cx_hsotg_req *hs_req = our_req(req);

	usbd("%s: ep %d req %p\n", __func__, our_ep(ep)->index, hs_req);

	free(hs_req);
}

/**
 * cx_hsotg_complete_oursetup - setup completion callback
 * @ep: The endpoint the request was on.
 * @req: The request completed.
 *
 * Called on completion of any requests the driver itself
 * submitted that need cleaning up.
 */
static void cx_hsotg_complete_oursetup(struct usb_ep *ep,
					struct usb_request *req)
{
	usbd("%s: ep %p, req %p\n", __func__, ep, req);
	cx_hsotg_ep_free_request(ep, req);
}

/**
 * ep_from_windex - convert control wIndex value to endpoint
 * @hsotg: The driver state.
 * @windex: The control request wIndex field (in host order).
 *
 * Convert the given wIndex into a pointer to an driver endpoint
 * structure, or return NULL if it is not a valid endpoint.
 */
static struct cx_hsotg_ep *ep_from_windex(struct cx_hsotg *hsotg,
					   u32 windex)
{
	struct cx_hsotg_ep *ep = &hsotg->eps[windex & 0x7F];
	int dir = (windex & USB_DIR_IN) ? 1 : 0;
	int idx = windex & 0x7F;

	if (windex >= 0x100)
		return NULL;

	if (idx > hsotg->num_of_eps)
		return NULL;

	if (idx && ep->dir_in != dir)
		return NULL;

	return ep;
}

/**
 * cx_hsotg_send_reply - send reply to control request
 * @hsotg: The device state
 * @ep: Endpoint 0
 * @buff: Buffer for request
 * @length: Length of reply.
 *
 * Create a request and queue it on the given endpoint. This is useful as
 * an internal method of sending replies to certain control requests, etc.
 */
static int cx_hsotg_send_reply(struct cx_hsotg *hsotg,
				struct cx_hsotg_ep *ep,
				void *buff,
				int length)
{
	struct usb_request *req;
	int ret;

	usbd("%s: buff %p, len %d\n", __func__, buff, length);

	req = cx_hsotg_ep_alloc_request(&ep->ep, 0);
	hsotg->ep0_reply = req;
	if (!req) {
		printf("%s: cannot alloc req\n", __func__);
		return -ENOMEM;
	}

	req->buf = hsotg->ep0_buff;
	req->length = length;
	req->zero = 1; /* always do zero-length final transfer */
	req->complete = cx_hsotg_complete_oursetup;

	if (length)
		memcpy(req->buf, buff, length);
	else
		ep->sent_zlp = 1;

	ret = cx_hsotg_ep_queue(&ep->ep, req, 0);
	if (ret) {
		printf("%s: cannot queue req\n", __func__);
		return ret;
	}

	return 0;
}

/**
 * cx_hsotg_process_req_status - process request GET_STATUS
 * @hsotg: The device state
 * @ctrl: USB control request
 */
static int cx_hsotg_process_req_status(struct cx_hsotg *hsotg,
					struct usb_ctrlrequest *ctrl)
{
	struct cx_hsotg_ep *ep0 = &hsotg->eps[0];
	struct cx_hsotg_ep *ep;
	__le16 reply;
	int ret;

	usbd("%s: USB_REQ_GET_STATUS\n", __func__);

	if (!ep0->dir_in) {
		printf("%s: direction out?\n", __func__);
		return -EINVAL;
	}

	switch (ctrl->bRequestType & USB_RECIP_MASK) {
	case USB_RECIP_DEVICE:
		reply = cpu_to_le16(0); /* bit 0 => self powered,
					 * bit 1 => remote wakeup */
		break;

	case USB_RECIP_INTERFACE:
		/* currently, the data result should be zero */
		reply = cpu_to_le16(0);
		break;

	case USB_RECIP_ENDPOINT:
		ep = ep_from_windex(hsotg, le16_to_cpu(ctrl->wIndex));
		if (!ep)
			return -ENOENT;

		reply = cpu_to_le16(ep->halted ? 1 : 0);
		break;

	default:
		return 0;
	}

	if (le16_to_cpu(ctrl->wLength) != 2)
		return -EINVAL;

	ret = cx_hsotg_send_reply(hsotg, ep0, &reply, 2);
	if (ret) {
		printf("%s: failed to send reply\n", __func__);
		return ret;
	}

	return 1;
}

static int cx_hsotg_ep_sethalt(struct usb_ep *ep, int value);

/**
 * cx_hsotg_process_req_featire - process request {SET,CLEAR}_FEATURE
 * @hsotg: The device state
 * @ctrl: USB control request
 */
static int cx_hsotg_process_req_feature(struct cx_hsotg *hsotg,
					 struct usb_ctrlrequest *ctrl)
{
	struct cx_hsotg_ep *ep0 = &hsotg->eps[0];
	struct cx_hsotg_req *hs_req;
	bool restart;
	bool set = (ctrl->bRequest == USB_REQ_SET_FEATURE);
	struct cx_hsotg_ep *ep;
	int ret;
	bool halted;

	usbd("%s: %s_FEATURE\n",
		__func__, set ? "SET" : "CLEAR");

	if (ctrl->bRequestType == USB_RECIP_ENDPOINT) {
		ep = ep_from_windex(hsotg, le16_to_cpu(ctrl->wIndex));
		if (!ep) {
			usbd("%s: no endpoint for 0x%04x\n",
				__func__, le16_to_cpu(ctrl->wIndex));
			return -ENOENT;
		}

		switch (le16_to_cpu(ctrl->wValue)) {
		case USB_ENDPOINT_HALT:
			halted = ep->halted;

			cx_hsotg_ep_sethalt(&ep->ep, set);

			ret = cx_hsotg_send_reply(hsotg, ep0, NULL, 0);
			if (ret) {
				printf("%s: failed to send reply\n", __func__);
				return ret;
			}

			/*
			 * we have to complete all requests for ep if it was
			 * halted, and the halt was cleared by CLEAR_FEATURE
			 */

			if (!set && halted) {
				/*
				 * If we have request in progress,
				 * then complete it
				 */
				if (ep->req) {
					hs_req = ep->req;
					ep->req = NULL;
					list_del_init(&hs_req->queue);
					hs_req->req.complete(&ep->ep,
							     &hs_req->req);
				}

				/* If we have pending request, then start it */
				restart = !list_empty(&ep->queue);
				if (restart) {
					hs_req = get_ep_head(ep);
					cx_hsotg_start_req(hsotg, ep,
							    hs_req, false);
				}
			}

			break;

		default:
			return -ENOENT;
		}
	} else
		return -ENOENT;  /* currently only deal with endpoint */

	return 1;
}

/**
 * cx_hsotg_stall_ep0 - stall ep0
 * @hsotg: The device state
 *
 * Set stall for ep0 as response for setup request.
 */
static void cx_hsotg_stall_ep0(struct cx_hsotg *hsotg)
{
	struct cx_hsotg_ep *ep0 = &hsotg->eps[0];
	u32 reg;
	u32 ctrl;

	usbd("%s(dir=%d)\n", __func__, ep0->dir_in);
	reg = (ep0->dir_in) ? DIEPCTL0 : DOEPCTL0;

	/*
	 * DxEPCTL_Stall will be cleared by EP once it has
	 * taken effect, so no need to clear later.
	 */

	ctrl = readl(hsotg->regs + reg);
	ctrl |= DXEPCTL_STALL;
	ctrl |= DXEPCTL_CNAK;
	writel(ctrl, hsotg->regs + reg);

	usbd("written DXEPCTL=0x%08x to %08x (DXEPCTL=0x%08x)\n",
		ctrl, reg, readl(hsotg->regs + reg));

	 /*
	  * complete won't be called, so we enqueue
	  * setup request here
	  */
	 cx_hsotg_enqueue_setup(hsotg);
}

/**
 * cx_hsotg_process_control - process a control request
 * @hsotg: The device state
 * @ctrl: The control request received
 *
 * The controller has received the SETUP phase of a control request, and
 * needs to work out what to do next (and whether to pass it on to the
 * gadget driver).
 */
static void cx_hsotg_process_control(struct cx_hsotg *hsotg,
				      struct usb_ctrlrequest *ctrl)
{
	struct cx_hsotg_ep *ep0 = &hsotg->eps[0];
	int ret = 0;
	u32 dcfg;

	ep0->sent_zlp = 0;

	usbd("%s:\n" "ctrl Req=%02x, Type=%02x, V=%04x, L=%04x\n",
		 __func__, ctrl->bRequest, ctrl->bRequestType,
		 ctrl->wValue, ctrl->wLength);

	/*
	 * record the direction of the request, for later use when enquing
	 * packets onto EP0.
	 */

	ep0->dir_in = (ctrl->bRequestType & USB_DIR_IN) ? 1 : 0;
	usbd("ctrl: dir_in=%d\n", ep0->dir_in);

	/*
	 * if we've no data with this request, then the last part of the
	 * transaction is going to implicitly be IN.
	 */
	if (ctrl->wLength == 0)
		ep0->dir_in = 1;

	if ((ctrl->bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
		switch (ctrl->bRequest) {
		case USB_REQ_SET_ADDRESS:
			dcfg = readl(hsotg->regs + DCFG);
			dcfg &= ~DCFG_DEVADDR_MASK;
			dcfg |= (le16_to_cpu(ctrl->wValue) <<
				 DCFG_DEVADDR_SHIFT) & DCFG_DEVADDR_MASK;
			writel(dcfg, hsotg->regs + DCFG);

			usbd("new address %d\n", ctrl->wValue);

			ret = cx_hsotg_send_reply(hsotg, ep0, NULL, 0);
			return;

		case USB_REQ_GET_STATUS:
			ret = cx_hsotg_process_req_status(hsotg, ctrl);
			break;

		case USB_REQ_CLEAR_FEATURE:
		case USB_REQ_SET_FEATURE:
			ret = cx_hsotg_process_req_feature(hsotg, ctrl);
			break;
		}
	}

	/* as a fallback, try delivering it to the driver to deal with */

	if (ret == 0 && hsotg->driver) {
		ret = hsotg->driver->setup(&hsotg->gadget, ctrl);
		if (ret < 0)
			usbd("driver->setup() ret %d\n", ret);
	}

	/*
	 * the request is either unhandlable, or is not formatted correctly
	 * so respond with a STALL for the status stage to indicate failure.
	 */

	if (ret < 0)
		cx_hsotg_stall_ep0(hsotg);
}

/**
 * cx_hsotg_rx_data - receive data from the FIFO for an endpoint
 * @hsotg: The device state.
 * @ep_idx: The endpoint index for the data
 * @size: The size of data in the fifo, in bytes
 *
 * The FIFO status shows there is data to read from the FIFO for a given
 * endpoint, so sort out whether we need to read the data into a request
 * that has been made for that endpoint.
 */
static void cx_hsotg_rx_data(struct cx_hsotg *hsotg, int ep_idx, int size)
{
	struct cx_hsotg_ep *hs_ep = &hsotg->eps[ep_idx];
	struct cx_hsotg_req *hs_req = hs_ep->req;
	void __iomem *fifo = hsotg->regs + EPFIFO(ep_idx);
	int to_read;
	int max_req;
	int read_ptr;


	if (!hs_req) {
		u32 epctl = readl(hsotg->regs + DOEPCTL(ep_idx));
		int ptr;

		printf("%s: FIFO %d bytes on ep%d but no req (DXEPCTl=0x%08x)\n",
			 __func__, size, ep_idx, epctl);

		/* dump the data from the FIFO, we've nothing we can do */
		for (ptr = 0; ptr < size; ptr += 4)
			(void)readl(fifo);

		return;
	}

	to_read = size;
	read_ptr = hs_req->req.actual;
	max_req = hs_req->req.length - read_ptr;

	usbd("%s: read %d/%d, done %d/%d\n",
		__func__, to_read, max_req, read_ptr, hs_req->req.length);

	if (to_read > max_req) {
		/*
		 * more data appeared than we where willing
		 * to deal with in this request.
		 */
		printf("more data appeared to deal with\n");
	}

	hs_ep->total_data += to_read;
	hs_req->req.actual += to_read;
	to_read = DIV_ROUND_UP(to_read, 4);

	/*
	 * note, we might over-write the buffer end by 3 bytes depending on
	 * alignment of the data.
	 */
	cx_pop_data(fifo, hs_req->req.buf + read_ptr, to_read);
}

/**
 * cx_hsotg_send_zlp - send zero-length packet on control endpoint
 * @hsotg: The device instance
 * @req: The request currently on this endpoint
 *
 * Generate a zero-length IN packet request for terminating a SETUP
 * transaction.
 *
 * Note, since we don't write any data to the TxFIFO, then it is
 * currently believed that we do not need to wait for any space in
 * the TxFIFO.
 */
static void cx_hsotg_send_zlp(struct cx_hsotg *hsotg,
			       struct cx_hsotg_req *req)
{
	u32 ctrl;

	usbd("%s\n", __func__);

	if (!req) {
		printf("%s: no request?\n", __func__);
		return;
	}

	if (req->req.length == 0) {
		hsotg->eps[0].sent_zlp = 1;
		cx_hsotg_enqueue_setup(hsotg);
		return;
	}

	hsotg->eps[0].dir_in = 1;
	hsotg->eps[0].sent_zlp = 1;

	usbd("sending zero-length packet\n");

	/* issue a zero-sized packet to terminate this */
	writel(DXEPTSIZ_MC(1) | DXEPTSIZ_PKTCNT(1) |
	       DXEPTSIZ_XFERSIZE(0), hsotg->regs + DIEPTSIZ(0));

	ctrl = readl(hsotg->regs + DIEPCTL0);
	ctrl |= DXEPCTL_CNAK;  /* clear NAK set by core */
	ctrl |= DXEPCTL_EPENA; /* ensure ep enabled */
	ctrl |= DXEPCTL_USBACTEP;
	writel(ctrl, hsotg->regs + DIEPCTL0);
}

/**
 * cx_hsotg_handle_outdone - handle receiving OutDone/SetupDone from RXFIFO
 * @hsotg: The device instance
 * @epnum: The endpoint received from
 * @was_setup: Set if processing a SetupDone event.
 *
 * The RXFIFO has delivered an OutDone event, which means that the data
 * transfer for an OUT endpoint has been completed, either by a short
 * packet or by the finish of a transfer.
 */
static void cx_hsotg_handle_outdone(struct cx_hsotg *hsotg,
				     int epnum, bool was_setup)
{
	u32 epsize = readl(hsotg->regs + DOEPTSIZ(epnum));
	struct cx_hsotg_ep *hs_ep = &hsotg->eps[epnum];
	struct cx_hsotg_req *hs_req = hs_ep->req;
	struct usb_request *req = &hs_req->req;
	unsigned size_left = DXEPTSIZ_XFERSIZE_GET(epsize);
	int result = 0;

	usbd("%s: %d@%s packet\n", __func__, 
			epnum, was_setup ? "setup" : "out");

	if (!hs_req) {
		usbd("%s: no request active\n", __func__);
		return;
	}

	/* if there is more request to do, schedule new transfer */
	if (req->actual < req->length && size_left == 0) {
		cx_hsotg_start_req(hsotg, hs_ep, hs_req, true);
		return;
	} else if (epnum == 0) {
		/*
		 * After was_setup = 1 =>
		 * set CNAK for non Setup requests
		 */
		hsotg->setup = was_setup ? 0 : 1;
	}

	if (req->actual < req->length && req->short_not_ok) {
		usbd("%s: got %d/%d (short not ok) => error\n",
			__func__, req->actual, req->length);

		/*
		 * todo - what should we return here? there's no one else
		 * even bothering to check the status.
		 */
	}

	if (epnum == 0) {
		/*
		 * Condition req->complete != cx_hsotg_complete_setup says:
		 * send ZLP when we have an asynchronous request from gadget
		 */
		if (!was_setup && req->complete != cx_hsotg_complete_setup)
			cx_hsotg_send_zlp(hsotg, hs_req);
	}

	cx_hsotg_complete_request(hsotg, hs_ep, hs_req, result);
}

/**
 * cx_hsotg_read_frameno - read current frame number
 * @hsotg: The device instance
 *
 * Return the current frame number
 */
static u32 cx_hsotg_read_frameno(struct cx_hsotg *hsotg)
{
	u32 dsts;

	dsts = readl(hsotg->regs + DSTS);
	dsts &= DSTS_SOFFN_MASK;
	dsts >>= DSTS_SOFFN_SHIFT;

	return dsts;
}

/**                           
 * check out token handle. 
 */                                                                         
static bool cx_hsotg_intkn_handle(struct cx_hsotg *hsotg, int ep_idx)          
{                                                                
	struct cx_hsotg_ep *hs_ep = &hsotg->eps[ep_idx];                    
	struct cx_hsotg_req *hs_req = hs_ep->req;                           

	if (!hs_req) {                                                      
		usbd(
			"%s-->ep%d has no request atached\n", __func__, ep_idx);
		cx_hsotg_enqueue_setup(hsotg);                              
	} else if (hs_ep->dir_in) {
		/*
		 * Setup packet is high priority, can result
		 * ep0-in NAK
		 */
		usbd("%s-->ep%d in token handle",
				__func__, ep_idx);
		return true;
	}

	return false;
}                                                                          


/**
 * cx_hsotg_handle_rx - RX FIFO has data
 * @hsotg: The device instance
 *
 * The IRQ handler has detected that the RX FIFO has some data in it
 * that requires processing, so find out what is in there and do the
 * appropriate read.
 *
 * The RXFIFO is a true FIFO, the packets coming out are still in packet
 * chunks, so if you have x packets received on an endpoint you'll get x
 * FIFO events delivered, each with a packet's worth of data in it.
 *
 * When using DMA, we should not be processing events from the RXFIFO
 * as the actual data should be sent to the memory directly and we turn
 * on the completion interrupts to get notifications of transfer completion.
 */
static void cx_hsotg_handle_rx(struct cx_hsotg *hsotg)
{
	u32 grxstsr = readl(hsotg->regs + GRXSTSP);
	u32 epnum, status, size;
	u8 mpid;
	char *dpid[] = { "DATA0", "DATA1", "DATA2", "MDATA", NULL};

	epnum = grxstsr & GRXSTS_EPNUM_MASK;
	status = grxstsr & GRXSTS_PKTSTS_MASK;

	size = grxstsr & GRXSTS_BYTECNT_MASK;
	size >>= GRXSTS_BYTECNT_SHIFT;

	mpid = grxstsr & GRXSTS_DPID_MASK;
	mpid >>= GRXSTS_DPID_SHIFT;

	usbd("%s: GRXSTSP=0x%08x (%d@%d)\n",
			__func__, grxstsr, size, epnum);

	switch ((status & GRXSTS_PKTSTS_MASK) >> GRXSTS_PKTSTS_SHIFT) {
	case GRXSTS_PKTSTS_GLOBALOUTNAK:
		usbd("GLOBALOUTNAK\n");
		break;

	case GRXSTS_PKTSTS_OUTDONE:
		usbd("OutDone (Frame=0x%08x)\n",
			cx_hsotg_read_frameno(hsotg));

		cx_hsotg_handle_outdone(hsotg, epnum, false);
		break;

	case GRXSTS_PKTSTS_SETUPDONE:
		usbd("SetupDone (Frame=0x%08x, DOPEPCTL=0x%08x)\n",
			cx_hsotg_read_frameno(hsotg),
			readl(hsotg->regs + DOEPCTL(0)));

		cx_hsotg_handle_outdone(hsotg, epnum, true);
		break;

	case GRXSTS_PKTSTS_OUTRX:
		if (!size) {
			usbd("%s zlp recv\n", dpid[mpid]);
			return;
		}

		/*
		 * pop receive data.
		 */
		cx_hsotg_rx_data(hsotg, epnum, size);
		break;

	case GRXSTS_PKTSTS_SETUPRX:
		usbd("SetupRX (Frame=0x%08x, DOPEPCTL=0x%08x)\n",
			cx_hsotg_read_frameno(hsotg),
			readl(hsotg->regs + DOEPCTL(0)));
		cx_hsotg_rx_data(hsotg, epnum, size);
		break;

	default:
		printf("%s: Invalid Packet Status (%08x)\n",
			 __func__, grxstsr);
		break;
	}
}

/**
 * usb_ep_set_maxpacket_limit - set maximum packet size limit for endpoint
 * @ep: endpoint being configured
 * @maxpacket_limit: value of maximum packet size limit
 *
 * This function should be used only in udc drivers to initialie endpoint
 * (usually in prove function.
 */
static inline void usb_ep_set_maxpacket_limit(struct usb_ep *ep, 
		unsigned maxpacket_limit)
{
	ep->maxpacket = maxpacket_limit;
}

/**
 * cx_hsotg_ep0_mps - turn max packet size into register setting
 * @mps: The maximum packet size in bytes.
 */
static u32 cx_hsotg_ep0_mps(unsigned int mps)
{
	switch (mps) {
	case 64:
		return D0EPCTL_MPS_64;
	case 32:
		return D0EPCTL_MPS_32;
	case 16:
		return D0EPCTL_MPS_16;
	case 8:
		return D0EPCTL_MPS_8;
	}

	/* bad max packet size, warn and return invalid result */
	return (u32)-1;
}

/**
 * cx_hsotg_set_ep_maxpacket - set endpoint's max-packet field
 * @hsotg: The driver state.
 * @ep: The index number of the endpoint
 * @mps: The maximum packet size in bytes
 *
 * Configure the maximum packet size for the given endpoint, updating
 * the hardware control registers to reflect this.
 */
static void cx_hsotg_set_ep_maxpacket(struct cx_hsotg *hsotg,
				       unsigned int ep, unsigned int mps)
{
	struct cx_hsotg_ep *hs_ep = &hsotg->eps[ep];
	void __iomem *regs = hsotg->regs;
	u32 mpsval;
	u32 mcval;
	u32 reg;

	if (ep == 0) {
		/* EP0 is a special case */
		mpsval = cx_hsotg_ep0_mps(mps);
		if (mpsval > 3)
			goto bad_mps;
		hs_ep->ep.maxpacket = mps;
		hs_ep->mc = 1;
	} else {
		mpsval = mps & DXEPCTL_MPS_MASK;
		if (mpsval > 1024)
			goto bad_mps;
		mcval = ((mps >> 11) & 0x3) + 1; /* Maximum Packet Size */
		hs_ep->mc = mcval;
		if (mcval > 3)
			goto bad_mps;
		hs_ep->ep.maxpacket = mpsval;
	}

	/*
	 * update both the in and out endpoint controldir_ registers, even
	 * if one of the directions may not be in use.
	 */

	reg = readl(regs + DIEPCTL(ep));
	reg &= ~DXEPCTL_MPS_MASK;
	reg |= mpsval;
	writel(reg, regs + DIEPCTL(ep));

	if (ep) {
		reg = readl(regs + DOEPCTL(ep));
		reg &= ~DXEPCTL_MPS_MASK;
		reg |= mpsval;
		writel(reg, regs + DOEPCTL(ep));
	}

	return;

bad_mps:
	printf("ep%d: bad mps of %d\n", ep, mps);
}

/**
 * cx_hsotg_trytx - check to see if anything needs transmitting
 * @hsotg: The driver state
 * @hs_ep: The driver endpoint to check.
 *
 * Check to see if there is a request that has data to send, and if so
 * make an attempt to write data into the FIFO.
 */
static int cx_hsotg_trytx(struct cx_hsotg *hsotg,
			   struct cx_hsotg_ep *hs_ep)
{
	struct cx_hsotg_req *hs_req = hs_ep->req;

	usbdq("%s %s@%d\n", __func__,
			hs_ep->name, hs_ep->index);

	if (!hs_ep->dir_in || !hs_req) {
		/**
		 * if request is not enqueued, we disable interrupts
		 * for endpoints, excepting ep0
		 */
		if (hs_ep->index != 0)
			cx_hsotg_ctrl_epint(hsotg, hs_ep->index,
					     hs_ep->dir_in, 0);
		return 0;
	}

	if (hs_req->req.actual < hs_req->req.length) {
		usbd("trying to write more for ep%d\n",
			hs_ep->index);
		return cx_hsotg_write_fifo(hsotg, hs_ep, hs_req);
	}

	return 0;
}

/**
 * cx_hsotg_complete_in - complete IN transfer
 * @hsotg: The device state.
 * @hs_ep: The endpoint that has just completed.
 *
 * An IN transfer has been completed, update the transfer's state and then
 * call the relevant completion routines.
 */
static void cx_hsotg_complete_in(struct cx_hsotg *hsotg,
				  struct cx_hsotg_ep *hs_ep)
{
	struct cx_hsotg_req *hs_req = hs_ep->req;
	u32 epsize = readl(hsotg->regs + DIEPTSIZ(hs_ep->index));
	int size_left, size_done;

	usbd("%s %s req %p\n", __func__, hs_ep->name, hs_req);

	if (!hs_req) {
		usbd("XferCompl but no req\n");
		return;
	}

	/* Finish ZLP handling for IN EP0 transactions */
	if (hsotg->eps[0].sent_zlp) {
		usbd("zlp packet received\n");
		cx_hsotg_complete_request(hsotg, hs_ep, hs_req, 0);
		return;
	}

	/*
	 * Calculate the size of the transfer by checking how much is left
	 * in the endpoint size register and then working it out from
	 * the amount we loaded for the transfer.
	 *
	 * We do this even for DMA, as the transfer may have incremented
	 * past the end of the buffer (DMA transfers are always 32bit
	 * aligned).
	 */

	size_left = DXEPTSIZ_XFERSIZE_GET(epsize);

	size_done = hs_ep->size_loaded - size_left;
	size_done += hs_ep->last_load;

	if (hs_req->req.actual != size_done)
		usbd("%s: adjusting size done %d => %d\n",
			__func__, hs_req->req.actual, size_done);

	hs_req->req.actual = size_done;
	usbd("req->length:%d req->actual:%d req->zero:%d\n",
		hs_req->req.length, hs_req->req.actual, hs_req->req.zero);

	/*
	 * Check if dealing with Maximum Packet Size(MPS) IN transfer at EP0
	 * When sent data is a multiple MPS size (e.g. 64B ,128B ,192B
	 * ,256B ... ), after last MPS sized packet send IN ZLP packet to
	 * inform the host that no more data is available.
	 * The state of req.zero member is checked to be sure that the value to
	 * send is smaller than wValue expected from host.
	 * Check req.length to NOT send another ZLP when the current one is
	 * under completion (the one for which this completion has been called).
	 */
	if (hs_req->req.length && hs_ep->index == 0 && hs_req->req.zero &&
	    hs_req->req.length == hs_req->req.actual &&
	    !(hs_req->req.length % hs_ep->ep.maxpacket)) {
		usbd("ep0 zlp IN packet sent\n");
		cx_hsotg_send_zlp(hsotg, hs_req);

		return;
	}

	if (!size_left && hs_req->req.actual < hs_req->req.length) {
		usbd("%s trying more for req...\n", __func__);
		cx_hsotg_start_req(hsotg, hs_ep, hs_req, true);
	} else
		cx_hsotg_complete_request(hsotg, hs_ep, hs_req, 0);
}

/**
 * cx_hsotg_epint - handle an in/out endpoint interrupt
 * @hsotg: The driver state
 * @idx: The index for the endpoint (0..15)
 * @dir_in: Set if this is an IN endpoint
 *
 * Process and clear any interrupt pending for an individual endpoint
 */
static void cx_hsotg_epint(struct cx_hsotg *hsotg, unsigned int idx,
			    int dir_in)
{
	struct cx_hsotg_ep *hs_ep = &hsotg->eps[idx];
	u32 epint_reg = dir_in ? DIEPINT(idx) : DOEPINT(idx);
	u32 epctl_reg = dir_in ? DIEPCTL(idx) : DOEPCTL(idx);
	u32 epsiz_reg = dir_in ? DIEPTSIZ(idx) : DOEPTSIZ(idx);
	u32 ints;
	u32 ctrl;

	ints = readl(hsotg->regs + epint_reg);
	ctrl = readl(hsotg->regs + epctl_reg);

	/* Clear endpoint interrupts */
	writel(ints, hsotg->regs + epint_reg);

	usbd("%s: ep%d(%s) DxEPINT=0x%08x\n",
		__func__, idx, dir_in ? "in" : "out", ints);

	if (ints & DXEPINT_XFERCOMPL) {
		if (hs_ep->isochronous && hs_ep->interval == 1) {
			if (ctrl & DXEPCTL_EOFRNUM)
				ctrl |= DXEPCTL_SETEVENFR;
			else
				ctrl |= DXEPCTL_SETODDFR;
			writel(ctrl, hsotg->regs + epctl_reg);
		}

		usbd("%s: XferCompl: DxEPCTL=0x%08x, DXEPTSIZ=%08x\n",
			__func__, readl(hsotg->regs + epctl_reg),
			readl(hsotg->regs + epsiz_reg));

		/*
		 * we get OutDone from the FIFO, so we only need to look
		 * at completing IN requests here
		 */
		if (dir_in) {
			cx_hsotg_complete_in(hsotg, hs_ep);

			if (idx == 0 && !hs_ep->req) {
				usbd("%s(%d) in token completed\n",
						__func__, __LINE__);
				cx_hsotg_enqueue_setup(hsotg);
			}
		}
	}

	if (ints & DXEPINT_EPDISBLD) {
		usbd("%s: EPDisbld\n", __func__);

		if (dir_in) {
			int epctl = readl(hsotg->regs + epctl_reg);

			cx_hsotg_txfifo_flush(hsotg, idx);

			if ((epctl & DXEPCTL_STALL) &&
				(epctl & DXEPCTL_EPTYPE_BULK)) {
				int dctl = readl(hsotg->regs + DCTL);

				dctl |= DCTL_CGNPINNAK;
				writel(dctl, hsotg->regs + DCTL);
			}
		}
	}

	if (ints & DXEPINT_AHBERR)
		usbd("%s: EP%d AHB Error\n", __func__, idx);

	if (ints & DXEPINT_SETUP) {  /* Setup or Timeout */
		if (idx == 0) {
			if (dir_in) {
				struct cx_hsotg_req *hs_req = hs_ep->req;

				usbd("%s: EP%d-in Time-out\n",  __func__, idx);

				/*
				 * This is done to take into account the TIMEOUT due to
				 * lost ACK case (DWC_otg did not receive the 
				 * ACK send by the host)
				 */
				if (cx_hsotg_intkn_handle(hsotg, idx)) {
					usbd("IN Token lost ACK\n");

					cx_hsotg_complete_request(hsotg, 
							hs_ep, hs_req, -ETIMEDOUT);
					/*
					 * setup enqueue
					 */
					cx_hsotg_enqueue_setup(hsotg);
				}
			} else {
				/*
				 * Indicates that the SETUP phase for the control 
				 * endpoint is complete, the application can
				 * decode the received SETUP data packet.
				 */
				struct cx_hsotg_req *hs_req = hs_ep->req;

				usbd("%s: EP%d-out Setup\n",  __func__, idx);

				usbd("hs_req %p , hs_ep %p\n", hs_req, hs_ep);

				if (hs_req->req.complete == cx_hsotg_complete_setup)
					hs_req->req.complete(&hs_ep->ep, &hs_req->req);
			}
		}
	}

	if (ints & DXEPINT_BACK2BACKSETUP) {
		usbd("%s: EP%d B2BSetup/INEPNakEff\n", __func__, idx);
	}

	if (dir_in && !hs_ep->isochronous) {
		/* not sure if this is important, but we'll clear it anyway */
		if (ints & DIEPMSK_INTKNTXFEMPMSK) {
			usbd("%s: ep%d: INTknTXFEmpMsk\n",
				__func__, idx);
		}

		/* this probably means something bad is happening */
		if (ints & DIEPMSK_INTKNEPMISMSK) {
			printf("%s: ep%d: INTknEP\n", __func__, idx);
		}

		/* FIFO has space or is empty (see GAHBCFG) */
		if (hsotg->dedicated_fifos &&
		    ints & DIEPMSK_TXFIFOEMPTY) {
			usbd("%s: ep%d: TxFIFOEmpty\n", __func__, idx);
			cx_hsotg_trytx(hsotg, hs_ep);
		}
	}
}

/**
 * cx_hsotg_irq_enumdone - Handle EnumDone interrupt (enumeration done)
 * @hsotg: The device state.
 *
 * Handle updating the device settings after the enumeration phase has
 * been completed.
 */
static void cx_hsotg_irq_enumdone(struct cx_hsotg *hsotg)
{
	u32 dsts = readl(hsotg->regs + DSTS);
	int ep0_mps = 0, ep_mps = 8;

	/*
	 * This should signal the finish of the enumeration phase
	 * of the USB handshaking, so we should now know what rate
	 * we connected at.
	 */

	usbd("%s(DSTS=0x%08x)\n", __func__, dsts);

	/*
	 * note, since we're limited by the size of transfer on EP0, and
	 * it seems IN transfers must be a even number of packets we do
	 * not advertise a 64byte MPS on EP0.
	 */

	/* catch both EnumSpd_FS and EnumSpd_FS48 */
	switch (dsts & DSTS_ENUMSPD_MASK) {
	case DSTS_ENUMSPD_FS:
	case DSTS_ENUMSPD_FS48:
		hsotg->gadget.speed = USB_SPEED_FULL;
		ep0_mps = EP0_MPS_LIMIT;
		ep_mps = 64;
		break;

	case DSTS_ENUMSPD_HS:
		hsotg->gadget.speed = USB_SPEED_HIGH;
		ep0_mps = EP0_MPS_LIMIT;
		ep_mps = 512;
		break;

	case DSTS_ENUMSPD_LS:
		hsotg->gadget.speed = USB_SPEED_LOW;
		/*
		 * note, we don't actually support LS in this driver at the
		 * moment, and the documentation seems to imply that it isn't
		 * supported by the PHYs on some of the devices.
		 */
		break;
	}

	usbd("new device is %s\n", usb_speed_string(hsotg->gadget.speed));

	/*
	 * we should now know the maximum packet size for an
	 * endpoint, so set the endpoints to a default value.
	 */

	if (ep0_mps) {
		int i;
		cx_hsotg_set_ep_maxpacket(hsotg, 0, ep0_mps);
		for (i = 1; i < hsotg->num_of_eps; i++)
			cx_hsotg_set_ep_maxpacket(hsotg, i, ep_mps);
	}

	/* 
	 * ensure after hardware enumeration our EP0 is active 
	 * DEBUG: prev request is handling, but we receive a core 
	 * enumdone irq, in this case, we should kill all request in
	 * EP0.
	 */
	cx_hsotg_enqueue_setup(hsotg);

	usbd("%s(%d) EP0: DIEPCTL0=0x%08x, DOEPCTL0=0x%08x\n",
		__func__, __LINE__,
		readl(hsotg->regs + DIEPCTL0),
		readl(hsotg->regs + DOEPCTL0));
}

/**
 * cx_hsotg_disconnect - disconnect service
 * @hsotg: The device state.
 *
 * The device has been disconnected. Remove all current
 * transactions and signal the gadget driver that this
 * has happened.
 */
static void cx_hsotg_disconnect(struct cx_hsotg *hsotg)
{
	struct usb_gadget_driver *driver = hsotg->driver;
	unsigned ep;

	usbd("%s\n", __func__);

	for (ep = 0; ep < hsotg->num_of_eps; ep++)
		kill_all_requests(hsotg, &hsotg->eps[ep], -ESHUTDOWN, true);

	/* call gadget core */
	driver->disconnect(&hsotg->gadget);
}

/**
 * cx_hsotg_irq_fifoempty - TX FIFO empty interrupt handler
 * @hsotg: The device state:
 * @periodic: True if this is a periodic FIFO interrupt
 */
static void cx_hsotg_irq_fifoempty(struct cx_hsotg *hsotg, bool periodic)
{
	struct cx_hsotg_ep *ep;
	int epno, ret;

	/* look through for any more data to transmit */
	for (epno = 0; epno < hsotg->num_of_eps; epno++) {
		ep = &hsotg->eps[epno];

		if (!ep->dir_in)
			continue;

		if ((periodic && !ep->periodic) ||
		    (!periodic && ep->periodic))
			continue;

		ret = cx_hsotg_trytx(hsotg, ep);
		if (ret < 0)
			break;
	}
}

/* IRQ flags which will trigger a retry around the IRQ loop */
#define IRQ_RETRY_MASK (GINTSTS_NPTXFEMP | \
			GINTSTS_PTXFEMP |  \
			GINTSTS_RXFLVL)

/**
 * cx_hsotg_corereset - issue softreset to the core
 * @hsotg: The device state
 *
 * Issue a soft reset to the core, and await the core finishing it.
 */
static int cx_hsotg_corereset(struct cx_hsotg *hsotg)
{
	int timeout = 10000;
	u32 grstctl;

	usbd("%s -- resetting core\n", __func__);

	/* issue soft reset */
	writel(GRSTCTL_CSFTRST, hsotg->regs + GRSTCTL);
	do {
		grstctl = readl(hsotg->regs + GRSTCTL);
	} while ((grstctl & GRSTCTL_CSFTRST) && timeout-- > 0);

	if (grstctl & GRSTCTL_CSFTRST) {
		printf("core software reset Timeouted\n");
		return -ETIMEDOUT;
	}

	/* second check AHB master state machine in idle state */
	timeout = 10000;
	while (1) {
		u32 grstctl = readl(hsotg->regs + GRSTCTL);

		/* AHB master state machine in idle state, out */
		if (grstctl & GRSTCTL_AHBIDLE)
			break;

		if (timeout-- < 0) {
			printf("AHB master state machine not idle state\n");
			return -ETIMEDOUT;
		}
	}

	usbd("reset successful\n");
	return 0;
}

/**
 * cx_hsotg_core_init - issue softreset to the core
 * @hsotg: The device state
 *
 * Issue a soft reset to the core, and await the core finishing it.
 */
static void cx_hsotg_core_init(struct cx_hsotg *hsotg)
{
	struct cx_hsotg_plat *plat = hsotg->plat;
	uint8_t turnaround_time = 0;

	usbd("%s\n", __func__);
	cx_hsotg_corereset(hsotg);

	/* set the PLL on, remove the HNP/SRP and set the PHY */
	writel(hsotg->force |	/* force mode or not */
	       0 << 15 | /* PHY Low-Power Clock Select */
	       turnaround_time << 10 | /* USB Turnaround Time */
	       0 << 9 | /* HNP-Capable disable */
	       0 << 8 | /* SRP-Capable disable */
	       0 << 7 | /* ULPI DDR Select: 8-bit-wide data bus */
	       0 << 6 | /* 0: high speed utmi+, 1: full speed serial */
	       plat->phy_type << 4 | /* 0: utmi+, 1:ulpi */
	       hsotg->phyif << 3 | /* phy i/f  0:8bit, 1:16bit */
	   //    GUSBCFG_TOUTCAL(0x7) << 0, /* HS/FS Timeout Calibration */
	       GUSBCFG_TOUTCAL(0x2) << 0, /* HS/FS Timeout Calibration */
	       hsotg->regs + GUSBCFG /* usb config */
	);

	/* FIFO initialization */
	cx_hsotg_init_fifo(hsotg);

	/* let's usb host send SE0 */
	__orr32(hsotg->regs + DCTL, DCTL_SFTDISCON);

	/* high speed otg device conf */
	writel(1 << 18 | DCFG_DEVSPD_HS,  hsotg->regs + DCFG);

	/* Clear any pending OTG interrupts */
	writel(0xFFFFFFFF, hsotg->regs + GOTGINT);

	/* Clear any pending interrupts */
	writel(0xFFFFFFFF, hsotg->regs + GINTSTS);

	writel(1 << 7 | /* Non-Periodic TxFIFO Empty Level: half */
	       0 << 5 | /* no dma mode */
	       0 << 4 | /* Burst Length: 1 word */
	       0 << 0, /* Global Interrupt mask */
	       hsotg->regs + GAHBCFG /* usb AHB config */
	);

	/*
	 * If INTknTXFEmpMsk is enabled, it's important to disable ep interrupts
	 * when we have no data to transfer. Otherwise we get being flooded by        
	 * interrupts.
	 */

	writel(((hsotg->dedicated_fifos) ? DIEPMSK_TXFIFOEMPTY | 
			DIEPMSK_INTKNTXFEMPMSK : 0) | DIEPMSK_EPDISBLDMSK | 
			DIEPMSK_XFERCOMPLMSK | DIEPMSK_TIMEOUTMSK | 
			DIEPMSK_AHBERRMSK | DIEPMSK_INTKNEPMISMSK,
			hsotg->regs + DIEPMSK
	);

	/*
	 * don't need XferCompl, we get that from RXFIFO in slave mode. In
	 * DMA mode we may need this.
	 */
	writel(DOEPMSK_EPDISBLDMSK | DOEPMSK_AHBERRMSK | DOEPMSK_SETUPMSK,
			hsotg->regs + DOEPMSK
	);

	writel(0, hsotg->regs + DAINTMSK);

	usbd("EP0: DIEPCTL0=0x%08x, DOEPCTL0=0x%08x\n",
		readl(hsotg->regs + DIEPCTL0),
		readl(hsotg->regs + DOEPCTL0));

	/* Enable interrupts for EP0 in and out */
	cx_hsotg_ctrl_epint(hsotg, 0, 0, 1);
	cx_hsotg_ctrl_epint(hsotg, 0, 1, 1);

	__orr32(hsotg->regs + DCTL, DCTL_PWRONPRGDONE);
	udelay(10);  /* see openiboot */
	__bic32(hsotg->regs + DCTL, DCTL_PWRONPRGDONE);

	usbd("DCTL=0x%08x\n", readl(hsotg->regs + DCTL));

	/*
	 * DxEPCTL_USBActEp says RO in manual, but seems 
	 * to be set by writing to the EPCTL register, set 
	 * to read 1 8byte packet 
	 */
	writel(DXEPTSIZ_PKTCNT(1) | DXEPTSIZ_XFERSIZE(8), 
	       hsotg->regs + DOEPTSIZ0);

	writel(cx_hsotg_ep0_mps(hsotg->eps[0].ep.maxpacket) |
	       DXEPCTL_CNAK | DXEPCTL_EPENA |
	       DXEPCTL_USBACTEP,
	       hsotg->regs + DOEPCTL0);

	/* enable, but don't activate EP0in */
	writel(cx_hsotg_ep0_mps(hsotg->eps[0].ep.maxpacket) |
	       DXEPCTL_USBACTEP, hsotg->regs + DIEPCTL0);

	cx_hsotg_enqueue_setup(hsotg);

	usbd("%s(%d) EP0: DIEPCTL0=0x%08x, DOEPCTL0=0x%08x\n",
			__func__, __LINE__,
		readl(hsotg->regs + DIEPCTL0),
		readl(hsotg->regs + DOEPCTL0));

	/* clear global NAKs */
	writel(DCTL_CGOUTNAK | DCTL_CGNPINNAK,
	       hsotg->regs + DCTL);

	/* must be at-least 3ms to allow bus to see disconnect */
	mdelay(3);

	/* remove the soft-disconnect and let's go */
	__bic32(hsotg->regs + DCTL, DCTL_SFTDISCON);
}

int usb_gadget_cable_connected(void)
{
	u32 usb_status = readl(hsotg->regs + GOTGCTL);
	if (usb_status & GOTGCTL_BSESVLD)
		return 1;
	return 0;
}

/**
 * usb_gadget_handle_interrupts - handle device interrupt
 */
int usb_gadget_handle_interrupts(void)
{
	u32 gintsts;

irq_retry:
	gintsts = readl(hsotg->regs + GINTSTS);
	if (gintsts & GINTSTS_OTGINT) {
		u32 otgint = readl(hsotg->regs + GOTGINT);
		writel(otgint, hsotg->regs + GOTGINT);
	}

	if (gintsts & GINTSTS_USBRST) {
		u32 usb_status = readl(hsotg->regs + GOTGCTL);

		writel(GINTSTS_USBRST, hsotg->regs + GINTSTS);
		if (usb_status & GOTGCTL_BSESVLD) {
			if (get_timer(0) - hsotg->last_rst  > 500000) {
				/*
				 * usb reset event indicate a new enumlation
				 */
				kill_all_requests(hsotg, &hsotg->eps[0],
						-ECONNRESET, true);
				cx_hsotg_core_init(hsotg);
				hsotg->last_rst = get_timer(0);;
			}
		}
	}

	if (gintsts & GINTSTS_DISCONNINT) {
		writel(GINTSTS_DISCONNINT,  hsotg->regs + GINTSTS);
		cx_hsotg_disconnect(hsotg);

	}

	if (gintsts & GINTSTS_SESSREQINT) {
		writel(GINTSTS_SESSREQINT, hsotg->regs + GINTSTS);
	}

	if (gintsts & GINTSTS_ENUMDONE) {
		writel(GINTSTS_ENUMDONE, hsotg->regs + GINTSTS);
		cx_hsotg_irq_enumdone(hsotg);
	}

	if (gintsts & GINTSTS_CONIDSTSCHNG) {
		writel(GINTSTS_CONIDSTSCHNG, hsotg->regs + GINTSTS);
	}

	if (gintsts & (GINTSTS_OEPINT | GINTSTS_IEPINT)) {
		u32 daint = readl(hsotg->regs + DAINT);
		u32 daintmsk = readl(hsotg->regs + DAINTMSK);
		u32 daint_out, daint_in;
		int ep;

		daint &= daintmsk;
		daint_out = daint >> DAINT_OUTEP_SHIFT;
		daint_in = daint & ~(daint_out << DAINT_OUTEP_SHIFT);

		for (ep = 0; ep < 15 && daint_out; ep++, daint_out >>= 1) {
			if (daint_out & 1)
				cx_hsotg_epint(hsotg, ep, 0);
		}

		for (ep = 0; ep < 15 && daint_in; ep++, daint_in >>= 1) {
			if (daint_in & 1)
				cx_hsotg_epint(hsotg, ep, 1);
		}
	}

	/* check both FIFOs */
	if (gintsts & GINTSTS_NPTXFEMP) {
		/*
		 * Disable the interrupt to stop it happening again
		 * unless one of these endpoint routines decides that
		 * it needs re-enabling
		 */
		cx_hsotg_irq_fifoempty(hsotg, false);
	}

	if (gintsts & GINTSTS_PTXFEMP) {
		/* See note in GINTSTS_NPTxFEmp */
		cx_hsotg_irq_fifoempty(hsotg, true);
	}

	if (gintsts & GINTSTS_RXFLVL) {
		/*
		 * note, since GINTSTS_RxFLvl doubles as FIFO-not-empty,
		 * we need to retry cx_hsotg_handle_rx if this is still
		 * set.
		 */
		cx_hsotg_handle_rx(hsotg);
	}

	if (gintsts & GINTSTS_MODEMIS) {
		writel(GINTSTS_MODEMIS, hsotg->regs + GINTSTS);
	}

	if (gintsts & GINTSTS_USBSUSP) {
		writel(GINTSTS_USBSUSP, hsotg->regs + GINTSTS);
	}

	if (gintsts & GINTSTS_WKUPINT) {
		writel(GINTSTS_WKUPINT, hsotg->regs + GINTSTS);
	}

	if (gintsts & GINTSTS_ERLYSUSP) {
		writel(GINTSTS_ERLYSUSP, hsotg->regs + GINTSTS);
	}

	/*
	 * these next two seem to crop-up occasionally causing the core
	 * to shutdown the USB transfer, so try clearing them and logging
	 * the occurrence.
	 */

	if (gintsts & GINTSTS_GOUTNAKEFF) {
		writel(DCTL_CGOUTNAK, hsotg->regs + DCTL);
	}

	if (gintsts & GINTSTS_GINNAKEFF) {
		writel(DCTL_CGNPINNAK, hsotg->regs + DCTL);
	}

	return 0;
}

/**
 * cx_hsotg_ep_enable - enable the given endpoint
 * @ep: The USB endpint to configure
 * @desc: The USB endpoint descriptor to configure with.
 *
 * This is called from the USB gadget code's usb_ep_enable().
 */
static int cx_hsotg_ep_enable(struct usb_ep *ep,
			       const struct usb_endpoint_descriptor *desc)
{
	struct cx_hsotg_ep *hs_ep = our_ep(ep);
	struct cx_hsotg *hsotg = hs_ep->parent;
	int index = hs_ep->index;
	u32 epctrl_reg;
	u32 epctrl;
	u32 mps;
	int dir_in;
	int ret = 0;

	usbd("%s: ep %s: a 0x%02x, attr 0x%02x, mps 0x%04x, intr %d\n",
		__func__, ep->name, desc->bEndpointAddress, desc->bmAttributes,
		desc->wMaxPacketSize, desc->bInterval);

	dir_in = (desc->bEndpointAddress & USB_ENDPOINT_DIR_MASK) ? 1 : 0;
	if (dir_in != hs_ep->dir_in) {
		printf("%s: direction mismatch!\n", __func__);
		return -EINVAL;
	}

	mps = usb_endpoint_maxp(desc);

	/* note, we handle this here instead of cx_hsotg_set_ep_maxpacket */

	epctrl_reg = dir_in ? DIEPCTL(index) : DOEPCTL(index);
	epctrl = readl(hsotg->regs + epctrl_reg);

	usbd("%s: read DxEPCTL=0x%08x from 0x%08x\n", __func__, epctrl, epctrl_reg);

	epctrl &= ~(DXEPCTL_EPTYPE_MASK | DXEPCTL_MPS_MASK);
	epctrl |= DXEPCTL_MPS(mps);

	/*
	 * mark the endpoint as active, otherwise the core may ignore
	 * transactions entirely for this endpoint
	 */
	epctrl |= DXEPCTL_USBACTEP;

	/*
	 * set the NAK status on the endpoint, otherwise we might try and
	 * do something with data that we've yet got a request to process
	 * since the RXFIFO will take data for an endpoint even if the
	 * size register hasn't been set.
	 */

	epctrl |= DXEPCTL_SNAK;

	/* update the endpoint state */
	cx_hsotg_set_ep_maxpacket(hsotg, hs_ep->index, mps);

	/* default, set to non-periodic */
	hs_ep->isochronous = 0;
	hs_ep->periodic = 0;
	hs_ep->halted = 0;
	hs_ep->interval = desc->bInterval;

	if (hs_ep->interval > 1 && hs_ep->mc > 1)
		printf("MC > 1 when interval is not 1\n");

	switch (desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
	case USB_ENDPOINT_XFER_ISOC:
		epctrl |= DXEPCTL_EPTYPE_ISO;
		epctrl |= DXEPCTL_SETEVENFR;
		hs_ep->isochronous = 1;
		if (dir_in)
			hs_ep->periodic = 1;
		break;

	case USB_ENDPOINT_XFER_BULK:
		epctrl |= DXEPCTL_EPTYPE_BULK;
		break;

	case USB_ENDPOINT_XFER_INT:
		if (dir_in) {
			/*
			 * Allocate our TxFNum by simply using the index
			 * of the endpoint for the moment. We could do
			 * something better if the host indicates how
			 * many FIFOs we are expecting to use.
			 */

			hs_ep->periodic = 1;
			epctrl |= DXEPCTL_TXFNUM(index);
		}

		epctrl |= DXEPCTL_EPTYPE_INTERRUPT;
		break;

	case USB_ENDPOINT_XFER_CONTROL:
		epctrl |= DXEPCTL_EPTYPE_CONTROL;
		break;
	}

	/*
	 * if the hardware has dedicated fifos, we must give each IN EP
	 * a unique tx-fifo even if it is non-periodic.
	 */
	if (dir_in && hsotg->dedicated_fifos)
		epctrl |= DXEPCTL_TXFNUM(index);

	/* for non control endpoints, set PID to D0 */
	if (index)
		epctrl |= DXEPCTL_SETD0PID;

	usbd("%s: write DxEPCTL=0x%08x\n",
		__func__, epctrl);

	writel(epctrl, hsotg->regs + epctrl_reg);
	usbd("%s: read DxEPCTL=0x%08x\n",
		__func__, readl(hsotg->regs + epctrl_reg));

	/* enable the endpoint interrupt */
	cx_hsotg_ctrl_epint(hsotg, index, dir_in, 1);

	return ret;
}

/**
 * on_list - check request is on the given endpoint
 * @ep: The endpoint to check.
 * @test: The request to test if it is on the endpoint.
 */
static bool on_list(struct cx_hsotg_ep *ep, struct cx_hsotg_req *test)
{
	struct cx_hsotg_req *req, *treq;

	list_for_each_entry_safe(req, treq, &ep->queue, queue) {
		if (req == test)
			return true;
	}

	return false;
}

/**
 * cx_hsotg_ep_dequeue - dequeue given endpoint
 * @ep: The endpoint to dequeue.
 * @req: The request to be removed from a queue.
 */
static int cx_hsotg_ep_dequeue(struct usb_ep *ep, struct usb_request *req)
{
	struct cx_hsotg_req *hs_req = our_req(req);
	struct cx_hsotg_ep *hs_ep = our_ep(ep);
	struct cx_hsotg *hs = hs_ep->parent;

	usbd("%s(%p,%p)\n", __func__, ep, req);
	if (!on_list(hs_ep, hs_req)) {
		return -EINVAL;
	}

	cx_hsotg_complete_request(hs, hs_ep, hs_req, -ECONNRESET);

	return 0;
}

/**
 * cx_hsotg_ep_sethalt - set halt on a given endpoint
 * @ep: The endpoint to set halt.
 * @value: Set or unset the halt.
 */
static int cx_hsotg_ep_sethalt(struct usb_ep *ep, int value)
{
	struct cx_hsotg_ep *hs_ep = our_ep(ep);
	struct cx_hsotg *hs = hs_ep->parent;
	int index = hs_ep->index;
	u32 epreg;
	u32 epctl;
	u32 xfertype;

	usbd("%s(ep %p %s, %d)\n", __func__, ep, ep->name, value);

	if (index == 0) {
		if (value)
			cx_hsotg_stall_ep0(hs);
		else
			printf("%s: can't clear halt on ep0\n", __func__);
		return 0;
	}

	/* write both IN and OUT control registers */

	epreg = DIEPCTL(index);
	epctl = readl(hs->regs + epreg);

	if (value) {
		epctl |= DXEPCTL_STALL + DXEPCTL_SNAK;
		if (epctl & DXEPCTL_EPENA)
			epctl |= DXEPCTL_EPDIS;
	} else {
		epctl &= ~DXEPCTL_STALL;
		xfertype = epctl & DXEPCTL_EPTYPE_MASK;
		if (xfertype == DXEPCTL_EPTYPE_BULK ||
			xfertype == DXEPCTL_EPTYPE_INTERRUPT)
				epctl |= DXEPCTL_SETD0PID;
	}

	writel(epctl, hs->regs + epreg);

	epreg = DOEPCTL(index);
	epctl = readl(hs->regs + epreg);

	if (value)
		epctl |= DXEPCTL_STALL;
	else {
		epctl &= ~DXEPCTL_STALL;
		xfertype = epctl & DXEPCTL_EPTYPE_MASK;
		if (xfertype == DXEPCTL_EPTYPE_BULK ||
			xfertype == DXEPCTL_EPTYPE_INTERRUPT)
				epctl |= DXEPCTL_SETD0PID;
	}

	writel(epctl, hs->regs + epreg);

	hs_ep->halted = value;

	return 0;
}

static struct usb_ep_ops cx_hsotg_ep_ops = {
	.enable		= cx_hsotg_ep_enable,
	.disable	= cx_hsotg_ep_disable,
	.alloc_request	= cx_hsotg_ep_alloc_request,
	.free_request	= cx_hsotg_ep_free_request,
	.queue		= cx_hsotg_ep_queue,
	.dequeue	= cx_hsotg_ep_dequeue,
	.set_halt	= cx_hsotg_ep_sethalt,
};

/**
 * cx_hsotg_phy_enable - enable platform phy dev
 * @hsotg: The driver state
 *
 * A wrapper for platform code responsible for controlling
 * low-level USB code
 */
static void cx_hsotg_phy_enable(struct cx_hsotg *hsotg)
{
	usbd("%s\n", __func__);
	if (hsotg->plat && hsotg->plat->phy_init)
		hsotg->plat->phy_init(hsotg->plat->phy_type);
	else {
		printf("usb phy enable, nothing to do\n");
	}
}

/**
 * cx_hsotg_phy_disable - disable platform phy dev
 * @hsotg: The driver state
 *
 * A wrapper for platform code responsible for controlling
 * low-level USB code
 */
static void cx_hsotg_phy_disable(struct cx_hsotg *hsotg)
{
	usbd("%s\n", __func__);
	if (hsotg->plat && hsotg->plat->phy_exit)
		hsotg->plat->phy_exit(hsotg->plat->phy_type);
	else {
		printf("usb phy diable, nothing to do\n");
	}
}

/**
 * cx_hsotg_init - initalize the usb core
 * @hsotg: The driver state
 */
static void cx_hsotg_init(struct cx_hsotg *hsotg)
{
	struct cx_hsotg_plat *plat = hsotg->plat;
	uint8_t turnaround_time = 0;

	/* unmask subset of interrupts */
	writel(DIEPMSK_TIMEOUTMSK | DIEPMSK_AHBERRMSK | 
		DIEPMSK_EPDISBLDMSK | DIEPMSK_XFERCOMPLMSK,
		hsotg->regs + DIEPMSK
	);

	writel(DOEPMSK_SETUPMSK | DOEPMSK_AHBERRMSK |
		DOEPMSK_EPDISBLDMSK | DOEPMSK_XFERCOMPLMSK, 
		hsotg->regs + DOEPMSK
	);

	writel(0, hsotg->regs + DAINTMSK);

	/* Be in disconnected state until gadget is registered */
	__orr32(hsotg->regs + DCTL, DCTL_SFTDISCON);

	/* post global nak until we're ready TODO */ 
	writel(DCTL_SGNPINNAK | DCTL_SGOUTNAK,  
		hsotg->regs + DCTL);             

	/* setup fifos */
	usbd("GRXFSIZ=0x%08x, GNPTXFSIZ=0x%08x\n",
		readl(hsotg->regs + GRXFSIZ),
		readl(hsotg->regs + GNPTXFSIZ));

	cx_hsotg_init_fifo(hsotg);

	/* turnaround time is limit: FIXED */
	if (plat->phy_type == HS_UTMI_INTERFACE) {
		if (plat->phyif == USB_PHYIF_16)
			turnaround_time = 0x5; /* 16 bits */
		else 
			turnaround_time = 0x9; /* 8 bits */
	}

	/* set the PLL on, remove the HNP/SRP and set the PHY */
	writel(hsotg->force |	/* force mode or not */
	       0 << 15 | /* PHY Low-Power Clock Select */
	       turnaround_time << 10 | /* USB Turnaround Time */
	       0 << 9 | /* HNP-Capable disable */
	       0 << 8 | /* SRP-Capable disable */
	       0 << 7 | /* ULPI DDR Select: 8-bit-wide data bus */
	       0 << 6 | /* 0: high speed utmi+, 1: full speed serial */
	       plat->phy_type << 4 | /* 0: utmi+, 1:ulpi */
	       hsotg->phyif << 3 | /* phy i/f  0:8bit, 1:16bit */
//	       GUSBCFG_TOUTCAL(0x7) << 0, /* HS/FS Timeout Calibration */
	       GUSBCFG_TOUTCAL(0x2) << 0, /* HS/FS Timeout Calibration */
	       hsotg->regs + GUSBCFG /* usb config */
	);

	writel(1 << 7 | /* Non-Periodic TxFIFO Empty Level: half */
	       0 << 5 | /* no dma mode */
	       0 << 4 | /* Burst Length: 1 word */
	       0 << 0, /* Global Interrupt mask */
	       hsotg->regs + GAHBCFG /* usb AHB config */
	);
}

/**
 * cx_hsotg_gadget_getframe - read the frame number
 * @gadget: The usb gadget state
 *
 * Read the {micro} frame number
 */
static int cx_hsotg_gadget_getframe(struct usb_gadget *gadget)
{
	return cx_hsotg_read_frameno(to_hsotg(gadget));
}

/**
 * cx_hsotg_pullup - connect/disconnect the USB PHY
 * @gadget: The usb gadget state
 * @is_on: Current state of the USB PHY
 *
 * Connect/Disconnect the USB PHY pullup
 */
static int cx_hsotg_pullup(struct usb_gadget *gadget, int is_on)
{
	struct cx_hsotg *hsotg = to_hsotg(gadget);

	usbd("%s %s\n", __func__, is_on ? "up" : "down");
	if (is_on) {
		cx_hsotg_phy_enable(hsotg);
		cx_hsotg_core_init(hsotg);
	} else {
		cx_hsotg_phy_disable(hsotg);
	}

	hsotg->gadget.speed = USB_SPEED_UNKNOWN;

	return 0;
}

static const struct usb_gadget_ops cx_hsotg_gadget_ops = {
	.get_frame	= cx_hsotg_gadget_getframe,
	.pullup         = cx_hsotg_pullup,
};

/**
 * cx_hsotg_initep - initialise a single endpoint
 * @hsotg: The device state.
 * @hs_ep: The endpoint to be initialised.
 * @epnum: The endpoint number
 *
 * Initialise the given endpoint (as part of the probe and device state
 * creation) to give to the gadget driver. Setup the endpoint name, any
 * direction information and other state that may be required.
 */
static void cx_hsotg_initep(struct cx_hsotg *hsotg,
				       struct cx_hsotg_ep *hs_ep,
				       int epnum)
{
	u32 ptxfifo;
	char *dir;

	debug("%s : ep%d-0x%p\n", __func__, epnum, &hs_ep->ep);

	if (epnum == 0)
		dir = "";
	else if ((epnum % 2) == 0) {
		dir = "out";
	} else {
		dir = "in";
		hs_ep->dir_in = 1;
	}

	hs_ep->index = epnum;

	snprintf(hs_ep->name, sizeof(hs_ep->name), "ep%d%s", epnum, dir);

	INIT_LIST_HEAD(&hs_ep->queue);
	INIT_LIST_HEAD(&hs_ep->ep.ep_list);

	/* add to the list of endpoints known by the gadget driver */
	if (epnum)
		list_add_tail(&hs_ep->ep.ep_list, &hsotg->gadget.ep_list);

	hs_ep->parent = hsotg;
	hs_ep->ep.name = hs_ep->name;
	usb_ep_set_maxpacket_limit(&hs_ep->ep, epnum ? 512 : EP0_MPS_LIMIT);
	hs_ep->ep.ops = &cx_hsotg_ep_ops;

	/*
	 * Read the FIFO size for the Periodic TX FIFO, even if we're
	 * an OUT endpoint, we may as well do this if in future the
	 * code is changed to make each endpoint's direction changeable.
	 */
	ptxfifo = readl(hsotg->regs + DPTXFSIZN(epnum));
	hs_ep->fifo_size = FIFOSIZE_DEPTH_GET(ptxfifo) * 4;
}

/**
 * cx_hsotg_hw_cfg - read HW configuration registers
 * @param: The device state
 *
 * Read the USB core HW configuration registers
 */
static void cx_hsotg_hw_cfg(struct cx_hsotg *hsotg)
{
	u32 cfg1, cfg2, cfg4;
	int i;
	char *dir[] = {
		"bidir(IN and OUT)",
		"IN",
		"OUT",
		"reserved",
	};

	/* check hardware configuration */
	cfg2 = readl(hsotg->regs + 0x48);
	hsotg->num_of_eps = (cfg2 >> 10) & 0xF;
	debug("EPs:%d\n", hsotg->num_of_eps);

	/* select fifo mode */
	cfg4 = readl(hsotg->regs + 0x50);
	hsotg->dedicated_fifos = (cfg4 >> 25) & 1;
	debug("%s fifos\n", hsotg->dedicated_fifos ? "dedicated" : "shared");

	/* dump endpoint infomation */
	cfg1 = readl(hsotg->regs + 0x44);
	debug("EPs direction:\n"); 
	for (i = 0; i < hsotg->num_of_eps + 1; i++) {
		printf("\tep %d %s\n", i, dir[(cfg1 >> (i * 2))&0x3]);
	}
}

int cx_high_speed_otg_init(struct cx_hsotg_plat *plat)
{
	struct cx_hsotg_ep *eps;
	int epnum;

	debug("high speed otg initialization, Cloudersemi Inc\n");

	hsotg = malloc(sizeof(*hsotg));
	if (!hsotg) {
		printf("no memory alloced\n");
		return -ENOMEM;
	}

	memset(hsotg, 0x0, sizeof(*hsotg));
	hsotg->plat = plat;
	debug("plat private data list: %s-%s-%s-%s\n",
		({ char *phy_type;
			switch(plat->phy_type) {
			case HS_UTMI_INTERFACE:
				phy_type = "utmi";
				break;
			case HS_ULPI_INTERFACE:
				phy_type = "ulpi";
				break;
			default:
				phy_type = "utmi(default)";
				break;
			};
			phy_type;
		}), ({ char *phyif;
			switch(plat->phyif) {
			case USB_PHYIF_8:
				phyif = "8 bit interface";
				break;
			case USB_PHYIF_16:
				phyif = "16 bit interface";
				break;
			default:
				phyif = "16 bit interface(default)";
				break;
			};
			phyif;
		}), ({char *dma_desc;
			switch(plat->dma) {
			case CX_HSOTG_DMA_NONE:
				dma_desc = "no dma";
				break;
			case CX_HSOTG_DMA_ONLY:
				dma_desc = "always use DMA";
				break;
			default:
				dma_desc = "DMA is chosen by driver";
				break;
			};
			dma_desc;
		}), ({ char *otg;
			switch(plat->force) {
			case USB_FORCE_HOST:
				otg = "Force Device Mode";
				break;
			case USB_FORCE_DEVICE:
				otg = "Force Device Mode";
				break;
			case USB_FORCE_NONE:
			default:
				otg = "Normal Mode";
				break;
			};
			otg;
		})

	);

	/* IP memory mmaping */
	hsotg->regs = plat->mmap;
	debug("mmap regs 0x%p\n", hsotg->regs);

	hsotg->gadget.speed = USB_SPEED_HIGH; /* Default speed */
	hsotg->gadget.ops = &cx_hsotg_gadget_ops;
	hsotg->gadget.name = "cx-hsotg";
	debug("on the go '%s' -- high speed\n", hsotg->gadget.name);

	if (plat->phyif == USB_PHYIF_8)
		hsotg->phyif = GUSBCFG_PHYIF8;
	else 
		hsotg->phyif = GUSBCFG_PHYIF16;

	/* hs otg device reset */
	hsotg->force = plat->force;
	cx_hsotg_corereset(hsotg);
	cx_hsotg_hw_cfg(hsotg);
	cx_hsotg_init(hsotg);

	/* otg options */
	hsotg->gadget.is_dualspeed = 1; 
	hsotg->gadget.is_otg = 0;
	hsotg->gadget.is_a_peripheral = 0;
	hsotg->gadget.b_hnp_enable = 0;
	hsotg->gadget.a_hnp_support = 0;
	hsotg->gadget.a_alt_hnp_support = 0;

	/* hsotg->num_of_eps holds number of EPs other than ep0 */
	if (!hsotg->num_of_eps) {
		printf("number of EPs (zero)\n");
		free(hsotg);
		return -EINVAL;
	}

	eps = malloc((hsotg->num_of_eps + 1) * sizeof(struct cx_hsotg_ep));
	if (!eps) {
		printf("no memory alloced for EPs(%d)\n", hsotg->num_of_eps + 1);
		free(hsotg);
		return -ENOMEM;
	}
	hsotg->eps = eps;

	/* setup endpoint information */
	INIT_LIST_HEAD(&hsotg->gadget.ep_list);
	hsotg->gadget.ep0 = &hsotg->eps[0].ep;

	/* allocate EP0 request */
	hsotg->ctrl_req = cx_hsotg_ep_alloc_request(&hsotg->eps[0].ep, 0);
	if (!hsotg->ctrl_req) {
		printf("no memory alloced for ctrl request.\n");
		free(eps);
		free(hsotg);
		return -ENOMEM;
	}

	/* initialise the endpoints now the core has been initialised */
	for (epnum = 0; epnum < hsotg->num_of_eps; epnum++)
		cx_hsotg_initep(hsotg, &hsotg->eps[epnum], epnum);

	/*
	 * fifo init usbd, when insmod module, usb controller
	 * trig irq always.
	 */
	debug("DegsinWare high speed otg initialization successfully\n");

	return 0;
}

/*
  Register entry point for the peripheral controller driver.
*/
int usb_gadget_register_driver(struct usb_gadget_driver *driver)
{
	struct cx_hsotg *udc = hsotg;
	int retval = 0;

	debug("%s device %s\n", usb_speed_string(driver->speed),  __func__);

	if (!driver || (driver->speed != USB_SPEED_FULL && 
	    driver->speed != USB_SPEED_HIGH) || !driver->bind || 
	    !driver->disconnect || !driver->setup) {
		printf("hs otg gadget driver sanity check lost\n");
		return -EINVAL;
	}

	if (!udc) {
		printf("no usb device found\n");
		return -ENODEV;
	}

	if (udc->driver) {
		printf("usb device driver attached\n");
		return -EBUSY;
	}

	/* first hook up the driver ... */
	udc->driver = driver;
	retval = driver->bind(&udc->gadget);
	if (retval) {
		usbd("%s: bind to driver --> error %d\n",
			udc->gadget.name, retval);
		udc->driver = 0;
		return retval;
	}

	debug("registered gadget driver %s\n", udc->gadget.name);

	return 0;
}

/*
 * Unregister entry point for the peripheral controller driver.
 */
int usb_gadget_unregister_driver(struct usb_gadget_driver *driver)
{
	struct cx_hsotg *udc = hsotg;
	int ep;

	debug("usb gadget driver unregistered\n");

	if (!udc) {
		printf("no usb device found\n");
		return -ENODEV;
	}

	if (!driver || driver != udc->driver)
		return -EINVAL;

	debug("sanity check OK, starting...\n");

	udc->driver = 0;
	if (udc->gadget.speed == USB_SPEED_UNKNOWN)
		driver = 0;
	udc->gadget.speed = USB_SPEED_UNKNOWN;

	/* all endpoints should be shutdown */
	for (ep = 1; ep < hsotg->num_of_eps; ep++)
		cx_hsotg_ep_disable(&udc->eps[ep].ep);

	if (driver)
		driver->disconnect(&udc->gadget);

	driver->unbind(&udc->gadget);

	return 0;
}
