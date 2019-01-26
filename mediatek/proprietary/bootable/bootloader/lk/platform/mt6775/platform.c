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
#include <platform/mtk_smi.h>
#include <platform/mt_rtc.h>
#include <platform/mt_leds.h>
#include <platform/upmu_common.h>
#include <platform/mtk_wdt.h>
#include <platform/disp_drv_platform.h>
#include <platform/verified_boot.h>
#include <libfdt.h>
#include <mt_boot.h>                // for bldr_load_dtb()
#include <fpga_boot_argument.h>
#include <profiling.h>
#include <assert.h>
#include <platform/mt_gpt.h>		// for get_timer()
#include <verified_boot.h>
#include "memory_layout.h"

#define DEVAPC_TURN_ON 1

#if defined(MTK_VPU_SUPPORT)
#include <platform/mt_vpu.h>
#endif

#ifdef MTK_AB_OTA_UPDATER
#include "bootctrl.h"
#endif

#if defined(MTK_SECURITY_SW_SUPPORT)
#include "oemkey.h"
#endif

#ifdef MTK_SECURITY_SW_SUPPORT
extern u8 g_oemkey[OEM_PUBK_SZ];
#endif

#ifdef LK_DL_CHECK
/*block if check dl fail*/
#undef LK_DL_CHECK_BLOCK_LEVEL
#endif

extern void platform_early_init_timer();
extern void jump_da(u32 addr, u32 arg1, u32 arg2);
extern int i2c_hw_init(void);
extern int mboot_common_load_logo(unsigned long logo_addr, char* filename);
extern void mtk_wdt_disable(void);
extern void platform_deinit_interrupts(void);
extern int mmc_get_dl_info(void);
extern int mmc_legacy_init(int);
extern void platform_clear_all_on_mux(void);
#ifdef MTK_AEE_PLATFORM_DEBUG_SUPPORT
extern void latch_lastbus_init(void);
#endif

#ifdef MTK_BATLOWV_NO_PANEL_ON_EARLY
extern kal_bool is_low_battery(kal_int32 val);
extern int hw_charging_get_charger_type(void);
#endif

#ifdef MTK_LK_REGISTER_WDT
extern void lk_register_wdt_callback(void);
#endif

void platform_uninit(void);
void config_shared_SRAM_size(void);
extern int dev_info_nr_cpu(void);
extern void mt_pll_turn_off(void);

struct mmu_initial_mapping mmu_initial_mappings[] = {

	{
		.phys = (uint64_t)0,
		.virt = (uint32_t)0,
		.size = 0x40000000,
		.flags = MMU_MEMORY_TYPE_STRONGLY_ORDERED | MMU_MEMORY_AP_P_RW_U_NA,
		.name = "mcusys"
	},
	{
		.phys = (uint64_t)MEMBASE,
		.virt = (uint32_t)MEMBASE,
		.size = ROUNDUP(MEMSIZE, SECTION_SIZE),
		.flags = MMU_MEMORY_TYPE_NORMAL_WRITE_BACK | MMU_MEMORY_AP_P_RW_U_NA,
		.name = "ram"
	},
	{
		.phys = (uint64_t)CFG_BOOTIMG_LOAD_ADDR,
		.virt = (uint32_t)CFG_BOOTIMG_LOAD_ADDR,
		.size = 256*MB,
		.flags = MMU_MEMORY_TYPE_NORMAL_WRITE_BACK | MMU_MEMORY_AP_P_RW_U_NA,
		.name = "bootimg"
	},
	{
		.phys = (uint64_t)SCRATCH_ADDR,
		.virt = (uint32_t)SCRATCH_ADDR,
		.size = SCRATCH_SIZE + 16*MB,
		.flags = MMU_MEMORY_TYPE_NORMAL_WRITE_BACK | MMU_MEMORY_AP_P_RW_U_NA,
		.name = "download"
	},
	/* null entry to terminate the list */
	{ 0 }
};

BOOT_ARGUMENT *g_boot_arg;
BOOT_ARGUMENT boot_addr;
int g_nr_bank;
BI_DRAM bi_dram[MAX_NR_BANK];
unsigned int g_fb_base;
unsigned int g_fb_size;
static int g_dram_init_ret;
static unsigned int bootarg_addr;
static unsigned int bootarg_size;
extern unsigned int logo_lk_t;
extern unsigned int boot_time;

