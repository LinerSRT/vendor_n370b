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

#ifndef _DDP_REG_MIPI_H_
#define _DDP_REG_MIPI_H_
/* come from CODA mipi_tx_config H file */

#define MIPITX_LANE_CON						(0x000CUL)
#define MIPITX_VOLTAGE_SEL					(0x0010UL)
#define MIPITX_DSI_PRESERVED				(0x0014UL)
#define MIPITX_TOP_CON						(0x0018UL)
#define MIPITX_PLL_PWR						(0x0028UL)
#define MIPITX_PLL_CON0						(0x002CUL)
#define MIPITX_PLL_CON1						(0x0030UL)
#define MIPITX_PLL_CON2						(0x0034UL)
#define MIPITX_PLL_CON3						(0x0038UL)
#define MIPITX_PLL_CON4						(0x003CUL)
#define MIPITX_PHY_SEL0						(0x0040UL)
#define MIPITX_PHY_SEL1						(0x0044UL)
#define MIPITX_GPIO_CON						(0x0068UL)
#define MIPITX_DBG_CON						(0x0070UL)
#define MIPITX_APB_ASYNC_STA				(0x0078UL)
#define MIPITX_D2_LDOOUT_EN					(0x0100UL)
#define MIPITX_D2_LOOPBACK_EN				(0x0104UL)
#define MIPITX_D2_LPTX_IPLUS1				(0x0108UL)
#define MIPITX_D2_LPTX_IPLUS2				(0x010CUL)
#define MIPITX_D2_LPTX_IMINUS				(0x0110UL)
#define MIPITX_D2_LPCD_IPLUS				(0x0114UL)
#define MIPITX_D2_LPCD_IMINUS				(0x0118UL)
#define MIPITX_D2_RTCODE0					(0x011CUL)
#define MIPITX_D2_RTCODE1					(0x0120UL)
#define MIPITX_D2_RTCODE2					(0x0124UL)
#define MIPITX_D2_RTCODE3					(0x0128UL)
#define MIPITX_D2_CKLANE_EN					(0x0130UL)
#define MIPITX_D2_DIG_PN_SWAP_EN			(0x0140UL)
#define MIPITX_D2_SW_CTL_EN					(0x0144UL)
#define MIPITX_D2_SW_LPTX_PRE_OE			(0x0148UL)
#define MIPITX_D2_SW_LPTX_OE				(0x014CUL)
#define MIPITX_D2_SW_LPTX_DP				(0x0150UL)
#define MIPITX_D2_SW_LPTX_DN				(0x0154UL)
#define MIPITX_D2_SW_LPRX_EN				(0x0158UL)
#define MIPITX_D2_SW_HSTX_PRE_OE			(0x015CUL)
#define MIPITX_D2_SW_HSTX_OE				(0x0160UL)
#define MIPITX_D2_SW_HSTX_RDY				(0x0164UL)
#define MIPITX_D2_SW_HSTX_DATA0				(0x0180UL)
#define MIPITX_D2_SW_HSTX_DATA1				(0x0184UL)
#define MIPITX_D2_SW_HSTX_DATA2				(0x0188UL)
#define MIPITX_D2_SW_HSTX_DATA3				(0x018CUL)
#define MIPITX_D2_SW_HSTX_DATA4				(0x0190UL)
#define MIPITX_D2_SW_HSTX_DATA5				(0x0194UL)
#define MIPITX_D2_SW_HSTX_DATA6				(0x0198UL)
#define MIPITX_D2_SW_HSTX_DATA7				(0x019CUL)
#define MIPITX_D2_AD_RX						(0x01C0UL)
#define MIPITX_D2_AD_CD						(0x01C4UL)
#define MIPITX_D0_LDOOUT_EN					(0x0200UL)
#define MIPITX_D0_LOOPBACK_EN				(0x0204UL)
#define MIPITX_D0_LPTX_IPLUS1				(0x0208UL)
#define MIPITX_D0_LPTX_IPLUS2				(0x020CUL)
#define MIPITX_D0_LPTX_IMINUS				(0x0210UL)
#define MIPITX_D0_LPCD_IPLUS				(0x0214UL)
#define MIPITX_D0_LPCD_IMINUS				(0x0218UL)
#define MIPITX_D0_RTCODE0					(0x021CUL)
#define MIPITX_D0_RTCODE1					(0x0220UL)
#define MIPITX_D0_RTCODE2					(0x0224UL)
#define MIPITX_D0_RTCODE3					(0x0228UL)
#define MIPITX_D0_CKLANE_EN					(0x0230UL)
#define MIPITX_D0_DIG_PN_SWAP_EN			(0x0240UL)
#define MIPITX_D0_SW_CTL_EN					(0x0244UL)
#define MIPITX_D0_SW_LPTX_PRE_OE			(0x0248UL)
#define MIPITX_D0_SW_LPTX_OE				(0x024CUL)
#define MIPITX_D0_SW_LPTX_DP				(0x0250UL)
#define MIPITX_D0_SW_LPTX_DN				(0x0254UL)
#define MIPITX_D0_SW_LPRX_EN				(0x0258UL)
#define MIPITX_D0_SW_HSTX_PRE_OE			(0x025CUL)
#define MIPITX_D0_SW_HSTX_OE				(0x0260UL)
#define MIPITX_D0_SW_HSTX_RDY				(0x0264UL)
#define MIPITX_D0_SW_HSTX_DATA0				(0x0280UL)
#define MIPITX_D0_SW_HSTX_DATA1				(0x0284UL)
#define MIPITX_D0_SW_HSTX_DATA2				(0x0288UL)
#define MIPITX_D0_SW_HSTX_DATA3				(0x028CUL)
#define MIPITX_D0_SW_HSTX_DATA4				(0x0290UL)
#define MIPITX_D0_SW_HSTX_DATA5				(0x0294UL)
#define MIPITX_D0_SW_HSTX_DATA6				(0x0298UL)
#define MIPITX_D0_SW_HSTX_DATA7				(0x029CUL)
#define MIPITX_D0_AD_RX						(0x02C0UL)
#define MIPITX_D0_AD_CD						(0x02C4UL)
#define MIPITX_CK_LDOOUT_EN					(0x0300UL)
#define MIPITX_CK_LOOPBACK_EN				(0x0304UL)
#define MIPITX_CK_LPTX_IPLUS1				(0x0308UL)
#define MIPITX_CK_LPTX_IPLUS2				(0x030CUL)
#define MIPITX_CK_LPTX_IMINUS				(0x0310UL)
#define MIPITX_CK_LPCD_IPLUS				(0x0314UL)
#define MIPITX_CK_LPCD_IMINUS				(0x0318UL)
#define MIPITX_CK_RTCODE0					(0x031CUL)
#define MIPITX_CK_RTCODE1					(0x0320UL)
#define MIPITX_CK_RTCODE2					(0x0324UL)
#define MIPITX_CK_RTCODE3					(0x0328UL)
#define MIPITX_CK_CKLANE_EN					(0x0330UL)
#define MIPITX_CK_DIG_PN_SWAP_EN			(0x0340UL)
#define MIPITX_CK_SW_CTL_EN					(0x0344UL)
#define MIPITX_CK_SW_LPTX_PRE_OE			(0x0348UL)
#define MIPITX_CK_SW_LPTX_OE				(0x034CUL)
#define MIPITX_CK_SW_LPTX_DP				(0x0350UL)
#define MIPITX_CK_SW_LPTX_DN				(0x0354UL)
#define MIPITX_CK_SW_LPRX_EN				(0x0358UL)
#define MIPITX_CK_SW_HSTX_PRE_OE			(0x035CUL)
#define MIPITX_CK_SW_HSTX_OE				(0x0360UL)
#define MIPITX_CK_SW_HSTX_RDY				(0x0364UL)
#define MIPITX_CK_SW_HSTX_DATA0				(0x0380UL)
#define MIPITX_CK_SW_HSTX_DATA1				(0x0384UL)
#define MIPITX_CK_SW_HSTX_DATA2				(0x0388UL)
#define MIPITX_CK_SW_HSTX_DATA3				(0x038CUL)
#define MIPITX_CK_SW_HSTX_DATA4				(0x0390UL)
#define MIPITX_CK_SW_HSTX_DATA5				(0x0394UL)
#define MIPITX_CK_SW_HSTX_DATA6				(0x0398UL)
#define MIPITX_CK_SW_HSTX_DATA7				(0x039CUL)
#define MIPITX_CK_AD_RX						(0x03C0UL)
#define MIPITX_CK_AD_CD						(0x03C4UL)
#define MIPITX_D1_LDOOUT_EN					(0x0400UL)
#define MIPITX_D1_LOOPBACK_EN				(0x0404UL)
#define MIPITX_D1_LPTX_IPLUS1				(0x0408UL)
#define MIPITX_D1_LPTX_IPLUS2				(0x040CUL)
#define MIPITX_D1_LPTX_IMINUS				(0x0410UL)
#define MIPITX_D1_LPCD_IPLUS				(0x0414UL)
#define MIPITX_D1_LPCD_IMINUS				(0x0418UL)
#define MIPITX_D1_RTCODE0					(0x041CUL)
#define MIPITX_D1_RTCODE1					(0x0420UL)
#define MIPITX_D1_RTCODE2					(0x0424UL)
#define MIPITX_D1_RTCODE3					(0x0428UL)
#define MIPITX_D1_CKLANE_EN					(0x0430UL)
#define MIPITX_D1_DIG_PN_SWAP_EN			(0x0440UL)
#define MIPITX_D1_SW_CTL_EN					(0x0444UL)
#define MIPITX_D1_SW_LPTX_PRE_OE			(0x0448UL)
#define MIPITX_D1_SW_LPTX_OE				(0x044CUL)
#define MIPITX_D1_SW_LPTX_DP				(0x0450UL)
#define MIPITX_D1_SW_LPTX_DN				(0x0454UL)
#define MIPITX_D1_SW_LPRX_EN				(0x0458UL)
#define MIPITX_D1_SW_HSTX_PRE_OE			(0x045CUL)
#define MIPITX_D1_SW_HSTX_OE				(0x0460UL)
#define MIPITX_D1_SW_HSTX_RDY				(0x0464UL)
#define MIPITX_D1_SW_HSTX_DATA0				(0x0480UL)
#define MIPITX_D1_SW_HSTX_DATA1				(0x0484UL)
#define MIPITX_D1_SW_HSTX_DATA2				(0x0488UL)
#define MIPITX_D1_SW_HSTX_DATA3				(0x048CUL)
#define MIPITX_D1_SW_HSTX_DATA4				(0x0490UL)
#define MIPITX_D1_SW_HSTX_DATA5				(0x0494UL)
#define MIPITX_D1_SW_HSTX_DATA6				(0x0498UL)
#define MIPITX_D1_SW_HSTX_DATA7				(0x049CUL)
#define MIPITX_D1_AD_RX						(0x04C0UL)
#define MIPITX_D1_AD_CD						(0x04C4UL)
#define MIPITX_D3_LDOOUT_EN					(0x0500UL)
#define MIPITX_D3_LOOPBACK_EN				(0x0504UL)
#define MIPITX_D3_LPTX_IPLUS1				(0x0508UL)
#define MIPITX_D3_LPTX_IPLUS2				(0x050CUL)
#define MIPITX_D3_LPTX_IMINUS				(0x0510UL)
#define MIPITX_D3_LPCD_IPLUS				(0x0514UL)
#define MIPITX_D3_LPCD_IMINUS				(0x0518UL)
#define MIPITX_D3_RTCODE0					(0x051CUL)
#define MIPITX_D3_RTCODE1					(0x0520UL)
#define MIPITX_D3_RTCODE2					(0x0524UL)
#define MIPITX_D3_RTCODE3					(0x0528UL)
#define MIPITX_D3_CKLANE_EN					(0x0530UL)
#define MIPITX_D3_DIG_PN_SWAP_EN			(0x0540UL)
#define MIPITX_D3_SW_CTL_EN					(0x0544UL)
#define MIPITX_D3_SW_LPTX_PRE_OE			(0x0548UL)
#define MIPITX_D3_SW_LPTX_OE				(0x054CUL)
#define MIPITX_D3_SW_LPTX_DP				(0x0550UL)
#define MIPITX_D3_SW_LPTX_DN				(0x0554UL)
#define MIPITX_D3_SW_LPRX_EN				(0x0558UL)
#define MIPITX_D3_SW_HSTX_PRE_OE			(0x055CUL)
#define MIPITX_D3_SW_HSTX_OE				(0x0560UL)
#define MIPITX_D3_SW_HSTX_RDY				(0x0564UL)
#define MIPITX_D3_SW_HSTX_DATA0				(0x0580UL)
#define MIPITX_D3_SW_HSTX_DATA1				(0x0584UL)
#define MIPITX_D3_SW_HSTX_DATA2				(0x0588UL)
#define MIPITX_D3_SW_HSTX_DATA3				(0x058CUL)
#define MIPITX_D3_SW_HSTX_DATA4				(0x0590UL)
#define MIPITX_D3_SW_HSTX_DATA5				(0x0594UL)
#define MIPITX_D3_SW_HSTX_DATA6				(0x0598UL)
#define MIPITX_D3_SW_HSTX_DATA7				(0x059CUL)
#define MIPITX_D3_AD_RX						(0x05C0UL)
#define MIPITX_D3_AD_CD						(0x05C4UL)
#define MIPITX_DPHY_BIST_CON0				(0x0600UL)
#define MIPITX_DPHY_BIST_CON1				(0x0604UL)
#define MIPITX_DPHY_BIST_LANE_NUM			(0x0608UL)
#define MIPITX_DPHY_BIST_TIMING_0			(0x060CUL)
#define MIPITX_DPHY_BIST_TIMING_1			(0x0610UL)
#define MIPITX_DPHY_BIST_PATTERN_0			(0x0614UL)
#define MIPITX_DPHY_BIST_PATTERN_1			(0x0618UL)
#define MIPITX_DPHY_BIST_PATTERN_2			(0x061CUL)
#define MIPITX_DPHY_BIST_PATTERN_3			(0x0620UL)
#define MIPITX_DPHY_BIST_PATTERN_4			(0x0624UL)
#define MIPITX_DPHY_BIST_PATTERN_5			(0x0628UL)
#define MIPITX_DPHY_BIST_PATTERN_6			(0x062CUL)
#define MIPITX_DPHY_BIST_PATTERN_7			(0x0630UL)


