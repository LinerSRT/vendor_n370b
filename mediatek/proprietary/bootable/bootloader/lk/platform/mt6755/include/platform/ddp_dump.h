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

#ifndef _DDP_DUMP_H_
#define _DDP_DUMP_H_

#include "ddp_info.h"
#include "ddp_path.h"

typedef enum
{
    DDP_SIGNAL_OVL0__OVL0_MOUT        =1 ,
    DDP_SIGNAL_OVL0_MOUT1__WDMA0_SEL0 =0 ,
    DDP_SIGNAL_COLOR0_SEL__COLOR0     =3 ,
    DDP_SIGNAL_COLOR0__COLOR0_SOUT    =2 ,
    DDP_SIGNAL_COLOR0_SOUT1__MERGE0   =23,
    DDP_SIGNAL_COLOR0_SOUT0__AAL_SEL0 =22,
    DDP_SIGNAL_MERGE0__AAL_SEL1       =18,
    DDP_SIGNAL_AAL_SEL__AAL0          =24,
    DDP_SIGNAL_AAL0__OD               =10,
    DDP_SIGNAL_PATH0_SOUT0__UFOE_SEL0 =26,
    DDP_SIGNAL_PATH0_SOUT1__SPLIT0    =25,
    DDP_SIGNAL_SPLIT0__PATH1_SEL2     =29,
    DDP_SIGNAL_SPLIT0__UFOE_SEL1      =28,
    DDP_SIGNAL_UFOE_SEL__UFOE0        =27,
    DDP_SIGNAL_UFOE0__UFOE_MOUT       =11,

    DDP_SIGNAL_OVL1__OVL1_MOUT        =5 ,
    DDP_SIGNAL_OVL1_MOUT1__WDMA1_SEL0 =4 ,
    DDP_SIGNAL_OVL1_MOUT0__COLOR1_SEL1=7 ,
    DDP_SIGNAL_COLOR1_SOUT0__GAMMA0   =21,
    DDP_SIGNAL_COLOR1__COLOR1_SOUT    =20,
    DDP_SIGNAL_COLOR1_SOUT1__MERGE0   =19,
    DDP_SIGNAL_COLOR1_SEL__COLOR1     =6 ,
    DDP_SIGNAL_GAMMA0__GAMMA_MOUT     =31,
    DDP_SIGNAL_PATH1_SEL__PATH1_SOUT  =30,
    DDP_SIGNAL_PATH1_SOUT0__DSI0_SEL2 =9 ,
    DDP_SIGNAL_RDMA2__RDMA2_SOUT      =17,
    DDP_SIGNAL_RDMA2_SOUT1__DPI_SEL2  =16,
    DDP_SIGNAL_RDMA2_SOUT0__DSI1_SEL2 =15,
    DDP_SIGNAL_SPLIT1__DSI1_SEL0      =13,
    DDP_SIGNAL_DSI0_SEL__DSI0         =12,
    DDP_SIGNAL_DSI1_SEL__DSI1         =8 ,
    DDP_SIGNAL_DPI_SEL__DPI0          =14
} DISP_ENGINE_SIGNAL0;

typedef enum
{
    DDP_SIGNAL_OVL0_MOUT0__COLOR0_SEL1 =7 ,
    DDP_SIGNAL_RDMA0__RDMA0_SOUT       =6 ,
    DDP_SIGNAL_RDMA0_SOUT0__PATH0_SEL0 =11,
    DDP_SIGNAL_RDMA0_SOUT1__COLOR0_SEL0=8 ,
    DDP_SIGNAL_OD__OD_MOUT             =23,
    DDP_SIGNAL_OD_MOUT2__WDMA0_SEL1    =14,
    DDP_SIGNAL_OD_MOUT0__RDMA0         =10,
    DDP_SIGNAL_OD_MOUT1__PATH0_SEL1    =9 ,
    DDP_SIGNAL_PATH0_SEL__PATH0_SOUT   =12,
    DDP_SIGNAL_UFOE_MOUT2__DPI_SEL0    =3 ,
    DDP_SIGNAL_UFOE_MOUT1__SPLIT1      =2 ,
    DDP_SIGNAL_UFOE_MOUT0__DSI0_SEL0   =1 ,
    DDP_SIGNAL_UFOE_MOUT3__WDMA0_SEL2  =22,
    DDP_SIGNAL_WDMA0_SEL__WDMA0        =13,

    DDP_SIGNAL_RDMA1_SOUT0__PATH1_SEL0 =21,
    DDP_SIGNAL_RDMA1_SOUT1__COLOR1_SEL0=16,
    DDP_SIGNAL_RDMA1__RDMA1_SOUT       =15,
    DDP_SIGNAL_GAMMA_MOUT2__WDMA1_SEL1 =17,
    DDP_SIGNAL_GAMMA_MOUT1__PATH1_SEL1 =20,
    DDP_SIGNAL_GAMMA_MOUT0__RDMA1      =19,
    DDP_SIGNAL_PATH1_SOUT1__DSI1_SEL1  =5 ,
    DDP_SIGNAL_PATH1_SOUT2__DPI_SEL1   =4 ,
    DDP_SIGNAL_SPLIT1__DSI0_SEL1       =0 ,
    DDP_SIGNAL_WDMA1_SEL__WDMA1        =18
} DISP_ENGINE_SIGNAL1;

char* ddp_get_fmt_name(DISP_MODULE_ENUM module, unsigned int fmt);
int ddp_dump_analysis(DISP_MODULE_ENUM module);
int ddp_dump_reg(DISP_MODULE_ENUM module);
int ddp_check_signal(DDP_SCENARIO_ENUM scenario);

#endif
