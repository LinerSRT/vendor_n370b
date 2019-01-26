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

#ifdef BUILD_LK
#include <string.h>
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h>
#include <platform/upmu_common.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#endif

#include "lcm_drv.h"

/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE								0
#define FRAME_WIDTH                                     (800)
#define FRAME_HEIGHT                                    (1280)

#define GPIO_OUT_ONE 1
#define GPIO_OUT_ZERO 0

/*GPIO45 for GPIO_LCM_RST*/
#ifdef GPIO_LCM_RST
#define GPIO_LCD_RST      GPIO_LCM_RST
#else
#define GPIO_LCD_RST      GPIO45	//0xFFFFFFFF
#endif

/*GPIO55 for GPIO_LCM1.8_EN*/
#ifdef GPIO_LCM_PWR2_EN
#define GPIO_LCD_PWR2_EN      GPIO_LCM_PWR2_EN
#else
#define GPIO_LCD_PWR2_EN		GPIO55	//0xFFFFFFFF
#endif

/*GPIO56 for GPIO_LCM_POWER_EN*/
#ifdef GPIO_LCM_PWR_EN
#define GPIO_LCD_PWR_EN      GPIO_LCM_PWR_EN
#else
#define GPIO_LCD_PWR_EN		GPIO56	//0xFFFFFFFF
#endif

/*GPIO44 for GPIO_LCM_STBY*/
#ifdef GPIO_LCM_STB
#define GPIO_LCD_STB_EN      GPIO_LCM_STB
#else
#define GPIO_LCD_STB_EN      GPIO44	//0xFFFFFFFF
#endif

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                          lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)      lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)                           lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define REGFLAG_DELAY                               0xFC
#define REGFLAG_END_OF_TABLE                        0xFD	/* END OF REGISTERS MARKER */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* --------------------------------------------------------------------------- */
/* Local Variables */
/* --------------------------------------------------------------------------- */
static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	if (GPIO == 0xFFFFFFFF) {
#ifdef BUILD_LK
		printf("[LK/LCM] GPIO_LCD_PWR_EN =   0x%x\n", GPIO_LCD_PWR_EN);
		printf("[LK/LCM] GPIO_LCM_RST =   0x%x\n", GPIO_LCD_RST);
		printf("[LK/LCM] GPIO_LCD_PWR2_EN =   0x%x\n", GPIO_LCD_PWR2_EN);
		printf("[LK/LCM] GPIO_LCD_STB_EN =   0x%x\n", GPIO_LCD_STB_EN);	
#endif
		return;
	}
#ifdef BUILD_LK
	mt_set_gpio_mode(GPIO, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO, (output > 0) ? GPIO_OUT_ONE : GPIO_OUT_ZERO);
#endif
}

