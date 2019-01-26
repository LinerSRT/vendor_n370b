/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */


#include <platform/mt_gpio.h>

#if (!defined(MACH_FPGA) || defined (FPGA_SIMULATION))
#include <cust_power.h>
#include <cust_gpio_boot.h>
#endif

#include <platform/mt_reg_base.h>
#include <platform/gpio_init.h>

#include <debug.h>
#define GPIO_INIT_DEBUG 1
/*----------------------------------------------------------------------------*/

#define GPIOTAG "[GPIO] "
#define GPIODBG(fmt, arg...)    dprintf(CRITICAL, GPIOTAG "%s: " fmt, __FUNCTION__ ,##arg)
#define GPIOERR(fmt, arg...)    dprintf(CRITICAL, GPIOTAG "%s: " fmt, __FUNCTION__ ,##arg)
#define GPIOVER(fmt, arg...)    dprintf(CRITICAL, GPIOTAG "%s: " fmt, __FUNCTION__ ,##arg)

#define GPIO_WR32(addr, data)   DRV_WriteReg32(addr,data)
#define GPIO_RD32(addr)         DRV_Reg32(addr)

#define ADDR_BIT 0
#define VAL_BIT  1
#define MASK_BIT 2
/*----------------------------------------------------------------------------*/
#if (defined(MACH_FPGA) && !defined (FPGA_SIMULATION))

void mt_gpio_set_default(void)
{
	return;
}

void mt_gpio_set_default_dump(void)
{
	return;
}

#else

void gpio_dump(void)
{
	int i = 0;
	int idx = 0;
	u32 val = 0;

	GPIOVER("fwq .... gpio dct config ++++++++++++++++++++++++++++\n");
	for (i = 0; i < MAX_GPIO_PIN; i++) {
		GPIOVER("gpio[%d]\n", i);
		GPIOVER("gpio[%d], mode(%x)\n", i, mt_get_gpio_mode(0x80000000 + i));
		GPIOVER("gpio[%d], dir(%x)\n", i, mt_get_gpio_dir(0x80000000 + i));
		GPIOVER("gpio[%d], pull_en(%x)\n", i, mt_get_gpio_pull_enable(0x80000000 + i));
		GPIOVER("gpio[%d], pull_sel(%x)\n", i, mt_get_gpio_pull_select(0x80000000 + i));
		GPIOVER("gpio[%d], out(%x)\n", i, mt_get_gpio_out(0x80000000 + i));
		GPIOVER("gpio[%d], smt(%x)\n", i, mt_get_gpio_smt(0x80000000 + i));
		GPIOVER("gpio[%d], ies(%x)\n", i, mt_get_gpio_ies(0x80000000 + i));
		GPIOVER("gpio[%d], in(%x)\n", i, mt_get_gpio_in(0x80000000 + i));
	}

	GPIOVER("fwq .... gpio dct config ----------------------------\n");
}

void mt_gpio_set_default_dump(void)
{
	gpio_dump();
}

#include <platform/boot_mode.h>
#include <debug.h>
#include <stdlib.h>
#include <string.h>
#include <video.h>
#include <dev/uart.h>
#include <arch/arm.h>
#include <arch/arm/mmu.h>
#include <arch/ops.h>
#include <target/board.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_disp_drv.h>
#include <platform/disp_drv.h>
#include <platform/boot_mode.h>
#include <platform/mt_logo.h>
#include <platform/partition.h>
#include <platform/env.h>
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h>
#include <platform/mt_pmic_wrap_init.h>
#include <platform/mt_i2c.h>
#include <platform/mtk_key.h>
#include <platform/mt_rtc.h>
#include <platform/mt_leds.h>
#include <platform/upmu_common.h>
#include <platform/mtk_wdt.h>
#include <platform/disp_drv_platform.h>

const UINT32 gpio_init_value[][3] = {
};

