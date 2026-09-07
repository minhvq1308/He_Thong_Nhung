ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
    /* Vector Table */
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH


    /* Code chương trình */
    .text :
    {
        *(.text*)
        *(.rodata*)
    } > FLASH


    /* Biến đã khởi tạo */
    .data :
    {
        *(.data*)
    } > RAM


    /* Biến chưa khởi tạo */
    .bss :
    {
        *(.bss*)
        *(COMMON)
    } > RAM
}