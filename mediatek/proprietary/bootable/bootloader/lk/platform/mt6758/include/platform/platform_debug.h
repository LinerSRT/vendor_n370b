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
#ifndef __PLATFORM_DEBUG_H__
#define __PLATFORM_DEBUG_H__

#include <latch.h>

int plt_perisys_init(const struct plt_cfg_bus_latch *self);
int plt_is_perisys_timeout(const struct plt_cfg_bus_latch *self);
int plt_lastbus_perisys_dump(const struct plt_cfg_bus_latch *self,
			     char *buf, int *wp);
int plt_infrasys_is_timeout(const struct plt_cfg_bus_latch *self);

const struct plt_cfg_pc_latch cfg_pc_latch = {
	.nr_max_core = 8,
	.nr_max_big_core = 0,
	.mp0_dbg_ctrl = 0x0404,
	.mp0_dbg_flag = 0x0408,
	.mp1_dbg_ctrl = 0x0504,
	.mp1_dbg_flag = 0x0508,
	.spm_pwr_sts = 0x01b0,
	.plat_sram_flag0 = 0x11dbf0,
};

const struct plt_cfg_bus_latch cfg_bus_latch = {
	.supported = 1,
	.num_master_port = 3,
	.num_slave_port = 4,
	.num_perisys_mon = 8,
	.num_infrasys_mon = 19,
	.mcusys_offsets = {
		.bus_mcu_m0 = 0x05A4,
		.bus_mcu_s1 = 0x05B0,
		.bus_mcu_m0_m = 0x05C0,
		.bus_mcu_s1_m = 0x05CC,
	},
	.perisys_offsets = {
		.bus_peri_r0 = 0x00f4,
		.bus_peri_r1 = 0x00f8,
		.bus_peri_mon = 0x00fc,
	},
	.secure_perisys = 0,
	.perisys_enable = 0x60000,
	.perisys_eventmask = 0x0,
	.perisys_timeout = 0x3fff,
	.peri_timeout_26m = 0x3fff,
	.infra_timeout_26m = 0xffff,
	.perisys_ops = {
		.init = plt_perisys_init,
		.is_timeout = plt_is_perisys_timeout,
		.dump = plt_lastbus_perisys_dump,
	},
	.infrasys_offsets = {
		.bus_infra_ctrl = 0x0514,
		.bus_infra_mask_l = 0x0518,
		.bus_infra_mask_h = 0x051C,
		.bus_infra_snapshot = 0x0514,
	},
	.infrasys_ops = {
		.is_timeout = plt_infrasys_is_timeout,
	},
	.infrasys_config = 0x0004ffff,
	.infrasys_enable = 0x00010000,
};

const struct plt_cfg_l2_parity_latch cfg_l2_parity_latch = {
	.supported = 0,
	.mp0_l2_cache_parity1_rdata = 0x007c,
	.mp0_l2_cache_parity2_rdata = 0x0080,
	.mp1_l2_cache_parity1_rdata = 0x027c,
	.mp1_l2_cache_parity2_rdata = 0x0280,
};

const struct plt_cfg_big_core cfg_big_core[] = {};
struct plt_circular_buffer_op circular_buffer_op;

/******* dfd *******/
#include "dfd.h"

const struct plt_cfg_dfd cfg_dfd = {
	.version = DFD_V3_0,
	.sw_version = DFD_SW_V1,
	.nr_header_row = 0,
	.buffer_addr = 0x0,
	.buffer_length = 0x40000,
	.plat_sram_flag1 = 0x11dbf4,
	.plat_sram_flag2 = 0x11dbf8,
	.dfd_bypass_efuse_check = 1,
	.dfd_disable_devinfo_index = 0x3,
	.dfd_disable_bit = 0xc,
	.chip_id_offset = 0x18,
	.chip_id = {'6','7','5','8',0,0,0,0},
};

struct plt_dfd_op dfd_op = {
	.acquire_ram_control = NULL,
	.release_ram_control = NULL,
	.setup_dfd_file_name = plt_setup_dfd_file_name,
};

/******* etb *******/
#include "etb.h"

const struct plt_cfg_etb cfg_etb = {
	.nr_etb = 1,
	.total_etb_sz = 8000,
	.dem_base = 0x0d01a000,
	.etb = {
		{0x0d010000, 0},
	},
};

#endif
