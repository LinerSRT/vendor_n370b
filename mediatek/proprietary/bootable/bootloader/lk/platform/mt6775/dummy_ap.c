/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2016. All rights reserved.
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
//#define CTP_ENV
#ifndef CTP_ENV
/* Note: Pleae enable DUMMY_AP option at rule.mk if hope to use this function */
#include <platform/boot_mode.h>
#include <debug.h>
#include <dev/uart.h>
#include <platform/mtk_key.h>
#include <target/cust_key.h>
#include <platform/mt_gpio.h>
#include <sys/types.h>
#include <debug.h>
#include <err.h>
#include <reg.h>
#include <string.h>
#include <platform/mt_typedefs.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_irq.h>
#include <platform/mt_pmic.h>
#include <platform/timer.h>
#include <sys/types.h>
#include <arch/ops.h>
#include <platform/mt_pmic.h>
#include <platform/upmu_common.h>
#include <platform/upmu_hw.h>
#include <platform/spm.h>
#else
/*CTP environment*/
#include <gpio.h>
#include <barriers.h>
#include <sync_write.h>
#include <upmu_hw.h>
#include <mt_spm_reg.h>
#include <mt_spm_mtcmos.h>
#include <efuse.h>

#define dprintf(CRITICAL, fmt, args...) dbg_print("[MD]: "fmt, ##args)
static void let_md_go(int md_id);
static void config_md_boot_env(int md_id, int boot_mode);
#endif	/*CTP_ENV*/

//------- feature option part ---------------------------------------
//#define DEFAULT_META
//#define ENABLE_MD_RESET_SPM
//#define ENABLE_MD_RESET_RGU
//#define IGNORE_MD_WDT
//#define IGNORE_MD1_WDT
//#define IGNORE_MD2_WDT
//#define FOR_C2K_SLT

//------- enum and macro part ---------------------------------------
enum {
	MD_SYS1 = 0,
	MD_SYS2,
	MD_SYS3,
	MD_SYS4,
};

enum {
	AP_ONLY = -1,
	MD1_ONLY = 0,
	MD2_ONLY,
	MD1_MD2,
};

#define ccci_write32(b, a, v)           DRV_WriteReg32((b)+(a), (v))
#define ccci_read32(b, a)               DRV_Reg32((b)+(a))
#define ccci_write16(b, a, v)           DRV_WriteReg16((b)+(a), (v))
#define ccci_read16(b, a)               DRV_Reg16((b)+(a))
#define ccci_write8(b, a, v)            DRV_WriteReg8((b)+(a), (v))
#define ccci_read8(b, a)                DRV_Reg8((b)+(a))

// FIXME, just in case of spm.h is wrong
#define C2K_PWR_STA_MASK (0x1 << 6)
#define PWR_RST_B        (0x1 << 0)
#define PWR_ISO          (0x1 << 1)
#define PWR_ON           (0x1 << 2)
#define PWR_ON_2ND       (0x1 << 3)
#define PWR_CLK_DIS      (0x1 << 4)

struct sram_cfg {
	unsigned int offset;
	unsigned int start_bit;
	unsigned int end_bit;
};


//------- IRQ ID part ---------------------------------------
#define GIC_PRIVATE_SIGNALS (32)
#define MT_MD_WDT1_IRQ_ID   (361+GIC_PRIVATE_SIGNALS)
#define MT_MD_WDT2_IRQ_ID   (360+GIC_PRIVATE_SIGNALS)


//------- register part ---------------------------------------
#define MD1_BUS_PROTECT_SET  (0x100002A8)
#define MD1_BUS_PROTECT_CLR  (0x100002AC)
#define MD1_BUS_PROTECT_STA  (0x10000258)
#define MD1_BOOT_VECTOR_EN   (0x20000024)
#define MD1_C2K_CCIRQ_BASE   (0x10705000)
#define C2K_MD1_CCIRQ_BASE   (0x10706000)
#define MD1_META_FLAG        (0x20000010)


#define TOPRGU_BASE            (0x10211000)
#define TOP_RGU_WDT_MODE       (0x0)
#define TOP_RGU_WDT_SWRST      (0x14)
#define TOP_RGU_WDT_SWSYSRST   (0x18)
#define TOP_RGU_WDT_NONRST_REG (0x20)
#define TOP_RGU_LATCH_CONTROL  (0x44)
#define MD1_SYS (1 << 7)
#define C2K_SYS (1 << 15)
#define UNLOCK_KEY (0x88000000)

#define MD_USB_INTERRUPT_MUX 0x10000280

// MD RGU PCore
#define BASE_ADDR_MDRSTCTL   0x200F0000
#define MD_RGU_BASE          (BASE_ADDR_MDRSTCTL + 0x100)
#define WDT_MD_MODE          (0x0)
#define WDT_MD_MODE_KEY      (0x55000030)

#define APMIXED_BASE        (0x10212000)
#define AP_PLL_CON0         (0x0)
#define AP_PLL_CON1         (0x4)
#define MDPLL_CON0          (0x3A0)
#define MDPLL_CON3          (0x3AC)

/* MD1 PLL */
#define PLL_TYPE    (volatile kal_uint32 *)
// AP view
#define BASE_MADDR_MDTOP_PLLMIXED (0x20140000)
#define BASE_MADDR_MDSYS_CLKCTL   (0x20120000)
#define BASE_MADDR_MDTOP_CLKSW    (0x20150000)

#define BASE_MADDR_APMIXED_AP_PLL_CON1	    (PLL_TYPE(APMIXED_BASE+0x4))

#define REG_MDTOP_PLLMIXED_CODA_VERSION	    (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x0))
#define REG_MDTOP_PLLMIXED_DCXO_PLL_SETTLE_CTL	(PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x4))
#define REG_MDTOP_PLLMIXED_PLL_ON_CTL       (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x10))
#define REG_MDTOP_PLLMIXED_PLL_DLY_CTL0     (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x30))
#define REG_MDTOP_PLLMIXED_MDMCUPLL_CTL0    (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x40))
#define REG_MDTOP_PLLMIXED_MDINFRAPLL_CTL0  (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x48))
#define REG_MDTOP_PLLMIXED_MDINFRAPLL_CTL1  (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x4C))
#define REG_MDTOP_PLLMIXED_DFEPLL_CTL0      (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x58))
#define REG_MDTOP_PLLMIXED_RAKEPLL_CTL0	    (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x78))
#define REG_MDTOP_PLLMIXED_RAKEPLL_CTL1	    (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x7C))
#define REG_MDTOP_PLLMIXED_BRPPLL_CTL0	    (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x88))
#define REG_MDTOP_PLLMIXED_BRPPLL_CTL1	    (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x8C))
#define REG_MDTOP_PLLMIXED_IMCPLL_CTL0	    (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x90))
#define REG_MDTOP_PLLMIXED_IMCPLL_CTL1	    (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x94))
#define REG_MDTOP_PLLMIXED_ICCPLL_CTL0	    (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x98))
#define REG_MDTOP_PLLMIXED_ICCPLL_CTL1	    (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x9C))
#define REG_MDTOP_PLLMIXED_PLL_HP_RDY_IRQ   (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x314))
#define REG_MDTOP_PLLMIXED_PLL_HP_RDY_IRQ_MASK  (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0x318))
#define REG_MDTOP_PLLMIXED_MDMCUPLL_STS         (PLL_TYPE(BASE_MADDR_MDTOP_PLLMIXED+0xC00))

