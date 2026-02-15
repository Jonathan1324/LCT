; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

section .text
    global _start

_start:
    db 0, 1, 2
    dw 0, 1, 2
    dd 0, 1, 2
    dq 0, 1, 2

    resb 5
    resw 5
    resd 5
    resq 5