/* field definition */
#define FLD_RG_DSI_DSI_LNT_INTR_EN			REG_FLD(1, 15)
#define FLD_RG_DSI_LNT_HS_BIAS_EN			REG_FLD(1, 14)
#define FLD_RG_DSI_LNT_IMP_CAL				REG_FLD(1, 13)
#define FLD_RG_DSI_LNT_TESTMODE_EN			REG_FLD(1, 12)
#define FLD_RG_DSI_LNT_IMP_CAL_CODE			REG_FLD(4, 8)
#define FLD_RG_DSI_DSI_LNT_AIO_SEL			REG_FLD(3, 4)
#define FLD_RG_DSI_DSI_PAD_TIE_LOW_EN		REG_FLD(1, 1)
#define FLD_RG_DSI_DEBUG_INPUT_EN			REG_FLD(1, 0)

#define FLD_RG_DSI_V12_SEL			REG_FLD(3, 20)
#define FLD_RG_DSI_V10_SEL			REG_FLD(3, 16)
#define FLD_RG_DSI_V072_SEL			REG_FLD(3, 12)
#define FLD_RG_DSI_V04_SEL			REG_FLD(3, 8)
#define FLD_RG_DSI_V032_SEL			REG_FLD(3, 4)
#define FLD_RG_DSI_V02_SEL			REG_FLD(3, 0)

