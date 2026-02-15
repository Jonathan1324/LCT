; FORMATS: BIN,ELF
; BITS: 16
; EXPECT: SUCCESS

[bits 16]

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
