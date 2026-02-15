; FORMATS: BIN,ELF
; BITS: 32
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    clc
    stc
    cmc

    cld
    std

    cli
    sti

    lahf
    sahf
