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

#ifndef _MT_SPM_
#define _MT_SPM_

#include <platform/mt_typedefs.h>

#define SPM_BASE (0x10A00000)
#define INFRACFG_AO_BASE (0x10000000)

#define STA_POWER_DOWN	0
#define STA_POWER_ON	1

#define SPM_REG(offset)         (SPM_BASE + offset)
#define SPM_PROJECT_CODE    0xB16
#define POWERON_CONFIG_EN	SPM_REG(0x0000)
#define PWR_STATUS	SPM_REG(0x0190)
#define PWR_STATUS_2ND	SPM_REG(0x0194)
#define C2K_PWR_CON	SPM_REG(0x0314)
#define MD1_PWR_CON	SPM_REG(0x0318)
#define AUD_PWR_CON	SPM_REG(0x032C)
#define EXT_BUCK_ISO	SPM_REG(0x0394)

#define INFRACFG_REG(offset)		(INFRACFG_AO_BASE + offset)
#define INFRA_TOPAXI_PROTECTSTA1_1	INFRACFG_REG(0x0258)
#define INFRA_TOPAXI_PROTECTEN_1_SET	INFRACFG_REG(0x02A8)
#define INFRA_TOPAXI_PROTECTEN_1_CLR	INFRACFG_REG(0x02AC)

#define PWR_RST_B                        (0x1 << 0)
#define PWR_ISO                          (0x1 << 1)
#define PWR_ON                           (0x1 << 2)
#define PWR_ON_2ND                       (0x1 << 3)
#define PWR_CLK_DIS                      (0x1 << 4)

/* Define MTCMOS Bus Protect Mask */
#define C2K_PROT_BIT_MASK                ((0x1 << 22) \
					  |(0x1 << 23) \
					  |(0x1 << 24) \
					  |(0x1 << 25))
#define C2K_PROT_BIT_ACK_MASK            ((0x1 << 22) \
					  |(0x1 << 23) \
					  |(0x1 << 24) \
					  |(0x1 << 25))
#define MD1_PROT_BIT_MASK                ((0x1 << 16) \
					  |(0x1 << 17) \
					  |(0x1 << 18) \
					  |(0x1 << 19) \
					  |(0x1 << 21) \
					  |(0x1 << 25) \
					  |(0x1 << 28))
#define MD1_PROT_BIT_ACK_MASK            ((0x1 << 16) \
					  |(0x1 << 17) \
					  |(0x1 << 18) \
					  |(0x1 << 19) \
					  |(0x1 << 21) \
					  |(0x1 << 25))

/* Define MTCMOS Power Status Mask */
#define C2K_PWR_STA_MASK                 (0x1 << 6)
#define MD1_PWR_STA_MASK                 (0x1 << 7)
#define AUD_PWR_STA_MASK                 (0x1 << 12)

/* Define Non-CPU SRAM Mask */
#define MD1_SRAM_PDN                     (0x7 << 8)
#define MD1_SRAM_PDN_ACK                 (0x0 << 24)
#define MD1_SRAM_PDN_ACK_BIT0            (0x1 << 24)
#define MD1_SRAM_PDN_ACK_BIT1            (0x1 << 25)
#define MD1_SRAM_PDN_ACK_BIT2            (0x1 << 26)
#define AUD_SRAM_PDN                     (0xF << 8)
#define AUD_SRAM_PDN_ACK                 (0xF << 24)
#define AUD_SRAM_PDN_ACK_BIT0            (0x1 << 24)
#define AUD_SRAM_PDN_ACK_BIT1            (0x1 << 25)
#define AUD_SRAM_PDN_ACK_BIT2            (0x1 << 26)
#define AUD_SRAM_PDN_ACK_BIT3            (0x1 << 27)

/**************************************
 * Macro and Inline
 **************************************/
#define spm_read(addr)          DRV_Reg32(addr)
#define spm_write(addr, val)    DRV_WriteReg32(addr, val)

extern int spm_mtcmos_ctrl_md1(int state);
extern int spm_mtcmos_ctrl_c2k(int state);
extern int spm_mtcmos_ctrl_aud(int state);

#endif