static void init_lcm_registers(void)
{
    unsigned int data_array[16];

#ifdef BUILD_LK
    printf("[BND][LK/LCM] %s() enter\n", __func__);
#else
    printk("[BND][Kernel/LCM] %s() enter\n", __func__);
#endif
	// Page0
	data_array[0] = 0x00E01500;
	dsi_set_cmdq(data_array, 1, 1);
	
	// PASSWORD 
	data_array[0] = 0x93E11500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x65E21500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xF8E31500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//Lane select by internal reg  4 lanes
	data_array[0] = 0x04E01500; 
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x032D1500; //defult 0x01	
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0] = 0x00E01500; 
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x03801500; //0X03=4-LANE; 0x02=3-LANE
	dsi_set_cmdq(data_array, 1, 1);
		
	//--- Sequence Ctrl  ----//	
	data_array[0] = 0x02701500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x23711500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x06721500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//--- Page1  ----//
	data_array[0] = 0x01E01500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//Set VCOM
	data_array[0] = 0x00001500;
	dsi_set_cmdq(data_array, 1, 1);
	//vcom_data_1 = vcom_data_1 - 0x02000000;
	//data_array[0] = vcom_data_1;
	data_array[0] = 0x62011500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//Set VCOM_Reverse
	data_array[0] = 0x00031500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x6D041500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//Set Gamma Power, VGMP,VGMN,VGSP,VGSN
	data_array[0] = 0x00171500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xBF181500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00191500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x001A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xBF1B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x001C1500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//Set Gate Power
	data_array[0] = 0x3E1F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x28201500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x28211500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x0E221500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//SETPANEL
	data_array[0] = 0x09371500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//SET RGBCYC
	data_array[0] = 0x04381500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x08391500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x123A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x783C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xFF3D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xFF3E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x7F3F1500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//Set TCON
	data_array[0] = 0x06401500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0xA0411500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//added bnd zby for test
	//data_array[0] = 0x354A1500;
	//dsi_set_cmdq(data_array, 1, 1);
	
	//--- power voltage  ----//
	data_array[0] = 0x01551500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x01561500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x69571500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x0A581500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x0A591500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x295A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x155B1500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//--- Gamma  ----//
	data_array[0] = 0x7C5D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x655E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x555F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x49601500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x44611500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x35621500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x3A631500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x23641500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x3D651500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x3C661500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x3D671500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x5D681500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x4D691500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x566A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x486B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x456C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x386D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x256E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x006F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x7C701500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x65711500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x55721500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x49731500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x44741500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x35751500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x3A761500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x23771500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x3D781500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x3C791500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x3D7A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x5D7B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x4D7C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x567D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x487E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x457F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x38801500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x25811500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00821500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//Page2, for GIP  
	data_array[0] = 0x02E01500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//GIP_L Pin mapping   
	data_array[0] = 0x1E001500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E011500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x41021500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x41031500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x43041500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x43051500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F061500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F071500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F081500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F091500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E0A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E0B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F0C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x470D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x470E1500;  
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x450F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x45101500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x4B111500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x4B121500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x49131500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x49141500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F151500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//GIP_R Pin mapping
	data_array[0] = 0x1E161500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E171500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x40181500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x40191500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x421A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x421B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F1C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F1D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F1E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1f1F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E201500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E211500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1f221500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x46231500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x46241500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x44251500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x44261500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x4A271500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x4A281500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x48291500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x482A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1f2B1500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//GIP_L_GS Pin mapping
	data_array[0] = 0x1F2C1500;//1  VDS 		0x1E
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F2D1500;//2  VDS          0x1E
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x422E1500;//3  STV2         0x41
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x422F1500;//4  STV2         0x41
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x40301500;//5  STV4         0x43
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x40311500;//6  STV4         0x43
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E321500;//7  VSD          0x1F
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E331500;//8  VSD          0x1F
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F341500;//9  GCL          0x1F
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F351500;//10              0x1F
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E361500;//11 GCH          0x1E
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E371500;//12 GCH          0x1E
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F381500;//13              0x1F
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x48391500;//14 CLK8         0x47
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x483A1500;//15 CLK8         0x47
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x4A3B1500;//16 CLK6         0x45
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x4A3C1500;//17 CLK6         0x45
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x443D1500;//18 CLK4         0x4B
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x443E1500;//19 CLK4         0x4B
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x463F1500;//20 CLK2         0x49
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x46401500;//21 CLK2         0x49
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F411500;//22 VGL          0x1F
	dsi_set_cmdq(data_array, 1, 1);

	//GIP_R_GS Pin mapping
	data_array[0] = 0x1F421500;//1  VDS 		0x1E
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F431500;//2  VDS          0x1E
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x43441500;//3  STV1         0x40
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x43451500;//4  STV1         0x40
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x41461500;//5  STV3         0x42
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x41471500;//6  STV3         0x42
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E481500;//7  VSD          0x1F
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E491500;//8  VSD          0x1F
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E4A1500;//9  GCL          0x1F
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F4B1500;//10              0x1f
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E4C1500;//11 GCH          0x1E
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1E4D1500;//12 GCH          0x1E
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F4E1500;//13              0x1f
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x494F1500;//14 CLK7         0x46
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x49501500;//15 CLK7         0x46
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x4B511500;//16 CLK5         0x44
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x4B521500;//17 CLK5         0x44
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x45531500;//18 CLK3         0x4A
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x45541500;//19 CLK3         0x4A
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x47551500;//20 CLK1         0x48
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x47561500;//21 CLK1         0x48
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x1F571500;//22 VGL          0x1f
	dsi_set_cmdq(data_array, 1, 1);	
		
	//GIP Timing  
	data_array[0] = 0x10581500;
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x00591500; 
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x005A1500; 
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x305B1500; //STV_S0
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x025C1500; //STV_S0
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x405D1500; //STV_W / S1
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x015E1500; //STV_S2
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x025F1500; //STV_S3
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x30601500; //ETV_W / S1
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x01611500; //ETV_S2
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x02621500; //ETV_S3
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x6A631500; //SETV_ON  
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x6A641500; //SETV_OFF 
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x05651500; //ETV
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x12661500; 
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x74671500; 
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x04681500; 
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x6A691500; 
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x6A6A1500; 
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x086B1500; 
	dsi_set_cmdq(data_array, 1, 1);
		
	data_array[0] = 0x006C1500;
	dsi_set_cmdq(data_array, 1, 1);  
	data_array[0] = 0x066D1500;
	dsi_set_cmdq(data_array, 1, 1);  
	data_array[0] = 0x006E1500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x886F1500; 
	dsi_set_cmdq(data_array, 1, 1); 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x00701500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x00711500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x06721500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x7B731500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x00741500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x07751500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x00761500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x5D771500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x17781500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x1F791500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x007A1500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x007B1500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x007C1500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x037D1500; 
	dsi_set_cmdq(data_array, 1, 1); 
	data_array[0] = 0x7B7E1500; 
	dsi_set_cmdq(data_array, 1, 1); 		
	
	//Page4
	data_array[0] = 0x04E01500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x2B2B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x442E1500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//Page1
	data_array[0] = 0x01E01500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x010E1500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//Page3
	data_array[0] = 0x03E01500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x2F981500;
	dsi_set_cmdq(data_array, 1, 1);
	
	//Page0
	data_array[0] = 0x00E01500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x02E61500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x02E71500;
	dsi_set_cmdq(data_array, 1, 1);	
	
	data_array[0] = 0x00110500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120);
	
	data_array[0] = 0x00290500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(5);
	
	data_array[0] = 0x00351500;
	dsi_set_cmdq(data_array, 1, 1);
}