int dram_init(void)
{
	int i;
	struct boot_tag *tags;

	/* Get parameters from pre-loader. Get as early as possible
	 * The address of BOOT_ARGUMENT_LOCATION will be used by Linux later
	 * So copy the parameters from BOOT_ARGUMENT_LOCATION to LK's memory region
	 */
	g_boot_arg = &boot_addr;
	bootarg_addr = (unsigned int *)BOOT_ARGUMENT_LOCATION;
	bootarg_size = 0;

	fpga_set_boot_argument((BOOT_ARGUMENT*)bootarg_addr);

	if (*(unsigned int *)BOOT_ARGUMENT_LOCATION == BOOT_ARGUMENT_MAGIC) {
		memcpy(g_boot_arg, (void*)BOOT_ARGUMENT_LOCATION, sizeof(BOOT_ARGUMENT));
		bootarg_size = sizeof(BOOT_ARGUMENT);
	} else {
		g_boot_arg->maggic_number = BOOT_ARGUMENT_MAGIC;
		for (tags = (void *)BOOT_ARGUMENT_LOCATION; tags->hdr.size; tags = boot_tag_next(tags)) {
			switch (tags->hdr.tag) {
				case BOOT_TAG_BOOT_REASON:
					g_boot_arg->boot_reason = tags->u.boot_reason.boot_reason;
					break;
				case BOOT_TAG_BOOT_MODE:
					g_boot_arg->boot_mode = tags->u.boot_mode.boot_mode;
					break;
				case BOOT_TAG_META_COM:
					g_boot_arg->meta_com_type = tags->u.meta_com.meta_com_type;
					g_boot_arg->meta_com_id = tags->u.meta_com.meta_com_id;
					g_boot_arg->meta_uart_port = tags->u.meta_com.meta_uart_port;
					break;
				case BOOT_TAG_LOG_COM:
					g_boot_arg->log_port = tags->u.log_com.log_port;
					g_boot_arg->log_baudrate = tags->u.log_com.log_baudrate;
					g_boot_arg->log_enable = tags->u.log_com.log_enable;
					g_boot_arg->log_dynamic_switch = tags->u.log_com.log_dynamic_switch;
					break;
				case BOOT_TAG_MEM:
					g_boot_arg->dram_rank_num = tags->u.mem.dram_rank_num;
					for (i = 0; i < tags->u.mem.dram_rank_num; i++) {
						g_boot_arg->dram_rank_size[i] = tags->u.mem.dram_rank_size[i];
					}
					g_boot_arg->mblock_info = tags->u.mem.mblock_info;
					g_boot_arg->orig_dram_info = tags->u.mem.orig_dram_info;
					g_boot_arg->lca_reserved_mem = tags->u.mem.lca_reserved_mem;
					g_boot_arg->tee_reserved_mem = tags->u.mem.tee_reserved_mem;
					break;
				case BOOT_TAG_MD_INFO:
					for (i = 0; i < 4; i++) {
						g_boot_arg->md_type[i] = tags->u.md_info.md_type[i];
					}
					break;
				case BOOT_TAG_BOOT_TIME:
					g_boot_arg->boot_time = tags->u.boot_time.boot_time;
					break;
				case BOOT_TAG_DA_INFO:
					memcpy(&g_boot_arg->da_info, &tags->u.da_info.da_info, sizeof(da_info_t));
					break;
				case BOOT_TAG_SEC_INFO:
					memcpy(&g_boot_arg->sec_limit, &tags->u.sec_info.sec_limit, sizeof(SEC_LIMIT));
					break;
				case BOOT_TAG_PART_NUM:
					g_boot_arg->part_num = tags->u.part_num.part_num;
					break;
				case BOOT_TAG_PART_INFO:
					g_boot_arg->part_info = tags->u.part_info.part_info;  /* only copy the pointer but the contains*/
					break;
				case BOOT_TAG_EFLAG:
					g_boot_arg->e_flag = tags->u.eflag.e_flag;
					break;
				case BOOT_TAG_DDR_RESERVE:
					g_boot_arg->ddr_reserve_enable = tags->u.ddr_reserve.ddr_reserve_enable;
					g_boot_arg->ddr_reserve_success = tags->u.ddr_reserve.ddr_reserve_success;
					g_boot_arg->ddr_reserve_ready = tags->u.ddr_reserve.ddr_reserve_ready;
					break;
				case BOOT_TAG_DRAM_BUF:
					g_boot_arg->dram_buf_size = tags->u.dram_buf.dram_buf_size;
					break;
				case BOOT_TAG_SRAM_INFO:
					g_boot_arg->non_secure_sram_addr = tags->u.sram_info.non_secure_sram_addr;
					g_boot_arg->non_secure_sram_size = tags->u.sram_info.non_secure_sram_size;
					break;
				case BOOT_TAG_PLAT_DBG_INFO:
					g_boot_arg->plat_dbg_info_max = tags->u.plat_dbg_info.info_max;
					if (g_boot_arg->plat_dbg_info_max > INFO_TYPE_MAX)
						g_boot_arg->plat_dbg_info_max = INFO_TYPE_MAX;
					for (i = 0; i < g_boot_arg->plat_dbg_info_max; i++) {
						g_boot_arg->plat_dbg_info[i].key = tags->u.plat_dbg_info.info[i].key;
						g_boot_arg->plat_dbg_info[i].base = tags->u.plat_dbg_info.info[i].base;
						g_boot_arg->plat_dbg_info[i].size = tags->u.plat_dbg_info.info[i].size;
					}
					break;
				case BOOT_TAG_PTP:
					memcpy(&g_boot_arg->ptp_volt_info, &tags->u.ptp_volt.ptp_volt_info, sizeof(ptp_info_t));
					break;
				case BOOT_TAG_EMI_INFO:
					memcpy(&(g_boot_arg->emi_info), &(tags->u.emi_info), sizeof(emi_info_t));
					break;
				case BOOT_TAG_IMGVER_INFO:
					g_boot_arg->pl_imgver_status = tags->u.imgver_info.pl_imgver_status;
					break;
				case BOOT_TAG_MAX_CPUS:
					g_boot_arg->max_cpus = tags->u.max_cpus.max_cpus;
					break;
				default:
					break;
			}
			bootarg_size += tags->hdr.size;
		}
	}
	fpga_copy_boot_argument(g_boot_arg);

	g_nr_bank = g_boot_arg->dram_rank_num;

	if (g_nr_bank == 0 || g_nr_bank > MAX_NR_BANK) {
		g_dram_init_ret = -1;
		//dprintf(CRITICAL, "[LK ERROR] DRAM bank number is not correct!!!");
		//while (1) ;
		return -1;
	}

	return 0;
}

