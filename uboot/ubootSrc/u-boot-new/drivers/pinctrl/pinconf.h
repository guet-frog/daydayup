/*
 * Internal interface between the core pin control system and the
 * pin config portions
 *
 * Copyright (C) 2011 ST-Ericsson SA
 * Written on behalf of Linaro for ST-Ericsson
 * Based on bits of regulator core, gpio core and clk core
 *
 * Author: Linus Walleij <linus.walleij@linaro.org>
 *
 * License terms: GNU General Public License (GPL) version 2
 */

#ifndef __PINCTRL_PINCONF_H
#define __PINCTRL_PINCONF_H

int pinconf_check_ops(struct pinctrl_dev *pctldev);
int pinconf_validate_map(struct pinctrl_map const *map, int i);
int pinconf_map_to_setting(struct pinctrl_map const *map,
			  struct pinctrl_setting *setting);
void pinconf_free_setting(struct pinctrl_setting const *setting);
int pinconf_apply_setting(struct pinctrl_setting const *setting);

/*
 * You will only be interested in these if you're using PINCONF
 * so don't supply any stubs for these.
 */
int pin_config_get_for_pin(struct pinctrl_dev *pctldev, unsigned pin,
			   unsigned long *config);
int pin_config_group_get(const char *dev_name, const char *pin_group,
			 unsigned long *config);

#endif /* __PINCTRL_PINCONF_H */