#define FLD_RG_DSI_PRESERVE			REG_FLD(32, 0)

#define FLD_RG_DSI_BG_CORE_EN		REG_FLD(1, 29)
#define FLD_RG_DSI_BG_CKEN			REG_FLD(1, 28)
#define FLD_RG_DSI_BG_DIV			REG_FLD(2, 25)
#define FLD_RG_DSI_BG_FAST_CHARGE	REG_FLD(1, 24)
#define FLD_RG_DSI_BG_R1_TRIM		REG_FLD(4, 20)
#define FLD_RG_DSI_BG_R2_TRIM		REG_FLD(4, 16)
#define FLD_RG_DSI_LDOCORE_EN		REG_FLD(1, 15)
#define FLD_RG_DSI_CKG_LDOOUT_EN	REG_FLD(1, 14)
#define FLD_RG_DSI_BCLK_SEL			REG_FLD(2, 12)
#define FLD_RG_DSI_LD_IDX_SEL		REG_FLD(3, 8)
#define FLD_RG_DSI_PHYCLK_SEL		REG_FLD(2, 6)
#define FLD_RG_DSI_DSICLK_FREQ_SEL	REG_FLD(1, 5)
#define FLD_RG_DSI_LPTX_CLMP_EN		REG_FLD(1, 4)
#define FLD_RG_DSI_LPRXCD_SEL		REG_FLD(3, 0)

