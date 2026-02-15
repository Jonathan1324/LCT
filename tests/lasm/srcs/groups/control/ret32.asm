; FORMATS: BIN,ELF
; BITS: 32
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    ret

    ret 100

    ret -5

    ret 10000000

    ret _start