void mt_gpio_set_default_chip(void)
{
	u32 idx;
	u32 val;
	u32 mask;
	unsigned pin = 0;

#ifdef FPGA_SIMULATION

	GPIOVER("fwq .... gpio.................................. 6759 real 0805\n");
	GPIOVER("gpio debug base =%x\n", GPIO_RD32(GPIO_BASE));
	memset(GPIO_BASE, 0, 4096);
#endif

	for (idx = 0; idx < sizeof(gpio_init_value) / ((sizeof(UINT32)) * (MASK_BIT + 1)); idx++) {
		mask = gpio_init_value[idx][MASK_BIT];
		val = GPIO_RD32(gpio_init_value[idx][ADDR_BIT]);
		val &= ~(mask);
		val |= ((gpio_init_value[idx][VAL_BIT]) & mask);
		GPIO_WR32(gpio_init_value[idx][ADDR_BIT], val);
	}

	for (pin = 0; pin < MT_GPIO_BASE_MAX; pin++) {
#ifdef FPGA_SIMULATION
		GPIOVER("GPIO %d dump\n", pin);
#endif
		/* set GPIOx_MODE */
		mt_set_gpio_mode(0x80000000 + pin, gpio_array[pin].mode);

		/* set GPIOx_DIR */
		mt_set_gpio_dir(0x80000000 + pin, gpio_array[pin].dir);

		/* set GPIOx_PULLEN */
		mt_set_gpio_pull_enable(0x80000000 + pin, gpio_array[pin].pullen);

		/* set GPIOx_PULL */
		if (mt_get_gpio_pull_enable(0x80000000 + pin) == GPIO_PULL_ENABLE) {
			mt_set_gpio_pull_select(0x80000000 + pin, gpio_array[pin].pull);
		}

		/* set GPIOx_DATAOUT */
		mt_set_gpio_out(0x80000000 + pin, gpio_array[pin].dataout);

		/* set GPIOx_SMT */
		mt_set_gpio_smt(0x80000000 + pin, gpio_array[pin].smt);

		/* set GPIOx_IES */
		mt_set_gpio_ies(0x80000000 + pin, gpio_array[pin].ies);

		/* set GPIOx_DRV */
		if (gpio_array[pin].driving != GPIO_DRV_UNSUPPORTED)
			mt_set_gpio_drving(0x80000000 + pin, gpio_array[pin].driving);

	}

/* workaround for ap spi1's gpio mode */
/*	mt_set_gpio_mode(0x80000000 + 84, GPIO_MODE_01);
	mt_set_gpio_mode(0x80000000 + 85, GPIO_MODE_01);
	mt_set_gpio_mode(0x80000000 + 86, GPIO_MODE_01);
	mt_set_gpio_mode(0x80000000 + 87, GPIO_MODE_01);
*/
#ifdef SELF_TEST
	mt_gpio_self_test();
#endif
}

#if 0
//Need to remove inform to the related owner
void mt_gpio_set_power(u8 mc1_power, u8 mc2_power, u8 sim1_power, u8 sim2_power)
{

	u32 reg = 0;
	if (mc1_power == GPIO_VIO28) {

		reg = GPIO_RD32(IOCFG_B_BASE + 0x1b0);
		reg &= ~(0x1f << 16);
		reg |= 0x1f << 16;
		GPIO_WR32(IOCFG_B_BASE + 0x0c28, reg);

	} else if (mc1_power == GPIO_VIO18) {
		reg = GPIO_RD32(IOCFG_B_BASE + 0x1b0);
		reg &= ~(0x1f << 16);
		GPIO_WR32(IOCFG_B_BASE + 0x1b0, reg);
	} else {
	 /*VCM*/}

	//sim1
	if (sim1_power == GPIO_VIO28) {

		reg = GPIO_RD32(IOCFG_B_BASE + 0x1a0);
		reg &= ~(0x3f << 20);
		reg |= 0x3f << 20;
		GPIO_WR32(IOCFG_B_BASE + 0x1a0, reg);
	} else if (sim1_power == GPIO_VIO18) {
		reg = GPIO_RD32(IOCFG_B_BASE + 0x1a0);
		reg &= ~(0x3f << 20);
		GPIO_WR32(IOCFG_B_BASE + 0x1a0, reg);
	} else {
	 /*VSIM*/}
	//sim2
	if (sim2_power == GPIO_VIO28) {

		reg = GPIO_RD32(IOCFG_B_BASE + 0x1a0);
		reg &= ~(0x1f);
		reg |= 0x1f << 12;
		GPIO_WR32(IOCFG_B_BASE + 0x1a0, reg);
	} else if (sim1_power == GPIO_VIO18) {
		reg = GPIO_RD32(IOCFG_B_BASE + 0x1a0);
		reg &= ~(0x1f);
		GPIO_WR32(IOCFG_B_BASE + 0x1a0, reg);
	} else {
	 /*VSIM*/}

}
#endif

void mt_gpio_set_default(void)
{

	mt_gpio_set_default_chip();
	return;
}

