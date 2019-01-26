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

#include <stdlib.h>			/* for atoi() */
#include <stdint.h>
#include <string.h>
#include <debug.h>
#include <platform/boot_mode.h>
#include <platform/partition.h>
#include <platform/env.h>
#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#endif
#include <assert.h>
#include <platform/mt_scp.h>
#include <platform/emi_mpu.h>
#include <platform/errno.h>

#define SCP_NO_FIRMWARE         0
#define SCP_SRAM_LOCK_REG       (*(volatile unsigned int *) 0x108A0010)
#define SCP_SRAM_LOCK_UNIT	    12
#define SCP_SRAM_LOCK_SHIFT	    8
#define SCP_JTAG_REG            (*(volatile unsigned int *) 0x108a00A0)
#define SCP_JTAG_EN             0x0000003C
#define SCP_SRAM_EN_LOCK_REG    (*(volatile unsigned int *) 0x108a00DC)
#define SCP_SRAM_EN_REG         (*(volatile unsigned int *) 0x108a00E0)
#define SCP_SRAM_EN_BIT         (1 << 20)
#define SCP_SRAM_PWR_REG        (*(volatile unsigned int *) 0x108a402C)
#define SCP_SET_CLK_CG          (*(volatile unsigned int *) 0x108a4030)

#define SCP_SRAM_BASE           0x10800000
#define SCP_EFUSE_DIS_INDEX           88
#define SCP_ROM_SIZE                  88

extern void dsb(void);
extern int load_scp_image(char *part_name, char *img_name, void *addr);
extern unsigned int get_devinfo_with_index(unsigned int index);


unsigned int get_scp_status(void);
extern u64 physical_memory_size(void);
extern int mboot_common_load_part(char *part_name, char *img_name, unsigned long addr);
/* return 0: success */
extern uint32_t sec_img_auth_init(uint8_t *part_name, uint8_t *img_name, unsigned int unused) ;
/* return 0: success */
extern uint32_t sec_img_auth(uint8_t *img_buf, uint32_t img_buf_sz);
/* return -1: fail; >0 :size*/
extern int verify_load_scp_image(char *part_name, char *loader_name, char *firmware_name, char *loader_name_dual, char *firmware_name_dual, void *addr, unsigned int offset, int scp_A_sram_size);
int platform_scp_set_sram_region(int scp_A_sram_size);
unsigned int platform_fdt_scp_get_sram_size(void);
extern void mtk_wdt_restart(void);
extern BOOT_ARGUMENT *g_boot_arg;
#if defined(CFG_DTB_EARLY_LOADER_SUPPORT)
/*dts header*/
extern void *g_fdt;
#endif

#ifdef MT_DO_SUPPORT
#if defined(CFG_DTB_EARLY_LOADER_SUPPORT)
/* do nothing */
#else
extern void *g_fdt;
#endif
int load_do(char *part_name, void *do_addr);
#endif

static int load_scp_status = 0;
static void *dram_addr;
static char *scp_part_name[] = {"scp1", "scp2"};