unsigned int platform_get_bootarg_addr(void)
{
	return bootarg_addr;
}

unsigned int platform_get_bootarg_size(void)
{
	return bootarg_size;
}

/*******************************************************
 * Routine: memory_size
 * Description: return DRAM size to LCM driver
 ******************************************************/
u32 ddr_enable_4gb(void)
{
	u32 status;
#define INFRA_MISC      (INFRACFG_AO_BASE + 0x0F00)
#define INFRA_4GB_EN    (1 << 13)
#define PERI_MISC       (PERICFG_BASE + 0x0208)
#define PERI_4GB_EN     (1 << 15)

	status = ((DRV_Reg32(INFRA_MISC) & INFRA_4GB_EN) && (DRV_Reg32(PERI_MISC) & PERI_4GB_EN)) ? 1 : 0;
	dprintf(CRITICAL, "%s() status=%u\n", __func__, status);

	return status;
}

u64 physical_memory_size(void)
{
	int i;
	unsigned long long size = 0;

	for (i = 0; i < g_boot_arg->orig_dram_info.rank_num; i++) {
		size += g_boot_arg->orig_dram_info.rank_info[i].size;
	}

	return size;
}

u32 memory_size(void)
{
	unsigned long long size = physical_memory_size();

	while (((unsigned long long)DRAM_PHY_ADDR + size) > 0x100000000ULL) {
		size -= (unsigned long long)(1024*1024*1024);
	}

	return (unsigned int)size;
}

