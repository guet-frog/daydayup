/*
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Boot support
 */
#include <common.h>
#include <bootm.h>
#include <command.h>
#include <environment.h>
#include <image.h>
#include <lmb.h>
#include <malloc.h>
#include <nand.h>
#include <asm/byteorder.h>
#include <linux/compiler.h>
#include <linux/ctype.h>
#include <linux/err.h>
#include <u-boot/zlib.h>
#include <mmc.h>
/*Android mkbooting format*/
#include <bootimg.h>
#include <version.h>
#include <android_misc.h>
#include <ext4fs.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_CMD_IMI)
static int image_info(unsigned long addr);
#endif

#if defined(CONFIG_CMD_IMLS)
#include <flash.h>
#include <mtd/cfi_flash.h>
extern flash_info_t flash_info[]; /* info for FLASH chips */
#endif

#if defined(CONFIG_CMD_IMLS) || defined(CONFIG_CMD_IMLS_NAND)
static int do_imls(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#endif

bootm_headers_t images;		/* pointers to os/initrd/fdt images */

/* we overload the cmd field with our state machine info instead of a
 * function pointer */
static cmd_tbl_t cmd_bootm_sub[] = {
	U_BOOT_CMD_MKENT(start, 0, 1, (void *)BOOTM_STATE_START, "", ""),
	U_BOOT_CMD_MKENT(loados, 0, 1, (void *)BOOTM_STATE_LOADOS, "", ""),
#ifdef CONFIG_SYS_BOOT_RAMDISK_HIGH
	U_BOOT_CMD_MKENT(ramdisk, 0, 1, (void *)BOOTM_STATE_RAMDISK, "", ""),
#endif
#ifdef CONFIG_OF_LIBFDT
	U_BOOT_CMD_MKENT(fdt, 0, 1, (void *)BOOTM_STATE_FDT, "", ""),
#endif
	U_BOOT_CMD_MKENT(cmdline, 0, 1, (void *)BOOTM_STATE_OS_CMDLINE, "", ""),
	U_BOOT_CMD_MKENT(bdt, 0, 1, (void *)BOOTM_STATE_OS_BD_T, "", ""),
	U_BOOT_CMD_MKENT(prep, 0, 1, (void *)BOOTM_STATE_OS_PREP, "", ""),
	U_BOOT_CMD_MKENT(fake, 0, 1, (void *)BOOTM_STATE_OS_FAKE_GO, "", ""),
	U_BOOT_CMD_MKENT(go, 0, 1, (void *)BOOTM_STATE_OS_GO, "", ""),
};

static int do_bootm_subcommand(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[])
{
	int ret = 0;
	long state;
	cmd_tbl_t *c;

	c = find_cmd_tbl(argv[0], &cmd_bootm_sub[0], ARRAY_SIZE(cmd_bootm_sub));
	argc--; argv++;

	if (c) {
		state = (long)c->cmd;
		if (state == BOOTM_STATE_START)
			state |= BOOTM_STATE_FINDOS | BOOTM_STATE_FINDOTHER;
	} else {
		/* Unrecognized command */
		return CMD_RET_USAGE;
	}

	if (state != BOOTM_STATE_START && images.state >= state) {
		printf("Trying to execute a command out of order\n");
		return CMD_RET_USAGE;
	}

	ret = do_bootm_states(cmdtp, flag, argc, argv, state, &images, 0);

	return ret;
}

/*******************************************************************/
/* bootm - boot application image from image in memory */
/*******************************************************************/

int do_bootm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#ifdef CONFIG_NEEDS_MANUAL_RELOC
	static int relocated = 0;

	if (!relocated) {
		int i;

		/* relocate names of sub-command table */
		for (i = 0; i < ARRAY_SIZE(cmd_bootm_sub); i++)
			cmd_bootm_sub[i].name += gd->reloc_off;

		relocated = 1;
	}
#endif

	/* determine if we have a sub command */
	argc--; argv++;
	if (argc > 0) {
		char *endp;

		simple_strtoul(argv[0], &endp, 16);
		/* endp pointing to NULL means that argv[0] was just a
		 * valid number, pass it along to the normal bootm processing
		 *
		 * If endp is ':' or '#' assume a FIT identifier so pass
		 * along for normal processing.
		 *
		 * Right now we assume the first arg should never be '-'
		 */
		if ((*endp != 0) && (*endp != ':') && (*endp != '#'))
			return do_bootm_subcommand(cmdtp, flag, argc, argv);
	}

	return do_bootm_states(cmdtp, flag, argc, argv, BOOTM_STATE_START |
			BOOTM_STATE_FINDOS | BOOTM_STATE_FINDOTHER |
			BOOTM_STATE_LOADOS |
#if defined(CONFIG_PPC) || defined(CONFIG_MIPS)
			BOOTM_STATE_OS_CMDLINE |
#endif
			BOOTM_STATE_OS_PREP | BOOTM_STATE_OS_FAKE_GO |
			BOOTM_STATE_OS_GO, &images, 1);
}