#ifdef SELF_TEST
int smt_test(void)
{

	int i, val;
	s32 out;
	int res;
	GPIOVER("smt_test test+++++\n");

	for (i = 0; i < MT_GPIO_EXT_MAX; i++) {
		/*prepare test */
		res = mt_set_gpio_mode(i | 0x80000000, 0);
		if (res)
			return -1;
		/*test */
		for (val = 0; val < GPIO_SMT_MAX; val++) {
			GPIOVER("test gpio[%d],smt[%d]\n", i, val);
			if (-1 == mt_set_gpio_smt(i | 0x80000000, val)) {
				GPIOERR(" set smt unsupport\n");
				continue;
			}
			if ((res = mt_set_gpio_smt(i | 0x80000000, val)) != RSUCCESS) {
				GPIOERR(" set smt[%d] fail: %d\n", val, res);
				return -1;
			}
			if (val != mt_get_gpio_smt(i | 0x80000000)) {
				GPIOERR(" get smt[%d] fail: real get %d\n", val, mt_get_gpio_smt(i | 0x80000000));
				return -1;
			}
			if (mt_get_gpio_smt(i | 0x80000000) > 1) {
				GPIOERR(" get smt[%d] value fail: real get %d\n", val, mt_get_gpio_smt(i | 0x80000000));
				return -1;
			}
		}

	}
	GPIOVER("smt_test test----- PASS!\n");
	return 0;

}

int output_test(void)
{

	int i, val;
	s32 out;
	int res;
	GPIOVER("output test+++++\n");

	for (i = 0; i < MT_GPIO_EXT_MAX; i++) {
		/*prepare test */
		res = mt_set_gpio_mode(i | 0x80000000, 0);
		if (res)
			return -1;
		res = mt_set_gpio_dir(i | 0x80000000, GPIO_DIR_OUT);
		if (res)
			return -1;
		/*test */
		for (val = 0; val < GPIO_OUT_MAX; val++) {
			GPIOVER("test gpio[%d],output[%d]\n", i, val);
			if ((res = mt_set_gpio_out(i | 0x80000000, val)) != RSUCCESS) {
				GPIOERR(" set out[%d] fail: %d\n", val, res);
				return -1;
			}
			if (val != mt_get_gpio_out(i | 0x80000000)) {
				GPIOERR(" get out[%d] fail: real get %d\n", val, mt_get_gpio_out(i | 0x80000000));
				return -1;
			}
			if (mt_get_gpio_out(i | 0x80000000) > 1) {
				GPIOERR(" get out[%d] value fail: real get %d\n", val, mt_get_gpio_out(i | 0x80000000));
				return -1;
			}
		}

	}
	GPIOVER("output test----- PASS!\n");
	return 0;

}

int direction_test(void)
{
	int i, val;
	s32 out;
	int res;
	GPIOVER("direction_test test+++++\n");
	for (i = 0; i < MT_GPIO_EXT_MAX; i++) {
		/*prepare test */
		res = mt_set_gpio_mode(i | 0x80000000, 0);
		if (res)
			return -1;

		/*test */
		for (val = 0; val < GPIO_DIR_MAX; val++) {
			GPIOVER("test gpio[%d],direction[%d]\n", i, val);
			if ((res = mt_set_gpio_dir(i | 0x80000000, val)) != RSUCCESS) {
				GPIOERR(" set direction[%d] fail: %d\n", val, res);
				return -1;
			}
			if (val != mt_get_gpio_dir(i | 0x80000000)) {
				GPIOERR(" get direction[%d] fail1 real get %d\n", val, mt_get_gpio_dir(i | 0x80000000));
				return -1;
			}
			if (mt_get_gpio_dir(i | 0x80000000) > 1) {
				GPIOERR(" get direction[%d] value fail2 real get %d\n", val,
					mt_get_gpio_dir(i | 0x80000000));
				return -1;
			}
		}

	}
	GPIOVER("direction_test----- PASS!\n");

	return 0;
}

int mode_test(void)
{
	int i, val;
	s32 out;
	int res;
	GPIOVER("mode_test test+++++\n");
	for (i = 0; i < MT_GPIO_EXT_MAX; i++) {

		/*test */
		for (val = 0; val < GPIO_MODE_MAX; val++) {
			GPIOVER("test gpio[%d],dir[%d]\n", i, val);
			if ((res = mt_set_gpio_mode(i | 0x80000000, val)) != RSUCCESS) {
				GPIOERR(" set mode[%d] fail: %d\n", val, res);
				return -1;
			}
			if (val != mt_get_gpio_mode(i | 0x80000000)) {
				GPIOERR(" get mode[%d] fail: real get %d\n", val, mt_get_gpio_mode(i | 0x80000000));
				return -1;
			}
			if (mt_get_gpio_mode(i | 0x80000000) > 7) {
				GPIOERR(" get mode[%d] value fail: real get %d\n", val,
					mt_get_gpio_mode(i | 0x80000000));
				return -1;
			}
		}

	}
	GPIOVER("mode_test----- PASS!\n");

	return 0;
}

