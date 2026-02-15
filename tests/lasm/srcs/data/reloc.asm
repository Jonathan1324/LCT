; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

section .text
    global _start

_start:
    db _start, _start, _start
    dw _start, _start, _start
    dd _start, _start, _start
    dq _start, _start, _start
