/*
 * Driver for the Clouder pin controller
 *
 * Based on the original CLOUDER padmux functions
 * Copyright (C) 2009-2011 ST-Ericsson AB
 * Author: Martin Persson <martin.persson@stericsson.com>
 * Author: Linus Walleij <linus.walleij@linaro.org>
 * Author: Wenpin Cui <wenpincui224@gmail.com>
 */
#include <linux/list.h>
#include <asm/io.h>
#include <linux/err.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/consumer.h>
#include <malloc.h>
#include <cx_pmx.h>
#include "core.h"
#include "pinctrl-cx800.h"

#define PLAT "pinplat: "

static const struct clouder_pin_group clouder_pin_groups[] = {
	{
		.name = "uart0grp",
		.pins = uart0_pins,
		.num_pins = ARRAY_SIZE(uart0_pins),
		.func = uart0_func,
	}, {
		.name = "uart1grp",
		.pins = uart1_pins,
		.num_pins = ARRAY_SIZE(uart1_pins),
		.func = uart1_func,
	}, {
		.name = "uart2grp",
		.pins = uart2_pins,
		.num_pins = ARRAY_SIZE(uart2_pins),
		.func = uart2_func,
	}, {
		.name = "uart3grp",
		.pins = uart3_pins,
		.num_pins = ARRAY_SIZE(uart3_pins),
		.func = uart3_func,
	}, {
		.name = "uart4grp",
			.pins = uart4_pins,
			.num_pins = ARRAY_SIZE(uart4_pins),
			.func = uart4_func,
	}, {
		.name = "i2c0grp",
			.pins = i2c0_pins,
			.num_pins = ARRAY_SIZE(i2c0_pins),
			.func = i2c0_func,
	}, {
		.name = "i2c1grp",
			.pins = i2c1_pins,
			.num_pins = ARRAY_SIZE(i2c1_pins),
			.func = i2c1_func,
	}, {
		.name = "i2c2grp",
			.pins = i2c2_pins,
			.num_pins = ARRAY_SIZE(i2c2_pins),
			.func = i2c2_func,
	}, {         
		.name = "i2c3grp",
			.pins = i2c3_pins,
			.num_pins = ARRAY_SIZE(i2c3_pins),
			.func = i2c3_func,
	}, {         
		.name = "i2c4grp",
			.pins = i2c4_pins,
			.num_pins = ARRAY_SIZE(i2c4_pins),
			.func = i2c4_func,
	}, {         
		.name = "i2s0grp",
			.pins = i2s0_pins,
			.num_pins = ARRAY_SIZE(i2s0_pins),
			.func = i2s0_func,
	}, {
		.name = "i2s04chgrp",
			.pins = i2s0_4ch_pins,
			.num_pins = ARRAY_SIZE(i2s0_4ch_pins),
			.func = i2s0_4ch_func,
	}, {
		.name = "mmc04bgrp",
			.pins = mmc0_4b_pins,
			.num_pins = ARRAY_SIZE(mmc0_4b_pins),
			.func = mmc0_4b_func,
	}, {
		.name = "mmc1grp",
			.pins = mmc1_pins,
			.num_pins = ARRAY_SIZE(mmc1_pins),
			.func = mmc1_func,
	},{
		.name = "mmc28bgrp",
			.pins = mmc2_8b_pins,
			.num_pins = ARRAY_SIZE(mmc2_8b_pins),
			.func = mmc2_8b_func,

	},{
		.name = "spi0grp",
			.pins = spi0_pins,
			.num_pins = ARRAY_SIZE(spi0_pins),
			.func = spi0_func,
	},{
		.name = "lcdgrp",
			.pins = lcd_pins,
			.num_pins = ARRAY_SIZE(lcd_pins),
			.func = lcd_func,
	},{            
		.name = "i80grp",
			.pins = i80_pins,
			.num_pins = ARRAY_SIZE(i80_pins),
			.func = i80_func,                 
	},{            
		.name = "pwm0grp",                    
			.pins = pwm0_pins,
			.num_pins = ARRAY_SIZE(pwm0_pins),
			.func = pwm0_func,                 
	},{   
		.name = "csigrp",
			.pins = csi_pins,
			.num_pins = ARRAY_SIZE(csi_pins),
			.func = csi_func,
	}
};

static int clouder_get_groups_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(clouder_pin_groups);
}

