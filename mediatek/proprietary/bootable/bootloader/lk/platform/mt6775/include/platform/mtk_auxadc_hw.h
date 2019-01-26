#ifndef _MTK_ADC_HW_H
#define _MTK_ADC_HW_H

#ifdef AUXADC_BASE
#undef AUXADC_BASE
#endif

/* 1,clock default is closed;0,clock default is open */
#define  CLOCK_DEFAULT_CLOSE		0/* mt6758 default open */
#if CLOCK_DEFAULT_CLOSE
#define CLOCK_BASE 0x10000000
/* CLOCK reg */
#define AUXADC_STA_CG_GATING (CLOCK_BASE+0x94)/* bit10 for auxadc*/
#define AUXADC_SET_CG_GATING (CLOCK_BASE+0x88)
#define AUXADC_CLR_CG_GATING (CLOCK_BASE+0x8C)
#define CLOCK_SHIFT           10
#endif

/* Base addr will modify by platfrom */
#define AUXADC_BASE 0x10310000
/* AUXADC reg */
#define AUXADC_CON0                     (AUXADC_BASE + 0x000)
#define AUXADC_CON1                     (AUXADC_BASE + 0x004)
#define AUXADC_CON1_SET                 (AUXADC_BASE + 0x008)
#define AUXADC_CON1_CLR                 (AUXADC_BASE + 0x00C)
#define AUXADC_CON2                     (AUXADC_BASE + 0x010)
#define AUXADC_DAT0                     (AUXADC_BASE + 0x014)

#if 0
#define AUXADC_TP_CMD            (AUXADC_BASE + 0x005c)
#define AUXADC_TP_ADDR           (AUXADC_BASE + 0x0060)
#define AUXADC_TP_CON0           (AUXADC_BASE + 0x0064)
#define AUXADC_TP_DATA0          (AUXADC_BASE + 0x0074)

#define PAD_AUX_XP				13
#define TP_CMD_ADDR_X			0x0005

#define AUXADC_CON_RTP		(10209000 + 0x0404)

#define AUXADC_DET_VOLT                 (AUXADC_BASE + 0x084)
#define AUXADC_DET_SEL                  (AUXADC_BASE + 0x088)
#define AUXADC_DET_PERIOD               (AUXADC_BASE + 0x08C)
#define AUXADC_DET_DEBT                 (AUXADC_BASE + 0x090)
#define AUXADC_MISC                     (AUXADC_BASE + 0x094)
#define AUXADC_ECC                      (AUXADC_BASE + 0x098)
#define AUXADC_SAMPLE_LIST              (AUXADC_BASE + 0x09c)
#define AUXADC_ABIST_PERIOD             (AUXADC_BASE + 0x0A0)
#endif

#endif/*_MTK_ADC_HW_H*/

