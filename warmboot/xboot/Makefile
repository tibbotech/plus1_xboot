sinclude ../.config

OUT     = ./
XBOOT_NAME = xboot

CROSS_COMPILE    ?= ../../../../crossgcc/gcc-arm-9.2-2019.12-x86_64-arm-none-eabi/bin/arm-none-eabi-

ifneq ($(CROSS_COMPILE),)
GCC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)cpp
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
endif

XBOOT_CFLAGS  += -Os -Wall -g -nostdlib -fno-builtin -I../../include -I../../ext_config
XBOOT_CFLAGS  += -march=armv8-a
XBOOT_CFLAGS  += -ffunction-sections -fdata-sections
XBOOT_CFLAGS  += -Wl,--build-id=none

# no warn when coverting (int)pointer
XBOOT_CFLAGS += -Wno-pointer-to-int-cast


XBOOT_LD_SRC   = xboot.ldi
XBOOT_LD_GEN   = xboot.ld
XBOOT_LDFLAGS  = -L $(shell dirname `$(GCC) -print-libgcc-file-name`) -lgcc
XBOOT_LDFLAGS += -Wl,--gc-sections


XBOOT_ASRC  = start.S
XBOOT_CSRC  = main.c
XBOOT_CSRC += common.c

XBOOT_OBJS = $(XBOOT_ASRC:.S=.o) $(XBOOT_CSRC:.c=.o)

COLOR_YELLOW="\033[0;1;33m"
COLOR_BLUE="\033[0;1;34m"
COLOR_GREEN="\033[0;1;32m"
COLOR_RED="\033[0;1;31m"
COLOR_ORIGIN="\033[0m"

all:
	$(MAKE) $(XBOOT_NAME)

$(XBOOT_NAME): $(XBOOT_OBJS) a64bin
	@echo ">>>>> A64-Link $@"
	@$(CPP) -P $(XBOOT_CFLAGS) $(XBOOT_LD_SRC) $(XBOOT_LD_GEN)
	$(GCC) $(XBOOT_CFLAGS) $(XBOOT_OBJS) -T $(XBOOT_LD_GEN) $(XBOOT_LDFLAGS) \
		-o $(OUT)/$(XBOOT_NAME) -Wl,-Map,$(OUT)/$(XBOOT_NAME).map
	@$(OBJCOPY) -O binary -S $(OUT)/$(XBOOT_NAME) $(OUT)/$(XBOOT_NAME).bin
	@$(OBJDUMP) -d -S $(OUT)/$(XBOOT_NAME) > $(OUT)/$(XBOOT_NAME).dis
a64bin:
	@echo "Build a64 bin"
	@$(MAKE) -C ../a64up/
	@$(OBJCOPY) -I binary -O elf32-littlearm -B arm --rename-section .data=.a64bin ../a64up/a64up.bin ../a64up/a64bin.o

%.o: %.S
	@echo "  A32-CC $@"
	@$(GCC) $(XBOOT_CFLAGS) -c -o $@ $<

%.o: %.c
	@echo "  A32-CC $@"
	@$(GCC) $(XBOOT_CFLAGS) -c -o $@ $<

.depend: $(XBOOT_ASRC) $(XBOOT_CSRC)
	@rm -f .depend >/dev/null
	@$(GCC) $(XBOOT_CFLAGS) -MM $^ >> ./.depend
sinclude .depend

.PHONY:clean
clean:
	@-rm -rf .depend $(XBOOT_LD_GEN) $(OBJS) *.o *.map *.dis \
		$(OUT)/$(XBOOT_NAME) $(OUT)/$(XBOOT_NAME).bin >/dev/null
