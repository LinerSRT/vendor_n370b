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
#include <malloc.h>
#include <stdlib.h>
#include <arch/arm/mmu.h>
#include <dev/aee_platform_debug.h>
#include <spm_common.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_typedefs.h>
#include <platform/mtk_wdt.h>
#include <platform/partition.h>
#include <platform/platform_debug.h>
#include <platform/log_store_lk.h>
#include <platform/msdc_utils.h>
#include <plat_debug_interface.h>
#include <reg.h>
#include <fdt.h>
#include <libfdt.h>
#include <debug.h>

#include <platform/mt_sspm.h>

#include <mtk_mcdi.h>

#ifdef MTK_AB_OTA_UPDATER
#include <mt_boot.h>
#endif

int plt_get_cluster_id(unsigned int cpu_id, unsigned int *core_id_in_cluster)
{
	if (core_id_in_cluster == NULL)
		return -1;

	*core_id_in_cluster = (cpu_id % 4);
	return (cpu_id / 4);
}

unsigned long plt_get_cpu_power_status_at_wdt(void)
{
	unsigned long bitmask = 0, ret;

	ret = readl(SLEEP_BASE + cfg_pc_latch.spm_pwr_sts);

	/* CPU0 ~ CPU3 */
	bitmask |= (ret & (0xf << 3)) >> 3;
	/* CPU4 ~ CPU7 */
	bitmask |= (ret & (0xf << 8)) >> 4;

	return bitmask;
}

unsigned int plt_get_dfd_dump_type(void)
{
	/* for mt6758 with DFD 3.0 -> always dump to DRAM*/
	if (cfg_dfd.version >= DFD_V3_0)
		return DFD_DUMP_TO_DRAM;
	else
		return DFD_DUMP_NOT_SUPPORT;
}

static unsigned int save_cpu_bus_data(u64 offset, int *len, CALLBACK dev_write)
{
	char *buf = NULL;
	int ret;
	unsigned int datasize = 0;

	/* Save latch buffer */
	ret = latch_get((void **)&buf, len);
	if (ret && (buf != NULL)) {
		if (*len > 0)
			datasize = dev_write(buf, *len);
		latch_put((void **)&buf);
	}

	/* Save systracker buffer */
	ret = systracker_get((void **)&buf, len);
	if (ret && (buf != NULL)) {
		if (*len > 0)
			datasize += dev_write(buf, *len);
		systracker_put((void **)&buf);
	}

	/* Save etb buffer */
	ret = etb_get((void **)&buf, len);
	if (buf != NULL) {
		if (*len > 0)
			datasize += dev_write(buf, *len);
		etb_put((void **)&buf);
	}

	return datasize;
}

static unsigned int save_dfd_data(u64 offset, int *len, CALLBACK dev_write)
{
	char *buf = NULL;
	unsigned int datasize = 0;

	/* Save dfd buffer */
	if (dfd_get((void **)&buf, len)) {
		datasize = dev_write(buf, *len);
		dfd_put((void **)&buf);
	}

	return datasize;
}


void platform_lastpc_postinit(void)
{
	mt_secure_call(MTK_SIP_LK_LASTPC_AARCH32, 0, 0, 0);
}

/* SPM2 Debug Features */
#define SPM2_WDT_LATCH0     (0x10227000 + 0x190)
#define SPM2_WDT_LATCH1     (0x10227000 + 0x194)
#define SPM2_WDT_LATCH2     (0x10227000 + 0x198)
#define SPM2_WDT_LATCH3     (0x10227000 + 0x1e4)
#define SPM2_WDT_LATCH4     (0x10227000 + 0x1e8)
#define SPM2_WDT_LATCH_NUM  (5)

static unsigned long get_spm2_wdt_latch(int index)
{
	unsigned long ret;

	switch (index) {
#if 0
		case 0:
			ret = readl(SPM2_WDT_LATCH0);
			break;
		case 1:
			ret = readl(SPM2_WDT_LATCH1);
			break;
		case 2:
			ret = readl(SPM2_WDT_LATCH2);
			break;
		case 3:
			ret = readl(SPM2_WDT_LATCH3);
			break;
		case 4:
			ret = readl(SPM2_WDT_LATCH4);
			break;
#endif
		default:
			ret = 0;
	}

	return ret;
}

