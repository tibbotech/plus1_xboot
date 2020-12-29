sinclude .config

ifeq ($(CONFIG_ARCH_ARM), y)
ARCH := arm
else ifeq ($(CONFIG_ARCH_RISCV), y)
ARCH := riscv
endif
# Toolchain path
# Toolchain path v5
ifneq ($(CROSS),)
CC = $(CROSS)gcc
CPP = $(CROSS)cpp
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
endif

BIN     := bin
TARGET  := xboot
CFLAGS   = -Os -Wall -g -nostdlib -fno-builtin -Iinclude -Iarch/$(ARCH)/include -I.
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-pie -fno-PIE -fno-pic -fno-PIC
CFLAGS += -fno-partial-inlining -fno-jump-tables
CFLAGS += -static
LD_GEN   = arch/$(ARCH)/boot.ld
LD_SRC   = arch/$(ARCH)/boot.ldi
LDFLAGS  = -L $(shell dirname `$(CC) -print-libgcc-file-name`) -lgcc
#LDFLAGS += -Wl,--gc-sections,--print-gc-sections
LDFLAGS += -Wl,--gc-sections
LDFLAGS +=  -Wl,--build-id=none

ifeq ($(ARCH),arm)
CFLAGS  += -march=armv5te -mthumb -mthumb-interwork
else
CFLAGS	+= -march=rv64gc -mabi=lp64d -mcmodel=medany -msave-restore
endif
ifeq ($(CONFIG_I143_C_P), y)
CROSS_ARM   := ../../crossgcc/armv5-eabi--glibc--stable/bin/armv5-glibc-linux-
CFLAGS_C_P   = -Os -Wall -g -nostdlib -fno-builtin -Iinclude -Iarch/arm/include -I. -ffunction-sections -fdata-sections -march=armv5te
LDFLAGS_C_P  = -L $(shell dirname `$(CROSS_ARM)gcc -print-libgcc-file-name`) -lgcc -Wl,--gc-sections,--print-gc-sections
TARGET_C_P	 = xboot_C_P
START_C_P_PATH = arch/riscv/arm_ca7
endif
ifeq ($(CONFIG_PLATFORM_IC_REV),2)
XBOOT_MAX := $$((26 * 1024))
else
XBOOT_MAX := $$((28 * 1024))
endif
ifeq ($(ARCH),riscv)
XBOOT_MAX = $$((36 * 1024))
endif
.PHONY: release debug

# default target
release debug: all


all:  $(TARGET)
	@# 32-byte xboot header
	@bash ./add_xhdr.sh $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).img 0

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
	@$(MAKE) size_check

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
DRAMINIT_OBJ := ../draminit/plf_dram.o
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

# Boot up
ASOURCES_START := arch/$(ARCH)/start.S
ifeq ($(CONFIG_SECURE_BOOT_SIGN), y)
ifeq ($(ARCH),arm)
ASOURCES_V5 := arch/$(ARCH)/cpu/mmu_ops.S
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
CSOURCES += arch/$(ARCH)/cpu/cpu.c arch/$(ARCH)/cpu/interrupt.c lib/eabi_compat.c
ifeq ($(ARCH),arm)
space :=
space +=
arch/$(ARCH)/cpu/cpu.o: CFLAGS:=$(subst -mthumb$(space),,$(CFLAGS))
endif

# Generic Boot Device
ifeq ($(CONFIG_HAVE_NAND_COMMON), y)
CSOURCES += nand/nandop.c nand/bch.c
endif

# Parallel NAND
ifeq ($(CONFIG_HAVE_PARA_NAND), y)
CSOURCES += nand/nfdriver.c
endif

# SPI NAND
ifeq ($(CONFIG_HAVE_SPI_NAND), y)
CSOURCES += nand/spi_nand.c
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
	@rm -rf .depend $(LD_GEN) $(OBJS) *.o *.d>/dev/null
	@if [ -d $(BIN) ];then \
		cd $(BIN) && rm -rf $(TARGET) $(TARGET).bin $(TARGET).map $(TARGET).dis \
			$(TARGET).img $(TARGET).img.orig $(TARGET).sig >/dev/null ;\
	 fi;
	@echo "$@: done"

distclean: clean
	@rm -rf .config .config.old $(BIN)/v7
	@rm -f GPATH GTAGS GRTAGS
	@rm -rf $(BIN)
	@echo "$@: done"

#################
# configurations
.PHONY: prepare
prepare: auto_config build_draminit
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
