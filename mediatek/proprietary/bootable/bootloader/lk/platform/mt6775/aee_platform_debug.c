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
#include <dev/aee_platform_debug.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_typedefs.h>
#include <platform/mtk_wdt.h>
#include <platform/platform_debug.h>
#include <platform/log_store_lk.h>
#include <plat_debug_interface.h>
#include <reg.h>
#include <fdt.h>
#include <libfdt.h>
#include <debug.h>
#ifdef MTK_TINYSYS_SSPM_SUPPORT
#include <platform/mt_sspm.h>
#include <arch/arm/mmu.h>
#endif
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

void reset_snoop_filter_ctrl(void)
{
	mt_secure_call(MTK_SIP_LK_PLAT_DEBUG, PLAT_MTK_DFD_RESET_SNOOP_FILTER_MAGIC, 0, 0);
}

static void setup_snoop_filter_ram_ctrl(void)
{
	mt_secure_call(MTK_SIP_LK_PLAT_DEBUG, PLAT_MTK_DFD_SETUP_SNOOP_FILTER_MAGIC, 0, 0);
}

static void return_snoop_filter_ram_ctrl(void)
{
	mt_secure_call(MTK_SIP_LK_PLAT_DEBUG, PLAT_MTK_DFD_RETURN_SNOOP_FILTER_MAGIC, 0, 0);
}

static void circular_buffer_lock(void)
{
	mt_secure_call(MTK_SIP_LK_PLAT_DEBUG, PLAT_MTK_CIRCULAR_BUFFER_LOCK, 0, 0);
}

static void circular_buffer_unlock(void)
{
	mt_secure_call(MTK_SIP_LK_PLAT_DEBUG, PLAT_MTK_CIRCULAR_BUFFER_UNLOCK, 0, 0);
}

unsigned int plt_get_dfd_dump_type(void)
{
	return DFD_DUMP_TO_SRAM;
}

static unsigned int save_cpu_bus_data(u64 offset, int *len, CALLBACK dev_write)
{
	char *buf = NULL;
	int ret;
	unsigned int datasize = 0;

	/* Save latch buffer */
	ret = latch_get((void **)&buf, len);
	if (buf != NULL) {
		if (*len > 0)
			datasize = dev_write(buf, *len);
		latch_put((void **)&buf);
	}

	/* Save systracker buffer */
	ret = systracker_get((void **)&buf, len);
	if (buf != NULL) {
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
	int ret;
	unsigned int datasize = 0;

	/* Save dfd buffer */
	ret = dfd_get((void **)&buf, len);

	if (buf != NULL) {
		if (*len > 0)
			datasize = dev_write(buf, *len);
		dfd_put((void **)&buf);
	}

	return datasize;
}

void platform_clear_all_on_mux(void)
{
	/* clear rg_mcu_pwr_iso_dis */
	writel(readl(MCU_ALL_PWR_ON_CTRL) & ~(1 << 2), MCU_ALL_PWR_ON_CTRL);
	dsb();

	/* clear rg_mcu_pwr_on */
	writel(readl(MCU_ALL_PWR_ON_CTRL) & ~(1 << 1), MCU_ALL_PWR_ON_CTRL);
	dsb();
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
		default:
			ret = 0;
	}

	return ret;
}

/* VCOREFS Debug Features */
#define VCOREFS_SRAM_BASE               (0x0011CF80)
#define VCOREFS_SRAM_DVFS_UP_COUNT      (VCOREFS_SRAM_BASE + 0x54)
#define VCOREFS_SRAM_DVFS_DOWN_COUNT    (VCOREFS_SRAM_BASE + 0x58)
#define VCOREFS_SRAM_DVFS2_UP_COUNT     (VCOREFS_SRAM_BASE + 0x5c)
#define VCOREFS_SRAM_DVFS2_DOWN_COUNT   (VCOREFS_SRAM_BASE + 0x60)
#define VCOREFS_SRAM_DVFS_UP_TIME       (VCOREFS_SRAM_BASE + 0x64)
#define VCOREFS_SRAM_DVFS_DOWN_TIME     (VCOREFS_SRAM_BASE + 0x68)
#define VCOREFS_SRAM_DVFS2_UP_TIME      (VCOREFS_SRAM_BASE + 0x6c)
#define VCOREFS_SRAM_DVFS2_DOWN_TIME    (VCOREFS_SRAM_BASE + 0x70)
#define VCOREFS_SRAM_EMI_BLOCK_TIME     (VCOREFS_SRAM_BASE + 0x74)
#define VCOREFS_SRAM_NUM                (9)

