; FORMATS: BIN,ELF
; BITS: 16
; EXPECT: SUCCESS

[bits 16]

section .text
    global _start

_start:
    int3
    into
    icebp
    int1

    iret
    iretd

    syscall
    sysret
    sysenter
    sysexit