/* VCOREFS Debug Features */
#define VCOREFS_SRAM_BASE               (0x0011BB80)
#define VCOREFS_SRAM_OPP1_OPP0_COUNT    (VCOREFS_SRAM_BASE + 0)
#define VCOREFS_SRAM_OPP0_OPP1_COUNT    (VCOREFS_SRAM_BASE + 0x4)
#define VCOREFS_SRAM_OPP2_OPP1_COUNT    (VCOREFS_SRAM_BASE + 0x8)
#define VCOREFS_SRAM_OPP1_OPP2_COUNT    (VCOREFS_SRAM_BASE + 0xC)
#define VCOREFS_SRAM_NUM                (4)

static unsigned long get_vcorefs_sram(int index)
{
	unsigned long ret;

	switch (index) {
		case 0:
			ret = readl(VCOREFS_SRAM_OPP1_OPP0_COUNT);
			break;
		case 1:
			ret = readl(VCOREFS_SRAM_OPP0_OPP1_COUNT);
			break;
		case 2:
			ret = readl(VCOREFS_SRAM_OPP2_OPP1_COUNT);
			break;
		case 3:
			ret = readl(VCOREFS_SRAM_OPP1_OPP2_COUNT);
			break;
		default:
			ret = 0;
			break;
	}

	return ret;
}

/* VCOREFS Histroy Debug Features */
#define VCOREFS_SRAM_RECORD1_LSB	(VCOREFS_SRAM_BASE + 0x20)
#define VCOREFS_SRAM_RECORD1_MSB	(VCOREFS_SRAM_BASE + 0x24)
#define VCOREFS_SRAM_RECORD2_LSB	(VCOREFS_SRAM_BASE + 0x28)
#define VCOREFS_SRAM_RECORD2_MSB	(VCOREFS_SRAM_BASE + 0x2C)
#define VCOREFS_SRAM_RECORD3_LSB	(VCOREFS_SRAM_BASE + 0x30)
#define VCOREFS_SRAM_RECORD3_MSB	(VCOREFS_SRAM_BASE + 0x34)
#define VCOREFS_SRAM_RECORD4_LSB	(VCOREFS_SRAM_BASE + 0x38)
#define VCOREFS_SRAM_RECORD4_MSB	(VCOREFS_SRAM_BASE + 0x3C)
#define VCOREFS_SRAM_RECORD5_LSB	(VCOREFS_SRAM_BASE + 0x40)
#define VCOREFS_SRAM_RECORD5_MSB	(VCOREFS_SRAM_BASE + 0x44)
#define VCOREFS_SRAM_RECORD6_LSB	(VCOREFS_SRAM_BASE + 0x48)
#define VCOREFS_SRAM_RECORD6_MSB	(VCOREFS_SRAM_BASE + 0x4C)
#define VCOREFS_SRAM_RECORD7_LSB	(VCOREFS_SRAM_BASE + 0x50)
#define VCOREFS_SRAM_RECORD7_MSB	(VCOREFS_SRAM_BASE + 0x54)
#define VCOREFS_SRAM_RECORD8_LSB	(VCOREFS_SRAM_BASE + 0x58)
#define VCOREFS_SRAM_RECORD8_MSB	(VCOREFS_SRAM_BASE + 0x5C)

#define VCOREFS_SRAM_RECORD_IDX		(VCOREFS_SRAM_BASE + 0x70)

#define VCOREFS_HISTORY_SRAM_NUM                (8)

static unsigned long get_vcorefs_history_sram(int index)
{
	unsigned long ret;

	switch (index) {
		case 0:
			ret = readl(VCOREFS_SRAM_RECORD1_LSB);
			break;
		case 1:
			ret = readl(VCOREFS_SRAM_RECORD1_MSB);
			break;
		case 2:
			ret = readl(VCOREFS_SRAM_RECORD2_LSB);
			break;
		case 3:
			ret = readl(VCOREFS_SRAM_RECORD2_MSB);
			break;
		case 4:
			ret = readl(VCOREFS_SRAM_RECORD3_LSB);
			break;
		case 5:
			ret = readl(VCOREFS_SRAM_RECORD3_MSB);
			break;
		case 6:
			ret = readl(VCOREFS_SRAM_RECORD4_LSB);
			break;
		case 7:
			ret = readl(VCOREFS_SRAM_RECORD4_MSB);
			break;
		case 8:
			ret = readl(VCOREFS_SRAM_RECORD5_LSB);
			break;
		case 9:
			ret = readl(VCOREFS_SRAM_RECORD5_MSB);
			break;
		case 10:
			ret = readl(VCOREFS_SRAM_RECORD6_LSB);
			break;
		case 11:
			ret = readl(VCOREFS_SRAM_RECORD6_MSB);
			break;
		case 12:
			ret = readl(VCOREFS_SRAM_RECORD7_LSB);
			break;
		case 13:
			ret = readl(VCOREFS_SRAM_RECORD7_MSB);
			break;
		case 14:
			ret = readl(VCOREFS_SRAM_RECORD8_LSB);
			break;
		case 15:
			ret = readl(VCOREFS_SRAM_RECORD8_MSB);
			break;
		case 16:
			ret = readl(VCOREFS_SRAM_RECORD_IDX);
			break;
		default:
			ret = 0;
			break;
	}

	return ret;
}

