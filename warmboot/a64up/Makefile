sinclude ../.config

OUT     = ./
A64_NAME = a64up

A64_CROSS_COMPILE   = ../../../../crossgcc/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-

ifneq ($(A64_CROSS_COMPILE),)
GCC = $(A64_CROSS_COMPILE)gcc
CPP = $(A64_CROSS_COMPILE)cpp
OBJCOPY = $(A64_CROSS_COMPILE)objcopy
OBJDUMP = $(A64_CROSS_COMPILE)objdump
endif

A64_CFLAGS  += -Os -Wall -g -nostdlib -fno-builtin -I../../include -I../../ext_config
A64_CFLAGS  += -march=armv8-a
A64_CFLAGS  += -ffunction-sections -fdata-sections
A64_CFLAGS  += -Wl,--build-id=none

# no warn when coverting (int)pointer
A64_CFLAGS += -Wno-pointer-to-int-cast

# inform gcc that ARM unaligned access is disabled
A64_CFLAGS += -mstrict-align -mcmodel=large


A64_LD_SRC   = boot_a64.ldi
A64_LD_GEN   = boot_a64.ld
A64_LDFLAGS  = -L $(shell dirname `$(GCC) -print-libgcc-file-name $(A64_CFLAGS)`) -lgcc
#A64_LDFLAGS += -Wl,--gc-sections,--print-gc-sections
A64_LDFLAGS += -Wl,--gc-sections


all: $(A64_NAME)



A64_ASRC  = start_a64.S
A64_CSRC  = smp.c
A64_CSRC += diag_min.c

A64_OBJS = $(A64_ASRC:.S=.o) $(A64_CSRC:.c=.o)

COLOR_YELLOW="\033[0;1;33m"
COLOR_BLUE="\033[0;1;34m"
COLOR_GREEN="\033[0;1;32m"
COLOR_RED="\033[0;1;31m"
COLOR_ORIGIN="\033[0m"

all:
	$(MAKE) $(A64_NAME)

$(A64_NAME): $(A64_OBJS)
	@echo ">>>>> A64-Link $@"
	@$(CPP) -P $(A64_CFLAGS) $(A64_LD_SRC) $(A64_LD_GEN)
	$(GCC) $(A64_CFLAGS) $(A64_OBJS) -T $(A64_LD_GEN) $(A64_LDFLAGS) \
		-o $(OUT)/$(A64_NAME) -Wl,-Map,$(OUT)/$(A64_NAME).map
	@$(OBJCOPY) -O binary -S $(OUT)/$(A64_NAME) $(OUT)/$(A64_NAME).bin
	@$(OBJDUMP) -d -S $(OUT)/$(A64_NAME) > $(OUT)/$(A64_NAME).dis

%.o: %.S
	@echo "  A64-CC $@"
	@$(GCC) $(A64_CFLAGS) -c -o $@ $<

%.o: %.c
	@echo "  A64-CC $@"
	@$(GCC) $(A64_CFLAGS) -c -o $@ $<

.depend: $(A64_ASRC) $(A64_CSRC)
	@rm -f .depend >/dev/null
	@$(GCC) $(A64_CFLAGS) -MM $^ >> ./.depend
sinclude .depend

.PHONY:clean
clean:
	@-rm -rf .depend $(A64_LD_GEN) $(OBJS) *.o *.map *.dis \
		$(OUT)/$(A64_NAME) $(OUT)/$(A64_NAME).bin >/dev/null