#define REG_MDSYS_CLKCTL_MDMCU_AO_CLK_CTL	(PLL_TYPE(BASE_MADDR_MDSYS_CLKCTL+0x240))

#define REG_MDTOP_CLKSW_CLKON_CTL           (PLL_TYPE(BASE_MADDR_MDTOP_CLKSW+0x20))
#define REG_MDTOP_CLKSW_CLKSEL_CTL          (PLL_TYPE(BASE_MADDR_MDTOP_CLKSW+0x24))
#define REG_MDTOP_CLKSW_BUS_FLEXCKGEN_STS   (PLL_TYPE(BASE_MADDR_MDTOP_CLKSW+0x80))
#define REG_MDTOP_CLKSW_PLL_STS	            (PLL_TYPE(BASE_MADDR_MDTOP_CLKSW+0xD0))
#define REG_MDTOP_CLKSW_CLK_DUMMY           (BASE_MADDR_MDTOP_CLKSW+0xF00)

#define MD_GLOBAL_CON_DUMMY         (PLL_TYPE(REG_MDTOP_CLKSW_CLK_DUMMY))
#define MD_PLL_MAGIC_NUM            0x62920000


/*MD PCore SRAM register*/
#define MD_SRAM_PD_PSMCUSYS_BASE   (0x200D0000)
#define sw_pd_psmcusys_sram_1  (0x160)
#define sw_pd_psmcusys_sram_2  (0x164)
#define sw_pd_psmcusys_sram_3  (0x168)
#define sw_pd_psmcusys_sram_4  (0x16C)
#define sw_pd_psmcusys_sram_10 (0x184)
#define sw_pd_psmcusys_sram_11 (0x188)
#define sw_pd_psmcusys_sram_12 (0x18C)
#define SW_PD_FOR_SRAM_1        (0x190)
#define MD_MCU_CLOCK_REG1      (0x50)
#define MD_MCU_CLOCK_REG2      (0x54)
#define MD_MCU_CLOCK_REG3      (0x840)
#define MD_MCU_CLOCK_REG4      (0x850)
#define MD_MCU_CLOCK_REG5      (0xA34)


#define UINT32P         (volatile unsigned int *)

#define SLEEP_BASE         (0x10A00000)
#define POWERON_CONFIG_EN  (UINT32P (SLEEP_BASE+0x0))
#define SPM_POWER_ON_VAL1  (UINT32P (SLEEP_BASE+0x8))
#define PWR_STATUS         (UINT32P (SLEEP_BASE+0x190))
#define PWR_STATUS_2ND     (UINT32P (SLEEP_BASE+0x194))
#define C2K_PWR_CON        (UINT32P (SLEEP_BASE+0x314))

#define INFRACFG_AO_BASE    (0x10000000)
#define INFRA_TOPAXI_PROTECTEN_1_SET (0x2A8)
#define INFRA_TOPAXI_PROTECTEN_1_CLR (0x2AC)
#define C2K_SPM_CTRL            (0x368)
#define C2K_STATUS              (0x364)
#define C2K_CONFIG              (0x360)
#define INFRA_MISC2             (0xF0C)


#define C2KSYS_BASE             (0x38000000)
#define C2K_CGBR1               (0x0200B004)
#define C2K_PLL_CON2            (0x02013004)
#define C2K_PLL_CON3            (0x02013008)
#define C2K_PLLTD_CON0          (0x02013074)
#define C2K_CLK_CTRL4           (0x02000010)
#define C2K_CLK_CTRL9           (0x0200029C)
#define C2K_CG_ARM_AMBA_CLKSEL  (0x02000234)
#define C2K_CPPLL_CON0          (0x02013040)


//------- GPIO part ---------------------------------------
#define GPIO_MAGIC		(0x80000000)
// -- SIM1
#define SIM1_SIO_GPIO_ID    (GPIO193 | GPIO_MAGIC)
#define SIM1_SRST_GPIO_ID   (GPIO192 | GPIO_MAGIC)
#define SIM1_SCLK_GPIO_ID   (GPIO191 | GPIO_MAGIC)
#define SIM1_HOT_PLUG_GPIO_ID   (GPIO38 | GPIO_MAGIC)
// -- SIM2
#define SIM2_SIO_GPIO_ID    (GPIO18 | GPIO_MAGIC)
#define SIM2_SRST_GPIO_ID   (GPIO17 | GPIO_MAGIC)
#define SIM2_SCLK_GPIO_ID   (GPIO16 | GPIO_MAGIC)
#define SIM2_HOT_PLUG_GPIO_ID   (GPIO39 | GPIO_MAGIC)
// -- UART
#define UART1_TX_GPIO_ID (GPIO98 | GPIO_MAGIC)
#define UART1_RX_GPIO_ID (GPIO97 | GPIO_MAGIC)
#define UART2_TX_GPIO_ID (GPIO161 | GPIO_MAGIC)
#define UART2_RX_GPIO_ID (GPIO160 | GPIO_MAGIC)
#define UART3_TX_GPIO_ID (GPIO87 | GPIO_MAGIC)
#define UART3_RX_GPIO_ID (GPIO86 | GPIO_MAGIC)
#define UART4_TX_GPIO_ID (GPIO190 | GPIO_MAGIC)
#define UART4_RX_GPIO_ID (GPIO189 | GPIO_MAGIC)


//------- code part ---------------------------------------
static unsigned int img_load_flag = 0;
struct sram_cfg sram_on_sequence[] = {
#if 1
	{sw_pd_psmcusys_sram_1, 0, 18},
	{sw_pd_psmcusys_sram_3, 8, 31},
	{sw_pd_psmcusys_sram_4, 0, 31},
	{SW_PD_FOR_SRAM_1, 0, 31},
#else
	{0x160, 0, 31},
	{0x164, 0, 31},
	{0x168, 0, 31},
	{0x16C, 0, 31},
	{0x170, 0, 31},
	{0x174, 0, 31},
	{0x178, 0, 31},
	{0x17C, 0, 31},
	{0x180, 0, 31},
	{0x184, 0, 31},
	{0x188, 0, 31},
	{0x18C, 0, 31},
	{0x190, 0, 31},
#endif
};

static void md_gpio_get(GPIO_PIN pin, char *tag);
static void md_gpio_set(GPIO_PIN pin, GPIO_MODE mode, GPIO_DIR dir, GPIO_OUT out, GPIO_PULL_EN pull_en, GPIO_PULL pull, GPIO_SMT smt);

#ifndef CTP_ENV
extern BOOT_ARGUMENT    *g_boot_arg;
extern char *ld_md_errno_to_str(int errno);

static int meta_detection(void)
{
	int boot_mode;
#ifdef DEFAULT_META
	boot_mode = 1;
	return boot_mode;
#endif
	boot_mode = 0;
	if (g_boot_arg->boot_mode != NORMAL_BOOT)
		boot_mode = 1;
	dprintf(CRITICAL, "Meta mode: %d, boot_mode: %d\n", boot_mode, g_boot_arg->boot_mode);
	return boot_mode;
}

