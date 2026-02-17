; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    int3
    icebp
    int1

    iret
    iretq
    iretd

    syscall
    sysret
    sysenter
    sysexit
