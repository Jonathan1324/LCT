; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    add eax, 500
    add eax, ebx
    add ebx, esi
    add edi, ebp
    add di, si
    add sp, 900
    add al, ah

    sub ah, 5
    sub bh, ch

    adc edx, ecx

    sbb eax, eax

    cmp ebx, ebx

    test eax, 200
    test ebx, edx

    and eax, ecx

    or eax, 0xffff

    xor edi, esi
