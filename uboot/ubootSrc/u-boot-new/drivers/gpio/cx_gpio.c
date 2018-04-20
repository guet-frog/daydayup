/* 
 * Copyright (c) 2014-2018, Shanghai SemiClouder Ltd. All rights reserved.
 *
 * Author:
 *	Warnus   <huxianan@cloudersemi.com>
 *       
 *  Revision History: 
 *  ----------------- 
 *  1.1  XXX 12/29/2014 XXX      Initialized by warnus
 *  
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details
 *
 */

#include <linux/list.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <linux/pinctrl/consumer.h>
#include <asm/types.h>
#include <malloc.h>
#include <cx_gpio.h>
static LIST_HEAD(chips);

static struct gpio_desc *cx_gpio_to_desc(unsigned gpio) 
{
	struct cx_gpio_chip *chip, *chip2;
	struct gpio_desc *desc = NULL;

	list_for_each_entry_safe(chip, chip2, &chips, node) {
		if (gpio >= chip->base && gpio <= chip->base + chip->ngpio) {
			struct cx_gpio_bank *bank;
			int i;

			for (i = 0; i < chip->nbank; i++) {
				bank = &chip->banks[i];
				if (!bank) continue;
	
				if ((bank->base <= gpio) && (gpio < bank->base + bank->ngpio)) {
					desc = &bank->desc[gpio-bank->base];
					if (!desc->private)
						desc->private = bank;

					debug("Found desc %p bank %p\n", desc, bank);

					goto claimed;
				}
			}
		}
	}

claimed:
	return desc;
}

int gpio_request(unsigned gpio, const char *label)
{
	struct gpio_desc *desc = cx_gpio_to_desc(gpio);
	struct cx_gpio_bank *bank = (struct cx_gpio_bank*)desc->private;

	debug("gpio %d request, label %s, desc %p bank %p, flags %08x\n", 
			gpio, label, desc, bank, desc->flags);

	if (!desc || !bank || desc->flags & GPIOD_REQUESTED) 
		return 1;

	desc->flags |= GPIOD_REQUESTED;
	desc->offset = gpio - bank->base;

	debug("gpio request pinmux\n");

	/* pmx */
	return pinctrl_request_gpio(gpio);
}

int gpio_free(unsigned gpio)
{
	struct gpio_desc *desc = cx_gpio_to_desc(gpio);

	/* pmx */
	pinctrl_free_gpio(gpio);

	memset(desc, 0x0, sizeof(*desc));

	return 0;
}

int gpio_direction_input(unsigned gpio)
{
	struct gpio_desc *desc = cx_gpio_to_desc(gpio);
	struct cx_gpio_bank *bank = (struct cx_gpio_bank*)desc->private;

	debug("gpio %d direction input\n", gpio);

	if (!desc || !bank)
		return 1;

	if (!(desc->flags & GPIOD_REQUESTED))
		return 1;

	if (desc->flags & GPIOD_IS_IN)
		return 0;

	desc->flags |= GPIOD_IS_IN;

	debug("input gpio %d reg %p offset %d\n", gpio, 
			bank->chip->mmap + bank->port->ddr, desc->offset);

	writel(GPIOF_INPUT << desc->offset, bank->chip->mmap + bank->port->ddr);
	
	return 0;
}

int gpio_direction_output(unsigned gpio, int value)
{
	struct gpio_desc *desc = cx_gpio_to_desc(gpio);
	struct cx_gpio_bank *bank = (struct cx_gpio_bank*)desc->private;
	unsigned int val = 0;

	if (!desc || !bank)
		return 1;

	if (!(desc->flags & GPIOD_REQUESTED))
		return 1;

	if (desc->flags & GPIOD_IS_OUT)
		return 0;

	desc->flags |= GPIOD_IS_OUT;

	val = readl(bank->chip->mmap + bank->port->ddr);
	val &= ~BIT(desc->offset);
	if (value)
		val |= BIT(desc->offset);

	writel(val, bank->chip->mmap + bank->port->ddr);
	
	gpio_set_value(gpio, value);

	return 0;
}

int gpio_get_value(unsigned gpio)                               
{
	struct gpio_desc *desc = cx_gpio_to_desc(gpio);
	struct cx_gpio_bank *bank = (struct cx_gpio_bank*)desc->private;
	u8 reg;
	int val;

	if (!desc || !bank)
		return 1;

	if (!(desc->flags & GPIOD_REQUESTED))
		return 1;

	if (desc->flags & GPIOD_IS_IN)
		reg = bank->port->ext_port;
	else
		reg = bank->port->dr;

	debug("gpio_get_value gpio %d reg %p offset %d\n",
			gpio, bank->chip->mmap + reg, desc->offset);

	val = readl(bank->chip->mmap + reg);

	return (val >> desc->offset) & 0x1;
}