#define FLD_AD_DSI_PLL_SDM_PWR_ACK			REG_FLD(1, 8)
#define FLD_RG_DSI_PLL_SDM_ISO_EN			REG_FLD(1, 1)
#define FLD_RG_DSI_PLL_SDM_PWR_ON			REG_FLD(1, 0)

#define FLD_RG_DSI_PLL_SDM_PCW			REG_FLD(32, 0)

#define FLD_RG_DSI_PLL_TSTSEL			REG_FLD(2, 28)
#define FLD_RG_DSI_PLL_TSTCK_EN			REG_FLD(1, 26)
#define FLD_RG_DSI_PLL_TST_EN			REG_FLD(1, 25)
#define FLD_RG_DSI_MPPLL_VOD_EN			REG_FLD(1, 24)
#define FLD_RG_DSI_MPPLL_TXDIV1			REG_FLD(2, 20)
#define FLD_RG_DSI_MPPLL_TXDIV0			REG_FLD(2, 16)
#define FLD_RG_DSI_PLL_SDM_FRA_EN		REG_FLD(1, 13)
#define FLD_RG_DSI_PLL_PREDIV			REG_FLD(2, 11)
#define FLD_RG_DSI_PLL_POSDIV			REG_FLD(3, 8)
#define FLD_RG_DSI_PLL_EN				REG_FLD(1, 4)
#define FLD_RG_DSI_PLL_SDM_PCW_CHG		REG_FLD(1, 0)

