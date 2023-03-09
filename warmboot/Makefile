.PHONY: all

XBOOT_CROSS ?=


OUT = warmboot

all: clean
	$(MAKE) -C xboot CROSS_COMPILE="$(XBOOT_CROSS)"
	@cp xboot/xboot ./$(OUT)


.PHONY: clean
clean:
	$(MAKE) -C xboot clean
	$(MAKE) -C a64up clean