void md_wdt_init(void)
{
	if (img_load_flag & (1 << MD_SYS1)) {
		mt_irq_set_sens(MT_MD_WDT1_IRQ_ID, MT65xx_EDGE_SENSITIVE);
		mt_irq_set_polarity(MT_MD_WDT1_IRQ_ID, MT65xx_POLARITY_LOW);
		mt_irq_unmask(MT_MD_WDT1_IRQ_ID);
	}
	if (img_load_flag & (1 << MD_SYS3)) {
		mt_irq_set_sens(MT_MD_WDT2_IRQ_ID, MT65xx_EDGE_SENSITIVE);
		mt_irq_set_polarity(MT_MD_WDT2_IRQ_ID, MT65xx_POLARITY_LOW);
		mt_irq_unmask(MT_MD_WDT2_IRQ_ID);
	}
}

static void md_gpio_config(unsigned int boot_mode_case)
{
	if ((boot_mode_case == MD2_ONLY) || (boot_mode_case == MD1_MD2)) {
		dprintf(CRITICAL, "set C2K GPIO\n");
		md_gpio_set(GPIO35, GPIO_MODE_06, GPIO_DIR_IN, GPIO_OUT_ZERO, GPIO_PULL_ENABLE, GPIO_PULL_DOWN, GPIO_SMT_ENABLE);
		md_gpio_set(GPIO32, GPIO_MODE_06, GPIO_DIR_IN, GPIO_OUT_ZERO, GPIO_PULL_ENABLE, GPIO_PULL_DOWN, GPIO_SMT_ENABLE);
		md_gpio_set(GPIO33, GPIO_MODE_06, GPIO_DIR_IN, GPIO_OUT_ZERO, GPIO_PULL_ENABLE, GPIO_PULL_UP, GPIO_SMT_DISABLE);
		md_gpio_set(GPIO37, GPIO_MODE_06, GPIO_DIR_IN, GPIO_OUT_ZERO, GPIO_PULL_ENABLE, GPIO_PULL_UP, GPIO_SMT_DISABLE);
		md_gpio_set(GPIO34, GPIO_MODE_06, GPIO_DIR_OUT, GPIO_OUT_ZERO, GPIO_PULL_UNSUPPORTED, GPIO_PULL_DOWN, GPIO_SMT_UNSUPPORTED);
		md_gpio_set(GPIO36, GPIO_MODE_06, GPIO_DIR_OUT, GPIO_OUT_ZERO, GPIO_PULL_UNSUPPORTED, GPIO_PULL_DOWN, GPIO_SMT_UNSUPPORTED);
		md_gpio_set(GPIO26, GPIO_MODE_04, GPIO_DIR_IN, GPIO_OUT_ZERO, GPIO_PULL_ENABLE, GPIO_PULL_DOWN, GPIO_SMT_ENABLE);
		md_gpio_set(GPIO27, GPIO_MODE_04, GPIO_DIR_IN, GPIO_OUT_ZERO, GPIO_PULL_ENABLE, GPIO_PULL_UP, GPIO_SMT_DISABLE);
		md_gpio_set(GPIO28, GPIO_MODE_04, GPIO_DIR_IN, GPIO_OUT_ZERO, GPIO_PULL_ENABLE, GPIO_PULL_UP, GPIO_SMT_DISABLE);
		md_gpio_set(GPIO25, GPIO_MODE_04, GPIO_DIR_OUT, GPIO_OUT_ZERO, GPIO_PULL_UNSUPPORTED, GPIO_PULL_DOWN, GPIO_SMT_UNSUPPORTED);
		md_gpio_set(GPIO29, GPIO_MODE_04, GPIO_DIR_OUT, GPIO_OUT_ZERO, GPIO_PULL_UNSUPPORTED, GPIO_PULL_DOWN, GPIO_SMT_UNSUPPORTED);
	}
	switch (boot_mode_case) {
		case MD1_ONLY:
#if 0 // same as dws initial setting
			//SIM1=> MD1 SIM1IF
			mt_set_gpio_mode(SIM1_SCLK_GPIO_ID, GPIO_MODE_01);
			mt_set_gpio_mode(SIM1_SRST_GPIO_ID, GPIO_MODE_01);
			mt_set_gpio_mode(SIM1_SIO_GPIO_ID, GPIO_MODE_01);
			mt_set_gpio_mode(SIM1_HOT_PLUG_GPIO_ID, GPIO_MODE_01);
			//SIM2=> MD1 SIM2IF
			mt_set_gpio_mode(SIM2_SCLK_GPIO_ID, GPIO_MODE_01);
			mt_set_gpio_mode(SIM2_SRST_GPIO_ID, GPIO_MODE_01);
			mt_set_gpio_mode(SIM2_SIO_GPIO_ID, GPIO_MODE_01);
			mt_set_gpio_mode(SIM2_HOT_PLUG_GPIO_ID, GPIO_MODE_01);
#endif
			break;
		case MD2_ONLY:
			//SIM1=> MD2 UIM0IF
			mt_set_gpio_mode(SIM1_SCLK_GPIO_ID, GPIO_MODE_03);
			mt_set_gpio_mode(SIM1_SRST_GPIO_ID, GPIO_MODE_03);
			mt_set_gpio_mode(SIM1_SIO_GPIO_ID, GPIO_MODE_03);
			mt_set_gpio_mode(SIM1_HOT_PLUG_GPIO_ID, GPIO_MODE_01);
			//SIM2=> MD2 UIM1IF
			mt_set_gpio_mode(SIM2_SCLK_GPIO_ID, GPIO_MODE_03);
			mt_set_gpio_mode(SIM2_SRST_GPIO_ID, GPIO_MODE_03);
			mt_set_gpio_mode(SIM2_SIO_GPIO_ID, GPIO_MODE_03);
			mt_set_gpio_mode(SIM2_HOT_PLUG_GPIO_ID, GPIO_MODE_01);
			break;
		case MD1_MD2:
			//SIM1=> MD1 SIM1IF
			mt_set_gpio_mode(SIM1_SCLK_GPIO_ID, GPIO_MODE_01);
			mt_set_gpio_mode(SIM1_SRST_GPIO_ID, GPIO_MODE_01);
			mt_set_gpio_mode(SIM1_SIO_GPIO_ID, GPIO_MODE_01);
			mt_set_gpio_mode(SIM1_HOT_PLUG_GPIO_ID, GPIO_MODE_01);
			//SIM2=> MD2 UIM0IF
			mt_set_gpio_mode(SIM2_SCLK_GPIO_ID, GPIO_MODE_03);
			mt_set_gpio_mode(SIM2_SRST_GPIO_ID, GPIO_MODE_03);
			mt_set_gpio_mode(SIM2_SIO_GPIO_ID, GPIO_MODE_03);
			mt_set_gpio_mode(SIM2_HOT_PLUG_GPIO_ID, GPIO_MODE_01); // FIXME: UIM1_HOT_PLUG
			break;
		default:
			break;
	}

	md_gpio_get(SIM1_SCLK_GPIO_ID, "sclk");
	md_gpio_get(SIM1_SRST_GPIO_ID, "srst");
	md_gpio_get(SIM1_SIO_GPIO_ID, "sio");
	md_gpio_get(SIM1_HOT_PLUG_GPIO_ID, "hp");
	md_gpio_get(SIM2_SCLK_GPIO_ID, "sclk2");
	md_gpio_get(SIM2_SRST_GPIO_ID, "srst2");
	md_gpio_get(SIM2_SIO_GPIO_ID, "sio2");
	md_gpio_get(SIM2_HOT_PLUG_GPIO_ID, "hp2");
}
#endif	//#ifndef CTP_ENV