#define FLD_RG_DSI_PLL_SDM_SSC_PRD			REG_FLD(16, 16)
#define FLD_RG_DSI_PLL_SDM_SSC_EN			REG_FLD(1, 1)
#define FLD_RG_DSI_PLL_SDM_SSC_PH_INIT		REG_FLD(1, 0)

#define FLD_RG_DSI_PLL_SDM_SSC_DELTA		REG_FLD(16, 16)
#define FLD_RG_DSI_PLL_SDM_SSC_DELTA1		REG_FLD(16, 0)

#define FLD_RG_DSI_PLL_RESERVED			REG_FLD(16, 16)
#define FLD_RG_DSI_PLL_MONREF_EN		REG_FLD(1, 4)
#define FLD_RG_DSI_PLL_MONVC_EN			REG_FLD(2, 2)

#define FLD_MIPI_TX_PHY1_SEL			REG_FLD(4, 28)
#define FLD_MIPI_TX_PHYC_SEL			REG_FLD(4, 20)
#define FLD_MIPI_TX_PHY0_SEL			REG_FLD(4, 12)
#define FLD_MIPI_TX_PHY2_SEL			REG_FLD(4, 4)

#define FLD_MIPI_TX_PHY3_SEL			REG_FLD(4, 4)
#define FLD_MIPI_TX_LPRX_SEL			REG_FLD(4, 0)

#define FLD_RG_DSI_SMT_EN			REG_FLD(1, 13)
#define FLD_RG_DSI_DRIVE_EN			REG_FLD(1, 12)
#define FLD_RG_DSI_GPI9_EN			REG_FLD(1, 9)
#define FLD_RG_DSI_GPI8_EN			REG_FLD(1, 8)
#define FLD_RG_DSI_GPI7_EN			REG_FLD(1, 7)
#define FLD_RG_DSI_GPI6_EN			REG_FLD(1, 6)
#define FLD_RG_DSI_GPI5_EN			REG_FLD(1, 5)
#define FLD_RG_DSI_GPI4_EN			REG_FLD(1, 4)
#define FLD_RG_DSI_GPI3_EN			REG_FLD(1, 3)
#define FLD_RG_DSI_GPI2_EN			REG_FLD(1, 2)
#define FLD_RG_DSI_GPI1_EN			REG_FLD(1, 1)
#define FLD_RG_DSI_GPI0_EN			REG_FLD(1, 0)

#define FLD_MIPI_TX_APB_ASYNC_CNT_EN			REG_FLD(1, 6)
#define FLD_MIPI_TX_GPIO_MODE_EN				REG_FLD(1, 5)
#define FLD_MIPI_TX_DBG_OUT_EN					REG_FLD(1, 4)

#define FLD_MIPI_TX_APB_ASYNC_ERR_ADDR		REG_FLD(10, 1)
#define FLD_MIPI_TX_APB_ASYNC_ERR				REG_FLD(1, 0)

#define FLD_DSI_D2_LDOOUT_EN					REG_FLD(1, 0)

#define FLD_DSI_D2_LOOPBACK_EN					REG_FLD(1, 0)

#define FLD_DSI_D2_LPTX_IPLUS1					REG_FLD(1, 0)

#define FLD_DSI_D2_LPTX_IPLUS2					REG_FLD(1, 0)

#define FLD_DSI_D2_LPTX_IMINUS					REG_FLD(1, 0)

#define FLD_DSI_D2_LPCD_IPLUS					REG_FLD(1, 0)

