#include <debug.h>
#include <platform/mt_typedefs.h>
#include <platform/emi_mpu.h>
#include <kernel/thread.h>

#define DBG_EMI(x...) dprintf(CRITICAL, x)

#define MTK_SIP_KERNEL_EMIMPU_SET           0x82000209

#define emi_mpu_smc_set(start, end, apc8, apc0) \
    mt_secure_call(MTK_SIP_KERNEL_EMIMPU_SET, start, end, apc8, apc0)

/*
 * emi_mpu_set_region_protection: protect a region.
 * @start: start address of the region
 * @end: end address of the region
 * @region: EMI MPU region id
 * @access_permission: EMI MPU access permission
 * Return 0 for success, otherwise negative status code.
 */
int emi_mpu_set_region_protection(unsigned int start, unsigned int end, int region, unsigned long long access_permission)
{
	int ret = 0;
	unsigned long flags;
	unsigned int apc8;
	unsigned int apc0;

	apc0 = (access_permission & 0x04FFFFFF) | (((unsigned long long)region & 0x1F) << 27);
	apc8 = (access_permission >> 32) & 0x00FFFFFF;

	enter_critical_section();
	emi_mpu_smc_set(start, end, apc8, apc0);
	exit_critical_section();

	DBG_EMI("LK set emi mpu region protection start:%x end=%x region=%d apc8=%x apc0=%x\n", start, end, region, apc8, apc0);

	return ret;
}

