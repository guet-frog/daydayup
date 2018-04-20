/*
 * Core driver for the pin config portions of the pin control subsystem
 *
 * Copyright (C) 2011 ST-Ericsson SA
 * Written on behalf of Linaro for ST-Ericsson
 *
 * Author: Linus Walleij <linus.walleij@linaro.org>
 *
 * License terms: GNU General Public License (GPL) version 2
 */
#include <asm-generic/errno.h>
#include <linux/errno.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinconf.h>
#include "core.h"
#include "pinconf.h"

#define CONF "pinconfig core: "

int pinconf_check_ops(struct pinctrl_dev *pctldev)
{
	const struct pinconf_ops *ops = pctldev->desc->confops;

	/* We have to be able to config the pins in SOME way */
	if (!ops->pin_config_set && !ops->pin_config_group_set) {
		pin_info(CONF "pinconf has to be able to set a pins config\n");
		return -EINVAL;
	}
	return 0;
}

int pinconf_validate_map(struct pinctrl_map const *map, int i)
{
	if (!map->data.configs.group_or_pin) {
		pin_info(CONF "failed to register map %s (%d): no group/pin given\n",
		       map->name, i);
		return -EINVAL;
	}

	if (!map->data.configs.num_configs ||
			!map->data.configs.configs) {
		pin_info(CONF "failed to register map %s (%d): no configs given\n",
		       map->name, i);
		return -EINVAL;
	}

	return 0;
}

int pin_config_get_for_pin(struct pinctrl_dev *pctldev, unsigned pin,
			   unsigned long *config)
{
	const struct pinconf_ops *ops = pctldev->desc->confops;

	if (!ops || !ops->pin_config_get) {
		pin_info(CONF "cannot get pin configuration, .pin_config_get missing in driver\n");
		return -ENOTSUPP;
	}

	return ops->pin_config_get(pctldev, pin, config);
}

int pin_config_group_get(const char *dev_name, const char *pin_group,
			 unsigned long *config)
{
	struct pinctrl_dev *pctldev;
	const struct pinconf_ops *ops;
	int selector, ret;

	pctldev = get_pinctrl_dev_from_devname(dev_name);
	if (!pctldev) {
		ret = -EINVAL;
		return ret;
	}

	ops = pctldev->desc->confops;

	if (!ops || !ops->pin_config_group_get) {
		pin_info(CONF "cannot get configuration for pin "
			"group, missing group config get function in "
			"driver\n");
		ret = -ENOTSUPP;
		goto unlock;
	}

	selector = pinctrl_get_group_selector(pctldev, pin_group);
	if (selector < 0) {
		ret = selector;
		goto unlock;
	}

	ret = ops->pin_config_group_get(pctldev, selector, config);

unlock:
	return ret;
}

int pinconf_map_to_setting(struct pinctrl_map const *map,
			  struct pinctrl_setting *setting)
{
	struct pinctrl_dev *pctldev = setting->pctldev;
	int pin;

	switch (setting->type) {
	case PIN_MAP_TYPE_CONFIGS_PIN:
		pin = pin_get_from_name(pctldev,
					map->data.configs.group_or_pin);
		if (pin < 0) {
			pin_info(CONF "could not map pin config for \"%s\"",
				map->data.configs.group_or_pin);
			return pin;
		}
		setting->data.configs.group_or_pin = pin;
		break;
	case PIN_MAP_TYPE_CONFIGS_GROUP:
		pin = pinctrl_get_group_selector(pctldev,
					 map->data.configs.group_or_pin);
		if (pin < 0) {
			pin_info(CONF "could not map group config for \"%s\"",
				map->data.configs.group_or_pin);
			return pin;
		}
		setting->data.configs.group_or_pin = pin;
		break;
	default:
		return -EINVAL;
	}

	setting->data.configs.num_configs = map->data.configs.num_configs;
	setting->data.configs.configs = map->data.configs.configs;

	return 0;
}

void pinconf_free_setting(struct pinctrl_setting const *setting)
{
}

int pinconf_apply_setting(struct pinctrl_setting const *setting)
{
	struct pinctrl_dev *pctldev = setting->pctldev;
	const struct pinconf_ops *ops = pctldev->desc->confops;
	int ret;

	if (!ops) {
		pin_info(CONF "missing confops\n");
		return -EINVAL;
	}

	switch (setting->type) {
	case PIN_MAP_TYPE_CONFIGS_PIN:
		if (!ops->pin_config_set) {
			pin_info(CONF "missing pin_config_set op\n");
			return -EINVAL;
		}
		ret = ops->pin_config_set(pctldev,
				setting->data.configs.group_or_pin,
				setting->data.configs.configs,
				setting->data.configs.num_configs);
		if (ret < 0) {
			pin_info(CONF "pin_config_set op failed for pin %d\n",
				setting->data.configs.group_or_pin);
			return ret;
		}
		break;
	case PIN_MAP_TYPE_CONFIGS_GROUP:
		if (!ops->pin_config_group_set) {
			pin_info(CONF "missing pin_config_group_set op\n");
			return -EINVAL;
		}
		ret = ops->pin_config_group_set(pctldev,
				setting->data.configs.group_or_pin,
				setting->data.configs.configs,
				setting->data.configs.num_configs);
		if (ret < 0) {
			pin_info(CONF "pin_config_group_set op failed for group %d\n",
				setting->data.configs.group_or_pin);
			return ret;
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
