OUTPUT_FORMAT(binary)
ENTRY(_start)
OUTPUT(out/kernel.bin)
SECTIONS
{
    . = 0x8000;
    .test : ALIGN(0x1000)
    {
        *(.text)
    }
    .data : ALIGN(0x1000)
    {
        *(.data)
    }
    .rodata : ALIGN(0x1000)
    {
        *(.rodata)
    }
    .bss : ALIGN(0x1000)
    {
        *(COMMON)
        *(.bss)
        *(.heap)
    }
}