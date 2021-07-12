sinclude .config

###########  ARCH CPU_PATH config ######################
ifeq ($(CONFIG_ARCH_ARM), y)
ARCH := arm
else ifeq ($(CONFIG_ARCH_RISCV), y)
ARCH := riscv
endif

CPU_PATH ?=
ifeq ($(CONFIG_PLATFORM_Q628),y)
CPU_PATH := arm/q628
else ifeq ($(CONFIG_PLATFORM_Q645),y)
CPU_PATH := arm/q645
else ifeq ($(CONFIG_PLATFORM_I143),y)
CPU_PATH := riscv/i143
endif

###########  Toolchain path ######################
ifneq ($(CROSS),)
CC = $(CROSS)gcc
CPP = $(CROSS)cpp
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
endif

###########  LDFLAGS CONFIG ######################
LD_GEN   = arch/$(CPU_PATH)/boot.ld
LD_SRC   = arch/$(CPU_PATH)/boot.ldi
LDFLAGS  = -L $(shell dirname `$(CC) -print-libgcc-file-name`) -lgcc
#LDFLAGS += -Wl,--gc-sections,--print-gc-sections
LDFLAGS += -Wl,--gc-sections
LDFLAGS +=  -Wl,--build-id=none

###########  CFLAGS CONFIG ######################
CFLAGS   = -Os -Wall -g -nostdlib -fno-builtin -Iinclude -Iarch/$(CPU_PATH)/include -I.
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-pie -fno-PIE -fno-pic -fno-PIC
CFLAGS += -fno-partial-inlining -fno-jump-tables
CFLAGS += -static

ifeq ($(ARCH),arm)
CFLAGS  += -mthumb -mthumb-interwork
else
CFLAGS	+= -march=rv64gc -mabi=lp64d -mcmodel=medany -msave-restore
endif

ifeq ($(CONFIG_PLATFORM_Q628),y)
CFLAGS  += -march=armv5te
endif

ifeq ($(CONFIG_PLATFORM_Q645),y)
CFLAGS  += -march=armv8-a -fno-delete-null-pointer-checks
CFLAGS  += -mno-unaligned-access
CFLAGS  += -ffunction-sections -fdata-sections
CFLAGS  += -Wno-unused-function
ifeq ($(SECURE),1)
CFLAGS  += -DCONFIG_COMPILE_WITH_SECURE=1
CFLAGS  += -DCONFIG_SECURE_BOOT_SIGN=1
else
CFLAGS  += -DCONFIG_COMPILE_WITH_SECURE=0
endif
endif

################## RISCV C+P config ##################
ifeq ($(CONFIG_I143_C_P), y)
CROSS_ARM   := ../../crossgcc/armv5-eabi--glibc--stable/bin/armv5-glibc-linux-
CFLAGS_C_P   = -Os -Wall -g -nostdlib -fno-builtin -Iinclude -Iarch/arm/q628/include -I. -ffunction-sections -fdata-sections -march=armv5te
LDFLAGS_C_P  = -L $(shell dirname `$(CROSS_ARM)gcc -print-libgcc-file-name`) -lgcc -Wl,--gc-sections,--print-gc-sections
TARGET_C_P  = xboot_C_P
START_C_P_PATH = arch/riscv/i143/arm_ca7
endif

################# xboot size config ################
ifeq ($(CONFIG_PLATFORM_IC_REV),2)
XBOOT_MAX := $$((26 * 1024))
else
XBOOT_MAX := $$((28 * 1024))
endif
ifeq ($(ARCH),riscv)
XBOOT_MAX = $$((38 * 1024))
endif

ifeq ($(CONFIG_PLATFORM_Q645),y)
XBOOT_MAX =$$((96 * 1024))
endif

#################### make #######################
.PHONY: release debug

# default target
release debug: all

BIN     := bin
TARGET  := xboot

all:  $(TARGET)
	@# 32-byte xboot header

ifeq ($(CONFIG_PLATFORM_Q645),y)
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/lpddr4_pmu_train_imem.bin $(BIN)/lpddr4_pmu_train_imem.img 0 im1d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/lpddr4_pmu_train_dmem.bin $(BIN)/lpddr4_pmu_train_dmem.img 0 dm1d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/lpddr4_2d_pmu_train_imem.bin $(BIN)/lpddr4_2d_pmu_train_imem.img 0 im2d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/lpddr4_2d_pmu_train_dmem.bin $(BIN)/lpddr4_2d_pmu_train_dmem.img 0 dm2d
endif	

