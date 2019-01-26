/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2017. All rights reserved.
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
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/

#include <stdint.h>
#include <printf.h>
#include <string.h>
#include <platform/errno.h>
#include <platform/mmc_core.h>
#include <platform/partition.h>
#include "partition_wp.h"
#if defined(MTK_UFS_BOOTING)
#include "ufs_aio_core.h"
#include "ufs_aio_rpmb.h"
#endif

int partition_write_prot_set(const char *start_part_name, const char *end_part_name, STORAGE_WP_TYPE type)
{
	int err = 0;
	part_t start, end;
	part_t *pstart, *pend;
	unsigned long nr_wp_sects;

	pstart = mt_part_get_partition((char *)start_part_name);
	if (!pstart) {
		dprintf(CRITICAL, "[Part_WP]Not found start partition %s info\n", start_part_name);
		err = -EINVAL;
		goto out;
	}
	memcpy(&start, pstart, sizeof(start));

	pend = mt_part_get_partition((char *)end_part_name);
	if (!pend) {
		dprintf(CRITICAL, "[Part_WP]Not found end partition %s info\n", end_part_name);
		err = -EINVAL;
		goto out;
	}
	memcpy(&end, pend, sizeof(end));

	if (start.part_id != end.part_id) {
		dprintf(CRITICAL, "[Part_WP]WP Range(region): %d->%d\n", start.part_id, end.part_id);
		err = -EINVAL;
		goto out;
	}

	if (start.start_sect > end.start_sect) {
		dprintf(CRITICAL, "[Part_WP]WP Range(block): 0x%lx->0x%lx\n", start.start_sect, end.start_sect);
		err = -EINVAL;
		goto out;
	}

	nr_wp_sects = end.start_sect + end.nr_sects - start.start_sect;
	dprintf(CRITICAL, "[Part_WP]WP(%s->%s): Region(%d), start_sect(0x%lx), nr_blk(0x%lx), type(%d)\n",
	        start_part_name, end_part_name, start.part_id, start.start_sect, nr_wp_sects, type);
#if defined(MTK_EMMC_SUPPORT)
	err = mmc_set_write_protect(0, start.part_id, start.start_sect, nr_wp_sects, type);
#elif defined(MTK_UFS_BOOTING)
	err = ufs_set_write_protect(0, start.part_id, start.start_sect, nr_wp_sects, type);
#endif

out:
	return err;
}
