/*****************************************************************************
 *
 * Filename:
 * ---------
 *    mtk_auxadc.c
 *
 * Project:
 * --------
 *   Android_Software
 *
 * Description:
 * ------------
 *   This Module defines functions of AUXADC
 *
 * Author:
 * -------
 * Mediatek
 *
 ****************************************************************************/
//#include <common.h>
//#include <asm/io.h>

#include <platform/mtk_auxadc_sw.h>
#include <platform/mtk_auxadc_hw.h>

///////////////////////////////////////////////////////////////////////////////////////////
//// Define
static int adc_auto_set =0;

#define AUXADC_SET_BITS(BS,REG)       ((*(volatile unsigned int*)(REG)) |= (unsigned int)(BS))
#define AUXADC_CLR_BITS(BS,REG)       ((*(volatile unsigned int*)(REG)) &= ~((unsigned int)(BS)))

#define VOLTAGE_FULL_RANGE 	1500 // VA voltage
#define AUXADC_PRECISE 		4096 // 12 bits
#define MAX_CHN 		16


//step1 check con3 if auxadc is busy
//step2 clear bit
//step3  read channle and make sure old ready bit ==0
//step4 set bit  to trigger sample
//step5  read channle and make sure  ready bit ==1
//step6 read data

int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata)
{
     int idle_count = 0;
   int data_ready_count = 0;
   unsigned int channel[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  // mutex_lock(&mutex_get_cali_value);

#if CLOCK_DEFAULT_CLOSE
   /* enable clock */
   (*(volatile unsigned int *)AUXADC_CLR_CG_GATING) |= (1<<30);
#endif

   //step1 check con3 if auxadc is busy
   while ((*(volatile unsigned short *)AUXADC_CON2) & 0x01) {
       printf("[adc_api]: wait for module idle\n");
       udelay(10000);
       idle_count++;
       if(idle_count>30) {
           //wait for idle time out
           printf("[adc_api]: wait for aux/adc idle time out\n");
           return -1;
       }
   }
   // step2 clear bit
   if(0 == adc_auto_set)//clear bit
       *(volatile unsigned short *)AUXADC_CON1 = *(volatile unsigned short *)AUXADC_CON1 & (~(1 << dwChannel));


   //step3  read channle and make sure old ready bit ==0
   while ((*(volatile unsigned short *)(AUXADC_DAT0 + dwChannel * 0x04)) & (1<<12)) {
       printf("[adc_api]: wait for channel[%d] ready bit clear\n",dwChannel);
       udelay(10000);
       data_ready_count++;
       if(data_ready_count>30) {//wait for idle time out
           printf("[adc_api]: wait for channel[%d] ready bit clear time out\n",dwChannel);
           return -2;
        }
   }

   //step4 set bit  to trigger sample
   if(0==adc_auto_set)
        *(volatile unsigned short *)AUXADC_CON1 = *(volatile unsigned short *)AUXADC_CON1 | (1 << dwChannel);

   //step5  read channle and make sure  ready bit ==1
   udelay(1000);//we must dealay here for hw sample channel data
   while (0==((*(volatile unsigned short *)(AUXADC_DAT0 + dwChannel * 0x04)) & (1<<12))) {
       printf("[adc_api]: wait for channel[%d] ready bit ==1\n",dwChannel);
       udelay(10000);
       data_ready_count++;
       if(data_ready_count>30) {
            //wait for idle time out
           printf("[adc_api]: wait for channel[%d] data ready time out\n",dwChannel);
          return -3;
       }
   }
   ////step6 read data
   channel[dwChannel] = (*(volatile unsigned short *)(AUXADC_DAT0 + dwChannel * 0x04)) & 0x0FFF;
   if(rawdata)
      *rawdata = channel[dwChannel];

   //printk("[adc_api: imm mode raw data => channel[%d] = %d\n",dwChannel, channel[dwChannel]);
   //printk("[adc_api]: imm mode => channel[%d] = %d.%d\n",
  //dwChannel, (channel[dwChannel] * 250 / 4096 / 100), ((channel[dwChannel] * 250 / 4096) % 100));
   data[0] = (channel[dwChannel] * 150 / AUXADC_PRECISE / 100);//unit: 1V
   data[1] = ((channel[dwChannel] * 150 / AUXADC_PRECISE) % 100);//unit: 10mV
   data[2] = ((channel[dwChannel] * 1500 / AUXADC_PRECISE) % 1000);//unit:1mV

#if CLOCK_DEFAULT_CLOSE
   /* disable clock */
   (*(volatile unsigned int *)AUXADC_SET_CG_GATING) |= (1<<30);;
#endif

   return 0;
}

// 1v == 1000000 uv
// this function voltage Unit is uv
int IMM_GetOneChannelValue_Cali(int Channel, int*voltage)
{
     int ret = 0;
     int rawvalue = 0;
     int data[4] = {0};

     ret = IMM_GetOneChannelValue( Channel,  data, &rawvalue);
     if(ret) {
         ret = IMM_GetOneChannelValue( Channel,  data, &rawvalue);
         if(ret) {
              printf("[adc_api]:IMM_GetOneChannelValue_Cali  get raw value error %d \n",ret);
              return -1;
          }
     }

     // *voltage = rawvalue*1500000 / 4096;
     *voltage = (int)((long long)rawvalue*1500000 / (long long)4096);

      //printk("[adc_api]:IMM_GetOneChannelValue_Cali  voltage= %d uv \n",*voltage);
      return 0;
}

static int IMM_auxadc_get_evrage_data(int times, int Channel)
{
       int ret = 0, data[4], i, ret_value = 0, ret_temp = 0;

        i = times;
       while (i--) {
           ret_value = IMM_GetOneChannelValue(Channel, data, &ret_temp);
           ret += ret_temp;
           printf("[auxadc_get_data(channel%d)]: ret_temp=%d\n",Channel,ret_temp);
           //msleep(10);
       }

      ret = ret / times;
      return ret;
}

#if TEST_AUXADC
void Test_auxadc(void)
{
	int ret = 0;
	int chn = 0;
	int rawdata = 0;
	int data[4] = {0};

	for (chn = 0; chn < MAX_CHN; chn++) {
		ret = IMM_GetOneChannelValue(chn, data, &rawdata);
		if (!ret)
			printf("[Test_auxadc]chn[%d]: code(0x%x) = %d mV\n", chn, rawdata, (data[0]*1000 + data[2]));
	         else
			printf("[Test_auxadc]chn[%d] get data failed!err_ret:%d\n", chn, ret);
       }
}
#endif

