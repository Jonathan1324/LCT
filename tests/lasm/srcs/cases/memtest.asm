; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .data
    msg db "Memory value: 0", 10, 0

    num db 0

section .text
    global _start

_start:
    mov byte [num], 42

    mov al, [num]
    add al, '0'            ; This doesn't work yet

    mov [msg+14], al       ; This doesn't work yet

    mov rax, 1
    mov rdi, 1
    mov rsi, msg           ; This doesn't work yet
    mov rdx, 16
    syscall

    mov rax, 60
    xor rdi, rdi
    syscall
