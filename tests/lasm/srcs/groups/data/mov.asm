; FORMATS: BIN,ELF
; BITS: 16,32,64
; EXPECT: SUCCESS

[bits 64]

section .text
    global _start

_start:
    ; MOV - 8-bit Register to Register
    mov al, bl
    mov al, cl
    mov al, dl
    mov al, bh
    mov ah, al
    mov ah, bl
    mov cl, dl
    mov dl, sil
    mov r8b, r9b
    mov r10b, r11b
    mov r12b, r13b
    mov r14b, r15b

    ; MOV - 8-bit Register to Immediate
    mov al, 0
    mov al, 1
    mov al, 127
    mov bl, 255
    mov cl, 100
    mov dl, 99
    mov ah, 50
    mov r8b, 200
    mov r9b, 75
    mov r10b, 150

    ; MOV - 16-bit Register to Register
    mov ax, bx
    mov ax, cx
    mov ax, dx
    mov bx, ax
    mov cx, dx
    mov dx, si
    mov si, di
    mov r8w, r9w
    mov r10w, r11w
    mov r12w, r13w
    mov r14w, r15w

    ; MOV - 16-bit Register to Immediate
    mov ax, 0
    mov ax, 1
    mov ax, 100
    mov ax, 1000
    mov ax, 5000
    mov bx, 10000
    mov cx, 32767
    mov dx, 65535
    mov r8w, 2000
    mov r9w, 4000

    ; MOV - 32-bit Register to Register
    mov eax, ebx
    mov eax, ecx
    mov eax, edx
    mov ebx, eax
    mov ecx, edx
    mov edx, esi
    mov esi, edi
    mov r8d, r9d
    mov r10d, r11d
    mov r12d, r13d
    mov r14d, r15d

    ; MOV - 32-bit Register to Immediate
    mov eax, 0
    mov eax, 1
    mov eax, 100
    mov eax, 1000
    mov eax, 10000
    mov eax, 100000
    mov ebx, 50000
    mov ecx, 2147483647
    mov edx, 4294967295
    mov r8d, 5000
    mov r9d, 20000
    mov r10d, 75000

    ; MOV - 64-bit Register to Register
    mov rax, rbx
    mov rax, rcx
    mov rax, rdx
    mov rbx, rax
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov r8, r9
    mov r10, r11
    mov r12, r13
    mov r14, r15

    ; MOV - 64-bit Register to Immediate (sign-extended)
    ;mov rax, 0
    ;mov rax, 1
    ;mov rax, 100
    ;mov rax, 1000
    ;mov rax, 10000
    ;mov rax, 100000
    ;mov rbx, 1000000
    ;mov rcx, 10000000
    ;mov rdx, 100000000
    ;mov r8, 1000000000
    mov r9, 10000000000