static unsigned long get_vcorefs_sram(int index)
{
	unsigned long ret;

	switch (index) {
		case 0:
			ret = readl(VCOREFS_SRAM_DVFS_UP_COUNT);
			break;
		case 1:
			ret = readl(VCOREFS_SRAM_DVFS_DOWN_COUNT);
			break;
		case 2:
			ret = readl(VCOREFS_SRAM_DVFS2_UP_COUNT);
			break;
		case 3:
			ret = readl(VCOREFS_SRAM_DVFS2_DOWN_COUNT);
			break;
		case 4:
			ret = readl(VCOREFS_SRAM_DVFS_UP_TIME);
			break;
		case 5:
			ret = readl(VCOREFS_SRAM_DVFS_DOWN_TIME);
			break;
		case 6:
			ret = readl(VCOREFS_SRAM_DVFS2_UP_TIME);
			break;
		case 7:
			ret = readl(VCOREFS_SRAM_DVFS2_DOWN_TIME);
			break;
		case 8:
			ret = readl(VCOREFS_SRAM_EMI_BLOCK_TIME);
			break;
		default:
			ret = 0;
	}

	return ret;
}

/* SPM Debug Features */
#define PCM_WDT_LATCH_0     (SLEEP_BASE + 0x190)
#define PCM_WDT_LATCH_1     (SLEEP_BASE + 0x194)
#define PCM_WDT_LATCH_2     (SLEEP_BASE + 0x198)
#define PCM_WDT_LATCH_3     (SLEEP_BASE + 0x1C4)
#define PCM_WDT_LATCH_4     (SLEEP_BASE + 0x1E0)
#define PCM_WDT_LATCH_5     (SLEEP_BASE + 0x1E4)
#define PCM_WDT_LATCH_6     (SLEEP_BASE + 0x1E8)
#define PCM_WDT_LATCH_7     (SLEEP_BASE + 0x1EC)
#define PCM_WDT_LATCH_8     (SLEEP_BASE + 0x1F0)
#define PCM_WDT_LATCH_9     (SLEEP_BASE + 0x1F4)
#define PCM_WDT_LATCH_10    (SLEEP_BASE + 0x1F8)
#define PCM_WDT_LATCH_11    (SLEEP_BASE + 0x1FC)
#define PCM_WDT_LATCH_NUM   (12)
#define SPM_DATA_BUF_LENGTH (2048)
static unsigned long get_spm_wdt_latch(int index)
{
	unsigned long ret;

	switch (index) {
		case 0:
			ret = readl(PCM_WDT_LATCH_0);
			break;
		case 1:
			ret = readl(PCM_WDT_LATCH_1);
			break;
		case 2:
			ret = readl(PCM_WDT_LATCH_2);
			break;
		case 3:
			ret = readl(PCM_WDT_LATCH_3);
			break;
                case 4:
                        ret = readl(PCM_WDT_LATCH_4);
                        break;
                case 5:
                        ret = readl(PCM_WDT_LATCH_5);
                        break;
                case 6:
                        ret = readl(PCM_WDT_LATCH_6);
                        break;
                case 7:
                        ret = readl(PCM_WDT_LATCH_7);
                        break;
                case 8:
                        ret = readl(PCM_WDT_LATCH_8);
                        break;
                case 9:
                        ret = readl(PCM_WDT_LATCH_9);
                        break;
                case 10:
                        ret = readl(PCM_WDT_LATCH_10);
                        break;
                case 11:
                        ret = readl(PCM_WDT_LATCH_11);
                        break;
		default:
			ret = 0;
	}

	return ret;
}

/* SPM Debug Features */
static unsigned int spm_wdt_latch_regs[] = {
	SLEEP_BASE + 0x1A0, /* PCM_WDT_LATCH_0  */
	SLEEP_BASE + 0x1A4, /* PCM_WDT_LATCH_1  */
	SLEEP_BASE + 0x1A8, /* PCM_WDT_LATCH_2  */
	SLEEP_BASE + 0x1AC, /* PCM_WDT_LATCH_3  */
	SLEEP_BASE + 0x1B0, /* PCM_WDT_LATCH_4  */
	SLEEP_BASE + 0x1B4, /* PCM_WDT_LATCH_5  */
	SLEEP_BASE + 0x1B8, /* PCM_WDT_LATCH_6  */
	SLEEP_BASE + 0x1BC, /* PCM_WDT_LATCH_7  */
	SLEEP_BASE + 0x1C0, /* PCM_WDT_LATCH_8  */
	SLEEP_BASE + 0x1C4, /* PCM_WDT_LATCH_9  */
	SLEEP_BASE + 0x67C, /* PCM_WDT_LATCH_10 */
	SLEEP_BASE + 0x680, /* PCM_WDT_LATCH_11 */
	SLEEP_BASE + 0x6D0, /* PCM_WDT_LATCH_12 */
	SLEEP_BASE + 0x6D4, /* PCM_WDT_LATCH_13 */
	SLEEP_BASE + 0x6D8, /* PCM_WDT_LATCH_14 */
};