void sw_env()
{
#ifdef LK_DL_CHECK
#ifdef MTK_EMMC_SUPPORT
	int dl_status = 0;
	dl_status = mmc_get_dl_info();
	dprintf(INFO, "mt65xx_sw_env--dl_status: %d\n", dl_status);
	if (dl_status != 0) {
		video_printf("=> TOOL DL image Fail!\n");
		dprintf(CRITICAL, "TOOL DL image Fail\n");
#ifdef LK_DL_CHECK_BLOCK_LEVEL
		dprintf(CRITICAL, "uboot is blocking by dl info\n");
		while (1) ;
#endif
	}
#endif
#endif

#ifndef MACH_FPGA_NO_DISPLAY
#ifndef USER_BUILD
	switch (g_boot_mode) {
		case META_BOOT:
			video_printf(" => META MODE\n");
			break;
		case FACTORY_BOOT:
			video_printf(" => FACTORY MODE\n");
			break;
		case RECOVERY_BOOT:
			video_printf(" => RECOVERY MODE\n");
			break;
		case SW_REBOOT:
			//video_printf(" => SW RESET\n");
			break;
		case NORMAL_BOOT:
			//if(g_boot_arg->boot_reason != BR_RTC && get_env("hibboot") != NULL && atoi(get_env("hibboot")) == 1)
			if (get_env("hibboot") != NULL && atoi(get_env("hibboot")) == 1)
				video_printf(" => HIBERNATION BOOT\n");
			else
				video_printf(" => NORMAL BOOT\n");
			break;
		case ADVMETA_BOOT:
			video_printf(" => ADVANCED META MODE\n");
			break;
		case ATE_FACTORY_BOOT:
			video_printf(" => ATE FACTORY MODE\n");
			break;
#ifdef MTK_KERNEL_POWER_OFF_CHARGING
		case KERNEL_POWER_OFF_CHARGING_BOOT:
			video_printf(" => POWER OFF CHARGING MODE\n");
			break;
		case LOW_POWER_OFF_CHARGING_BOOT:
			video_printf(" => LOW POWER OFF CHARGING MODE\n");
			break;
#endif
		case ALARM_BOOT:
			video_printf(" => ALARM BOOT\n");
			break;
		case FASTBOOT:
			video_printf(" => FASTBOOT mode...\n");
			break;
		default:
			video_printf(" => UNKNOWN BOOT\n");
	}
	return;
#endif

#ifdef USER_BUILD
	/* it's ok to display META MODE since it already verfied by preloader */
	if (g_boot_mode == META_BOOT)
		video_printf(" => META MODE\n");
	if (g_boot_mode == FASTBOOT)
		video_printf(" => FASTBOOT mode...\n");
	return;
#endif
#endif
}

extern uint64_t ld_tt_l1[4];
void platform_init_mmu(void)
{
	unsigned long long addr;
	unsigned int vaddr;
	unsigned long long dram_size;

	/* configure available RAM banks */
	dram_init();
	/* Long-descriptor translation with lpae enable */
	arm_mmu_lpae_init();

	struct mmu_initial_mapping *m = mmu_initial_mappings;

	for (uint i = 0; i < countof(mmu_initial_mappings); i++, m++) {
		arch_mmu_map(m->phys, m->virt, m->flags, m->size);
	}

	arch_enable_mmu();  //enable mmu after setup page table to avoid cpu prefetch which may bring on emi violation
}


/******************************************************************************
******************************************************************************/
void init_storage(void)
{
	PROFILING_START("NAND/EMMC init");

#if defined(MTK_EMMC_SUPPORT)
	mmc_legacy_init(1);
#elif defined(MTK_UFS_BOOTING)
	ufs_lk_init();
#else
#ifndef MACH_FPGA
	nand_init();
	nand_driver_test();
#endif  // MACH_FPGA
#endif  // MTK_EMMC_SUPPORT

	PROFILING_END(); /* NAND/EMMC init */
}


/******************************************************************************
* FIXME:
* 1. Read RTC register, load "boot"/"recovery dtb according to register value.
* 2. If no RTC bit set, load "boot" dtb.
* 3. If no RTC bit set and no "boot" partition, load "recovery" dtb.
* 4. Consider to make use of (or integrate with) meta_detection(). (Optional)
******************************************************************************/
void load_device_tree(void)
{
	PROFILING_START("early load dtb");

#if defined(CFG_DTB_EARLY_LOADER_SUPPORT)
	if (bldr_load_dtb("boot") < 0)
		dprintf(CRITICAL, "Error: %s failed\n", __func__);
#endif

	PROFILING_END();
}


/******************************************************************************
******************************************************************************/
void platform_early_init(void)
{
	PROFILING_START("platform_early_init");
#ifdef MTK_LK_REGISTER_WDT
	lk_register_wdt_callback();
#endif

	platform_init_interrupts();

	platform_early_init_timer();

#ifndef MACH_FPGA
#ifndef USE_DTB_NO_DWS
	mt_gpio_set_default();
#endif
#endif

	/* initialize the uart */
	uart_init_early();

        PROFILING_START("Devinfo Init");
        init_devinfo_data();
        enable_MTK_test_mode();
        PROFILING_END();

	dprintf(SPEW, "bootarg_addr: 0x%x, bootarg_size: 0x%x\n", platform_get_bootarg_addr(), platform_get_bootarg_size());

	if (g_dram_init_ret < 0) {
		dprintf(CRITICAL, "[LK ERROR] DRAM bank number is not correct!!!\n");
		while (1) ;
	}

	//i2c_v1_init();
	PROFILING_START("WDT Init");
	mtk_wdt_init();
	PROFILING_END();

	//i2c init
	i2c_hw_init();

#ifdef MACH_FPGA
	mtk_timer_init();  // GPT4 will be initialized at PL after
	mtk_wdt_disable();  // WDT will be triggered when uncompressing linux image on FPGA
#endif
	pwrap_init_lk();

#if !defined(MACH_FPGA) && !defined(NO_PMIC)
	PROFILING_START("pmic_init");
	pmic_init();
	PROFILING_END();
#endif  // !defined(MACH_FPGA) && !defined(NO_PMIC)
	PROFILING_END(); /* platform_early_init */
}

