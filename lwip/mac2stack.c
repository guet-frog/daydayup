
 -- PLC_LinkReceiveToStack(uint8_t *data, uint32_t size)
  |
  | -- ETHPLC_SendQue(data, size)
	 |
	 | -- ETHPLC_RxQue_T buf;	// typedef struct { uint8_t *data; uint32_t size; }ETHPLC_RxQue_T;
	 |
	 | -- buf.data = data;
	 | -- buf.size = size;
	 |
	 | -- if (OK == xQueueSendFromISR(ethplc_netif_rxque_handle, &buf, &xHigherPriorityTaskWoken))
		|
		| -- portYIELD_FROM_ISR(xHigherPriorityTaskWoken)
		| -- return											// eth or plc not free static buff
		|
	   -- else
		|
		| -- ETHPLC_FreeRxBuf(data)
		|
		| -- LINK_STATS_INC(link.drop)
		| -- LINK_STATS_INC(link.err)
		| -- return

 -- ETHPLC_NetIfRxTask(void *param)
  |
  | -- xStatus = xQueueReceive(ethplc_netif_rxque_handle, &buf, portMAX_DELAY)
  |
  | -- p = pbuf_alloc(PBUF_RAW, buf.size, PBUF_RAM)		// struct pbuf *p, *q  -- p has already init
	 |
	 | -- switch (layer)
		|
		| -- case PBUF_TRANSPORT：// unrecognized token	表征, 记号
		| -- case PBUF_IP:
		| -- case PBUF_LINK:
		| -- case PBUF_RAW:
		| -- default:
	 |
	 | -- switch (type)
		|
		| -- case PBUF_RAM:
		   |
		   | -- p = (struct pbuf *)mem_malloc((SIZEOF_STRUCT_PBUF + offset) + length)
		   | -- p->payload = LWIP_MEM_ALIGN((uint8_t *)p + SIZEOF_STRUCT_PBUF + offset)
		   | -- p->len = p->tot_len = length	/* payload point to the actual data in the buffer */
		   | -- p->next = NULL
		   | -- p->type = type
		   | -- break				// warning: switch format
  |
  | -- memcpy((uint8_t *)p->payload, buf.data, p->len)
  |
  | -- ETHPLC_FreeRxBuf(buf.data)		// copy then free
  |
  | -- ethhdr = p->payload				// payload is frame first addr
  |
  | -- switch (htons(ethhdr->type))
	 |
	 | -- case ETHTYPE_IP:		// no break -- test no break program flow
	 | -- case ETHTYPE_ARP:
		|
		| -- if (ethplc_netif.input(p, &ethplc_netif) != ERR_OK)	// netif.input = ethplcnetif_init
		   |
	       | -- pbuf_free(p)
		   | -- p = NULL
		|
		| -- break
	 |
	 | -- default: break
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		