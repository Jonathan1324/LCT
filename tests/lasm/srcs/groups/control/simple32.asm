; FORMATS: BIN,ELF
; BITS: 32
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    nop
    
    hlt