int load_scp_image(char *part_name, char *img_name, void *addr)
{
	uint32_t sec_ret;
	uint32_t scp_vfy_time;
	int ret;
#ifdef MTK_SECURITY_SW_SUPPORT
	unsigned int policy_entry_idx = 0;
	unsigned int img_auth_required = 0;
	if (img_name == NULL || addr == NULL) {
		dprintf(CRITICAL, "[SCP]%s(): img_name and address is null\n", __func__);
		return -1;
	}
	policy_entry_idx = get_policy_entry_idx(part_name);
	img_auth_required = get_vfy_policy(policy_entry_idx);
	/* verify cert chain of boot img */
	if (img_auth_required) {
		mtk_wdt_restart();
		scp_vfy_time = get_timer(0);
		sec_ret = sec_img_auth_init(part_name, img_name, 0);
		if (sec_ret)
			return -1;
#ifdef MTK_SECURITY_ANTI_ROLLBACK
		sec_ret = sec_rollback_check(1);
		if (sec_ret) {
			return -1;
		}
#endif
		dprintf(DEBUG, "[SBC] scp cert vfy pass(%d ms)\n", (unsigned int)get_timer(scp_vfy_time));
	}
	dprintf(INFO, "[SCP]%s(): scp in lk security sw support\n", __func__);
#endif
	if (img_name == NULL || addr == NULL) {
		dprintf(CRITICAL, "[SCP]%s(): img_name and address is null\n", __func__);
		return -1;
	}
	ret = mboot_common_load_part(part_name, img_name, addr);

	if (ret <= 0) {
		dprintf(CRITICAL, "[SCP]%s(): ret=%d\n", __func__, ret);
		return -1;
	}
#ifdef MTK_SECURITY_SW_SUPPORT
	if (img_auth_required) {
		mtk_wdt_restart();
		scp_vfy_time = get_timer(0);
		sec_ret = sec_img_auth(addr, ret);
		if (sec_ret)
			return -1;
		dprintf(DEBUG, "[SBC] scp vfy pass(%d ms)\n", (unsigned int)get_timer(scp_vfy_time));
	}
#endif

	return ret;
}

static char *scp_partition_name(void)
{
	int i;
	part_t *part;

	for (i = 0; i < (sizeof(scp_part_name) / sizeof(*scp_part_name)); i++) {
		part = get_part(scp_part_name[i]);
		if (part && mt_part_get_part_active_bit(part))
			return scp_part_name[i];
	}

	dprintf(CRITICAL, "[SCP]no partition with active bit marked, load %s\n", scp_part_name[0]);

	return scp_part_name[0];
}

int load_scp(void)
{
	int ret, fw_size;
	int scp_A_sram_size = 0;
	unsigned int dram_ofs, perm;
	u64 dram_size;
	char *part_name;

	/* check if scp is turned off manually*/
	if (get_scp_status() == 0) {
		ret = -1;
		dprintf(CRITICAL, "[SCP]get_scp_status fail\n");
		goto error;
	}

	dram_addr = (void *) mblock_reserve_ext(&g_boot_arg->mblock_info, SCP_DRAM_SIZE, 0x10000, DRAM_ADDR_MAX, 0, "SCP-reserved");

	dprintf(INFO, "[SCP]%s(): dram_addr=%p\n", __func__, dram_addr);

	if (dram_addr == ((void *) 0))
		goto error;

	dram_size = physical_memory_size();

	/*
	 * scp dram mapping offset
	 */
	dram_ofs = 0;

	part_name = scp_partition_name();
	dprintf(INFO, "[SCP]part_name=%s\n", part_name);
#if defined(CFG_DTB_EARLY_LOADER_SUPPORT)
	scp_A_sram_size = platform_fdt_scp_get_sram_size();
	if(scp_A_sram_size < 0)
		return -1;
	dprintf(INFO, "[SCP]scp_A_sram_size=%d\n",scp_A_sram_size);
#else
	/*bug on, lk not support fdt early load*/
#endif
	if (part_name) {
		ret = platform_scp_set_sram_region(scp_A_sram_size);
		if (ret < 0) {
			dprintf(CRITICAL, "[SCP]platform_scp_set_sram_region fail\n");
			goto error;
		}
		ret = verify_load_scp_image(part_name, LOADER_NAME_SCP_A, FIRMWARE_NAME_SCP_A, LOADER_NAME_SCP_B, FIRMWARE_NAME_SCP_B, dram_addr, dram_ofs, scp_A_sram_size);
		if (ret < 0) {
			dprintf(CRITICAL, "[SCP]verify %s failed, ret=%d(-1: load image fail, -5:sram size<0, -6:sram size not 32*N)\n", part_name, ret);
			goto error;
		}
		/* verify scp boot img size*/
		if (ret > SCP_DRAM_SIZE) {
			dprintf(CRITICAL, "[SCP]image size > reserved, size = %d\n", ret);
			goto error;
		}
		fw_size = ret;
		dprintf(INFO, "[SCP]image size = %d\n", fw_size);
#ifdef MT_DO_SUPPORT
		/* load DO bin from partition to the end of scp images */
		ret = load_do(part_name, dram_addr + (size_t)fw_size);
		if (ret < 0) {
			dprintf(CRITICAL, "[DO] load DO failed\n");
			goto error;
		} else {
			dprintf(CRITICAL, "[DO] load DO done, A + B size = %d\n", ret);
		}
		/* verify DO img size*/
		if ((ret + fw_size) > SCP_DRAM_SIZE) {
			dprintf(CRITICAL, "[DO] image + DO size > reserved, size = %d\n", ret + fw_size);
			goto error;
		}
#endif

	} else {
		ret = -1;
		dprintf(CRITICAL, "[SCP]get partition failed\n");
		goto error;
	}

	/*clean dcache & icache before set up EMI MPU*/
	arch_sync_cache_range((addr_t)dram_addr, SCP_DRAM_SIZE);

	/*
	 * setup EMI MPU
	 * domain 0: AP
	 * domain 3: SCP
	 */
#if ENABLE_SCP_EMI_PROTECTION
	perm = SET_ACCESS_PERMISSON(UNLOCK,
	FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
	FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN, NO_PROTECTION, FORBIDDEN, FORBIDDEN, NO_PROTECTION);
	emi_mpu_set_region_protection(dram_addr, dram_addr + SCP_DRAM_SIZE - 1, SCP_EMI_REGION, perm);
#endif
	dprintf(INFO, "%s(): done\n", __func__);

	load_scp_status = 1;

	return 0;

error:
	/*
	 * @ret = 0, malloc() error
	 * @ret < 0, error code from load_scp_image()
	 */
	load_scp_status = -1;

	return -1;
}