extern void mt65xx_bat_init(void);
extern bool mtk_bat_allow_backlight_enable(void);
#if defined (MTK_KERNEL_POWER_OFF_CHARGING)

int kernel_charging_boot(void)
{
	if ((g_boot_mode == KERNEL_POWER_OFF_CHARGING_BOOT || g_boot_mode == LOW_POWER_OFF_CHARGING_BOOT) && upmu_is_chr_det() == KAL_TRUE) {
		dprintf(INFO,"[%s] Kernel Power Off Charging with Charger/Usb \n", __func__);
		return  1;
	} else if ((g_boot_mode == KERNEL_POWER_OFF_CHARGING_BOOT || g_boot_mode == LOW_POWER_OFF_CHARGING_BOOT) && upmu_is_chr_det() == KAL_FALSE) {
		dprintf(INFO,"[%s] Kernel Power Off Charging without Charger/Usb \n", __func__);
		return -1;
	} else
		return 0;
}
#endif

static void lk_vb_init(void)
{
#ifdef MTK_SECURITY_SW_SUPPORT
	u64 pl_start_addr = 0;
	plinfo_get_brom_header_block_size(&pl_start_addr);

	PROFILING_START("Security init");
	/* initialize security library */
	sec_func_init(pl_start_addr);
	seclib_set_oemkey(g_oemkey, OEM_PUBK_SZ);
	PROFILING_END();
#endif
}

static void lk_vb_vfy_logo(void)
{
#ifdef MTK_SECURITY_SW_SUPPORT
	/*  bypass logo vfy in fast meta mode */
	if(g_boot_mode == META_BOOT)
		return;

	PROFILING_START("logo verify");
	/*Verify logo before use it*/
	if (0 != img_auth_stor("logo", "logo", 0x0))
		assert(0);

	PROFILING_END();
#endif
}

static void lk_vb_vfy_dtbo(void)
{
#ifdef MTK_SECURITY_SW_SUPPORT
	PROFILING_START("dtbo vfy");
	if (0 != img_auth_stor("odmdtbo", "dtbo", 0x0))
		assert(0);

	PROFILING_END();
#endif
}