static const char *clouder_get_group_name(struct pinctrl_dev *pctldev,
				       unsigned selector)
{
	if (selector >= ARRAY_SIZE(clouder_pin_groups))
		return NULL;

	return clouder_pin_groups[selector].name;
}

static int clouder_get_group_pins(struct pinctrl_dev *pctldev, unsigned selector,
			       const unsigned **pins,
			       unsigned *num_pins)
{
	if (selector >= ARRAY_SIZE(clouder_pin_groups))
		return -EINVAL;

	*pins = clouder_pin_groups[selector].pins;
	*num_pins = clouder_pin_groups[selector].num_pins;

	return 0;
}

static struct pinctrl_ops clouder_pctrl_ops = {
	.get_groups_count = clouder_get_groups_count,
	.get_group_name = clouder_get_group_name,
	.get_group_pins = clouder_get_group_pins,
};

/*
 * functions ...
 */
static const char * const uart_grps[] =
	{ "uart0grp", "uart1grp", "uart2grp", "uart3grp", "uart4grp" };
static const char * const i2c_grps[] =
	{ "i2c0grp", "i2c1grp","i2c2grp","i2c3grp","i2c4grp" };
static const char * const i2s_grps[] =
	{ "i2s0grp", "i2s04chgrp" };
static const char * const mmc_grps[] =
	{ "mmc04bgrp", "mmc08bgrp", "mmc1grp","mmc28bgrp" };
static const char * const spi_grps[] =
	{ "spi0grp"};
static const char * const lcd_grps[] =
	{ "lcdgrp" };
static const char * const i80_grps[] =
	{ "i80grp" };
static const char * const pwm_grps[] =
    { "pwm0grp"  };                   
static const char * const csi_grps[] =
	{ "csigrp" };
	
static const struct clouder_pmx_func clouder_pmx_functions[] = {
	{
		.name = "uart",
		.groups = uart_grps,
		.num_groups = ARRAY_SIZE(uart_grps),
	}, {
		.name = "i2c",
		.groups = i2c_grps,
		.num_groups = ARRAY_SIZE(i2c_grps),
	}, {
		.name = "i2s",
		.groups = i2s_grps,
		.num_groups = ARRAY_SIZE(i2s_grps),
	}, {
		.name = "lcd",
		.groups = lcd_grps,
		.num_groups = ARRAY_SIZE(lcd_grps),
	}, {
		.name = "mmc",
		.groups = mmc_grps,
		.num_groups = ARRAY_SIZE(mmc_grps),
	}, {
		.name = "spi",
		.groups = spi_grps,
		.num_groups = ARRAY_SIZE(spi_grps),
	},{
		.name = "lcd",
		.groups = lcd_grps,
		.num_groups = ARRAY_SIZE(lcd_grps),
	},{
		.name = "i80",
		.groups = i80_grps,
		.num_groups = ARRAY_SIZE(i80_grps),
	},{
        .name = "pwm",
        .groups = pwm_grps,
        .num_groups = ARRAY_SIZE(pwm_grps),
    },{
		.name = "csi",
		.groups = csi_grps,
		.num_groups = ARRAY_SIZE(csi_grps),
	}
};

static int cloduer_pmx_get_functions_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(clouder_pmx_functions);
}

static const char *clouder_pmx_get_func_name(struct pinctrl_dev *pctldev,
					  unsigned selector)
{
	if (selector >= ARRAY_SIZE(clouder_pmx_functions))
		return NULL;

	return clouder_pmx_functions[selector].name;
}

static int clouder_pmx_get_groups(struct pinctrl_dev *pctldev, unsigned selector,
			       const char * const **groups,
			       unsigned * const num_groups)
{
	if (selector >= ARRAY_SIZE(clouder_pmx_functions))
		return -EINVAL;

	*groups = clouder_pmx_functions[selector].groups;
	*num_groups = clouder_pmx_functions[selector].num_groups;

	return 0;
}

static void clouder_setpin_func(struct pinctrl_dev *pctldev, int pin, enum CLOUDER_IOMUX_FUNC func)
{
	u32 val, reg, offset;
	iomux_info_t *info_table;
	struct clouder_pmx *pmx;
	const char *name;

	name = clouder_pads[pin].name;
	pmx = pinctrl_dev_get_drvdata(pctldev);

	for (info_table = iomux_lookup_table;
	     info_table->name != NULL;
	     info_table++) {
		if (!strcmp(name, info_table->name))
			break;
	}

	if (info_table->name == NULL)
		return;

	reg = info_table->reg;
	offset = info_table->offset;

	val = readl(pmx->virtbase + reg);
	val &= ~(0x3<<offset);
	val |= func<<offset;
	writel(val, pmx->virtbase + reg);

	pin_debug(PLAT "%s, reg 0x%x, offset %02d, reg val 0x%08x\n",
			name, reg, offset, val);
}

