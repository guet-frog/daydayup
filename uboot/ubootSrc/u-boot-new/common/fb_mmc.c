/*
 * Copyright 2014 Broadcom Corporation.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <common.h>
#include <fb_mmc.h>
#include <part.h>
#include <aboot.h>
#include <sparse_format.h>
#include <mmc.h>
#include <linux/sizes.h>

#ifndef CONFIG_FASTBOOT_GPT_NAME
#define CONFIG_FASTBOOT_GPT_NAME GPT_ENTRY_NAME
#endif

/* The 64 defined bytes plus the '\0' */
#define RESPONSE_LEN	(64 + 1)

static char *response_str;

void fastboot_fail(const char *s)
{
	strncpy(response_str, "FAIL\0", 5);
	strncat(response_str, s, RESPONSE_LEN - 4 - 1);
}

void fastboot_okay(const char *s)
{
	strncpy(response_str, "OKAY\0", 5);
	strncat(response_str, s, RESPONSE_LEN - 4 - 1);
}

static int get_partition_info_efi_by_name_or_alias(block_dev_desc_t *dev_desc,
		const char *name, disk_partition_t *info)
{
	int ret;

	ret = get_partition_info_efi_by_name(dev_desc, name, info);
	if (ret) {
		/* strlen("fastboot_partition_alias_") + 32(part_name) + 1 */
		char env_alias_name[25 + 32 + 1];
		char *aliased_part_name;

		/* check for alias */
		strcpy(env_alias_name, "fastboot_partition_alias_");
		strncat(env_alias_name, name, 32);
		aliased_part_name = getenv(env_alias_name);
		if (aliased_part_name != NULL)
			ret = get_partition_info_efi_by_name(dev_desc,
					aliased_part_name, info);
	}
	return ret;
}

static void write_raw_image(block_dev_desc_t *dev_desc, disk_partition_t *info,
		const char *part_name, void *buffer,
		unsigned int download_bytes)
{
	lbaint_t blkcnt;
	lbaint_t blks;

	/* determine number of blocks to write */
	blkcnt = ((download_bytes + (info->blksz - 1)) & ~(info->blksz - 1));
	blkcnt = blkcnt / info->blksz;

	if (blkcnt > info->size) {
		error("too large for partition: '%s'\n", part_name);
		fastboot_fail("too large for partition");
		return;
	}

	puts("Flashing Raw Image\n");

	blks = dev_desc->block_write(dev_desc->dev, info->start, blkcnt,
				     buffer);
	if (blks != blkcnt) {
		error("failed writing to device %d\n", dev_desc->dev);
		fastboot_fail("failed writing to device");
		return;
	}

	printf("........ wrote " LBAFU " bytes to '%s'\n", blkcnt * info->blksz,
	       info->name);
	fastboot_okay("");
}

