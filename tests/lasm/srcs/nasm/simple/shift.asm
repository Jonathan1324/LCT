; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    shl qword [rbp - 5 + rax * 4], 6
    shl qword [rax * 2], cl

    shl eax, cl
    shr ebx, cl

    sal ecx, cl
    sar edx, cl

    rol esi, cl
    ror edi, cl

    rcl esp, cl
    rcr ebp, cl

    shl eax, 2
    shr ebx, 3

    sal ecx, 4
    sar edx, 5

    rol esi, 6
    ror edi, 7

    rcl esp, 8
    rcr ebp, 0
