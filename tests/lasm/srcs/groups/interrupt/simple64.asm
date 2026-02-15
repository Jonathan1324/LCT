; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    iret
    iretq
    iretd

    syscall
    sysret
    sysenter
    sysexit
