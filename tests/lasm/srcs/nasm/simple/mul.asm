; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    mul eax
    div ebx
    idiv ecx

    imul edx

    imul edx, eax

    imul rsp, rbp, 100000000


    mul qword [rbp - 8]
    div byte [rax + rbp + rbp + _start]
    idiv dword [esp]

    imul word [r15]

    imul eax, [r15d + ebp * 4 - 8]

    imul rax, qword [rbp - 19], 900
