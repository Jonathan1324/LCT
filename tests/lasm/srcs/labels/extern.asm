; FORMATS: ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start
    extern unusedExternLabel
    extern usedExternLabel

L1:
    mov eax, usedExternLabel