#ifdef MT_DO_SUPPORT
#define NEXT_DO_OFFSET 8 /* to prevent scp A parsing the DOs of scp B */
/*
 * Load Dynamic Object from partition to DRAM
 * @ret: > 0 : load success, return size of total loaded DO
 *       = 0 : no DO image in partition or DO DRAM addr invalid
 */
int load_do(char *part_name, void *do_addr)
{
	int ret, total_loaded = 0;
	void *tmp;
	void *addr_A = 0;
	void *addr_B = 0;

	dprintf(CRITICAL, "[DO] try to load scp A DO to 0x%p\n", do_addr);
	ret = mboot_common_load_part(part_name, DO_NAME_SCP_A, do_addr);
	if (ret >= 0) {
		addr_A = do_addr;
		total_loaded += ret;
	} else if (ret == -EINVAL) {
		dprintf(CRITICAL, "[DO] mboot_common_load_part: cannot find scp A DO\n");
	} else {
		dprintf(CRITICAL, "[DO] mboot_common_load_part: load scp A error: %d\n", ret);
	}

	tmp = do_addr + (size_t)total_loaded + NEXT_DO_OFFSET;

	dprintf(CRITICAL, "[DO] try to load scp B DO to 0x%p\n", tmp);
	ret = mboot_common_load_part(part_name, DO_NAME_SCP_B, tmp);
	if (ret >= 0) {
		addr_B = tmp;
		total_loaded += ret;
	} else if (ret == -EINVAL) {
		dprintf(CRITICAL, "[DO] mboot_common_load_part: cannot find scp B DO\n");
	} else {
		dprintf(CRITICAL, "[DO] mboot_common_load_part: load scp B error: %d\n", ret);
	}

	/* save DO head addresses to device tree */
	/* NOTE: address will be 0 if the DO bin does not exist */
	save_do_addr(addr_A, addr_B);

	return total_loaded;
}
#endif

