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
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/

#include <debug.h>
#include <reg.h>
#include <platform/bitops.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_typedefs.h>
#include <platform/timer.h>
#include <platform/mt_ssusb_sifslv_ippc.h>
#include <platform/mt_usb.h>
#include <platform/mt_usbphy.h>

#include <platform/mt_usbphy_a60810.h>
#include <platform/mt_ssusb_usb3_mac_csr.h>

#ifdef MACH_FPGA
#define CFG_FPGA_PLATFORM   (1)
#else
#define DBG_PHY_CALIBRATION 1
#endif

#ifdef DBG_USB_PHY
#define PHY_LOG(x...) dprintf(INFO, "[USB][PHY] " x)
#else
#define PHY_LOG(x...) do{} while(0)
#endif

#define PHY_DRV_SHIFT   3
#define PHY_PHASE_SHIFT 3
#define PHY_PHASE_DRV_SHIFT 1

#if CFG_FPGA_PLATFORM
u32 u2_slew_rate_calibration_a60810(struct u3phy_info *info)
{
	u32 i=0;
	u32 fgRet = 0;
	u32 u4FmOut = 0;
	u32 u4Tmp = 0;

	// => RG_USB20_HSTX_SRCAL_EN = 1
	// enable HS TX SR calibration
	u3_phy_write_field32(((u32)&info->u2phy_regs_a->usbphyacr5)
	                     , A60810_RG_USB20_HSTX_SRCAL_EN_OFST, A60810_RG_USB20_HSTX_SRCAL_EN, 1);
	mdelay(1);

	// => RG_FRCK_EN = 1
	// Enable free run clock
	u3_phy_write_field32(((u32)&info->sifslv_fm_regs_a->fmmonr1)
	                     , A60810_RG_FRCK_EN_OFST, A60810_RG_FRCK_EN, 0x1);

	// => RG_CYCLECNT = 0x400
	// Setting cyclecnt = 0x400
	u3_phy_write_field32(((u32)&info->sifslv_fm_regs_a->fmcr0)
	                     , A60810_RG_CYCLECNT_OFST, A60810_RG_CYCLECNT, 0x400);

	// => RG_FREQDET_EN = 1
	// Enable frequency meter
	u3_phy_write_field32(((u32)&info->sifslv_fm_regs_a->fmcr0)
	                     , A60810_RG_FREQDET_EN_OFST, A60810_RG_FREQDET_EN, 0x1);

	// wait for FM detection done, set 10ms timeout
	for (i=0; i<10; i++) {
		// => u4FmOut = USB_FM_OUT
		// read FM_OUT
		u4FmOut = u3_phy_read_reg32(((u32)&info->sifslv_fm_regs_a->fmmonr0));
		PHY_LOG("FM_OUT value: u4FmOut = %d(0x%08X)\n", u4FmOut, u4FmOut);

		// check if FM detection done
		if (u4FmOut != 0) {
			fgRet = 0;
			PHY_LOG("FM detection done! loop = %d\n", i);

			break;
		}

		fgRet = 1;
		mdelay(1);
	}
	// => RG_FREQDET_EN = 0
	// disable frequency meter
	u3_phy_write_field32(((u32)&info->sifslv_fm_regs_a->fmcr0)
	                     , A60810_RG_FREQDET_EN_OFST, A60810_RG_FREQDET_EN, 0);

	// => RG_FRCK_EN = 0
	// disable free run clock
	u3_phy_write_field32(((u32)&info->sifslv_fm_regs_a->fmmonr1)
	                     , A60810_RG_FRCK_EN_OFST, A60810_RG_FRCK_EN, 0);

	// => RG_USB20_HSTX_SRCAL_EN = 0
	// disable HS TX SR calibration
	u3_phy_write_field32(((u32)&info->u2phy_regs_a->usbphyacr5)
	                     , A60810_RG_USB20_HSTX_SRCAL_EN_OFST, A60810_RG_USB20_HSTX_SRCAL_EN, 0);
	mdelay(1);

	if (u4FmOut == 0) {
		u3_phy_write_field32(((u32)&info->u2phy_regs_a->usbphyacr5)
		                     , A60810_RG_USB20_HSTX_SRCTRL_OFST, A60810_RG_USB20_HSTX_SRCTRL, 0x4);

		fgRet = 1;
	} else {
		// set reg = (1024/FM_OUT) * REF_CK * U2_SR_COEF_A60810 / 1000 (round to the nearest digits)
		u4Tmp = (((1024 * 26 * U2_SR_COEF_A60810) / u4FmOut) + 500) / 1000;
		PHY_LOG("SR calibration value u1SrCalVal = %d\n", (u8)u4Tmp);
		u3_phy_write_field32(((u32)&info->u2phy_regs_a->usbphyacr5)
		                     , A60810_RG_USB20_HSTX_SRCTRL_OFST, A60810_RG_USB20_HSTX_SRCTRL, u4Tmp);
	}

	PHY_LOG("[USBPHY] %s ends\n", __func__);
	return fgRet;
}