int bootm_maybe_autostart(cmd_tbl_t *cmdtp, const char *cmd)
{
	const char *ep = getenv("autostart");

	if (ep && !strcmp(ep, "yes")) {
		char *local_args[2];
		local_args[0] = (char *)cmd;
		local_args[1] = NULL;
		printf("Automatic boot of image at addr 0x%08lX ...\n", load_addr);
		return do_bootm(cmdtp, 0, 1, local_args);
	}

	return 0;
}

#ifdef CONFIG_SYS_LONGHELP
static char bootm_help_text[] =
"[addr [arg ...]]\n    - boot application image stored in memory\n"
"\tpassing arguments 'arg ...'; when booting a Linux kernel,\n"
"\t'arg' can be the address of an initrd image\n"
#if defined(CONFIG_OF_LIBFDT)
"\tWhen booting a Linux kernel which requires a flat device-tree\n"
"\ta third argument is required which is the address of the\n"
"\tdevice-tree blob. To boot that kernel without an initrd image,\n"
"\tuse a '-' for the second argument. If you do not pass a third\n"
"\ta bd_info struct will be passed instead\n"
#endif
#if defined(CONFIG_FIT)
"\t\nFor the new multi component uImage format (FIT) addresses\n"
"\tmust be extened to include component or configuration unit name:\n"
"\taddr:<subimg_uname> - direct component image specification\n"
"\taddr#<conf_uname>   - configuration specification\n"
"\tUse iminfo command to get the list of existing component\n"
"\timages and configurations.\n"
#endif
"\nSub-commands to do part of the bootm sequence.  The sub-commands "
"must be\n"
"issued in the order below (it's ok to not issue all sub-commands):\n"
"\tstart [addr [arg ...]]\n"
"\tloados  - load OS image\n"
#if defined(CONFIG_SYS_BOOT_RAMDISK_HIGH)
"\tramdisk - relocate initrd, set env initrd_start/initrd_end\n"
#endif
#if defined(CONFIG_OF_LIBFDT)
"\tfdt     - relocate flat device tree\n"
#endif
"\tcmdline - OS specific command line processing/setup\n"
"\tbdt     - OS specific bd_t processing\n"
"\tprep    - OS specific prep before relocation or go\n"
"\tgo      - start OS";
#endif

U_BOOT_CMD(
		bootm,	CONFIG_SYS_MAXARGS,	1,	do_bootm,
		"boot application image from memory", bootm_help_text
	  );

/*******************************************************************/
/* bootd - boot default image */
/*******************************************************************/
#if defined(CONFIG_CMD_BOOTD)
int do_bootd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return run_command(getenv("bootcmd"), flag);
}

U_BOOT_CMD(
		boot,	1,	1,	do_bootd,
		"boot default, i.e., run 'bootcmd'",
		""
	  );

/* keep old command name "bootd" for backward compatibility */
U_BOOT_CMD(
		bootd, 1,	1,	do_bootd,
		"boot default, i.e., run 'bootcmd'",
		""
	  );

#endif


/*******************************************************************/
/* iminfo - print header info for a requested image */
/*******************************************************************/
#if defined(CONFIG_CMD_IMI)
static int do_iminfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int	arg;
	ulong	addr;
	int	rcode = 0;

	if (argc < 2) {
		return image_info(load_addr);
	}

	for (arg = 1; arg < argc; ++arg) {
		addr = simple_strtoul(argv[arg], NULL, 16);
		if (image_info(addr) != 0)
			rcode = 1;
	}
	return rcode;
}

