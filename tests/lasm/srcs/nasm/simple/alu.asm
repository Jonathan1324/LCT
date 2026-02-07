; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    add bx, 200

    add ah, 250

    add al, 1

    dd 0
    dw 0

    add ax, 600
    
    dw 0

    test eax, ebx
    test ax, 200

    dd 0

    add eax, _start

    add ebx, _start

    add rax, rbx

    sbb rcx, rsi

    add rbx, 124134
