#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm-generic/gpio.h>
#include <asm/io.h>
#include <cx_gpio_map.h>
#include <bootargs.h>

#define  STRENGTH_REG        0xbfd360d4
#define  TOTAL_STRENGTH_REG  0xbfd36014
#define  SWITCH_REG          0xbfd3601c

unsigned int ret = 0;

void motor_init(int status)
{
        unsigned char enable = 3;
        unsigned char disable = 0;
	unsigned short strength = 256;
	unsigned short total_strength = 512;

        unsigned char ldo8[2] = {0x53,0x60};
        unsigned char ldo8_on[2] = {0x44,0x80};
        unsigned char ldo8_off[2] = {0x44,0x00};

        unsigned char ldo9[2] = {0x54,0x60};
        unsigned char ldo9_on[2] = {0x45,0x31};
        unsigned char ldo9_off[2] = {0x45,0x30};

        i2c_set_bus_num(CONFIG_PMU_BUS);

	if(status == 1)
	{

#if defined(PRODUCT_CLOUDER_MINI)||defined(PRODUCT_CLOUDER_MINI2)
		unsigned char buf_2V8[2] = {0x50,0x58};
		unsigned char buf_1V8[2] = {0x53,0x24};
		unsigned char buf_en[2] = {0x44,0x90};
#elif defined(PRODUCT_CLOUDER_M93)||defined(PRODUCT_CLOUDER_GC100)
		unsigned char buf_2V8[2] = {0x4E,0x4C};
		unsigned char buf_1V8[2] = {0x4F,0x24};
		unsigned char buf_en[2] = {0x44,0x0c};
#else
		unsigned char buf_2V8[2] = {0x4E,0x4C};
		unsigned char buf_1V8[2] = {0x4F,0x24};
		unsigned char buf_en[2] = {0x44,0x0c};
#endif
		unsigned char temp = 0;
		unsigned char buffer = 0;
#if defined(PRODUCT_CLOUDER_MINI)||defined(PRODUCT_CLOUDER_MINI2)||defined(PRODUCT_CLOUDER_M93)||defined(PRODUCT_CLOUDER_GC100)
		i2c_write(CONFIG_SYS_I2C_SLAVE,buf_2V8[0],1,&buf_2V8,2);
		i2c_write(CONFIG_SYS_I2C_SLAVE,buf_1V8[0],1,&buf_1V8,2);
		i2c_read(CONFIG_SYS_I2C_SLAVE,0x44,1, &temp,1);
		buf_en[1] |= temp;
		i2c_write(CONFIG_SYS_I2C_SLAVE,0x44,1,&buf_en,2);
#endif

#if defined(PRODUCT_CLOUDER_M93)||defined(PRODUCT_CLOUDER_GC100)
                i2c_read(CONFIG_SYS_I2C_SLAVE, 0x53, 1, &ret, 1);
                ldo8[1] = ldo8[1] | ret;
                i2c_write(CONFIG_SYS_I2C_SLAVE, 0x53, 1, &ldo8, 2);

                i2c_read(CONFIG_SYS_I2C_SLAVE, 0x44, 1, &ret, 1);
                ldo8_on[1] = ldo8_on[1] | ret;
                i2c_write(CONFIG_SYS_I2C_SLAVE, 0x44, 1, &ldo8_on, 2);

#endif

#ifdef PRODUCT_CLOUDER_MINI2
                i2c_read(CONFIG_SYS_I2C_SLAVE, 0x54, 1, &ret, 1);
		ldo9[1] = ldo9[1] | ret;
                i2c_write(CONFIG_SYS_I2C_SLAVE, 0x54, 1, &ldo9, 2);
                i2c_read(CONFIG_SYS_I2C_SLAVE, 0x45, 1, &ret, 1);
                ldo9_on[1] = ldo9_on[1] | ret;
                i2c_write(CONFIG_SYS_I2C_SLAVE, 0x45, 1, &ldo9_on, 2);
#endif

		writel(disable, (void*)SWITCH_REG);
		writel(strength, (void*)STRENGTH_REG);

		writel(total_strength, (void*)TOTAL_STRENGTH_REG);
	}else if(status == 0){

		writel(disable, (void*)SWITCH_REG);

#ifdef PRODUCT_CLOUDER_MINI2
                ldo9_off[1] = ret & (~1);
                i2c_write(CONFIG_SYS_I2C_SLAVE, 0x45, 1, &ldo9_off, 2);
#endif

#if defined(PRODUCT_CLOUDER_M93)||defined(PRODUCT_CLOUDER_GC100)
                ldo8_off[1] = ret & (~(1 << 7));
                i2c_write(CONFIG_SYS_I2C_SLAVE, 0x44, 1, &ldo8_off, 2);
#endif

	}

}