#define DVFSRC_DUMP	(DVFSRC_BASE + 0x400)
#define DVFSRC_SIZE	0xE0

/* need to check aee_db_file_info[] @ app/mt_boot/aee/KEDump.c */
#define SPM_DATA_BUF_LENGTH (4096)

static int spm_dump_data(char *buf, int *wp)
{
	unsigned int i;
	unsigned val;
#ifdef SPM_FW_USE_PARTITION
	char part_name[16] = "spmfw";

#ifdef MTK_AB_OTA_UPDATER
	get_AB_OTA_name((void *)&part_name, sizeof(part_name));
#endif /* MTK_AB_OTA_UPDATER */
#endif /* SPM_FW_USE_PARTITION */

	if (buf == NULL || wp == NULL)
		return -1;

	for (i = 0; i < (sizeof(spm_wdt_latch_regs)/sizeof(unsigned int)); i++) {
		val = readl(spm_wdt_latch_regs[i]);
		*wp += sprintf(buf + *wp,
		        "SPM regs(0x%x) = 0x%x\n",
		        spm_wdt_latch_regs[i], val);
	}

#ifdef SPM_FW_USE_PARTITION
	get_spmfw_version(part_name, "spmfw", buf, wp);
#endif /* SPM_FW_USE_PARTITION */

	for (i = 0; i < DVFSRC_SIZE; i += 4) {
		val = readl(DVFSRC_DUMP + i);
		*wp += sprintf(buf + *wp,
		       "DVFSRC regs(0x%x) = 0x%08x\n", DVFSRC_DUMP + i, val);
	}

	if (*wp > SPM_DATA_BUF_LENGTH) {
		dprintf(CRITICAL, "[spm] out of range: 0x%x > SPM_DATA_BUF_LENGTH(0x%x)\n", *wp, SPM_DATA_BUF_LENGTH);
		assert(0);
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
static unsigned int save_sspm_coredump(u64 offset, int *len, CALLBACK dev_write)
{
	unsigned int *buf = NULL;
	unsigned int datasize = 0;
	int retry = SSPM_COREDUP_RETRY;

	if (!(*(unsigned int *)SSPM_BACKUP)) {
		return 0;
	}

	do {
		buf = *(unsigned int **)(SSPM_DM_ADDR);
		*len = *(int *)SSPM_DM_SZ;
		if ( (buf != NULL) && (*len > 0) ) {
			datasize = dev_write( buf, *len);
			break;
		} else {
			udelay(100);
		}
	} while ( --retry);

	buf = *(unsigned int **)(SSPM_RM_ADDR);
	*len = *(int *)SSPM_RM_SZ;
	if ( (buf != NULL) && (*len > 0) ) {
		    datasize += dev_write( buf, *len);
	}

	return datasize;
}

static unsigned int save_sspm_data(u64 offset, int *len, CALLBACK dev_write)
{
	char *buf = NULL;
	unsigned int datasize = 0, tbufl, tbufh, r, i, j;
	int length = 0;

	buf = malloc(SSPM_DATA_BUF_SZ);
	if (!buf) {
		return 0;
	}

	memset(buf, 0, SSPM_DATA_BUF_SZ);
	length += snprintf(buf + length, SSPM_DATA_BUF_SZ - length,
		"AHB_STATUS: 0x%x\n"
		"AHB_M0_ADDR: 0x%x\n"
		"AHB_M1_ADDR: 0x%x\n"
		"AHB_M2_ADDR: 0x%x\n"
		"LastSP: 0x%x\n"
		"LastLR: 0x%x\n"
		"LastPC: 0x%x\n",
		DRV_Reg32(SSPM_AHB_STATUS), DRV_Reg32(SSPM_AHB_M0_ADDR),
		DRV_Reg32(SSPM_AHB_M1_ADDR), DRV_Reg32(SSPM_AHB_M2_ADDR),
		DRV_Reg32(SSPM_MD32_SP), DRV_Reg32(SSPM_MD32_LR),
		DRV_Reg32(SSPM_MD32_PC));

	r = DRV_Reg32(SSPM_TBUF_WPTR);
	length += snprintf(buf + length, SSPM_DATA_BUF_SZ - length, "\nSSPM_TBUF_WPTR=%u\n", r);

	for (i = 0, j = r; i < 16; ++i, j = --j & 0xF) {
		DRV_WriteReg32(SSPM_DBG_SEL, j);
		tbufl = DRV_Reg32(SSPM_TBUFL);
		tbufh = DRV_Reg32(SSPM_TBUFH);
		length += snprintf(buf + length, SSPM_DATA_BUF_SZ - length, "%u: TBUF=%u TBUFH=0x%x TBUFL=0x%x\n", i, j, tbufh, tbufl);
	}

	*len = length;

	if (*len > 0) {
		datasize = dev_write((unsigned int*)buf, (*len > SSPM_DATA_BUF_SZ ? SSPM_DATA_BUF_SZ : *len));
	}

	free(buf);
	return datasize;
}

static unsigned int save_sspm_xfile(u64 offset, int *len, CALLBACK dev_write)
{
	unsigned int *buf = NULL;
	unsigned int *sspm_info = NULL;
	unsigned int datasize = 0;

	/* Get the information stored in *SSPM_INFO by preloader
	    struct sspm_info_t {
	        unsigned int sspm_dm_ofs;
	        unsigned int sspm_dm_sz;
	        unsigned int rd_ofs;
	        unsigned int rd_sz;
	        unsigned int xfile_addr;
	        unsigned int xfile_sz;
	    };
	*/

	sspm_info = *(unsigned int **)(SSPM_INFO);
	buf = *(unsigned int **)(sspm_info + 4);
	*len = *(int *)(sspm_info + 5);

	if ( (buf != NULL) && (*len > 0) ) {
		datasize = dev_write( buf, *len);
	}

	return datasize;
}

static unsigned int save_sspm_last_log(u64 offset, int *len, CALLBACK dev_write)
{
	unsigned int *buf = NULL;
	unsigned int datasize = 0;

	buf = *(unsigned int **)(SSPM_LASTK_ADDR);
	*len = *(int *)SSPM_LASTK_SZ;
	if ( (buf != NULL) && (*len > 0) ) {
		datasize = dev_write( buf, *len);
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
#define HVFS_SRAM_LENGTH  0x1400		/* 5K bytes */
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
	*len = (void *)mirror_buf_pl_lk_log_size_get();
	if((*data == 0) || (*len == 0)) {
		dprintf(CRITICAL, "[LK_LOG_STORE] invalid address or log length(addr 0x%x, len 0x%x)\n", *data, *len);
		return 0;
	}
	dprintf(CRITICAL, "[LK_LOG_STORE] the mirror buf addr is 0x%x, log len is 0x%x\n", *data, *len);
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
	plat_cpu_bus_get = save_cpu_bus_data;
	plat_hvfs_get = save_hvfs_data;
#ifdef MTK_TINYSYS_SSPM_SUPPORT
#ifdef MTK_3LEVEL_PAGETABLE
	int ret = arch_mmu_map(ROUNDDOWN(*(uint64_t *)(SSPM_INFO), SECTION_SIZE),
                            ROUNDDOWN(*(uint32_t *)(SSPM_INFO), SECTION_SIZE),
                            MMU_MEMORY_TYPE_NORMAL_WRITE_BACK | MMU_MEMORY_AP_P_RW_U_NA,
                            SECTION_SIZE);

	if (ret) {
		dprintf(CRITICAL, "kedump: mmu map to 0x%llx fail(%d), SSPM dump might fail.\n",
                (unsigned long long)ROUNDDOWN(*(uint64_t *)(SSPM_INFO), SECTION_SIZE), ret);
	}
#endif
	plat_sspm_coredump_get = save_sspm_coredump;
	plat_sspm_data_get = save_sspm_data;
	plat_sspm_xfile_get = save_sspm_xfile;
	plat_sspm_log_get = save_sspm_last_log;
#endif
	plat_pllk_last_log_get = save_pllk_last_log;
	plat_mcdi_get = save_mcdi_data;

	plat_dfd20_get = save_dfd_data;
	dfd_op.acquire_ram_control = setup_snoop_filter_ram_ctrl;
	dfd_op.release_ram_control = return_snoop_filter_ram_ctrl;
	circular_buffer_op.lock = circular_buffer_lock;
	circular_buffer_op.unlock = circular_buffer_unlock;

	/* routine tasks */
	plat_write_dram_klog();

	return 1;
}
