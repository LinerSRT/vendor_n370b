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

#include <platform/spm.h>

/**************************************
 * Define and Declare
 **************************************/

#define SPM_BASE                    SLEEP_BASE
#define SPM_REG(offset)             (SPM_BASE + offset)
#define INFRACFG_REG(offset)        (INFRACFG_AO_BASE + offset)
#define SMI_COMMON_REG(offset)      (SMI_COMMON_BASE + offset)

#define POWERON_CONFIG_EN           SPM_REG(0x000)
#define PWR_STATUS                  SPM_REG(0x190)
#define PWR_STATUS_2ND              SPM_REG(0x194)
#define MD1_PWR_CON                 SPM_REG(0x318)
#define C2K_PWR_CON                 SPM_REG(0x314)
#define AUD_PWR_CON                 SPM_REG(0x32C)
#define MM0_PWR_CON                 SPM_REG(0x334)
#define ISP_PWR_CON                 SPM_REG(0x340)
#define EXT_BUCK_ISO                SPM_REG(0x394)

#define INFRA_TOPAXI_PROTECTSTA1	INFRACFG_REG(0x0228)
#define INFRA_TOPAXI_PROTECTEN_SET	INFRACFG_REG(0x02A0)
#define INFRA_TOPAXI_PROTECTEN_CLR	INFRACFG_REG(0x02A4)

#define INFRA_TOPAXI_PROTECTSTA1_1	INFRACFG_REG(0x0258)
#define INFRA_TOPAXI_PROTECTEN_1_SET	INFRACFG_REG(0x02A8)
#define INFRA_TOPAXI_PROTECTEN_1_CLR	INFRACFG_REG(0x02AC)

#define INFRA_TOPAXI_PROTECTEN_2_SET	INFRACFG_REG(0x02C8)
#define INFRA_TOPAXI_PROTECTSTA1_2      INFRACFG_REG(0x02D4)
#define INFRA_TOPAXI_PROTECTEN_2_CLR	INFRACFG_REG(0x02CC)


#define SMI_CLAMP                       SMI_COMMON_REG(0x03C0)
#define SMI_CLAMP_SET                   SMI_COMMON_REG(0x03C4)
#define SMI_CLAMP_CLR                   SMI_COMMON_REG(0x03C8)



#define  SPM_PROJECT_CODE		0xB16

/* Define MTCMOS power control */
#define PWR_RST_B                        (0x1 << 0)
#define PWR_ISO                          (0x1 << 1)
#define PWR_ON                           (0x1 << 2)
#define PWR_ON_2ND                       (0x1 << 3)
#define PWR_CLK_DIS                      (0x1 << 4)
#define SRAM_CKISO                       (0x1 << 5)
#define SRAM_ISOINT_B                    (0x1 << 6)
#define SLPB_CLAMP                       (0x1 << 7)

#define C2K_PWR_STA_MASK                 (0x1 << 6)
#define MD1_PWR_STA_MASK                 (0x1 << 7)
#define AUD_PWR_STA_MASK                 (0x1 << 12)
#define MM0_PWR_STA_MASK                 (0x1 << 14)
#define ISP_PWR_STA_MASK                 (0x1 << 17)

#define MD1_SRAM_PDN                     (0x7 << 8)
#define MD1_SRAM_PDN_ACK                 (0x0 << 24)
#define MD1_SRAM_PDN_ACK_BIT0            (0x1 << 24)
#define MD1_SRAM_PDN_ACK_BIT1            (0x1 << 25)
#define MD1_SRAM_PDN_ACK_BIT2            (0x1 << 26)

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


