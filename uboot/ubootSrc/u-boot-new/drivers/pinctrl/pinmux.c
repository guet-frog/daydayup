/*
 * Core driver for the pin muxing portions of the pin control subsystem
 *
 * Copyright (C) 2011-2012 ST-Ericsson SA
 * Written on behalf of Linaro for ST-Ericsson
 * Based on bits of regulator core, gpio core and clk core
 *
 * Author: Linus Walleij <linus.walleij@linaro.org>
 *
 * Copyright (C) 2012 NVIDIA CORPORATION. All rights reserved.
 *
 * License terms: GNU General Public License (GPL) version 2
 */
#include <linux/err.h>
#include <linux/list.h>
#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/consumer.h>
#include <malloc.h>
#include <linux/compat.h>
#include "core.h"
#include "pinmux.h"

#define PMUX "pinmux core: "

int pinmux_check_ops(struct pinctrl_dev *pctldev)
{
	const struct pinmux_ops *ops = pctldev->desc->pmxops;
	unsigned nfuncs;
	unsigned selector = 0;

	/* Check that we implement required operations */
	if (!ops ||
	    !ops->get_functions_count ||
	    !ops->get_function_name ||
	    !ops->get_function_groups ||
	    !ops->set_mux) {
		pin_info(PMUX "pinmux ops lacks necessary functions\n");
		return -EINVAL;
	}
	/* Check that all functions registered have names */
	nfuncs = ops->get_functions_count(pctldev);
	while (selector < nfuncs) {
		const char *fname = ops->get_function_name(pctldev,
							   selector);
		if (!fname) {
			pin_info(PMUX "pinmux ops has no name for function%u\n",
				selector);
			return -EINVAL;
		}
		selector++;
	}

	return 0;
}

int pinmux_validate_map(struct pinctrl_map const *map, int i)
{
	if (!map->data.mux.function) {
		pin_info(PMUX "failed to register map %s (%d): no function given\n",
		       map->name, i);
		return -EINVAL;
	}

	return 0;
}

/**
 * pin_request() - request a single pin to be muxed in, typically for GPIO
 * @pin: the pin number in the global pin space
 * @owner: a representation of the owner of this pin; typically the device
 *	name that controls its mux function, or the requested GPIO name
 * @gpio_range: the range matching the GPIO pin if this is a request for a
 *	single GPIO pin
 */
static int pin_request(struct pinctrl_dev *pctldev,
		       int pin, const char *owner,
		       struct pinctrl_gpio_range *gpio_range)
{
	struct pin_desc *desc;
	const struct pinmux_ops *ops = pctldev->desc->pmxops;
	int status = -EINVAL;

	desc = pin_desc_get(pctldev, pin);
	if (desc == NULL) {
		pin_info(PMUX  "pin %d is not registered so it cannot be requested\n",
			pin);
		goto out;
	}

	pin_debug(PMUX "request pin %d (%s) for %s\n", pin, desc->name, owner);

	if (gpio_range) {
		/* There's no need to support multiple GPIO requests */
		if (desc->gpio_owner) {
			pin_info(PMUX "pin %s already requested by %s; cannot claim for %s\n",
				desc->name, desc->gpio_owner, owner);
			goto out;
		}
		if (ops->strict && desc->mux_usecount &&
		    strcmp(desc->mux_owner, owner)) {
			pin_info(PMUX "pin %s already requested by %s; cannot claim for %s\n",
				desc->name, desc->mux_owner, owner);
			goto out;
		}

		desc->gpio_owner = owner;
	} else {
		if (desc->mux_usecount && strcmp(desc->mux_owner, owner)) {
			pin_info(PMUX "pin %s already requested by %s; cannot claim for %s\n",
				desc->name, desc->mux_owner, owner);
			goto out;
		}
		if (ops->strict && desc->gpio_owner) {
			pin_info(PMUX "pin %s already requested by %s; cannot claim for %s\n",
				desc->name, desc->gpio_owner, owner);
			goto out;
		}

		desc->mux_usecount++;
		if (desc->mux_usecount > 1)
			return 0;

		desc->mux_owner = owner;
	}

	/*
	 * If there is no kind of request function for the pin we just assume
	 * we got it by default and proceed.
	 */
	if (gpio_range && ops->gpio_request_enable)
		/* This requests and enables a single GPIO pin */
		status = ops->gpio_request_enable(pctldev, gpio_range, pin);
	else if (ops->request)
		status = ops->request(pctldev, pin);
	else
		status = 0;

	if (status) {
		pin_info(PMUX "request() failed for pin %d\n", pin);

		if (gpio_range) {
			desc->gpio_owner = NULL;
		} else {
			desc->mux_usecount--;
			if (!desc->mux_usecount)
				desc->mux_owner = NULL;
		}

		pin_info(PMUX "pin-%d (%s) status %d\n", pin, owner, status);
	}
out:
	return status;
}

