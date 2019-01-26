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

#include <reg.h>
#include <latch.h>
#include <debug.h>
#include <mt_reg_base.h>
#include <mt_typedefs.h>
#include "dfd.h"
#include <dev/aee_platform_debug.h>

int plt_perisys_init(const struct plt_cfg_bus_latch *self)
{
	unsigned int timeout_26m_mask = 0;
	unsigned int timeout_133m_mask = 0;
	unsigned int addr = 0;

	if (!self) {
		dprintf(CRITICAL, "%s:%d: self is Null\n",
			__func__, __LINE__);
		return -1;
	}

	/*only monitor MD*/
	if (self->num_infrasys_mon != 0) {
		addr = INFRACFG_AO_BASE + self->infrasys_offsets.bus_infra_mask_l;
		writel(0x0, addr);
		addr = INFRACFG_AO_BASE + self->infrasys_offsets.bus_infra_mask_h;
		writel(0x0, addr);
	}

	writel(self->perisys_eventmask, PERICFG_BASE + self->perisys_offsets.bus_peri_r1);
	writel(self->perisys_timeout | self->perisys_enable, PERICFG_BASE + self->perisys_offsets.bus_peri_r0);
	timeout_26m_mask = self->infra_timeout_26m & 0xffff;
	timeout_26m_mask = timeout_26m_mask | ((self->peri_timeout_26m & 0xffff) << 16);
	writel(timeout_26m_mask, 0x10a20e24); //for SPM's SRAM.  for 26M axi clock. [31:16]peri, [15:0]infra
	timeout_133m_mask = self->infrasys_config & 0xffff;
	timeout_133m_mask = timeout_133m_mask | ((self->perisys_timeout & 0xffff) << 16);
	writel(timeout_133m_mask, 0x10a20e34); //for SPM's SRAM.  for 133M axi clock [31:16]peri, [15:0]infra
	dprintf(CRITICAL, "[last bus]26M:0x%x, 133M: 0x%x\n", timeout_26m_mask, timeout_133m_mask);
	return 0;
}

int plt_infrasys_is_timeout(const struct plt_cfg_bus_latch *self)
{
        return 1;
}


/*
 * The peripheral debug mointor in Vinson don't have timeout bit,
 * so I have to regard infra monitor's timeout bit as its timeout bit.
 */
int plt_is_perisys_timeout(const struct plt_cfg_bus_latch *self)
{
	unsigned long infra_base = INFRACFG_AO_BASE;
	unsigned int timeout;

	return 1;

	if (self->infrasys_ops.is_timeout)
		timeout = self->infrasys_ops.is_timeout(self);
	else
		timeout = readl(infra_base + self->infrasys_offsets.bus_infra_ctrl) & 0xFF000000;

	if(timeout)
		return 1;
	else
		return 0;
}

static unsigned long preisys_dump_offset[] = {
	0x0f4, /* PERIBUS_DBG0 */
	0x0f8, /* PERIBUS_DBG1 */
	0x0fc, /* PERIBUS_DBG2 */
	0x100, /* PERIBUS_DBG3 */
	0x104, /* PERIBUS_DBG4 */
	0x108, /* PERIBUS_DBG5 */
	0x10c, /* PERIBUS_DBG6 */
	0x110, /* PERIBUS_DBG7 */
	0x114, /* PERIBUS_DBG8 */
	0x118, /* PERIBUS_DBG9 */
	0x15c, /* PERIBUS_DBG10 */
	0x160, /* PERIBUS_DBG11 */
	0x174, /* PERIBUS_DBG12 */
	0x178, /* PERIBUS_DBG13 */
	0x17c, /* PERIBUS_DBG14 */
	0x190, /* PERIBUS_DBG15 */
	0x194, /* PERIBUS_DBG16 */
};

int plt_lastbus_perisys_dump(const struct plt_cfg_bus_latch *self,
			     char *buf, int *wp)
{
	unsigned int i;
	unsigned long peri_base = PERICFG_BASE;
	unsigned long dump_size = sizeof(preisys_dump_offset) / sizeof(unsigned long);
	unsigned long reg, data;

	*wp += snprintf(buf + *wp, LATCH_BUF_LENGTH - *wp, "[LAST BUS] PERISYS TIMEOUT:\n");
	for (i = 0; i < dump_size; ++i) {
		reg = peri_base + preisys_dump_offset[i];
		data = readl(reg);
		*wp += snprintf(buf + *wp, LATCH_BUF_LENGTH - *wp,
				"PERIBUS_DBG%d(0x%x) = %04x\n",
				i, reg, data);
	}

	return 0;
}

void plt_setup_dfd_file_name(struct plt_cfg_dfd *cfg_dfd)
{
	struct aee_db_file_info *files;

	files = get_file_info();
	strcpy(files[AEE_PLAT_DFD20].filename, "DFD30.dfd");
	files[AEE_PLAT_DFD20].filesize = cfg_dfd->buffer_length;
}
