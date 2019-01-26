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

#define LOG_TAG "DUMP"

#include <platform/ddp_info.h>
#include <platform/ddp_log.h>
#include <platform/ddp_path.h>
#include <platform/ddp_dump.h>
#include <platform/disp_drv_platform.h>
#include "platform/ddp_reg.h"
#include <platform/ddp_dsi.h>
#include <platform/ddp_rdma.h>
#include <platform/ddp_ovl.h>

#include <string.h>


static const char* ddp_signal[3][32] = {
	{
		"aal0__to__gamma0",
		"ccorr0__to__aal0",
		"color0__to__disp_color_out_sel_in0",
		"disp_color_out_sel__to__ccorr0",
		"disp_path0_sel__to__disp_rdma0",
		"disp_rdma0__to__disp_rdma0_rsz_in_sout",
		"disp_rdma0_sout_out0__to__dsi0_sel_in1",
		"disp_rdma0_sout_out1__to__color0",
		"disp_rdma0_sout_out2__to__disp_color_out_sel_in1",
		"disp_rdma0_sout_out3__to__dsi1_sel_in1",
		"disp_rdma1__to__disp_rdma1_sout",
		"disp_rdma1_sout_out0__to__dsi1_sel_in2",
		"disp_rdma1_sout_out1__to__dsi0_sel_in3",
		"disp_rsz__to__disp_rsz_mout",
		"disp_rsz_mout0__to__ovl0",
		"disp_rsz_mout1__to__ovl0_2L",
		"disp_rsz_mout2__to__ovl1_2L",
		"disp_rsz_mout3__to__disp_path0_sel_in3",
		"disp_rsz_mout4__to__ovl_to_wdma_sel_in3",
		"disp_rsz_sel__to__disp_rsz",
		"disp_split_out0__to__dsi0_sel_in2",
		"disp_split_out1__to__dsi1_sel_in0",
		"disp_wdma_pre_sel__to__disp_to_wdma",
		"dither0__to__dither0_mout",
		"dither0_mout0__to__dsi0_sel_in0",
		"dither0_mout1__to__disp_split",
		"dither0_mout2__to__dsi1_sel_in3",
		"dither0_mout3__to__disp_wdma0_pre_sel_in1",
		"dsi0_sel__to__dsi0",
		"dsi1_sel__to__dsi1",
		"gamma0__to__dither0",
		"mdp_to_disp0__to__mdp_to_disp0_sout",

	},
	{
		"mdp_to_disp0_sout_out0__to__ovl0_sel_in0",
		"mdp_to_disp0_sout_out1__to__ovl0_2L_sel_in0",
		"mdp_to_disp1__to__mdp_to_disp1_sout",
		"mdp_to_disp1_sout_out0__to__ovl0_2L_sel_in1",
		"mdp_to_disp1_sout_out1__to__ovl1_2L",
		"mdp_to_disp1_sout_out2__to__ovl0_sel_in1",
		"ovl0_2L__to__ovl0_2L_mout",
		"ovl0_2L_mout0__to__disp_path0_sel_in1",
		"ovl0_2L_mout1__to__ovl_to_wrot_sel_in1",
		"ovl0_2L_mout2__to__ovl_to_wdma_sel_in1",
		"ovl0_2L_mout3__to__ovl_to_rsz_sel_in1",
		"ovl0_2L_mout4__to__ovl1_2L",
		"ovl0_2L_mout5__to__disp_rsz_sel_in1",
		"ovl0_2L_sel__to__ovl0_2L",
		"ovl0__to__ovl0_mout",
		"ovl0_mout0__to__disp_path0_sel_in0",
		"ovl0_mout1__to__ovl_to_wrot_sel_in0",
		"ovl0_mout2__to__ovl_to_wdma_sel_in0",
		"ovl0_mout3__to__ovl_to_rsz_sel_in0",
		"ovl0_mout4__to__ovl0_2L",
		"ovl0_mout5__to__disp_rsz_sel_in0",
		"ovl0_sel__to__ovl0",
		"ovl1_2L__to__ovl1_2L_mout",
		"ovl1_2L_mout0__to__disp_path0_sel_in2",
		"ovl1_2L_mout1__to__ovl_to_wrot_sel_in2",
		"ovl1_2L_mout2__to__ovl_to_wdma_sel_in2",
		"ovl1_2L_mout3__to__ovl_to_rsz_sel_in2",
		"ovl1_2L_mout4__to__disp_rdma1",
		"ovl1_2L_mout5__to__disp_rsz_sel_in2",
		"ovl_to_rsz_sel__to__disp_to_rsz",
		"ovl_to_wdma_sel__to__disp_wdma0_pre_sel_in0",
		"ovl_to_wrot_sel__to__disp_to_wrot_ready",

	},
	{
		"disp_rdma0_rsz_in_sout_out0__to__disp_rdma0_rsz_out_sel_in0",
		"disp_rdma0_rsz_in_sout_out1__to__disp_rsz_rsz_sel_in3",
		"disp_rdma0_rsz_out_sel__to__disp_rdma0_sout",
		"disp_rsz_mout5__to__disp_rdma0_rsz_out_sel_in1",

	}

};

static char *ddp_greq_name(int bit)
{
	switch (bit) {
	case 0:
		return "OVL0";
	case 1:
		return "OVL0_2L";
	case 2:
		return "RDMA0";
	case 3:
		return "WDMA0";
	case 4:
		return "MDP_RDMA0";
	case 5:
		return "MDP_WDMA";
	case 6:
		return "MDP_WROT0";
	case 16:
		return "OVL1";
	case 17:
		return "OVL1_2L";
	case 18:
		return "RDMA1";
	case 19:
		return "RDMA2";
	case 20:
		return "WDMA1";
	case 21:
		return "OD_R";
	case 22:
		return "OD_W";
	case 23:
		return "OVL0_2L_LARB4";
	case 26:
		return "WMDA0_LARB5";
	default:
		return NULL;
	}
}