static int image_info(ulong addr)
{
	void *hdr = (void *)addr;

	printf("\n## Checking Image at %08lx ...\n", addr);

	switch (genimg_get_format(hdr)) {
#if defined(CONFIG_IMAGE_FORMAT_LEGACY)
		case IMAGE_FORMAT_LEGACY:
			puts("   Legacy image found\n");
			if (!image_check_magic(hdr)) {
				puts("   Bad Magic Number\n");
				return 1;
			}

			if (!image_check_hcrc(hdr)) {
				puts("   Bad Header Checksum\n");
				return 1;
			}

			image_print_contents(hdr);

			puts("   Verifying Checksum ... ");
			if (!image_check_dcrc(hdr)) {
				puts("   Bad Data CRC\n");
				return 1;
			}
			puts("OK\n");
			return 0;
#endif
#if defined(CONFIG_FIT)
		case IMAGE_FORMAT_FIT:
			puts("   FIT image found\n");

			if (!fit_check_format(hdr)) {
				puts("Bad FIT image format!\n");
				return 1;
			}

			fit_print_contents(hdr);

			if (!fit_all_image_verify(hdr)) {
				puts("Bad hash in FIT image!\n");
				return 1;
			}

			return 0;
#endif
		default:
			puts("Unknown image format!\n");
			break;
	}

	return 1;
}

U_BOOT_CMD(
		iminfo,	CONFIG_SYS_MAXARGS,	1,	do_iminfo,
		"print header information for application image",
		"addr [addr ...]\n"
		"    - print header information for application image starting at\n"
		"      address 'addr' in memory; this includes verification of the\n"
		"      image contents (magic number, header and payload checksums)"
	  );
#endif


/*******************************************************************/
/* imls - list all images found in flash */
/*******************************************************************/
#if defined(CONFIG_CMD_IMLS)
static int do_imls_nor(void)
{
	flash_info_t *info;
	int i, j;
	void *hdr;

	for (i = 0, info = &flash_info[0];
			i < CONFIG_SYS_MAX_FLASH_BANKS; ++i, ++info) {

		if (info->flash_id == FLASH_UNKNOWN)
			goto next_bank;
		for (j = 0; j < info->sector_count; ++j) {

			hdr = (void *)info->start[j];
			if (!hdr)
				goto next_sector;

			switch (genimg_get_format(hdr)) {
#if defined(CONFIG_IMAGE_FORMAT_LEGACY)
				case IMAGE_FORMAT_LEGACY:
					if (!image_check_hcrc(hdr))
						goto next_sector;

					printf("Legacy Image at %08lX:\n", (ulong)hdr);
					image_print_contents(hdr);

					puts("   Verifying Checksum ... ");
					if (!image_check_dcrc(hdr)) {
						puts("Bad Data CRC\n");
					} else {
						puts("OK\n");
					}
					break;
#endif
#if defined(CONFIG_FIT)
				case IMAGE_FORMAT_FIT:
					if (!fit_check_format(hdr))
						goto next_sector;

					printf("FIT Image at %08lX:\n", (ulong)hdr);
					fit_print_contents(hdr);
					break;
#endif
				default:
					goto next_sector;
			}

next_sector:		;
		}
next_bank:	;
	}
	return 0;
}
#endif

#if defined(CONFIG_CMD_IMLS_NAND)
static int nand_imls_legacyimage(nand_info_t *nand, int nand_dev, loff_t off,
		size_t len)
{
	void *imgdata;
	int ret;

	imgdata = malloc(len);
	if (!imgdata) {
		printf("May be a Legacy Image at NAND device %d offset %08llX:\n",
				nand_dev, off);
		printf("   Low memory(cannot allocate memory for image)\n");
		return -ENOMEM;
	}

	ret = nand_read_skip_bad(nand, off, &len,
			imgdata);
	if (ret < 0 && ret != -EUCLEAN) {
		free(imgdata);
		return ret;
	}

	if (!image_check_hcrc(imgdata)) {
		free(imgdata);
		return 0;
	}

	printf("Legacy Image at NAND device %d offset %08llX:\n",
			nand_dev, off);
	image_print_contents(imgdata);

	puts("   Verifying Checksum ... ");
	if (!image_check_dcrc(imgdata))
		puts("Bad Data CRC\n");
	else
		puts("OK\n");

	free(imgdata);

	return 0;
}