static void md_gpio_get(GPIO_PIN pin, char *tag)
{
	pin |= GPIO_MAGIC;
	dprintf(CRITICAL, "GPIO(0x%X)(%s): mode=%d,dir=%d,in=%d,out=%d,pull_en=%d,pull_sel=%d,smt=%d\n",
	        pin, tag,
	        mt_get_gpio_mode(pin),
	        mt_get_gpio_dir(pin),
	        mt_get_gpio_in(pin),
	        mt_get_gpio_out(pin),
	        mt_get_gpio_pull_enable(pin),
	        mt_get_gpio_pull_select(pin),
	        mt_get_gpio_smt(pin));
}

static void md_gpio_set(GPIO_PIN pin, GPIO_MODE mode, GPIO_DIR dir, GPIO_OUT out, GPIO_PULL_EN pull_en, GPIO_PULL pull, GPIO_SMT smt)
{
	pin |= GPIO_MAGIC;
	mt_set_gpio_mode(pin, mode);
	if (dir != GPIO_DIR_UNSUPPORTED)
		mt_set_gpio_dir(pin, dir);

	if (dir == GPIO_DIR_OUT) {
		mt_set_gpio_out(pin, out);
	}
	if (dir == GPIO_DIR_IN) {
		mt_set_gpio_smt(pin, smt);
	}
	if (pull_en != GPIO_PULL_EN_UNSUPPORTED) {
		mt_set_gpio_pull_enable(pin, pull_en);
		mt_set_gpio_pull_select(pin, pull);
	}
	md_gpio_get(pin, "-");
}

void md_uart_config(int type_id, int boot_mode)
{
	switch (type_id) {
		case AP_ONLY: // for AP only
			dprintf(CRITICAL, "md_uart_config:%d, UART1->AP_0, UART2->MD1_0, UART3->C2K_0, UART4->MD1_1\n", type_id);
			// same as dws initial setting
			mt_set_gpio_mode(UART1_TX_GPIO_ID, GPIO_MODE_01);
			mt_set_gpio_mode(UART1_RX_GPIO_ID, GPIO_MODE_01);
			mt_set_gpio_mode(UART2_TX_GPIO_ID, GPIO_MODE_03);
			mt_set_gpio_mode(UART2_RX_GPIO_ID, GPIO_MODE_03);
			mt_set_gpio_mode(UART3_TX_GPIO_ID, GPIO_MODE_06);
			mt_set_gpio_mode(UART3_RX_GPIO_ID, GPIO_MODE_06);
			mt_set_gpio_mode(UART4_TX_GPIO_ID, GPIO_MODE_02);
			mt_set_gpio_mode(UART4_RX_GPIO_ID, GPIO_MODE_02);
			break;
		case MD1_ONLY: // for AP & MD1
		case MD2_ONLY: // for AP & C2K
		case MD1_MD2: // for both MD1 and C2K
			if (boot_mode) {
				dprintf(CRITICAL, "md_uart_config:%d, UART1->MD1_0, UART2->N/A, UART3->C2K_0, UART4->N/A\n", type_id);
				mt_set_gpio_mode(UART1_TX_GPIO_ID, GPIO_MODE_03);
				mt_set_gpio_mode(UART1_RX_GPIO_ID, GPIO_MODE_03);
				mt_set_gpio_mode(UART2_TX_GPIO_ID, GPIO_MODE_00);
				mt_set_gpio_mode(UART2_RX_GPIO_ID, GPIO_MODE_00);
				mt_set_gpio_mode(UART3_TX_GPIO_ID, GPIO_MODE_06);
				mt_set_gpio_mode(UART3_RX_GPIO_ID, GPIO_MODE_06);
				mt_set_gpio_mode(UART4_TX_GPIO_ID, GPIO_MODE_00);
				mt_set_gpio_mode(UART4_RX_GPIO_ID, GPIO_MODE_00);
			}
			break;
		default:
			break;
	}
}

void bus_protection_en(int md_id)
{
#define PROTECTION_BITMASK 0x002F0000 // bit 16,17,18,19,21

	if (md_id == MD_SYS1) {
		/* enable protection for MD1 */
		dprintf(CRITICAL, "enable protection for md\n");
		DRV_WriteReg32(MD1_BUS_PROTECT_SET, PROTECTION_BITMASK);
		/* poll protection ready */
		dprintf(CRITICAL, "wait protection ....\n");
		while ((DRV_Reg32(MD1_BUS_PROTECT_STA)&PROTECTION_BITMASK) != PROTECTION_BITMASK) {
			dprintf(CRITICAL, "0x%x\n", DRV_Reg32(MD1_BUS_PROTECT_STA));
		}
		dprintf(CRITICAL, "protection enable done\n");
		return;
	}
}

void bus_protection_diable(int md_id)
{
	if (md_id == MD_SYS1) {
		/* enable protection for MD1 */
		dprintf(CRITICAL, "disable protection for md\n");
		DRV_WriteReg32(MD1_BUS_PROTECT_CLR, PROTECTION_BITMASK);
		/* poll protection ready */
		dprintf(CRITICAL, "wait protection disable....\n");
		while ((DRV_Reg32(MD1_BUS_PROTECT_STA)&PROTECTION_BITMASK) != 0x00000000) {
			dprintf(CRITICAL, "0x%x\n", DRV_Reg32(MD1_BUS_PROTECT_STA));
		}
		dprintf(CRITICAL, "protection disable done\n");
		return;
	}
}

static void reset_ccirq_reg(void)
{
	int i;
	unsigned int ccirq_base[2];

	dprintf(CRITICAL, "reset CCIRQ\n");
	ccirq_base[0] = MD1_C2K_CCIRQ_BASE;
	ccirq_base[1] = C2K_MD1_CCIRQ_BASE;

	for (i = 0; i < 2; i++) {
		ccci_write32(ccirq_base[i], 0x4, 0xA00000FF);
		ccci_write32(ccirq_base[i], 0xC, 0xA00000FF);

		ccci_write32(ccirq_base[i], 0x40, 0x0);
		ccci_write32(ccirq_base[i], 0x44, 0x0);
		ccci_write32(ccirq_base[i], 0x48, 0x0);
		ccci_write32(ccirq_base[i], 0x4C, 0x0);
	}

}

void pmic_init_sequence(void)
{
	dprintf(CRITICAL, "pmic_init_sequence skipped!\n");
}

static void md1_pmic_setting(void)
{
#if 0
	/*---VMD1, VMODEM, VSRAM_VMD ENABLE---*/
	pmic_set_register_value(PMIC_RG_BUCK_VMD1_EN, 1);
	pmic_set_register_value(PMIC_RG_BUCK_VMODEM_EN, 1);
	pmic_set_register_value(PMIC_RG_VSRAM_VMD_SW_EN, 1);
	udelay(220);

	if (!pmic_get_register_value(PMIC_DA_QI_VMD1_EN) ||
	        !pmic_get_register_value(PMIC_DA_QI_VMODEM_EN) ||
	        !pmic_get_register_value(PMIC_DA_QI_VSRAM_VMD_EN))
		dprintf(CRITICAL, "[vmd1_pmic_setting_on] VMD1 = %d, VMODEM = %d, VSRAM_VMD = %d\n",
		        pmic_get_register_value(PMIC_DA_QI_VMD1_EN),
		        pmic_get_register_value(PMIC_DA_QI_VMODEM_EN),
		        pmic_get_register_value(PMIC_DA_QI_VSRAM_VMD_EN));

	/*---VMD1, VMODEM, VSRAM_VMD Voltage Select---*/
	/*--0x40 (0x40*0.00625+0.4 =0.8V)--*/
	pmic_set_register_value(PMIC_RG_BUCK_VMD1_VOSEL, 0x40);
	/*--0x40 (0x40*0.00625+0.4 =0.8V)--*/
	pmic_set_register_value(PMIC_RG_BUCK_VMODEM_VOSEL, 0x40);
	/*--0x50 (0x50*0.00625+0.4 =0.9V)--*/
	pmic_set_register_value(PMIC_RG_VSRAM_VMD_VOSEL, 0x50);

	dprintf(CRITICAL, "md1_pmic_setting done!\n");
#else
	// assume set in preloader
	dprintf(CRITICAL, "md1_pmic_setting skipped!\n");
#endif
}