/* SPM Debug Features */
static unsigned int spm_wdt_latch_regs[] = { SLEEP_BASE + 0x1A0, /* PCM_WDT_LATCH_0 */
	SLEEP_BASE + 0x1A4, /* PCM_WDT_LATCH_1 */
	SLEEP_BASE + 0x1A8, /* PCM_WDT_LATCH_2 */
	SLEEP_BASE + 0x1AC, /* PCM_WDT_LATCH_3 */
	SLEEP_BASE + 0x1B0, /* PCM_WDT_LATCH_4 */
	SLEEP_BASE + 0x1B4, /* PCM_WDT_LATCH_5 */
	SLEEP_BASE + 0x1B8, /* PCM_WDT_LATCH_6 */
	SLEEP_BASE + 0x1BC, /* PCM_WDT_LATCH_7 */
	SLEEP_BASE + 0x1C0, /* PCM_WDT_LATCH_8 */
	SLEEP_BASE + 0x1C4, /* PCM_WDT_LATCH_9 */
	SLEEP_BASE + 0x67C, /* PCM_WDT_LATCH_10 */
	SLEEP_BASE + 0x680 /* PCM_WDT_LATCH_11 */
};
#define SPM_DATA_BUF_LENGTH (4096)

/* SPM SRAM Debug Features */
static unsigned int spm_rsv_debug_regs[] = {
	SLEEP_BASE + 0x6C4, /* SPM_SW_RSV_6 */
	SLEEP_BASE + 0x6C8, /* SPM_SW_RSV_7 */
	SLEEP_BASE + 0x6CC, /* SPM_SW_RSV_8 */
	SLEEP_BASE + 0x6D0,
	SLEEP_BASE + 0x6D4
};

#define SPM_RSV_DEBUG_NUM                (3)

static int spm_dump_data(char *buf, int *wp)
{
	unsigned int i;
	unsigned val;
	unsigned val2;
#ifdef SPM_FW_USE_PARTITION
	char part_name[16] = "spmfw";

#ifdef MTK_AB_OTA_UPDATER
	get_AB_OTA_name((void *)&part_name, sizeof(part_name));
#endif /* MTK_AB_OTA_UPDATER */
#endif /* SPM_FW_USE_PARTITION */

	if (buf == NULL || wp == NULL)
		return -1;

	/*
	 * Example output:
	 * SPM Suspend debug regs(index 1) = 0x8320535
	 * SPM Suspend debug regs(index 2) = 0xfe114200
	 * SPM Suspend debug regs(index 3) = 0x3920fffe
	 * SPM Suspend debug regs(index 4) = 0x3ac06f4f
	 */

	for (i = 0; i < (sizeof(spm_wdt_latch_regs)/sizeof(unsigned int)); i++) {
		val = readl(spm_wdt_latch_regs[i]);
		*wp += sprintf(buf + *wp,
		               "SPM Suspend debug regs(index %d) = 0x%x\n",
		               i + 1, val);
	}

	for (i = 0; i < (sizeof(spm_rsv_debug_regs)/sizeof(spm_rsv_debug_regs[0])); i++) {
		val = readl(spm_rsv_debug_regs[i]);
		*wp += sprintf(buf + *wp,
		               "SPM rsv debug regs(index %d) = 0x%x\n",
		               i + 1, val);
	}

#ifdef SPM_FW_USE_PARTITION
	get_spmfw_version(part_name, "spmfw", buf, wp);
#endif /* SPM_FW_USE_PARTITION */

	for (i = 0; i < VCOREFS_SRAM_NUM; i++) {
		val = get_vcorefs_sram(i);
		*wp += sprintf(buf + *wp,
		               "vcore dvfs debug regs(index %d) = 0x%x\n",
		               i + 1, val);
	}

	for (i = 0; i < VCOREFS_HISTORY_SRAM_NUM; i++) {
		val = get_vcorefs_history_sram(i * 2);
		val2 = get_vcorefs_history_sram(i * 2 + 1);
		*wp += sprintf(buf + *wp,
		               "vcore dvfs record(index %d) = 0x%08X%08X\n",
		               i+1, val2, val);
	}

	val = get_vcorefs_history_sram(VCOREFS_HISTORY_SRAM_NUM * 2);
	*wp += sprintf(buf + *wp,
		               "vcore dvfs record(idx) = %d\n", val);

	for (i = 0; i < SPM2_WDT_LATCH_NUM; i++) {
		val = get_spm2_wdt_latch(i);
		*wp += sprintf(buf + *wp,
		               "SPM2_WDT_Latch%d = 0x%x\n",
		               i, val);
	}

	return 1;
}