int spm_mtcmos_ctrl_md1(int state)
{
	int err = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* TINFO="Start to turn off MD1" */
		/* TINFO="Set bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_1_SET, MD1_PROT_BIT_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRA_TOPAXI_PROTECTSTA1_1) & MD1_PROT_BIT_ACK_MASK) != MD1_PROT_BIT_ACK_MASK) {
		}
#endif
		/* TINFO="Set SRAM_PDN = 1" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) | MD1_SRAM_PDN);
#ifndef IGNORE_MTCMOS_CHECK
#endif
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="EXT_BUCK_ISO[0]=1"*/
		spm_write(EXT_BUCK_ISO, spm_read(EXT_BUCK_ISO) | (0x1 << 0));
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) & ~PWR_ON_2ND);
		/* TINFO="Finish to turn off MD1" */
	} else {    /* STA_POWER_ON */
		/* TINFO="Start to turn on MD1" */
		/* TINFO="EXT_BUCK_ISO[0]=0"*/
		spm_write(EXT_BUCK_ISO, spm_read(EXT_BUCK_ISO) & ~(0x1 << 0));
		/* TINFO="Set PWR_ON = 1" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) | PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) | PWR_ON_2ND);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1 and PWR_STATUS_2ND = 1" */
		while (((spm_read(PWR_STATUS) & MD1_PWR_STA_MASK) != MD1_PWR_STA_MASK)
		       || ((spm_read(PWR_STATUS_2ND) & MD1_PWR_STA_MASK) != MD1_PWR_STA_MASK)) {
				/*  */
		}
#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_1_SET, MD1_PROT_BIT_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRA_TOPAXI_PROTECTSTA1_1) & MD1_PROT_BIT_ACK_MASK) != MD1_PROT_BIT_ACK_MASK) {
		}
#endif
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set SRAM_PDN = 0" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) & ~(0x1 << 8));
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) & ~(0x1 << 9));
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) & ~(0x1 << 10));
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(MD1_PWR_CON, spm_read(MD1_PWR_CON) | PWR_RST_B);
		/* TINFO="Release bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_1_CLR, MD1_PROT_BIT_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		/* Note that this protect ack check after releasing protect has been ignored */
#endif
		/* TINFO="Finish to turn on MD1" */
	}
	return err;
}


int spm_mtcmos_ctrl_c2k(int state)
{
	int err = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* TINFO="Start to turn off C2K" */
		/* TINFO="Set bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_1_SET, C2K_PROT_BIT_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRA_TOPAXI_PROTECTSTA1_1) & C2K_PROT_BIT_ACK_MASK) != C2K_PROT_BIT_ACK_MASK) {
		}
#endif
#ifndef IGNORE_MTCMOS_CHECK
#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) & ~PWR_ON_2ND);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(PWR_STATUS) & C2K_PWR_STA_MASK)
		       || (spm_read(PWR_STATUS_2ND) & C2K_PWR_STA_MASK)) {
				/*  */
		}
#endif
		/* TINFO="Finish to turn off C2K" */
	} else {    /* STA_POWER_ON */
		/* TINFO="Start to turn on C2K" */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) | PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) | PWR_ON_2ND);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1 and PWR_STATUS_2ND = 1" */
		while (((spm_read(PWR_STATUS) & C2K_PWR_STA_MASK) != C2K_PWR_STA_MASK)
		       || ((spm_read(PWR_STATUS_2ND) & C2K_PWR_STA_MASK) != C2K_PWR_STA_MASK)) {
				/*  */
		}
#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(C2K_PWR_CON, spm_read(C2K_PWR_CON) | PWR_RST_B);
#ifndef IGNORE_MTCMOS_CHECK
#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_1_CLR, C2K_PROT_BIT_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		/* Note that this protect ack check after releasing protect has been ignored */
#endif
		/* TINFO="Finish to turn on C2K" */
	}
	return err;
}




#define AUD_SRAM_PDN                     (0xF << 8)
#define AUD_SRAM_PDN_ACK                 (0xF << 24)
#define AUD_SRAM_PDN_ACK_BIT0            (0x1 << 24)
#define AUD_SRAM_PDN_ACK_BIT1            (0x1 << 25)
#define AUD_SRAM_PDN_ACK_BIT2            (0x1 << 26)
#define AUD_SRAM_PDN_ACK_BIT3            (0x1 << 27)