#define FLD_DSI_D2_LPCD_IMINUS					REG_FLD(1, 0)

#define FLD_DSI_D2_RTCODE0						REG_FLD(1, 0)

#define FLD_DSI_D2_RTCODE1				REG_FLD(1, 0)

#define FLD_DSI_D2_RTCODE2				REG_FLD(1, 0)

#define FLD_DSI_D2_RTCODE3				REG_FLD(1, 0)

#define FLD_DSI_D2_CKLANE_EN			REG_FLD(1, 0)

#define FLD_DSI_D2_DIG_PN_SWAP_EN		REG_FLD(1, 0)

#define FLD_DSI_D2_SW_CTL_EN			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_LPTX_PRE_OE		REG_FLD(1, 8)
#define FLD_DSI_D2_SW_LPTX_PRE_OE			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_LPTX_OE			REG_FLD(1, 8)
#define FLD_DSI_D2_SW_LPTX_OE					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_LPTX_DP			REG_FLD(1, 8)
#define FLD_DSI_D2_SW_LPTX_DP					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_LPTX_DN			REG_FLD(1, 8)
#define FLD_DSI_D2_SW_LPTX_DN					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_LPRX_EN			REG_FLD(1, 8)
#define FLD_DSI_D2_SW_LPRX_EN					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_PRE_OE		REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_PRE_OE				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_OE			REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_OE					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_RDY			REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_RDY					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA0		REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA0				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA1		REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA1				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA2		REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA2				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA3		REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA3				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA4		REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA4				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA5		REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA5				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA6		REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA6				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_SW_HSTX_DATA7		REG_FLD(1, 8)
#define FLD_DSI_D2_SW_HSTX_DATA7				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_AD_LPRX_N			REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D2_AD_LPRX_P			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D2_AD_LPCD_N			REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D2_AD_LPCD_P			REG_FLD(1, 0)

#define FLD_DSI_D0_LDOOUT_EN					REG_FLD(1, 0)

#define FLD_DSI_D0_LOOPBACK_EN					REG_FLD(1, 0)

#define FLD_DSI_D0_LPTX_IPLUS1					REG_FLD(1, 0)

#define FLD_DSI_D0_LPTX_IPLUS2					REG_FLD(1, 0)

#define FLD_DSI_D0_LPTX_IMINUS					REG_FLD(1, 0)

#define FLD_DSI_D0_LPCD_IPLUS					REG_FLD(1, 0)

#define FLD_DSI_D0_LPCD_IMINUS					REG_FLD(1, 0)

#define FLD_DSI_D0_RTCODE0			REG_FLD(1, 0)

#define FLD_DSI_D0_RTCODE1			REG_FLD(1, 0)

#define FLD_DSI_D0_RTCODE2			REG_FLD(1, 0)

#define FLD_DSI_D0_RTCODE3			REG_FLD(1, 0)

#define FLD_DSI_D0_CKLANE_EN		REG_FLD(1, 0)

#define FLD_DSI_D0_DIG_PN_SWAP_EN			REG_FLD(1, 0)

#define FLD_DSI_D0_SW_CTL_EN				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_LPTX_PRE_OE	REG_FLD(1, 8)
#define FLD_DSI_D0_SW_LPTX_PRE_OE			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_LPTX_OE		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_LPTX_OE				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_LPTX_DP		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_LPTX_DP				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_LPTX_DN		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_LPTX_DN				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_LPRX_EN		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_LPRX_EN				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_PRE_OE	REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_PRE_OE			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_OE		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_OE				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_RDY			REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_RDY					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA0		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA0				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA1		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA1				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA2		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA2				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA3		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA3				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA4		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA4				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA5		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA5				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA6		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA6				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_SW_HSTX_DATA7		REG_FLD(1, 8)
#define FLD_DSI_D0_SW_HSTX_DATA7				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_AD_LPRX_N			REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D0_AD_LPRX_P			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D0_AD_LPCD_N			REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D0_AD_LPCD_P			REG_FLD(1, 0)

#define FLD_DSI_CK_LDOOUT_EN			REG_FLD(1, 0)

#define FLD_DSI_CK_LOOPBACK_EN			REG_FLD(1, 0)

#define FLD_DSI_CK_LPTX_IPLUS1			REG_FLD(1, 0)

#define FLD_DSI_CK_LPTX_IPLUS2			REG_FLD(1, 0)