#ifdef DEVICE_TREE_SUPPORT
int platform_fdt_scp(void *fdt)
{
	int nodeoffset;
	char *ret;

	dprintf(INFO, "%s()\n", __func__);
	nodeoffset = fdt_node_offset_by_compatible(fdt, -1, "mediatek,scp");

	if (nodeoffset >= 0) {
		if (load_scp_status <= 0)
			ret = "fail";
		else
			ret = "okay";

		dprintf(CRITICAL, "[SCP]status=%s,%d\n", ret, load_scp_status);

		fdt_setprop(fdt, nodeoffset, "status", ret, strlen(ret));

		return 0;
	}

	return 1;
}
#if defined(CFG_DTB_EARLY_LOADER_SUPPORT)
/* Load SCP sram size from dts */
unsigned int platform_fdt_scp_get_sram_size(void)
{
	int nodeoffset;
	unsigned int *data = NULL;
	int len = 0;

	dprintf(INFO, "%s()\n", __func__);
	nodeoffset = fdt_node_offset_by_compatible(g_fdt, -1, "mediatek,scp");

	if (nodeoffset >= 0) {
		data = (unsigned int*)fdt_getprop(g_fdt, nodeoffset, "scp_sramSize", &len);
		if (data == NULL) {
			dprintf(CRITICAL, "scp get dts info fail\n");
			return -1;
		}

		dprintf(INFO, "scp size=%d\n", fdt32_to_cpu(*(unsigned int *)data));
		dprintf(INFO, "scp size len=%d\n", len);
		return fdt32_to_cpu(*(unsigned int *)data);
	} else {
		dprintf(CRITICAL, "mediatek,scp not found!\n");
		/*bug on, assert fail*/
	}

	return -1;
}

#endif
#endif

#ifdef MT_DO_SUPPORT
void save_do_addr(void *do_addr_A, void *do_addr_B)
{
	int offset;
	u32 addr[2];

	dprintf(INFO, "%s()\n", __func__);

	offset = fdt_node_offset_by_compatible(g_fdt, -1, "mediatek,scp");
	if (offset >= 0) {

		addr[0] = (u32)cpu_to_fdt32((u64)do_addr_A >> 32);
		addr[1] = (u32)cpu_to_fdt32(do_addr_A);
		fdt_setprop(g_fdt, offset, "do_addr_A", addr, sizeof(u32)*2);

		addr[0] = (u32)cpu_to_fdt32((u64)do_addr_B >> 32);
		addr[1] = (u32)cpu_to_fdt32(do_addr_B);
		fdt_setprop(g_fdt, offset, "do_addr_B", addr, sizeof(u32)*2);

		dprintf(CRITICAL, "[DO] save DO addrs to DT, A: 0x%x, B: 0x%x\n",
				(u32)do_addr_A, (u32)do_addr_B);
	} else {
		dprintf(CRITICAL, "[DO]ERR: DT mediatek,scp not found %s\n", offset);
	}
}
#endif

unsigned int get_scp_status(void)
{
	unsigned int scp_status;
	char *scp_env = get_env("scp");

	/*if there is no env[scp], scp should be still enabled*/
	scp_status = (scp_env == NULL) ? 1 : atoi(scp_env);
	dprintf(CRITICAL,"[SCP] current setting is %d.\n", scp_status);
	return scp_status;
}

unsigned int set_scp_status(unsigned int en)
{
	char *SCP_STATUS[2] = {"0","1"};

	if (set_env("scp", SCP_STATUS[en]) == 0) {
		dprintf(CRITICAL,"[SCP]set SCP %s success. Plz reboot to make it applied.\n",SCP_STATUS[en]);
		return 0;
	} else {
		dprintf(CRITICAL,"[SCP]set SCP %s fail.\n",SCP_STATUS[en]);
		return 1;
	}
}

/******************************************************************************
******************************************************************************/
unsigned int get_scp_log_thru_ap_uart(void)
{
	unsigned int enable = 0;
	char *env = get_env("scp_ap_uart");

	if (env == NULL) {
		dprintf(CRITICAL, "[SCP] Get SCP log thru AP UART: 0 (default)\n");
	} else {
		enable = atoi(env);
		dprintf(CRITICAL, "[SCP] Get SCP log thru AP UART: %d\n", enable);
	}
	return enable;
}