int spm_mtcmos_ctrl_aud(int state)
{
	int err = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* TINFO="Start to turn off AUD" */
		/* TINFO="Set SRAM_PDN = 1" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) | AUD_SRAM_PDN);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until AUD_SRAM_PDN_ACK = 1" */
		while ((spm_read(AUD_PWR_CON) & AUD_SRAM_PDN_ACK) != AUD_SRAM_PDN_ACK) {
				/*  */
		}
#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) & ~PWR_ON_2ND);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(PWR_STATUS) & AUD_PWR_STA_MASK)
		       || (spm_read(PWR_STATUS_2ND) & AUD_PWR_STA_MASK)) {
				/*  */
		}
#endif
		/* TINFO="Finish to turn off AUD" */
	} else {    /* STA_POWER_ON */
		/* TINFO="Start to turn on AUD" */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) | PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) | PWR_ON_2ND);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1 and PWR_STATUS_2ND = 1" */
		while (((spm_read(PWR_STATUS) & AUD_PWR_STA_MASK) != AUD_PWR_STA_MASK)
		       || ((spm_read(PWR_STATUS_2ND) & AUD_PWR_STA_MASK) != AUD_PWR_STA_MASK)) {
				/*  */
		}
#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) | PWR_RST_B);
		/* TINFO="Set SRAM_PDN = 0" */
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) & ~(0x1 << 8));
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until AUD_SRAM_PDN_ACK_BIT0 = 0" */
		while (spm_read(AUD_PWR_CON) & AUD_SRAM_PDN_ACK_BIT0) {
				/*  */
		}
#endif
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) & ~(0x1 << 9));
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until AUD_SRAM_PDN_ACK_BIT1 = 0" */
		while (spm_read(AUD_PWR_CON) & AUD_SRAM_PDN_ACK_BIT1) {
				/*  */
		}
#endif
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) & ~(0x1 << 10));
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until AUD_SRAM_PDN_ACK_BIT2 = 0" */
		while (spm_read(AUD_PWR_CON) & AUD_SRAM_PDN_ACK_BIT2) {
				/*  */
		}
#endif
		spm_write(AUD_PWR_CON, spm_read(AUD_PWR_CON) & ~(0x1 << 11));
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until AUD_SRAM_PDN_ACK_BIT3 = 0" */
		while (spm_read(AUD_PWR_CON) & AUD_SRAM_PDN_ACK_BIT3) {
				/*  */
		}
#endif
		/* TINFO="Finish to turn on AUD" */
	}
	return err;
}


#define ISP_SRAM_PDN                     (0x3 << 8)
#define ISP_SRAM_PDN_ACK                 (0x3 << 24)
#define ISP_SRAM_PDN_ACK_BIT0            (0x1 << 24)
#define ISP_SRAM_PDN_ACK_BIT1            (0x1 << 25)

#define ISP_PROT_BIT_MASK                ((0x1 << 20) \
					  |(0x1 << 6))
#define ISP_PROT_BIT_ACK_MASK            ((0x1 << 20) \
					  |(0x1 << 6))
#define ISP_PROT_BIT_2ND_MASK            ((0x1 << 21))
#define ISP_PROT_BIT_ACK_2ND_MASK        ((0x1 << 21))
#define ISP_PROT_BIT_3RD_MASK            ((0x1 << 3))
#define ISP_PROT_BIT_ACK_3RD_MASK        ((0x1 << 3))

