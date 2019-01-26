LOCAL_PATH := $(call my-dir)
ifneq (,$(findstring /$(patsubst kernel-%,%,$(LINUX_KERNEL_VERSION)),$(LOCAL_PATH)))

MY_KERNEL_ROOT_DIR := $(PWD)
MY_KERNEL_CONFIG_FILE := $(MY_KERNEL_ROOT_DIR)/kernel-4.4/arch/$(TARGET_ARCH)/configs/$(KERNEL_DEFCONFIG)
MY_KERNEL_CONFIG_MODULES := $(shell grep ^CONFIG_MODULES=y $(MY_KERNEL_CONFIG_FILE))
MY_TEST := CONFIG_MODULES=y

# we should not build ko for some project without define CONFIG_MODULES
ifeq ($(MY_KERNEL_CONFIG_MODULES),$(MY_TEST))

include $(CLEAR_VARS)
LOCAL_MODULE := met.ko

ifeq (user,$(TARGET_BUILD_VARIANT))
   MET_INTERNAL_USE := $(shell test -f $(MY_KERNEL_ROOT_DIR)/vendor/mediatek/kernel_modules/met_drv_secure/4.4/init.met.rc && echo yes)
   ifeq ($(MET_INTERNAL_USE),yes)
      LOCAL_INIT_RC := ../../met_drv_secure/4.4/init.met.rc
   endif
endif

include $(MTK_KERNEL_MODULE)

endif # $(MY_KERNEL_CONFIG_MODULES) == $(MY_TEST)
endif # Kernel version matches current path