/******************************************************************************
* Caller must make sure that the argument "en" is less than 2.
******************************************************************************/
void set_scp_log_thru_ap_uart(unsigned int en)
{
	char *en_array[2] = {"0", "1"};

	if (set_env("scp_ap_uart", en_array[en]) == 0) {
		dprintf(CRITICAL, "[SCP] Set SCP log thru AP UART: %s\n", en_array[en]);
	} else {
		dprintf(CRITICAL, "[SCP] Failed to set SCP log thru AP UART to %s!\n",
			en_array[en]);
	}
}

/*config scp dual size setting
 *return -1: setting fail
 *        0: success
 */
int platform_scp_set_sram_region(int scp_A_sram_size) {

	/*verify scp sram size setting*/
	if(scp_A_sram_size < 0) {
		dprintf(CRITICAL,"[SCP]scp_A_sram_size < 0\n");
		return -1;
	}
	/*sram size must be 32k * N*/
	if((scp_A_sram_size % SCP_REGION) != 0) {
		dprintf(CRITICAL,"[SCP]scp_A_sram_size % 32k != 0\n");
		return -1;
	}
	/*  Set scp sram regions
	 *  SRAM         Clk region
	 *  0x08000      0x1F
	 *  0x10000      0x1E
	 *  0x18000      0x1D
	 *  ...          ...
	 */
	return 0;
}


int verify_load_scp_image(char *part_name, char *loader_name, char *firmware_name, char *loader_name_dual, char *firmware_name_dual, void *addr, unsigned int offset, int scp_A_sram_size)
{
	int ret = 0;
	int scp_image_size = 0;
    uint32_t *info;
	void *loader, *firmware;
	void *loader_dual, *firmware_dual;
	uint32_t reg_temp;
	size_t ld_size, fw_size;
	size_t ld_dual_size, fw_dual_size;

	//enable sram
    SCP_SRAM_PWR_REG = 0x0;

	//clock SCP and SCP dual clock on
	reg_temp = SCP_SET_CLK_CG | 0x00300000;
	SCP_SET_CLK_CG = reg_temp;

    // step 1: verify loader
	loader = addr;
	ret = load_scp_image(part_name, loader_name, loader);

	if (ret <= 0)
		return -1;

	ld_size = ret;
    // step 2: verify firmware
	firmware = loader + ld_size;

	ret = load_scp_image(part_name, firmware_name, firmware);

	if (ret <= 0)
		return -2;

	fw_size = ret;

	// step 5: get scp image size
	scp_image_size = (ld_size + fw_size);

    // step 6: copy loader to sram
    memcpy((void *) SCP_SRAM_BASE, loader, ld_size);
#if !SCP_NO_FIRMWARE
	// step 8: copy firmware to dram
	memcpy(addr, firmware, fw_size);
#else
	fw_size = 0x3f800;	// 254K
	fw_dual_size = 0x7f800;	// 510K
#endif

    // step 10: copy firmware info to sram
    info = (uint32_t *) (SCP_SRAM_BASE + ld_size);
    *(info++) = (uint32_t) addr + offset;
    *info = (uint32_t) fw_size;
/*
    // step 12: lock sram
	ret = ld_size + fw_size;
	ret >>= SCP_SRAM_LOCK_UNIT; //shift 12 bit for computing by unit 4k
	ret++; //add more 4k
	ret <<= SCP_SRAM_LOCK_SHIFT;
	//to do: return scp image size unit > 0xff
    SCP_SRAM_LOCK_REG = ret;
    SCP_SRAM_EN_REG |= SCP_SRAM_EN_BIT;
*/
    dsb();

/*
	if (scp_force_lock_sram())
		SCP_SRAM_EN_LOCK_REG |= SCP_SRAM_EN_BIT; //enable one time lock
*/
    // step 8: enable jtag
    SCP_JTAG_REG |= SCP_JTAG_EN;

    dsb();

    return scp_image_size;
}
