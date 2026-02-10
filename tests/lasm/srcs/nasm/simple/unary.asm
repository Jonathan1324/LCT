; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    not word [eax + 10]
    not qword [rbp - 16]

    neg dword [rax + rsp]
    neg byte [5]

    inc dword [1000]
    inc qword [rbp + rax + rbp]

    dec byte [eax + ebx*4 + 16]
    dec word [r15d*2 + _start]

    not eax
    not ebx
    neg ax
    neg al

    inc rax
