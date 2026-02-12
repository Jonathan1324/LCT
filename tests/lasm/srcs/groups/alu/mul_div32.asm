; FORMATS: BIN,ELF
; BITS: 32
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    mul dh
    mul cx
    mul eax
    mul byte [eax - 8]
    mul word [ebp]
    mul dword [eax + 10]
    mul byte [bx - 8]
    mul word [bp]
    mul dword [bx + si + 10]

    div dl
    div cx
    div ebx
    div byte [eax * 2 + _start]
    div word [ebp]
    div dword [eax + 10]
    div byte [bx + _start]
    div word [bp]
    div dword [bx + si + 10]

    idiv al
    idiv di
    idiv ecx
    idiv byte [eax * 2 + _start]
    idiv word [ebp]
    idiv dword [eax + 10]
    idiv byte [bx + _start]
    idiv word [bp]
    idiv dword [bx + si + 10]

    imul ah
    imul dx
    imul edx
    imul byte [eax * 3 + _start]
    imul word [ecx + esp]
    imul dword [eax + 15]
    imul byte [bx + _start]
    imul word [si + bp]
    imul dword [bx + si + 15]

    imul di, ax
    imul eax, edi
    imul cx, [edi + esi]
    imul eax, [1000]
    imul di, [bx + si]
    imul ax, [bp]

    imul di, ax, 128
    imul eax, edi, 74567
    imul cx, [edi + esi], -9
    imul eax, [1000], -1342
    imul di, [bx + si], 6
    imul ax, [bp], 1