int spm_mtcmos_ctrl_isp(int state)
{
	int err = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* TINFO="Start to turn off ISP" */
		/* TINFO="Set bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_2_SET, ISP_PROT_BIT_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRA_TOPAXI_PROTECTSTA1_2) & ISP_PROT_BIT_ACK_MASK) != ISP_PROT_BIT_ACK_MASK) {
		}
#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_2_SET, ISP_PROT_BIT_2ND_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRA_TOPAXI_PROTECTSTA1_2) & ISP_PROT_BIT_ACK_2ND_MASK) != ISP_PROT_BIT_ACK_2ND_MASK) {
		}
#endif
		/* TINFO="Set bus protect" */
		spm_write(SMI_CLAMP_SET, ISP_PROT_BIT_3RD_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(SMI_CLAMP) & ISP_PROT_BIT_ACK_3RD_MASK) != ISP_PROT_BIT_ACK_3RD_MASK) {
		}
#endif
		/* TINFO="Set SRAM_PDN = 1" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) | ISP_SRAM_PDN);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ISP_SRAM_PDN_ACK = 1" */
		while ((spm_read(ISP_PWR_CON) & ISP_SRAM_PDN_ACK) != ISP_SRAM_PDN_ACK) {
				/*  */
		}
#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) & ~PWR_ON_2ND);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(PWR_STATUS) & ISP_PWR_STA_MASK)
		       || (spm_read(PWR_STATUS_2ND) & ISP_PWR_STA_MASK)) {
				/*  */
		}
#endif
		/* TINFO="Finish to turn off ISP" */
	} else {    /* STA_POWER_ON */
		/* TINFO="Start to turn on ISP" */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) | PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) | PWR_ON_2ND);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1 and PWR_STATUS_2ND = 1" */
		while (((spm_read(PWR_STATUS) & ISP_PWR_STA_MASK) != ISP_PWR_STA_MASK)
		       || ((spm_read(PWR_STATUS_2ND) & ISP_PWR_STA_MASK) != ISP_PWR_STA_MASK)) {
				/*  */
		}
#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) | PWR_RST_B);
		/* TINFO="Set SRAM_PDN = 0" */
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) & ~(0x1 << 8));
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ISP_SRAM_PDN_ACK_BIT0 = 0" */
		while (spm_read(ISP_PWR_CON) & ISP_SRAM_PDN_ACK_BIT0) {
				/*  */
		}
#endif
		spm_write(ISP_PWR_CON, spm_read(ISP_PWR_CON) & ~(0x1 << 9));
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ISP_SRAM_PDN_ACK_BIT1 = 0" */
		while (spm_read(ISP_PWR_CON) & ISP_SRAM_PDN_ACK_BIT1) {
				/*  */
		}
#endif
		/* TINFO="Release bus protect" */
		spm_write(SMI_CLAMP_CLR, ISP_PROT_BIT_3RD_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		/* Note that this protect ack check after releasing protect has been ignored */
#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_2_CLR, ISP_PROT_BIT_2ND_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		/* Note that this protect ack check after releasing protect has been ignored */
#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_2_CLR, ISP_PROT_BIT_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		/* Note that this protect ack check after releasing protect has been ignored */
#endif
		/* TINFO="Finish to turn on ISP" */
	}
	return err;
}




#define MM0_SRAM_PDN                     (0x1 << 8)
#define MM0_SRAM_PDN_ACK                 (0x1 << 24)
#define MM0_SRAM_PDN_ACK_BIT0            (0x1 << 24)


#define MM0_PROT_BIT_MASK                ((0x1 << 0) \
					  |(0x1 << 1) \
					  |(0x1 << 4) \
					  |(0x1 << 6) \
					  |(0x1 << 8) \
					  |(0x1 << 9) \
					  |(0x1 << 12) \
					  |(0x1 << 14) \
					  |(0x1 << 15))
#define MM0_PROT_BIT_ACK_MASK            ((0x1 << 0) \
					  |(0x1 << 1) \
					  |(0x1 << 4) \
					  |(0x1 << 6) \
					  |(0x1 << 8) \
					  |(0x1 << 9) \
					  |(0x1 << 12) \
					  |(0x1 << 14) \
					  |(0x1 << 15))
