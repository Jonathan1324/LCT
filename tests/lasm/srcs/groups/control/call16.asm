; FORMATS: BIN,ELF
; BITS: 16
; EXPECT: SUCCESS

[bits 16]

section .text
    global _start

_start:
    ; CALL - 32-bit Register
    call eax
    call ebx
    call ecx
    call edx
    call esi
    call edi
    call ebp
    call esp

    ; CALL - 16-bit Register
    call ax
    call bx
    call cx
    call dx
    call si
    call di
    call bp
    call sp

    ; CALL - 32-bit Memory (32-bit addressing)
    call word [eax]
    call dword [ebx + 10]
    call word [ecx + eax*2 + 100]
    call dword [edx + ebx*4 - 5]
    call word [esi + edi*8 + 1000]

    ; CALL - 32-bit Memory (16-bit addressing)
    call word [bx]
    call dword [bp + 10]
    call word [bx + si + 100]
    call dword [bx + di - 5]

    ; CALL - 16-bit Memory (32-bit addressing)
    call [eax]
    call [ebx + 10]
    call [ecx + eax*2 + 100]

    ; CALL - 16-bit Memory (16-bit addressing)
    call [bx]
    call [bp + 10]
    call [bx + si]

    ; CALL - Direct Labels (relative to next instruction)
    call label1
    call label2
    call label3
    call label4
    call label5
    call label6
    call label7
    call label8

label1:
    nop

label2:
    nop

label3:
    nop

label4:
    nop

label5:
    nop

label6:
    nop

label7:
    nop

label8:
    nop
