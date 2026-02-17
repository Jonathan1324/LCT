; FORMATS: BIN,ELF
; BITS: 16
; EXPECT: SUCCESS

[bits 16]

section .text
    global _start

_start:
    ; PUSH - 16-bit Register
    push ax
    push bx
    push cx
    push dx
    push si
    push di
    push bp
    push sp

    ; PUSH - 16-bit Memory (32-bit addressing)
    push word [eax]
    push word [ebx + 10]
    push word [ecx + eax*2 + 100]
    push word [edx + ebx*4 - 5]
    push word [esi + edi*8 + 1000]

    ; PUSH - 16-bit Memory (16-bit addressing)
    push word [bx]
    push word [bp + 10]
    push word [bx + si + 100]

    push word [100]

    ; PUSH - 16-bit Immediate (int8_t range)
    push 0
    push 1
    push 127
    push -1
    push -128

    ; PUSH - 16-bit Immediate (outside int8_t range)
    push _start
    push 256
    push 1000
    push 10000
    push 32767
    push -129
    push -1000
    push -10000
    push -32768

    ; PUSH - 32-bit Register
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    push esp

    ; PUSH - 32-bit Memory (32-bit addressing)
    push dword [eax]
    push dword [ebx + 10]
    push dword [ecx + eax*2 + 100]
    push dword [edx + ebx*4 - 5]
    push dword [esi + edi*8 + 1000]

    ; PUSH - 32-bit Memory (16-bit addressing)
    push dword [bx]
    push dword [bp + 10]
    push dword [bx + si + 100]

    push dword [6433]

    ; PUSH - 32-bit Immediate (int8_t range)
    push 0
    push 1
    push 127
    push -1
    push -128

    ; PUSH - 32-bit Immediate (outside int8_t range)
    push 255
    push 256
    push 1000
    push 10000
    push 100000
    push 1000000
    push 2147483647
    push -129
    push -1000
    push -10000
    push -100000
    push -1000000
    push -2147483648

    ; POP - 16-bit Register
    pop ax
    pop bx
    pop cx
    pop dx
    pop si
    pop di
    pop bp
    pop sp

    ; POP - 16-bit Memory (32-bit addressing)
    pop word [eax]
    pop word [ebx + 10]
    pop word [ecx + eax*2 + 100]
    pop word [edx + ebx*4 - 5]
    pop word [esi + edi*8 + 1000]

    ; POP - 16-bit Memory (16-bit addressing)
    pop word [bx]
    pop word [bp + 10]
    pop word [bx + si + 100]

    pop word [1234]

    ; POP - 32-bit Register
    pop eax
    pop ebx
    pop ecx
    pop edx
    pop esi
    pop edi
    pop ebp
    pop esp

    ; POP - 32-bit Memory (32-bit addressing)
    pop dword [eax]
    pop dword [ebx + 10]
    pop dword [ecx + eax*2 + 100]
    pop dword [edx + ebx*4 - 5]
    pop dword [esi + edi*8 + 1000]

    ; POP - 32-bit Memory (16-bit addressing)
    pop dword [bx]
    pop dword [bp + 10]
    pop dword [bx + si + 100]

    pop dword [90]
