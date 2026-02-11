; FORMATS: BIN,ELF
; BITS: 16
; EXPECT: SUCCESS

[bits 16]

section .text
    global _start

_start:
    int 0
    int 1
    int 2
    int 3
    int 4
    int 5
    int 0x80
    int 100