#define FLD_DSI_CK_LPTX_IMINUS			REG_FLD(1, 0)

#define FLD_DSI_CK_LPCD_IPLUS			REG_FLD(1, 0)

#define FLD_DSI_CK_LPCD_IMINUS			REG_FLD(1, 0)

#define FLD_DSI_CK_RTCODE0			REG_FLD(1, 0)

#define FLD_DSI_CK_RTCODE1			REG_FLD(1, 0)

#define FLD_DSI_CK_RTCODE2			REG_FLD(1, 0)

#define FLD_DSI_CK_RTCODE3			REG_FLD(1, 0)

#define FLD_DSI_CK_CKLANE_EN			REG_FLD(1, 0)

#define FLD_DSI_CK_DIG_PN_SWAP_EN		REG_FLD(1, 0)

#define FLD_DSI_CK_SW_CTL_EN			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_LPTX_PRE_OE	REG_FLD(1, 8)
#define FLD_DSI_CK_SW_LPTX_PRE_OE			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_LPTX_OE	REG_FLD(1, 8)
#define FLD_DSI_CK_SW_LPTX_OE			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_LPTX_DP			REG_FLD(1, 8)
#define FLD_DSI_CK_SW_LPTX_DP					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_LPTX_DN			REG_FLD(1, 8)
#define FLD_DSI_CK_SW_LPTX_DN					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_LPRX_EN			REG_FLD(1, 8)
#define FLD_DSI_CK_SW_LPRX_EN					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_PRE_OE		REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_PRE_OE				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_OE		REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_OE				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_RDY			REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_RDY					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA0		REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA0				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA1	REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA1			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA2	REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA2			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA3	REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA3			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA4	REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA4			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA5	REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA5			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA6	REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA6			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_SW_HSTX_DATA7	REG_FLD(1, 8)
#define FLD_DSI_CK_SW_HSTX_DATA7			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_AD_LPRX_N			REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_CK_AD_LPRX_P			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_CK_AD_LPCD_N			REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_CK_AD_LPCD_P			REG_FLD(1, 0)

#define FLD_DSI_D1_LDOOUT_EN			REG_FLD(1, 0)

#define FLD_DSI_D1_LOOPBACK_EN			REG_FLD(1, 0)

#define FLD_DSI_D1_LPTX_IPLUS1			REG_FLD(1, 0)

#define FLD_DSI_D1_LPTX_IPLUS2			REG_FLD(1, 0)

#define FLD_DSI_D1_LPTX_IMINUS			REG_FLD(1, 0)

#define FLD_DSI_D1_LPCD_IPLUS			REG_FLD(1, 0)

#define FLD_DSI_D1_LPCD_IMINUS			REG_FLD(1, 0)

#define FLD_DSI_D1_RTCODE0			REG_FLD(1, 0)

#define FLD_DSI_D1_RTCODE1			REG_FLD(1, 0)

#define FLD_DSI_D1_RTCODE2			REG_FLD(1, 0)

#define FLD_DSI_D1_RTCODE3			REG_FLD(1, 0)

#define FLD_DSI_D1_CKLANE_EN			REG_FLD(1, 0)

#define FLD_DSI_D1_DIG_PN_SWAP_EN		REG_FLD(1, 0)

#define FLD_DSI_D1_SW_CTL_EN			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_LPTX_PRE_OE		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_LPTX_PRE_OE			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_LPTX_OE		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_LPTX_OE				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_LPTX_DP		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_LPTX_DP				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_LPTX_DN		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_LPTX_DN				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_LPRX_EN		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_LPRX_EN				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_PRE_OE		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_PRE_OE				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_OE			REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_OE					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_RDY			REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_RDY					REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA0		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA0				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA1		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA1				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA2		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA2				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA3		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA3				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA4		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA4				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA5		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA5				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA6		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA6				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_SW_HSTX_DATA7		REG_FLD(1, 8)
#define FLD_DSI_D1_SW_HSTX_DATA7				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_AD_LPRX_N			REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D1_AD_LPRX_P			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D1_AD_LPCD_N			REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D1_AD_LPCD_P			REG_FLD(1, 0)

#define FLD_DSI_D3_LDOOUT_EN			REG_FLD(1, 0)

#define FLD_DSI_D3_LOOPBACK_EN			REG_FLD(1, 0)

#define FLD_DSI_D3_LPTX_IPLUS1			REG_FLD(1, 0)

#define FLD_DSI_D3_LPTX_IPLUS2			REG_FLD(1, 0)