int md_common_setting()
{
	unsigned int reg_value;

	// MD srcclkena setting: [7:4]=4'h0110, [3:0]=4'h1101
	reg_value = ccci_read32(INFRACFG_AO_BASE, INFRA_MISC2);
	if (img_load_flag & (1<<MD_SYS3)) {
		reg_value &= ~(0x92);
		reg_value |= 0x29;
	} else {
		reg_value &= ~(0xFF);
		reg_value |= 0x29;
	}
	ccci_write32(INFRACFG_AO_BASE, INFRA_MISC2, reg_value);
	dprintf(CRITICAL, "MD srcclkena setting:0x%x\n", ccci_read32(INFRACFG_AO_BASE, INFRA_MISC2));

	dprintf(CRITICAL, "Write: 0->0x11840010, 0xFFFFFFFF->0x1184000C");
	// mask msdc irq
	ccci_write32(0x11840010, 0, 0);
	// clear MSDC0 interrupt as it's also connected to MD side
	ccci_write32(0x1184000C, 0, 0xFFFFFFFF);

	dprintf(CRITICAL, "Readout: reg_0x11840010=0x%x,  reg_0x1184000C=0x%x\n", ccci_read32(0x11840010, 0), ccci_read32(0x1184000C, 0));

	pmic_init_sequence();

	return 0;
}

void md_common_pll_init(void)
{
	static int initialized = 0;

	/* MD1 and C2K share MDM PLL */
	if (!initialized)
		initialized = 1;
	else
		return;

	// initial CLKSQ_LPF
	ccci_write32(APMIXED_BASE, AP_PLL_CON1, ccci_read32(APMIXED_BASE, AP_PLL_CON1) | (0x1 << 4));
	udelay(100);
}

void md1_pll_init(void)
{
	md_common_pll_init();

    *REG_MDTOP_PLLMIXED_DCXO_PLL_SETTLE_CTL = 0x02020E8D; //dcxo settle time with (140+1)T 32K	

    *REG_MDTOP_PLLMIXED_PLL_DLY_CTL0 = 0x0019000D;      // Change ISO <-> EN delay to 0x19 count
    *REG_MDTOP_PLLMIXED_ICCPLL_CTL0 = 0x801EEC4F;       // 268MHz * 3*4 = 3216MHz, dvfs driver use
    *REG_MDTOP_PLLMIXED_MDINFRAPLL_CTL0 = 0x801EC4EC;   // 200MHz * 16
    *REG_MDTOP_PLLMIXED_DFEPLL_CTL0 = 0x801BB13B;       // 360MHz * 8
    *REG_MDTOP_PLLMIXED_IMCPLL_CTL0 = 0x801FD89D;       // 552MHz * 6
    *REG_MDTOP_PLLMIXED_MDMCUPLL_CTL0 = 0x80213B14;     // 964MHz * 4
 /*----------------------------------------------------------------------------------------
    Above configuration PLL clock
 ----------------------------------------------------------------------------------------*/

    /* Original method: Polling until MDMCUPLL complete frequency adjustment */
    //delay 4T 26M for safe polling between adjust freq to sdmpll_ctl state machine start
    //delay_26M(4);
    //while ((*REG_MDTOP_PLLMIXED_MDMCUPLL_STS >> 14) & 0x1){};
    /* Polling until MDMCUPLL complete frequency adjustment */
    while ( ((*REG_MDTOP_PLLMIXED_MDMCUPLL_STS >> 16) & 0x7FFF) != (*REG_MDTOP_PLLMIXED_MDMCUPLL_CTL0 >> 7) ){};

    /* PLL to h/w mode, default PLL with SW off mode */
    *REG_MDTOP_PLLMIXED_PLL_ON_CTL = 0x0;

    /* Wait MD bus clock ready from flexckgen */
    while ((*REG_MDTOP_CLKSW_BUS_FLEXCKGEN_STS & 0x8000) != 0x8000){};

    /* switch MDMCU & MD BUS clock to PLL frequency */
    *REG_MDTOP_CLKSW_CLKSEL_CTL |= 0x3;

    /* Switch MDMCUBUS_CK to DIV2*/
    while ((*REG_MDSYS_CLKCTL_MDMCU_AO_CLK_CTL & 0x40000) != 0x40000){};
    *REG_MDSYS_CLKCTL_MDMCU_AO_CLK_CTL = ((*REG_MDSYS_CLKCTL_MDMCU_AO_CLK_CTL & 0xFFFFFF00) | 0x00000002);
    *REG_MDSYS_CLKCTL_MDMCU_AO_CLK_CTL |= 0x00000100;
    while ((*REG_MDSYS_CLKCTL_MDMCU_AO_CLK_CTL & 0x40000) != 0x40000){};

    /* switch MD DBG & TRACE & MODEM clock to PLL frequency */
    *REG_MDTOP_CLKSW_CLKSEL_CTL |= 0x150177FC;

    /* Turn off all SW clock request */
    *REG_MDTOP_CLKSW_CLKON_CTL = 0x0;

    /* Clear PLL ADJ RDY IRQ fired by initial period adjustment */
    *REG_MDTOP_PLLMIXED_PLL_HP_RDY_IRQ = 0xFFFF;

    /* Mask all PLL ADJ RDY IRQ */
    *REG_MDTOP_PLLMIXED_PLL_HP_RDY_IRQ_MASK = 0xFFFF;

    /* make a record that means MD pll has been initialized. */
    *MD_GLOBAL_CON_DUMMY = MD_PLL_MAGIC_NUM;

	dprintf(CRITICAL, "md1_pll_init done!\n");
}

static void md1_pcore_sram_turn_on(void)
{
	unsigned int i, j;
	for (i = 0; i < sizeof(sram_on_sequence) / sizeof(struct sram_cfg); i++) {
		dprintf(CRITICAL, "SRAM reg before %x=%x, %d->%d\n",
			MD_SRAM_PD_PSMCUSYS_BASE + sram_on_sequence[i].offset,
			ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, sram_on_sequence[i].offset),
			sram_on_sequence[i].start_bit, sram_on_sequence[i].end_bit);
		for (j = sram_on_sequence[i].start_bit; j <= sram_on_sequence[i].end_bit; j++) {
			ccci_write32(MD_SRAM_PD_PSMCUSYS_BASE, sram_on_sequence[i].offset,
				ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, sram_on_sequence[i].offset) & (~(0x1 << j)));
		}
		dprintf(CRITICAL, "SRAM reg after %x=%x\n",
			MD_SRAM_PD_PSMCUSYS_BASE + sram_on_sequence[i].offset,
			ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, sram_on_sequence[i].offset));
	}

	dprintf(CRITICAL, "md1_pcore_sram_turn_on done!\n");
}

