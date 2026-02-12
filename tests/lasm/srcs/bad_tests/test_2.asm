; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    syscall

section .data

testV db 0, 0xff, -5
