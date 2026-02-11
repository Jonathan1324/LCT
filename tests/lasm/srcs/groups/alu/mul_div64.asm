; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    mul dh
    mul cx
    mul eax
    mul rax
    mul byte [rax - 8]
    mul word [rbp]
    mul dword [eax + 10]
    mul qword [rsp - 12]

    div sil
    div bp
    div ebx
    div rbx
    div byte [rax * 2 + _start]
    div word [rbp]
    div dword [rax + 10]
    div qword [esp - 12]

    idiv al
    idiv di
    idiv ecx
    idiv r15
    idiv byte [rax * 2 + _start]
    idiv word [rbp]
    idiv dword [eax + 10]
    idiv qword [rbp + rbp + rbp + rax + rbp + 16]

    imul ah
    imul r10w
    imul edx
    imul r14
    imul byte [rax * 3 + _start]
    imul word [rcx + rsp]
    imul dword [eax + 15]
    imul qword [rbp + rbp + 16]

    imul di, ax
    imul eax, edi
    imul rax, rbx
    imul cx, [r15 + r14]
    imul eax, [1000]
    imul r15, [eax]

    imul di, ax, 1222
    imul eax, edi, 74567
    imul rax, rbx, 21345
    imul cx, [r15 + r14], 853
    imul eax, [1000], 1342
    imul r15, [eax], 1245
