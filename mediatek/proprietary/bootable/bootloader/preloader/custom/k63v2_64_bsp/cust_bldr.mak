###################################################################
# Include Project Feautre  (cust_bldr.h)
###################################################################
CFG_BOOT_DEV :=BOOTDEV_SDMMC
CFG_UART_LOG :=UART1
CFG_UART_META :=UART1

CFG_FUNCTION_PICACHU_SUPPORT :=1

##### TEE >PLEASE ADD CONFIGS ABOVE THIS LINE< TEE #####
ifeq ($(strip $(MTK_TEE_SUPPORT)),yes)
  CFG_TEE_SUPPORT = 1
  ifeq ($(strip $(TRUSTONIC_TEE_SUPPORT)),yes)
    CFG_TRUSTONIC_TEE_SUPPORT = 1
  else
    CFG_TRUSTONIC_TEE_SUPPORT = 0
  endif
  ifeq ($(strip $(WATCHDATA_TEE_SUPPORT)),yes)
    CFG_WATCHDATA_TEE_SUPPORT = 1
  else
    CFG_WATCHDATA_TEE_SUPPORT = 0
  endif
  ifeq ($(strip $(MICROTRUST_TEE_SUPPORT)),yes)
    CFG_MICROTRUST_TEE_SUPPORT = 1
  else
    CFG_MICROTRUST_TEE_SUPPORT = 0
  endif
  ifeq ($(strip $(MTK_GOOGLE_TRUSTY_SUPPORT)),yes)
    CFG_GOOGLE_TRUSTY_SUPPORT = 1
  else
    CFG_GOOGLE_TRUSTY_SUPPORT = 0
  endif
else
  CFG_TEE_SUPPORT = 0
  CFG_TRUSTONIC_TEE_SUPPORT = 0
  CFG_WATCHDATA_TEE_SUPPORT = 0
  CFG_MICROTRUST_TEE_SUPPORT = 0
  CFG_GOOGLE_TRUSTY_SUPPORT = 0
endif
$(warning CFG_TEE_SUPPORT=$(CFG_TEE_SUPPORT))
$(warning CFG_TRUSTONIC_TEE_SUPPORT=$(CFG_TRUSTONIC_TEE_SUPPORT))
$(warning CFG_WATCHDATA_TEE_SUPPORT=$(CFG_WATCHDATA_TEE_SUPPORT))
$(warning CFG_MICROTRUST_TEE_SUPPORT=$(CFG_MICROTRUST_TEE_SUPPORT))
$(warning CFG_GOOGLE_TRUSTY_SUPPORT=$(CFG_GOOGLE_TRUSTY_SUPPORT))