int spm_data_get(void **data, int *len)
{
	int ret;

	*len = 0;
	*data = malloc(SPM_DATA_BUF_LENGTH);
	if (*data == NULL)
		return 0;

	ret = spm_dump_data(*data, len);
	if (ret < 0 || *len > SPM_DATA_BUF_LENGTH) {
		*len = (*len > SPM_DATA_BUF_LENGTH) ? SPM_DATA_BUF_LENGTH : *len;
		return ret;
	}

	return 1;
}

void spm_data_put(void **data)
{
	free(*data);
}

static unsigned int save_spm_data(u64 offset, int *len, CALLBACK dev_write)
{
	char *buf = NULL;
	unsigned int datasize = 0;

	/* Save SPM buffer */
	spm_data_get((void **)&buf, len);
	if (buf != NULL) {
		if (*len > 0)
			datasize = dev_write(buf, *len);
		spm_data_put((void **)&buf);
	}

	return datasize;
}

/* FOR DRAMC data and DRAM Calibration Log
*
* This area is applied for DRAM related debug.
*/
/* DRAMC data */
#define DRAM_DEBUG_SRAM_ADDRESS 0x11D800
#define DRAM_DEBUG_SRAM_LENGTH  1024
static int plat_dram_debug_get(void **data, int *len)
{
	*data = (void *)DRAM_DEBUG_SRAM_ADDRESS;
	*len = DRAM_DEBUG_SRAM_LENGTH;
	return 1;
}

/* shared SRAM for DRAMLOG calibration log */
#define DRAM_KLOG_SRAM_ADDRESS 0x0011E000
#define DRAM_KLOG_SRAM_LENGTH  8192
#define DRAM_KLOG_VALID_ADDRESS 0x0011E00C
static int plat_dram_klog_get(void **data, int *len)
{
	*data = (void *)DRAM_KLOG_SRAM_ADDRESS;
	*len = DRAM_KLOG_SRAM_LENGTH;
	return 1;
}

static bool plat_dram_has_klog(void)
{
	if (*(volatile unsigned int*)DRAM_KLOG_VALID_ADDRESS)
		return true;

	return false;
}

static unsigned int save_dram_data(u64 offset, int *len, CALLBACK dev_write)
{
	char *buf = NULL;
	unsigned int datasize = 0, allsize = 0;

	if (plat_dram_debug_get((void **)&buf, len)) {
		datasize = dev_write(buf, *len);
		allsize = datasize;
	}

	if (plat_dram_klog_get((void **)&buf, len)) {
		buf = malloc(*len);
		if (buf) {
			mrdump_read_log(buf, *len, MRDUMP_EXPDB_DRAM_KLOG_OFFSET);
			datasize = dev_write(buf, *len);
			allsize += datasize;
			free(buf);
		}
	}

	return allsize;
}

