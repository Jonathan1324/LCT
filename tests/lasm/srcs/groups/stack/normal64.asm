; FORMATS: BIN,ELF
; BITS: 64
; EXPECT: SUCCESS

[bits 64]

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
    push r8w
    push r9w
    push r10w
    push r11w
    push r12w
    push r13w
    push r14w
    push r15w

    ; PUSH - 16-bit Memory
    push word [rax]
    push word [rbx + 10]
    push word [100]
    push word [edx + ebx*4 - 5]
    push word [rsi + rdi*8 + 1000]

    ; PUSH - 16-bit Immediate (int8_t range)
    push 0
    push 1
    push 127
    push -1
    push -128

    ; PUSH - 16-bit Immediate (outside int8_t range)
    push 255
    push 256
    push 1000
    push 10000
    push 32767
    push -129
    push -1000
    push -10000
    push -32768

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

    ; PUSH - 64-bit Register
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push rsp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; PUSH - 64-bit Memory
    push qword [rax]
    push qword [rbx + 10]
    push qword [rcx + rax*2 + 100]
    push qword [rdx + rbx*4 - 5]
    push qword [esi + edi*8 + 1000]

    ; PUSH - 64-bit Immediate (int8_t range, sign-extended)
    push 0
    push 1
    push 127
    push -1
    push -128

    ; PUSH - 64-bit Immediate (outside int8_t range, but within int32_t)
    push 255
    push 256
    push 1000
    push 10000
    push 100000
    push 1000000
    push 10000000
    push 100000000
    push 1000000000
    push 2147483647
    push -129
    push -1000
    push -10000
    push -100000
    push -1000000
    push -10000000
    push -100000000
    push -1000000000
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
    pop r8w
    pop r9w
    pop r10w
    pop r11w
    pop r12w
    pop r13w
    pop r14w
    pop r15w

    ; POP - 16-bit Memory
    pop word [eax]
    pop word [rbx + 10]
    pop word [rcx + rax*2 + 100]
    pop word [rdx + rbx*4 - 5]
    pop word [rsi + rdi*8 + 1000]

    ; POP - 64-bit Register
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop rsp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    ; POP - 64-bit Memory
    pop qword [rax]
    pop qword [rbx + 10]
    pop qword [rcx + rax*2 + 100]
    pop qword [rdx + rbx*4 - 5]
    pop qword [rsi + rdi*8 + 1000]
