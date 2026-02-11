; FORMATS: BIN,ELF
; BITS: 16
; EXPECT: SUCCESS

[bits 16]

section .text
    global _start

_start:
    nop
    
    hlt