int phy_change_pipe_phase_a60810(struct u3phy_info *info, int phy_drv, int pipe_phase)
{

	int drv_reg_value;
	int phase_reg_value;
	int temp;

	drv_reg_value = phy_drv << PHY_DRV_SHIFT;
	phase_reg_value = (pipe_phase << PHY_PHASE_SHIFT) | (phy_drv << PHY_PHASE_DRV_SHIFT);
	temp = u3_phy_read_reg8(((u32)&info->sifslv_chip_regs_a->gpio_ctla)+2);
	temp &= ~(0x3 << PHY_DRV_SHIFT);
	temp |= drv_reg_value;
	u3_phy_write_reg8(((u32)&info->sifslv_chip_regs_a->gpio_ctla)+2, temp);

	temp = u3_phy_read_reg8(((u32)&info->sifslv_chip_regs_a->gpio_ctla)+3);
	temp &= ~((0x3 << PHY_PHASE_DRV_SHIFT) | (0x1f << PHY_PHASE_SHIFT));
	temp |= phase_reg_value;
	u3_phy_write_reg8(((u32)&info->sifslv_chip_regs_a->gpio_ctla)+3, temp);

	return TRUE;
}

void phy_init_a60810(struct u3phy_info *info)
{

	PHY_LOG("phy_init_a60810==========\n");

	//BANK 0x00
	//for U2 hS eye diagram
	u3_phy_write_field32(((u32)&info->u2phy_regs_a->usbphyacr1)
	                     , A60810_RG_USB20_TERM_VREF_SEL_OFST, A60810_RG_USB20_TERM_VREF_SEL, 0x05);
	//for U2 hS eye diagram
	u3_phy_write_field32(((u32)&info->u2phy_regs_a->usbphyacr1)
	                     , A60810_RG_USB20_VRT_VREF_SEL_OFST, A60810_RG_USB20_VRT_VREF_SEL, 0x05);
	//for U2 sensititvity
	u3_phy_write_field32(((u32)&info->u2phy_regs_a->usbphyacr6)
	                     , A60810_RG_USB20_SQTH_OFST, A60810_RG_USB20_SQTH, 0x04);

	//BANK 0x10
	//disable ssusb_p3_entry to work around resume from P3 bug
	u3_phy_write_field32(((u32)&info->u3phyd_regs_a->phyd_lfps0)
	                     , A60810_RG_SSUSB_P3_ENTRY_OFST, A60810_RG_SSUSB_P3_ENTRY, 0x00);
	//force disable ssusb_p3_entry to work around resume from P3 bug
	u3_phy_write_field32(((u32)&info->u3phyd_regs_a->phyd_lfps0)
	                     , A60810_RG_SSUSB_P3_ENTRY_SEL_OFST, A60810_RG_SSUSB_P3_ENTRY_SEL, 0x01);


	//BANK 0x40
	// fine tune SSC delta1 to let SSC min average ~0ppm
	u3_phy_write_field32(((u32)&info->u3phya_da_regs_a->reg19)
	                     , A60810_RG_SSUSB_PLL_SSC_DELTA1_U3_OFST, A60810_RG_SSUSB_PLL_SSC_DELTA1_U3, 0x46);
	//u3_phy_write_field32(((u32)&info->u3phya_da_regs_a->reg19)
	u3_phy_write_field32(((u32)&info->u3phya_da_regs_a->reg21)
	                     , A60810_RG_SSUSB_PLL_SSC_DELTA1_PE1H_OFST, A60810_RG_SSUSB_PLL_SSC_DELTA1_PE1H, 0x40);


	// fine tune SSC delta to let SSC min average ~0ppm

	// Fine tune SYSPLL to improve phase noise
	// I2C  60    0x08[01:00]   0x03   RW  RG_SSUSB_PLL_BC_U3
	u3_phy_write_field32(((u32)&info->u3phya_da_regs_a->reg4)
	                     , A60810_RG_SSUSB_PLL_BC_U3_OFST, A60810_RG_SSUSB_PLL_BC_U3, 0x3);
	// I2C  60    0x08[12:10]   0x03   RW  RG_SSUSB_PLL_DIVEN_U3
	u3_phy_write_field32(((u32)&info->u3phya_da_regs_a->reg4)
	                     , A60810_RG_SSUSB_PLL_DIVEN_U3_OFST, A60810_RG_SSUSB_PLL_DIVEN_U3, 0x3);
	// I2C  60    0x0C[03:00]   0x01   RW  RG_SSUSB_PLL_IC_U3
	u3_phy_write_field32(((u32)&info->u3phya_da_regs_a->reg5)
	                     , A60810_RG_SSUSB_PLL_IC_U3_OFST, A60810_RG_SSUSB_PLL_IC_U3, 0x1);
	// I2C  60    0x0C[23:22]   0x01   RW  RG_SSUSB_PLL_BR_U3
	u3_phy_write_field32(((u32)&info->u3phya_da_regs_a->reg5)
	                     , A60810_RG_SSUSB_PLL_BR_U3_OFST, A60810_RG_SSUSB_PLL_BR_U3, 0x1);
	// I2C  60    0x10[03:00]   0x01   RW  RG_SSUSB_PLL_IR_U3
	u3_phy_write_field32(((u32)&info->u3phya_da_regs_a->reg6)
	                     , A60810_RG_SSUSB_PLL_IR_U3_OFST, A60810_RG_SSUSB_PLL_IR_U3, 0x1);
	// I2C  60    0x14[03:00]   0x0F   RW  RG_SSUSB_PLL_BP_U3
	u3_phy_write_field32(((u32)&info->u3phya_da_regs_a->reg7)
	                     ////    , A60810_RG_SSUSB_PLL_BP_U3, A60810_RG_SSUSB_PLL_BP_U3, 0xF);
	                     , A60810_RG_SSUSB_PLL_BP_U3_OFST, A60810_RG_SSUSB_PLL_BP_U3, 0x0f);

	//BANK 0x60
	//force xtal pwd mode enable
	u3_phy_write_field32(((u32)&info->spllc_regs_a->u3d_xtalctl_2)
	                     , A60810_RG_SSUSB_FORCE_XTAL_PWD_OFST, A60810_RG_SSUSB_FORCE_XTAL_PWD, 0x1);
	//force bias pwd mode enable
	u3_phy_write_field32(((u32)&info->spllc_regs_a->u3d_xtalctl_2)
	                     , A60810_RG_SSUSB_FORCE_BIAS_PWD_OFST, A60810_RG_SSUSB_FORCE_BIAS_PWD, 0x1);
	//force xtal pwd mode off to work around xtal drv de
	u3_phy_write_field32(((u32)&info->spllc_regs_a->u3d_xtalctl_2)
	                     , A60810_RG_SSUSB_XTAL_PWD_OFST, A60810_RG_SSUSB_XTAL_PWD, 0x0);
	//force bias pwd mode off to work around xtal drv de
	u3_phy_write_field32(((u32)&info->spllc_regs_a->u3d_xtalctl_2)
	                     , A60810_RG_SSUSB_BIAS_PWD_OFST, A60810_RG_SSUSB_BIAS_PWD, 0x0);

	//******** test chip settings ***********
	//BANK 0x00
	// slew rate setting
	u3_phy_write_field32(((u32)&info->u2phy_regs_a->usbphyacr5)
	                     , A60810_RG_USB20_HSTX_SRCTRL_OFST, A60810_RG_USB20_HSTX_SRCTRL, 0x4);

	//BANK 0x50

	// PIPE setting  BANK5
	// PIPE drv = 2
	u3_phy_write_reg8(((u32)&info->sifslv_chip_regs_a->gpio_ctla)+2, 0x10);
	// PIPE phase
	//u3_phy_write_reg8(((u32)&info->sifslv_chip_regs_a->gpio_ctla)+3, 0xdc);
	u3_phy_write_reg8(((u32)&info->sifslv_chip_regs_a->gpio_ctla)+3, 0x24);
}
#endif
