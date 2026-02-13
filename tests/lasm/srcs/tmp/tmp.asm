; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    div byte [rax * 2 + _start]
    div word [rbp]
    div dword [rax + 10]
    div qword [esp - 12]

    idiv byte [rax * 2 + _start]
    idiv word [rbp]
    idiv dword [eax + 10]
    idiv qword [rbp + rbp + rbp + rax + rbp + 16]

    imul byte [rax * 3 + _start]
    imul word [rcx + rsp]
    imul dword [eax + 15]
    imul qword [rbp + rbp + 16]