static char *ddp_get_mutex_module0_name(unsigned int bit)
{
	switch (bit) {
	case 0:  return "rdma0";
	case 1:  return "rdma1";
	case 2:  return "mdp_rdma0";
	case 4:  return "mdp_rsz0";
	case 5:  return "mdp_rsz1";
	case 6:  return "mdp_tdshp";
	case 7: return "mdp_wrot0";
	case 8: return "mdp_wrot1";
	case 9: return "ovl0";
	case 10: return "ovl0_2L";
	case 11: return "ovl1_2L";
	case 12: return "wdma0";
	case 13: return "color0";
	case 14: return "ccorr0";
	case 15: return "aal0";
	case 16: return "gamma0";
	case 17: return "dither0";
	case 18: return "PWM0";
	case 19: return "DSI0";
	case 20: return "DSI1";
	default: return "mutex-unknown";
	}
}

static char *ddp_get_mutex_module1_name(unsigned int bit)
{
	switch (bit) {
	case 0:  return "pwm";
	case 1:  return "dsi0";
	case 2:  return "dsi1";
	default:
		return "mutex-unknown";
	}
}

char *ddp_get_fmt_name(DISP_MODULE_ENUM module, unsigned int fmt)
{
	if (module == DISP_MODULE_WDMA0) {
		switch (fmt) {
		case 0:
			return "rgb565";
		case 1:
			return "rgb888";
		case 2:
			return "rgba8888";
		case 3:
			return "argb8888";
		case 4:
			return "uyvy";
		case 5:
			return "yuy2";
		case 7:
			return "y-only";
		case 8:
			return "iyuv";
		case 12:
			return "nv12";
		default:
			DDPDUMP("ddp_get_fmt_name, unknown fmt=%d, module=%d\n", fmt, module);
			return "unknown";
		}
	} else if (module == DISP_MODULE_OVL0) {
		switch (fmt) {
		case 0:
			return "rgb565";
		case 1:
			return "rgb888";
		case 2:
			return "rgba8888";
		case 3:
			return "argb8888";
		case 4:
			return "uyvy";
		case 5:
			return "yuyv";
		default:
			DDPDUMP("ddp_get_fmt_name, unknown fmt=%d, module=%d\n", fmt, module);
			return "unknown";
		}
	} else if (module == DISP_MODULE_RDMA0 || module == DISP_MODULE_RDMA1 || module == DISP_MODULE_RDMA2) {
		switch (fmt) {
		case 0:
			return "rgb565";
		case 1:
			return "rgb888";
		case 2:
			return "rgba8888";
		case 3:
			return "argb8888";
		case 4:
			return "uyvy";
		case 5:
			return "yuyv";
		default:
			DDPDUMP("ddp_get_fmt_name, unknown fmt=%d, module=%d\n", fmt, module);
			return "unknown";
		}
	} else {
		DDPDUMP("ddp_get_fmt_name, unknown module=%d\n", module);
	}

	return "unknown";
}

static char *ddp_clock_0(int bit)
{
	switch (bit) {
	case 0:
		return "smi_common(cg), ";
	case 1:
		return "smi_larb0(cg), ";
	case 2:
		return "smi_larb1(cg), ";

	case 19:
		return "fake_engine, ";
	case 20:
		return "ovl0, ";
	case 21:
		return "ovl0_2L, ";
	case 22:
		return "ovl1_2L, ";
	case 23:
		return "rdma0, ";
	case 24:
		return "rdma1, ";
	case 25:
		return "wdma0, ";
	case 26:
		return "color, ";
	case 27:
		return "ccorr, ";
	case 28:
		return "aal, ";
	case 29:
		return "gamma, ";
	case 30:
		return "dither, ";
	case 31:
		return "split, ";
	default:
		return NULL;
	}
}

static char *ddp_clock_1(int bit)
{
	switch (bit) {
	case 0:
		return "dsi0_mm(cg), ";
	case 1:
		return "dsi0_interface(cg), ";
	case 2:
		return "dsi1_mm(cg), ";
	case 3:
		return "dsi1_interface(cg), ";
	case 4:
		return "fake_engine2, ";
	case 7:
		return "26m, ";
	case 8:
		return "mmsys_r2y, ";
	case 9:
		return "disp_rsz, ";

	default:
		return NULL;
	}
}