void platform_init(void)
{
	int ret;
	int logo_size;
	bool bearly_backlight_on = false;
	u64 pl_start_addr = 0;
	plinfo_get_brom_header_block_size(&pl_start_addr);

	PROFILING_START("platform_init");
	dprintf(CRITICAL, "platform_init()\n");

	init_storage();

	extern void dummy_ap_entry(void)__attribute__((weak)); /* This is empty function for normal load */
	if (dummy_ap_entry)
		dummy_ap_entry();

#ifdef MTK_AB_OTA_UPDATER
	/* get A/B system parameter before load dtb from boot image */
	get_AB_OTA_param();
#endif

	/* The device tree should be loaded as early as possible. */
	load_device_tree();

#ifndef MACH_FPGA
#ifdef USE_DTB_NO_DWS
	mt_gpio_set_default();
#endif/* for support NO DWS DTB */
#endif

#ifndef MACH_FPGA
	PROFILING_START("led init");
	leds_init();
	PROFILING_END();
#endif // MACH_FPGA

#ifdef MTK_KERNEL_POWER_OFF_CHARGING
	if ((g_boot_arg->boot_reason == BR_USB) && (upmu_is_chr_det() == KAL_FALSE)) {
		dprintf(INFO, "[%s] Unplugged Charger/Usb between Pre-loader and Uboot in Kernel Charging Mode, Power Off \n", __func__);
		mt_power_off();
	}
#endif // MTK_KERNEL_POWER_OFF_CHARGING

	PROFILING_START("ENV init");
	env_init();
	print_env();
	PROFILING_END();

	PROFILING_START("disp init");
	/* initialize the frame buffet information */
	g_fb_size = mt_disp_get_vram_size();
	g_fb_base = mblock_reserve_ext(&g_boot_arg->mblock_info, g_fb_size, 0x10000, 0x80000000, 0, "framebuffer");
	if (!g_fb_base) {
		dprintf(CRITICAL, "reserve framebuffer failed\n");
	}

	dprintf(CRITICAL, "FB base = 0x%x, FB size = 0x%x (%d)\n", g_fb_base, g_fb_size, g_fb_size);

#ifndef MACH_FPGA
#ifdef MTK_SMI_SUPPORT
	/* write SMI non on-the-fly register before DISP init */
	smi_apply_register_setting();
#endif
#endif

#ifndef MACH_FPGA_NO_DISPLAY
	mt_disp_init((void *)g_fb_base);
	PROFILING_END();

	PROFILING_START("vedio init");
	drv_video_init();
	PROFILING_END();
#endif

	/*for kpd pmic mode setting*/
	set_kpd_pmic_mode();

#ifndef MACH_FPGA
	PROFILING_START("boot mode select");

#if !defined(NO_BOOT_MODE_SEL)
	boot_mode_select();
#endif
#ifdef MTK_USB2JTAG_SUPPORT
	if (g_boot_mode != FASTBOOT) {
		extern void usb2jtag_init(void);
		usb2jtag_init();
	}
#endif

#ifdef MTK_AEE_PLATFORM_DEBUG_SUPPORT
	/* init lastbus: MUST call after kedump (in boot_mode_select) */
	latch_lastbus_init();
#endif

#ifdef CFG_DTB_EARLY_LOADER_SUPPORT
	/* reload dtb when boot mode = recovery */
	if (g_boot_mode == RECOVERY_BOOT) {
		if (bldr_load_dtb("recovery") < 0)
			dprintf(CRITICAL, "bldr_load_dtb fail\n");
	}
#endif  // CFG_DTB_EARLY_LOADER_SUPPORT

	PROFILING_END(); /* boot mode select */
#endif /*MACH_FPGA */

#ifndef MACH_FPGA_NO_DISPLAY
	/*  fast meta mode */
	if(g_boot_mode != META_BOOT){
		PROFILING_START("load_logo");
		logo_size = mboot_common_load_logo((unsigned long)mt_get_logo_db_addr_pa(), "logo");
		assert(logo_size <= LK_LOGO_MAX_SIZE);
		PROFILING_END();
	}
#endif // MACH_FPGA_NO_DISPLAY

	lk_vb_init();
	lk_vb_vfy_logo();
	lk_vb_vfy_dtbo();

	/*Show download logo & message on screen */
	if (g_boot_arg->boot_mode == DOWNLOAD_BOOT) {
		dprintf(CRITICAL, "[LK] boot mode is DOWNLOAD_BOOT\n");

#ifndef MACH_FPGA_NO_DISPLAY
		PROFILING_START("show logo");
		mt_disp_show_boot_logo();
		PROFILING_END();
#endif
		video_printf(" => Downloading...\n");
		dprintf(CRITICAL, "enable backlight after show bootlogo! \n");
#ifndef MACH_FPGA
		PROFILING_START("backlight");
		mt65xx_backlight_on();
		PROFILING_END();
#endif
#ifndef MACH_FPGA_NO_DISPLAY
		/* pwm need display sof */
		PROFILING_START("disp update");
		mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
		PROFILING_END();
#endif
		logo_lk_t = ((unsigned int)get_timer(boot_time));
		PROFILING_START("disable wdt, l2, cache, mmu");
		mtk_wdt_disable(); //Disable wdt before jump to DA
		platform_uninit();
#ifdef HAVE_CACHE_PL310
		l2_disable();
#endif
		arch_disable_cache(UCACHE);
		arch_disable_mmu();
#ifdef ENABLE_L2_SHARING
		config_shared_SRAM_size();
#endif
		PROFILING_END();
		jump_da(g_boot_arg->da_info.addr, g_boot_arg->da_info.arg1, g_boot_arg->da_info.arg2);
	}
#ifdef MTK_KERNEL_POWER_OFF_CHARGING
	else if (g_boot_mode != ALARM_BOOT && g_boot_mode != FASTBOOT && g_boot_mode != KERNEL_POWER_OFF_CHARGING_BOOT && g_boot_mode != LOW_POWER_OFF_CHARGING_BOOT && mtk_bat_allow_backlight_enable()) {
#ifndef MACH_FPGA_NO_DISPLAY
		/*  fast meta mode */
		if(g_boot_mode != META_BOOT){
			PROFILING_START("show logo");
			mt_disp_show_boot_logo();
			PROFILING_END();
		}
#endif // MACH_FPGA_NO_DISPLAY

#ifndef MACH_FPGA
		PROFILING_START("backlight");
		mt65xx_backlight_on();
		PROFILING_END(); /* backlight */
#endif // MACH_FPGA

#ifndef MACH_FPGA_NO_DISPLAY
	/*  fast meta mode */
		if(g_boot_mode != META_BOOT){
			PROFILING_START("disp update");
			mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
			PROFILING_END();
		}
#endif // MACH_FPGA_NO_DISPLAY
		bearly_backlight_on = true;
		dprintf(INFO, "backlight enabled before battery init\n");
		logo_lk_t = ((unsigned int)get_timer(boot_time));
	}
#endif  // MTK_KERNEL_POWER_OFF_CHARGING

	PROFILING_START("battery init");
#ifndef MACH_FPGA
#if !defined(NO_PLL_TURN_OFF)
	mt_pll_turn_off();
#endif  // !defined(NO_PLL_TURN_OFF)
#if !defined(NO_DCM)
	if (mt_dcm_init())
		dprintf(CRITICAL, "mt_dcm_init fail\n");
#endif  // !defined(NO_DCM)
#if !defined(NO_MD_POWER_DOWN)
	md1_power_down();
#endif  // !defined(NO_MD_POWER_DOWN)
#if !defined(NO_BAT_INIT)
	mt65xx_bat_init();
#endif  // !defined(NO_BAT_INIT)
#endif  // MACH_FPGA
	PROFILING_END(); /* battery init */

#ifndef CFG_POWER_CHARGING
	PROFILING_START("RTC boot check Init");
	/* NOTE: if define CFG_POWER_CHARGING, will rtc_boot_check() in mt65xx_bat_init() */
	rtc_boot_check(false);
	PROFILING_END();
#endif // CFG_POWER_CHARGING

#ifdef MTK_KERNEL_POWER_OFF_CHARGING
	if (kernel_charging_boot() == 1) {
		PROFILING_START("show logo");
#ifdef MTK_BATLOWV_NO_PANEL_ON_EARLY
		CHARGER_TYPE CHR_Type_num = CHARGER_UNKNOWN;
		CHR_Type_num = hw_charging_get_charger_type();
		if ((g_boot_mode != LOW_POWER_OFF_CHARGING_BOOT) ||
		        ((CHR_Type_num != STANDARD_HOST) && (CHR_Type_num != NONSTANDARD_CHARGER))) {
#endif // MTK_BATLOWV_NO_PANEL_ON_EARLY
			mt_disp_power(TRUE);
#ifndef MACH_FPGA_NO_DISPLAY
			mt_disp_show_low_battery();
#endif
			mt65xx_leds_brightness_set(6, 110);
#ifdef MTK_BATLOWV_NO_PANEL_ON_EARLY
		}
#endif
		PROFILING_END();
	} else if (g_boot_mode != KERNEL_POWER_OFF_CHARGING_BOOT && g_boot_mode != LOW_POWER_OFF_CHARGING_BOOT) {
	/*  fast meta mode */
		if(g_boot_mode != META_BOOT)
			if (g_boot_mode != ALARM_BOOT && (g_boot_mode != FASTBOOT) && !bearly_backlight_on) {
#ifndef MACH_FPGA_NO_DISPLAY
				PROFILING_START("show logo");
				mt_disp_show_boot_logo();
				PROFILING_END();
#endif
			}
	}
#else // MTK_KERNEL_POWER_OFF_CHARGING
	/*  fast meta mode */
	if(g_boot_mode != META_BOOT)
		if (g_boot_mode != ALARM_BOOT && (g_boot_mode != FASTBOOT) && !bearly_backlight_on) {
#ifndef MACH_FPGA_NO_DISPLAY
			PROFILING_START("show logo");
			mt_disp_show_boot_logo();
			PROFILING_END(); /* show logo */
#endif
	}
#endif // MTK_KERNEL_POWER_OFF_CHARGING

#ifdef MTK_BATLOWV_NO_PANEL_ON_EARLY
	if (!is_low_battery(0)) {
#endif
#ifndef MACH_FPGA
		PROFILING_START("backlight");
		mt65xx_backlight_on();
		PROFILING_END(); /* backlight */
#endif
		//pwm need display sof
#ifndef MACH_FPGA_NO_DISPLAY
		PROFILING_START("display update");
		mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
		PROFILING_END();
		if (!bearly_backlight_on)
			logo_lk_t = ((unsigned int)get_timer(boot_time));
#endif
#ifdef MTK_BATLOWV_NO_PANEL_ON_EARLY
	}
#endif


#ifndef MACH_FPGA
	PROFILING_START("sw_env");
	sw_env();
	PROFILING_END();
#endif
	PROFILING_END(); /* platform_init */
}

