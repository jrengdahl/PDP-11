OUTPUT_FORMAT("a.out-pdp11") 

ENTRY(startup)

_memory_start = 0;
_memory_end = 06000000;

MEMORY
{
  RAM    (xrw)    : ORIGIN = 0,   LENGTH = 1536K
}

SECTIONS
{
  .text :
  {
    crt0.o(.text)
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(2);
    _etext = .;        /* define a global symbols at end of code */
  } >RAM

  .rodata :
  {
    . = ALIGN(2);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(2);
  } >RAM

  .data :
  {
    . = ALIGN(2);
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
  } >RAM

  . = ALIGN(2);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(2);
    _ebss = .;         /* define a global symbol at bss end */
  } >RAM

}
