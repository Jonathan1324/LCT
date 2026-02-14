; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    ; CALL - 64-bit Register
    call rax
    call rbx
    call rcx
    call rdx
    call rsi
    call rdi
    call rbp
    call rsp
    call r8
    call r9
    call r10
    call r11
    call r12
    call r13
    call r14
    call r15

    ; CALL - 64-bit Memory
    call [rax]
    call [rbx + 10]
    call [rcx + rax*2 + 100]
    call [rdx + rbx*4 - 5]
    call [rsi + rdi*8 + 1000]

    ; CALL - 32-bit Memory (zero-extended to 64-bit)
    call [eax]
    call [ebx + 10]
    call [ecx + eax*2 + 100]
    call [edx + ebx*4 - 5]

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
