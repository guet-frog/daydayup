/*
 * Consumer interface the pin control subsystem
 *
 * Copyright (C) 2012 ST-Ericsson SA
 * Written on behalf of Linaro for ST-Ericsson
 * Based on bits of regulator core, gpio core and clk core
 *
 * Author: Linus Walleij <linus.walleij@linaro.org>
 *
 * License terms: GNU General Public License (GPL) version 2
 */
#ifndef __LINUX_PINCTRL_CONSUMER_H
#define __LINUX_PINCTRL_CONSUMER_H

#include <common.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/pinctrl/pinctrl-state.h>

/* This struct is private to the core and should be regarded as a cookie */
struct pinctrl;
struct pinctrl_state;

/* External interface to pin control */
extern int pinctrl_request_gpio(unsigned gpio);
extern void pinctrl_free_gpio(unsigned gpio);
extern int pinctrl_gpio_direction_input(unsigned gpio);
extern int pinctrl_gpio_direction_output(unsigned gpio);

extern struct pinctrl * __must_check pinctrl_get(const char *pinctrl);
extern void pinctrl_put(struct pinctrl *p);
extern struct pinctrl_state * __must_check pinctrl_lookup_state(
							struct pinctrl *p,
							const char *name);
extern int pinctrl_select_state(struct pinctrl *p, struct pinctrl_state *s);

static inline struct pinctrl * __must_check pinctrl_get_select(const char *dev, 
		const char *name)
{
	struct pinctrl *p;
	struct pinctrl_state *s;
	int ret;

	p = pinctrl_get(dev);
	if (IS_ERR(p))
		return p;

	s = pinctrl_lookup_state(p, name);
	if (IS_ERR(s)) {
		pinctrl_put(p);
		return ERR_PTR(PTR_ERR(s));
	}

	ret = pinctrl_select_state(p, s);
	if (ret < 0) {
		pinctrl_put(p);
		return ERR_PTR(ret);
	}

	return p;
}

static inline struct pinctrl * __must_check pinctrl_get_select_default(
		const char *dev)
{
	return pinctrl_get_select(dev, PINCTRL_STATE_DEFAULT);
}

#define pin_debug(fmt, args...) debug(fmt, ##args)
#define pin_info(fmt, args...) printf(fmt, ##args)

#endif /* __LINUX_PINCTRL_CONSUMER_H */
