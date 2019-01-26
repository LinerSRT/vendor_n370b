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
#include <sys/types.h>
#include <stdint.h>
#include <platform/partition.h>
#include <platform/mt_typedefs.h>
#include <platform/boot_mode.h>
#include <platform/mt_reg_base.h>
#include <platform/errno.h>
#include <printf.h>
#include <string.h>
#include <malloc.h>
#include <libfdt.h>
#include <platform/mt_gpt.h>
#include <platform/mt_emi_mpu.h>
#include <debug.h>
#define MODULE_NAME "LK_LD_MD"
#include "ccci_ld_md_core.h"
#include "ccci_ld_md_errno.h"

unsigned char * ccb_data_buf;

#define CCCI_SMEM_SIZE_RAW_DHL (16*1024*1024)
#define CCCI_SMEM_SIZE_LWA (0) // (8*1024*1024)
#define CCB_DATA_BUF_SIZE (48*1024*1024 + CCCI_SMEM_SIZE_RAW_DHL + CCCI_SMEM_SIZE_LWA)

/***************************************************************************************************
** Feature Option setting part
***************************************************************************************************/
#define ENABLE_EMI_PROTECTION

/***************************************************************************************************
** HW remap section
***************************************************************************************************/
extern unsigned int ddr_enable_4gb(void)__attribute__((weak));
static int is_4gb_ddr_support_en(void)
{
	int ret;
	if (ddr_enable_4gb) {
		ret = ddr_enable_4gb();
		ALWAYS_LOG("ddr_enable_4GB sta:%d\n", ret);
		return ret;
	} else {
		ALWAYS_LOG("ddr 4GB disable\n");
		return 0;
	}
}

/*-------- Register base part -------------------------------*/
/* HW remap for MD1 */

#define INFRA_AO_BASE	(0x10000000)
/* -- MD1 Bank 0 */
#define MD1_BANK0_MAP0 (INFRA_AO_BASE + 0x300)
#define MD1_BANK0_MAP1 (INFRA_AO_BASE + 0x304)
#define MD1_BANK0_MAP2 (INFRA_AO_BASE + 0x308)
#define MD1_BANK0_MAP3 (INFRA_AO_BASE + 0x30C)
/* -- MD1 Bank 1 */
#define MD1_BANK1_MAP0 (INFRA_AO_BASE + 0x310)
#define MD1_BANK1_MAP1 (INFRA_AO_BASE + 0x314)
#define MD1_BANK1_MAP2 (INFRA_AO_BASE + 0x318)
#define MD1_BANK1_MAP3 (INFRA_AO_BASE + 0x31C)

/* -- MD1 Bank 4 */
#define MD1_BANK4_MAP0 (INFRA_AO_BASE + 0x320)
#define MD1_BANK4_MAP1 (INFRA_AO_BASE + 0x324)
#define MD1_BANK4_MAP2 (INFRA_AO_BASE + 0x328)
#define MD1_BANK4_MAP3 (INFRA_AO_BASE + 0x32C)

/* HW remap for MD3(C2K) */
/* -- MD3 Bank 0 */
#define MD2_BANK0_MAP0 (INFRA_AO_BASE + 0x330)
#define MD2_BANK0_MAP1 (INFRA_AO_BASE + 0x334)
#define MD2_BANK0_MAP2 (INFRA_AO_BASE + 0x338)
#define MD2_BANK0_MAP3 (INFRA_AO_BASE + 0x33C)

/* -- MD3 Bank 4 */
#define MD2_BANK4_MAP0 (INFRA_AO_BASE + 0x350)
#define MD2_BANK4_MAP1 (INFRA_AO_BASE + 0x354)
#define MD2_BANK4_MAP2 (INFRA_AO_BASE + 0x358)
#define MD2_BANK4_MAP3 (INFRA_AO_BASE + 0x35C)

/* HW remap lock register */
#define MD_HW_REMAP_LOCK (0x10000F80)
#define MD1_LOCK         (1<<16)
#define MD3_LOCK         (1<<17)

static int md_mem_ro_rw_remapping(unsigned int md_id, unsigned long long addr)
{
	unsigned long long md_img_start_addr;
	unsigned int hw_remapping_bank0_map0 = 0;
	unsigned int hw_remapping_bank0_map1 = 0;
	unsigned int hw_remapping_bank0_map2 = 0;
	unsigned int hw_remapping_bank0_map3 = 0;
	unsigned int write_val;

	switch (md_id) {
		case 0: // MD1
			hw_remapping_bank0_map0 = MD1_BANK0_MAP0;
			hw_remapping_bank0_map1 = MD1_BANK0_MAP1;
			hw_remapping_bank0_map2 = MD1_BANK0_MAP2;
			hw_remapping_bank0_map3 = MD1_BANK0_MAP3;
			break;
		case 2: // MD3
			hw_remapping_bank0_map0 = MD2_BANK0_MAP0;
			hw_remapping_bank0_map1 = MD2_BANK0_MAP1;
			hw_remapping_bank0_map2 = MD2_BANK0_MAP2;
			hw_remapping_bank0_map3 = MD2_BANK0_MAP3;
			break;
		default:
			ALWAYS_LOG("Invalid md id:%d\n", md_id);
			return -1;
	}

	md_img_start_addr = addr;
	ALWAYS_LOG("---> Map 0x00000000 to 0x%llx for MD%d\n", addr, md_id+1);

	/* For MDx_BANK0_MAP0 */
	write_val = (((md_img_start_addr >> 24) | 0x1) & 0x3FF)
		+ ((((md_img_start_addr + 0x2000000*1) >> 8) | 1<<16) & 0x3FF0000);
	DRV_WriteReg32(hw_remapping_bank0_map0, write_val);
	ALWAYS_LOG("BANK0_MAP0 value:0x%X\n", DRV_Reg32(hw_remapping_bank0_map0));
	/* For MDx_BANK0_MAP1 */
	write_val = ((((md_img_start_addr + 0x2000000*2) >> 24) | 0x1) & 0x3FF)
		+ ((((md_img_start_addr + 0x2000000*3) >> 8) | 1<<16) & 0x3FF0000);
	DRV_WriteReg32(hw_remapping_bank0_map1, write_val);
	ALWAYS_LOG("BANK0_MAP1 value:0x%X\n", DRV_Reg32(hw_remapping_bank0_map1));
	/* For MDx_BANK0_MAP2 */
	write_val = ((((md_img_start_addr + 0x2000000*4) >> 24) | 0x1) & 0x3FF)
		+ ((((md_img_start_addr + 0x2000000*5) >> 8) | 1<<16) & 0x3FF0000);
	DRV_WriteReg32(hw_remapping_bank0_map2, write_val);
	ALWAYS_LOG("BANK0_MAP2 value:0x%X\n", DRV_Reg32(hw_remapping_bank0_map2));
	/* For MDx_BANK0_MAP3 */
	write_val = ((((md_img_start_addr + 0x2000000*6) >> 24) | 0x1) & 0x3FF)
		+ ((((md_img_start_addr + 0x2000000*7) >> 8) | 1<<16) & 0x3FF0000);
	DRV_WriteReg32(hw_remapping_bank0_map3, write_val);
	ALWAYS_LOG("BANK0_MAP3 value:0x%X\n", DRV_Reg32(hw_remapping_bank0_map3));

#ifdef DUMMY_AP_MODE
	/* For 256~512MB */
	if (md_id == MD_SYS1) {
		write_val = ((((md_img_start_addr + 0x2000000*8) >> 24) | 0x1) & 0x3FF)
			+ ((((md_img_start_addr + 0x2000000*9) >> 8) | 1<<16) & 0x3FF0000);
		DRV_WriteReg32(MD1_BANK1_MAP0, write_val);
		ALWAYS_LOG("BANK1_MAP0 value:0x%X\n", DRV_Reg32(MD1_BANK1_MAP0));

		write_val = ((((md_img_start_addr + 0x2000000*10) >> 24) | 0x1) & 0x3FF)
			+ ((((md_img_start_addr + 0x2000000*11) >> 8) | 1<<16) & 0x3FF0000);
		DRV_WriteReg32(MD1_BANK1_MAP1, write_val);
		ALWAYS_LOG("BANK1_MAP1 value:0x%X\n", DRV_Reg32(MD1_BANK1_MAP1));
		write_val = ((((md_img_start_addr + 0x2000000*12) >> 24) | 0x1) & 0x3FF)
			+ ((((md_img_start_addr + 0x2000000*13) >> 8) | 1<<16) & 0x3FF0000);
		DRV_WriteReg32(MD1_BANK1_MAP2, write_val);
		ALWAYS_LOG("BANK1_MAP2 value:0x%X\n", DRV_Reg32(MD1_BANK1_MAP2));

		write_val = ((((md_img_start_addr + 0x2000000*14) >> 24) | 0x1) & 0x3FF)
		+ ((((md_img_start_addr + 0x2000000*15) >> 8) | 1<<16) & 0x3FF0000);
		DRV_WriteReg32(MD1_BANK1_MAP3, write_val);
		ALWAYS_LOG("BANK1_MAP3 value:0x%X\n", DRV_Reg32(MD1_BANK1_MAP3));
	}
#endif

	return 0;
}