static int nand_imls_fitimage(nand_info_t *nand, int nand_dev, loff_t off,
		size_t len)
{
	void *imgdata;
	int ret;

	imgdata = malloc(len);
	if (!imgdata) {
		printf("May be a FIT Image at NAND device %d offset %08llX:\n",
				nand_dev, off);
		printf("   Low memory(cannot allocate memory for image)\n");
		return -ENOMEM;
	}

	ret = nand_read_skip_bad(nand, off, &len,
			imgdata);
	if (ret < 0 && ret != -EUCLEAN) {
		free(imgdata);
		return ret;
	}

	if (!fit_check_format(imgdata)) {
		free(imgdata);
		return 0;
	}

	printf("FIT Image at NAND device %d offset %08llX:\n", nand_dev, off);

	fit_print_contents(imgdata);
	free(imgdata);

	return 0;
}

static int do_imls_nand(void)
{
	nand_info_t *nand;
	int nand_dev = nand_curr_device;
	size_t len;
	loff_t off;
	u32 buffer[16];

	if (nand_dev < 0 || nand_dev >= CONFIG_SYS_MAX_NAND_DEVICE) {
		puts("\nNo NAND devices available\n");
		return -ENODEV;
	}

	printf("\n");

	for (nand_dev = 0; nand_dev < CONFIG_SYS_MAX_NAND_DEVICE; nand_dev++) {
		nand = &nand_info[nand_dev];
		if (!nand->name || !nand->size)
			continue;

		for (off = 0; off < nand->size; off += nand->erasesize) {
			const image_header_t *header;
			int ret;

			if (nand_block_isbad(nand, off))
				continue;

			len = sizeof(buffer);

			ret = nand_read(nand, off, &len, (u8 *)buffer);
			if (ret < 0 && ret != -EUCLEAN) {
				printf("NAND read error %d at offset %08llX\n",
						ret, off);
				continue;
			}

			switch (genimg_get_format(buffer)) {
#if defined(CONFIG_IMAGE_FORMAT_LEGACY)
				case IMAGE_FORMAT_LEGACY:
					header = (const image_header_t *)buffer;

					len = image_get_image_size(header);
					nand_imls_legacyimage(nand, nand_dev, off, len);
					break;
#endif
#if defined(CONFIG_FIT)
				case IMAGE_FORMAT_FIT:
					len = fit_get_size(buffer);
					nand_imls_fitimage(nand, nand_dev, off, len);
					break;
#endif
			}
		}
	}

	return 0;
}
#endif

#if defined(CONFIG_CMD_IMLS) || defined(CONFIG_CMD_IMLS_NAND)
static int do_imls(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret_nor = 0, ret_nand = 0;

#if defined(CONFIG_CMD_IMLS)
	ret_nor = do_imls_nor();
#endif

#if defined(CONFIG_CMD_IMLS_NAND)
	ret_nand = do_imls_nand();
#endif

	if (ret_nor)
		return ret_nor;

	if (ret_nand)
		return ret_nand;

	return (0);
}

U_BOOT_CMD(
		imls,	1,		1,	do_imls,
		"list all images found in flash",
		"\n"
		"    - Prints information about all images found at sector/block\n"
		"      boundaries in nor/nand flash."
	  );
#endif

#ifdef CONFIG_CMD_BOOTZ

int __weak bootz_setup(ulong image, ulong *start, ulong *end)
{
	/* Please define bootz_setup() for your platform */

	puts("Your platform's zImage format isn't supported yet!\n");
	return -1;
}

/*
 * zImage booting support
 */
static int bootz_start(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[], bootm_headers_t *images)
{
	int ret;
	ulong zi_start, zi_end;

	ret = do_bootm_states(cmdtp, flag, argc, argv, BOOTM_STATE_START,
			images, 1);

	/* Setup Linux kernel zImage entry point */
	if (!argc) {
		images->ep = load_addr;
		debug("*  kernel: default image load address = 0x%08lx\n",
				load_addr);
	} else {
		images->ep = simple_strtoul(argv[0], NULL, 16);
		debug("*  kernel: cmdline image address = 0x%08lx\n",
				images->ep);
	}

	ret = bootz_setup(images->ep, &zi_start, &zi_end);
	if (ret != 0)
		return 1;

	lmb_reserve(&images->lmb, images->ep, zi_end - zi_start);

	/*
	 * Handle the BOOTM_STATE_FINDOTHER state ourselves as we do not
	 * have a header that provide this informaiton.
	 */
	if (bootm_find_ramdisk_fdt(flag, argc, argv))
		return 1;

	return 0;
}

