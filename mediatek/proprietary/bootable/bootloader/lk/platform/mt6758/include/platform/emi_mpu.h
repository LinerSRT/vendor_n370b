#ifndef __EMI_MPU_H__
#define __EMI_MPU_H__

#define NO_PROTECTION 0
#define SEC_RW 1
#define SEC_RW_NSEC_R 2
#define SEC_RW_NSEC_W 3
#define SEC_R_NSEC_R 4
#define FORBIDDEN 5
#define SEC_R_NSEC_RW 6

#define LOCK                1
#define UNLOCK              0

#define SET_ACCESS_PERMISSON(lock, d15, d14, d13, d12, d11, d10, d9, d8, d7, d6, d5, d4, d3, d2, d1, d0) \
    ((((unsigned long long) d15) << 53) | (((unsigned long long) d14) << 50) | (((unsigned long long) d13) << 47) | \
     (((unsigned long long) d12) << 44) | (((unsigned long long) d11) << 41) | (((unsigned long long) d10) << 38) | \
     (((unsigned long long) d9)  << 35) | (((unsigned long long) d8)  << 32) | (((unsigned long long) d7)  << 21) | \
     (((unsigned long long) d6)  << 18) | (((unsigned long long) d5)  << 15) | (((unsigned long long) d4)  << 12) | \
     (((unsigned long long) d3)  <<  9) | (((unsigned long long) d2)  <<  6) | (((unsigned long long) d1)  <<  3) | \
      ((unsigned long long) d0) | ((unsigned long long) lock << 26))

extern int emi_mpu_set_region_protection(unsigned int start, unsigned int end, int region, unsigned long long access_permission);

#endif /* __EMI_MPU_H__ */