static int md_smem_rw_remapping(unsigned int md_id, unsigned long long addr)
{
	unsigned long long md_smem_start_addr, ccb_data_addr;
	unsigned int hw_remapping_bank4_map0 = 0;
	unsigned int hw_remapping_bank4_map1 = 0;
	unsigned int hw_remapping_bank4_map2 = 0;
	unsigned int hw_remapping_bank4_map3 = 0;
	unsigned int write_val;

	switch (md_id) {
		case 0: // MD1
			hw_remapping_bank4_map0 = MD1_BANK4_MAP0;
			hw_remapping_bank4_map1 = MD1_BANK4_MAP1;
			hw_remapping_bank4_map2 = MD1_BANK4_MAP2;
			hw_remapping_bank4_map3 = MD1_BANK4_MAP3;
			break;
		case 2: // MD3
			hw_remapping_bank4_map0 = MD2_BANK4_MAP0;
			hw_remapping_bank4_map1 = MD2_BANK4_MAP1;
			hw_remapping_bank4_map2 = MD2_BANK4_MAP2;
			hw_remapping_bank4_map3 = MD2_BANK4_MAP3;
			break;
		default:
			ALWAYS_LOG("Invalid md id:%d\n", md_id);
			return -1;
	}

	md_smem_start_addr = addr;
	ALWAYS_LOG("---> Map 0x40000000 to 0x%llx for MD%d\n", addr, md_id+1);

	/* For MDx_BANK4_MAP0 */
	write_val = (((md_smem_start_addr >> 24) | 0x1) & 0x3FF)
		+ ((((md_smem_start_addr + 0x2000000*1) >> 8) | 1<<16) & 0x3FF0000);
	DRV_WriteReg32(hw_remapping_bank4_map0, write_val);
	ALWAYS_LOG("BANK4_MAP0 value:0x%X\n", DRV_Reg32(hw_remapping_bank4_map0));
	/* For MDx_BANK4_MAP1 */
	write_val = ((((md_smem_start_addr + 0x2000000*2) >> 24) | 0x1) & 0x3FF)
		+ ((((md_smem_start_addr + 0x2000000*3) >> 8) | 1<<16) & 0x3FF0000);
	DRV_WriteReg32(hw_remapping_bank4_map1, write_val);
	ALWAYS_LOG("BANK4_MAP1 value:0x%X\n", DRV_Reg32(hw_remapping_bank4_map1));


	if (md_id == 0) {
		/*ccb data memory*/
		ccb_data_addr = (unsigned int)ccb_data_buf;
		ALWAYS_LOG("---> Map 0x40000000+128M to 0x%x for MD%d\n", (unsigned int)ccb_data_buf, md_id+1);

		write_val = ((((ccb_data_addr) >> 24) | 0x1) & 0x3FF)
			+ ((((ccb_data_addr + 0x2000000*1) >> 8) | 1<<16) & 0x3FF0000);
		DRV_WriteReg32(hw_remapping_bank4_map2, write_val);
		ALWAYS_LOG("BANK4_MAP2 value:0x%X\n", DRV_Reg32(hw_remapping_bank4_map2));
		/* For MDx_BANK4_MAP3 */
		write_val = ((((ccb_data_addr + 0x2000000*2) >> 24) | 0x1) & 0x3FF)
			+ ((((ccb_data_addr + 0x2000000*3) >> 8) | 1<<16) & 0x3FF0000);
		DRV_WriteReg32(hw_remapping_bank4_map3, write_val);
		ALWAYS_LOG("BANK4_MAP3 value:0x%X\n", DRV_Reg32(hw_remapping_bank4_map3));
	} else {
		/* For MDx_BANK4_MAP2 */
		write_val = ((((md_smem_start_addr + 0x2000000*4) >> 24) | 0x1) & 0x3FF)
			+ ((((md_smem_start_addr + 0x2000000*5) >> 8) | 1<<16) & 0x3FF0000);
		DRV_WriteReg32(hw_remapping_bank4_map2, write_val);
		ALWAYS_LOG("BANK4_MAP2 value:0x%X\n", DRV_Reg32(hw_remapping_bank4_map2));
		/* For MDx_BANK4_MAP3 */
		write_val = ((((md_smem_start_addr + 0x2000000*6) >> 24) | 0x1) & 0x3FF)
			+ ((((md_smem_start_addr + 0x2000000*7) >> 8) | 1<<16) & 0x3FF0000);
		DRV_WriteReg32(hw_remapping_bank4_map3, write_val);
		ALWAYS_LOG("BANK4_MAP3 value:0x%X\n", DRV_Reg32(hw_remapping_bank4_map3));
	}

	return 0;
}

static void md_emi_remapping_lock(unsigned int md_id)
{
	unsigned int reg_val;
	unsigned int lock_bit;

	switch (md_id) {
		case 0: // MD1
			lock_bit = MD1_LOCK;
			break;
		case 2: // MD3
			lock_bit = MD3_LOCK;
			break;
		default:
			ALWAYS_LOG("Invalid md id:%d for lock\n", md_id);
			return;
	}

	reg_val = DRV_Reg32(MD_HW_REMAP_LOCK);
	ALWAYS_LOG("before hw remap lock: MD1[%d], MD3[%d]\n", !!(reg_val&MD1_LOCK), !!(reg_val&MD3_LOCK));
	DRV_WriteReg32(MD_HW_REMAP_LOCK, (reg_val|lock_bit));
	reg_val = DRV_Reg32(MD_HW_REMAP_LOCK);
	ALWAYS_LOG("before hw remap lock: MD1[%d], MD3[%d]\n", !!(reg_val&MD1_LOCK), !!(reg_val&MD3_LOCK));
}

/* =================================================== */
/* MPU Region defination                               */
/* =================================================== */
/* Note: This structure should sync with Kernel!!!!    */
typedef struct _mpu_cfg {
	unsigned int start;
	unsigned int end;
	int region;
	unsigned int permission[EMI_MPU_DGROUP_NUM];
	int relate_region;
} mpu_cfg_t;

#define MPU_REGION_ID_SEC_OS            0
#define MPU_REGION_ID_ATF               1
#define MPU_REGION_ID_SVP_SMEM          2
#define MPU_TINYSYS_SSPM_ROM            3
#define MPU_TINYSYS_SSPM_SMEM           4
#define MPU_TINYSYS_SCP_ROM             5
#define MPU_TINYSYS_SCP_SMEM            6
#define MPU_REGION_ID_TRUSTED_UI        7
#define MPU_REGION_ID_MD_DYNAMIC        8
#define MPU_REGION_ID_MD1_SEC_SMEM      9
#define MPU_REGION_ID_MD3_SEC_SMEM      10

#define MPU_REGION_ID_MD1_SMEM          11
#define MPU_REGION_ID_MD3_SMEM          12
#define MPU_REGION_ID_MD1MD3_SMEM       13
#define MPU_REGION_ID_MD1_MCURW_HWRW    14
#define MPU_REGION_ID_MD1_ROM           15  /* contain DSP in Jade */
#define MPU_REGION_ID_MD1_MCURW_HWRO    16
#define MPU_REGION_ID_MD1_MCURO_HWRW    17
#define MPU_REGION_ID_WIFI_EMI_FW       18
#define MPU_REGION_ID_WMT               19
#define MPU_REGION_ID_MD1_CCB           20
#define MPU_REGION_ID_MD3_ROM           21
#define MPU_REGION_ID_MD3_RW            22
#define MPU_REGION_ID_PADDING1          23
#define MPU_REGION_ID_PADDING2          24
#define MPU_REGION_ID_PADDING3          25
#define MPU_REGION_ID_PADDING4          26
#define MPU_REGION_ID_PADDING5          27
#define MPU_REGION_ID_MD_PROTECT        28