void platform_uninit(void)
{
#ifndef MACH_FPGA
	leds_deinit();
	platform_clear_all_on_mux();
#endif
	platform_deinit_interrupts();
	return;
}

#ifdef ENABLE_L2_SHARING
#define ADDR_CA7L_CACHE_CONFIG_MP(x) (CA7MCUCFG_BASE + 0x200 * x)
#define L2C_SIZE_CFG_OFFSET  8
#define L2C_SHARE_EN_OFFSET  12
/* 4'b1111: 2048KB(not support)
 * 4'b0111: 1024KB(not support)
 * 4'b0011: 512KB
 * 4'b0001: 256KB
 * 4'b0000: 128KB (not support)
 */

int is_l2_need_config(void)
{
	volatile unsigned int cache_cfg, addr;

	addr = ADDR_CA7L_CACHE_CONFIG_MP(0);
	cache_cfg = DRV_Reg32(addr);
	cache_cfg = cache_cfg >> L2C_SIZE_CFG_OFFSET;

	/* only read 256KB need to be config.*/
	if ((cache_cfg &(0x7)) == 0x1) {
		return 1;
	}
	return 0;
}

void cluster_l2_share_enable(int cluster)
{
	volatile unsigned int cache_cfg, addr;

	addr = ADDR_CA7L_CACHE_CONFIG_MP(cluster);
	/* set L2C size to 256KB */
	cache_cfg = DRV_Reg32(addr);
	cache_cfg &= (~0x7) << L2C_SIZE_CFG_OFFSET;
	cache_cfg |= 0x1 << L2C_SIZE_CFG_OFFSET;

	/* enable L2C_share_en. Sram only for other to use*/
	cache_cfg |= (0x1 << L2C_SHARE_EN_OFFSET);
	DRV_WriteReg32(addr, cache_cfg);
}