static void mutex_dump_reg(void)
{
	unsigned long module_base = DISP_REG_CONFIG_MUTEX_INTEN;

	DDPDUMP("== START: DISP mutex REGS ==\n");
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x0, INREG32(module_base + 0x0),
		0x4, INREG32(module_base + 0x4),
		0x8, INREG32(module_base + 0x8),
		0xc, INREG32(module_base + 0xc));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x10, INREG32(module_base + 0x10),
		0x18, INREG32(module_base + 0x18),
		0x1c, INREG32(module_base + 0x1c),
		0x020, INREG32(module_base + 0x020));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x024, INREG32(module_base + 0x024),
		0x028, INREG32(module_base + 0x028),
		0x02c, INREG32(module_base + 0x02c),
		0x030, INREG32(module_base + 0x030));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x040, INREG32(module_base + 0x040),
		0x044, INREG32(module_base + 0x044),
		0x048, INREG32(module_base + 0x048),
		0x04c, INREG32(module_base + 0x04c));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x050, INREG32(module_base + 0x050),
		0x060, INREG32(module_base + 0x060),
		0x064, INREG32(module_base + 0x064),
		0x068, INREG32(module_base + 0x068));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x06c, INREG32(module_base + 0x06c),
		0x070, INREG32(module_base + 0x070),
		0x080, INREG32(module_base + 0x080),
		0x084, INREG32(module_base + 0x084));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x088, INREG32(module_base + 0x088),
		0x08c, INREG32(module_base + 0x08c),
		0x090, INREG32(module_base + 0x090),
		0x0a0, INREG32(module_base + 0x0a0));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x0a4, INREG32(module_base + 0x0a4),
		0x0a8, INREG32(module_base + 0x0a8),
		0x0ac, INREG32(module_base + 0x0ac),
		0x0b0, INREG32(module_base + 0x0b0));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x0c0, INREG32(module_base + 0x0c0),
		0x0c4, INREG32(module_base + 0x0c4),
		0x0c8, INREG32(module_base + 0x0c8),
		0x0cc, INREG32(module_base + 0x0cc));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x0d0, INREG32(module_base + 0x0d0),
		0x0e0, INREG32(module_base + 0x0e0),
		0x0e4, INREG32(module_base + 0x0e4),
		0x0e8, INREG32(module_base + 0x0e8));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x0ec, INREG32(module_base + 0x0ec),
		0x0f0, INREG32(module_base + 0x0f0),
		0x100, INREG32(module_base + 0x100),
		0x104, INREG32(module_base + 0x104));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x108, INREG32(module_base + 0x108),
		0x10c, INREG32(module_base + 0x10c),
		0x110, INREG32(module_base + 0x110),
		0x120, INREG32(module_base + 0x120));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x124, INREG32(module_base + 0x124),
		0x128, INREG32(module_base + 0x128),
		0x12c, INREG32(module_base + 0x12c),
		0x130, INREG32(module_base + 0x130));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x140, INREG32(module_base + 0x140),
		0x144, INREG32(module_base + 0x144),
		0x148, INREG32(module_base + 0x148),
		0x14c, INREG32(module_base + 0x14c));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x150, INREG32(module_base + 0x150),
		0x160, INREG32(module_base + 0x160),
		0x164, INREG32(module_base + 0x164),
		0x168, INREG32(module_base + 0x168));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x16c, INREG32(module_base + 0x16c),
		0x170, INREG32(module_base + 0x170),
		0x180, INREG32(module_base + 0x180),
		0x184, INREG32(module_base + 0x184));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x188, INREG32(module_base + 0x188),
		0x18c, INREG32(module_base + 0x18c),
		0x190, INREG32(module_base + 0x190),
		0x300, INREG32(module_base + 0x300));
	DDPDUMP("mutex: 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x304, INREG32(module_base + 0x304),
		0x30c, INREG32(module_base + 0x30c));

	DDPDUMP("-- END: DISP mutex REGS --\n");

}


static void mutex_dump_analysis(void)
{
	int i = 0;
	int j = 0;
	char mutex_module[512] = { '\0' };
	char *p = NULL;
	int len = 0;
	unsigned int val;

	DDPDUMP("== DISP Mutex Analysis ==\n");
	for (i = 0; i < 5; i++) {
		p = mutex_module;
		len = 0;
		if (DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD0(i)) == 0)
			continue;

		val = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_SOF(i));
		len = sprintf(p, "MUTEX%d :SOF=%s,EOF=%s,WAIT=%d,module=(",
			      i,
			      ddp_get_mutex_sof_name(REG_FLD_VAL_GET(SOF_FLD_MUTEX0_SOF, val)),
			      ddp_get_mutex_sof_name(REG_FLD_VAL_GET(SOF_FLD_MUTEX0_EOF, val)),
				REG_FLD_VAL_GET(SOF_FLD_MUTEX0_SOF_WAIT, val));

		p += len;
		for (j = 0; j < 32; j++) {
			unsigned int regval = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD0(i));

			if ((regval & (1 << j))) {
				len = sprintf(p, "%s,", ddp_get_mutex_module0_name(j));
				p += len;
			}
		}
/*		for (j = 0; j < 32; j++) {
			unsigned int regval = DISP_REG_GET(DISP_REG_CONFIG_MUTEX_MOD1(i));

			if ((regval & (1 << j))) {
				len = sprintf(p, "%s,", ddp_get_mutex_module1_name(j));
				p += len;
			}
		}
*/
		DDPDUMP("%s)\n", mutex_module);
	}
}