ifeq ($(CONFIG_STANDALONE_DRAMINIT), y)
	@# print draminit.img size
	@sz=`du -sb $(DRAMINIT_IMG) | cut -f1` ; \
	    printf "$(DRAMINIT_IMG) size = %d (hex %x)\n" $$sz $$sz
	@echo "Append $(DRAMINIT_IMG)"
	@# xboot.img = xboot.img.orig + draminit.img
	@mv  $(BIN)/$(TARGET).img  $(BIN)/$(TARGET).img.orig
	@cat $(BIN)/$(TARGET).img.orig $(DRAMINIT_IMG) > $(BIN)/$(TARGET).img
else
	@echo "Linked with $(DRAMINIT_OBJ)"
endif

size_check:
	@# print xboot size
	@sz=`du -sb bin/$(TARGET).img | cut -f1` ; \
	 printf "xboot.img size = %d (hex %x)\n" $$sz $$sz ; \
	 if [ $$sz -gt $(XBOOT_MAX) ];then \
		echo "xboot size limit is $(XBOOT_MAX). Please reduce its size.\n" ; \
		exit 1; \
	 fi

###################
# draminit

# If CONFIG_STANDALONE_DRAMINIT=y, use draminit.img.
ifeq ($(CONFIG_STANDALONE_DRAMINIT), y)
DRAMINIT_IMG := ../draminit/bin/draminit.img
else
# Otherwise, link xboot with plf_dram.o
ifeq ($(CONFIG_PLATFORM_Q645),y)
DWC_SRC_DIR = ../draminit/dwc/software/lpddr4/src
DWC_USER_DIR = ../draminit/dwc/software/lpddr4/userCustom
DWC = dwc_ddrphy_phyinit_
DWC_USER = dwc_ddrphy_phyinit_userCustom_
DRAMINIT_OBJ := ../draminit/dwc/dwc_dram.o
DRAMINIT_OBJ += ../draminit/dwc/dwc_umctl2.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)main.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)globals.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)sequence.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)restore_sequence.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)C_initPhyConfig.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)I_loadPIEImage.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)progCsrSkipTrain.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)setUserInput.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)getUserInput.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)cmnt.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)print.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)assert.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)print_dat.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)setMb.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)softSetMb.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)getMb.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)LoadPieProdCode.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)calcMb.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)G_execFW.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)H_readMsgBlock.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)mapDrvStren.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)D_loadIMEM.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)F_loadDMEM.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)storeIncvFile.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)storeMsgBlk.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)WriteOutMem.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)IsDbyteDisabled.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)initStruct.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)regInterface.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)setStruct.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC)setDefault.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)overrideUserInput.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)A_bringupPower.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)B_startClockResetPhy.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)customPreTrain.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)customPostTrain.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)E_setDfiClk.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)G_waitFwDone.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)H_readMsgBlock.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)J_enterMissionMode.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)io_write16.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)io_read16.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)saveRetRegs.o
else 
DRAMINIT_OBJ := ../draminit/plf_dram.o
endif
# Use prebuilt obj if provided
CONFIG_PREBUILT_DRAMINIT_OBJ := $(shell echo $(CONFIG_PREBUILT_DRAMINIT_OBJ))
ifneq ($(CONFIG_PREBUILT_DRAMINIT_OBJ),)
DRAMINIT_OBJ := $(CONFIG_PREBUILT_DRAMINIT_OBJ)
endif
endif # CONFIG_STANDALONE_DRAMINIT

# build target
debug: DRAMINIT_TARGET:=debug

build_draminit:
	@echo ">>>>>>>>>>> Build draminit"
	$(MAKE) -C ../draminit $(DRAMINIT_TARGET) ARCH=$(ARCH) CROSS=$(CROSS) MKIMAGE=$(MKIMAGE)
	@echo ">>>>>>>>>>> Build draminit (done)"
	@echo ""
ifeq ($(CONFIG_PLATFORM_Q645),y)
dwc:
	@echo ">>>>>>>>>>> Build dwc obj"
	$(MAKE) -C ../draminit/dwc ARCH=$(ARCH) CROSS=$(CROSS)
	@echo ">>>>>>>>>>> Build dwc obj (done)"
	@echo ""
endif
# Boot up
ASOURCES_START := arch/$(CPU_PATH)/start.S
ifeq ($(CONFIG_SECURE_BOOT_SIGN), y)
ifeq ($(CONFIG_PLATFORM_Q628),y)
ASOURCES_V5 := arch/$(CPU_PATH)/cpu/mmu_ops.S
endif
endif
#ASOURCES_V7 := v7_start.S
ASOURCES = $(ASOURCES_V5) $(ASOURCES_V7) $(ASOURCES_START)
CSOURCES += xboot.c

CSOURCES += common/diag.c

