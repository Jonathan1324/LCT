; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    jmp ld2

    nop
    nop
    nop
L2:
    hlt

section .data

ld1 resq 51

ld2 dq 8, 2