#define MM0_PROT_BIT_2ND_MASK            ((0x1 << 8) \
					  |(0x1 << 9))
#define MM0_PROT_BIT_ACK_2ND_MASK        ((0x1 << 8) \
					  |(0x1 << 9))

int spm_mtcmos_ctrl_mm0(int state)
{
	int err = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* TINFO="Start to turn off MM0" */
                /* TINFO="Set way_en = 0" */
		*((UINT32P)(0x10000200)) = *((UINT32P)(0x10000200)) & (~(0x1 << 6));
//#ifndef IGNORE_MTCMOS_CHECK
//		/* TINFO="Polling way_en ctrl_update=1" */
//		while ((*((UINT32P)(0x10260000)) & (0x1 << 24)) != (0x1 << 24)){
//		}
//		/* TINFO="Polling protect_idle=1" */
//		while ((*((UINT32P)(0x10000224)) & (0x1 << 18)) != (0x1 << 18)){
//		}
//#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_2_SET, MM0_PROT_BIT_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRA_TOPAXI_PROTECTSTA1_2) & MM0_PROT_BIT_ACK_MASK) != MM0_PROT_BIT_ACK_MASK) {
		}
#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_SET, MM0_PROT_BIT_2ND_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRA_TOPAXI_PROTECTSTA1) & MM0_PROT_BIT_ACK_2ND_MASK) != MM0_PROT_BIT_ACK_2ND_MASK) {
		}
#endif
		/* TINFO="Set SRAM_PDN = 1" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) | MM0_SRAM_PDN);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MM0_SRAM_PDN_ACK = 1" */
		while ((spm_read(MM0_PWR_CON) & MM0_SRAM_PDN_ACK) != MM0_SRAM_PDN_ACK) {
				/*  */
		}
#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) & ~PWR_ON_2ND);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(PWR_STATUS) & MM0_PWR_STA_MASK)
		       || (spm_read(PWR_STATUS_2ND) & MM0_PWR_STA_MASK)) {
				/*  */
		}
#endif
		/* TINFO="Finish to turn off MM0" */
	} else {    /* STA_POWER_ON */
		/* TINFO="Start to turn on MM0" */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) | PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) | PWR_ON_2ND);
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1 and PWR_STATUS_2ND = 1" */
		while (((spm_read(PWR_STATUS) & MM0_PWR_STA_MASK) != MM0_PWR_STA_MASK)
		       || ((spm_read(PWR_STATUS_2ND) & MM0_PWR_STA_MASK) != MM0_PWR_STA_MASK)) {
				/*  */
		}
#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) | PWR_RST_B);
		/* TINFO="Set SRAM_PDN = 0" */
		spm_write(MM0_PWR_CON, spm_read(MM0_PWR_CON) & ~(0x1 << 8));
#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MM0_SRAM_PDN_ACK_BIT0 = 0" */
		while (spm_read(MM0_PWR_CON) & MM0_SRAM_PDN_ACK_BIT0) {
				/*  */
		}
#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_CLR, MM0_PROT_BIT_2ND_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		/* Note that this protect ack check after releasing protect has been ignored */
#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRA_TOPAXI_PROTECTEN_2_CLR, MM0_PROT_BIT_MASK);
#ifndef IGNORE_MTCMOS_CHECK
		/* Note that this protect ack check after releasing protect has been ignored */
#endif
		/* TINFO=Set way_en = 1" */
		*((UINT32P)(0x10000200)) = *((UINT32P)(0x10000200)) | (0x1 << 6);
//#ifndef IGNORE_MTCMOS_CHECK
//		/* TINFO="Polling way_en ctrl_update=1" */
//		while ((*((UINT32P)(0x10260000)) & (0x1 << 24)) != (0x1 << 24)){
//		}
//#endif
		/* TINFO="Finish to turn on MM0" */
	}
	return err;
}

