; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

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