int pullen_test(void)
{
	int i, val;
	s32 out;
	int res;
	GPIOVER("pullen_test  +++++\n");
	for (i = 0; i < MT_GPIO_EXT_MAX; i++) {
		/*prepare test */
		res = mt_set_gpio_mode(i | 0x80000000, 0);
		if (res)
			return -1;

		/*test */
		for (val = 0; val < GPIO_PULL_EN_MAX; val++) {
			GPIOVER("test gpio[%d],pullen[%d]\n", i, val);
			if (-1 == mt_set_gpio_pull_enable(i | 0x80000000, val)) {
				GPIOERR(" set pull_enable unsupport\n");
				continue;
			}
			if (GPIO_NOPULLDOWN == mt_set_gpio_pull_enable(i | 0x80000000, val)) {
				GPIOERR(" set pull_down unsupport\n");
				continue;
			}
			if (GPIO_NOPULLUP == mt_set_gpio_pull_enable(i | 0x80000000, val)) {
				GPIOERR(" set pull_up unsupport\n");
				continue;
			}
			if ((res = mt_set_gpio_pull_enable(i | 0x80000000, val)) != RSUCCESS) {
				GPIOERR(" set pull_enable[%d] fail1 %d\n", val, res);
				return -1;
			}
			if (val != mt_get_gpio_pull_enable(i | 0x80000000)) {
				GPIOERR(" get pull_enable[%d] fail2 real get %d\n", val,
					mt_get_gpio_pull_enable(i | 0x80000000));
				return -1;
			}

			if (mt_get_gpio_pull_enable(i | 0x80000000) > 1) {
				GPIOERR(" get pull_enable[%d] value fail3: real get %d\n", val,
					mt_get_gpio_pull_enable(i | 0x80000000));
				return -1;
			}
		}

	}
	GPIOVER("pullen_test----- PASS!\n");

	return 0;
}

int pullselect_test(void)
{
	int i, val;
	s32 out;
	int res;
	GPIOVER("pullselect_test  +++++\n");
	for (i = 0; i < MT_GPIO_EXT_MAX; i++) {
		/*prepare test */
		res = mt_set_gpio_mode(i | 0x80000000, 0);
		if (res)
			return -1;

		/*test */
		for (val = 0; val < GPIO_PULL_MAX; val++) {
			GPIOVER("test gpio[%d],pull_select[%d]\n", i, val);
			res = mt_set_gpio_pull_select(i | 0x80000000, val);
			if (GPIO_PULL_UNSUPPORTED == res || GPIO_NOPULLUP == res || GPIO_NOPULLDOWN == res) {
				GPIOERR(" set gpio[%d] pull_select[%d] unsupport\n", i, val);
				continue;
			}

			if ((res = mt_set_gpio_pull_select(i | 0x80000000, val)) != RSUCCESS) {
				GPIOERR(" set pull_select[%d] fail1: %d\n", val, res);
				return -1;
			}
			if (val != mt_get_gpio_pull_select(i | 0x80000000)) {
				GPIOERR(" get pull_select[%d] fail2: real get %d\n", val,
					mt_get_gpio_pull_select(i | 0x80000000));
				return -1;
			}
			if (-1 == mt_get_gpio_pull_select(i | 0x80000000)) {
				GPIOERR(" set gpio[%d] pull_select not support\n", i);
			} else if (mt_get_gpio_pull_select(i | 0x80000000) > 2) {
				GPIOERR(" get pull_select[%d] value fail: real get %d\n", val,
					mt_get_gpio_pull_select(i | 0x80000000));
				return -1;
			}
		}

	}
	GPIOVER("pullselect_test----- PASS!\n");

	return 0;
}

void mt_gpio_self_test(void)
{
	int err = 0;
	GPIOVER("GPIO self_test start\n");
	err = mode_test();
	if (err) {
		GPIOVER("GPIO self_test FAIL\n");
		return;
	}

	err = direction_test();
	if (err) {
		GPIOVER("GPIO self_test FAIL\n");
		return;
	}

	err = output_test();
	if (err) {
		GPIOVER("GPIO self_test FAIL\n");
		return;
	}

	err = smt_test();
	if (err) {
		GPIOVER("GPIO self_test FAIL\n");
		return;
	}

	err = pullen_test();
	if (err) {
		GPIOVER("GPIO self_test FAIL\n");
		return;
	}

	err = pullselect_test();
	if (err) {
		GPIOVER("GPIO self_test FAIL\n");
		return;
	}

	GPIOVER("GPIO self_test PASS\n");
}

#endif

#endif