int do_bootz(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;

	/* Consume 'bootz' */
	argc--; argv++;

	if (bootz_start(cmdtp, flag, argc, argv, &images))
		return 1;

	/*
	 * We are doing the BOOTM_STATE_LOADOS state ourselves, so must
	 * disable interrupts ourselves
	 */
	bootm_disable_interrupts();

	images.os.os = IH_OS_LINUX;
	ret = do_bootm_states(cmdtp, flag, argc, argv,
			BOOTM_STATE_OS_PREP | BOOTM_STATE_OS_FAKE_GO |
			BOOTM_STATE_OS_GO,
			&images, 1);

	return ret;
}

#ifdef CONFIG_SYS_LONGHELP
static char bootz_help_text[] =
"[addr [initrd[:size]] [fdt]]\n"
"    - boot Linux zImage stored in memory\n"
"\tThe argument 'initrd' is optional and specifies the address\n"
"\tof the initrd in memory. The optional argument ':size' allows\n"
"\tspecifying the size of RAW initrd.\n"
#if defined(CONFIG_OF_LIBFDT)
"\tWhen booting a Linux kernel which requires a flat device-tree\n"
"\ta third argument is required which is the address of the\n"
"\tdevice-tree blob. To boot that kernel without an initrd image,\n"
"\tuse a '-' for the second argument. If you do not pass a third\n"
"\ta bd_info struct will be passed instead\n"
#endif
"";
#endif

U_BOOT_CMD(
		bootz,	CONFIG_SYS_MAXARGS,	1,	do_bootz,
		"boot Linux zImage image from memory", bootz_help_text
	  );
#endif	/* CONFIG_CMD_BOOTZ */


#ifdef CONFIG_CMD_BOOTA
/* Section for Android bootimage format support
 * Refer:
 * http://android.git.kernel.org/?p=platform/system/core.git;a=blob;f=mkbootimg/bootimg.h
 */
struct mmc *mmc;
static block_dev_desc_t *dev_desc = NULL;
static unsigned char bhdr[512];
#define ALIGN_SECTOR(n, pagesz) ((n + (pagesz - 1)) & (~(pagesz - 1)))
void bootimg_print_image_hdr (boot_img_hdr *hdr)
{
#ifdef DEBUG
	int i;
	printf("   Image magic:   %s\n", hdr->magic);

	printf("   kernel_size:   0x%x\n", hdr->kernel_size);
	printf("   kernel_addr:   0x%x\n", hdr->kernel_addr);

	printf("   rdisk_size :   0x%x\n", hdr->ramdisk_size);
	printf("   rdisk_addr :   0x%x\n", hdr->ramdisk_addr);

	printf("   second_size:   0x%x\n", hdr->second_size);
	printf("   second_addr:   0x%x\n", hdr->second_addr);

	printf("   tags_addr:     0x%x\n", hdr->tags_addr);
	printf("   page_size:     0x%x\n", hdr->page_size);

	printf("   name:      %s\n", hdr->name);
	printf("   cmdline:   %s\n", hdr->cmdline);

	for (i = 0; i < 8; i++)
		printf("   id[%d]:   0x%x\n", i, hdr->id[i]);
#endif
}

void gpt_print_disk_partition(disk_partition_t *info)
{
#ifdef DEBUG
	printf("   partition_name :     %s\n", info->name);
	printf("   partition_type :     %s\n", info->type);

	printf("   partition_start:     0x%lx\n", info->start);
	printf("   partition_size :     0x%lx\n", info->size);
	printf("   partition_blksz:     0x%lx\n", info->blksz);

	printf("   partition_uuid :     %s\n", info->uuid);
#endif
}

