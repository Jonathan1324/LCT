; FORMATS: BIN,ELF
; BITS: 16
; EXPECT: SUCCESS

[bits 16]

section .text
    global _start

_start:
    ; CALL - 32-bit Register
    jmp eax
    jmp ebx
    jmp ecx
    jmp edx
    jmp esi
    jmp edi
    jmp ebp
    jmp esp

    ; CALL - 16-bit Register
    jmp ax
    jmp bx
    jmp cx
    jmp dx
    jmp si
    jmp di
    jmp bp
    jmp sp

    ; CALL - 32-bit Memory (32-bit addressing)
    jmp word [eax]
    jmp dword [ebx + 10]
    jmp word [ecx + eax*2 + 100]
    jmp dword [edx + ebx*4 - 5]
    jmp word [esi + edi*8 + 1000]

    ; CALL - 32-bit Memory (16-bit addressing)
    jmp word [bx]
    jmp dword [bp + 10]
    jmp word [bx + si + 100]
    jmp dword [bx + di - 5]

    ; CALL - 16-bit Memory (32-bit addressing)
    jmp [eax]
    jmp [ebx + 10]
    jmp [ecx + eax*2 + 100]

    ; CALL - 16-bit Memory (16-bit addressing)
    jmp [bx]
    jmp [bp + 10]
    jmp [bx + si]

    ; CALL - Direct Labels (relative to next instruction)
    jmp label1
    jmp label2
    jmp label3
    jmp label4
    jmp label5
    jmp label6
    jmp label7
    jmp label8

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
