; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

section .text
    global _start

_start:
    syscall