/*find a mmc device and init */
int  mmc_ready(unsigned int mmc_select)
{
	int err;

	mmc = find_mmc_device(mmc_select);
	if (!mmc) {
		printf("boota: cannot find '%d' mmc device\n", mmc_select);
		return -1;
	}

	dev_desc = get_dev("mmc", mmc_select);
	if (NULL == dev_desc) {
		printf("** Block device MMC %d not supported\n", mmc_select);
		printf("invalid mmc device\n");
		return -1;

	}

	err = mmc_init(mmc);
	if (err) {
		printf("boot mmc init failed: err - %d\n", err);
		return -1;
	}

	return 0;
}

/*change unsigned int to string*/
void uint2str(unsigned int pword, unsigned char* str)
{
	char alpha[] = "0123456789ABCDEF";
	unsigned int i;
	for (i = 0; i < 8; i++, pword <<= 4) {
		str[i] = (alpha[pword >> 28]);
	}
	str[i] = 0;
}

/* convert a boot_image at kernel_addr into a kernel + ramdisk + tags */
int setup_boot_tag(boot_img_hdr *os_hdr){

	static u32      *param_addr = 0;
	char initrd_param[64];
	unsigned int pos;
	static u8       cmdline[256] = CONFIG_BOOTARGS;


	param_addr = (u32 *)CONFIG_PARAM_BASE;
	/* might be address of ascii-z string: "memsize" */
	param_addr[0] = 0;
	/* might be address of ascii-z string: "0x01000000" */
	param_addr[1] = 0;
	param_addr[2] = 0;
	param_addr[3] = 0;
	param_addr[4] = 0;
	param_addr[5] = CONFIG_PARAM_BASE + 32;
	param_addr[6] = (u32)((void *)os_hdr->ramdisk_addr);

	memset(initrd_param, 0, 40);
	strcpy((char *)initrd_param, " rd_start=0x");
	pos = strlen(initrd_param);
	uint2str(os_hdr->ramdisk_addr, (unsigned char *)(initrd_param + pos));
	pos = strlen(initrd_param);
	strcpy((char *)(initrd_param + pos), " rd_size=0x");
	pos = strlen(initrd_param);
	uint2str(os_hdr->ramdisk_size, (unsigned char *)(initrd_param + pos));

	pos = strlen((char *)cmdline);
	strcpy((char *)(cmdline + pos), initrd_param);

	return 0;
}

/*boot.img has been in memory already. just call init_boot_linux() and jump to kernel.*/
void jump_kernel(boot_img_hdr *os_hdr)
{
	void (*kernel)(int, char **, char *);
	if (setup_boot_tag(os_hdr) < 0){
		printf("Jump to kernel start Addr 0x%x\n\n",os_hdr->kernel_addr);
		kernel = (void (*)(int, char **, char *))os_hdr->kernel_addr;
		//flush_cache_all();
		/*Jump to kernel image*/
		(*kernel)(2, (char **)(CONFIG_PARAM_BASE + 16), (char *)CONFIG_PARAM_BASE);
		printf("We should not come here ... \n");
	}
}