static void md1_enable_mcu_clock(void)
{
	ccci_write32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG1,
		ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG1) | (0x1 << 20));
	ccci_write32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG2,
		ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG2) | (0x1 << 20));
	ccci_write32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG3,
		ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG3) & (~(0x1 << 0)));
 	ccci_write8(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG4, 0x87);
	ccci_write32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG5,
		ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG5) & (~(0x1 << 0)));

	dprintf(CRITICAL, "md1_enable_mcu_clock done! %x %x %x %x %x\n",
		ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG1),
		ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG2),
		ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG3),
		ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG4),
		ccci_read32(MD_SRAM_PD_PSMCUSYS_BASE, MD_MCU_CLOCK_REG5));
}

void md1_boot(int boot_mode)
{
	unsigned int reg_value;

	// step 1: configure modem related buck
	md1_pmic_setting();

	// step 2: Power on MTCMOS
	spm_mtcmos_ctrl_md1(STA_POWER_ON);
	dprintf(CRITICAL, "MD1 MTCMOS power on done!\n");

	// step 3: power on MD Pcore's SRAM sequentially
	md1_pcore_sram_turn_on();

	// step 4: enable MDMCU clock
	md1_enable_mcu_clock();

	// step 5: configure md_srclkena setting
	// INFRA_MISC2 already finished in md_common_setting()
	spm_write(POWERON_CONFIG_EN, 0x0B160001);
	spm_write(SPM_POWER_ON_VAL1, 0x00215830);
	dprintf(CRITICAL, "md_srclkena done!\n");

	// step 6: configure PLL setting
	md1_pll_init();

	// step 7: disabel MD WDT
#if !defined(ENABLE_MD_RESET_SPM) && !defined(ENABLE_MD_RESET_RGU)
	ccci_write32(MD_RGU_BASE, WDT_MD_MODE, WDT_MD_MODE_KEY); // disable MD WDT & MD AUX_WDT
#endif

	// step+: set META Register
	if (boot_mode) {
		reg_value = DRV_Reg32(MD1_META_FLAG);
		DRV_WriteReg32(MD1_META_FLAG, (reg_value | 0x1));
	}

	// step 8: switch md usb interrupt MUX to partial interrupt
	SETREG32(MD_USB_INTERRUPT_MUX, 0x1);
}

void c2k_pll_init()
{
	ccci_write16(C2KSYS_BASE, C2K_PLL_CON3, 0x8805);
	ccci_write16(C2KSYS_BASE, C2K_PLL_CON3, 0x0005);
	ccci_write16(C2KSYS_BASE, C2K_PLL_CON3, 0x0001);
	ccci_write16(C2KSYS_BASE, C2K_PLL_CON2, 0x0);
	ccci_write16(C2KSYS_BASE, C2K_PLLTD_CON0, 0x0010);

	ccci_write16(C2KSYS_BASE, C2K_CPPLL_CON0, ccci_read16(C2KSYS_BASE, C2K_CPPLL_CON0) | (0x1 << 15));

	udelay(30);

	ccci_write16(C2KSYS_BASE, C2K_CG_ARM_AMBA_CLKSEL, 0xC124);
	ccci_write16(C2KSYS_BASE, C2K_CLK_CTRL4, 0x8E43);
	ccci_write16(C2KSYS_BASE, C2K_CLK_CTRL9, 0xA207);
}

void c2k_boot(int boot_mode)
{
	unsigned int reg_value;

	md_common_pll_init();

	/* step 1: config C2K boot mode */
	/* step 1.1: let CBP boot from EMI: [10:8] = 3'b101 */
	reg_value = ccci_read32(INFRACFG_AO_BASE, C2K_CONFIG);
	reg_value &= (~(7 << 8));
	reg_value |= (5 << 8);
	ccci_write32(INFRACFG_AO_BASE, C2K_CONFIG, reg_value);
	/* step 1.2: make CS_DEBUGOUT readable: [12:11] = 2'b00 */
	ccci_write32(INFRACFG_AO_BASE, C2K_CONFIG,  ccci_read32(INFRACFG_AO_BASE, C2K_CONFIG) & (~(0x3 << 11)));
	/* step 1.3: C2K state matchine not wait md1src_ack: [0] = 1'b0 */
	ccci_write32(INFRACFG_AO_BASE, C2K_CONFIG,  ccci_read32(INFRACFG_AO_BASE, C2K_CONFIG) & (~(0x1 << 0)));
	/* step 1.4: C2K state matchine md1src_req: [3] = 1'b1 */
	ccci_write32(INFRACFG_AO_BASE, C2K_CONFIG,  ccci_read32(INFRACFG_AO_BASE, C2K_CONFIG) | (0x1 << 3));

	/* step+: start to turn on C2K */
#if 1
	spm_write(POWERON_CONFIG_EN, 0x0b160001);

	spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) | PWR_ON);
	spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) | PWR_ON_2ND);

	dprintf(CRITICAL, "C2K, wait PWR_SATUS and PWR_STATUS_2ND\n");
	while (((spm_read(PWR_STATUS) & C2K_PWR_STA_MASK) != C2K_PWR_STA_MASK) ||
	       ((spm_read(PWR_STATUS_2ND) & C2K_PWR_STA_MASK) != C2K_PWR_STA_MASK)
		); // waiting for power ready
	dprintf(CRITICAL, "wait PWR_SATUS and PWR_STATUS_2ND done\n");

	spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) & ~PWR_CLK_DIS);
	spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) & ~PWR_ISO);
	spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) | PWR_RST_B);
#else
	spm_mtcmos_ctrl_c2k(STA_POWER_ON);
	dprintf(CRITICAL, "C2K MTCMOS power on done!\n");