# MON shell
ifeq ($(CONFIG_DEBUG_WITH_2ND_UART), y)
CSOURCES += romshare/regRW.c
endif
ifeq ($(MON), 1)
CFLAGS += -DMON=1
endif

# Common
CSOURCES += common/common.c common/bootmain.c common/stc.c
CSOURCES += common/string.c lib/image.c

# ARM code
CSOURCES += arch/$(CPU_PATH)/cpu/cpu.c arch/$(CPU_PATH)/cpu/interrupt.c lib/eabi_compat.c
ifeq ($(ARCH),arm)
empty :=
space += $(empty) $(empty)
arch/$(CPU_PATH)/cpu/cpu.o: CFLAGS:=$(subst -mthumb$(space),,$(CFLAGS))
endif

# Generic Boot Device
ifeq ($(CONFIG_HAVE_NAND_COMMON), y)
ifeq ($(CONFIG_PLATFORM_Q645), y)
CSOURCES += nand/nandop.c nand/bch_q645.c
else
CSOURCES += nand/nandop.c nand/bch.c
endif
endif

# Secure
CSOURCES += common/verify_image.c

# Parallel NAND
ifeq ($(CONFIG_HAVE_PARA_NAND), y)
CSOURCES += nand/nfdriver.c
endif

# SPI NAND
ifeq ($(CONFIG_HAVE_SPI_NAND), y)
ifeq ($(CONFIG_PLATFORM_Q645), y)
CSOURCES += nand/spi_nand_q645.c
else
CSOURCES += nand/spi_nand.c
endif
endif

# FAT
ifeq ($(CONFIG_HAVE_FS_FAT),y)
CSOURCES += fat/fat_boot.c
endif

# USB
ifeq ($(CONFIG_HAVE_USB_DISK), y)
CSOURCES += usb/ehci_usb.c
endif

# SNPS USB3
ifeq ($(CONFIG_HAVE_SNPS_USB3_DISK), y)
CSOURCES += usb/xhci_usb.c
endif

# MMC
ifeq ($(CONFIG_HAVE_MMC), y)
CSOURCES += sdmmc/drv_sd_mmc.c  sdmmc/hal_sd_mmc.c sdmmc/hw_sd.c
endif

# OTP
ifeq ($(CONFIG_HAVE_OTP), y)
CSOURCES += otp/sp_otp.c
CSOURCES += otp/mon_rw_otp.c
endif

CSOURCES += draminit/dram_test.c

OBJS = $(ASOURCES:.S=.o) $(CSOURCES:.c=.o)

$(OBJS): prepare

ifeq ($(CONFIG_I143_C_P), y)
$(TARGET): $(OBJS) I143_C_P
else ifeq ($(CONFIG_PLATFORM_Q645),y)
$(TARGET): $(OBJS) hsmk a64bin
else
$(TARGET): $(OBJS)
endif
	@echo ">>>>> Link $@  "
	@$(CPP) -P $(CFLAGS) -x c $(LD_SRC) -o $(LD_GEN)
	$(CC) $(CFLAGS) $(OBJS) $(DRAMINIT_OBJ) -T $(LD_GEN) $(LDFLAGS) -o $(BIN)/$(TARGET) -Wl,-Map,$(BIN)/$(TARGET).map
	@$(OBJCOPY) -O binary -S $(BIN)/$(TARGET) $(BIN)/$(TARGET).bin
	@$(OBJDUMP) -d -S $(BIN)/$(TARGET) > $(BIN)/$(TARGET).dis
ifeq ($(CONFIG_I143_C_P), y)
	@dd if=$(BIN)/$(TARGET_C_P).bin of=$(BIN)/$(TARGET).bin bs=1k seek=26 conv=notrunc 2>/dev/null
endif
ifeq ($(CONFIG_LOAD_LINUX),y)
ifeq ($(ARCH),riscv)
	@echo ">>>>>>>>>>> Build opensbi"
	@$(MAKE) -C ../opensbi distclean && $(MAKE) -C ../opensbi FW_PAYLOAD_TYPE=kernel CROSS_COMPILE=$(CROSS)
	@cd ../../ipack;./add_uhdr.sh uboot_i143_kernel ../boot/opensbi/out/fw_payload.bin ../boot/xboot/bin/OpenSBI_Kernel.img riscv 0xA0200000 0xA0200000
	@echo ">>>>>>>>>>> Build opensbi  end"
endif
else
	@if [ -f bin/OpenSBI_Kernel.img ]; then\
		echo "####delete the opensbi_kernel file #######" ;\
		rm -f bin/OpenSBI_Kernel.img ;\
	fi
endif
%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