/**
 * pin_free() - release a single muxed in pin so something else can be muxed
 * @pctldev: pin controller device handling this pin
 * @pin: the pin to free
 * @gpio_range: the range matching the GPIO pin if this is a request for a
 *	single GPIO pin
 *
 * This function returns a pointer to the previous owner. This is used
 * for callers that dynamically allocate an owner name so it can be freed
 * once the pin is free. This is done for GPIO request functions.
 */
static const char *pin_free(struct pinctrl_dev *pctldev, int pin,
			    struct pinctrl_gpio_range *gpio_range)
{
	const struct pinmux_ops *ops = pctldev->desc->pmxops;
	struct pin_desc *desc;
	const char *owner;

	desc = pin_desc_get(pctldev, pin);
	if (desc == NULL) {
		pin_info(PMUX "pin is not registered so it cannot be freed\n");
		return NULL;
	}

	if (!gpio_range) {
		/*
		 * A pin should not be freed more times than allocated.
		 */
		if (!desc->mux_usecount)
			return NULL;
		desc->mux_usecount--;
		if (desc->mux_usecount)
			return NULL;
	}

	/*
	 * If there is no kind of request function for the pin we just assume
	 * we got it by default and proceed.
	 */
	if (gpio_range && ops->gpio_disable_free)
		ops->gpio_disable_free(pctldev, gpio_range, pin);
	else if (ops->free)
		ops->free(pctldev, pin);

	if (gpio_range) {
		owner = desc->gpio_owner;
		desc->gpio_owner = NULL;
	} else {
		owner = desc->mux_owner;
		desc->mux_owner = NULL;
		desc->mux_setting = NULL;
	}

	return owner;
}

/**
 * pinmux_request_gpio() - request pinmuxing for a GPIO pin
 * @pctldev: pin controller device affected
 * @pin: the pin to mux in for GPIO
 * @range: the applicable GPIO range
 */
int pinmux_request_gpio(struct pinctrl_dev *pctldev,
			struct pinctrl_gpio_range *range,
			unsigned pin, unsigned gpio)
{
	char dynamic[32];
	char *owner;
	int ret;

	/* Conjure some name stating what chip and pin this is taken by */
	snprintf(dynamic, 32, "%s:%d", range->name, gpio);
	owner = strdup(dynamic);
	if (!owner)
		return -EINVAL;

	ret = pin_request(pctldev, pin, owner, range);
	if (ret < 0)
		kfree(owner);

	return ret;
}

/**
 * pinmux_free_gpio() - release a pin from GPIO muxing
 * @pctldev: the pin controller device for the pin
 * @pin: the affected currently GPIO-muxed in pin
 * @range: applicable GPIO range
 */
void pinmux_free_gpio(struct pinctrl_dev *pctldev, unsigned pin,
		      struct pinctrl_gpio_range *range)
{
	const char *owner;

	owner = pin_free(pctldev, pin, range);
	kfree((void*)owner);
}

/**
 * pinmux_gpio_direction() - set the direction of a single muxed-in GPIO pin
 * @pctldev: the pin controller handling this pin
 * @range: applicable GPIO range
 * @pin: the affected GPIO pin in this controller
 * @input: true if we set the pin as input, false for output
 */
int pinmux_gpio_direction(struct pinctrl_dev *pctldev,
			  struct pinctrl_gpio_range *range,
			  unsigned pin, bool input)
{
	const struct pinmux_ops *ops;
	int ret;

	ops = pctldev->desc->pmxops;

	if (ops->gpio_set_direction)
		ret = ops->gpio_set_direction(pctldev, range, pin, input);
	else
		ret = 0;

	return ret;
}

static int pinmux_func_name_to_selector(struct pinctrl_dev *pctldev,
					const char *function)
{
	const struct pinmux_ops *ops = pctldev->desc->pmxops;
	unsigned nfuncs = ops->get_functions_count(pctldev);
	unsigned selector = 0;

	/* See if this pctldev has this function */
	while (selector < nfuncs) {
		const char *fname = ops->get_function_name(pctldev, selector);

		if (!strcmp(function, fname))
			return selector;

		selector++;
	}

	pin_info(PMUX "function '%s' not supported\n", function);
	return -EINVAL;
}

int pinmux_map_to_setting(struct pinctrl_map const *map,
			  struct pinctrl_setting *setting)
{
	struct pinctrl_dev *pctldev = setting->pctldev;
	const struct pinmux_ops *pmxops = pctldev->desc->pmxops;
	char const * const *groups;
	unsigned num_groups;
	int ret;
	const char *group;
	int i;

	if (!pmxops) {
		pin_info(PMUX "does not support mux function\n");
		return -EINVAL;
	}

	ret = pinmux_func_name_to_selector(pctldev, map->data.mux.function);
	if (ret < 0) {
		pin_info(PMUX "invalid function %s in map table\n",
				map->data.mux.function);
		return ret;
	}
	setting->data.mux.func = ret;

