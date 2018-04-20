/*
 * Core driver for the pin control subsystem
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
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/machine.h>
#include <linux/compat.h>
#include <asm-generic/gpio.h>
#include <malloc.h>
#include <common.h>

#include "core.h"
#include "pinmux.h"
#include "pinconf.h"

#define TAGS "pinctrl core: "

static bool pinctrl_dummy_state;

/* Global list of pin control devices (struct pinctrl_dev) */
static LIST_HEAD(pinctrldev_list);

/* List of pin controller handles (struct pinctrl) */
static LIST_HEAD(pinctrl_list);

/* List of pinctrl maps (struct pinctrl_maps) */
LIST_HEAD(pinctrl_maps);

/**
 * pinctrl_provide_dummies() - indicate if pinctrl provides dummy state support
 *
 * Usually this function is called by platforms without pinctrl driver support
 * but run with some shared drivers using pinctrl APIs.
 * After calling this function, the pinctrl core will return successfully
 * with creating a dummy state for the driver to keep going smoothly.
 */
void pinctrl_provide_dummies(void)
{
	pinctrl_dummy_state = true;
}

const char *pinctrl_dev_get_name(struct pinctrl_dev *pctldev)
{
	/* We're not allowed to register devices without name */
	return pctldev->desc->name;
}

const char *pinctrl_dev_get_devname(struct pinctrl_dev *pctldev)
{
	return pctldev->pinctrl;
}

void *pinctrl_dev_get_drvdata(struct pinctrl_dev *pctldev)
{
	return pctldev->driver_data;
}

/**
 * get_pinctrl_dev_from_devname() - look up pin controller device
 * @devname: the name of a device instance, as returned by dev_name()
 *
 * Looks up a pin control device matching a certain device name or pure device
 * pointer, the pure device pointer will take precedence.
 */
struct pinctrl_dev *get_pinctrl_dev_from_devname(const char *devname)
{
	struct pinctrl_dev *pctldev = NULL;

	if (!devname)
		return NULL;

	list_for_each_entry(pctldev, &pinctrldev_list, node) {
		if (!strcmp(pctldev->pinctrl, devname)) {
			/* Matched on device name */
			return pctldev;
		}
	}

	return NULL;
}

/**
 * pin_get_from_name() - look up a pin number from a name
 * @pctldev: the pin control device to lookup the pin on
 * @name: the name of the pin to look up
 */
int pin_get_from_name(struct pinctrl_dev *pctldev, const char *name)
{
	unsigned i, pin;

	/* The pin number can be retrived from the pin controller descriptor */
	for (i = 0; i < pctldev->desc->npins; i++) {
		struct pin_desc *desc;

		pin = pctldev->desc->pins[i].number;
		desc = pin_desc_get(pctldev, pin);
		/* Pin space may be sparse */
		if (desc && !strcmp(name, desc->name))
			return pin;
	}

	return -EINVAL;
}

/**
 * pin_get_name_from_id() - look up a pin name from a pin id
 * @pctldev: the pin control device to lookup the pin on
 * @name: the name of the pin to look up
 */
const char *pin_get_name(struct pinctrl_dev *pctldev, const unsigned pin)
{
	const struct pin_desc *desc;

	desc = pin_desc_get(pctldev, pin);
	if (desc == NULL) {
		pin_info(TAGS "failed to get pin(%d) name\n", pin);
		return NULL;
	}

	return desc->name;
}

/**
 * pin_is_valid() - check if pin exists on controller
 * @pctldev: the pin control device to check the pin on
 * @pin: pin to check, use the local pin controller index number
 *
 * This tells us whether a certain pin exist on a certain pin controller or
 * not. Pin lists may be sparse, so some pins may not exist.
 */
bool pin_is_valid(struct pinctrl_dev *pctldev, int pin)
{
	struct pin_desc *pindesc;

	if (pin < 0)
		return false;

	pindesc = pin_desc_get(pctldev, pin);

	return pindesc != NULL;
}

/* Deletes a range of pin descriptors */
static void pinctrl_free_pindescs(struct pinctrl_dev *pctldev,
				  const struct pinctrl_pin_desc *pins,
				  unsigned num_pins)
{
	int i;

	for (i = 0; i < num_pins; i++) {
		struct pin_desc *pindesc, *pindesc2;

		list_for_each_entry_safe(pindesc, pindesc2,
				&pctldev->pin_desc_tree, pin_desc) {
			if (pindesc->pin == pins[i].number) {
				list_del(&pindesc->pin_desc);

				if (pindesc->dynamic_name)
					kfree((void*)pindesc->name);

				/* free pin description */
				kfree(pindesc);
			}
		}
	}
}