int mmc_sectors_boot(unsigned int mmc_select,unsigned int sectors)
{
	unsigned int offset, boffset;
	char run_cmd[20];
	int status;
	boot_img_hdr *os_hdr = (void *)bhdr;

	printf("Enter mmc_boot routine ...\n");
	offset = sectors * mmc->block_dev.blksz;
	boffset = (offset % mmc->block_dev.blksz) ? (offset / mmc->block_dev.blksz + 1) : \
		  (offset / mmc->block_dev.blksz);

	/* Load header*/
	if(mmc->block_dev.block_read(mmc_select, boffset, 4, (void *)os_hdr) < 0){
		printf("boota: mmc failed to read bootimg header\n");
		return -1;
	};

	/*printf the os_hdr info*/
	bootimg_print_image_hdr(os_hdr);

	if (memcmp(os_hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		printf("boota: bad boot image magic\n");
		return -1;
	}
	/* Load kernel*/
	boffset = boffset + (os_hdr->page_size /512);
	if (mmc->block_dev.block_read(mmc_select, boffset,(os_hdr->kernel_size / 512) + 1,
				(void *)os_hdr->kernel_addr) < 0) {
		printf("boota: mmc failed to read kernel\n");
		return -1;
	}

	memset(run_cmd, 0, 20);
	sprintf(run_cmd, "bootelf 0x%x",os_hdr->kernel_addr);
	status = run_command(run_cmd, 0);
	if (status) {
		printf("FAILED jump_kernel\n");
	}

	return 0;
}

int mmc_gpt_boot_kernel(int argc, char * const argv[], unsigned int mmc_select)
{

	char run_cmd[20];
	int status,ret;
	char *ptn = "boot";
	disk_partition_t info;

	/*find gpt boot partition start sector by name*/
	ret = get_partition_info_efi_by_name(dev_desc, ptn, &info);
	if (ret){
		printf("cannot the %s efi partition info",ptn);
	}

	/* Load vmlinux.strip 32M size*/

	if (mmc->block_dev.block_read(mmc_select, info.start, 61440,
				(void *)0x83000000) < 0) {
		printf("boota: mmc failed to read kernel+ ramdisk\n");
		return -1;
	}

	printf("Load vmlinux.strip done!\n");

	memset(run_cmd, 0, 20);
	sprintf(run_cmd, "bootelf");
	status = run_command(run_cmd, 0);
	if (status) {
		printf("FAILED jump_kernel\n");
	}

	return 0;
}

int mmc_gpt_boot(int argc, char * const argv[], unsigned int mmc_select)
{

	int ret;
	unsigned int sector;
	char *ptn = "boot";
	boot_img_hdr *os_hdr = (void *)bhdr;
	disk_partition_t info;
	static int boot_recovery_mode = 0, message_length = 0;
	struct bootloader_message *misc_message;
	char *bootmode;

	bootmode = getenv("bootmode");
	if (bootmode == NULL)
		return -1;

	if (!strcmp(bootmode, "recovery")) {
		boot_recovery_mode = 1;
		ptn = "recovery";
	} else if (!strcmp(bootmode, "fastboot")) {
		run_command("fastboot", 0);
		cli_loop();
		return 0;
	}

	if (boot_recovery_mode) {
		disk_partition_t cache;
		lbaint_t blkcnt;
		lbaint_t blks;
		int recovery_command_size = 0;

		ret = get_partition_info_efi_by_name(dev_desc, CONFIG_ANDROID_CACHE_PARTITION_MMC, &cache);
		if (ret) {
			printf("cannot the %s efi partition info", CONFIG_ANDROID_CACHE_PARTITION_MMC);
			return -1;
		}

		ret = ext4fs_probe(dev_desc, &cache);
		if (ret) {
			printf("mount ext4 filesystem, partition cache FAILE\n");
			return -1;
		}

		/* misc partition */
		ret = get_partition_info_efi_by_name(dev_desc, "misc", &info);
		if (ret) {
			printf("cannot the %s efi partition info", "misc");
			return -1;
		}

		message_length = (sizeof(*misc_message) + info.blksz - 1) & 
			(~(info.blksz - 1));
		blkcnt = message_length / info.blksz;

		if (blkcnt > info.size) {
			printf("too large for partition: '%s'\n", "misc");
			return -1;
		}


		debug("message length %d\n", message_length);

		misc_message = (struct bootloader_message *)malloc(message_length);
		if (!misc_message) {
			printf("FAILED alloc bootload message");
			return -1;
		}

		memset(misc_message, 0x0, message_length);

		/* App update */
		recovery_command_size = ext4fs_open(CONFIG_ANDROID_RECOVERY_CMD_FILE);
		if (recovery_command_size < 0) {
			/* --show_text */
			snprintf(misc_message->recovery, sizeof(misc_message->recovery), 
					"%s", "recovery\n--show_text\n");

			debug("writing ..., info start %lu\n", info.start);
			blks = dev_desc->block_write(dev_desc->dev, info.start, blkcnt,
					(void*)misc_message);
			if (blks != blkcnt) {
				printf("failed writing to device %d\n", dev_desc->dev);
				return -1;
			}

		}

		/*find gpt boot partition start sector by name*/
		ret = get_partition_info_efi_by_name(dev_desc, ptn, &info);
		if (ret) {
			printf("cannot the %s efi partition info",ptn);
		}

	} else {
		/*find gpt boot partition start sector by name*/
		ret = get_partition_info_efi_by_name(dev_desc, ptn, &info);
		if (ret){
			printf("cannot the %s efi partition info",ptn);
		}
	}

	/*printf the partition info*/
	gpt_print_disk_partition(&info);

	/* Load header*/
	if(mmc->block_dev.block_read(mmc_select, info.start, 1, (void *)os_hdr) < 0){
		printf("boota: mmc failed to read bootimg header\n");
		return -1;
	};

	/*printf the os_hdr info*/
	bootimg_print_image_hdr(os_hdr);

	if (memcmp(os_hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		printf("boota: bad boot image magic\n");
		return -1;
	}

	/* Load kernel*/
	sector = info.start + (os_hdr->page_size / 512);
	if (mmc->block_dev.block_read(mmc_select, sector,(os_hdr->kernel_size / 512) + 1,
				(void *)os_hdr->kernel_addr) < 0) {
		printf("boota: mmc failed to read kernel\n");
		return -1;
	}

	/* Load ramdisk*/
	sector += ALIGN_SECTOR(os_hdr->kernel_size, os_hdr->page_size) / 512;
	if (mmc->block_dev.block_read(mmc_select, sector,(os_hdr->ramdisk_size / 512) + 1,
				(void *)os_hdr->ramdisk_addr) < 0) {
		printf("boota: mmc failed to read kernel\n");
		return -1;
	}

	images.os.type = IH_TYPE_KERNEL;
	images.os.comp = IH_COMP_NONE;
	images.os.os = IH_OS_LINUX;

	images.os.end = os_hdr->kernel_addr + os_hdr->kernel_size;
	images.os.load = os_hdr->kernel_addr;
	images.ep = images.os.load + 0x400;//0x80100400
	images.initrd_start = os_hdr->ramdisk_addr;
	images.initrd_end   = os_hdr->ramdisk_addr + os_hdr->ramdisk_size;

	printf("Prepare kernel parameters ...\n");
	do_bootm_linux(BOOTM_STATE_LOADOS, argc, argv, &images);

	return 0;
}

/* boota [ mmc0 | mmc1 [ <partition> ] ] */
int do_boota(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	unsigned long mmc_select, mem_address, sectors;

	if (argc < 2)
		return -1;

	if (!strcmp("mmc",argv[1])) {
		mmc_select=simple_strtoul(argv[2], NULL, 10);
		mem_address=simple_strtoul(argv[3], NULL, 16);
		sectors=simple_strtoul(argv[4], NULL, 10);

		printf("EMMC boot start ......\n");

		if(mmc_ready(mmc_select) != 0){
			printf("MSC boot error\n");
			goto fail;
		}
		if(mmc_sectors_boot(mmc_select, sectors) != 0){
			printf("MSC boot error\n");
			goto fail;
		}
		return 0;
	} else if (!strcmp(argv[1], "mmc0")) {
		mmc_select = 0;
		printf("emmc booting start ...\n");
		mmc_ready(mmc_select);
		mmc_gpt_boot(argc, argv, mmc_select);
	} else if (!strcmp(argv[1], "mmc1")) {
		printf("emmc booting start ...\n");
		mmc_select = 1;
		mmc_ready(mmc_select);
		mmc_gpt_boot(argc, argv, mmc_select);
	} else if (!strcmp(argv[1], "kernel")) {
		printf("kernel booting start ...\n");
		mmc_select = 0;
		mmc_ready(mmc_select);
		mmc_gpt_boot_kernel(argc, argv, mmc_select);
	}

	puts ("boota: Control returned to monitor - resetting...\n");
	do_reset (cmdtp, flag, argc, argv);

	return 1;

fail:
#ifdef CONFIG_FASTBOOT
	return run_command("fastboot", 0);
#else
	return -1;
#endif
}

U_BOOT_CMD(
	boota,	5,	1,	do_boota,
	"boota   - boot android bootimg from memory\n",
	"[ mmc0 | mmc1 | mmc2 | mmcX] [<partition>] \n    - boot application image stored in memory or mmc\n"
	"\t'mmcX' is the mmc device you store your boot.img, which will read the boot.img from 1M offset('/boot' partition)\n"
	"\t 'partition' (optional) is the partition id of your device, if no partition give, will going to 'boot' partition\n"
);
#endif	/* CMD_BOOTA */
