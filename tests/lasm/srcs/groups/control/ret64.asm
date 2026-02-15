; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    ret

    ret 100

    ret -5

    ret 10000000

    ret _start