#ifdef MTK_TINYSYS_SSPM_SUPPORT
#define SSPM_RETRY          10
static unsigned int save_sspm_coredump(u64 offset, int *len, CALLBACK dev_write)
{
	unsigned int buf = 0;
	unsigned int datasize = 0;
	int retry = SSPM_RETRY;

	if (!(*(unsigned int *)SSPM_BACKUP)) {
		return 0;
	}

	do {
		buf = *(unsigned int *)(SSPM_DM_ADDR);
		*len = *(int *)SSPM_DM_SZ;
		if ( (buf != 0) && (*len > 0) ) {
			datasize = dev_write( (unsigned int *)buf, *len);
			break;
		} else {
			udelay(100);
		}
	} while ( --retry);

	buf = *(unsigned int *)(SSPM_RM_ADDR);
	*len = *(int *)SSPM_RM_SZ;
	if ( (buf != 0) && (*len > 0) ) {
		    datasize += dev_write( (unsigned int *)buf, *len);
	}

	return datasize;
}

static unsigned int save_sspm_data(u64 offset, int *len, CALLBACK dev_write)
{
#define SSPM_BUF_LEN        256
	char *buf = NULL;
	unsigned int datasize = 0;

	buf = malloc(SSPM_BUF_LEN);
	if (!buf) {
		return 0;
	}

	memset(buf, 0, SSPM_BUF_LEN);
	*len = snprintf(buf, SSPM_BUF_LEN,
			"AHB_STATUS: 0x%x\n"
			"AHB_M0_ADDR: 0x%x\n"
			"AHB_M1_ADDR: 0x%x\n"
			"PC: 0x%x\n",
			DRV_Reg32(SSPM_AHB_STATUS), DRV_Reg32(SSPM_AHB_M0_ADDR),
			DRV_Reg32(SSPM_AHB_M1_ADDR), DRV_Reg32(SSPM_PC));

	if (*len > 0) {
		datasize = dev_write(buf, (*len > SSPM_BUF_LEN ? SSPM_BUF_LEN : *len));
	}

	free(buf);
	return datasize;
}

static unsigned int save_sspm_xfile(u64 offset, int *len, CALLBACK dev_write)
{
	unsigned int buf = 0;
	unsigned int datasize = 0;
	unsigned int sspm_info;
	int ret;

	/* Get the information stored in SSPM_INFO by preloader
	    struct sspm_info_t {
	        unsigned int sspm_dm_ofs;
	        unsigned int sspm_dm_sz;
	        unsigned int rd_ofs;
	        unsigned int rd_sz;
	        unsigned int xfile_addr;
	        unsigned int xfile_sz;
	    };
	*/
	sspm_info = *(unsigned int *)SSPM_INFO;

#ifdef MTK_3LEVEL_PAGETABLE
	ret = arch_mmu_map(ROUNDDOWN((uint64_t)sspm_info, SECTION_SIZE), ROUNDDOWN((uint32_t)sspm_info, SECTION_SIZE),
		MMU_MEMORY_TYPE_NORMAL_WRITE_BACK | MMU_MEMORY_AP_P_RW_U_NA, SECTION_SIZE);

	if (ret) {
		dprintf(CRITICAL, "kedump: mmu map to 0x%llx fail(%d)\n", ROUNDDOWN((uint64_t)sspm_info, SECTION_SIZE), ret);
		return 0;
	}
#endif

	buf = *((unsigned int *)sspm_info + 4);
	*len = *((int *)sspm_info + 5);
	dprintf(CRITICAL, "buf 0x%lx, len:0x%x\n", buf, *len);

	if ( (buf != 0) && (*len > 0) ) {
		datasize = dev_write( (unsigned int *)buf, *len);
	}

	return datasize;
}

static unsigned int save_sspm_last_log(u64 offset, int *len, CALLBACK dev_write)
{
	unsigned int buf = 0;
	unsigned int datasize = 0;

	buf = *(unsigned int *)(SSPM_LASTK_ADDR);
	*len = *(int *)SSPM_LASTK_SZ;
	if ( (buf != 0) && (*len > 0) ) {
		datasize = dev_write( (unsigned int *)buf, *len);
	}

	return datasize;
}
#endif

static int plat_write_dram_klog(void)
{
	char *sram_base = NULL;
	int len = 0;

	if (plat_dram_klog_get((void **)&sram_base, (int *)&len)) {
		if (plat_dram_has_klog()) {
			mrdump_write_log(MRDUMP_EXPDB_DRAM_KLOG_OFFSET, sram_base, len);
		}
	}
	return 0;
}

/* SRAM for Hybrid CPU DVFS */
#define HVFS_SRAM_ADDRESS 0x0011bc00
#define HVFS_SRAM_LENGTH  0x1400	/* 5K bytes */
static int plat_hvfs_data_get(void **data, int *len)
{
	*data = (void *)HVFS_SRAM_ADDRESS;
	*len = HVFS_SRAM_LENGTH;
	return 1;
}