void fb_mmc_flash_write(const char *cmd, void *download_buffer,
			unsigned int download_bytes, char *response)
{
	block_dev_desc_t *dev_desc;
	disk_partition_t info;
	struct mmc *mmc;
	bool boot_partition_access = false;

	/* initialize the response buffer */
	response_str = response;

	mmc = find_mmc_device(CONFIG_FASTBOOT_FLASH_MMC_DEV);
	if (!mmc) {
		error("no mmc device found\n");
		fastboot_fail("no mmc device found");
		return;
	}

	dev_desc = get_dev("mmc", CONFIG_FASTBOOT_FLASH_MMC_DEV);
	if (!dev_desc || dev_desc->type == DEV_TYPE_UNKNOWN) {
		error("invalid mmc device\n");
		fastboot_fail("invalid mmc device");
		return;
	}

	if (strcmp(cmd, CONFIG_FASTBOOT_GPT_NAME) == 0) {
		printf("%s: updating MBR, Primary and Backup GPT(s)\n",
				__func__);
		if (is_valid_gpt_buf(dev_desc, download_buffer)) {
			printf("%s: invalid GPT - refusing to write to flash\n",
					__func__);
			fastboot_fail("invalid GPT partition");
			return;
		}
		if (write_mbr_and_gpt_partitions(dev_desc, download_buffer)) {
			printf("%s: writing GPT partitions failed\n", __func__);
			fastboot_fail("writing GPT partitions failed");
			return;
		}
		printf("........ success\n");
		fastboot_okay("");
		return;
	} else if (strncmp("preloader", cmd, 9) == 0) {
		if (download_bytes >= mmc->capacity_boot) {
			printf("writing mmcblk0boot0 out of range\n");
			fastboot_fail("writing mmcblk0boot0 out of range");
			return;
		}

		printf("boot partition0 size %llu KBit\n", mmc->capacity_boot / SZ_1K);

		snprintf((char *)&info.name, sizeof(info.name), "%s", "mmcblk0boot0");

		info.start = 0x0;
		info.blksz = mmc->block_dev.blksz; 
		info.size  = mmc->capacity_boot / mmc->block_dev.blksz; 

		printf("flashing emmc boot partition0 ......\n");
		boot_partition_access = true;
		/* enable mmcblk0boot0 */
		if (mmc_set_part_conf(mmc, 0x0, 0x1, 0x1)) {
			printf("can not access boot partition0\n");
			fastboot_fail("can not access boot partition0");
			return;
		}
	}else if (strncmp("bootloader", cmd, 10) == 0) {
		if (download_bytes >= mmc->capacity_boot) {
			printf("writing mmcblk0boot1 out of range\n");
			fastboot_fail("writing mmcblk0boot1 out of range");
			return;
		}

		printf("boot partition1 size %llu KBit\n", mmc->capacity_boot / SZ_1K);

		snprintf((char*)&info.name, sizeof(info.name), "%s", "mmcblk0boot1");

		info.start = 0x0;
		info.blksz = mmc->block_dev.blksz; 
		info.size  = mmc->capacity_boot / mmc->block_dev.blksz; 

		printf("flashing emmc boot partition1 ......\n");
		boot_partition_access = true;
		/* enable mmcblk0boot1 */
		if (mmc_set_part_conf(mmc, 0x0, 0x2, 0x2)) {
			printf("can not access boot partition1\n");
			fastboot_fail("can not access boot partition1");
			return;
		}
	} else if (get_partition_info_efi_by_name_or_alias(dev_desc, cmd, &info)) {
		error("cannot find partition: '%s'\n", cmd);
		fastboot_fail("cannot find partition");
		return;
	}

	if (is_sparse_image(download_buffer))
		write_sparse_image(dev_desc, &info, cmd, download_buffer,
				download_bytes);
	else
		write_raw_image(dev_desc, &info, cmd, download_buffer,
				download_bytes);

	if(boot_partition_access) {
		if (mmc_set_part_conf(mmc, 0x0, 0x0, 0x0)) {
			printf("warn, disable boot partition denied\n");
			fastboot_fail("warn, disable boot partition denied");
		}
	}
}

void fb_mmc_erase(const char *cmd, char *response)
{
	int ret;
	block_dev_desc_t *dev_desc;
	disk_partition_t info;
	lbaint_t blks, blks_start, blks_size, grp_size;
	struct mmc *mmc = find_mmc_device(CONFIG_FASTBOOT_FLASH_MMC_DEV);

	if (mmc == NULL) {
		error("invalid mmc device");
		fastboot_fail("invalid mmc device");
		return;
	}

	/* initialize the response buffer */
	response_str = response;

	dev_desc = get_dev("mmc", CONFIG_FASTBOOT_FLASH_MMC_DEV);
	if (!dev_desc || dev_desc->type == DEV_TYPE_UNKNOWN) {
		error("invalid mmc device");
		fastboot_fail("invalid mmc device");
		return;
	}

	ret = get_partition_info_efi_by_name_or_alias(dev_desc, cmd, &info);
	if (ret) {
		error("cannot find partition: '%s'", cmd);
		fastboot_fail("cannot find partition");
		return;
	}

	printf("Erasing Partition found: %s\n", info.name);
	/* Align blocks to erase group size to avoid erasing other partitions */
	grp_size = mmc->erase_grp_size;
	blks_start = (info.start + grp_size - 1) & ~(grp_size - 1);
	if (info.size >= grp_size)
		blks_size = (info.size - (blks_start - info.start)) &
				(~(grp_size - 1));
	else
		blks_size = 0;

	printf("Erasing blocks " LBAFU " to " LBAFU " due to alignment\n",
	       blks_start, blks_start + blks_size);

	blks = dev_desc->block_erase(dev_desc->dev, blks_start, blks_size);
	if (blks != blks_size) {
		error("failed erasing from device %d", dev_desc->dev);
		fastboot_fail("failed erasing from device");
		return;
	}

	printf("........ erased " LBAFU " bytes from '%s'\n",
	       blks_size * info.blksz, info.name);
	fastboot_okay("");
}
