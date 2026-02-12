; FORMATS: ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

section .text
    global _start

    extern externLabel

_start:
    db _start, externLabel, externLabel
    dw externLabel, externLabel, _start
    dd _start, externLabel, externLabel
    dq externLabel, externLabel, _start
