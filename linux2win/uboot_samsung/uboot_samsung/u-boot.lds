OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
 . = 0x00000000;
 . = ALIGN(4);
 .text :
 {
   cpu/samsung/s5p6450/start.o (.text)
   cpu/samsung/s5p6450/start_irom.o (.text)
   cpu/samsung/s5p6450/cpu_init.o (.text)
   board/samsung/smdk6450/lowlevel_init.o (.text)
   cpu/samsung/s5p6450/nand_cp.o (.text)
   cpu/samsung/s5p6450/movi.o (.text)
   *(.text)
 }
 . = ALIGN(4);
 .rodata : { *(.rodata) }
 .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) }
  __exidx_start = .;
  .ARM.exidx : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
  __exidx_end = .;
 . = ALIGN(4);
 .data : { *(.data) }
 . = ALIGN(4);
 .got : { *(.got) }
 . = ALIGN(4);
 .got : { *(.got) }
 __u_boot_cmd_start = .;
 .u_boot_cmd : { *(.u_boot_cmd) }
 __u_boot_cmd_end = .;
 . = ALIGN(4);
 .mmudata : { *(.mmudata) }
 . = ALIGN(4);
 __bss_start = .;
 .bss : { *(.bss) }
 _end = .;
}
