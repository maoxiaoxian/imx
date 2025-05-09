SECTIONS {
    . = 0x87800000;
    .text :
    {
        obj/start.o
        *(.text)
    }
    .rodata ALIGN(4) : {*(.rodata*)}
    .data ALIGN(4) : {*(.data)}
    _bss_start = . ;
    .bss ALIGN(4) : {*(.bss) *(COMMON)}
    _bss_end = . ;
}