#endif

	/* step 2: config srcclkena selection mask: |= 0x44 */
	ccci_write32(INFRACFG_AO_BASE, INFRA_MISC2, ccci_read32(INFRACFG_AO_BASE, INFRA_MISC2) | 0x44);
	dprintf(CRITICAL, "INFRA_MISC2 = %x\n", ccci_read32(INFRACFG_AO_BASE, INFRA_MISC2));

	/* step 3: config ClkSQ resigeter */
	ccci_write32(APMIXED_BASE, AP_PLL_CON0, ccci_read32(APMIXED_BASE, AP_PLL_CON0) | (0x1 << 1));
	dprintf(CRITICAL, "AP_PLL_CON0 = 0x%x\n", ccci_read32(APMIXED_BASE, AP_PLL_CON0));

	/* step 3.1: */
	spm_write(SPM_POWER_ON_VAL1, 0x00215830);

	/* step 4: hold C2K ARM core */
	ccci_write32(INFRACFG_AO_BASE, C2K_CONFIG,  ccci_read32(INFRACFG_AO_BASE, C2K_CONFIG) | (0x1 << 1));
	dprintf(CRITICAL, "C2K_CONFIG = %x\n", ccci_read32(INFRACFG_AO_BASE, C2K_CONFIG));

	/* step 5: wake up C2K */
	/* step 5.1: switch MDPLL1(208M) Control to hw mode: [28] = 1'b0 */
	/*ccci_write32(APMIXED_BASE, MDPLL_CON3, ccci_read32(APMIXED_BASE, MDPLL_CON3) & 0xEFFFFFFF);*/
	/*dprintf(CRITICAL, "MDPLL1_CON3 = 0x%x\n", ccci_read32(APMIXED_BASE, MDPLL_CON3));*/
	/* step 5.2: release c2ksys_rstb */
	ccci_write32(TOPRGU_BASE, TOP_RGU_WDT_SWSYSRST,
			(ccci_read32(TOPRGU_BASE, TOP_RGU_WDT_SWSYSRST) | UNLOCK_KEY) & (~C2K_SYS));
	dprintf(CRITICAL, "[C2K] TOP_RGU_WDT_SWSYSRST = 0x%x\n",
			ccci_read32(TOPRGU_BASE, TOP_RGU_WDT_SWSYSRST));
	/* step 5.4: wakeup C2KSYS: [1] = 1'b1 */
	ccci_write32(INFRACFG_AO_BASE, C2K_SPM_CTRL, ccci_read32(INFRACFG_AO_BASE, C2K_SPM_CTRL) | (0x1 << 1));
	/* step 5.5: polling C2K_STATUS[1] is high - C2KSYS has enter idle state */
	dprintf(CRITICAL, "[C2K] C2K_STATUS before = 0x%x\n", ccci_read32(INFRACFG_AO_BASE, C2K_STATUS));
	while (!((ccci_read32(INFRACFG_AO_BASE, C2K_STATUS) >> 1) & 0x1))
		;
	dprintf(CRITICAL, "[C2K] C2K_STATUS after = 0x%x\n", ccci_read32(INFRACFG_AO_BASE, C2K_STATUS));
	/* step 5.6 */
	ccci_write32(INFRACFG_AO_BASE, C2K_SPM_CTRL, ccci_read32(INFRACFG_AO_BASE, C2K_SPM_CTRL) & (~(0x1 << 1)));
	dprintf(CRITICAL, "[C2K] C2K_SPM_CTRL = 0x%x, C2K_STATUS = 0x%x\n",
			ccci_read32(INFRACFG_AO_BASE, C2K_SPM_CTRL),
			ccci_read32(INFRACFG_AO_BASE, C2K_STATUS));
	ccci_write32(INFRACFG_AO_BASE, INFRA_TOPAXI_PROTECTEN_1_CLR,
			ccci_read32(INFRACFG_AO_BASE, INFRA_TOPAXI_PROTECTEN_1_SET) & 0x01C00000);
	/* step 5.7: waiting for C2KSYS bus ready for operation */
	while (ccci_read32(C2KSYS_BASE, C2K_CGBR1) != 0xFE8)
		;
	dprintf(CRITICAL, "[C2K] C2K_CGBR1 = 0x%x\n", ccci_read32(C2KSYS_BASE, C2K_CGBR1));

	/* step 6: initialize C2K PLL */
	c2k_pll_init();
}

static void config_md_boot_env(int md_id, int boot_mode)
{
	switch (md_id) {
		case MD_SYS1:
			md1_boot(boot_mode);
			break;
		case MD_SYS3:
			c2k_boot(boot_mode);
			break;
		default:
			break;
	}
}

static void let_md_go(int md_id)
{
	switch (md_id) {
		case MD_SYS1:
			/* step 8: trigger modem SW to run */
#if 1
			ccci_write32(MD1_BOOT_VECTOR_EN, 0, 1);
#else
			dprintf(CRITICAL, "do not let MD1 go\n");
#endif
			break;
		case MD_SYS3:
#ifdef FOR_C2K_SLT
			ccci_write32(0x10212600, 0, ccci_read32(0x10212600, 0) | (1 << 29) | (1 << 25) | (1 << 16));
			dprintf(CRITICAL, "0x10212600: 0x%x\n", ccci_read32(0x10212600, 0));
#endif
			/* step 7: release C2K ARM core */
			ccci_write32(INFRACFG_AO_BASE, C2K_CONFIG,
				ccci_read32(INFRACFG_AO_BASE, C2K_CONFIG) & (~(0x1 << 1)));
			break;
		default:
			break;
	}
}

static void ap_dying_wish()
{
#ifdef FOR_C2K_SLT
#define HWD_MDCCIF_CHDATA (0x10701100)
#define SLT_RESULT_INTERVAL (100)
#define SLT_RESULT_TIMEOUT (30000)

	int step = 0;
	int result = 1;
	int count = 0;

	while (step < 2 && count < (SLT_RESULT_TIMEOUT / SLT_RESULT_INTERVAL)) {
		if (step == 0 && 0xA0A0A0A0 == ccci_read32(HWD_MDCCIF_CHDATA, 0))
			step = 1;
		if (step == 1 && 0x85858585 == ccci_read32(HWD_MDCCIF_CHDATA, 0)) {
			step = 2;
			result = ccci_read32(HWD_MDCCIF_CHDATA, 4) ? 1 : 0;
		}
		mdelay(SLT_RESULT_INTERVAL);
		count++;
	}
	dprintf(CRITICAL, "C2K MD SLT test %s, step = %d, result = %d!\n", result ? "Fail" : "Pass", step, result);
	if (result)
		dprintf(CRITICAL, "\n\n******MediaTek Device Test for MT6758 Built C2K_V1.0********\n\n"
				"================= SLT Summary ==================\n"
				"SLT_BIN2\n"
				"================================================\n\n");
	else
		dprintf(CRITICAL, "\n\n******MediaTek Device Test for MT6758 Built C2K_V1.0********\n\n"
				"================= SLT Summary ==================\n"
				"SLT_BIN1\n"
				"================================================\n\n");
#endif
}

