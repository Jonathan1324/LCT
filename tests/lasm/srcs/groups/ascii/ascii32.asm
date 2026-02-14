; FORMATS: BIN,ELF
; BITS: 32
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    aaa
    aad
    aam
    aas

    aam 5
    aad 9
    aam 100
    aad -5

    aam _start
    aad _start

    aam _start - 5
    aad _start + 100
