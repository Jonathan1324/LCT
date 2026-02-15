; FORMATS: BIN,ELF
; BITS: 32
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

_start:
    mov eax, 4
    mov ebx, 1
    mov ecx, msg
    mov edx, msg_len
    int 0x80
    
    mov eax, 1
    xor ebx, ebx
    int 0x80
    
    hlt

section .data
msg db "Hello, World!", 0x0A
msg_len equ $ - msg