void cluster_l2_share_disable(int cluster)
{
	volatile unsigned int cache_cfg, addr;

	addr = ADDR_CA7L_CACHE_CONFIG_MP(cluster);
	/* set L2C size to 512KB */
	cache_cfg = DRV_Reg32(addr);
	cache_cfg &= (~0x7) << L2C_SIZE_CFG_OFFSET;
	cache_cfg |= 0x3 << L2C_SIZE_CFG_OFFSET;
	DRV_WriteReg32(addr, cache_cfg);

	/* disable L2C_share_en. Sram only for cpu to use*/
	cache_cfg &= ~(0x1 << L2C_SHARE_EN_OFFSET);
	DRV_WriteReg32(addr, cache_cfg);
}

/* config L2 cache and sram to its size */
void config_L2_size(void)
{
	int cluster;

	if (is_l2_need_config()) {
		/*
		 * Becuase mcu config is protected.
		 * only can write in secutity mode
		 */

		if (dev_info_nr_cpu() == 6) {
			cluster_l2_share_disable(0);
			cluster_l2_share_enable(1);
		}

		else {
			for (cluster = 0; cluster < 2; cluster++) {
				cluster_l2_share_disable(cluster);
			}
		}
	}
}

/* config SRAM back from L2 cache for DA relocation */
void config_shared_SRAM_size(void)
{
	int cluster;

	if (is_l2_need_config()) {
		/*
		 * Becuase mcu config is protected.
		 * only can write in secutity mode
		 */

		for (cluster = 0; cluster < 2; cluster++) {
			cluster_l2_share_enable(cluster);
		}
	}
}
#endif


void platform_sec_post_init(void)
{
	unsigned int ret = 0;
	ret = crypto_hw_engine_disable();
	if (ret) {
		dprintf(CRITICAL, "[SEC] crypto engine HW disable fail\n");
	}
	/* Lock Device APC in LK */
	dprintf(CRITICAL, "[DEVAPC] sec_post_init\n");

#if DEVAPC_TURN_ON
	dprintf(CRITICAL, "[DEVAPC] platform_sec_post_init - SMC call to ATF from LK\n");
	mt_secure_call(0x82000101, 0, 0, 0);
#endif

}

u32 get_devinfo_with_index(u32 index)
{
	return internal_get_devinfo_with_index(index);
}

int platform_skip_hibernation(void)
{
	switch (g_boot_arg->boot_reason) {
#if 0 // let schedule power on to go hiberantion bootup process
		case BR_RTC:
#endif
		case BR_WDT:
		case BR_WDT_BY_PASS_PWK:
		case BR_WDT_SW:
		case BR_WDT_HW:
			return 1;
	}

	return 0;
}