/* --------------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* --------------------------------------------------------------------------- */
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->physical_height = 216;
	params->physical_width 	= 135;
#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
#else
	params->dsi.mode = SYNC_PULSE_VDO_MODE;
#endif

	/* DSI */
	/* Command mode setting */
	params->dsi.LANE_NUM = LCM_FOUR_LANE;
	/* The following defined the fomat for data coming from LCD engine. */
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	params->dsi.intermediat_buffer_num = 2;
	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active  = 4;  
    params->dsi.vertical_backporch = 8;  
    params->dsi.vertical_frontporch  = 24;  
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 18;  
    params->dsi.horizontal_backporch = 18; 
    params->dsi.horizontal_frontporch = 18; 
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;

	/* video mode timing */
	params->dsi.word_count = FRAME_WIDTH * 3;

	params->dsi.PLL_CLOCK = 235;
}

static void lcm_init_power(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_init() enter\n");

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(10);

	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(10);

	lcm_set_gpio_output(GPIO_LCD_PWR2_EN, GPIO_OUT_ONE);
	MDELAY(10);

	lcm_set_gpio_output(GPIO_LCD_STB_EN, GPIO_OUT_ONE);
	MDELAY(10);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(30);
	
    lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(80);
    
    lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
#endif
}

static void lcm_init_lcm(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_init() enter\n");

	init_lcm_registers();
#endif
}

void lcm_suspend(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_suspend() enter\n");
#endif
}

void lcm_resume(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_resume() enter\n");
#endif
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
	unsigned char x0_LSB = (x0 & 0xFF);
	unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
	unsigned char x1_LSB = (x1 & 0xFF);
	unsigned char y0_MSB = ((y0 >> 8) & 0xFF);
	unsigned char y0_LSB = (y0 & 0xFF);
	unsigned char y1_MSB = ((y1 >> 8) & 0xFF);
	unsigned char y1_LSB = (y1 & 0xFF);

	unsigned int data_array[16];

	data_array[0] = 0x00053902;
	data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
	data_array[2] = (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00053902;
	data_array[1] = (y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2b;
	data_array[2] = (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);
}
#endif

LCM_DRIVER jd9365_pbt101wxb40i072_wxga_boe_lcm_drv = {
	.name = "jd9365_pbt101wxb40i072_wxga_boe",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init_lcm,
	.init_power = lcm_init_power,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};
