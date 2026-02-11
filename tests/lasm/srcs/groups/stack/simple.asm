; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    pusha
    popa

    pushad
    popad

    pushf
    popf

    pushfd
    popfd

[bits 64]
    pushf
    popf

    pushfq
    popfq

[bits 16]
    pusha
    popa

    pushad
    popad

    pushf
    popf

    pushfd
    popfd