void md_wdt_irq_handler(unsigned int irq)
{

	spm_mtcmos_ctrl_aud(STA_POWER_DOWN);
	dprintf(CRITICAL, "turn off audio.\n");
#if defined(ENABLE_MD_RESET_SPM) || defined(ENABLE_MD_RESET_RGU)
	unsigned int reg_value = 0;
	unsigned int cnt = ccci_read32(TOPRGU_BASE, TOP_RGU_WDT_NONRST_REG);

	// update counter
	ccci_write32(TOPRGU_BASE, TOP_RGU_WDT_NONRST_REG, cnt + 1);
	// reset UART config
	md_uart_config(AP_ONLY, 0);
	dprintf(CRITICAL, "\n\n\n\nCurrent wdt cnt:%d\n", cnt + 1);

	if (irq == MT_MD_WDT1_IRQ_ID) {
#ifdef ENABLE_MD_RESET_SPM
		dprintf(CRITICAL, "MD1 power off\n");
		spm_mtcmos_ctrl_md1(STA_POWER_DOWN);
		mdelay(5);
#endif
#ifdef ENABLE_MD_RESET_RGU
		dprintf(CRITICAL, "MD1 reset\n");

		bus_protection_en(0);
		ccci_write32(TOPRGU_BASE, TOP_RGU_WDT_SWSYSRST,
			(ccci_read32(TOPRGU_BASE, TOP_RGU_WDT_SWSYSRST) | UNLOCK_KEY) | MD1_SYS);
		mdelay(5);
		ccci_write32(TOPRGU_BASE, TOP_RGU_WDT_SWSYSRST,
			(ccci_read32(TOPRGU_BASE, TOP_RGU_WDT_SWSYSRST) | UNLOCK_KEY) & (~MD1_SYS));
		bus_protection_diable(0);
#endif
		config_md_boot_env(MD_SYS1, 0);
		let_md_go(MD_SYS1);
	}
	if (irq == MT_MD_WDT2_IRQ_ID) {
#ifdef ENABLE_MD_RESET_SPM
		dprintf(CRITICAL, "MD2 power off\n");
		spm_mtcmos_ctrl_c2k(STA_POWER_DOWN);
		mdelay(5);
		reset_ccirq_reg();
#endif
#ifdef ENABLE_MD_RESET_RGU
		dprintf(CRITICAL, "MD2 reset\n");
		ccci_write32(TOPRGU_BASE, TOP_RGU_WDT_SWSYSRST,
			(ccci_read32(TOPRGU_BASE, TOP_RGU_WDT_SWSYSRST) | UNLOCK_KEY) | C2K_SYS);
		mdelay(5);
		ccci_write32(TOPRGU_BASE, TOP_RGU_WDT_SWSYSRST,
			(ccci_read32(TOPRGU_BASE, TOP_RGU_WDT_SWSYSRST) | UNLOCK_KEY) & (~C2K_SYS));
#endif
		config_md_boot_env(MD_SYS3, 0);
		let_md_go(MD_SYS3);
	}

#if 1
	dprintf(CRITICAL, "Config UART after MD WDT! %d\n", cnt+1);
	if ((img_load_flag&((1 << MD_SYS1) | (1 << MD_SYS3))) == ((1 << MD_SYS1) | (1 << MD_SYS3))) {
		md_uart_config(MD1_MD2, 0);
	} else if (img_load_flag & (1 << MD_SYS1)) {
		md_uart_config(MD1_ONLY, 0);
	} else if (img_load_flag & (1 << MD_SYS3)) {
		md_uart_config(MD2_ONLY, 0);
	}
#endif
#else
	md_uart_config(AP_ONLY, 0);
	dprintf(CRITICAL, "Get MD WDT irq, STA:%x!!\n", ccci_read32(MD_RGU_BASE, 0xC));
#ifdef IGNORE_MD_WDT
	dprintf(CRITICAL, "ignore MD WDT\n");
#else
	dprintf(CRITICAL, "whole system reboot\n");
	ccci_write32(TOPRGU_BASE, TOP_RGU_LATCH_CONTROL, 0x95000000);
	ccci_write32(TOPRGU_BASE, TOP_RGU_WDT_MODE, 0x22000004);
	ccci_write32(TOPRGU_BASE, TOP_RGU_WDT_SWRST, 0x1209);
	while (1);
#endif
#endif
	spm_mtcmos_ctrl_aud(STA_POWER_ON);
	dprintf(CRITICAL, "turn on audio.\n");
}

int dummy_ap_irq_helper(unsigned int irq)
{
	switch (irq) {
	case MT_MD_WDT1_IRQ_ID:
		if (img_load_flag &(1 << MD_SYS1)) {
#ifndef IGNORE_MD1_WDT
			md_wdt_irq_handler(MT_MD_WDT1_IRQ_ID);
#else
			dprintf(CRITICAL, "ignore MD1 WDT\n");
#endif
		}
		return 1;
	case MT_MD_WDT2_IRQ_ID:
		if (img_load_flag &(1 << MD_SYS3)) {
#ifndef IGNORE_MD2_WDT
			md_wdt_irq_handler(MT_MD_WDT2_IRQ_ID);
#else
			dprintf(CRITICAL, "ignore MD2 WDT\n");
#endif
		}
		return 1;
	default:
		break;
}

	return 0;
}

void dummy_ap_boot_up_md(int md_ld_flag)
{
	int boot_mode = 0;
	int i;
	int ret;
	img_load_flag = (unsigned int)md_ld_flag;

	// reinit UART, overwrite DWS setting
	md_uart_config(AP_ONLY, 0);

	// Disable AP WDT
	*(volatile unsigned int *)(TOPRGU_BASE) = 0x22000000;

	dprintf(CRITICAL, "Welcome to use dummy AP!\n");

	dprintf(CRITICAL, "load flag for dummy AP: %x\n", img_load_flag);
#ifndef	CTP_ENV
	if (img_load_flag == 0) {
		dprintf(CRITICAL, "no MD loaded for dummy AP\n");
		ret = get_md_err_from_lk_info(MD_SYS1);
		dprintf(CRITICAL, "hint for MD1 errno: %x, %s\n", ret, ld_md_errno_to_str(-ret));
		ret = get_md_err_from_lk_info(MD_SYS3);
		dprintf(CRITICAL, "hint for MD3 errno: %x, %s\n", ret, ld_md_errno_to_str(-ret));
		dprintf(CRITICAL, "stop.....\n");
		while (1);
	}

	if (img_load_flag & (1 << MD_SYS1)) {
		dprintf(CRITICAL, "MD1 loaded");
		ret = get_md_err_from_lk_info(MD_SYS1);
		if (ret < 0) {
			dprintf(CRITICAL, "MD1 load image has error, errno:%s", ld_md_errno_to_str(-ret));
			while (1);
		}
	}
	if (img_load_flag & (1 << MD_SYS3)) {
		dprintf(CRITICAL, "MD3 loaded");
		ret = get_md_err_from_lk_info(MD_SYS3);
		if (ret < 0) {
			dprintf(CRITICAL, "MD3 load image has error, errno:%s", ld_md_errno_to_str(-ret));
			while (1);
		}
	}

	// 1, Setup special GPIO request (RF/SIM/UART ... etc)
	dprintf(CRITICAL, "Configure GPIO!\n");
	if ((img_load_flag&((1 << MD_SYS1) | (1 << MD_SYS3))) == ((1 << MD_SYS1) | (1 << MD_SYS3))) {
		md_gpio_config(MD1_MD2);
	} else if (img_load_flag & (1 << MD_SYS1)) {
		md_gpio_config(MD1_ONLY);
	} else if (img_load_flag & (1 << MD_SYS3)) {
		md_gpio_config(MD2_ONLY);
	}

	// 2, Check boot Mode
	boot_mode = meta_detection();

	dprintf(CRITICAL, "Get boot mode is %d\n", boot_mode);

	// 3, MD WDT ISR init
	dprintf(CRITICAL, "Init MD WDT\n");
	md_wdt_init();
#endif

	// 4. Common setting for all MD
	md_common_setting();

	// 5. Setup per-MD env before boot up MD
	for (i=0; i<3; i++) {
		if (img_load_flag & (1 << i)) {
			dprintf(CRITICAL, "MD%d Enabled\n", i+1);
			config_md_boot_env(i, boot_mode);
		}
	}

	// 6. Switch UART
	dprintf(CRITICAL, "Switch UART!\n");
	if ((img_load_flag&((1 << MD_SYS1) | (1 << MD_SYS3))) == ((1 << MD_SYS1) | (1 << MD_SYS3))) {
		md_uart_config(MD1_MD2, boot_mode);
	} else if (img_load_flag & (1 << MD_SYS1)) {
		md_uart_config(MD1_ONLY, boot_mode);
	} else if (img_load_flag & (1 << MD_SYS3)) {
		md_uart_config(MD2_ONLY, boot_mode);
	}

	for (i=0; i<3; i++) {
		if (img_load_flag & (1 << i)) {
			dprintf(CRITICAL, "Trigger MD%d run\n", i+1);
			let_md_go(i);
		}
	}

	spm_mtcmos_ctrl_aud(STA_POWER_ON);
	dprintf(CRITICAL, "turn on audio.\n");
#ifndef CTP_ENV
	ap_dying_wish();
	dprintf(CRITICAL, "enter while(1), ^O^!!!!!!!!!\n");
	while (1);
#endif
}