#define MPU_REGION_ID_GPS_OFFLOAD       30
#define MPU_REGION_ID_AP                31
#define MPU_REGION_ID_TOTAL_NUM         (MPU_REGION_ID_AP + 1)

#define MPU_MDOMAIN_ID_AP       0
#define MPU_MDOMAIN_ID_MD       1
#define MPU_MDOMAIN_ID_MD3      5
#define MPU_MDOMAIN_ID_MDHW     7
#define MPU_MDOMAIN_ID_TOTAL_NUM    16

static const unsigned int mpu_att_default[MPU_REGION_ID_TOTAL_NUM][MPU_MDOMAIN_ID_TOTAL_NUM] = {
	/*===================================================================================================================*/
	/* No |  | D0(AP)    | D1(MD1)      | D2(CONN) | D3(SCP)  | D4(MM)       | D5(MD3 )      | D6(MFG)      | D7(MDHW)  | D8(SSPM)  | D9(SPM)  | D10~D15(N/A)  */
	/*--------------+----------------------------------------------------------------------------------------------------*/
	/* 0*/{ SEC_RW       , FORBIDDEN    , FORBIDDEN    , FORBIDDEN, SEC_RW       , FORBIDDEN, FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in Secure OS*/
	/* 1*/{ SEC_RW       , FORBIDDEN    , FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in ATF*/
	/* 2*/{ SEC_RW       , FORBIDDEN    , FORBIDDEN    , FORBIDDEN, SEC_RW       , FORBIDDEN, FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Secure Memory*/
	/* 3*/{ FORBIDDEN    , FORBIDDEN    , FORBIDDEN    , FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN    , FORBIDDEN, NO_PROTECTION, FORBIDDEN, [10 ... 15] = FORBIDDEN}, /*Tinysys-SSPM ROM*/
	/* 4*/{ NO_PROTECTION, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN    , FORBIDDEN, NO_PROTECTION, FORBIDDEN, [10 ... 15] = FORBIDDEN}, /*Tinysys-SSPM share buffer*/
  /* 5*/{ FORBIDDEN    , FORBIDDEN    , FORBIDDEN    , SEC_R_NSEC_R,  FORBIDDEN, FORBIDDEN, FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Tinysys-SCP ROM*/
	/* 6*/{ NO_PROTECTION, FORBIDDEN    , FORBIDDEN    , NO_PROTECTION, FORBIDDEN, FORBIDDEN, FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Tinysys-SCP share buffer*/
	/* 7*/{ SEC_RW       , FORBIDDEN		, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in TEE Trust UI*/
	/* 8*/{ FORBIDDEN    , NO_PROTECTION, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in LK MD dynamic*/
	/* 9*/{ SEC_RW       , NO_PROTECTION, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in TEE MD1 SEC MEM*/
	/*10*/{ SEC_RW       , NO_PROTECTION, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in TEE MD1 SEC MEM*/
	/*11*/{ NO_PROTECTION, NO_PROTECTION, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in LK CCCI_MD1*/
	/*12*/{ NO_PROTECTION, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, FORBIDDEN    , NO_PROTECTION, FORBIDDEN, FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in LK CCCI_MD3*/
	/*13*/{ NO_PROTECTION, NO_PROTECTION, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , NO_PROTECTION, FORBIDDEN, FORBIDDEN, [8 ... 15] = FORBIDDEN},/*Set in LK MD1_MD3*/
	/*14*/{ SEC_R_NSEC_R , NO_PROTECTION, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, NO_PROTECTION, [8 ... 15] = FORBIDDEN}, /*Set in LK MD1_MCURW_HWRW*/
	/*15*/{ SEC_R_NSEC_R , SEC_R_NSEC_R , FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, SEC_R_NSEC_R , [8 ... 15] = FORBIDDEN}, /*Set in LK MD1_ROM_DSP*/
	/*16*/{ SEC_R_NSEC_R , NO_PROTECTION, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, SEC_R_NSEC_R , [8 ... 15] = FORBIDDEN}, /*Set in LK MD1_MCURW_HWRO*/
	/*17*/{ SEC_R_NSEC_R , SEC_R_NSEC_R , FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, NO_PROTECTION, [8 ... 15] = FORBIDDEN}, /*Set in LK MD1_MCURO_HWRW*/
	/*18*/{ FORBIDDEN    , FORBIDDEN    , NO_PROTECTION, FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in Kernel WIFI EMI FW*/
	/*19*/{ NO_PROTECTION, FORBIDDEN    , NO_PROTECTION, FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in Kernel WMT*/
	/*20*/{ NO_PROTECTION, NO_PROTECTION, FORBIDDEN    , FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, NO_PROTECTION, [8 ... 15] = FORBIDDEN}, /*Set in MD1_CCB*/
	/*21*/{ SEC_R_NSEC_R , FORBIDDEN    , FORBIDDEN    , FORBIDDEN, FORBIDDEN    , SEC_R_NSEC_R , FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in LK MD3 ROM*/
	/*22*/{ SEC_R_NSEC_R , FORBIDDEN    , FORBIDDEN    , FORBIDDEN, FORBIDDEN    , NO_PROTECTION    , FORBIDDEN, FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in LK MD3 R/W*/
	/*23*/{ NO_PROTECTION, NO_PROTECTION , FORBIDDEN   , FORBIDDEN, FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in LK MD Padding1*/
	/*24*/{ NO_PROTECTION, NO_PROTECTION , FORBIDDEN   , FORBIDDEN, FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in LK MD Padding2*/
	/*25*/{ NO_PROTECTION, NO_PROTECTION , FORBIDDEN   , FORBIDDEN, FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in LK MD Padding3*/
	/*26*/{ NO_PROTECTION, NO_PROTECTION , FORBIDDEN   , FORBIDDEN, FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in LK MD Padding4*/
	/*27*/{ NO_PROTECTION, NO_PROTECTION , FORBIDDEN   , FORBIDDEN, FORBIDDEN, FORBIDDEN    , FORBIDDEN    , FORBIDDEN, [8 ... 15] = FORBIDDEN}, /*Set in LK MD Padding5*/
	/*28*/{ NO_PROTECTION, SEC_R_NSEC_R , FORBIDDEN    , FORBIDDEN, NO_PROTECTION, FORBIDDEN, SEC_R_NSEC_RW, FORBIDDEN, NO_PROTECTION, NO_PROTECTION, [10 ... 15] = FORBIDDEN}, /*Set in LK MD Protect*/

	/*30*/{ NO_PROTECTION, FORBIDDEN , NO_PROTECTION   , FORBIDDEN, FORBIDDEN, FORBIDDEN    , FORBIDDEN, FORBIDDEN, [8 ... 15] = FORBIDDEN},/*Set in GPS OFFLOAD*/
	/*31*/{ NO_PROTECTION, FORBIDDEN , FORBIDDEN    , FORBIDDEN, NO_PROTECTION, FORBIDDEN   , SEC_R_NSEC_RW, FORBIDDEN, NO_PROTECTION, NO_PROTECTION, [10 ... 15] = FORBIDDEN},/*Set in Kernel AP*/
}; /*=================================================================================================================*/

#define MPU_STR_BUF_SIZE 64

static void get_mpu_attr_str(int lock, unsigned int apc[EMI_MPU_DGROUP_NUM], char buf[], int size)
{
	unsigned long long curr_attr = ((unsigned long long)apc[1] << 32) | apc[0];
	char ch = lock?'L':'U';
	snprintf(buf, size, "%lld-%lld-%lld-%lld-%lld-%lld-%lld-%lld-%lld-%lld-%lld-%lld-%lld-%lld-%lld-%lld(%c)",
	         curr_attr&7, (curr_attr>>3)&7, (curr_attr>>6)&7, (curr_attr>>9)&7,
	         (curr_attr>>12)&7, (curr_attr>>15)&7, (curr_attr>>18)&7, (curr_attr>>21)&7,
	         (curr_attr>>32)&7, (curr_attr>>35)&7, (curr_attr>>38)&7, (curr_attr>>41)&7,
	         (curr_attr>>44)&7, (curr_attr>>47)&7, (curr_attr>>50)&7, (curr_attr>>53)&7, ch);
}

static const unsigned char region_mapping_at_hdr_md1[] = {
	MPU_REGION_ID_MD1_ROM, MPU_REGION_ID_MD1_MCURO_HWRW, MPU_REGION_ID_MD1_MCURW_HWRO,
	MPU_REGION_ID_MD1_MCURW_HWRW
};

static const int free_mpu_region[] = {MPU_REGION_ID_PADDING1, MPU_REGION_ID_PADDING2,
	MPU_REGION_ID_PADDING3, MPU_REGION_ID_PADDING4, MPU_REGION_ID_PADDING5, MPU_REGION_ID_MD_PROTECT, -1};
static int curr_free_mpu_idx;
static int get_free_mpu_region(void)
{
	int ret;
	if (curr_free_mpu_idx < (int)(sizeof(free_mpu_region)/sizeof(int))) {
		ret = free_mpu_region[curr_free_mpu_idx];
		curr_free_mpu_idx++;
	} else
		ret = -LD_ERR_PLAT_MPU_REGION_EMPTY;
	return ret;
}

/*make sure protect region is the last valid region*/
static int get_md_protect_mpu_region(void)
{
	int last_index;

	last_index = (int)(sizeof(free_mpu_region)/sizeof(int)) -1;
	if (free_mpu_region[last_index] < 0)	/*free region end by -1*/
		last_index--;	/*make sure it is index of last valid region*/

	if (curr_free_mpu_idx > last_index)
		return -LD_ERR_PLAT_MPU_REGION_EMPTY;

	return free_mpu_region[last_index];
}

static void get_mpu_region_default_access_att(
	unsigned int apc[EMI_MPU_DGROUP_NUM], int region, int lock)
{
#ifdef ENABLE_EMI_PROTECTION
	SET_ACCESS_PERMISSION(apc, lock,
			mpu_att_default[region][15], mpu_att_default[region][14],
			mpu_att_default[region][13], mpu_att_default[region][12],
			mpu_att_default[region][11], mpu_att_default[region][10],
			mpu_att_default[region][9], mpu_att_default[region][8],
			mpu_att_default[region][7], mpu_att_default[region][6],
	    mpu_att_default[region][5], mpu_att_default[region][4],
	    mpu_att_default[region][3], mpu_att_default[region][2],
	    mpu_att_default[region][1], mpu_att_default[region][0]);
#else
	return;
#endif
}

static void mpu_attr_calculate(
	unsigned int apc[EMI_MPU_DGROUP_NUM],int region_id, unsigned int request_attr)
{
	/* |D0(AP)|D1(MD1)|D2(CONN)|D3(Res)|D4(MM)|D5(MD3)|D6(MFG)|D7(MDHW)*/
	unsigned int tmp_mpu_att[MPU_MDOMAIN_ID_TOTAL_NUM], i;
	for (i = 0; i < MPU_MDOMAIN_ID_TOTAL_NUM; i++)
		tmp_mpu_att[i] = mpu_att_default[region_id][i];

	/* AP MD1 MDHW: AP */
	if ((request_attr & 0xF) <= FORBIDDEN)
		tmp_mpu_att[0] = (request_attr & 0xF);
	/* AP MD1 MDHW: MD1 */
	request_attr = (request_attr >> 4);
	if ((request_attr & 0xF) <= FORBIDDEN)
		tmp_mpu_att[1] = (request_attr & 0xF);
	/* AP MD1 MDHW: MDHW */
	request_attr = (request_attr >> 4);
	if ((request_attr & 0xF) <= FORBIDDEN)
		tmp_mpu_att[7] = (request_attr & 0xF);

	/* MPU_REGION_ID_MD1_MCURO_HWRW can't lock, other region lock OK */
#ifdef ENABLE_EMI_PROTECTION
  /* MPU region lock */
	if (region_id != MPU_REGION_ID_MD1_MCURO_HWRW) {
		SET_ACCESS_PERMISSION(apc, 1,
			tmp_mpu_att[15], tmp_mpu_att[14],
			tmp_mpu_att[13], tmp_mpu_att[12],
			tmp_mpu_att[11], tmp_mpu_att[10],
			tmp_mpu_att[9], tmp_mpu_att[8],
			tmp_mpu_att[7], tmp_mpu_att[6],
			tmp_mpu_att[5], tmp_mpu_att[4],
			tmp_mpu_att[3], tmp_mpu_att[2],
			tmp_mpu_att[1], tmp_mpu_att[0]);

	} else {
		SET_ACCESS_PERMISSION(apc, 0,
			tmp_mpu_att[15], tmp_mpu_att[14],
			tmp_mpu_att[13], tmp_mpu_att[12],
			tmp_mpu_att[11], tmp_mpu_att[10],
			tmp_mpu_att[9], tmp_mpu_att[8],
			tmp_mpu_att[7], tmp_mpu_att[6],
			tmp_mpu_att[5], tmp_mpu_att[4],
			tmp_mpu_att[3], tmp_mpu_att[2],
			tmp_mpu_att[1], tmp_mpu_att[0]);
	}
#else
	return;
#endif
}

static void ccci_mem_access_cfg(mpu_cfg_t *mpu_cfg_list, int clear)
{
#ifdef ENABLE_EMI_PROTECTION
	mpu_cfg_t *curr;
	struct emi_region_info_t region_info;
	unsigned int curr_attr[EMI_MPU_DGROUP_NUM];
	char buf[MPU_STR_BUF_SIZE];
	int i;
	if (NULL == mpu_cfg_list)
		return;
	SET_ACCESS_PERMISSION(curr_attr, 0,
			NO_PROTECTION, NO_PROTECTION, NO_PROTECTION, NO_PROTECTION,
			NO_PROTECTION, NO_PROTECTION, NO_PROTECTION, NO_PROTECTION,
			NO_PROTECTION, NO_PROTECTION, NO_PROTECTION, NO_PROTECTION,
			NO_PROTECTION, NO_PROTECTION, NO_PROTECTION, NO_PROTECTION);
	for (curr = mpu_cfg_list; curr->region != -1; curr++) {
		if (clear) {
			region_info.region = (unsigned int)curr->region;
			emi_mpu_clear_protection(&region_info);
			get_mpu_attr_str(0, curr_attr, buf, MPU_STR_BUF_SIZE);
			ALWAYS_LOG("Clr MPU:S:0x%x E:0x%x A:<%d>[0~15]%s\n",
				0, 0, curr->region, buf);
		} else {
			region_info.start = curr->start;
			region_info.end = curr->end;
			region_info.region = (unsigned int)curr->region;
			for (i = 0; i < EMI_MPU_DGROUP_NUM; i++)
				region_info.apc[i] = curr->permission[i];
			emi_mpu_set_protection(&region_info);
			get_mpu_attr_str(0, curr->permission, buf, MPU_STR_BUF_SIZE);
			ALWAYS_LOG("Set MPU:S:0x%x E:0x%x A:<%d>[0~15]%s\n",
				curr->start, curr->end, curr->region, buf);
		}
	}

#endif
}

/*--------- Implement one by one -------------------------------------------------------------------------------*/
int plat_get_padding_mpu_num(void)
{
	return (int)(sizeof(free_mpu_region)/sizeof(unsigned int)) - 1;
}

/*---------------------------------------------------------------------------------------------------*/
/* Global variable for share memory                                                                  */
/*---------------------------------------------------------------------------------------------------*/
static unsigned int ap_md1_smem_size_at_lk_env;
static unsigned int md1_md3_smem_size_at_lk_env;
static unsigned int ap_md3_smem_size_at_lk_env;

static unsigned int ap_md1_smem_size_at_img;
static unsigned int md1_md3_smem_size_at_img;
static unsigned int ap_md3_smem_size_at_img;

#define AP_MD1_SMEM_SIZE    0x200000
#define MD1_MD3_SMEM_SIZE   0x200000
#define AP_MD3_SMEM_SIZE    0x200000
#define MAX_SMEM_SIZE       0x8000000/*Change from 6M to 128M,0x600000*/
typedef struct _smem_layout {
	unsigned long long base_addr;
	unsigned int ap_md1_smem_offset;
	unsigned int ap_md1_smem_size;
	unsigned int ap_md3_smem_offset;
	unsigned int ap_md3_smem_size;
	unsigned int md1_md3_smem_offset;
	unsigned int md1_md3_smem_size;
	unsigned int total_smem_size;
} smem_layout_t;
static smem_layout_t smem_info;

typedef struct _ccb_layout {
	unsigned long long ccb_data_buffer_addr;
	unsigned int ccb_data_buffer_size;
} ccb_layout_t;
static ccb_layout_t ccb_info;


/*---------------------------------------------------------------------------------------------------*/
/* HW remap function implement                                      */
/*---------------------------------------------------------------------------------------------------*/
int plat_apply_hw_remap_for_md_ro_rw(void* info)
{
	modem_info_t *md_ld_info = (modem_info_t *)info;
	return md_mem_ro_rw_remapping((unsigned int)md_ld_info->md_id, md_ld_info->base_addr);
}

int plat_apply_hw_remap_for_md_smem(void *addr, int size)
{
	/* For share memory final size depends on MD number, just store start address and size
	** actual setting will do later
	*/
	smem_info.base_addr = (unsigned long long)((unsigned long)addr);
	return 0;
}

int get_ccci_md_view_smem_addr(unsigned long long *ap_addr, unsigned int *md_addr)
{
	int ret = 0;

	if (ap_addr)
		*ap_addr = smem_info.base_addr + smem_info.ap_md1_smem_offset + 0x100000;
	else
		ret = -1;
	if (md_addr)
		*md_addr = 0x40000000 + smem_info.ap_md1_smem_offset + 0x100000;
	else
		ret = -2;
	if (ret >= 0)
		ALWAYS_LOG("[ccci]get_md_view_smem: ap_addr = 0x%llx, 0x%x\n", *ap_addr, *md_addr);
	else
		ALWAYS_LOG("[ccci]get_md_view_smem: param error, ret = %d\n", ret);
	return ret;
}

#ifdef RF_DTB_DETECT
#define RF_MIPI_PORT_0 3
#define RF_MIPI_PORT_3 6
#define RF_MIPI_PORT_5 8

static int MML1_MIPI_GEN_DATA(unsigned int inputdata)
{
   unsigned short data, temp;
   unsigned int parity = 1;
   data = inputdata >> 16;
   temp = data;
   while(temp) {
      parity += (temp & 0x1);
      temp >>= 1;
   }
   return ((data << 1) | (parity & 0x1));
}

static void MML1_MIPI_IMM_READ_BSI(unsigned int port_idx, unsigned int bsi_addr, unsigned int *bsidata)
{
	if(port_idx == RF_MIPI_PORT_0) {
		while((DRV_Reg32(0x1035119C) & 0x1) == 0x0) {
			//ALWAYS_LOG("Read MIPI 0x%x=0x%x\n", 0x1035119C, DRV_Reg32(0x1035119C));
			// use while loop to check BSI module is busy or not
		}
		DRV_WriteReg32(0x10351188, 0x2);
		DRV_WriteReg32(0x1035118C, bsi_addr);
		DRV_WriteReg32(0x10351190, 0x8000C);
		DRV_WriteReg32(0x10351198, 0x0);
		DRV_WriteReg32(0x10351188, 0x3);
		while((DRV_Reg32(0x1035152C) & 0x10) == 0x0) {
			//ALWAYS_LOG("Read MIPI 0x%x=0x%x\n", 0x1035152C, DRV_Reg32(0x1035152C));
			// use while loop to check bsidata can be read or not
		}
		*bsidata = DRV_Reg32(0x103511A0);
		DRV_WriteReg32(0x10351528, 0x10);
	} else if(port_idx == RF_MIPI_PORT_3) {
		while((DRV_Reg32(0x103511FC) & 0x1) == 0x0) {
			//ALWAYS_LOG("Read MIPI 0x%x=0x%x\n", 0x103511FC, DRV_Reg32(0x103511FC));
			// use while loop to check BSI module is busy or not
		}
		DRV_WriteReg32(0x103511E8, 0x2);
		DRV_WriteReg32(0x103511EC, bsi_addr);
		DRV_WriteReg32(0x103511F0, 0x8000C);
		DRV_WriteReg32(0x103511F8, 0x0);
		DRV_WriteReg32(0x103511E8, 0x3);
		while((DRV_Reg32(0x1035152C) & 0x80) == 0x0) {
			//ALWAYS_LOG("Read MIPI 0x%x=0x%x\n", 0x1035152C, DRV_Reg32(0x1035152C));
			// use while loop to check bsidata can be read or not
		}
		*bsidata = DRV_Reg32(0x10351200);
		DRV_WriteReg32(0x10351528, 0x80);
	} else if(port_idx == RF_MIPI_PORT_5) {
		while((DRV_Reg32(0x1035123C) & 0x1) == 0x0) {
			//ALWAYS_LOG("Read MIPI 0x%x=0x%x\n", 0x1035123C, DRV_Reg32(0x1035123C));
			// use while loop to check BSI module is busy or not
		}
		DRV_WriteReg32(0x10351228, 0x2);
		DRV_WriteReg32(0x1035122C, bsi_addr);
		DRV_WriteReg32(0x10351230, 0x8000C);
		DRV_WriteReg32(0x10351238, 0x0);
		DRV_WriteReg32(0x10351228, 0x3);
		while((DRV_Reg32(0x1035152C) & 0x200) == 0x0) {
			//ALWAYS_LOG("Read MIPI 0x%x=0x%x\n", 0x1035152C, DRV_Reg32(0x1035152C));
			// use while loop to check bsidata can be read or not
		}
		*bsidata = DRV_Reg32(0x10351240);
		DRV_WriteReg32(0x10351528, 0x200);
	}
}

static int read_mipi_component_product_id(int port_idx)
{
	int usid;
	unsigned int cmd_cwpid, mipi_cwpid, bsi_addr, product_id = 0;

	for (usid = 0; usid < 16; usid++) {
		//ALWAYS_LOG("Read MIPI port %d %d\n", port_idx, usid);
		cmd_cwpid = ((usid<<24) & 0xF000000) | 0x7D0000;
		bsi_addr = MML1_MIPI_GEN_DATA(cmd_cwpid);
		mipi_cwpid = 0;
		MML1_MIPI_IMM_READ_BSI(port_idx, bsi_addr, &mipi_cwpid);
		product_id = (mipi_cwpid >> 1) & 0xFF;
		if (product_id != 0)
			return 1;
	}
	return 0;
}

int ccci_detect_rf_board()
{
	ALWAYS_LOG("RF detecting...\n");
	ALWAYS_LOG("BSITOP init 0x%x\n", DRV_Reg32(0x102101D0));
	DRV_WriteReg32(0x10354804, 0x21);
	DRV_WriteReg32(0x10354810, 0xA08);
	DRV_WriteReg32(0x10354814, 0x150B0000);
	DRV_WriteReg32(0x10354830, 0x3);
	DRV_WriteReg32(0x10356004, 0x21);
	DRV_WriteReg32(0x10356010, 0xA08);
	DRV_WriteReg32(0x10356014, 0x150B0000);
	DRV_WriteReg32(0x10356030, 0x3);
	DRV_WriteReg32(0x10357004, 0x21);
	DRV_WriteReg32(0x10357010, 0xA08);
	DRV_WriteReg32(0x10357014, 0x150B0000);
	DRV_WriteReg32(0x10357030, 0x3);
	if (read_mipi_component_product_id(RF_MIPI_PORT_0)) {
			ALWAYS_LOG("RF found on RF_MIPI_PORT_0\n");
			return 1;
	}
	if (read_mipi_component_product_id(RF_MIPI_PORT_3)) {
			ALWAYS_LOG("RF found on RF_MIPI_PORT_3\n");
			return 1;
	}
	if (read_mipi_component_product_id(RF_MIPI_PORT_5)) {
			ALWAYS_LOG("RF found on RF_MIPI_PORT_5\n");
			return 1;
	}
	ALWAYS_LOG("RF not found\n");
	return 0;
}
#endif

/*---------------------------------------------------------------------------------------------------*/
/* check header info collection by plat_post_hdr_info                                                */
/*---------------------------------------------------------------------------------------------------*/
void plat_post_hdr_info(void* hdr, int ver, int id)
{
	if (id == MD_SYS1) {
		ap_md1_smem_size_at_img = ((struct md_check_header_v6*)hdr)->ap_md_smem_size;
		md1_md3_smem_size_at_img = ((struct md_check_header_v6*)hdr)->md_to_md_smem_size;
	} else if (id == MD_SYS3) {
		ap_md3_smem_size_at_img = ((struct md_check_header_v1*)hdr)->ap_md_smem_size;
	}
}

/*---------------------------------------------------------------------------------------------------*/
/* MPU static global variable and mpu relate function implement                                      */
/*---------------------------------------------------------------------------------------------------*/
#define MPU_REGION_TOTAL_NUM    (16) /* = MD1+MD3 */
static mpu_cfg_t mpu_tbl[MPU_REGION_TOTAL_NUM];
static int s_g_curr_mpu_num;
/*
** if set start=0x0, end=0x10000, the actural protected area will be 0x0-0x1FFFF,
** here we use 64KB align, MPU actually request 32KB align since MT6582, but this works...
** we assume emi_mpu_set_region_protection will round end address down to 64KB align.
*/
static void dump_received_pure_mpu_setting(struct image_section_desc *mem_info, int item_num)
{
	int i;
	for (i =0; i < item_num; i++)
		MPU_DBG_LOG("mpu sec dec %d: offset:%x, size:%x, mpu_attr:%x, ext_flag:%x, relate_idx:%x\n", i,
		            mem_info[i].offset, mem_info[i].size, mem_info[i].mpu_attr,
		            mem_info[i].ext_flag, mem_info[i].relate_idx);
}
static int find_bind_mpu_region(mpu_cfg_t *mpu_tbl_hdr, int item_num, unsigned int bind_key)
{
	int i;
	for (i = 0; i < item_num; i++) {
		if (mpu_tbl_hdr[i].relate_region == (int)bind_key)
			return i;
	}

	return -1;
}
static int md1_mpu_setting_process(void *p_md_ld_info, void *p_mem_info, mpu_cfg_t *mpu_tbl_hdr)
{
	modem_info_t *md_ld_info = (modem_info_t *)p_md_ld_info;
	struct image_section_desc *mem_info = (struct image_section_desc *)p_mem_info;
	int normal_region_num = 0;
	int total_region_num = 0;
	int curr_idx = 0;
	int i;
	int j;
	int had_padding = 0;
	int all_range_region_idx = -1;
	int bind_idx;
	int free_region_id;

	/* Calculate mpu num and padding num */
	for (i = 0; i < MPU_REGION_TOTAL_NUM; i++) {
		if ((mem_info[i].offset == 0) && (mem_info[i].size == 0))
			break;

		if (mem_info[i].ext_flag & VALID_PADDING)
			had_padding = 1;
		if (mem_info[i].ext_flag & MD_ALL_RANGE)
			all_range_region_idx = i;
	}
	total_region_num = i;

	dump_received_pure_mpu_setting(mem_info, total_region_num);

	for (i = 0; i < total_region_num; i++) {
		if (mem_info[i].ext_flag & (MD_ALL_RANGE|NEED_REMOVE|NEED_MPU_MORE))
			continue;
		/* Process normal case first */
		if (curr_idx >= (int)(sizeof(region_mapping_at_hdr_md1)/sizeof(unsigned char))) {
			ALWAYS_LOG("[error]md%d: mpu region too more %d\n", md_ld_info->md_id+1,
			           (int)(sizeof(region_mapping_at_hdr_md1)/sizeof(unsigned char)));
			return -LD_ERR_PLAT_MPU_REGION_TOO_MORE;
		}
		mpu_tbl_hdr[curr_idx].start = (unsigned int)md_ld_info->base_addr + mem_info[i].offset;
		mpu_tbl_hdr[curr_idx].end = mpu_tbl_hdr[curr_idx].start + mem_info[i].size;
		mpu_tbl_hdr[curr_idx].end = ((mpu_tbl_hdr[curr_idx].end + 0xFFFF)&(~0xFFFF)) - 1;/* 64K align */
		mpu_attr_calculate(mpu_tbl_hdr[curr_idx].permission, region_mapping_at_hdr_md1[curr_idx], mem_info[i].mpu_attr);
		mpu_tbl_hdr[curr_idx].region = (int)region_mapping_at_hdr_md1[curr_idx];
		mpu_tbl_hdr[curr_idx].relate_region = mem_info[i].relate_idx;
		curr_idx++;
		normal_region_num++;
	}
	if (normal_region_num != (int)(sizeof(region_mapping_at_hdr_md1)/sizeof(unsigned char))) {
		ALWAYS_LOG("[error]md%d: mpu region not sync %d:%d\n", md_ld_info->md_id+1, normal_region_num,
		           (int)(sizeof(region_mapping_at_hdr_md1)/sizeof(unsigned char)));
		return -LD_ERR_PLAT_MPU_REGION_NUM_NOT_SYNC;
	}
	for (i = 0; i < total_region_num; i++) {
		if (mem_info[i].ext_flag & NEED_MPU_MORE) {
			bind_idx = find_bind_mpu_region(mpu_tbl_hdr, curr_idx, mem_info[i].relate_idx);
			if (bind_idx >= 0) {
				mpu_tbl_hdr[curr_idx].start = (unsigned int)md_ld_info->base_addr + mem_info[i].offset;
				mpu_tbl_hdr[curr_idx].end = mpu_tbl_hdr[curr_idx].start + mem_info[i].size;
				/* 64K align */
				mpu_tbl_hdr[curr_idx].end = ((mpu_tbl_hdr[curr_idx].end + 0xFFFF)&(~0xFFFF)) - 1;
				for (j = 0; j < EMI_MPU_DGROUP_NUM; j++)
					mpu_tbl_hdr[curr_idx].permission[j] = mpu_tbl_hdr[bind_idx].permission[j];
				/* setting relate region */
				free_region_id = get_free_mpu_region();
				if (free_region_id < 0) {
					ALWAYS_LOG("[error]abnormal free region id %d +\n", free_region_id);
					return -LD_ERR_PLAT_ABNORMAL_FREE_REGION;
				}
				mpu_tbl_hdr[curr_idx].region = free_region_id;
				mpu_tbl_hdr[curr_idx].relate_region = mem_info[i].relate_idx;
				mpu_tbl_hdr[bind_idx].relate_region = free_region_id;
				curr_idx++;
			} else {
				ALWAYS_LOG("md%d: padding array abnormal\n", md_ld_info->md_id+1);
				return -LD_ERR_PLAT_ABNORMAL_PAD_ARRAY;
			}
		}
	}
	/* Apply MD all range mpu protect setting */
	if (had_padding) {
		free_region_id = get_md_protect_mpu_region(); //get_free_mpu_region();
		if (free_region_id < 0) {
			ALWAYS_LOG("[error]no more free region\n");
			return -LD_ERR_PLAT_NO_MORE_FREE_REGION;
		}
		get_mpu_region_default_access_att(mpu_tbl_hdr[curr_idx].permission, free_region_id, 1);
		mpu_tbl_hdr[curr_idx].start = (unsigned int)md_ld_info->base_addr + mem_info[all_range_region_idx].offset;
		mpu_tbl_hdr[curr_idx].end = mpu_tbl_hdr[curr_idx].start + mem_info[all_range_region_idx].size;
		/* 64K align */
		mpu_tbl_hdr[curr_idx].end = ((mpu_tbl_hdr[curr_idx].end + 0xFFFF)&(~0xFFFF)) - 1;
		mpu_tbl_hdr[curr_idx].region = free_region_id;
		mpu_tbl_hdr[curr_idx].relate_region = 0;
		curr_idx++;
	}

	/* Clear logic relate index to 0 to mark as end */
	for (i = 0; i < curr_idx; i++) {
		if (mpu_tbl_hdr[i].relate_region >= LOGIC_BINDING_IDX_START)
			mpu_tbl_hdr[i].relate_region = 0;
	}
	return curr_idx;
}

int plat_send_mpu_info_to_platorm(void *p_md_ld_info, void *p_mem_info)
{
	modem_info_t *md_ld_info = (modem_info_t *)p_md_ld_info;
	struct image_section_desc *mem_info = (struct image_section_desc *)p_mem_info;
	int md_id = md_ld_info->md_id;
	int ret;
	int i;
	char buf[MPU_STR_BUF_SIZE];

	if (md_id == MD_SYS1) {
		ret = md1_mpu_setting_process(p_md_ld_info, p_mem_info, &mpu_tbl[s_g_curr_mpu_num]);
		if (ret > 0)
			s_g_curr_mpu_num += ret;
	} else if (md_id == MD_SYS3) {
		/* RO part */
		get_mpu_region_default_access_att(mpu_tbl[s_g_curr_mpu_num].permission, MPU_REGION_ID_MD3_ROM, 1);
		mpu_tbl[s_g_curr_mpu_num].start = (unsigned int)md_ld_info->base_addr + mem_info[0].offset;
		mpu_tbl[s_g_curr_mpu_num].end = mpu_tbl[s_g_curr_mpu_num].start + mem_info[0].size;
		mpu_tbl[s_g_curr_mpu_num].region = MPU_REGION_ID_MD3_ROM;
		/* 64K align */
		mpu_tbl[s_g_curr_mpu_num].end = ((mpu_tbl[s_g_curr_mpu_num].end + 0xFFFF)&(~0xFFFF)) - 1;
		s_g_curr_mpu_num++;

		/* RW part */
		get_mpu_region_default_access_att(mpu_tbl[s_g_curr_mpu_num].permission, MPU_REGION_ID_MD3_RW, 1);
		mpu_tbl[s_g_curr_mpu_num].start = (unsigned int)md_ld_info->base_addr + mem_info[1].offset;
		mpu_tbl[s_g_curr_mpu_num].end = mpu_tbl[s_g_curr_mpu_num].start + mem_info[1].size;
		mpu_tbl[s_g_curr_mpu_num].region = MPU_REGION_ID_MD3_RW;
		/* 64K align */
		mpu_tbl[s_g_curr_mpu_num].end = ((mpu_tbl[s_g_curr_mpu_num].end + 0xFFFF)&(~0xFFFF)) - 1;
		s_g_curr_mpu_num++;
	}

	for (i =0; i < s_g_curr_mpu_num; i++) {
		get_mpu_attr_str(0, mpu_tbl[i].permission, buf, MPU_STR_BUF_SIZE);
		MPU_DBG_LOG("plat mpu dec %d: region:%d[%d], start:0x%x, end:0x%x, attr:%s\n", i,
		            mpu_tbl[i].region, mpu_tbl[i].relate_region, mpu_tbl[i].start, mpu_tbl[i].end, buf);
	}

	return 0;
}

/*------------------------------------------------------------------------------------------------*/
/* Suppor function for share memory calculate */
/*------------------------------------------------------------------------------------------------*/
static int cal_share_mem_layout(int load_flag)
{
	ap_md1_smem_size_at_lk_env = str2uint(get_env("apmd1_smem"));
	md1_md3_smem_size_at_lk_env = str2uint(get_env("md1md3_smem"));
	ap_md3_smem_size_at_lk_env = str2uint(get_env("apmd3_smem"));
	ALWAYS_LOG("env[apmd1_smem]%x.\n", ap_md1_smem_size_at_lk_env);
	ALWAYS_LOG("env[md1md3_smem]%x.\n", md1_md3_smem_size_at_lk_env);
	ALWAYS_LOG("env[apmd3_smem]%x.\n", ap_md3_smem_size_at_lk_env);
	/* MD Share memory layout */
	/*    AP    <-->   MD1    */
	/*    MD1   <-->   MD3    */
	/*    AP    <-->   MD3    */
	if (load_flag & (1<<MD_SYS3)) {
		smem_info.ap_md1_smem_offset = 0;
		if (ap_md1_smem_size_at_lk_env)
			smem_info.ap_md1_smem_size = ap_md1_smem_size_at_lk_env;
		else if (ap_md1_smem_size_at_img)
			smem_info.ap_md1_smem_size = ap_md1_smem_size_at_img;
		else
			smem_info.ap_md1_smem_size  = AP_MD1_SMEM_SIZE;

		smem_info.md1_md3_smem_offset = (smem_info.ap_md1_smem_offset + smem_info.ap_md1_smem_size + 0x10000 - 1)&(~(0x10000 - 1));
		if (md1_md3_smem_size_at_lk_env)
			smem_info.md1_md3_smem_size = md1_md3_smem_size_at_lk_env;
		else if (md1_md3_smem_size_at_img)
			smem_info.md1_md3_smem_size = md1_md3_smem_size_at_img;
		else
			smem_info.md1_md3_smem_size = MD1_MD3_SMEM_SIZE;

		smem_info.ap_md3_smem_offset = smem_info.md1_md3_smem_offset + smem_info.md1_md3_smem_size;
		if (ap_md3_smem_size_at_lk_env)
			smem_info.ap_md3_smem_size = ap_md3_smem_size_at_lk_env;
		else if (ap_md3_smem_size_at_img)
			smem_info.ap_md3_smem_size = ap_md3_smem_size_at_img;
		else
			smem_info.ap_md3_smem_size = AP_MD3_SMEM_SIZE;

		smem_info.total_smem_size = smem_info.ap_md3_smem_offset + smem_info.ap_md3_smem_size;
	} else {
		smem_info.ap_md1_smem_offset = 0;
		if (ap_md1_smem_size_at_lk_env)
			smem_info.ap_md1_smem_size = ap_md1_smem_size_at_lk_env;
		else if (ap_md1_smem_size_at_img)
			smem_info.ap_md1_smem_size = ap_md1_smem_size_at_img;
		else
			smem_info.ap_md1_smem_size  = AP_MD1_SMEM_SIZE;

		smem_info.md1_md3_smem_offset = 0;
		smem_info.md1_md3_smem_size = 0;
		smem_info.ap_md3_smem_offset = 0;
		smem_info.ap_md3_smem_size = 0;
		smem_info.total_smem_size = smem_info.ap_md1_smem_size;
	}
#if 0
	ccb_data_buf = ccci_request_mem(CCB_DATA_BUF_SIZE, 0x90000000LL, 0x2000000L);
	if (ccb_data_buf == NULL) {
		ALWAYS_LOG("allocate ccb data buffer share memory fail\n");
	} else {
		ccb_info.ccb_data_buffer_addr = (unsigned long long)((unsigned long)ccb_data_buf);
		ccb_info.ccb_data_buffer_size = CCB_DATA_BUF_SIZE;

		if (insert_ccci_tag_inf("ccb_info", (char*)&ccb_info, sizeof(ccb_layout_t)) < 0)
			ALWAYS_LOG("insert ccb_info fail\n");

		ALWAYS_LOG("ccb_info.ccb_data_buffer_addr: %x\n", (unsigned int)ccb_info.ccb_data_buffer_addr);
		ALWAYS_LOG("ccb_info.ccb_data_buffer_size: %x\n", ccb_info.ccb_data_buffer_size);
	}
#endif
	/* insert share memory layout to lk info */
	if (insert_ccci_tag_inf("smem_layout", (char*)&smem_info, sizeof(smem_layout_t)) < 0)
		ALWAYS_LOG("insert smem_layout fail\n");

	ALWAYS_LOG("smem_info.base_addr: %x\n", (unsigned int)smem_info.base_addr);
	ALWAYS_LOG("smem_info.ap_md1_smem_offset: %x\n", smem_info.ap_md1_smem_offset);
	ALWAYS_LOG("smem_info.ap_md1_smem_size: %x\n", smem_info.ap_md1_smem_size);
	ALWAYS_LOG("smem_info.ap_md3_smem_offset: %x\n", smem_info.ap_md3_smem_offset);
	ALWAYS_LOG("smem_info.ap_md3_smem_size: %x\n", smem_info.ap_md3_smem_size);
	ALWAYS_LOG("smem_info.md1_md3_smem_offset: %x\n", smem_info.md1_md3_smem_offset);
	ALWAYS_LOG("smem_info.md1_md3_smem_size: %x\n", smem_info.md1_md3_smem_size);
	ALWAYS_LOG("smem_info.total_smem_size: %x\n", smem_info.total_smem_size);

	return (int)smem_info.total_smem_size;
}

static void boot_to_dummy_ap_mode(int load_md_flag);
/*------------------------------------------------------------------------------------------------*/
/* Note: This function using global variable
** if set start=0x0, end=0x10000, the actural protected area will be 0x0-0x1FFFF,
** here we use 64KB align, MPU actually request 32KB align since MT6582, but this works...
** we assume emi_mpu_set_region_protection will round end address down to 64KB align.
*/
int plat_apply_platform_setting(int load_md_flag)
{
	int smem_final_size;

#ifdef DUMMY_AP_MODE
	/* This function will never return */
	ALWAYS_LOG("boot to dummy ap mode!!!\n");
	boot_to_dummy_ap_mode(load_md_flag);
	return 0;
#endif

	/* Check loading validation */
	if (((load_md_flag & (1<<MD_SYS1)) == 0) && (load_md_flag & (1<<MD_SYS3))) {
		ALWAYS_LOG("md3 depends on md1,but md1 not loaded\n");
		return -LD_ERR_PLAT_MD1_NOT_RDY;
	}
	if ((load_md_flag & ((1<<MD_SYS1)|(1<<MD_SYS3))) == 0) {
		ALWAYS_LOG("both md1 and md3 not enable\n");
		return 0;
	}

	smem_final_size = cal_share_mem_layout(load_md_flag);
	ALWAYS_LOG("ap md1 share mem MPU need configure\n");
	mpu_tbl[s_g_curr_mpu_num].region = MPU_REGION_ID_MD1_SMEM;
	get_mpu_region_default_access_att(mpu_tbl[s_g_curr_mpu_num].permission, MPU_REGION_ID_MD1_SMEM, 0);
	mpu_tbl[s_g_curr_mpu_num].start = (unsigned int)smem_info.base_addr + smem_info.ap_md1_smem_offset;
	mpu_tbl[s_g_curr_mpu_num].end = (unsigned int)smem_info.base_addr + smem_info.ap_md1_smem_offset
	                                + smem_info.ap_md1_smem_size;
	mpu_tbl[s_g_curr_mpu_num].end = ((mpu_tbl[s_g_curr_mpu_num].end + 0xFFFF)&(~0xFFFF)) - 1;
	s_g_curr_mpu_num++;

	/* add for ccb data buffer mpu */
	mpu_tbl[s_g_curr_mpu_num].region = MPU_REGION_ID_MD1_CCB;
	get_mpu_region_default_access_att(mpu_tbl[s_g_curr_mpu_num].permission, MPU_REGION_ID_MD1_CCB, 0);
	mpu_tbl[s_g_curr_mpu_num].start = (unsigned int)ccb_data_buf + smem_info.ap_md1_smem_offset;
	mpu_tbl[s_g_curr_mpu_num].end = (unsigned int)ccb_data_buf + CCB_DATA_BUF_SIZE;
	mpu_tbl[s_g_curr_mpu_num].end = ((mpu_tbl[s_g_curr_mpu_num].end + 0xFFFF)&(~0xFFFF)) - 1;
	s_g_curr_mpu_num++;

	if (load_md_flag & (1<<MD_SYS3)) {
		ALWAYS_LOG("md1 md3 share mem MPU need configure\n");
		mpu_tbl[s_g_curr_mpu_num].region = MPU_REGION_ID_MD1MD3_SMEM;
		get_mpu_region_default_access_att(mpu_tbl[s_g_curr_mpu_num].permission, MPU_REGION_ID_MD1MD3_SMEM, 0);
		mpu_tbl[s_g_curr_mpu_num].start = (unsigned int)smem_info.base_addr + smem_info.md1_md3_smem_offset;
		mpu_tbl[s_g_curr_mpu_num].end = (unsigned int)smem_info.base_addr + smem_info.md1_md3_smem_offset
		                                + smem_info.md1_md3_smem_size;
		mpu_tbl[s_g_curr_mpu_num].end = ((mpu_tbl[s_g_curr_mpu_num].end + 0xFFFF)&(~0xFFFF)) - 1;
		s_g_curr_mpu_num++;
		ALWAYS_LOG("ap md3 share mem MPU need configure\n");
		mpu_tbl[s_g_curr_mpu_num].region = MPU_REGION_ID_MD3_SMEM;
		get_mpu_region_default_access_att(mpu_tbl[s_g_curr_mpu_num].permission, MPU_REGION_ID_MD3_SMEM, 0);
		mpu_tbl[s_g_curr_mpu_num].start = (unsigned int)smem_info.base_addr + smem_info.ap_md3_smem_offset;
		mpu_tbl[s_g_curr_mpu_num].end = (unsigned int)smem_info.base_addr + smem_info.ap_md3_smem_offset
		                                + smem_info.ap_md3_smem_size;
		mpu_tbl[s_g_curr_mpu_num].end = ((mpu_tbl[s_g_curr_mpu_num].end + 0xFFFF)&(~0xFFFF)) - 1;
		s_g_curr_mpu_num++;
	}

	mpu_tbl[s_g_curr_mpu_num].region = -1; /* mark for end */
	/* Insert mpu tag info */
	if (insert_ccci_tag_inf("md_mpu_inf", (char*)mpu_tbl, sizeof(mpu_cfg_t)*s_g_curr_mpu_num) < 0)
		ALWAYS_LOG("insert md_mpu_inf fail\n");
	if (insert_ccci_tag_inf("md_mpu_num", (char*)&s_g_curr_mpu_num, sizeof(int)) < 0)
		ALWAYS_LOG("insert md_mpu_num fail\n");

	/* Apply all MPU setting */
	ccci_mem_access_cfg(mpu_tbl, 0);

	/* Apply share memory HW remap setting and lock it */
	if (load_md_flag & (1<<MD_SYS1)) {
		md_smem_rw_remapping(MD_SYS1, (unsigned int)(smem_info.base_addr + smem_info.ap_md1_smem_offset));
		md_emi_remapping_lock(MD_SYS1);
	}

	if (load_md_flag & (1<<MD_SYS3)) {
		md_smem_rw_remapping(MD_SYS3, (unsigned int)(smem_info.base_addr + smem_info.md1_md3_smem_offset));
		md_emi_remapping_lock(MD_SYS3);
	}

	return smem_final_size;
}

/*------------------------------------------------------------------------------------------------*/
/* platform configure setting info.                                                               */
/*------------------------------------------------------------------------------------------------*/
long long plat_ccci_get_ld_md_plat_setting(char cfg_name[])
{
	if (strcmp(cfg_name, "share_memory_size") == 0) {

#ifdef DUMMY_AP_MODE
		return 0x200000;
#endif
		return (long long)(MAX_SMEM_SIZE);
	}

	if (strcmp(cfg_name, "share_mem_limit") == 0)
		return 0x90000000LL;

	if (strcmp(cfg_name, "ro_rw_mem_limit") == 0) {
#ifdef DUMMY_AP_MODE
		return 0xA0000000LL;
#endif
		return 0xC0000000LL;
	}

	if (strcmp(cfg_name, "ro_rw_mem_align") == 0)
		return 0x2000000LL;

	if (strcmp(cfg_name, "share_mem_align") == 0)
		return 0x2000000LL;

	if (strcmp(cfg_name, "ld_version") == 0) {

#ifdef DUMMY_AP_MODE
		return 0x20001;
#endif
		return 0x20000;/* xxxx_yyyy, xxxx: main id, yyyy sub id */
	}

	return -1LL;
}

#ifdef DUMMY_AP_MODE
#include <platform/mt_irq.h>
extern void dummy_ap_boot_up_md(int md_en_flag);
extern void load_modem_image(void);
extern int dummy_ap_irq_helper(unsigned int);

/* Remember add this function to file platform.c(platform code) */
void dummy_ap_entry(void)
{
	load_modem_image();
}

/* Remember add this function to file interrupts.c(platform code) */
void dummy_ap_irq_handler(unsigned int irq)
{
	if (dummy_ap_irq_helper(irq)) {
		mt_irq_ack(irq);
		mt_irq_unmask(irq);
	}
}

void boot_to_dummy_ap_mode(int load_md_flag)
{
	md_smem_rw_remapping(MD_SYS1, smem_info.base_addr);
	md_smem_rw_remapping(MD_SYS3, smem_info.base_addr);
	/* Before boot dummy AP, clear share memory */
	memset((void*)((unsigned long)smem_info.base_addr), 0, 0x200000);

	dummy_ap_boot_up_md(load_md_flag);
}

#endif

