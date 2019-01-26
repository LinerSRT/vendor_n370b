# Common
TARGET=k37tv1_bsp_512
MTK_PLATFORM=MT6735
MACH_TYPE=mt6737t
MTK_SEC_CHIP_SUPPORT=yes
MTK_SEC_USBDL=ATTR_SUSBDL_ONLY_ENABLE_ON_SCHIP
MTK_SEC_BOOT=ATTR_SBOOT_ONLY_ENABLE_ON_SCHIP
MTK_SEC_MODEM_AUTH=no
MTK_SEC_SECRO_AC_SUPPORT=yes
# Platform
MTK_MT6333_SUPPORT=no
MTK_FAN5405_SUPPORT=no
MTK_EMMC_SUPPORT=yes
MTK_MT8193_SUPPORT=no
MTK_SECURITY_SW_SUPPORT=yes
MTK_FACTORY_LOCK_SUPPORT=no
CUSTOM_SEC_AUTH_SUPPORT=no
MTK_KERNEL_POWER_OFF_CHARGING=yes
MTK_COMBO_NAND_SUPPORT=no
MTK_DISABLE_POWER_ON_OFF_VOLTAGE_LIMITATION=no
MTK_BQ24160_SUPPORT=no
MTK_SEC_VIDEO_PATH_SUPPORT=no
CUSTOM_CONFIG_MAX_DRAM_SIZE=0x20000000
MTK_TEE_SUPPORT = no
TRUSTONIC_TEE_SUPPORT = no
MICROTRUST_TEE_SUPPORT = no
MTK_GOOGLE_TRUSTY_SUPPORT = no
export MTK_PLATFORM MACH_TYPE MTK_MT6333_SUPPORT MTK_FAN5405_SUPPORT MTK_EMMC_SUPPORT MTK_8193_SUPPORT MTK_SECURITY_SW_SUPPORT MTK_SEC_CHIP_SUPPORT MTK_SEC_USBDL MTK_SEC_BOOT MTK_SEC_MODEM_AUTH MTK_SEC_SECRO_AC_SUPPORT CUSTOM_SEC_AUTH_SUPPORT MTK_KERNEL_POWER_OFF_CHARGING MTK_EMMC_SUPPORT_OTP MTK_COMBO_NAND_SUPPORT MTK_DISABLE_POWER_ON_OFF_VOLTAGE_LIMITATION MTK_BQ24160_SUPPORT MTK_SEC_VIDEO_PATH_SUPPORT MTK_FACTORY_LOCK_SUPPORT CUSTOM_CONFIG_MAX_DRAM_SIZE MTK_TEE_SUPPORT TRUSTONIC_TEE_SUPPORT MICROTRUST_TEE_SUPPORT MTK_GOOGLE_TRUSTY_SUPPORT