 --	SAMPLE_VENC_1080P_CLASSIC()
  |
  | -- VB_CONF_S 			stVbconf
  | -- SAMPLE_VI_CONFIG_S 	stViConfig
  |
  |	   /* step1: init sys variable */
  | -- SAMPLE_COMM_VI_GetSizeBySensor()
  |
  | -- SAMPLE_COMM_SYS_CalcPicVbBlkSize() // cal VB block size
	 |
	 | -- SAMPLE_COMM_SYS_GetPicSize()
  |
  |    /* step2: mpp sys init */
  | -- SAMPLE_COMM_SYS_Init(&stVbConf)		// video buff
	 |
	 | -- HI_MPI_SYS_Exit()
	 | -- HI_MPI_VB_Exit()
	 |
	 | -- HI_MPI_VB_SetConf()
	 | -- HI_MPI_VB_Init()
	 |
	 | -- HI_MPI_SYS_SetConf()
	 | -- HI_MPI_SYS_Init()
  |
  |	   /* step3: start VI dev && chn to capture */
  | -- SAMPLE_COMM_VI_StartVi(&stViConfig)
	 |
	 | -- IsSensorInput()
		|
		| -- SAMPLE_COMM_VI_StartIspAndVi()
		   |
		   | -- SAMPLE_COMM_VI_StartMIPI()	// step3_1
			  |
		      | -- SAMPLE_COMM_VI_SetMipiAttr
				 |
				 | -- fd = open("/dev/hi_mipi", O_RDWR)
				 | -- ioctl(fd, HI_MIPI_SET_DEV_ATTR, pstcomboDevAttr)
		   |
		   | -- SAMPLE_COMM_ISP_Init()	// step3_2
		      |
			  | -- sensor_register_callback()
			  |
			  | -- HI_MPI_AE_Register()
			  | -- HI_MPI_AWB_Register()
			  | -- HI_MPI_AF_Register()
			  |
			  | -- HI_MPI_ISP_MemInit()
			  |
			  | -- HI_MPI_ISP_SetWDRMode()
			  |
			  | -- HI_MPI_ISP_SetPubAttr()
			  |
			  | -- HI_MPI_ISP_Init()
		   |
		   | -- SAMPLE_COMM_ISP_Run() // step3_3
			  |
			  | -- pthread_create()
		   |
		   | -- SAMPLE_COMM_VI_StartDev() // step3_4
		      |
			  | -- HI_MPI_VI_SetDevAttr()
			  |
			  | -- HI_MPI_ISP_GetWDRMode()
			  |
			  | -- HI_MPI_VI_SetWDRAttr()
			  |
			  | -- HI_MPI_VI_EnableDev()
		   |
		   | -- SAMPLE_COMM_VI_StartChn() // step3_5
			  |
			  | -- HI_MPI_VI_SetChnAttr()
			  |
			  | -- HI_MPI_VI_SetRotate()
			  |
			  | -- HI_MPI_VI_EnableChn()
  |
  |	   /* step4: start VPSS; VI bind VPSS */
  | -- SAMPLE_COMM_SYS_GetPicSize
	 |
	 | -- SAMPLE_COMM_VPSS_StartGroup()
	    |
		| -- HI_MPI_VPSS_CreateGrp()
		|
		| -- HI_MPI_VPSS_GetNRParam()
		| -- HI_MPI_VPSS_SetNRParam()
		|
		| -- HI_MPI_VPSS_StartGrp()
	 |
	 | -- SAMPLE_COMM_VI_BindVpss()
	    |
		| -- SAMPLE_COMM_VI_Mode2Param()
		|
		| -- HI_MPI_SYS_Bind()
	 |
	 | -- SAMPLE_COMM_VPSS_EnableChn()
	    |
		| -- HI_MPI_VPSS_SetChnAttr()
		|
		| -- HI_MPI_VPSS_SetChnMode()
		|
		| -- HI_MPI_VPSS_EnableChn()
  |
  |	   /* step 5: start stream venc */
  | -- SAMPLE_COMM_VENC_Start()
     |
	 | -- VENC_CHN_ATTR_S stVencChnAttr;
	 |
	 | -- HI_MPI_VENC_CreateChn() // step 5_1
	 |
	 | -- HI_MPI_VENC_StartRecvPic() // step 5_2
  |
  | -- SAMPLE_COMM_VENC_BindVpss()
     |
	 | -- HI_MPI_SYS_Bind()
  |
  |	   /* step 6: stream venc process -- get stream, then save it to file */
  | -- SAMPLE_COMM_VENC_StartGetStream()
	 |
	 | -- pthread_create()
	    |
		| -- SAMPLE_COMM_VENC_GetVencStreamProc_Svc_t()
  |
  | -- SAMPLE_COMM_VENC_StopGetStream()
  | 
 
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	