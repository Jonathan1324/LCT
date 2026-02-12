; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 32]

section .text
    global _start

F1:
    db 0
.L1:
    db 1
.L2:
    mov eax, .L1

F2:
    mov ebx, .L1
.L1:
    db 0