static void mmsys_config_dump_reg(void)
{
	DDPDUMP("== DISP MMSYS_Config REGS ==\n");
	DDPDUMP("MMSYS_INTEN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_INTEN));
	DDPDUMP("MMSYS_INTSTA=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_INTSTA));
	DDPDUMP("OVL0_MOUT_EN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_OVL0_MOUT_EN));
	DDPDUMP("OVL0_2L_MOUT_EN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_OVL0_2L_MOUT_EN));
	DDPDUMP("OVL1_2L_MOUT_EN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_OVL1_2L_MOUT_EN));
	DDPDUMP("DITHER_MOUT_EN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_DITHER_MOUT_EN));
	DDPDUMP("RSZ_MOUT_EN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_RSZ_MOUT_EN));
	DDPDUMP("MMSYS_MOUT_RST=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_MOUT_RST));
	DDPDUMP("COLOR0_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_COLOR_OUT_SEL_IN));
	DDPDUMP("WDMA0_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_WDMA0_SEL_IN));
	DDPDUMP("DSI0_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_DSI0_SEL_IN));
	DDPDUMP("DISP_PATH_SOUT_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_PATH0_SEL_IN));
	DDPDUMP("RDMA0_SOUT_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_RDMA0_SOUT_SEL_IN));
	DDPDUMP("RDMA1_SOUT_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_RDMA1_SOUT_SEL_IN));
	DDPDUMP("WDMA0_SOUT_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_OVLTOWDMA_SEL_IN));
	DDPDUMP("OVL1_INT_SOUT_SIN=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_OVLTOWDMA_SEL_IN));

	DDPDUMP("MM_MISC=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_MISC));
	DDPDUMP("MM_CG_CON0=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0));
	DDPDUMP("MM_CG_CON1=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));
	DDPDUMP("MM_HW_DCM_DIS0=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_HW_DCM_DIS0));
	DDPDUMP("MM_HW_DCM_DIS1=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_HW_DCM_DIS1));
	DDPDUMP("MM_SW0_RST_B=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_SW0_RST_B));
	DDPDUMP("MM_SW1_RST_B=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_SW1_RST_B));
	DDPDUMP("MM_LCM_RST_B=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_LCM_RST_B));
	DDPDUMP("MM_DBG_OUT_SEL=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DEBUG_OUT_SEL));
	DDPDUMP("MM_DUMMY0=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DUMMY0));
	DDPDUMP("MM_DUMMY1=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DUMMY1));
	DDPDUMP("MM_DUMMY2=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DUMMY2));
	DDPDUMP("MM_DUMMY3=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_MMSYS_DUMMY3));
	DDPDUMP("DISP_VALID_0=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_0));
	DDPDUMP("DISP_VALID_1=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_1));
	DDPDUMP("DISP_READY_0=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_0));
	DDPDUMP("DISP_READY_1=0x%x\n", DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_1));

}


/* ------ clock:
Before power on mmsys:
CLK_CFG_0_CLR (address is 0x10000048) = 0x80000000 (bit 31).
Before using DISP_PWM0 or DISP_PWM1:
CLK_CFG_1_CLR(address is 0x10000058)=0x80 (bit 7).
Before using DPI pixel clock:
CLK_CFG_6_CLR(address is 0x100000A8)=0x80 (bit 7).

Only need to enable the corresponding bits of MMSYS_CG_CON0 and MMSYS_CG_CON1 for the modules:
smi_common, larb0, mdp_crop, fake_eng, mutex_32k, pwm0, pwm1, dsi0, dsi1, dpi.
Other bits could keep 1. Suggest to keep smi_common and larb0 always clock on.

--------valid & ready
example:
ovl0 -> ovl0_mout_ready=1 means engines after ovl_mout are ready for receiving data
	ovl0_mout_ready=0 means ovl0_mout can not receive data, maybe ovl0_mout or after engines config error
ovl0 -> ovl0_mout_valid=1 means engines before ovl0_mout is OK,
	ovl0_mout_valid=0 means ovl can not transfer data to ovl0_mout, means ovl0 or before engines are not ready.
*/

static void mmsys_config_dump_analysis(void)
{
	unsigned int i = 0, j;
	unsigned int reg = 0;
	char clock_on[512] = { '\0' };
	char *pos = NULL;
	const char *name;
	/* int len = 0; */
	unsigned int valid[3], ready[3];
	unsigned int greq;

	valid[0] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_0);
	valid[1] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_1);
	valid[2] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_VALID_2);
	ready[0] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_0);
	ready[1] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_1);
	ready[2] = DISP_REG_GET(DISP_REG_CONFIG_DISP_DL_READY_2);
	greq = DISP_REG_GET(DISP_REG_CONFIG_SMI_LARB0_GREQ);

	DDPDUMP("== DISP MMSYS_CONFIG ANALYSIS ==\n");
#if 0 /* TODO: mmsys clk?? */
	DDPDUMP("mmsys clock=0x%x, CG_CON0=0x%x, CG_CON1=0x%x\n",
		DISP_REG_GET(DISP_REG_CLK_CFG_0_MM_CLK),
		DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
		DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));
	if ((DISP_REG_GET(DISP_REG_CLK_CFG_0_MM_CLK) >> 31) & 0x1)
		DDPERR("mmsys clock abnormal!!\n");
#endif
	DDPDUMP("MMSYS_CG_CON0: 0x%08x\n", INREG32(DISP_REG_CONFIG_MMSYS_CG_CON0));
	DDPDUMP("MMSYS_CG_CON1: 0x%08x\n", INREG32(DISP_REG_CONFIG_MMSYS_CG_CON1));

	reg = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0);
	for (i = 0; i < 32; i++) {
		if ((reg & (1 << i)) == 0) {
			name = ddp_clock_0(i);
			if (name)
				strncat(clock_on, name, sizeof(clock_on));
		}
	}

	reg = DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1);
	for (i = 0; i < 32; i++) {
		if ((reg & (1 << i)) == 0) {
			name = ddp_clock_1(i);
			if (name)
				strncat(clock_on, name, sizeof(clock_on));
		}
	}
	DDPDUMP("clock on modules:%s\n", clock_on);

	DDPDUMP("valid0=0x%x,valid1=0x%x,valid2=0x%x,ready0=0x%x,ready1=0x%x,ready2=0x%x,greq=0%x\n",
		valid[0], valid[1], valid[2], ready[0], ready[1], ready[2], greq);

	for (j = 0; j < ARRAY_SIZE(valid); j++) {
		for (i = 0; i < 32; i++) {
			name = ddp_signal[j][i];
			if (!name)
				continue;

			pos = clock_on;

			if ((valid[j] & (1 << i)))
				pos += snprintf(pos, sizeof(clock_on), "%s,", "v");
			else
				pos += snprintf(pos, sizeof(clock_on), "%s,", "n");

			if ((ready[j] & (1 << i)))
				pos += snprintf(pos, sizeof(clock_on), "%s", "r");
			else
				pos += snprintf(pos, sizeof(clock_on), "%s", "n");

			pos += snprintf(pos, sizeof(clock_on), ": %s", name);

			DDPDUMP("%s\n", clock_on);
		}
	}

	/* greq: 1 means SMI dose not grant, maybe SMI hang */
	if (greq)
		DDPDUMP("smi greq not grant module: (greq: 1 means SMI dose not grant, maybe SMI hang)");

	clock_on[0] = '\0';
	for (i = 0; i < 32; i++) {
		if (greq & (1 << i)) {
			name = ddp_greq_name(i);
			if (!name)
				continue;
			strncat(clock_on, name, sizeof(clock_on));
		}
	}
	DDPDUMP("%s\n", clock_on);
}

static void gamma_dump_reg(void)
{
	DDPDUMP("== DISP GAMMA REGS ==\n");
	DDPDUMP("(0x000)GA_EN=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_EN));
	DDPDUMP("(0x004)GA_RESET=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_RESET));
	DDPDUMP("(0x008)GA_INTEN=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_INTEN));
	DDPDUMP("(0x00c)GA_INTSTA=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_INTSTA));
	DDPDUMP("(0x010)GA_STATUS=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_STATUS));
	DDPDUMP("(0x020)GA_CFG=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_CFG));
	DDPDUMP("(0x024)GA_IN_COUNT=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_INPUT_COUNT));
	DDPDUMP("(0x028)GA_OUT_COUNT=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_OUTPUT_COUNT));
	DDPDUMP("(0x02c)GA_CHKSUM=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_CHKSUM));
	DDPDUMP("(0x030)GA_SIZE=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_SIZE));
	DDPDUMP("(0x0c0)GA_DUMMY_REG=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_DUMMY_REG));
	DDPDUMP("(0x800)GA_LUT=0x%x\n", DISP_REG_GET(DISP_REG_GAMMA_LUT));
}

static void gamma_dump_analysis(void)
{
	DDPDUMP("== DISP GAMMA ANALYSIS ==\n");
	DDPDUMP("gamma: en=%d, w=%d, h=%d, in_p_cnt=%d, in_l_cnt=%d, out_p_cnt=%d, out_l_cnt=%d\n",
		DISP_REG_GET(DISP_REG_GAMMA_EN),
		(DISP_REG_GET(DISP_REG_GAMMA_SIZE) >> 16) & 0x1fff,
		DISP_REG_GET(DISP_REG_GAMMA_SIZE) & 0x1fff,
		DISP_REG_GET(DISP_REG_GAMMA_INPUT_COUNT) & 0x1fff,
		(DISP_REG_GET(DISP_REG_GAMMA_INPUT_COUNT) >> 16) & 0x1fff,
		DISP_REG_GET(DISP_REG_GAMMA_OUTPUT_COUNT) & 0x1fff,
		(DISP_REG_GET(DISP_REG_GAMMA_OUTPUT_COUNT) >> 16) & 0x1fff);
}

static void color_dump_reg(DISP_MODULE_ENUM module)
{
	int index = 0;

	if (DISP_MODULE_COLOR0 == module) {
		index = 0;
	} else if (DISP_MODULE_COLOR1 == module) {
		DDPDUMP("error: DISP COLOR%d dose not exist!\n", index);
		return;
	}
	DDPDUMP("== DISP COLOR%d REGS ==\n", index);
	DDPDUMP("(0x400)COLOR_CFG_MAIN=0x%x\n", DISP_REG_GET(DISP_COLOR_CFG_MAIN));
	DDPDUMP("(0x404)COLOR_PXL_CNT_MAIN=0x%x\n", DISP_REG_GET(DISP_COLOR_PXL_CNT_MAIN));
	DDPDUMP("(0x408)COLOR_LINE_CNT_MAIN=0x%x\n", DISP_REG_GET(DISP_COLOR_LINE_CNT_MAIN));
	DDPDUMP("(0xc00)COLOR_START=0x%x\n", DISP_REG_GET(DISP_COLOR_START));
	DDPDUMP("(0xc28)DISP_COLOR_CK_ON=0x%x\n", DISP_REG_GET(DISP_COLOR_CK_ON));
	DDPDUMP("(0xc50)COLOR_INTER_IP_W=0x%x\n", DISP_REG_GET(DISP_COLOR_INTERNAL_IP_WIDTH));
	DDPDUMP("(0xc54)COLOR_INTER_IP_H=0x%x\n", DISP_REG_GET(DISP_COLOR_INTERNAL_IP_HEIGHT));
	return;
}

static void color_dump_analysis(DISP_MODULE_ENUM module)
{
	int index = 0;

	if (DISP_MODULE_COLOR0 == module) {
		index = 0;
	} else if (DISP_MODULE_COLOR1 == module) {
		DDPDUMP("error: DISP COLOR%d dose not exist!\n", index);
		return;
	}
	DDPDUMP("== DISP COLOR%d ANALYSIS ==\n", index);
	DDPDUMP("color%d: bypass=%d, w=%d, h=%d, pixel_cnt=%d, line_cnt=%d,\n",
		index,
		(DISP_REG_GET(DISP_COLOR_CFG_MAIN) >> 7) & 0x1,
		DISP_REG_GET(DISP_COLOR_INTERNAL_IP_WIDTH),
		DISP_REG_GET(DISP_COLOR_INTERNAL_IP_HEIGHT),
		DISP_REG_GET(DISP_COLOR_PXL_CNT_MAIN) & 0xffff,
		(DISP_REG_GET(DISP_COLOR_LINE_CNT_MAIN) >> 16) & 0x1fff);

	return;
}

static void aal_dump_reg(void)
{
	DDPDUMP("== DISP AAL REGS ==\n");
	DDPDUMP("(0x000)AAL_EN=0x%x\n", DISP_REG_GET(DISP_AAL_EN));
	DDPDUMP("(0x008)AAL_INTEN=0x%x\n", DISP_REG_GET(DISP_AAL_INTEN));
	DDPDUMP("(0x00c)AAL_INTSTA=0x%x\n", DISP_REG_GET(DISP_AAL_INTSTA));
	DDPDUMP("(0x020)AAL_CFG=0x%x\n", DISP_REG_GET(DISP_AAL_CFG));
	DDPDUMP("(0x024)AAL_IN_CNT=0x%x\n", DISP_REG_GET(DISP_AAL_IN_CNT));
	DDPDUMP("(0x028)AAL_OUT_CNT=0x%x\n", DISP_REG_GET(DISP_AAL_OUT_CNT));
	DDPDUMP("(0x030)AAL_SIZE=0x%x\n", DISP_REG_GET(DISP_AAL_SIZE));
	DDPDUMP("(0x20c)AAL_CABC_00=0x%x\n", DISP_REG_GET(DISP_AAL_CABC_00));
	DDPDUMP("(0x214)AAL_CABC_02=0x%x\n", DISP_REG_GET(DISP_AAL_CABC_02));
	DDPDUMP("(0x20c)AAL_STATUS_00=0x%x\n", DISP_REG_GET(DISP_AAL_STATUS_00));
	DDPDUMP("(0x210)AAL_STATUS_01=0x%x\n", DISP_REG_GET(DISP_AAL_STATUS_00 + 0x4));
	DDPDUMP("(0x2a0)AAL_STATUS_31=0x%x\n", DISP_REG_GET(DISP_AAL_STATUS_32 - 0x4));
	DDPDUMP("(0x2a4)AAL_STATUS_32=0x%x\n", DISP_REG_GET(DISP_AAL_STATUS_32));
	DDPDUMP("(0x3b0)AAL_DRE_MAPPING_00=0x%x\n", DISP_REG_GET(DISP_AAL_DRE_MAPPING_00));
	return;
}

static void aal_dump_analysis(void)
{
	DDPDUMP("== DISP AAL ANALYSIS ==\n");
	DDPDUMP("aal: bypass=%d, relay=%d, en=%d, w=%d, h=%d, in(%d,%d),out(%d,%d)\n",
		DISP_REG_GET(DISP_AAL_EN) == 0x0,
		DISP_REG_GET(DISP_AAL_CFG) & 0x01,
		DISP_REG_GET(DISP_AAL_EN),
		(DISP_REG_GET(DISP_AAL_SIZE) >> 16) & 0x1fff,
		DISP_REG_GET(DISP_AAL_SIZE) & 0x1fff,
		DISP_REG_GET(DISP_AAL_IN_CNT) & 0x1fff,
		(DISP_REG_GET(DISP_AAL_IN_CNT) >> 16) & 0x1fff,
		DISP_REG_GET(DISP_AAL_OUT_CNT) & 0x1fff,
		(DISP_REG_GET(DISP_AAL_OUT_CNT) >> 16) & 0x1fff);
}

static void pwm_dump_reg(DISP_MODULE_ENUM module)
{
	int index = 0;
	unsigned long reg_base = 0;

	if (module == DISP_MODULE_PWM0) {
		index = 0;
		reg_base = DISPSYS_PWM0_BASE;
	} else {
		index = 1;
		reg_base = DISPSYS_PWM0_BASE;
	}
	DDPDUMP("== DISP PWM%d REGS ==\n", index);
	DDPDUMP("(0x000)PWM_EN=0x%x\n", DISP_REG_GET(reg_base + DISP_PWM_EN_OFF));
	DDPDUMP("(0x008)PWM_CON_0=0x%x\n", DISP_REG_GET(reg_base + DISP_PWM_CON_0_OFF));
	DDPDUMP("(0x010)PWM_CON_1=0x%x\n", DISP_REG_GET(reg_base + DISP_PWM_CON_1_OFF));
	DDPDUMP("(0x028)PWM_DEBUG=0x%x\n", DISP_REG_GET(reg_base + 0x28));
	return;
}

static void pwm_dump_analysis(DISP_MODULE_ENUM module)
{
	int index = 0;
	//unsigned int reg_base = 0;

	if (module == DISP_MODULE_PWM0) {
		index = 0;
		//reg_base = DISPSYS_PWM0_BASE;
	} else {
		index = 1;
		//reg_base = DISPSYS_PWM0_BASE;
	}
	DDPDUMP("== DISP PWM%d ANALYSIS ==\n", index);
#if 0 /* TODO: clk reg?? */
	DDPDUMP("pwm clock=%d\n", (DISP_REG_GET(DISP_REG_CLK_CFG_1_CLR) >> 7) & 0x1);
#endif

	return;
}

static void ccorr_dump_reg(void)
{
	DDPDUMP("== DISP CCORR REGS ==\n");
	DDPDUMP("(00)EN=0x%x\n", DISP_REG_GET(DISP_REG_CCORR_EN));
	DDPDUMP("(20)CFG=0x%x\n", DISP_REG_GET(DISP_REG_CCORR_CFG));
	DDPDUMP("(24)IN_CNT=0x%x\n", DISP_REG_GET(DISP_REG_CCORR_IN_CNT));
	DDPDUMP("(28)OUT_CNT=0x%x\n", DISP_REG_GET(DISP_REG_CCORR_OUT_CNT));
	DDPDUMP("(30)SIZE=0x%x\n", DISP_REG_GET(DISP_REG_CCORR_SIZE));

}

static void ccorr_dump_analyze(void)
{
	DDPDUMP("ccorr: en=%d, config=%d, w=%d, h=%d, in_p_cnt=%d, in_l_cnt=%d, out_p_cnt=%d, out_l_cnt=%d\n",
	     DISP_REG_GET(DISP_REG_CCORR_EN), DISP_REG_GET(DISP_REG_CCORR_CFG),
	     (DISP_REG_GET(DISP_REG_CCORR_SIZE) >> 16) & 0x1fff,
	     DISP_REG_GET(DISP_REG_CCORR_SIZE) & 0x1fff,
	     DISP_REG_GET(DISP_REG_CCORR_IN_CNT) & 0x1fff,
	     (DISP_REG_GET(DISP_REG_CCORR_IN_CNT) >> 16) & 0x1fff,
	     DISP_REG_GET(DISP_REG_CCORR_IN_CNT) & 0x1fff,
	     (DISP_REG_GET(DISP_REG_CCORR_IN_CNT) >> 16) & 0x1fff);
}

static void dither_dump_reg(void)
{
	DDPDUMP("== DISP DITHER REGS ==\n");
	DDPDUMP("(00)EN=0x%x\n", DISP_REG_GET(DISP_REG_DITHER_EN));
	DDPDUMP("(20)CFG=0x%x\n", DISP_REG_GET(DISP_REG_DITHER_CFG));
	DDPDUMP("(24)IN_CNT=0x%x\n", DISP_REG_GET(DISP_REG_DITHER_IN_CNT));
	DDPDUMP("(28)OUT_CNT=0x%x\n", DISP_REG_GET(DISP_REG_DITHER_OUT_CNT));
	DDPDUMP("(30)SIZE=0x%x\n", DISP_REG_GET(DISP_REG_DITHER_SIZE));
}

static void dither_dump_analyze(void)
{
	DDPDUMP("dither: en=%d, config=%d, w=%d, h=%d, in_p_cnt=%d, in_l_cnt=%d, out_p_cnt=%d, out_l_cnt=%d\n",
		 DISP_REG_GET(DISPSYS_DITHER0_BASE + 0x000), DISP_REG_GET(DISPSYS_DITHER0_BASE + 0x020),
		 (DISP_REG_GET(DISP_REG_DITHER_SIZE) >> 16) & 0x1fff,
		 DISP_REG_GET(DISP_REG_DITHER_SIZE) & 0x1fff,
		 DISP_REG_GET(DISP_REG_DITHER_IN_CNT) & 0x1fff,
		 (DISP_REG_GET(DISP_REG_DITHER_IN_CNT) >> 16) & 0x1fff,
		 DISP_REG_GET(DISP_REG_DITHER_OUT_CNT) & 0x1fff,
		 (DISP_REG_GET(DISP_REG_DITHER_OUT_CNT) >> 16) & 0x1fff);

}

static void dsi_dump_reg(DISP_MODULE_ENUM module)
{
	DSI_DumpRegisters(module, 1);
#if 0
	if (DISP_MODULE_DSI0) {
		int i = 0;

		DDPDUMP("== DISP DSI0 REGS ==\n");
		for (i = 0; i < 25 * 16; i += 16) {
			DDPDUMP("DSI0+%04x: 0x%08x 0x%08x 0x%08x 0x%08x\n", i,
			       INREG32(DISPSYS_DSI0_BASE + i), INREG32(DISPSYS_DSI0_BASE + i + 0x4),
			       INREG32(DISPSYS_DSI0_BASE + i + 0x8),
			       INREG32(DISPSYS_DSI0_BASE + i + 0xc));
		}
		DDPDUMP("DSI0 CMDQ+0x200: 0x%08x 0x%08x 0x%08x 0x%08x\n",
		       INREG32(DISPSYS_DSI0_BASE + 0x200), INREG32(DISPSYS_DSI0_BASE + 0x200 + 0x4),
		       INREG32(DISPSYS_DSI0_BASE + 0x200 + 0x8),
		       INREG32(DISPSYS_DSI0_BASE + 0x200 + 0xc));
	}
#endif
}

static void dpi_dump_reg(void)
{
	int i;

	DDPDUMP("-- Start dump DPI registers --\n");

	for (i = 0; i <= 0x40; i += 4)
		DDPDUMP("DPI+%04x: 0x%08x\n", i, INREG32(DISPSYS_DPI_BASE + i));

	for (i = 0x68; i <= 0x7C; i += 4)
		DDPDUMP("DPI+%04x: 0x%08x\n", i, INREG32(DISPSYS_DPI_BASE + i));

	DDPDUMP("DPI+Color Bar    : 0x%04x : 0x%08x\n", 0xF00, INREG32(DISPSYS_DPI_BASE + 0xF00));
	DDPDUMP("DPI MMSYS_CG_CON0: 0x%08x\n", INREG32(DISP_REG_CONFIG_MMSYS_CG_CON0));
	DDPDUMP("DPI MMSYS_CG_CON1: 0x%08x\n", INREG32(DISP_REG_CONFIG_MMSYS_CG_CON1));
}

static void dpi_dump_analysis(void)
{
#if 0
	DDPDUMP("== DISP DPI ANALYSIS ==\n");
	/* TODO: mmsys clk?? */
	DDPDUMP("DPI clock=0x%x\n", DISP_REG_GET(DISP_REG_CLK_CFG_6_DPI));
	DDPDUMP("DPI  clock_clear=%d\n", (DISP_REG_GET(DISP_REG_CLK_CFG_6_CLR) >> 7) & 0x1);
#endif
}
static int split_dump_regs(DISP_MODULE_ENUM module)
{
	unsigned long module_base = DISPSYS_SPLIT0_BASE;

	DDPDUMP("== START: DISP %s REGS ==\n", ddp_get_module_name(module));
	DDPDUMP("split: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x000, INREG32(module_base + 0x000),
		0x004, INREG32(module_base + 0x004),
		0x008, INREG32(module_base + 0x008),
		0x00c, INREG32(module_base + 0x00c));
	DDPDUMP("split: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x010, INREG32(module_base + 0x010),
		0x020, INREG32(module_base + 0x020),
		0x024, INREG32(module_base + 0x024),
		0x028, INREG32(module_base + 0x028));
	DDPDUMP("split: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x02c, INREG32(module_base + 0x02c),
		0x030, INREG32(module_base + 0x030),
		0x034, INREG32(module_base + 0x034),
		0x038, INREG32(module_base + 0x038));
	DDPDUMP("split: 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x, 0x%04x=0x%08x\n",
		0x03c, INREG32(module_base + 0x03c),
		0x040, INREG32(module_base + 0x040),
		0x044, INREG32(module_base + 0x044),
		0x48, INREG32(module_base + 0x48));
	DDPDUMP("split: 0x%04x=0x%08x\n",
		0x080, INREG32(module_base + 0x080));

	DDPDUMP("-- END: DISP %s REGS --\n", ddp_get_module_name(module));

	return 0;
}
static int split_dump_analysis(DISP_MODULE_ENUM module)
{
	unsigned long module_base = DISPSYS_SPLIT0_BASE;

	DDPMSG("== DISP SPLIT0 ANALYSIS ==\n");
	DDPDUMP("split: en:%d, input_line=%d, input_pixel=%d, \
			left_output_line=%d, left_output_pixel=%d, \
			right_output_line=%d, right_output_pixel=%d\n",
			DISP_REG_GET(module_base + 0x0) & 0x1,
			(DISP_REG_GET(module_base + 0x24) >> 16) & 0x1fff,
			DISP_REG_GET(module_base + 0x24) & 0x1fff,
			DISP_REG_GET(module_base + 0x28) & 0x1fff,
			(DISP_REG_GET(module_base + 0x28) >> 16) & 0x1fff,
			DISP_REG_GET(module_base + 0x2c) & 0x1fff,
			(DISP_REG_GET(module_base + 0x2c) >> 16) & 0x1fff);
	return 0;
}

int ddp_dump_reg(DISP_MODULE_ENUM module)
{
	switch (module) {
	case DISP_MODULE_WDMA0:
		//wdma_dump_reg(module);
		break;
	case DISP_MODULE_RDMA0:
	case DISP_MODULE_RDMA1:
	case DISP_MODULE_RDMA2:
		RDMADump(module);
		break;
	case DISP_MODULE_OVL0:
	case DISP_MODULE_OVL1:
	case DISP_MODULE_OVL0_2L:
	case DISP_MODULE_OVL1_2L:
		OVLDump(module);
		break;
	case DISP_MODULE_GAMMA0:
		gamma_dump_reg();
		break;
	case DISP_MODULE_CONFIG:
		mmsys_config_dump_reg();
		break;
	case DISP_MODULE_MUTEX:
		mutex_dump_reg();
		break;
	case DISP_MODULE_COLOR0:
	case DISP_MODULE_COLOR1:
		color_dump_reg(module);
		break;
	case DISP_MODULE_AAL0:
		aal_dump_reg();
		break;
	case DISP_MODULE_PWM0:
		pwm_dump_reg(module);
		break;
	case DISP_MODULE_DSI0:
	case DISP_MODULE_DSI1:
	case DISP_MODULE_DSIDUAL:
		dsi_dump_reg(module);
		break;
	case DISP_MODULE_DPI:
		dpi_dump_reg();
		break;
	case DISP_MODULE_CCORR0:
		ccorr_dump_reg();
		break;
	case DISP_MODULE_DITHER0:
		dither_dump_reg();
		break;
	case DISP_MODULE_SPLIT0:
		split_dump_regs(module);
		break;
	default:
		DDPDUMP("no dump_reg for module %s(%d)\n", ddp_get_module_name(module), module);
	}
	return 0;
}

int ddp_dump_analysis(DISP_MODULE_ENUM module)
{
	switch (module) {
	case DISP_MODULE_WDMA0:
		//wdma_dump_analysis(module);
		break;
	case DISP_MODULE_RDMA0:
	case DISP_MODULE_RDMA1:
	case DISP_MODULE_RDMA2:
		rdma_dump_analysis(module);
		break;
	case DISP_MODULE_OVL0:
	case DISP_MODULE_OVL1:
	case DISP_MODULE_OVL0_2L:
	case DISP_MODULE_OVL1_2L:
		ovl_dump_analysis(module);
		break;
	case DISP_MODULE_GAMMA0:
		gamma_dump_analysis();
		break;
	case DISP_MODULE_CONFIG:
		mmsys_config_dump_analysis();
		break;
	case DISP_MODULE_MUTEX:
		mutex_dump_analysis();
		break;
	case DISP_MODULE_COLOR0:
	case DISP_MODULE_COLOR1:
		color_dump_analysis(module);
		break;
	case DISP_MODULE_AAL0:
		aal_dump_analysis();
		break;
	case DISP_MODULE_PWM0:
		pwm_dump_analysis(module);
		break;
	case DISP_MODULE_DSI0:
	case DISP_MODULE_DSI1:
	case DISP_MODULE_DSIDUAL:
		dsi_analysis(module);
		break;
	case DISP_MODULE_DPI:
		dpi_dump_analysis();
		break;
	case DISP_MODULE_CCORR0:
		ccorr_dump_analyze();
		break;
	case DISP_MODULE_DITHER0:
		dither_dump_analyze();
		break;
	case DISP_MODULE_SPLIT0:
		split_dump_analysis(module);
		break;
	default:
		DDPDUMP("no dump_analysis for module %s(%d)\n", ddp_get_module_name(module),
			module);
	}
	return 0;
}