int gpio_set_value(unsigned gpio, int value)
{
	struct gpio_desc *desc = cx_gpio_to_desc(gpio);
	struct cx_gpio_bank *bank = (struct cx_gpio_bank*)desc->private;
	int val;

	if (!desc || !bank)
		return 1;

	if (!(desc->flags & GPIOD_REQUESTED))
		return 1;

	if (desc->flags & GPIOD_IS_IN)
		return 1;

	debug("gpio_set_value gpio %d reg %p offset %d val %d\n",
			gpio, bank->chip->mmap + bank->port->dr, 
			desc->offset, value);

	val = readl(bank->chip->mmap + bank->port->dr);
	if(value) {
		val |= value << desc->offset;
	}else {
		 val &= (~(1<<desc->offset));
	}
	writel(val, bank->chip->mmap + bank->port->dr);

	return 0;
}

int cx_gpio_init(struct cx_gpio_platdata **pdata, int ngpio)
{
	struct cx_gpio_chip *chip;
	struct cx_gpio_platdata *local;
	int i;
	u32 cfg_reg1, cfg_reg2;
	int gpio_base = 0;

	for (i = 0; i < ngpio; i++) {
		int bk;
		char name[32];

		local = pdata[i];
		if (!local) continue;

		chip = malloc(sizeof(*chip));
		if (!chip) {
			printf("Alloc gpio chip %d FAILED\n", local->chip_id);
			continue;
		}

		memset(chip, 0x0, sizeof(*chip));

		/* init ... */
		chip->mmap = (void*)CKSEG1ADDR(local->io);
		chip->pdata = local;
		INIT_LIST_HEAD(&chip->node);
		snprintf(name, 32, "gpio-%d", local->chip_id);
		chip->label = strdup(name);
		chip->idx = local->chip_id;
		chip->base = gpio_base;
		
		cfg_reg1 = readl(chip->mmap + DWAPB_GPIO_CONFIG_REG1);
		chip->nbank = DWAPB_GPIO_PN_MASK(cfg_reg1);

		chip->banks = malloc(sizeof(struct cx_gpio_bank) * chip->nbank);
		if (!chip->banks) {
			printf("Alloc gpio bank %d FAILED\n", local->chip_id);
			free(chip->label);
			free(chip);
			continue;
		}

		memset(chip->banks, 0x0, sizeof(struct cx_gpio_bank) * chip->nbank);

		/* bank */
		cfg_reg2 = readl(chip->mmap + DWAPB_GPIO_CONFIG_REG2);
		for (bk = 0; bk < chip->nbank; bk++) {
			struct cx_gpio_bank *gbk = &chip->banks[bk];

			gbk->base = chip->base + chip->ngpio;

			if (ENCODED_ID_PWIDTH_A(cfg_reg2)) {
				gbk->port = local->porta;
				gbk->ngpio = ENCODED_ID_PWIDTH_A(cfg_reg2);
			} else if (ENCODED_ID_PWIDTH_B(cfg_reg2)) {
				gbk->port = local->portb;
				gbk->ngpio = ENCODED_ID_PWIDTH_B(cfg_reg2);
			} else if (ENCODED_ID_PWIDTH_C(cfg_reg2)) {
				gbk->port = local->portc;
				gbk->ngpio = ENCODED_ID_PWIDTH_C(cfg_reg2);
			} else if (ENCODED_ID_PWIDTH_D(cfg_reg2)) {
				gbk->port = local->portd;
				gbk->ngpio = ENCODED_ID_PWIDTH_D(cfg_reg2);
			}

			gbk->desc = malloc(sizeof(struct gpio_desc) * gbk->ngpio);
			if (!gbk->desc) {
				printf("Alloc gpio %d FAILED\n", local->chip_id);
				free(chip->label);
				free(chip);
				continue;
			}

			debug("bank %p base %d ngpio %d\n",  gbk, gbk->base, gbk->ngpio);

			memset(gbk->desc, 0x0, sizeof(struct gpio_desc) * gbk->ngpio);

			gbk->chip = chip;
			chip->ngpio += gbk->ngpio;
		}

		gpio_base += chip->ngpio;
		list_add(&chip->node, &chips);

		debug("gpio group %d add, base %d ngpio %d\n", chip->idx, chip->base, chip->ngpio);
	}

	return 0;
}
