; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    shl eax, cl
    shr ebx, cl

    dw 0xFFFF

    sal ecx, cl
    sar edx, cl

    dw 0xFFFF

    rol esi, cl
    ror edi, cl

    dw 0xFFFF

    rcl esp, cl
    rcr ebp, cl

    dd 0

    shl eax, 2
    shr ebx, 3

    dw 0xffff

    sal ecx, 4
    sar edx, 5

    dw 0xffff

    rol esi, 6
    ror edi, 7

    dw 0xffff

    rcl esp, 8
    rcr ebp, 0