static int pinctrl_register_one_pin(struct pinctrl_dev *pctldev,
				    unsigned number, const char *name)
{
	struct pin_desc *pindesc;

	pindesc = pin_desc_get(pctldev, number);
	if (pindesc != NULL) {
		pin_info(TAGS "pin %d already registered\n", number);
		return -EINVAL;
	}

	pindesc = kzalloc(sizeof(*pindesc), 0);
	if (pindesc == NULL) {
		pin_info(TAGS "failed to alloc struct pin_desc\n");
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&pindesc->pin_desc);

	/* Set owner */
	pindesc->pctldev = pctldev;
	pindesc->pin = number;

	/* Copy basic pin info */
	if (name) {
		pindesc->name = name;
	} else {
		char label[32];

		snprintf(label, 32, "PIN%u", number);
		pindesc->name = strdup(label);
		if (pindesc->name == NULL) {
			kfree(pindesc);
			return -ENOMEM;
		}
		pindesc->dynamic_name = true;
	}

	list_add_tail(&pindesc->pin_desc, &pctldev->pin_desc_tree);

	pin_debug(TAGS "registered pin %d (%s) on %s\n",
		 number, pindesc->name, pctldev->desc->name);

	return 0;
}

static int pinctrl_register_pins(struct pinctrl_dev *pctldev,
				 struct pinctrl_pin_desc const *pins,
				 unsigned num_descs)
{
	unsigned i;
	int ret = 0;

	for (i = 0; i < num_descs; i++) {
		ret = pinctrl_register_one_pin(pctldev,
					       pins[i].number, pins[i].name);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * gpio_to_pin() - GPIO range GPIO number to pin number translation
 * @range: GPIO range used for the translation
 * @gpio: gpio pin to translate to a pin number
 *
 * Finds the pin number for a given GPIO using the specified GPIO range
 * as a base for translation. The distinction between linear GPIO ranges
 * and pin list based GPIO ranges is managed correctly by this function.
 *
 * This function assumes the gpio is part of the specified GPIO range, use
 * only after making sure this is the case (e.g. by calling it on the
 * result of successful pinctrl_get_device_gpio_range calls)!
 */
static inline int gpio_to_pin(struct pinctrl_gpio_range *range,
				unsigned int gpio)
{
	unsigned int offset = gpio - range->base;
	if (range->pins)
		return range->pins[offset];
	else
		return range->pin_base + offset;
}

/**
 * pinctrl_match_gpio_range() - check if a certain GPIO pin is in range
 * @pctldev: pin controller device to check
 * @gpio: gpio pin to check taken from the global GPIO pin space
 *
 * Tries to match a GPIO pin number to the ranges handled by a certain pin
 * controller, return the range or NULL
 */
static struct pinctrl_gpio_range *
pinctrl_match_gpio_range(struct pinctrl_dev *pctldev, unsigned gpio)
{
	struct pinctrl_gpio_range *range = NULL;

	/* Loop over the ranges */
	list_for_each_entry(range, &pctldev->gpio_ranges, node) {
		/* Check if we're in the valid range */
		if (gpio >= range->base &&
		    gpio < range->base + range->npins) {
			return range;
		}
	}

	return NULL;
}

static bool pinctrl_ready_for_gpio_range(unsigned gpio) 
{ 
	return true; 
}

/**
 * pinctrl_get_device_gpio_range() - find device for GPIO range
 * @gpio: the pin to locate the pin controller for
 * @outdev: the pin control device if found
 * @outrange: the GPIO range if found
 *
 * Find the pin controller handling a certain GPIO pin from the pinspace of
 * the GPIO subsystem, return the device and the matching GPIO range. Returns
 * -EPROBE_DEFER if the GPIO range could not be found in any device since it
 * may still have not been registered.
 */
static int pinctrl_get_device_gpio_range(unsigned gpio,
					 struct pinctrl_dev **outdev,
					 struct pinctrl_gpio_range **outrange)
{
	struct pinctrl_dev *pctldev = NULL;


	/* Loop over the pin controllers */
	list_for_each_entry(pctldev, &pinctrldev_list, node) {
		struct pinctrl_gpio_range *range;

		range = pinctrl_match_gpio_range(pctldev, gpio);
		if (range != NULL) {
			*outdev = pctldev;
			*outrange = range;
			return 0;
		}
	}

	return -EPROBE_DEFER;
}

/**
 * pinctrl_add_gpio_range() - register a GPIO range for a controller
 * @pctldev: pin controller device to add the range to
 * @range: the GPIO range to add
 *
 * This adds a range of GPIOs to be handled by a certain pin controller. Call
 * this to register handled ranges after registering your pin controller.
 */
void pinctrl_add_gpio_range(struct pinctrl_dev *pctldev,
			    struct pinctrl_gpio_range *range)
{
	list_add_tail(&range->node, &pctldev->gpio_ranges);
}

void pinctrl_add_gpio_ranges(struct pinctrl_dev *pctldev,
			     struct pinctrl_gpio_range *ranges,
			     unsigned nranges)
{
	int i;

	for (i = 0; i < nranges; i++)
		pinctrl_add_gpio_range(pctldev, &ranges[i]);
}

struct pinctrl_dev *pinctrl_find_and_add_gpio_range(const char *devname,
		struct pinctrl_gpio_range *range)
{
	struct pinctrl_dev *pctldev;

	pctldev = get_pinctrl_dev_from_devname(devname);

	/*
	 * If we can't find this device, let's assume that is because
	 * it has not probed yet, so the driver trying to register this
	 * range need to defer probing.
	 */
	if (!pctldev) {
		return ERR_PTR(-EPROBE_DEFER);
	}
	pinctrl_add_gpio_range(pctldev, range);

	return pctldev;
}

int pinctrl_get_group_pins(struct pinctrl_dev *pctldev, const char *pin_group,
				const unsigned **pins, unsigned *num_pins)
{
	const struct pinctrl_ops *pctlops = pctldev->desc->pctlops;
	int gs;

	if (!pctlops->get_group_pins)
		return -EINVAL;

	gs = pinctrl_get_group_selector(pctldev, pin_group);
	if (gs < 0)
		return gs;

	return pctlops->get_group_pins(pctldev, gs, pins, num_pins);
}

/**
 * pinctrl_find_gpio_range_from_pin() - locate the GPIO range for a pin
 * @pctldev: the pin controller device to look in
 * @pin: a controller-local number to find the range for
 */
struct pinctrl_gpio_range *
pinctrl_find_gpio_range_from_pin(struct pinctrl_dev *pctldev,
				 unsigned int pin)
{
	struct pinctrl_gpio_range *range;

	/* Loop over the ranges */
	list_for_each_entry(range, &pctldev->gpio_ranges, node) {
		/* Check if we're in the valid range */
		if (range->pins) {
			int a;
			for (a = 0; a < range->npins; a++) {
				if (range->pins[a] == pin)
					goto out;
			}
		} else if (pin >= range->pin_base &&
			   pin < range->pin_base + range->npins)
			goto out;
	}
	range = NULL;
out:
	return range;
}

/**
 * pinctrl_remove_gpio_range() - remove a range of GPIOs fro a pin controller
 * @pctldev: pin controller device to remove the range from
 * @range: the GPIO range to remove
 */
void pinctrl_remove_gpio_range(struct pinctrl_dev *pctldev,
			       struct pinctrl_gpio_range *range)
{
	list_del(&range->node);
}

/**
 * pinctrl_get_group_selector() - returns the group selector for a group
 * @pctldev: the pin controller handling the group
 * @pin_group: the pin group to look up
 */
int pinctrl_get_group_selector(struct pinctrl_dev *pctldev,
			       const char *pin_group)
{
	const struct pinctrl_ops *pctlops = pctldev->desc->pctlops;
	unsigned ngroups = pctlops->get_groups_count(pctldev);
	unsigned group_selector = 0;

	while (group_selector < ngroups) {
		const char *gname = pctlops->get_group_name(pctldev,
							    group_selector);
		if (!strcmp(gname, pin_group)) {
			pin_debug(TAGS "found group selector %u for %s\n",
				group_selector, pin_group);
			return group_selector;
		}

		group_selector++;
	}

	pin_info(TAGS "does not have pin group %s\n", pin_group);

	return -EINVAL;
}

/**
 * pinctrl_request_gpio() - request a single pin to be used as GPIO
 * @gpio: the GPIO pin number from the GPIO subsystem number space
 *
 * This function should *ONLY* be used from gpiolib-based GPIO drivers,
 * as part of their gpio_request() semantics, platforms and individual drivers
 * shall *NOT* request GPIO pins to be muxed in.
 */
int pinctrl_request_gpio(unsigned gpio)
{
	struct pinctrl_dev *pctldev;
	struct pinctrl_gpio_range *range;
	int ret;
	int pin;

	pin_debug(TAGS "pinctrl request gpio\n");

	ret = pinctrl_get_device_gpio_range(gpio, &pctldev, &range);
	if (ret) {
		if (pinctrl_ready_for_gpio_range(gpio))
			ret = 0;
		return ret;
	}

	/* Convert to the pin controllers number space */
	pin = gpio_to_pin(range, gpio);

	pin_debug(TAGS "pinctrl gpio %d, pin %d\n", gpio, pin);

	ret = pinmux_request_gpio(pctldev, range, pin, gpio);

	return ret;
}

/**
 * pinctrl_free_gpio() - free control on a single pin, currently used as GPIO
 * @gpio: the GPIO pin number from the GPIO subsystem number space
 *
 * This function should *ONLY* be used from gpiolib-based GPIO drivers,
 * as part of their gpio_free() semantics, platforms and individual drivers
 * shall *NOT* request GPIO pins to be muxed out.
 */
void pinctrl_free_gpio(unsigned gpio)
{
	struct pinctrl_dev *pctldev;
	struct pinctrl_gpio_range *range;
	int ret;
	int pin;

	ret = pinctrl_get_device_gpio_range(gpio, &pctldev, &range);
	if (ret) {
		return;
	}

	/* Convert to the pin controllers number space */
	pin = gpio_to_pin(range, gpio);

	pinmux_free_gpio(pctldev, pin, range);
}

static int pinctrl_gpio_direction(unsigned gpio, bool input)
{
	struct pinctrl_dev *pctldev;
	struct pinctrl_gpio_range *range;
	int ret;
	int pin;

	ret = pinctrl_get_device_gpio_range(gpio, &pctldev, &range);
	if (ret) {
		return ret;
	}

	/* Convert to the pin controllers number space */
	pin = gpio_to_pin(range, gpio);
	ret = pinmux_gpio_direction(pctldev, range, pin, input);

	return ret;
}

/**
 * pinctrl_gpio_direction_input() - request a GPIO pin to go into input mode
 * @gpio: the GPIO pin number from the GPIO subsystem number space
 *
 * This function should *ONLY* be used from gpiolib-based GPIO drivers,
 * as part of their gpio_direction_input() semantics, platforms and individual
 * drivers shall *NOT* touch pin control GPIO calls.
 */
int pinctrl_gpio_direction_input(unsigned gpio)
{
	return pinctrl_gpio_direction(gpio, true);
}

/**
 * pinctrl_gpio_direction_output() - request a GPIO pin to go into output mode
 * @gpio: the GPIO pin number from the GPIO subsystem number space
 *
 * This function should *ONLY* be used from gpiolib-based GPIO drivers,
 * as part of their gpio_direction_output() semantics, platforms and individual
 * drivers shall *NOT* touch pin control GPIO calls.
 */
int pinctrl_gpio_direction_output(unsigned gpio)
{
	return pinctrl_gpio_direction(gpio, false);
}

static struct pinctrl_state *find_state(struct pinctrl *p,
					const char *name)
{
	struct pinctrl_state *state;

	pin_debug(TAGS "find state %s\n", name);

	list_for_each_entry(state, &p->states, node)
		if (!strcmp(state->name, name))
			return state;

	return NULL;
}

static struct pinctrl_state *create_state(struct pinctrl *p,
					  const char *name)
{
	struct pinctrl_state *state;

	state = kzalloc(sizeof(*state), 0);
	if (state == NULL) {
		pin_info(TAGS  "failed to alloc struct pinctrl_state\n");
		return ERR_PTR(-ENOMEM);
	}

	state->name = name;
	INIT_LIST_HEAD(&state->settings);

	list_add_tail(&state->node, &p->states);

	return state;
}

static int add_setting(struct pinctrl *p, struct pinctrl_map const *map)
{
	struct pinctrl_state *state;
	struct pinctrl_setting *setting;
	int ret;

	state = find_state(p, map->name);
	if (!state)
		state = create_state(p, map->name);
	if (IS_ERR(state))
		return PTR_ERR(state);

	if (map->type == PIN_MAP_TYPE_DUMMY_STATE)
		return 0;

	setting = kzalloc(sizeof(*setting), 0);
	if (setting == NULL) {
		pin_info(TAGS  "failed to alloc struct pinctrl_setting\n");
		return -ENOMEM;
	}

	setting->type = map->type;

	setting->pctldev = get_pinctrl_dev_from_devname(map->ctrl_dev_name);
	if (setting->pctldev == NULL) {
		kfree(setting);
		/* Do not defer probing of hogs (circular loop) */
		if (!strcmp(map->ctrl_dev_name, map->dev_name))
			return -ENODEV;
		/*
		 * OK let us guess that the driver is not there yet, and
		 * let's defer obtaining this pinctrl handle to later...
		 */
		pin_info(TAGS "unknown pinctrl device %s in map entry, deferring probe",
			map->ctrl_dev_name);
		return -EPROBE_DEFER;
	}

	setting->dev_name = map->dev_name;

	switch (map->type) {
	case PIN_MAP_TYPE_MUX_GROUP:
		ret = pinmux_map_to_setting(map, setting);
		break;
	case PIN_MAP_TYPE_CONFIGS_PIN:
	case PIN_MAP_TYPE_CONFIGS_GROUP:
		ret = pinconf_map_to_setting(map, setting);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	if (ret < 0) {
		kfree(setting);
		return ret;
	}

	list_add_tail(&setting->node, &state->settings);

	return 0;
}

static struct pinctrl *find_pinctrl(const char *pinctrl)
{
	struct pinctrl *p;

	list_for_each_entry(p, &pinctrl_list, node)
		if (p->dev->pinctrl == pinctrl) {
			return p;
		}

	return NULL;
}

static void pinctrl_free(struct pinctrl *p, bool inlist);

static struct pinctrl *create_pinctrl(const char *pinctrl)
{
	struct pinctrl *p;
	struct pinctrl_maps *maps_node;
	int i;
	struct pinctrl_map const *map;
	int ret;

	/*
	 * create the state cookie holder struct pinctrl for each
	 * mapping, this is what consumers will get when requesting
	 * a pin control handle with pinctrl_get()
	 */
	p = kzalloc(sizeof(*p), 0);
	if (p == NULL) {
		pin_info(TAGS "failed to alloc struct pinctrl\n");
		return ERR_PTR(-ENOMEM);
	}

	INIT_LIST_HEAD(&p->states);

	/* Iterate over the pin control maps to locate the right ones */
	for_each_maps(maps_node, i, map) {
		/* Map must be for this device */
		if (strcmp(map->ctrl_dev_name, pinctrl))
			continue;

		pin_debug(TAGS "add setting %s ...\n", map->dev_name);

		ret = add_setting(p, map);
		/*
		 * At this point the adding of a setting may:
		 *
		 * - Defer, if the pinctrl device is not yet available
		 * - Fail, if the pinctrl device is not yet available,
		 *   AND the setting is a hog. We cannot defer that, since
		 *   the hog will kick in immediately after the device
		 *   is registered.
		 *
		 * If the error returned was not -EPROBE_DEFER then we
		 * accumulate the errors to see if we end up with
		 * an -EPROBE_DEFER later, as that is the worst case.
		 */
		if (ret == -EPROBE_DEFER) {
			pinctrl_free(p, false);
			return ERR_PTR(ret);
		}
	}

	if (ret < 0) {
		/* If some other error than deferral occured, return here */
		pinctrl_free(p, false);
		return ERR_PTR(ret);
	}

	kref_init(&p->users);

	/* Add the pinctrl handle to the global list */
	list_add_tail(&p->node, &pinctrl_list);

	return p;
}

/**
 * pinctrl_get() - retrieves the pinctrl handle for a device
 * @dev: the device to obtain the handle for
 */
struct pinctrl *pinctrl_get(const char *pinctrl)
{
	struct pinctrl *p;

	if (!pinctrl)
		return ERR_PTR(-EINVAL);

	/*
	 * See if somebody else (such as the device core) has already
	 * obtained a handle to the pinctrl for this device. In that case,
	 * return another pointer to it.
	 */
	p = find_pinctrl(pinctrl);
	if (p != NULL) {
		pin_debug(TAGS "obtain a copy of previously claimed pinctrl\n");
		kref_get(&p->users);
		return p;
	}

	pin_debug(TAGS "creating pinctrl ...\n");

	return create_pinctrl(pinctrl);
}

static void pinctrl_free_setting(bool disable_setting,
				 struct pinctrl_setting *setting)
{
	switch (setting->type) {
	case PIN_MAP_TYPE_MUX_GROUP:
		if (disable_setting)
			pinmux_disable_setting(setting);
		pinmux_free_setting(setting);
		break;
	case PIN_MAP_TYPE_CONFIGS_PIN:
	case PIN_MAP_TYPE_CONFIGS_GROUP:
		pinconf_free_setting(setting);
		break;
	default:
		break;
	}
}

static void pinctrl_free(struct pinctrl *p, bool inlist)
{
	struct pinctrl_state *state, *n1;
	struct pinctrl_setting *setting, *n2;

	list_for_each_entry_safe(state, n1, &p->states, node) {
		list_for_each_entry_safe(setting, n2, &state->settings, node) {
			pinctrl_free_setting(state == p->state, setting);
			list_del(&setting->node);
			kfree(setting);
		}
		list_del(&state->node);
		kfree(state);
	}

	if (inlist)
		list_del(&p->node);

	kfree(p);
}

/**
 * pinctrl_release() - release the pinctrl handle
 * @kref: the kref in the pinctrl being released
 */
static void pinctrl_release(struct kref *kref)
{
	struct pinctrl *p = container_of(kref, struct pinctrl, users);

	pinctrl_free(p, true);
}

/**
 * pinctrl_put() - decrease use count on a previously claimed pinctrl handle
 * @p: the pinctrl handle to release
 */
void pinctrl_put(struct pinctrl *p)
{
	kref_put(&p->users, pinctrl_release);
}

/**
 * pinctrl_lookup_state() - retrieves a state handle from a pinctrl handle
 * @p: the pinctrl handle to retrieve the state from
 * @name: the state name to retrieve
 */
struct pinctrl_state *pinctrl_lookup_state(struct pinctrl *p,
						 const char *name)
{
	struct pinctrl_state *state;

	state = find_state(p, name);
	if (!state) {
		if (pinctrl_dummy_state) {
			/* create dummy state */
			pin_info(TAGS "using pinctrl dummy state (%s)\n", name);
			state = create_state(p, name);
		} else
			state = ERR_PTR(-ENODEV);
	}

	return state;
}

/**
 * pinctrl_select_state() - select/activate/program a pinctrl state to HW
 * @p: the pinctrl handle for the device that requests configuration
 * @state: the state handle to select/activate/program
 */
int pinctrl_select_state(struct pinctrl *p, struct pinctrl_state *state)
{
	struct pinctrl_setting *setting, *setting2;
	struct pinctrl_state *old_state = p->state;
	int ret;

	pin_debug(TAGS "pinctrl select state %s\n", state->name);

	if (p->state == state)
		return 0;

	if (p->state) {
		/*
		 * For each pinmux setting in the old state, forget SW's record
		 * of mux owner for that pingroup. Any pingroups which are
		 * still owned by the new state will be re-acquired by the call
		 * to pinmux_enable_setting() in the loop below.
		 */
		list_for_each_entry(setting, &p->state->settings, node) {
			if (setting->type != PIN_MAP_TYPE_MUX_GROUP)
				continue;
			pinmux_disable_setting(setting);
		}
	}

	p->state = NULL;

	/* Apply all the settings for the new state */
	list_for_each_entry(setting, &state->settings, node) {
		switch (setting->type) {
		case PIN_MAP_TYPE_MUX_GROUP:
			ret = pinmux_enable_setting(setting);
			break;
		case PIN_MAP_TYPE_CONFIGS_PIN:
		case PIN_MAP_TYPE_CONFIGS_GROUP:
			ret = pinconf_apply_setting(setting);
			break;
		default:
			ret = -EINVAL;
			break;
		}

		if (ret < 0) {
			goto unapply_new_state;
		}
	}

	p->state = state;

	return 0;

unapply_new_state:
	pin_info(TAGS "Error applying setting, reverse things back\n");

	list_for_each_entry(setting2, &state->settings, node) {
		if (&setting2->node == &setting->node)
			break;
		/*
		 * All we can do here is pinmux_disable_setting.
		 * That means that some pins are muxed differently now
		 * than they were before applying the setting (We can't
		 * "unmux a pin"!), but it's not a big deal since the pins
		 * are free to be muxed by another apply_setting.
		 */
		if (setting2->type == PIN_MAP_TYPE_MUX_GROUP)
			pinmux_disable_setting(setting2);
	}

	/* There's no infinite recursive loop here because p->state is NULL */
	if (old_state)
		pinctrl_select_state(p, old_state);

	return ret;
}

int pinctrl_register_map(struct pinctrl_map const *maps, unsigned num_maps,
			 bool dup)
{
	int i, ret;
	struct pinctrl_maps *maps_node;

	pin_debug(TAGS "add %u pinctrl maps\n", num_maps);

	/* First sanity check the new mapping */
	for (i = 0; i < num_maps; i++) {
		if (!maps[i].dev_name) {
			pin_info(TAGS "failed to register map %s (%d): no device given\n",
			       maps[i].name, i);
			return -EINVAL;
		}

		if (!maps[i].name) {
			pin_info(TAGS "failed to register map %d: no map name given\n",
			       i);
			return -EINVAL;
		}

		if (maps[i].type != PIN_MAP_TYPE_DUMMY_STATE &&
				!maps[i].ctrl_dev_name) {
			pin_info(TAGS "failed to register map %s (%d): no pin control device given\n",
			       maps[i].name, i);
			return -EINVAL;
		}

		switch (maps[i].type) {
		case PIN_MAP_TYPE_DUMMY_STATE:
			break;
		case PIN_MAP_TYPE_MUX_GROUP:
			ret = pinmux_validate_map(&maps[i], i);
			if (ret < 0)
				return ret;
			break;
		case PIN_MAP_TYPE_CONFIGS_PIN:
		case PIN_MAP_TYPE_CONFIGS_GROUP:
			ret = pinconf_validate_map(&maps[i], i);
			if (ret < 0)
				return ret;
			break;
		default:
			pin_info(TAGS "failed to register map %s (%d): invalid type given\n",
			       maps[i].name, i);
			return -EINVAL;
		}
	}

	maps_node = kzalloc(sizeof(*maps_node), 0);
	if (!maps_node) {
		pin_info(TAGS "failed to alloc struct pinctrl_maps\n");
		return -ENOMEM;
	}

	maps_node->num_maps = num_maps;
	if (dup) {
		maps_node->maps = malloc(sizeof(*maps) * num_maps);
		if (!maps_node->maps) {
			pin_info(TAGS "failed to duplicate mapping table\n");
			kfree(maps_node);
			return -ENOMEM;
		}

		memcpy((void*)maps_node->maps, 
				(void*)maps, sizeof(*maps) * num_maps);
	} else {
		maps_node->maps = maps;
	}

	list_add_tail(&maps_node->node, &pinctrl_maps);

	pin_debug(TAGS "pinctrl register map register okay\n");

	return 0;
}

/**
 * pinctrl_register_mappings() - register a set of pin controller mappings
 * @maps: the pincontrol mappings table to register. This should probably be
 *	marked with __initdata so it can be discarded after boot. This
 *	function will perform a shallow copy for the mapping entries.
 * @num_maps: the number of maps in the mapping table
 */
int pinctrl_register_mappings(struct pinctrl_map const *maps,
			      unsigned num_maps)
{
	return pinctrl_register_map(maps, num_maps, true);
}

void pinctrl_unregister_map(struct pinctrl_map const *map)
{
	struct pinctrl_maps *maps_node;

	list_for_each_entry(maps_node, &pinctrl_maps, node) {
		if (maps_node->maps == map) {
			list_del(&maps_node->node);
			kfree(maps_node);
			return;
		}
	}
}

/**
 * pinctrl_force_sleep() - turn a given controller device into sleep state
 * @pctldev: pin controller device
 */
int pinctrl_force_sleep(struct pinctrl_dev *pctldev)
{
	if (!IS_ERR(pctldev->p) && !IS_ERR(pctldev->hog_sleep))
		return pinctrl_select_state(pctldev->p, pctldev->hog_sleep);
	return 0;
}

/**
 * pinctrl_force_default() - turn a given controller device into default state
 * @pctldev: pin controller device
 */
int pinctrl_force_default(struct pinctrl_dev *pctldev)
{
	if (!IS_ERR(pctldev->p) && !IS_ERR(pctldev->hog_default))
		return pinctrl_select_state(pctldev->p, pctldev->hog_default);
	return 0;
}

static int pinctrl_check_ops(struct pinctrl_dev *pctldev)
{
	const struct pinctrl_ops *ops = pctldev->desc->pctlops;

	if (!ops || !ops->get_groups_count || !ops->get_group_name)
		return -EINVAL;

	return 0;
}

/**
 * pinctrl_register() - register a pin controller device
 * @pctldesc: descriptor for this pin controller
 * @dev: parent device for this pin controller
 * @driver_data: private pin controller data for this pin controller
 */
struct pinctrl_dev *pinctrl_register(struct pinctrl_desc *pctldesc,
		char *pinctrl, void *driver_data)
{
	struct pinctrl_dev *pctldev;
	int ret;

	if (!pctldesc)
		return ERR_PTR(-EINVAL);
	if (!pctldesc->name)
		return ERR_PTR(-EINVAL);

	pctldev = kzalloc(sizeof(*pctldev), 0);
	if (pctldev == NULL) {
		pin_info(TAGS "failed to alloc struct pinctrl_dev\n");
		return ERR_PTR(-ENOMEM);
	}

	/* Initialize pin control device struct */
	pctldev->desc = pctldesc;
	pctldev->driver_data = driver_data;
	INIT_LIST_HEAD(&pctldev->pin_desc_tree);
	INIT_LIST_HEAD(&pctldev->gpio_ranges);
	pctldev->pinctrl = pinctrl; 

	/* check core ops for sanity */
	ret = pinctrl_check_ops(pctldev);
	if (ret) {
		pin_info(TAGS "pinctrl ops lacks necessary functions\n");
		goto out_err;
	}

	/* If we're implementing pinmuxing, check the ops for sanity */
	if (pctldesc->pmxops) {
		ret = pinmux_check_ops(pctldev);
		if (ret)
			goto out_err;
	}

	/* If we're implementing pinconfig, check the ops for sanity */
	if (pctldesc->confops) {
		ret = pinconf_check_ops(pctldev);
		if (ret)
			goto out_err;
	}

	/* Register all the pins */
	pin_debug(TAGS "try to register %d pins ...\n",  pctldesc->npins);
	ret = pinctrl_register_pins(pctldev, pctldesc->pins, pctldesc->npins);
	if (ret) {
		pin_info(TAGS "error during pin registration\n");
		pinctrl_free_pindescs(pctldev, pctldesc->pins,
				      pctldesc->npins);
		goto out_err;
	}

	list_add_tail(&pctldev->node, &pinctrldev_list);

	pin_debug(TAGS "pinctrl get pinctrl handle '%s'.\n", pinctrl);

	pctldev->p = pinctrl_get(pinctrl);
	if (pctldev->p->dev == NULL)
		pctldev->p->dev = pctldev;

	pin_debug(TAGS "pinctrl created.\n");

	if (!IS_ERR(pctldev->p)) {
		pctldev->hog_default =
			pinctrl_lookup_state(pctldev->p, PINCTRL_STATE_DEFAULT);
		if (IS_ERR(pctldev->hog_default)) {
			pin_debug(TAGS "failed to lookup the default state\n");
		} else {
			if (pinctrl_select_state(pctldev->p,
						pctldev->hog_default))
				pin_info(TAGS  "failed to select default state\n");
		}

		pctldev->hog_sleep =
			pinctrl_lookup_state(pctldev->p,
						    PINCTRL_STATE_SLEEP);
		if (IS_ERR(pctldev->hog_sleep))
			pin_debug(TAGS "failed to lookup the sleep state\n");
	}

	pin_debug(TAGS "registed ...\n");

	return pctldev;

out_err:
	kfree(pctldev);
	return ERR_PTR(ret);
}

/**
 * pinctrl_unregister() - unregister pinmux
 * @pctldev: pin controller to unregister
 *
 * Called by pinmux drivers to unregister a pinmux.
 */
void pinctrl_unregister(struct pinctrl_dev *pctldev)
{
	struct pinctrl_gpio_range *range, *n;
	if (pctldev == NULL)
		return;

	if (!IS_ERR(pctldev->p))
		pinctrl_put(pctldev->p);

	/* TODO: check that no pinmuxes are still active? */
	list_del(&pctldev->node);
	/* Destroy descriptor tree */
	pinctrl_free_pindescs(pctldev, pctldev->desc->pins,
			      pctldev->desc->npins);
	/* remove gpio ranges map */
	list_for_each_entry_safe(range, n, &pctldev->gpio_ranges, node)
		list_del(&range->node);

	kfree(pctldev);
}
