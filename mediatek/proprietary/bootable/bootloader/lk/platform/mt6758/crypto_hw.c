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

#include <stdlib.h>
#include <string.h>
#include <arch/arm/mmu.h>
#include <arch/ops.h>
#include <platform/sec_status.h>
#include <platform/crypto_hw.h>

#define SHA_SHARED_MEM_ADDR   (0x54a04000)
#define SHA_SHARED_MEM_LEN    (0x1000)

u32 g_crypto_hw_initialized = 0;
u32 g_crypto_hw_disabled = 0;
CRYPTO_HW_SHARE_MEM *g_crypto = (CRYPTO_HW_SHARE_MEM *)SHA_SHARED_MEM_ADDR;

extern unsigned int crypto_hw_init(unsigned int share_mem_addr,
				   unsigned int share_mem_len);
extern unsigned int crypto_hw_disable(void);
extern unsigned int sha256atf_init(void);
extern unsigned int sha256atf_process(const unsigned char *in,
				      unsigned long len);
extern unsigned int sha256atf_done(void);

/* crypto hw engine can only be accessed in secure world */
/* share memory address can only be passed into atf once, and associated smc calls will be disabled before leaving LK */
/* note: if multi-thread is enabled, a mutex must be held in a hash calculation session */

static unsigned int crypto_hw_engine_init()
{
	unsigned int ret = 0;

	if (g_crypto_hw_initialized == 1)
		return 0;

	arch_mmu_map(((uint64_t)SHA_SHARED_MEM_ADDR) & 0xffffffff,
		     (uint32_t)SHA_SHARED_MEM_ADDR,
		     MMU_MEMORY_TYPE_NORMAL_WRITE_BACK | MMU_MEMORY_AP_P_RW_U_NA,
		     SHA_SHARED_MEM_LEN);

	ret = crypto_hw_init(SHA_SHARED_MEM_ADDR, SHA_SHARED_MEM_LEN);
	if (ret)
		return ret;

	g_crypto_hw_initialized = 1;

	return ret;
}

unsigned int crypto_hw_engine_disable()
{
	unsigned int ret = 0;
	if (g_crypto_hw_disabled == 1)
		return 0;

	ret = crypto_hw_disable();
	if (ret)
		return ret;

	g_crypto_hw_disabled = 1;

	return ret;
}

unsigned int sha256hw_init(void)
{
	unsigned int ret = 0;

	ret = crypto_hw_engine_init();
	if (ret)
		return ret;

	ret = sha256atf_init();
	if (ret)
		return ret;

	return ret;
}

unsigned int sha256hw_process(const unsigned char *in, unsigned long len)
{
	unsigned int ret = 0;

	if (in == NULL)
		return INVALID_PARAMETER;

	arch_clean_invalidate_cache_range((addr_t)in, len);
	ret = sha256atf_process(in, len);
	if (ret)
		return ret;

	return ret;
}

unsigned int sha256hw_done(unsigned char *hash)
{
	unsigned int ret = 0;

	if (hash == NULL)
		return INVALID_PARAMETER;

	memset(hash, 0x0, SHA256_HASH_SIZE);
	/* do not call crypto_hw_engine_deinit here, since it disables ATF crypto SMC permanently */
	arch_clean_invalidate_cache_range((addr_t)SHA_SHARED_MEM_ADDR,
					  SHA_SHARED_MEM_LEN);
	ret = sha256atf_done();
	if (ret)
		return ret;

	memcpy(hash, g_crypto->hash, SHA256_HASH_SIZE);

	return ret;
}

/* padding is not handled currently, and alignment is fixed to 16 bytes */
unsigned int calc_hash_hw(unsigned char *buf, unsigned int buf_sz,
			  unsigned int padding_sz, unsigned char *hash, unsigned int hash_sz)
{
	unsigned int ret = 0;
	unsigned int process_len = 0;

	if (hash_sz < SHA256_HASH_SIZE)
		return ERR_INVALID_HASH_SZ;

	if (padding_sz >= AES_BLK_SIZE)
		return ERR_INVALID_HASH_SZ;

	if ((NULL == hash) || (NULL == buf))
		return INVALID_PARAMETER;

	memset(hash, 0x0, SHA256_HASH_SIZE);

	ret = sha256hw_init();
	if (ret) {
		dprintf(CRITICAL, "sha256hw_init fail, ret = 0x%x\n", ret);
		return ret;
	}

	process_len = (buf_sz / AES_BLK_SIZE) * AES_BLK_SIZE;
	arch_clean_invalidate_cache_range((addr_t)buf, process_len);
	ret = sha256hw_process(buf, process_len);
	if (ret) {
		dprintf(CRITICAL, "sha256hw_process fail, ret = 0x%x\n", ret);
		return ret;
	}

	arch_clean_invalidate_cache_range((addr_t)SHA_SHARED_MEM_ADDR, SHA_SHARED_MEM_LEN);
	/* note alignment != 0 does not indicate process_len == len */
	if (process_len != buf_sz) {
		memset(g_crypto->last_block, 0x0, AES_BLK_SIZE);
		memcpy(g_crypto->last_block, buf + process_len, buf_sz - process_len);
		arch_clean_invalidate_cache_range((addr_t)SHA_SHARED_MEM_ADDR,
						  SHA_SHARED_MEM_LEN);
		/* secure world should detect this buffer is in shared memory and avoid mapping again */
		ret = sha256hw_process(g_crypto->last_block, AES_BLK_SIZE);
		if (ret) {
			dprintf(CRITICAL, "sha256hw_process fail, ret = 0x%x\n", ret);
			return ret;
		}
	}

	ret = sha256hw_done(hash);
	if (ret) {
		dprintf(CRITICAL, "sha256hw_done fail, ret = 0x%x\n", ret);
		return ret;
	}

	return ret;
}

