OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(_start_a64)
MEMORY
{
 ram : ORIGIN = (0x9E800000 + (63 * 1024)), LENGTH = 1024
}
SECTIONS
{
 .stext : {
  __stext = .;
  start_a64.o (.text)
  *(.text*)
  *(.rodata*)
  __etext = .;
 } > ram
 __init_stack_N = (0x9E800000 + (62 * 1024) - 64);
}