static int clouder_pmx_set_mux(struct pinctrl_dev *pctldev, unsigned selector,
			   unsigned group)
{
	const enum CLOUDER_IOMUX_FUNC * func;
	const unsigned *pins = NULL;
	unsigned num_pins = 0;
	int i, num_groups;

	pin_debug(PLAT "%s: function: %s, group %s\n",
			__func__, clouder_pmx_functions[selector].name,
			clouder_pin_groups[group].name);

	num_groups = clouder_pmx_functions[selector].num_groups;

	clouder_get_group_pins(pctldev, group, &pins, &num_pins);
	func = clouder_pin_groups[group].func;

	for (i = 0; i < num_pins; i++)
		clouder_setpin_func(pctldev, pins[i], func[i]);

	return 0;
}

static int clouder_pmx_gpio_request_enable(struct pinctrl_dev *pctldev,
		struct pinctrl_gpio_range *range, unsigned pin)
{
	struct clouder_pmx *pmx;
	struct clouder_gpio_map *gpio, *gpio2;
	const struct pinconf_ops *ops = pctldev->desc->confops;
	unsigned gpio_idx = 0;
	int ret;

	if (!range) {
		pin_info(PLAT "pmx request, gpio range unregister\n");
		return -ENODEV;
	}

	/* Get gpio index */
	gpio_idx = pin + range->base - range->pin_base;

	pmx = pinctrl_dev_get_drvdata(pctldev);
	if (!pmx) {
		pin_info(PLAT "pin %d unmap to gpio %d, pmx request\n", 
			pin, gpio_idx);
		return -EIO;
	}

	pin_debug("gpio base %d pin base %d gpio %d\n", 
			range->base, range->pin_base, gpio_idx);

	list_for_each_entry_safe(gpio, gpio2, &pmx->gmaps, gmap) {
		long unsigned pmx_func;

		if (gpio->gpio_num != gpio_idx) continue;

		pmx_func = gpio->pimnux_func;

		ret = ops->pin_config_set(pctldev, pin, &pmx_func, 1);
		if (ret) {
			pin_info(PLAT "unable to set pin configuration on pin %d\n", pin); 
			return ret;
		}

		pin_debug(PLAT "pinctrl request gpio %d success, pin %d.\n", gpio_idx, pin);

		return 0;
	}

	return -EIO;
}

static void clouder_pmx_gpio_disable_free(struct pinctrl_dev *pctldev,
		struct pinctrl_gpio_range *range, unsigned pin)
{
	struct clouder_gpio_map *gpio, *gpio2;
	struct clouder_pmx *pmx;
	unsigned gpio_idx = 0;

	if (!range) {
		pin_info(PLAT "pmx free, gpio range unregister\n");
		return;
	}

	/* Get gpio index */
	gpio_idx = pin + range->base - range->pin_base;

	pmx = pinctrl_dev_get_drvdata(pctldev);
	if (!pmx) {
		pin_info(PLAT "pin %d unmap to gpio %d, pmx free\n", 
				pin, gpio_idx);
		return;
	}

	list_for_each_entry_safe(gpio, gpio2, &pmx->gmaps, gmap) {
		if (gpio->pin_num == pin) {
			if (gpio->gpio_num != gpio_idx) {
				pin_info(PLAT "gpio %d desc differ as the given %d", 
					gpio->gpio_num, gpio_idx);
				return;
			}

			pin_info(PLAT "pinctrl free gpio %d success, pin %d.\n", 
					gpio_idx, pin);

			/* Default FUNC0 */
			clouder_setpin_func(pctldev, pin, IOMUX_FUNC0);
		}
	}
}

static struct pinmux_ops clouder_pmx_ops = {
	.get_functions_count = cloduer_pmx_get_functions_count,
	.get_function_name = clouder_pmx_get_func_name,
	.get_function_groups = clouder_pmx_get_groups,
	.set_mux = clouder_pmx_set_mux,
	.gpio_request_enable = clouder_pmx_gpio_request_enable,
	.gpio_disable_free = clouder_pmx_gpio_disable_free
};

/*
 * WARN: Wrong implementation, should not use this any more.
 * these API supposed to set pin properity such as drive strength, pull
 */
