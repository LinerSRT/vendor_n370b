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

#ifndef _MT_SCP_H_
#define _MT_SCP_H_


/*platform dependent address*/
#define SCP_DRAM_SIZE       0x00600000  // mblock_reserve requires max 6 MB(1MB platform image, 5MB overlay)
#define SCP_DRAM_ALIGN      0x00100000  // due to cache align = 1MB
#define SCP_SRAM_SIZE       0x80000 //512KB

#define DRAM_ADDR_MAX       0x9FFFFFFF  // max address can SCP remap
#define DRAM_4GB_MAX        0xFFFFFFFF
#define DRAM_4GB_OFFSET     0x40000000
#define SCP_EMI_REGION      5
#define ENABLE_SCP_EMI_PROTECTION       (1)

#define SCP_CFG_BASE             0x105c0000

#define SCP_SRAM_BASE            0x10500000
#define SCP_MAX_SRAM_SIZE        0x00080000

#define SCP_SYS_CTRL                 (SCP_CFG_BASE + 0x00E4)
	#define AHB_SLICE_POST_WT   1
	#define AUTO_DDREN_BIT      18
	#define DDREN_FIX_VALUE_BIT 28

#define SCP_SRAM_PDN             (SCP_CFG_BASE + 0x402c)
#define SCP_CLK_SEL              (SCP_CFG_BASE + 0x4000)
#define SCP_CLK_EN               (SCP_CFG_BASE + 0x4004)
#define SCP_CFGREG_SW_RSTN       (SCP_CFG_BASE + 0x0000)
#define SCP_SLP_EN               (SCP_CFG_BASE + 0x4020)
#define SCP_CLR_CLK_CG           (SCP_CFG_BASE + 0x4058)
#define CLK_CTRL_L1_SRAM_PD       (SCP_CFG_BASE + 0x4080)
#define CLK_CTRL_TCM_TAIL_SRAM_PD (SCP_CFG_BASE + 0x4094)

/*image name definition*/
#define LOADER_NAME_SCP_A         "tinysys-loader-CM4_A"
#define FIRMWARE_NAME_SCP_A       "tinysys-scp-CM4_A"
#define CACHED_NAME_SCP_A         "tinysys-scp-CM4_A_dram"
#define DO_NAME_SCP_A             "tinysys-dos-CM4_A"
#define DO_NAME_SCP_B             "tinysys-dos-CM4_B"

#define SCP_LOADER_OFFSET           0x0
#define SCP_FW_OFFSET               0x800
#define SCP_CACHED_OFFSET           0x80000 //fixed to 512K
#define SCP_CACHED_BACKUO_OFFSET    0x180000 //fixed to 1M


/******************************************************************************
* 1. New entries must be appended to the end of the structure.
* 2. Do NOT use conditional option such as #ifdef inside the structure.
******************************************************************************/
struct scp_region_info_st {
	uint32_t ap_loader_start;
	uint32_t ap_loader_size;
	uint32_t ap_firmware_start;
	uint32_t ap_firmware_size;
	uint32_t ap_cached_start;
	uint32_t ap_cached_size;
	uint32_t ap_cached_backup_start;
	uint32_t struct_size;
	                // This is the size of the structure. It can act as a
	                // version number if entries can only be added to (not
	                // deleted from) the structure. It should be the first
	                // entry of the structure, but for compatibility reason it
	                // is appended here.
	uint32_t scp_log_thru_ap_uart;
};

int load_scp(void);

#endif