static unsigned int save_hvfs_data(u64 offset, int *len, CALLBACK dev_write)
{
	char *buf = NULL;
	unsigned int datasize = 0;

	if (plat_hvfs_data_get((void **)&buf, len)) {
		datasize = dev_write(buf, *len);
	}

	return datasize;
}

/* last pl_lk log */
static int plat_pllk_log_get(void **data, int *len)
{
	*data = (void *)mirror_buf_addr_get();
	*len = mirror_buf_pl_lk_log_size_get();
	if((*data == 0) || (*len == 0)) {
		dprintf(CRITICAL, "[LK_LOG_STORE] invalid address or log length(addr 0x%x, len 0x%x)\n", (unsigned int)*data, (unsigned int)*len);
		return 0;
	}
	dprintf(CRITICAL, "[LK_LOG_STORE] the mirror buf addr is 0x%x, log len is 0x%x\n", (unsigned int)*data, (unsigned int)*len);
	return 1;
}

static unsigned int save_pllk_last_log(u64 offset, int *len, CALLBACK dev_write)
{
	char *buf = NULL;
	unsigned int datasize = 0;

	if (plat_pllk_log_get((void **)&buf, len)) {
		datasize = dev_write(buf, *len);
	}

	return datasize;
}

static int plat_mcdi_data_get(void **data, int *len)
{
	mcdi_setup_file_info_for_kedump();
	*data = (void *)MCDI_SRAM_ADDRESS;
	*len  = MCDI_SRAM_LENGTH;

	return 1;
}

static unsigned int save_mcdi_data(u64 offset, int *len, CALLBACK dev_write)
{
	char *buf = NULL;
	unsigned int datasize = 0;

	if (plat_mcdi_data_get((void **)&buf, len)) {
		datasize = dev_write(buf, *len);
	}

	return datasize;
}

/* platform initial function */
int platform_debug_init(void)
{
	/* function pointer assignment */
	plat_spm_data_get = save_spm_data;
	plat_dram_get = save_dram_data;
	plat_hvfs_get = save_hvfs_data;
	plat_cpu_bus_get = save_cpu_bus_data;
#ifdef MTK_TINYSYS_SSPM_SUPPORT
	plat_sspm_coredump_get = save_sspm_coredump;
	plat_sspm_data_get = save_sspm_data;
	plat_sspm_xfile_get = save_sspm_xfile;
	plat_sspm_log_get = save_sspm_last_log;
#endif
	plat_pllk_last_log_get = save_pllk_last_log;
	plat_mcdi_get = save_mcdi_data;

	/* check dfd_valid_before_reboot and efuse for DFD 3.0 */
	if ((readl(cfg_dfd.plat_sram_flag1) & 0x2) && (get_efuse_dfd_disabled() == 0x0)) {
		plat_dfd20_get = save_dfd_data;
	}

	/* routine tasks */
	plat_write_dram_klog();

	return 1;
}

extern int get_ccci_md_view_smem_addr(unsigned long long *ap_addr, unsigned int *md_addr);
int dfd_set_base_addr(void *fdt)
{
	int ret = 0;
	int offset;
	u64 addr;
	u32 addr_msb;
	unsigned int md_addr;
	unsigned long long ap_addr;

	if (!fdt)
		return -1;

	ret = get_ccci_md_view_smem_addr(&ap_addr, &md_addr);
	if (ret < 0)
		return ret;

	offset = fdt_path_offset(fdt, "/chosen");
	if (offset < 0)
		return offset;

	/* pass base address to kernel */
	addr = cpu_to_fdt64(md_addr);
	ret = fdt_setprop(fdt, offset, "dfd,base_addr", &addr, sizeof(addr));
	if (ret < 0)
		return ret;

	addr_msb = cpu_to_fdt32(ap_addr);
	ret = fdt_setprop(fdt, offset, "dfd,base_addr_msb", &addr_msb, sizeof(addr_msb));
	if (ret < 0)
		return ret;

	/*
	 * write base address[31:1] from AP view to plat_sram_flag2[31:1]
	 * write base address[32:32] from AP view to plat_sram_flag2[0:0]
	 */
	writel((ap_addr & ~(0x1)) | ((ap_addr >> 32) & 0x1),
	       cfg_dfd.plat_sram_flag2);

	return ret;
}