int clouder_pin_config_get(struct pinctrl_dev *pctldev,
			unsigned pin, unsigned long *config)
{
	u32 reg, offset;
	iomux_info_t *info_table;
	struct clouder_pmx *pmx;
	const char *name;

	pmx = pinctrl_dev_get_drvdata(pctldev);
	name = clouder_pads[pin].name;

	for (info_table = iomux_lookup_table;
	     info_table->name != NULL;
	     info_table++) {
		if (!strcmp(name, info_table->name))
			break;
	}

	if (info_table->name == NULL)
		return -EINVAL;

	reg = info_table->reg;
	offset = info_table->offset;

	*config = ((readl(pmx->virtbase + reg)>>offset)&0x3);

	pin_info(PLAT "read reg 0x%x, offset 0x%x\n", reg, offset);

	return 0;
}

/*
 * WARN: Wrong implementation, should not use this any more.
 * these API supposed to set pin properity such as drive strength, pull
 */
int clouder_pin_config_set(struct pinctrl_dev *pctldev,
			unsigned pin, unsigned long *configs, 
			unsigned num_configs)
{
	u32 reg, offset, val;
	iomux_info_t *info_table;
	struct clouder_pmx *pmx;
	const char *name;

	if (num_configs >= IOMUX_FUNC_INVALD)
		return -EINVAL;

	name = clouder_pads[pin].name;
	pmx = pinctrl_dev_get_drvdata(pctldev);

	for (info_table = iomux_lookup_table;
	     info_table->name != NULL;
	     info_table++) {
		if (!strcmp(name, info_table->name))
			break;
	}

	if (info_table->name == NULL)
		return -EINVAL;

	reg = info_table->reg;
	offset = info_table->offset;

	val = readl(pmx->virtbase + reg);
	val &= ~(0x3<<offset);
	val |= *configs <<offset;
	writel(val, pmx->virtbase + reg);

	pin_debug(PLAT "write reg 0x%x, offset 0x%x, val 0x%x\n",
			reg, offset, val);

	return 0;
}

static struct pinconf_ops clouder_pconf_ops = {
	.pin_config_get = clouder_pin_config_get,
	.pin_config_set = clouder_pin_config_set,
};

static struct pinctrl_desc clouder_pmx_desc = {
	.pins = clouder_pads,
	.npins = ARRAY_SIZE(clouder_pads),
	.pctlops = &clouder_pctrl_ops,
	.pmxops = &clouder_pmx_ops,
	.confops = &clouder_pconf_ops,
};

int clouder_pmx_init(struct cx_pmx_platdata *pdata)
{
	struct clouder_pmx *pmx;
	int i;

	/* Create state holders etc for this driver */
	pmx = malloc(sizeof(*pmx));
	if (!pmx)
		return -ENOMEM;

	memset(pmx, 0x0, sizeof(*pmx));

	/* resource */
	pmx->phybase = pdata->start;
	pmx->physize = pdata->physize;

	pmx->virtbase = (void*)CKSEG1ADDR(pmx->phybase);
	if (!pmx->virtbase) {
		free(pmx);
		return -ENOMEM;
	}

	pin_debug(PLAT "mmap 0x%08x remap 0x%p\n", pmx->phybase, pmx->virtbase);

	/* Init gmaps tree */
	INIT_LIST_HEAD(&pmx->gmaps);
	for (i = 0; i < ARRAY_SIZE(clouder800_gpio_maps); i++) {
		struct clouder_gpio_map *gpio = &clouder800_gpio_maps[i];

		INIT_LIST_HEAD(&gpio->gmap);

		/* insert to radix tree, reduce search time */
		list_add_tail(&gpio->gmap, &pmx->gmaps);
	}

	pin_debug(PLAT "plat pinctrl register ...\n");

	clouder_pmx_desc.name = pdata->pinctrl;
	pmx->pctl = pinctrl_register(&clouder_pmx_desc, pdata->pinctrl, pmx);
	if (!pmx->pctl) {
		pin_info(PLAT "could not register CLOUDER pinmux driver\n");
		free(pmx);
		return -EINVAL;
	}
	
	/* We will handle a range of GPIO pins */
	for (i = 0; i < ARRAY_SIZE(clouder800_gpio_ranges); i++) {
		pinctrl_add_gpio_range(pmx->pctl, &clouder800_gpio_ranges[i]);
	}

	pin_debug(PLAT "initialized cx pin control driver successful\n");

	return 0;
}