a64bin: prepare
	@echo "Build a64 bin"
	@$(MAKE) -C arch/arm/q645/a64up/
	@$(CROSS)objcopy -I binary -O elf32-littlearm -B arm --rename-section .data=.a64bin arch/arm/q645/a64up/a64up.bin arch/arm/q645/a64up/a64bin.o

HSMK_BIN=../../build/tools/secure_hsm/secure/hsm_keys/hsmk.bin
HSMK_OBJ=../../build/tools/secure_hsm/secure/hsm_keys/hsmk.o
hsmk:
ifeq ($(SECURE),1)
	@echo "Build hsm key obj"
	@if [ ! -f $(HSMK_BIN) ];then \
		echo "Not found hsm key bin: $(HSMK_BIN)" ; \
		exit 1; \
	 fi
	@$(CROSS)objcopy -I binary -O elf32-littlearm -B arm --rename-section .data=.hsmk $(HSMK_BIN) $(HSMK_OBJ)
endif

I143_C_P: prepare
	@echo "build arm ca7 !!!"
	@$(CROSS_ARM)gcc $(CFLAGS_C_P) -c -o $(START_C_P_PATH)/start_c_p.o $(START_C_P_PATH)/start_c_p.S
	@$(CROSS_ARM)cpp -P $(CFLAGS_C_P) $(START_C_P_PATH)/boot_c_p.ldi $(START_C_P_PATH)/boot_c_p.ld
	@$(CROSS_ARM)gcc $(CFLAGS_C_P) $(START_C_P_PATH)/start_c_p.o -T $(START_C_P_PATH)/boot_c_p.ld $(LDFLAGS_C_P) -o $(BIN)/$(TARGET_C_P) -Wl,-Map,$(BIN)/$(TARGET_C_P).map
	@$(CROSS_ARM)objcopy -O binary -S $(BIN)/$(TARGET_C_P) $(BIN)/$(TARGET_C_P).bin
	@$(CROSS_ARM)objdump -d -S $(BIN)/$(TARGET_C_P) > $(BIN)/$(TARGET_C_P).dis

#################
# dependency
.depend: $(ASOURCES) $(CSOURCES)
	@rm -f .depend >/dev/null
	@$(CC) $(CFLAGS) -MM $^ >> ./.depend
sinclude .depend

#################
# clean
.PHONY:clean
clean:
ifeq ($(CONFIG_PLATFORM_Q645),y)
	@$(MAKE) -C arch/arm/q645/a64up/ clean
endif
	@$(MAKE) -C ../draminit $(DRAMINIT_TARGET) ARCH=$(ARCH) CROSS=$(CROSS) $@
	@rm -rf .depend $(LD_GEN) $(OBJS) *.o *.d>/dev/null
	@if [ -d $(BIN) ];then \
		cd $(BIN) && rm -rf $(TARGET) $(TARGET).bin $(TARGET).map $(TARGET).dis $(TARGET).img $(TARGET).img.orig $(TARGET).sig >/dev/null ;\
	 fi;
	@echo "$@: done"
	@$(MAKE) -C ../draminit/dwc $(DRAMINIT_TARGET) ARCH=$(ARCH) CROSS=$(CROSS) $@
	@rm -rf $(OBJS) *.o *.d>/dev/null
	@echo "$@: done"

distclean: clean
	@rm -rf .config .config.old $(BIN)/v7
	@rm -f GPATH GTAGS GRTAGS
	@rm -rf $(BIN)
	@echo "$@: done"

#################
# configurations
.PHONY: prepare
ifeq ($(CONFIG_PLATFORM_Q645),y)
prepare: auto_config dwc
else
prepare: auto_config build_draminit
endif
	@mkdir -p $(BIN)

AUTOCONFH=tools/auto_config/auto_config_h
MCONF=tools/mconf

config_list=$(subst configs/,,$(shell find configs/ -maxdepth 1 -mindepth 1 -type f|sort))
$(config_list):
	@if [ ! -f configs/$@ ];then \
		echo "Not found config file for $@" ; \
		exit 1 ; \
	fi
	@make clean >/dev/null
	@echo "Configure to $@ ..."
	@cp configs/$@ .config

list:
	@echo "$(config_list)" | sed 's/ /\n/g'

auto_config: chkconfig
	@echo "  [KCFG] $@.h"
	$(AUTOCONFH) .config include/$@.h

chkconfig:
	@if [ ! -f .config ];then \
		echo "Please make XXX to generate .config. Find configs by: make list" ; \
		exit 1; \
	fi

config menuconfig:
	@$(MCONF) Kconfig

#################
# misc
r: clean all
pack:
	@make -C ../../ipack
