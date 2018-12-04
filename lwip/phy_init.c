
 -- KSZ8041_Init()
  |
  | -- KSZ8041_CTL_GPIO_Init();		// config reset gpio
  |
  | -- PHY_RESET_LOW
  | -- delay(10);		// more than 10ms
  | -- PHY_RESET_HIGH
  |
  | -- ETH_ReadPhy(id);	// read phy id
  |
  | -- ETH_WritePhy(control);	// enable auto-negotiation
  |
  | -- ETH_ReadPhy(status);	// check auto-negotiation
  |
  | -- if (ETH_ReadPhy(status))	// check link status
	 |
	 | -- ETH_ReadPhy(control)
		|
		| -- ETH_SetLinkDuplexMode(ETH_FULL_DUPLEX)
		| -- ETH_SetLinkSpeed(ETH_SPEED_100M)