	ret = pmxops->get_function_groups(pctldev, setting->data.mux.func,
					  &groups, &num_groups);
	if (ret < 0) {
		pin_info(PMUX "can't query groups for function %s\n",
			map->data.mux.function);
		return ret;
	}
	if (!num_groups) {
		pin_info(PMUX "function %s can't be selected on any group\n",
			map->data.mux.function);
		return -EINVAL;
	}
	if (map->data.mux.group) {
		bool found = false;
		group = map->data.mux.group;
		for (i = 0; i < num_groups; i++) {
			if (!strcmp(group, groups[i])) {
				found = true;
				break;
			}
		}
		if (!found) {
			pin_info(PMUX  "invalid group \"%s\" for function \"%s\"\n",
				group, map->data.mux.function);
			return -EINVAL;
		}
	} else {
		group = groups[0];
	}

	ret = pinctrl_get_group_selector(pctldev, group);
	if (ret < 0) {
		pin_info(PMUX "invalid group %s in map table\n",
			map->data.mux.group);
		return ret;
	}
	setting->data.mux.group = ret;

	return 0;
}

void pinmux_free_setting(struct pinctrl_setting const *setting)
{
	/* This function is currently unused */
}

int pinmux_enable_setting(struct pinctrl_setting const *setting)
{
	struct pinctrl_dev *pctldev = setting->pctldev;
	const struct pinctrl_ops *pctlops = pctldev->desc->pctlops;
	const struct pinmux_ops *ops = pctldev->desc->pmxops;
	int ret = 0;
	const unsigned *pins = NULL;
	unsigned num_pins = 0;
	int i;
	struct pin_desc *desc;

	pin_debug(PMUX "pinmux setting %s\n", setting->dev_name);

	if (pctlops->get_group_pins)
		ret = pctlops->get_group_pins(pctldev, setting->data.mux.group,
					      &pins, &num_pins);

	pin_debug(PMUX "group %d, pins %d\n", ret, num_pins);

	if (ret) {
		const char *gname;

		/* errors only affect debug data, so just warn */
		gname = pctlops->get_group_name(pctldev,
						setting->data.mux.group);
		pin_info(PMUX "could not get pins for group %s\n", gname);
		num_pins = 0;
	}

	/* Try to allocate all pins in this group, one by one */
	for (i = 0; i < num_pins; i++) {
		ret = pin_request(pctldev, pins[i], setting->dev_name, NULL);
		if (ret) {
			const char *gname;
			const char *pname;

			desc = pin_desc_get(pctldev, pins[i]);
			pname = desc ? desc->name : "non-existing";
			gname = pctlops->get_group_name(pctldev,
						setting->data.mux.group);
			pin_info(PMUX "could not request pin %d (%s) from group %s "
				" on device %s\n",
				pins[i], pname, gname,
				pinctrl_dev_get_name(pctldev));
			goto err_pin_request;
		}
	}

	/* Now that we have acquired the pins, encode the mux setting */
	for (i = 0; i < num_pins; i++) {
		desc = pin_desc_get(pctldev, pins[i]);
		if (desc == NULL) {
			pin_info(PMUX "could not get pin desc for pin %d\n",
				 pins[i]);
			continue;
		}
		desc->mux_setting = &(setting->data.mux);
	}

	ret = ops->set_mux(pctldev, setting->data.mux.func,
			   setting->data.mux.group);

	if (ret)
		goto err_set_mux;

	return 0;

err_set_mux:
	for (i = 0; i < num_pins; i++) {
		desc = pin_desc_get(pctldev, pins[i]);
		if (desc)
			desc->mux_setting = NULL;
	}
err_pin_request:
	/* On error release all taken pins */
	while (--i >= 0)
		pin_free(pctldev, pins[i], NULL);

	return ret;
}

void pinmux_disable_setting(struct pinctrl_setting const *setting)
{
	struct pinctrl_dev *pctldev = setting->pctldev;
	const struct pinctrl_ops *pctlops = pctldev->desc->pctlops;
	int ret = 0;
	const unsigned *pins = NULL;
	unsigned num_pins = 0;
	int i;
	struct pin_desc *desc;

	if (pctlops->get_group_pins)
		ret = pctlops->get_group_pins(pctldev, setting->data.mux.group,
					      &pins, &num_pins);
	if (ret) {
		const char *gname;

		/* errors only affect debug data, so just warn */
		gname = pctlops->get_group_name(pctldev,
						setting->data.mux.group);
		pin_info(PMUX "could not get pins for group %s\n",
			 gname);
		num_pins = 0;
	}

	/* Flag the descs that no setting is active */
	for (i = 0; i < num_pins; i++) {
		desc = pin_desc_get(pctldev, pins[i]);
		if (desc == NULL) {
			pin_info(PMUX "could not get pin desc for pin %d\n",
				 pins[i]);
			continue;
		}
		if (desc->mux_setting == &(setting->data.mux)) {
			desc->mux_setting = NULL;
			/* And release the pin */
			pin_free(pctldev, pins[i], NULL);
		} else {
			const char *gname;

			gname = pctlops->get_group_name(pctldev,
						setting->data.mux.group);
			pin_info(PMUX "not freeing pin %d (%s) as part of "
				 "deactivating group %s - it is already "
				 "used for some other setting",
				 pins[i], desc->name, gname);
		}
	}
}