#define FLD_DSI_D3_LPTX_IMINUS			REG_FLD(1, 0)

#define FLD_DSI_D3_LPCD_IPLUS			REG_FLD(1, 0)

#define FLD_DSI_D3_LPCD_IMINUS			REG_FLD(1, 0)

#define FLD_DSI_D3_RTCODE0			REG_FLD(1, 0)

#define FLD_DSI_D3_RTCODE1			REG_FLD(1, 0)

#define FLD_DSI_D3_RTCODE2			REG_FLD(1, 0)

#define FLD_DSI_D3_RTCODE3			REG_FLD(1, 0)

#define FLD_DSI_D3_CKLANE_EN			REG_FLD(1, 0)

#define FLD_DSI_D3_DIG_PN_SWAP_EN		REG_FLD(1, 0)

#define FLD_DSI_D3_SW_CTL_EN			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_LPTX_PRE_OE		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_LPTX_PRE_OE			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_LPTX_OE		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_LPTX_OE				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_LPTX_DP		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_LPTX_DP				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_LPTX_DN		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_LPTX_DN				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_LPRX_EN		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_LPRX_EN			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_PRE_OE		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_PRE_OE		REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_OE		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_OE				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_RDY			REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_RDY				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA0		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA0				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA1		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA1				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA2		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA2				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA3		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA3				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA4		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA4				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA5		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA5				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA6		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA6				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_SW_HSTX_DATA7		REG_FLD(1, 8)
#define FLD_DSI_D3_SW_HSTX_DATA7				REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_AD_LPRX_N			REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D3_AD_LPRX_P			REG_FLD(1, 0)

#define FLD_DBG_OUT_DSI_D3_AD_LPCD_N			REG_FLD(1, 8)
#define FLD_DBG_OUT_DSI_D3_AD_LPCD_P			REG_FLD(1, 0)

#define FLD_DPHY_BIST_SPECIFIED_PATTERN		REG_FLD(1, 24)
#define FLD_DPHY_BIST_FIX_PATTERN			REG_FLD(1, 16)
#define FLD_DPHY_BIST_MODE					REG_FLD(1, 8)
#define FLD_DPHY_BIST_ENABLE				REG_FLD(1, 0)

#define FLD_DPHY_BIST_CK_HS_PATTERN_SEL		REG_FLD(1, 16)
#define FLD_DPHY_BIST_DESKEW_MODE			REG_FLD(1, 8)
#define FLD_DPHY_BIST_HS_FREE				REG_FLD(1, 0)

#define FLD_DPHY_BIST_LANE_NUM_3		REG_FLD(1, 24)
#define FLD_DPHY_BIST_LANE_NUM_2		REG_FLD(1, 16)
#define FLD_DPHY_BIST_LANE_NUM_1		REG_FLD(1, 8)
#define FLD_DPHY_BIST_LANE_NUM_0		REG_FLD(1, 0)

#define FLD_DPHY_BIST_TIMIING_3			REG_FLD(1, 24)
#define FLD_DPHY_BIST_TIMIING_2			REG_FLD(1, 16)
#define FLD_DPHY_BIST_TIMIING_1			REG_FLD(1, 8)
#define FLD_DPHY_BIST_TIMIING_0			REG_FLD(1, 0)

#define FLD_DPHY_BIST_TIMIING_7			REG_FLD(1, 24)
#define FLD_DPHY_BIST_TIMIING_6			REG_FLD(1, 16)
#define FLD_DPHY_BIST_TIMIING_5			REG_FLD(1, 8)
#define FLD_DPHY_BIST_TIMIING_4			REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_3		REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_2		REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_1		REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_0		REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_7		REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_6		REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_5		REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_4		REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_11		REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_10		REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_9			REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_8			REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_15		REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_14		REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_13		REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_12		REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_19		REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_18		REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_17		REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_16		REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_23		REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_22		REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_21		REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_20		REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_27		REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_26		REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_25		REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_24		REG_FLD(1, 0)

#define FLD_DPHY_BIST_PATTERN_31		REG_FLD(1, 24)
#define FLD_DPHY_BIST_PATTERN_30		REG_FLD(1, 16)
#define FLD_DPHY_BIST_PATTERN_29		REG_FLD(1, 8)
#define FLD_DPHY_BIST_PATTERN_28		REG_FLD(1, 0)

#endif /* _DDP_REG_MIPI_H_ */
